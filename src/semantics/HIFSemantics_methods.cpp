/// @file HIFSemantics_methods.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cfloat>
#include <cmath>
#include <iostream>

#include "hif/HifFactory.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /* anon */
{

Expression *_makeUnaryExpression(Object *o)
{
    FunctionCall *fc = dynamic_cast<FunctionCall *>(o);
    messageAssert(fc != nullptr, "Expected function call object.", o, nullptr);
    messageAssert(fc->parameterAssigns.size() == 1, "Expected function call with 1 parameter.", fc, nullptr);
    Expression *ret = new Expression();
    ret->setValue1(fc->parameterAssigns.front()->setValue(nullptr));
    return ret;
}

Expression *_makeBinaryExpression(Object *o)
{
    FunctionCall *fc = dynamic_cast<FunctionCall *>(o);
    messageAssert(fc != nullptr, "Expected function call object.", o, nullptr);
    messageAssert(fc->parameterAssigns.size() == 2, "Expected function call with 2 parameters.", fc, nullptr);
    Expression *ret = new Expression();
    ret->setValue1(fc->parameterAssigns.front()->setValue(nullptr));
    ret->setValue2(fc->parameterAssigns.back()->setValue(nullptr));
    return ret;
}

Cast *_makeCast(Object *o, Type *t)
{
    FunctionCall *fc = dynamic_cast<FunctionCall *>(o);
    messageAssert(fc != nullptr, "Expected function call object.", o, nullptr);
    messageAssert(
        (fc->parameterAssigns.size() == 1 || fc->parameterAssigns.size() == 2),
        "Expected function call with 2 parameters.", fc, nullptr);
    messageAssert(t != nullptr, "Unable to type function call.", o, nullptr);
    Cast *ret = new Cast();
    ret->setValue(fc->parameterAssigns.front()->setValue(nullptr));
    ret->setType(t);
    if (fc->parameterAssigns.size() == 2) {
        hif::HifFactory f(HIFSemantics::getInstance());
        Expression *e = new Expression(op_minus, fc->parameterAssigns.back()->setValue(nullptr), f.intval(1));
        Range *r      = new Range(e, new IntValue(0), dir_downto);
        hif::typeSetSpan(t, r, HIFSemantics::getInstance(), true);
    }
    return ret;
}

Range *_getSpanFromSpanOperatingFunctions(FunctionCall *f, ILanguageSemantics *sem)
{
    Type *t = getSemanticType(f->getInstance(), sem);
    messageAssert((t != nullptr), "Cannot type instance", f, sem);

    Range *typeR = nullptr;
    Int *ii      = dynamic_cast<Int *>(t);
    Real *rr     = dynamic_cast<Real *>(t);
    if (ii != nullptr || rr != nullptr) {
        // Get range constraint from declaration
        Identifier *id = dynamic_cast<Identifier *>(hif::getTerminalPrefix(f->getInstance()));
        messageAssert(id != nullptr, "Unsupported or unexpected instance", f, sem);

        Identifier::DeclarationType *d = hif::semantics::getDeclaration(id, sem);
        messageAssert(d != nullptr, "Cannot find instance declaration", f, sem);

        typeR = d->getRange();
        if (typeR == nullptr) {
            if (ii != nullptr)
                typeR = new Range(-2147483647LL, 2147483647LL);
            else /* rr != nullptr */
                typeR = new Range(new RealValue(DBL_MIN), new RealValue(DBL_MAX), dir_upto);

            d->setRange(typeR);
        }
    } else {
        // array, bitvector ..
        typeR = hif::typeGetSpan(t, sem);
    }

    messageAssert((typeR != nullptr), "Cannot get range constraint.", f, sem);
    return typeR;
}

template <typename T>
T *_fixCallIntegerParameters(T *call, const std::vector<int> &intParamIndexes, ILanguageSemantics *sem)
{
    hif::HifFactory f(sem);
    T *callCopy = hif::copy(call);
    callCopy->setName("hif_verilog_" + callCopy->getName());

    for (std::vector<int>::const_iterator i = intParamIndexes.begin(); i != intParamIndexes.end(); ++i) {
        if (*i >= static_cast<int>(call->parameterAssigns.size()))
            continue;
        ParameterAssign *p = callCopy->parameterAssigns.at(static_cast<unsigned int>(*i));
        if (p == nullptr)
            continue;
        Cast *c = f.cast(f.integer(), (p->setValue(nullptr)));
        p->setValue(c);
    }

    Instance *inst = dynamic_cast<Instance *>(callCopy->getInstance());
    messageAssert(inst != nullptr, "Expected instance.", call, sem);
    Library *lib = dynamic_cast<Library *>(inst->getReferencedType());
    messageAssert(lib != nullptr, "Expected library instance.", call, sem);

    hif::semantics::ResetDeclarationsOptions ropt;
    ropt.onlySignatures = true;
    hif::semantics::resetDeclarations(callCopy, ropt);

    return callCopy;
}

} // namespace

