/// @file standardizeDescription.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <iostream>
#include <set>

#include "hif/application_utils/application_utils.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"
#include "hif/semantics/standardization.hpp"
#include "hif/semantics/standardizeDescription.hpp"

/////////////////////////////////////////
// Debug macros
/////////////////////////////////////////
//#define HIF_PRINT_DEBUG_FILES

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

void clearCastMap(CastMap &castMap)
{
    for (CastMap::iterator i = castMap.begin(); i != castMap.end(); ++i) {
        delete i->second;
    }

    castMap.clear();
}

void printInfo(const std::string &message, int &stepNr)
{
    ++stepNr;
    std::stringstream ssName;
    ssName << "   - STD ";
    if (stepNr < 10)
        ssName << "0";
    ssName << stepNr << ": " << message;

    messageInfo(ssName.str());
}

} // namespace

namespace
{

/// @brief Whether @p value is an integer without needing semantic types: an
/// integer literal, a reference to a generic already retyped as integer, or
/// arithmetic over those.
auto isIntegralNoType(
    hif::Value *value,
    const std::set<hif::Declaration *> &integerGenerics,
    hif::semantics::ILanguageSemantics *sem) -> bool
{
    if (dynamic_cast<hif::IntValue *>(value) != nullptr) {
        return true;
    }
    if (auto *id = dynamic_cast<hif::Identifier *>(value)) {
        return integerGenerics.count(hif::semantics::getDeclaration(id, sem)) != 0;
    }
    if (auto *expression = dynamic_cast<hif::Expression *>(value)) {
        return hif::operatorIsArithmetic(expression->getOperator()) && expression->getValue2() != nullptr &&
               isIntegralNoType(expression->getValue1(), integerGenerics, sem) &&
               isIntegralNoType(expression->getValue2(), integerGenerics, sem);
    }
    return false;
}

} // namespace

