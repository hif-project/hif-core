/// @file SemanticAnalysis.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/SemanticAnalysis.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hifIOUtils.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

SemanticAnalysis::~SemanticAnalysis()
{
    // ntd
}

void SemanticAnalysis::analyzeOperands(Type *op1Type, Type *op2Type)
{
    if (op1Type == nullptr)
        return;
    bool resultIsConstexpr = false;
    bool canReplace1 = op1Type->getParent() != nullptr;
    bool canReplace2 = op2Type != nullptr && op2Type->getParent() != nullptr;
    Type *op1Prefixed      = getPrefixedType(op1Type, _sem, _prefixOpt, _srcObj);
    Type *op2Prefixed      = getPrefixedType(op2Type, _sem, _prefixOpt, _srcObj);
    if (canReplace1)
        op1Type->replace(op1Prefixed);
    if (canReplace2)
        op2Type->replace(op2Prefixed);

    if (op2Prefixed == nullptr) {
        // Handle unary operator
        _callMap(op1Prefixed);

        // If referring to a template type, result of getBaseType() is still TypeReference
        // Reference design: checker_generator, run() method.
        Type *bt1          = getBaseType(op1Prefixed, false, _sem);
        TypeReference *tr1 = dynamic_cast<TypeReference *>(bt1);
        if (tr1 != nullptr)
            resultIsConstexpr = true;
        else
            resultIsConstexpr = (typeIsConstexpr(op1Prefixed, _sem));
    } else {
        // analyze the two operands
        _callMap(op1Prefixed, op2Prefixed);

        // If referring to a template type, result of getBaseType() is still TypeReference
        // Reference design: checker_generator, run() method.
        Type *bt1          = getBaseType(op1Prefixed, false, _sem);
        Type *bt2          = getBaseType(op2Prefixed, false, _sem);
        TypeReference *tr1 = dynamic_cast<TypeReference *>(bt1);
        TypeReference *tr2 = dynamic_cast<TypeReference *>(bt2);
        if (tr1 != nullptr && tr2 != nullptr) {
            resultIsConstexpr = true;
        } else if (tr1 != nullptr) {
            resultIsConstexpr = typeIsConstexpr(op2Prefixed, _sem);
        } else if (tr2 != nullptr) {
            resultIsConstexpr = typeIsConstexpr(op1Prefixed, _sem);
        } else {
            // if together operands are constexpr, results are always constexpr.
            resultIsConstexpr = (typeIsConstexpr(op1Prefixed, _sem) && typeIsConstexpr(op2Prefixed, _sem));
        }
    }

    typeSetConstexpr(_result.returnedType, resultIsConstexpr);
    typeSetConstexpr(_result.operationPrecision, resultIsConstexpr);

    if (canReplace1)
        op1Prefixed->replace(op1Type);
    if (canReplace2 && op2Prefixed != nullptr)
        op2Prefixed->replace(op2Type);
    delete op1Prefixed;
    delete op2Prefixed;
}

ILanguageSemantics::ExpressionTypeInfo SemanticAnalysis::getResult() { return _result; }

SemanticAnalysis::SemanticAnalysis(ILanguageSemantics *sem, Operator currOperator, Object *srcObj)
    : _sem(sem)
    , _currOperator(currOperator)
    , _srcObj(srcObj)
    , _result()
    , _factory(sem)
    , _prefixOpt()
{
    _prefixOpt.strictChecks = false;
    _prefixOpt.recursive    = true;
}