LibraryDef *HIFSemantics::_getVHDLStandardLibrary(const std::string &name)
{
    if (name == "hif_vhdl_ieee_math_complex") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getIeeeMathComplexPackage(true);
        return lStandard;
    }
    if (name == "hif_vhdl_ieee_math_real") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getIeeeMathRealPackage(true);
        return lStandard;
    }
    if (name == "hif_vhdl_ieee_numeric_bit") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getIeeeNumericBitPackage(true);
        return lStandard;
    }
    if (name == "hif_vhdl_ieee_numeric_std") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getIeeeNumericStdPackage(true);
        return lStandard;
    }
    if (name == "hif_vhdl_ieee_std_logic_1164") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getIeeeStdLogic1164Package(true);
        return lStandard;
    }
    if (name == "hif_vhdl_ieee_std_logic_arith") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getIeeeStdLogicArithPackage(true);
        return lStandard;
    }
    if (name == "hif_vhdl_ieee_std_logic_arith_ex") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getIeeeStdLogicArithExPackage(true);
        return lStandard;
    }
    if (name == "hif_vhdl_ieee_std_logic_misc") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getIeeeStdLogicMiscPackage(true);
        return lStandard;
    }
    if (name == "hif_vhdl_ieee_std_logic_signed") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getIeeeStdLogicSignedPackage(true);
        return lStandard;
    }
    if (name == "hif_vhdl_ieee_std_logic_textio") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getIeeeStdLogicTextIOPackage(true);
        return lStandard;
    }
    if (name == "hif_vhdl_ieee_std_logic_unsigned") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getIeeeStdLogicUnsignedPackage(true);
        return lStandard;
    }
    if (name == "hif_vhdl_standard") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getStandardPackage(true);
        return lStandard;
    }
    if (name == "hif_vhdl_std_textio") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getTextIOPackage(true);
        return lStandard;
    }
    if (name == "hif_vhdl_psl_standard") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VHDLSemantics::getInstance()->getPSLStandardPackage(true);
        return lStandard;
    }
    return nullptr;
}

LibraryDef *HIFSemantics::_getVerilogStandardLibrary(const std::string &name)
{
    if (name == "hif_verilog_standard") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VerilogSemantics::getInstance()->getStandardPackage(true);
        return lStandard;
    }
    if (name == "hif_verilog_vams_standard") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VerilogSemantics::getInstance()->getVAMSStandardPackage(true);
        return lStandard;
    }
    if (name == "hif_verilog_vams_constants") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VerilogSemantics::getInstance()->getVAMSConstantsPackage(true);
        return lStandard;
    }
    if (name == "hif_verilog_vams_disciplines") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VerilogSemantics::getInstance()->getVAMSDisciplinesPackage(true);
        return lStandard;
    }
    if (name == "hif_verilog_vams_driver_access") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = VerilogSemantics::getInstance()->getVAMSDriverAccessPackage(true);
        return lStandard;
    }
    return nullptr;
}

LibraryDef *HIFSemantics::_getSystemCStandardLibrary(const std::string &name)
{
    if (name == "hif_systemc_sc_core") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getScCorePackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_sc_dt") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getScDtPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_hif_systemc_extensions") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getSystemcExtensionsPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_standard") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getStandardPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_hdtlib") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getHdtlibPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_ddtclib") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getDdtClibPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_cmath") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getCMathPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_cstdlib") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getCStdLibPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_cstdio") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getCStdIOPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_ctime") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getCTimePackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_sca_eln") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getScAmsELNPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_iostream") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getIOStreamPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_vector") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getVectorPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_string") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getStringPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_cstring") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getCStringPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_new") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getNewPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_cstddef") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getCStdDefPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_tlm") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getTlmPackage(true);
        return lStandard;
    }
    if (name == "hif_systemc_SystemVueModelBuilder") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getSystemVueModelBuilder(true);
        return lStandard;
    }
    if (name == "hif_systemc_tlm_utils") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = SystemCSemantics::getInstance()->getTlmUtils(true);
        return lStandard;
    }

    return nullptr;
}

