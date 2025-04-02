/// @file declarationPropertyUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/declarationPropertyUtils.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

bool _checkSystemCLib(const std::string &ldName, bool &res, const DeclarationIsPartOfStandardOptions &opts)
{
    if (ldName.find("hif_systemc_") != 0)
        return false;

    res = false;

    if (ldName == "hif_systemc_sc_core")
        res = opts.allowSystemCRTL;
    else if (ldName == "hif_systemc_sc_dt")
        res = opts.allowSystemCRTL;
    else if (ldName == "hif_systemc_tlm")
        res = opts.allowSystemCTLM;
    else if (ldName == "hif_systemc_tlm_utils")
        res = opts.allowSystemCTLM;
    else if (ldName == "hif_systemc_sca_eln")
        res = opts.allowSystemCAMS;

    else if (ldName == "hif_systemc_standard")
        res = opts.allowCpp;

    else if (ldName == "hif_systemc_SystemVueModelBuilder")
        res = opts.allowCppExtraLibs;
    else if (ldName == "hif_systemc_ddtclib")
        res = opts.allowCppExtraLibs;
    else if (ldName == "hif_systemc_hdtlib")
        res = opts.allowCppExtraLibs;
    else if (ldName == "hif_systemc_extensions")
        res = opts.allowCppExtraLibs;

    else if (ldName == "hif_systemc_cmath")
        res = opts.allowC;
    else if (ldName == "hif_systemc_cstdio")
        res = opts.allowC;
    else if (ldName == "hif_systemc_cstdlib")
        res = opts.allowC;
    else if (ldName == "hif_systemc_ctime")
        res = opts.allowC;

    else if (ldName == "hif_systemc_iostream")
        res = opts.allowCpp;
    else if (ldName == "hif_systemc_string")
        res = opts.allowCpp;
    else if (ldName == "hif_systemc_vector")
        res = opts.allowCpp;

    return true;
}

bool _checkVhdlLib(const std::string &ldName, bool &res, const DeclarationIsPartOfStandardOptions &opts)
{
    if (ldName.find("hif_vhdl_") != 0)
        return false;

    res = false;

    if (ldName == "hif_vhdl_standard")
        res = opts.allowVhdl;
    else if (ldName == "hif_vhdl_psl_standard")
        res = opts.allowVhdlPSL;
    else
        res = opts.allowVhdl;

    return true;
}

bool _checkVerilogLib(const std::string &ldName, bool &res, const DeclarationIsPartOfStandardOptions &opts)
{
    if (ldName.find("hif_verilog_") != 0)
        return false;

    res = false;

    if (ldName == "hif_verilog_standard")
        res = true;
    else if (ldName == "hif_verilog_vams_standard")
        res = opts.allowVerilogAMS;
    else if (ldName == "hif_verilog_vams_constants")
        res = opts.allowVerilogAMS;
    else if (ldName == "hif_verilog_vams_disciplines")
        res = opts.allowVerilogAMS;
    else if (ldName == "hif_verilog_vams_driver_access")
        res = opts.allowVerilogAMS;
    else
        res = true;

    return true;
}

bool _checkLibraryDef(LibraryDef *ld, bool &res, const DeclarationIsPartOfStandardOptions &opts)
{
    if (opts.areAllAllowFlagsSet())
        return false;
    std::string name = ld->getName();
    if (!opts.sem->isNativeLibrary(name))
        return false;
    std::string nameString;
    if (opts.sem == hif::semantics::HIFSemantics::getInstance()) {
        nameString = name;
    } else {
        nameString = opts.sem->makeHifName(name);
    }

    if (_checkSystemCLib(nameString, res, opts))
        return true;
    if (_checkVhdlLib(nameString, res, opts))
        return true;
    if (_checkVerilogLib(nameString, res, opts))
        return true;

    return false;
}

} // namespace
DeclarationIsPartOfStandardOptions::DeclarationIsPartOfStandardOptions()
    : dontCheckStandardViews(false)
    , allowSystemCRTL(true)
    , allowSystemCTLM(true)
    , allowSystemCAMS(true)
    , allowC(true)
    , allowCpp(true)
    , allowCppExtraLibs(true)
    , allowVhdl(true)
    , allowVhdlPSL(true)
    , allowVerilog(true)
    , allowVerilogAMS(true)
    , sem(hif::semantics::HIFSemantics::getInstance())
{
    // ntd
}

DeclarationIsPartOfStandardOptions::~DeclarationIsPartOfStandardOptions()
{
    // ntd
}

DeclarationIsPartOfStandardOptions::DeclarationIsPartOfStandardOptions(const DeclarationIsPartOfStandardOptions &other)
    : dontCheckStandardViews(other.dontCheckStandardViews)
    , allowSystemCRTL(other.allowSystemCRTL)
    , allowSystemCTLM(other.allowSystemCTLM)
    , allowSystemCAMS(other.allowSystemCAMS)
    , allowC(other.allowC)
    , allowCpp(other.allowCpp)
    , allowCppExtraLibs(other.allowCppExtraLibs)
    , allowVhdl(other.allowVhdl)
    , allowVhdlPSL(other.allowVhdlPSL)
    , allowVerilog(other.allowVerilog)
    , allowVerilogAMS(other.allowVerilogAMS)
    , sem(other.sem)
{
    // ntd
}

DeclarationIsPartOfStandardOptions &
DeclarationIsPartOfStandardOptions::operator=(DeclarationIsPartOfStandardOptions other)
{
    swap(other);
    return *this;
}