void SemanticAnalysis::_restoreTypeReference(TypeReference *tr, Type *base)
{
    if (_result.returnedType == nullptr || _result.operationPrecision == nullptr)
        return;

    if (_currOperator == op_concat) {
        // type is the typeref with range as calculated by recursive call.
        TypeReference *ret = hif::copy(tr);
        Type *retbase      = hif::copy(base);
        TypeDef *td        = dynamic_cast<TypeDef *>(getDeclaration(tr, _sem));
        if (td == nullptr || td->templateParameters.empty()) // Typed range case
        {
            delete ret;
            delete retbase;
            // cannot restore since not templatic
            return;
        }

        if (td->templateParameters.size() == 2 && dynamic_cast<ValueTP *>(td->templateParameters.front()) != nullptr &&
            dynamic_cast<ValueTP *>(td->templateParameters.back()) != nullptr) // typed range
        {
            ValueTP *vtp1 = static_cast<ValueTP *>(td->templateParameters.front());
            ValueTP *vtp2 = static_cast<ValueTP *>(td->templateParameters.back());

            Range *retRange = hif::typeGetSpan(_result.returnedType, _sem);
            hif::typeSetSpan(retbase, hif::copy(retRange), _sem, true);

            ValueTPAssign *vtpa1 = new ValueTPAssign();
            vtpa1->setName(vtp1->getName());
            vtpa1->setValue(hif::copy(retRange->getLeftBound()));

            ValueTPAssign *vtpa2 = new ValueTPAssign();
            vtpa2->setName(vtp2->getName());
            vtpa2->setValue(hif::copy(retRange->getRightBound()));

            ret->templateParameterAssigns.clear();
            ret->templateParameterAssigns.push_back(vtpa1);
            ret->templateParameterAssigns.push_back(vtpa2);

            if (hif::equals(_result.returnedType, retbase)) {
                delete _result.returnedType;
                _result.returnedType = hif::copy(ret);
            }
        } else if (!td->templateParameters.empty()) {
            ret->templateParameterAssigns.clear();

            for (BList<Declaration>::iterator i = td->templateParameters.begin(); i != td->templateParameters.end();
                 ++i) {
                ValueTP *vtp = dynamic_cast<ValueTP *>(*i);
                messageAssert(vtp != nullptr, "Expected ValueTP.", *i, _sem);

                Object *obj =
                    hif::manipulation::getImplicitTemplate(vtp, td->getType(), _result.returnedType, _sem, false);
                messageAssert(obj != nullptr, "Cannot get implicit template.", td, _sem);
                Value *v = dynamic_cast<Value *>(obj);
                messageAssert(v != nullptr, "Wrong match.", obj, _sem);

                ValueTPAssign *vtpa = new ValueTPAssign();
                vtpa->setName(vtp->getName());
                vtpa->setValue(hif::manipulation::assureSyntacticType(hif::copy(v), _sem));
                ret->templateParameterAssigns.push_back(vtpa);
            }

            // Sanity check:
            Range *retRange = hif::typeGetSpan(_result.returnedType, _sem);
            hif::typeSetSpan(retbase, hif::copy(retRange), _sem, true);
            if (hif::equals(_result.returnedType, retbase)) {
                delete _result.returnedType;
                _result.returnedType = hif::copy(ret);
            }
        }

        delete retbase;
        delete ret;
    }

    hif::EqualsOptions eopt;
    eopt.checkFieldsInitialvalue = false;

    Type *cbase = hif::copy(base);
    Type *crt   = hif::copy(_result.returnedType);
    Type *prec  = hif::copy(_result.operationPrecision);
    hif::manipulation::PrefixTreeOptions prefOpt;
    prefOpt.strictChecks = false;
    hif::manipulation::prefixTree(cbase, _sem, prefOpt);
    hif::manipulation::prefixTree(crt, _sem, prefOpt);
    hif::manipulation::prefixTree(prec, _sem, prefOpt);
    bool retEquals  = hif::equals(crt, cbase, eopt);
    bool precEquals = hif::equals(prec, cbase, eopt);
    delete cbase;
    delete crt;
    delete prec;

    // returned type
    if (retEquals) {
        delete _result.returnedType;
        _result.returnedType = hif::copy(tr);
    }

    // operation precision
    if (precEquals) {
        delete _result.operationPrecision;
        _result.operationPrecision = hif::copy(tr);
    }
}

void SemanticAnalysis::_map(TypeReference *op)
{
    Type *base = getBaseType(op, false, _sem);

    if (dynamic_cast<TypeReference *>(base) != nullptr) {
        // type TP
        return;
    }

    analyzeOperands(base, nullptr);
    _restoreTypeReference(op, base);
}