LibraryDef *HIFSemantics::_getHIFStandardLibrary(const std::string &name)
{
    if (name == "hif_standard") {
        static LibraryDef *lStandard = nullptr;
        if (lStandard == nullptr)
            lStandard = getStandardPackage();
        return lStandard;
    }

    return nullptr;
}

LibraryDef *HIFSemantics::getStandardLibrary(const std::string &name)
{
    // HIF supports every standard library of any language as is.
    // !!! NOTE: ONLY HIF Semantics must ALWAYS return an implementation (for manipulators, at least) !!!

    LibraryDef *ret = nullptr;

    // VHDL
    ret = _getVHDLStandardLibrary(name);
    if (ret != nullptr)
        return ret;
    // Verilog
    ret = _getVerilogStandardLibrary(name);
    if (ret != nullptr)
        return ret;
    // SystemC
    ret = _getSystemCStandardLibrary(name);
    if (ret != nullptr)
        return ret;
    // HIF
    ret = _getHIFStandardLibrary(name);
    if (ret != nullptr)
        return ret;

    // !!! NOTE: ONLY HIF Semantics must ALWAYS return an implementation (for manipulators, at least) !!!
    //messageError(std::string("Unexpected standard library requested: ") + name, nullptr, nullptr);
    return nullptr;
}

bool HIFSemantics::isNativeLibrary(const std::string &name, bool /*hifFormat*/)
{
    if (SystemCSemantics::getInstance()->isNativeLibrary(name, true))
        return true;
    if (VHDLSemantics::getInstance()->isNativeLibrary(name, true))
        return true;
    if (VerilogSemantics::getInstance()->isNativeLibrary(name, true))
        return true;

    return false;
}

HIFSemantics::MapCases
HIFSemantics::mapStandardSymbol(Declaration *decl, KeySymbol &key, ValueSymbol &value, ILanguageSemantics *srcSem)
{
    bool askingLib = (key.first == key.second);

    auto lib = std::string("hif_") + srcSem->getName() + "_" + key.first;
    auto sym = std::string("hif_") + srcSem->getName() + "_" + key.second;

    value.libraries.clear();
    value.libraries.push_back(lib);
    value.mappedSymbol = sym;
    // Libraries are always replaced, therefore internal symbols must be kept
    if (askingLib) {
        value.mapAction = MAP_DELETE;
    } else if (key.first == "ieee_std_logic_1164") {
        if ((key.second == "to_x01") || (key.second == "to_ux01")) {
            Function *f = dynamic_cast<Function *>(decl);
            messageAssert(f != nullptr, "Expected function", decl, this);

            if (hif::typeIsLogic(f->parameters.front()->getType(), this)) {
                value.mapAction = MAP_KEEP; // see related methods
            } else
                value.mapAction = SIMPLIFIED; // simplified as cast
            return value.mapAction;
        }
    } else if (key.first == "ieee_std_logic_arith") {
        if ((key.second == "conv_integer") || (key.second == "conv_signed") || (key.second == "conv_unsigned") ||
            (key.second == "conv_std_logic_vector")) {
            Function *f = dynamic_cast<Function *>(decl);
            messageAssert(f != nullptr, "Expected function", decl, this);

            if (hif::typeIsLogic(f->parameters.front()->getType(), this) &&
                dynamic_cast<Bit *>(f->parameters.front()->getType()) == nullptr) {
                value.mapAction = MAP_KEEP; // see related methods
            } else
                value.mapAction = SIMPLIFIED; // simplified as cast
            return value.mapAction;
        }
    }

    if (!askingLib) {
        StandardSymbols::iterator it(_standardSymbols.find(key));
        if (it == _standardSymbols.end()) {
            value.mapAction = MAP_KEEP;
        } else {
            value = it->second;
        }
    }
    return value.mapAction;
}