void standardizeDescription(
    System *&root,
    ILanguageSemantics *sourceSem,
    ILanguageSemantics *destSem,
    hif::application_utils::StepFileManager *parentFileManager)
{
    // Keep count of current step number.
    int stepNr = 0;

    hif::application_utils::StepFileManager _stepFileManager;
    _stepFileManager.setParentManager(parentFileManager);
#ifdef HIF_PRINT_DEBUG_FILES
    _stepFileManager.setPrint(true);
#endif

    hif::manipulation::SimplifyOptions sopt;
    hif::semantics::UpdateDeclarationOptions dopt;
    dopt.forceRefresh = true;
    dopt.error        = true;
    CastMap castMap;

    if (root == nullptr)
        return;

    printInfo("Simplifying source tree", stepNr);
    // Reset types and update declarations:
    // this is necessary for set declaration of
    // objects for avoid problem of getDeclaration of objects not
    // related to the tree and that have not member declaration set.
    hif::manipulation::flushInstanceCache();
    hif::semantics::flushTypeCacheEntries();
    resetTypes(root);
    updateDeclarations(root, sourceSem, dopt);
    typeTree(root, sourceSem, true);

    // simplify source tree
    hif::manipulation::simplify(root, sourceSem, sopt);
    _stepFileManager.printStep(root, "post_first_simplify");

    // ///////////////////////////////////////////////////////////////////
    // Prefix source tree
    // ///////////////////////////////////////////////////////////////////
    printInfo("Prefixing source tree symbols", stepNr);
    hif::manipulation::prefixTree(root, sourceSem);
    hif::manipulation::flushInstanceCache();
    hif::semantics::flushTypeCacheEntries();
    resetTypes(root);
    updateDeclarations(root, sourceSem, dopt);
    typeTree(root, sourceSem, true);
    _stepFileManager.printStep(root, "post_prefix_tree");

    // ///////////////////////////////////////////////////////////////////
    // Integer generics
    // ///////////////////////////////////////////////////////////////////
    // A Verilog parameter with no declared type is a 32-bit signed integer, but
    // verilog2hif types it as a signed bit vector. In VHDL that is a generic of
    // type signed(31 downto 0), and every use of it as a bound or a count then
    // needs vector arithmetic against bit-string literals that VHDL cannot type:
    // `to_integer(W - "000...1")` (hif-backend#93). Type it as what it is, before
    // the expressions that read it are standardized, so they are cast against the
    // right type. VHDL never writes an integer generic as a signed Bitvector, so
    // one that is did not come from VHDL.
    if (destSem == VHDLSemantics::getInstance() && sourceSem != VHDLSemantics::getInstance()) {
        bool changed = false;
        hif::HifTypedQuery<ValueTP> query;
        std::list<ValueTP *> parameters;
        hif::search(parameters, root, query);
        for (auto *parameter : parameters) {
            auto *vector = dynamic_cast<Bitvector *>(parameter->getType());
            if (vector == nullptr || !vector->isSigned() || !vector->isConstexpr() || vector->getSpan() == nullptr ||
                hif::semantics::spanGetBitwidth(vector->getSpan(), sourceSem) != 32) {
                continue;
            }
            auto *value = dynamic_cast<BitvectorValue *>(parameter->getValue());
            if (parameter->getValue() != nullptr && (value == nullptr || !value->is01())) {
                continue;
            }

            auto *integerType = new Int();
            integerType->setSpan(hif::copy(vector->getSpan()));
            integerType->setSigned(true);
            integerType->setConstexpr(true);
            if (value != nullptr) {
                auto *integerValue = new IntValue(value->getValueAsSigned());
                integerValue->setType(hif::copy(integerType));
                delete parameter->setValue(integerValue);
            }
            delete parameter->setType(integerType);
            changed = true;
        }
        if (changed) {
            // Declarations first, so an Identifier can be traced to its parameter.
            hif::manipulation::flushInstanceCache();
            hif::semantics::flushTypeCacheEntries();
            resetTypes(root);
            updateDeclarations(root, sourceSem, dopt);

            // The constants the parameter is combined with are 32-bit signed
            // vectors too, and an integer cannot be combined with those in HIF
            // semantics. In `W - 1`, `1` is an integer. Walk the arithmetic until
            // nothing more is integral.
            std::set<Declaration *> integerGenerics;
            for (auto *parameter : parameters) {
                if (dynamic_cast<Int *>(parameter->getType()) != nullptr) {
                    integerGenerics.insert(parameter);
                }
            }
            auto toInteger = [&](Value *constant) -> Value * {
                auto *vectorValue = dynamic_cast<BitvectorValue *>(constant);
                auto *vectorType  = vectorValue != nullptr ? dynamic_cast<Bitvector *>(vectorValue->getType()) : nullptr;
                if (vectorValue == nullptr || vectorType == nullptr || !vectorType->isSigned() ||
                    !vectorValue->is01() || vectorType->getSpan() == nullptr ||
                    hif::semantics::spanGetBitwidth(vectorType->getSpan(), sourceSem) != 32) {
                    return nullptr;
                }
                auto *integerType = new Int();
                integerType->setSpan(hif::copy(vectorType->getSpan()));
                integerType->setSigned(true);
                integerType->setConstexpr(true);
                auto *integerValue = new IntValue(vectorValue->getValueAsSigned());
                integerValue->setType(integerType);
                return integerValue;
            };
            for (bool again = true; again;) {
                again = false;
                hif::HifTypedQuery<Expression> expressionQuery;
                std::list<Expression *> expressions;
                hif::search(expressions, root, expressionQuery);
                for (auto it = expressions.rbegin(); it != expressions.rend(); ++it) {
                    Expression *expression = *it;
                    if (!hif::operatorIsArithmetic(expression->getOperator()) || expression->getValue2() == nullptr) {
                        continue;
                    }
                    Value *first  = expression->getValue1();
                    Value *second = expression->getValue2();
                    Value *replacement = nullptr;
                    if (isIntegralNoType(first, integerGenerics, sourceSem) && (replacement = toInteger(second)) != nullptr) {
                        delete expression->setValue2(replacement);
                        again = true;
                    } else if (
                        isIntegralNoType(second, integerGenerics, sourceSem) &&
                        (replacement = toInteger(first)) != nullptr) {
                        delete expression->setValue1(replacement);
                        again = true;
                    }
                }
            }
            hif::manipulation::flushInstanceCache();
            hif::semantics::flushTypeCacheEntries();
            resetTypes(root);
            updateDeclarations(root, sourceSem, dopt);
            typeTree(root, sourceSem, true);
            _stepFileManager.printStep(root, "post_integer_generics");
        }
    }

    // ///////////////////////////////////////////////////////////////////
    // Standardization
    // ///////////////////////////////////////////////////////////////////
    printInfo("Perform tree standardization", stepNr);
    System *dstRoot = standardizeHif(root, sourceSem, destSem, castMap);
    delete root;
    root = dstRoot;
    _stepFileManager.printStep(root, "post_standardizeHif");

    // ///////////////////////////////////////////////////////////////////
    // Mapping standard symbols.
    // ///////////////////////////////////////////////////////////////////
    printInfo("Mapping destination tree standard symbols", stepNr);
    mapStandardSymbols(root, sourceSem, destSem);
    _stepFileManager.printStep(root, "post_standard_symbols_mapping");

    // simplify destination tree
    printInfo("Simplifying destination tree -- 1", stepNr);
    sopt.behavior = hif::manipulation::SimplifyOptions::BEHAVIOR_CONSERVATIVE;
    hif::manipulation::simplify(root, destSem, sopt);
    sopt.behavior = hif::manipulation::SimplifyOptions::BEHAVIOR_NORMAL;
    _stepFileManager.printStep(root, "post_second_simplify");

    // ///////////////////////////////////////////////////////////////////
    // Manage of casts
    // ///////////////////////////////////////////////////////////////////
    printInfo("Managing destination tree casts", stepNr);
    manageCasts(root, sourceSem, destSem, castMap);
    clearCastMap(castMap);
    _stepFileManager.printStep(root, "post_manageCast");

    // simplify destination tree
    printInfo("Simplifying destination tree -- 2", stepNr);
    hif::manipulation::simplify(root, destSem, sopt);
    _stepFileManager.printStep(root, "post_third_simplify");

    // avoid forbidden names in destination tree
    printInfo("Renaming destination tree forbidden names", stepNr);
    hif::manipulation::renameForbiddenNames(root, destSem);
    _stepFileManager.printStep(root, "post_renameForbiddenNames");

    // Try to minimize memory usage.
    printInfo("Final cleanup", stepNr);
    hif::manipulation::flushInstanceCache();
    hif::semantics::flushTypeCacheEntries();
    hif::semantics::resetDeclarations(root);
    hif::semantics::resetTypes(root);
}

} // namespace semantics
} // namespace hif