void SemanticAnalysis::_map(TypeReference *op1, Type *op2)
{
    Type *base1 = getBaseType(op1, false, _sem);
    if (base1 == nullptr)
        return;

    if (dynamic_cast<TypeReference *>(base1) != nullptr) {
        // type TP
        base1 = op2;
        analyzeOperands(base1, op2);
        _restoreTypeReference(op1, base1);
        return;
    }

    Type *base1Prefixed = getPrefixedType(base1, _sem, _prefixOpt, op1);
    analyzeOperands(base1Prefixed, op2);
    _restoreTypeReference(op1, base1Prefixed);
    delete base1Prefixed;
}

void SemanticAnalysis::_map(Type *op1, TypeReference *op2)
{
    Type *base2 = getBaseType(op2, false, _sem);
    if (base2 == nullptr)
        return;

    if (dynamic_cast<TypeReference *>(base2) != nullptr) {
        // type TP
        base2 = op1;
        analyzeOperands(op1, base2);
        _restoreTypeReference(op2, base2);
        return;
    }

    Type *base2Prefixed = getPrefixedType(base2, _sem, _prefixOpt, op2);
    analyzeOperands(op1, base2Prefixed);
    _restoreTypeReference(op2, base2Prefixed);
    delete base2Prefixed;
}

void SemanticAnalysis::_map(TypeReference *op1, TypeReference *op2, bool checkOpacity)
{
    Type *baseOpaque1 = getBaseType(op1, true, _sem);
    Type *baseOpaque2 = getBaseType(op2, true, _sem);

    if (baseOpaque1 == nullptr || baseOpaque2 == nullptr)
        return;

    TypeReference *tr1opaque = dynamic_cast<TypeReference *>(baseOpaque1);
    TypeReference *tr2opaque = dynamic_cast<TypeReference *>(baseOpaque2);

    if (checkOpacity) {
        if (tr1opaque == nullptr && tr2opaque == nullptr) {
            // non opaque typerefs.
            Type *baseOpaque1Prefixed = getPrefixedType(baseOpaque1, _sem, _prefixOpt, op1);
            Type *baseOpaque2Prefixed = getPrefixedType(baseOpaque2, _sem, _prefixOpt, op2);

            analyzeOperands(baseOpaque1Prefixed, baseOpaque2Prefixed);

            _restoreTypeReference(op1, baseOpaque1Prefixed);
            _restoreTypeReference(op2, baseOpaque2Prefixed);

            delete baseOpaque1Prefixed;
            delete baseOpaque2Prefixed;
            return;
        }

        // At least one opaque or a type TP

        if (tr1opaque != nullptr && tr2opaque != nullptr) {
            // Opaque type references must be the same, without checking span dimension
            // (i.e. template parameters that could be instantiated)
            if (tr1opaque->getName() != tr2opaque->getName())
                return;
        }
    }

    Type *base1 = getBaseType(baseOpaque1, false, _sem);
    Type *base2 = getBaseType(baseOpaque2, false, _sem);
    if (base1 == nullptr || base2 == nullptr)
        return;

    TypeReference *tr1base = dynamic_cast<TypeReference *>(base1);
    TypeReference *tr2base = dynamic_cast<TypeReference *>(base2);
    if (tr1base != nullptr && tr2base != nullptr) {
        if (tr1base->getName() != tr2base->getName())
            return;

        // both type TP.. allowed only assignment!
        if (hif::operatorIsAssignment(_currOperator)) {
            _result.operationPrecision = hif::copy(tr1base);
            _result.returnedType       = new Bool();
            return;
        } else {
            messageError("Unexpected operator on type TPs", _srcObj, _sem);
        }
    } else if (tr1base != nullptr || tr2base != nullptr) {
        // only one is a type TP
        // not allowed any operation
        return;
    }

    Type *base1Prefixed = getPrefixedType(base1, _sem, _prefixOpt, op1);
    Type *base2Prefixed = getPrefixedType(base2, _sem, _prefixOpt, op2);

    // Are opaque type refs non Type TPs
    analyzeOperands(base1Prefixed, base2Prefixed);

    _restoreTypeReference(op1, base1Prefixed);
    _restoreTypeReference(op2, base2Prefixed);

    delete base1Prefixed;
    delete base2Prefixed;
}

} // namespace semantics
} // namespace hif