Object *HIFSemantics::getSimplifiedSymbol(KeySymbol &key, Object *s)
{
    StandardSymbols::iterator it(_standardSymbols.find(key));
    if (it == _standardSymbols.end()) {
        messageError("Unexpected mapping", s, this);
    }

    Object *ret = nullptr;

    if (key.first == "ieee_math_complex") {
        ret = _getSimplifiedSymbolIeeeMathComplex(key, s);
    } else if (key.first == "ieee_math_real") {
        ret = _getSimplifiedSymbolIeeeMathReal(key, s);
    } else if (key.first == "ieee_numeric_bit") {
        ret = _getSimplifiedSymbolIeeeNumericBit(key, s);
    } else if (key.first == "ieee_numeric_std") {
        ret = _getSimplifiedSymbolIeeeNumericStd(key, s);
    } else if (key.first == "ieee_std_logic_1164") {
        ret = _getSimplifiedSymbolIeeeStdLogic1164(key, s);
    } else if (key.first == "ieee_std_logic_arith") {
        ret = _getSimplifiedSymbolIeeeStdLogicArith(key, s);
    } else if (key.first == "ieee_std_logic_arith_ex") {
        ret = _getSimplifiedSymbolIeeeStdLogicArithEx(key, s);
    } else if (key.first == "ieee_std_logic_misc") {
        ret = _getSimplifiedSymbolIeeeStdLogicMisc(key, s);
    } else if (key.first == "ieee_std_logic_signed") {
        ret = _getSimplifiedSymbolIeeeStdLogicSigned(key, s);
    } else if (key.first == "ieee_std_logic_textio") {
        ret = _getSimplifiedSymbolIeeeStdLogicTextIO(key, s);
    } else if (key.first == "ieee_std_logic_unsigned") {
        ret = _getSimplifiedSymbolIeeeStdLogicUnsigned(key, s);
    } else if (key.first == "standard") {
        ret = _getSimplifiedSymbolStandard(key, s);
    } else if (key.first == "vams_standard") {
        ret = _getSimplifiedSymbolVamsStandard(key, s);
    } else if (key.first == "std_textio") {
        ret = _getSimplifiedSymbolStdTextIO(key, s);
    }

    messageAssert(ret != nullptr, "Unable to map value.", s, this);
    return ret;
}

std::string HIFSemantics::getEventMethodName(bool /*hifFormat*/)
{
    messageError("unexpected call to getEventMethodName in HIF semantics", nullptr, nullptr);
}

bool HIFSemantics::isEventCall(FunctionCall *call)
{
    return call->getInstance() != nullptr &&
           (call->getName() == VHDLSemantics::getInstance()->getEventMethodName(true) ||
            call->getName() == VerilogSemantics::getInstance()->getEventMethodName(true) ||
            call->getName() == SystemCSemantics::getInstance()->getEventMethodName(true));
}

Object *HIFSemantics::_getSimplifiedSymbolIeeeMathComplex(KeySymbol & /*key*/, Object * /*s*/) { return nullptr; }

Object *HIFSemantics::_getSimplifiedSymbolIeeeMathReal(KeySymbol & /*key*/, Object * /*s*/) { return nullptr; }

Object *HIFSemantics::_getSimplifiedSymbolIeeeNumericBit(KeySymbol & /*key*/, Object * /*s*/) { return nullptr; }

Object *HIFSemantics::_getSimplifiedSymbolIeeeNumericStd(KeySymbol &key, Object *s)
{
    if (key.second == "shift_left") {
        Expression *e = _makeBinaryExpression(s);
        e->setOperator(op_sla);
        return e;
    }
    if (key.second == "shift_right") {
        Expression *e = _makeBinaryExpression(s);
        e->setOperator(op_sra);
        return e;
    }
    if (key.second == "rotate_left") {
        Expression *e = _makeBinaryExpression(s);
        e->setOperator(op_rol);
        return e;
    }
    if (key.second == "rotate_right") {
        Expression *e = _makeBinaryExpression(s);
        e->setOperator(op_ror);
        return e;
    }
    if (key.second == "resize") {
        return _makeCast(s, hif::copy(getSemanticType(dynamic_cast<FunctionCall *>(s), this)));
    }
    if (key.second == "to_signed") {
        Type *t = getSemanticType(dynamic_cast<FunctionCall *>(s), this);
        messageAssert(t != nullptr, "Unable to type.", s, this);
        Range *r = hif::copy(typeGetSpan(t, this));
        messageAssert(r != nullptr, "Unable to get type span.", t, this);
        return _makeCast(s, _factory.signedType(r));
    }
    if (key.second == "to_unsigned") {
        Type *t = getSemanticType(dynamic_cast<FunctionCall *>(s), this);
        messageAssert(t != nullptr, "Unable to type.", s, this);
        Range *r = hif::copy(typeGetSpan(t, this));
        messageAssert(r != nullptr, "Unable to get type span.", t, this);
        return _makeCast(s, _factory.unsignedType(r));
    }
    return nullptr;
}