void DeclarationIsPartOfStandardOptions::swap(DeclarationIsPartOfStandardOptions &other)
{
    std::swap(dontCheckStandardViews, other.dontCheckStandardViews);
    std::swap(allowSystemCRTL, other.allowSystemCRTL);
    std::swap(allowSystemCTLM, other.allowSystemCTLM);
    std::swap(allowSystemCAMS, other.allowSystemCAMS);
    std::swap(allowC, other.allowC);
    std::swap(allowCpp, other.allowCpp);
    std::swap(allowCppExtraLibs, other.allowCppExtraLibs);
    std::swap(allowVhdl, other.allowVhdl);
    std::swap(allowVhdlPSL, other.allowVhdlPSL);
    std::swap(allowVerilog, other.allowVerilog);
    std::swap(allowVerilogAMS, other.allowVerilogAMS);
    std::swap(sem, other.sem);
}

void DeclarationIsPartOfStandardOptions::reset()
{
    dontCheckStandardViews = true;
    allowSystemCRTL        = false;
    allowSystemCTLM        = false;
    allowSystemCAMS        = false;
    allowC                 = false;
    allowCpp               = false;
    allowCppExtraLibs      = false;
    allowVhdl              = false;
    allowVhdlPSL           = false;
    allowVerilog           = false;
    allowVerilogAMS        = false;
}

bool DeclarationIsPartOfStandardOptions::areAllAllowFlagsSet() const
{
    return allowSystemCRTL || allowSystemCTLM || allowSystemCAMS || allowC || allowCpp || allowCppExtraLibs ||
           allowVhdl || allowVhdlPSL || allowVerilog || allowVerilogAMS;
}

void DeclarationIsPartOfStandardOptions::enableCppFamily(bool enable)
{
    allowC            = enable;
    allowCpp          = enable;
    allowCppExtraLibs = enable;
}

void DeclarationIsPartOfStandardOptions::enableSystemCFamily(bool enable)
{
    allowSystemCRTL = enable;
    allowSystemCTLM = enable;
}

void DeclarationIsPartOfStandardOptions::enableSystemCAMSFamily(bool enable)
{
    allowSystemCRTL = enable;
    allowSystemCTLM = enable;
    allowSystemCAMS = enable;
}

void DeclarationIsPartOfStandardOptions::enableVhdlFamily(bool enable)
{
    allowVhdl    = enable;
    allowVhdlPSL = enable;
}

void DeclarationIsPartOfStandardOptions::enableVerilogFamily(bool enable)
{
    allowVerilog    = enable;
    allowVerilogAMS = enable;
}
bool declarationIsInstance(Declaration *obj, Object *prefix)
{
    if (dynamic_cast<Library *>(prefix) != nullptr)
        return false;
    if (dynamic_cast<Instance *>(prefix) != nullptr) {
        Instance *inst = static_cast<Instance *>(prefix);
        if (dynamic_cast<Library *>(inst->getReferencedType()) != nullptr)
            return false;
    }

    if (dynamic_cast<Variable *>(obj) != nullptr) {
        Variable *v = static_cast<Variable *>(obj);
        return v->isInstance();
    } else if (dynamic_cast<Const *>(obj) != nullptr) {
        Const *c = static_cast<Const *>(obj);
        return c->isInstance();
    } else if (dynamic_cast<EnumValue *>(obj) != nullptr) {
        return false;
    }

    return true;
}

bool declarationIsPartOfStandard(Declaration *decl, const DeclarationIsPartOfStandardOptions &opts)
{
    messageAssert(decl != nullptr, "Declaration not found.", nullptr, nullptr);

    if (dynamic_cast<LibraryDef *>(decl) != nullptr && static_cast<LibraryDef *>(decl)->isStandard()) {
        return true;
    }

    if (!opts.dontCheckStandardViews) {
        if (dynamic_cast<View *>(decl) != nullptr && static_cast<View *>(decl)->isStandard()) {
            return true;
        }

        View *nearestView = hif::getNearestParent<View>(decl);
        if (nearestView != nullptr && nearestView->isStandard()) {
            return true;
        }
    }

    LibraryDef *nearestLibDef = hif::getNearestParent<LibraryDef>(decl);
    if (nearestLibDef != nullptr && nearestLibDef->isStandard()) {
        bool res = false;
        if (_checkLibraryDef(nearestLibDef, res, opts))
            return res;
        return true;
    }

    return false;
}

bool declarationIsPartOfStandard(
    Object *symb,
    hif::semantics::ILanguageSemantics *sem,
    const DeclarationIsPartOfStandardOptions &opts)
{
    hif::features::ISymbol *symbol = dynamic_cast<hif::features::ISymbol *>(symb);
    messageAssert(symbol != nullptr, "Expected a symbol", symb, sem);

    Declaration *decl = hif::semantics::getDeclaration(symb, sem);
    messageAssert(decl != nullptr, "Cannot find declaration", symb, sem);
    return declarationIsPartOfStandard(decl, opts);
}

bool signatureDependsOnActualParameters(SubProgram *decl, DependingParameters &dependingParams)
{
    Function *foo = dynamic_cast<Function *>(decl);
    if (foo == nullptr)
        return false;

    BList<Parameter> &paramList = foo->parameters;
    Type *resultType            = foo->getType();
    hif::HifUntypedQuery q;
    for (BList<Parameter>::iterator i = paramList.begin(); i != paramList.end(); ++i) {
        Parameter *param = *i;
        q.name           = param->getName();
        std::list<Object *> result;
        hif::search(result, resultType, q);
        if (!result.empty()) {
            dependingParams.insert(param);
        }
    }
    return !dependingParams.empty();
}

} // namespace hif