Object *HIFSemantics::_getSimplifiedSymbolIeeeStdLogic1164(KeySymbol &key, Object *s)
{
    FunctionCall *call = dynamic_cast<FunctionCall *>(s);
    messageAssert(call != nullptr, "Expected FunctionCall.", s, this);

    if (key.second == "to_stdulogic") {
        return _makeCast(s, _factory.bit(true, false));
    }
    if (key.second == "to_stdlogicvector") {
        Type *t = getSemanticType(call->parameterAssigns.front()->getValue(), this);
        messageAssert(t != nullptr, "Unable to type.", s, this);
        Range *r = hif::copy(typeGetSpan(t, this));
        messageAssert(r != nullptr, "Unable to get type span.", t, this);
        return _makeCast(s, _factory.bitvector(r, true, true));
    }
    if (key.second == "to_stdulogicvector") {
        Type *t = getSemanticType(call->parameterAssigns.front()->getValue(), this);
        messageAssert(t != nullptr, "Unable to type.", s, this);
        Range *r = hif::copy(typeGetSpan(t, this));
        messageAssert(r != nullptr, "Unable to get type span.", t, this);
        return _makeCast(s, _factory.bitvector(r, true, false));
    }
    if (key.second == "to_x01") {
        Type *t = getSemanticType(call->parameterAssigns.front()->getValue(), this);
        messageAssert(t != nullptr, "Unable to type.", s, this);

        if (dynamic_cast<Bit *>(t) != nullptr) {
            return _makeCast(s, _factory.typeRef("x01"));
        } else if (dynamic_cast<Bitvector *>(t) != nullptr) {
            Range *r = hif::copy(typeGetSpan(t, this));
            messageAssert(r != nullptr, "Unable to get type span.", t, this);
            return _makeCast(s, _factory.bitvector(r, true, static_cast<Bitvector *>(t)->isResolved()));
        } else
            messageError("Unexpected type", t, this);
    } else if (key.second == "to_ux01") {
        Type *t = getSemanticType(call->parameterAssigns.front()->getValue(), this);
        messageAssert(t != nullptr, "Unable to type.", s, this);

        if (dynamic_cast<Bit *>(t) != nullptr) {
            return _makeCast(s, _factory.typeRef("ux01"));
        } else if (dynamic_cast<Bitvector *>(t) != nullptr) {
            Range *r = hif::copy(typeGetSpan(t, this));
            messageAssert(r != nullptr, "Unable to get type span.", t, this);
            return _makeCast(s, _factory.bitvector(r, true, static_cast<Bitvector *>(t)->isResolved()));
        } else
            messageError("Unexpected type", t, this);
    }
    return nullptr;
}

Object *HIFSemantics::_getSimplifiedSymbolIeeeStdLogicArith(KeySymbol &key, Object *s)
{
    if (key.second == "conv_integer") {
        FunctionCall *fc = dynamic_cast<FunctionCall *>(s);
        Type *t          = getSemanticType(fc->parameterAssigns.front()->getValue(), this);
        messageAssert(t != nullptr, "Unable to type.", s, this);
        Type *bt = getBaseType(t, false, this);
        messageAssert(bt != nullptr, "Unable to get base type.", t, this);
        if (dynamic_cast<Int *>(bt) != nullptr) {
            return _makeCast(s, _factory.integer());
        }

        if (dynamic_cast<Bit *>(bt) != nullptr && static_cast<Bit *>(bt)->isLogic()) {
            return _factory.cast(
                _factory.integer(), _factory.expression(
                                        hif::copy(fc->parameterAssigns.front()->getValue()), op_case_eq,
                                        _factory.bitval(bit_one, hif::copy(static_cast<Bit *>(bt)))));
        }

        messageError("Unexpected case.", bt, this);
    } else if (key.second == "conv_signed") {
        return _makeCast(s, _factory.signedType());
    } else if (key.second == "conv_unsigned") {
        return _makeCast(s, _factory.unsignedType());
    } else if (key.second == "conv_std_logic_vector") {
        return _makeCast(s, _factory.bitvector(nullptr, true, true));
    }
    return nullptr;
}

Object *HIFSemantics::_getSimplifiedSymbolIeeeStdLogicArithEx(KeySymbol & /*key*/, Object * /*s*/) { return nullptr; }

Object *HIFSemantics::_getSimplifiedSymbolIeeeStdLogicMisc(KeySymbol &key, Object *s)
{
    if (key.second == "and_reduce") {
        Expression *e = _makeUnaryExpression(s);
        e->setOperator(op_andrd);
        return e;
    }
    if (key.second == "nand_reduce") {
        Expression *e = _makeUnaryExpression(s);
        e->setOperator(op_andrd);
        return _factory.expression(op_not, e);
    }
    if (key.second == "or_reduce") {
        Expression *e = _makeUnaryExpression(s);
        e->setOperator(op_orrd);
        return e;
    }
    if (key.second == "nor_reduce") {
        Expression *e = _makeUnaryExpression(s);
        e->setOperator(op_orrd);
        return _factory.expression(op_not, e);
    }
    if (key.second == "xor_reduce") {
        Expression *e = _makeUnaryExpression(s);
        e->setOperator(op_xorrd);
        return e;
    }
    if (key.second == "xnor_reduce") {
        Expression *e = _makeUnaryExpression(s);
        e->setOperator(op_xorrd);
        return _factory.expression(op_not, e);
    }
    return nullptr;
}

Object *HIFSemantics::_getSimplifiedSymbolIeeeStdLogicSigned(KeySymbol &key, Object *s)
{
    if (key.second == "conv_integer") {
        return _makeCast(s, _factory.integer());
    }
    return nullptr;
}

Object *HIFSemantics::_getSimplifiedSymbolIeeeStdLogicTextIO(KeySymbol & /*key*/, Object * /*s*/) { return nullptr; }

Object *HIFSemantics::_getSimplifiedSymbolIeeeStdLogicUnsigned(KeySymbol &key, Object *s)
{
    if (key.second == "conv_integer") {
        return _makeCast(s, _factory.integer(nullptr, false));
    }
    return nullptr;
}

Object *HIFSemantics::_getSimplifiedSymbolStandard(KeySymbol &key, Object *s)
{
    // The name "standard" is referred to both VHDL and Verilog default libs...
    // Managing both here!

    if (key.second == "base") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "left") {
        return _getSimplifiedSymbol_left(s);
    } else if (key.second == "right") {
        return _getSimplifiedSymbol_right(s);
    } else if (key.second == "high") {
        return _getSimplifiedSymbol_high(s);
    } else if (key.second == "low") {
        return _getSimplifiedSymbol_low(s);
    } else if (key.second == "ascending") {
        return _getSimplifiedSymbol_ascending(s);
    } else if (key.second == "image") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "value") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "pos") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "val") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "succ") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "pred") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "leftof") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "rightof") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "range") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "reverse_range") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "length") {
        return _getSimplifiedSymbol_length(s);
    } else if (key.second == "delayed") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "stable") {
        return _getSimplifiedSymbol_stable(s);
    } else if (key.second == "quiet") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "transaction") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "event") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "active") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "last_event") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "last_active") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "last_value") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "driving") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "driving_value") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "simple_name") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "instance_name") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "path_name") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "foreign") {
        messageError("Unsupported mapping", s, this);
    } else if (key.second == "iterated_concat") {
        return _getSimplifiedSymbol_iteratedConcat(s);
    } else if (key.second == "_system_fclose") {
        std::vector<int> v;
        v.push_back(0);
        return _getSimplifiedSymbol_withVerilogIntegers(s, false, v);
    } else if (key.second == "_system_fflush") {
        std::vector<int> v;
        v.push_back(0);
        return _getSimplifiedSymbol_withVerilogIntegers(s, false, v);
    } else if (key.second == "_system_fopen") {
        std::vector<int> v; // only ret type!
        return _getSimplifiedSymbol_withVerilogIntegers(s, true, v);
    } else if (key.second == "_system_random") {
        std::vector<int> v;
        v.push_back(0);
        return _getSimplifiedSymbol_withVerilogIntegers(s, true, v);
    } else if (key.second == "_system_clog2") {
        std::vector<int> v;
        v.push_back(0);
        return _getSimplifiedSymbol_withVerilogIntegers(s, true, v);
    } else if (key.second == "_system_readmemb") {
        std::vector<int> v;
        v.push_back(2);
        v.push_back(3);
        v.push_back(4);
        v.push_back(5);
        return _getSimplifiedSymbol_withVerilogIntegers(s, false, v);
    } else if (key.second == "_system_readmemh") {
        std::vector<int> v;
        v.push_back(2);
        v.push_back(3);
        v.push_back(4);
        v.push_back(5);
        return _getSimplifiedSymbol_withVerilogIntegers(s, false, v);
    } else if (key.second == "_system_feof") {
        std::vector<int> v;
        v.push_back(0);
        return _getSimplifiedSymbol_withVerilogIntegers(s, true, v);
    } else if (key.second == "_system_fscanf") {
        std::vector<int> v;
        v.push_back(0);
        return _getSimplifiedSymbol_withVerilogIntegers(s, false, v);
    }

    assert(false);
    return nullptr;
}

Object *HIFSemantics::_getSimplifiedSymbolVamsStandard(KeySymbol &key, Object *s)
{
    if (key.second == "cross") {
        std::vector<int> v;
        v.push_back(1);
        return _getSimplifiedSymbol_withVerilogIntegers(s, false, v);
    } else if (key.second == "_system_driver_count") {
        std::vector<int> v;
        return _getSimplifiedSymbol_withVerilogIntegers(s, true, v);
    } else if (key.second == "_system_driver_state") {
        std::vector<int> v;
        v.push_back(1);
        return _getSimplifiedSymbol_withVerilogIntegers(s, false, v);
    } else if (key.second == "_system_driver_strength") {
        std::vector<int> v;
        v.push_back(1);
        return _getSimplifiedSymbol_withVerilogIntegers(s, false, v);
    } else if (key.second == "_system_driver_next_state") {
        std::vector<int> v;
        v.push_back(1);
        return _getSimplifiedSymbol_withVerilogIntegers(s, false, v);
    } else if (key.second == "_system_driver_next_strength") {
        std::vector<int> v;
        v.push_back(1);
        return _getSimplifiedSymbol_withVerilogIntegers(s, true, v);
    } else if (key.second == "_system_driver_type") {
        std::vector<int> v;
        v.push_back(1);
        return _getSimplifiedSymbol_withVerilogIntegers(s, true, v);
    }

    assert(false);
    return nullptr;
}

Object *HIFSemantics::_getSimplifiedSymbolStdTextIO(KeySymbol & /*key*/, Object * /*s*/) { return nullptr; }
// ///////////////////////////////////////////////////////////////////
// Simplified symbols, functions
// ///////////////////////////////////////////////////////////////////

Object *HIFSemantics::_getSimplifiedSymbol_left(Object *s)
{
    FunctionCall *f = dynamic_cast<FunctionCall *>(s);
    messageAssert((f != nullptr), "Expected FunctionCall", s, this);

    Range *typeR = _getSpanFromSpanOperatingFunctions(f, this);
    return hif::copy(typeR->getLeftBound());
}

Object *HIFSemantics::_getSimplifiedSymbol_right(Object *s)
{
    FunctionCall *f = dynamic_cast<FunctionCall *>(s);
    messageAssert((f != nullptr), "Expected FunctionCall", s, this);

    Range *typeR = _getSpanFromSpanOperatingFunctions(f, this);
    return hif::copy(typeR->getRightBound());
}

Object *HIFSemantics::_getSimplifiedSymbol_high(Object *s)
{
    FunctionCall *f = dynamic_cast<FunctionCall *>(s);
    messageAssert((f != nullptr), "Expected FunctionCall", s, this);

    Range *typeR = _getSpanFromSpanOperatingFunctions(f, this);
    messageAssert(
        typeR->getDirection() == dir_upto || typeR->getDirection() == dir_downto, "Unsupported range direction", f,
        this);

    return typeR->getDirection() == dir_upto ? hif::copy(typeR->getRightBound()) : hif::copy(typeR->getLeftBound());
}

Object *HIFSemantics::_getSimplifiedSymbol_low(Object *s)
{
    FunctionCall *f = dynamic_cast<FunctionCall *>(s);
    messageAssert((f != nullptr), "Expected FunctionCall", s, this);

    Range *typeR = _getSpanFromSpanOperatingFunctions(f, this);
    messageAssert(
        typeR->getDirection() == dir_upto || typeR->getDirection() == dir_downto, "Unsupported range direction", f,
        this);

    return typeR->getDirection() == dir_upto ? hif::copy(typeR->getLeftBound()) : hif::copy(typeR->getRightBound());
}

Object *HIFSemantics::_getSimplifiedSymbol_length(Object *s)
{
    FunctionCall *f = dynamic_cast<FunctionCall *>(s);
    messageAssert((f != nullptr), "Expected FunctionCall", s, this);

    Range *typeR = _getSpanFromSpanOperatingFunctions(f, this);
    return spanGetSize(typeR, this);
}

Object *HIFSemantics::_getSimplifiedSymbol_stable(Object *s)
{
    FunctionCall *f = dynamic_cast<FunctionCall *>(s);
    messageAssert((f != nullptr), "Expected FunctionCall", s, this);

    f = hif::copy(f);
    hif::semantics::resetDeclarations(f);
    f->setName("hif_vhdl_event");
    Expression *e = _hifFactory.expression(op_not, f);

    return e;
}

Object *HIFSemantics::_getSimplifiedSymbol_ascending(Object *s)
{
    FunctionCall *f = dynamic_cast<FunctionCall *>(s);
    messageAssert((f != nullptr), "Expected FunctionCall", s, this);

    Range *typeR = _getSpanFromSpanOperatingFunctions(f, this);
    messageAssert(
        typeR->getDirection() == dir_upto || typeR->getDirection() == dir_downto, "Unsupported range direction", f,
        this);

    return _factory.boolval(typeR->getDirection() == dir_upto);
}
Object *HIFSemantics::_getSimplifiedSymbol_iteratedConcat(Object *s)
{
    FunctionCall *f = dynamic_cast<FunctionCall *>(s);
    messageAssert((f != nullptr), "Expected FunctionCall", s, this);

    FunctionCall *ret = hif::copy(f);
    ret->setName("hif_verilog_iterated_concat");
    for (BList<TPAssign>::iterator i = ret->templateParameterAssigns.begin(); i != ret->templateParameterAssigns.end();
         ++i) {
        ValueTPAssign *vtpa = dynamic_cast<ValueTPAssign *>(*i);
        messageAssert(vtpa != nullptr, "Expected a ValueTPAssign for iterated_concat.", f, this);
        Cast *c = new Cast();
        c->setType(_factory.integer(nullptr, false));
        c->setValue(vtpa->setValue(nullptr));
        vtpa->setValue(c);
    }

    Instance *inst = dynamic_cast<Instance *>(ret->getInstance());
    messageAssert(inst != nullptr, "Expected instance.", s, this);
    Library *lib = dynamic_cast<Library *>(inst->getReferencedType());
    messageAssert(lib != nullptr, "Expected library instance.", s, this);

    lib->setName("hif_verilog_standard");
    hif::semantics::resetDeclarations(ret);

    return ret;
}
Object *HIFSemantics::_getSimplifiedSymbol_withVerilogIntegers(
    Object *s,
    bool /*intReturnedType*/,
    const std::vector<int> &intParamIndexes)
{
    FunctionCall *fc  = dynamic_cast<FunctionCall *>(s);
    ProcedureCall *pc = dynamic_cast<ProcedureCall *>(s);
    messageAssert(fc != nullptr || pc != nullptr, "Unexpected symbol", s, this);

    if (pc != nullptr) {
        return _fixCallIntegerParameters(pc, intParamIndexes, this);
    } else {
        FunctionCall *fcopy = _fixCallIntegerParameters(fc, intParamIndexes, this);
        Object *ret         = fcopy;
        //        if (intReturnedType)
        //        {
        //            ret = _factory.cast(
        //                        _factory.integer(),
        //                        fcopy);
        //        }

        return ret;
    }
}

} // namespace semantics
} // namespace hif
