/// @file hifEnums.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hifEnums.hpp"

#include "hif/hif.hpp"

namespace hif
{

std::string rangeDirectionToString(const RangeDirection t)
{
    switch (t) {
    case dir_upto:
        return "UPTO";
    case dir_downto:
        return "DOWNTO";
    default:;
    }
    messageError("Unexpected range direction", nullptr, nullptr);
}

RangeDirection rangeDirectionFromString(const std::string &s)
{
    if (s == "UPTO")
        return dir_upto;
    else if (s == "DOWNTO")
        return dir_downto;

    messageError(std::string("Unexpected range direction: ") + s, nullptr, nullptr);
}

std::string portDirectionToString(const PortDirection t)
{
    switch (t) {
    case dir_none:
        return "NONE";
    case dir_in:
        return "IN";
    case dir_out:
        return "OUT";
    case dir_inout:
        return "INOUT";

    default:;
    }

    messageError("Unexpected port direction", nullptr, nullptr);
}

PortDirection portDirectionFromString(const std::string &s)
{
    if (s == "NONE")
        return dir_none;
    else if (s == "IN")
        return dir_in;
    else if (s == "OUT")
        return dir_out;
    else if (s == "INOUT")
        return dir_inout;

    messageError(std::string("Unexpected port direction: ") + s, nullptr, nullptr);
}

std::string bitConstantToString(const BitConstant t)
{
    switch (t) {
    case bit_u:
        return "U";
    case bit_x:
        return "X";
    case bit_zero:
        return "0";
    case bit_one:
        return "1";
    case bit_z:
        return "Z";
    case bit_w:
        return "W";
    case bit_l:
        return "L";
    case bit_h:
        return "H";
    case bit_dontcare:
        return "-";

    default:;
    }

    messageError("Unexpected bit constant", nullptr, nullptr);
}

BitConstant bitConstantFromString(const std::string &s)
{
    if (s == "U")
        return bit_u;
    else if (s == "X")
        return bit_x;
    else if (s == "0")
        return bit_zero;
    else if (s == "1")
        return bit_one;
    else if (s == "Z")
        return bit_z;
    else if (s == "W")
        return bit_w;
    else if (s == "L")
        return bit_l;
    else if (s == "H")
        return bit_h;
    else if (s == "-")
        return bit_dontcare;

    messageError(std::string("Unexpected bit constant: ") + s, nullptr, nullptr);
}

std::string operatorToString(const Operator t)
{
    switch (t) {
    case op_none:
        return "NONE";
    case op_or:
        return "||";
    case op_bor:
        return "|";
    case op_and:
        return "&&";
    case op_band:
        return "&";
    case op_xor:
        return "^^";
    case op_bxor:
        return "^";
    case op_not:
        return "!";
    case op_bnot:
        return "~";
    case op_eq:
        return "==";
    case op_neq:
        return "!=";
    case op_case_eq:
        return "===";
    case op_case_neq:
        return "!==";
    case op_le:
        return "<=";
    case op_ge:
        return ">=";
    case op_lt:
        return "<";
    case op_gt:
        return ">";
    case op_sll:
        return "SLL";
    case op_srl:
        return "SRL";
    case op_sla:
        return "SLA";
    case op_sra:
        return "SRA";
    case op_rol:
        return "ROL";
    case op_ror:
        return "ROR";
    case op_plus:
        return "+";
    case op_minus:
        return "-";
    case op_mult:
        return "*";
    case op_div:
        return "/";
    case op_mod:
        return "MOD";
    case op_rem:
        return "REM";
    case op_pow:
        return "POW";
    case op_abs:
        return "ABS";
    case op_concat:
        return "CONCAT";
    case op_ref:
        return "@";
    case op_deref:
        return "$";
    case op_andrd:
        return "ANDRD";
    case op_orrd:
        return "ORRD";
    case op_xorrd:
        return "XORRD";
    case op_assign:
        return ":=";
    case op_log:
        return "LOG";
    case op_conv:
        return "<--";
    case op_bind:
        return "-->";
    case op_reverse:
        return "REVERSE";
    case op_size:
        return "SIZE";
    default:;
    }

    messageError("Unexpected operator", nullptr, nullptr);
}

std::string operatorToPlainString(const Operator t, const std::string &prefix, const std::string &suffix)
{
    std::string ret(prefix);
    switch (t) {
    case op_none:
        ret += "op_none";
        break;
    case op_or:
        ret += "op_or";
        break;
    case op_bor:
        ret += "op_bor";
        break;
    case op_and:
        ret += "op_and";
        break;
    case op_band:
        ret += "op_band";
        break;
    case op_xor:
        ret += "op_xor";
        break;
    case op_bxor:
        ret += "op_bxor";
        break;
    case op_not:
        ret += "op_not";
        break;
    case op_bnot:
        ret += "op_bnot";
        break;
    case op_eq:
        ret += "op_eq";
        break;
    case op_neq:
        ret += "op_neq";
        break;
    case op_case_eq:
        ret += "op_case_eq";
        break;
    case op_case_neq:
        ret += "op_case_neq";
        break;
    case op_le:
        ret += "op_le";
        break;
    case op_ge:
        ret += "op_ge";
        break;
    case op_lt:
        ret += "op_lt";
        break;
    case op_gt:
        ret += "op_gt";
        break;
    case op_sll:
        ret += "op_sll";
        break;
    case op_srl:
        ret += "op_srl";
        break;
    case op_sla:
        ret += "op_sla";
        break;
    case op_sra:
        ret += "op_sra";
        break;
    case op_rol:
        ret += "op_rol";
        break;
    case op_ror:
        ret += "op_ror";
        break;
    case op_plus:
        ret += "op_plus";
        break;
    case op_minus:
        ret += "op_minus";
        break;
    case op_mult:
        ret += "op_mult";
        break;
    case op_div:
        ret += "op_div";
        break;
    case op_mod:
        ret += "op_mod";
        break;
    case op_rem:
        ret += "op_rem";
        break;
    case op_pow:
        ret += "op_pow";
        break;
    case op_abs:
        ret += "op_abs";
        break;
    case op_concat:
        ret += "op_concat";
        break;
    case op_ref:
        ret += "op_ref";
        break;
    case op_deref:
        ret += "op_deref";
        break;
    case op_andrd:
        ret += "op_andrd";
        break;
    case op_orrd:
        ret += "op_orrd";
        break;
    case op_xorrd:
        ret += "op_xorrd";
        break;
    case op_assign:
        ret += "op_assign";
        break;
    case op_log:
        ret += "op_log";
        break;
    case op_conv:
        ret += "op_conv";
        break;
    case op_bind:
        ret += "op_bind";
        break;
    case op_reverse:
        ret += "op_reverse";
        break;
    case op_size:
        ret += "op_size";
        break;
    default:
        messageError("Unexpected operator", nullptr, nullptr);
    }

    return ret + suffix;
}

Operator operatorFromString(const std::string &s)
{
    if (s == "NONE")
        return op_none;
    else if (s == "||")
        return op_or;
    else if (s == "|")
        return op_bor;
    else if (s == "&&")
        return op_and;
    else if (s == "&")
        return op_band;
    else if (s == "^^")
        return op_xor;
    else if (s == "^")
        return op_bxor;
    else if (s == "!")
        return op_not;
    else if (s == "~")
        return op_bnot;
    else if (s == "==")
        return op_eq;
    else if (s == "!=")
        return op_neq;
    else if (s == "===")
        return op_case_eq;
    else if (s == "!==")
        return op_case_neq;
    else if (s == "<=")
        return op_le;
    else if (s == ">=")
        return op_ge;
    else if (s == "<")
        return op_lt;
    else if (s == ">")
        return op_gt;
    else if (s == "SLL")
        return op_sll;
    else if (s == "SRL")
        return op_srl;
    else if (s == "SLA")
        return op_sla;
    else if (s == "SRA")
        return op_sra;
    else if (s == "ROL")
        return op_rol;
    else if (s == "ROR")
        return op_ror;
    else if (s == "+")
        return op_plus;
    else if (s == "-")
        return op_minus;
    else if (s == "*")
        return op_mult;
    else if (s == "/")
        return op_div;
    else if (s == "MOD")
        return op_mod;
    else if (s == "REM")
        return op_rem;
    else if (s == "POW")
        return op_pow;
    else if (s == "ABS")
        return op_abs;
    else if (s == "CONCAT")
        return op_concat;
    else if (s == "@")
        return op_ref;
    else if (s == "$")
        return op_deref;
    else if (s == "ANDRD")
        return op_andrd;
    else if (s == "ORRD")
        return op_orrd;
    else if (s == "XORRD")
        return op_xorrd;
    else if (s == ":=")
        return op_assign;
    else if (s == "LOG")
        return op_log;
    else if (s == "<--")
        return op_conv;
    else if (s == "-->")
        return op_bind;
    else if (s == "REVERSE")
        return op_reverse;
    else if (s == "SIZE")
        return op_size;

    messageError(std::string("Unexpected operator: ") + s, nullptr, nullptr);
}

Operator operatorFromPlainString(const std::string &s, const std::string &prefix, const std::string &suffix)
{
    std::string tmp(s);
    if (prefix.size() >= tmp.size())
        return op_none;
    if (tmp.substr(0, prefix.size()) != prefix)
        return op_none;
    tmp = tmp.substr(prefix.size());
    if (suffix.size() >= tmp.size())
        return op_none;
    if (tmp.substr(tmp.size() - suffix.size()) != suffix)
        return op_none;
    tmp = tmp.substr(0, tmp.size() - suffix.size());

    if (tmp == "op_none")
        return op_none;
    else if (tmp == "op_or")
        return op_or;
    else if (tmp == "op_bor")
        return op_bor;
    else if (tmp == "op_and")
        return op_and;
    else if (tmp == "op_band")
        return op_band;
    else if (tmp == "op_xor")
        return op_xor;
    else if (tmp == "op_bxor")
        return op_bxor;
    else if (tmp == "op_not")
        return op_not;
    else if (tmp == "op_bnot")
        return op_bnot;
    else if (tmp == "op_eq")
        return op_eq;
    else if (tmp == "op_neq")
        return op_neq;
    else if (tmp == "op_case_eq")
        return op_case_eq;
    else if (tmp == "op_case_neq")
        return op_case_neq;
    else if (tmp == "op_le")
        return op_le;
    else if (tmp == "op_ge")
        return op_ge;
    else if (tmp == "op_lt")
        return op_lt;
    else if (tmp == "op_gt")
        return op_gt;
    else if (tmp == "op_sll")
        return op_sll;
    else if (tmp == "op_srl")
        return op_srl;
    else if (tmp == "op_sla")
        return op_sla;
    else if (tmp == "op_sra")
        return op_sra;
    else if (tmp == "op_rol")
        return op_rol;
    else if (tmp == "op_ror")
        return op_ror;
    else if (tmp == "op_plus")
        return op_plus;
    else if (tmp == "op_minus")
        return op_minus;
    else if (tmp == "op_mult")
        return op_mult;
    else if (tmp == "op_div")
        return op_div;
    else if (tmp == "op_mod")
        return op_mod;
    else if (tmp == "op_rem")
        return op_rem;
    else if (tmp == "op_pow")
        return op_pow;
    else if (tmp == "op_abs")
        return op_abs;
    else if (tmp == "op_concat")
        return op_concat;
    else if (tmp == "op_ref")
        return op_ref;
    else if (tmp == "op_deref")
        return op_deref;
    else if (tmp == "op_andrd")
        return op_andrd;
    else if (tmp == "op_orrd")
        return op_orrd;
    else if (tmp == "op_xorrd")
        return op_xorrd;
    else if (tmp == "op_assign")
        return op_assign;
    else if (tmp == "op_log")
        return op_log;
    else if (tmp == "op_conv")
        return op_conv;
    else if (tmp == "op_bind")
        return op_bind;

    return op_none;
}

std::string processFlavourToString(const ProcessFlavour t)
{
    switch (t) {
    case pf_method:
        return "METHOD";
    case pf_thread:
        return "THREAD";
    case pf_hdl:
        return "HDL";
    case pf_initial:
        return "INITIAL";
    case pf_analog:
        return "ANALOG";

    default:;
    }

    messageError("Unexpected process flavour", nullptr, nullptr);
}

ProcessFlavour processFlavourFromString(const std::string &s)
{
    if (s == "METHOD")
        return pf_method;
    else if (s == "THREAD")
        return pf_thread;
    else if (s == "HDL")
        return pf_hdl;
    else if (s == "INITIAL")
        return pf_initial;
    else if (s == "ANALOG")
        return pf_analog;

    messageError(std::string("Unexpected process flavour: ") + s, nullptr, nullptr);
}

std::string languageIDToString(const LanguageID t)
{
    switch (t) {
    case rtl:
        return "RTL";
    case tlm:
        return "TLM";
    case cpp:
        return "CPP";
    case c:
        return "C";
    case psl:
        return "PSL";
    case ams:
        return "AMS";

    default:;
    }

    messageError("Unexpected language ID", nullptr, nullptr);
}

LanguageID languageIDFromString(const std::string &s)
{
    if (s == "RTL")
        return rtl;
    else if (s == "TLM")
        return tlm;
    else if (s == "CPP")
        return cpp;
    else if (s == "C")
        return c;
    else if (s == "PSL")
        return psl;
    else if (s == "AMS")
        return ams;

    messageError(std::string("Unexpected language ID: ") + s, nullptr, nullptr);
}

const char *getPropertyName(const PropertyId id)
{
    switch (id) {
    case PROPERTY_CONFIGURATION_FLAG:
        return "CONFIGURATION_FLAG";
    case PROPERTY_TLM_FORCEARROW:
        return "TLM_FORCEARROW";
    case PROPERTY_REQUIRED_MACRO:
        return "REQUIRED_MACRO";
    case PROPERTY_REQUIRED_MACRO_HH:
        return "REQUIRED_MACRO_HH";
    case PROPERTY_REQUIRED_MACRO_CC:
        return "REQUIRED_MACRO_CC";
    case PROPERTY_UNSUPPORTED:
        return "UNSUPPORTED";
    case PROPERTY_CONSTEXPR:
        return "CONSTEXPR";
    case PROPERTY_METHOD_EXPLICIT_PARAMETERS:
        return "METHOD_EXPLICIT_PARAMETERS";
    case PROPERTY_TEMPORARY_OBJECT:
        return "TEMPORARY_OBJECT";
    case PROPERTY_ORIGINAL_BITWIDTH:
        return "ORIGINAL_BITWIDTH";
    default:;
    }

    messageError("Unknown property id.", nullptr, nullptr);
}

std::string classIDToString(const ClassId id)
{
    switch (id) {
    case CLASSID_AGGREGATEALT:
        return "AggregateAlt";
    case CLASSID_AGGREGATE:
        return "Aggregate";
    case CLASSID_ALIAS:
        return "Alias";
    case CLASSID_ARRAY:
        return "Array";
    case CLASSID_ASSIGN:
        return "Assign";
    case CLASSID_BIT:
        return "Bit";
    case CLASSID_BITVALUE:
        return "BitValue";
    case CLASSID_BITVECTOR:
        return "Bitvector";
    case CLASSID_BITVECTORVALUE:
        return "BitvectorValue";
    case CLASSID_BOOL:
        return "Bool";
    case CLASSID_BOOLVALUE:
        return "BoolValue";
    case CLASSID_BREAK:
        return "Break";
    case CLASSID_CAST:
        return "Cast";
    case CLASSID_CHAR:
        return "Char";
    case CLASSID_CHARVALUE:
        return "CharValue";
    case CLASSID_CONST:
        return "Const";
    case CLASSID_CONTENTS:
        return "Contents";
    case CLASSID_CONTINUE:
        return "Continue";
    case CLASSID_DESIGNUNIT:
        return "DesignUnit";
    case CLASSID_ENTITY:
        return "Entity";
    case CLASSID_ENUM:
        return "Enum";
    case CLASSID_ENUMVALUE:
        return "EnumValue";
    case CLASSID_EVENT:
        return "Event";
    case CLASSID_EXPRESSION:
        return "Expression";
    case CLASSID_FIELD:
        return "Field";
    case CLASSID_FIELDREFERENCE:
        return "FieldReference";
    case CLASSID_FILE:
        return "File";
    case CLASSID_FORGENERATE:
        return "ForGenerate";
    case CLASSID_FOR:
        return "For";
    case CLASSID_FUNCTIONCALL:
        return "FunctionCall";
    case CLASSID_FUNCTION:
        return "Function";
    case CLASSID_GLOBALACTION:
        return "GlobalAction";
    case CLASSID_IDENTIFIER:
        return "Identifier";
    case CLASSID_IFALT:
        return "IfAlt";
    case CLASSID_IFGENERATE:
        return "IfGenerate";
    case CLASSID_IF:
        return "If";
    case CLASSID_INSTANCE:
        return "Instance";
    case CLASSID_INT:
        return "Int";
    case CLASSID_INTVALUE:
        return "IntValue";
    case CLASSID_LIBRARYDEF:
        return "LibraryDef";
    case CLASSID_LIBRARY:
        return "Library";
    case CLASSID_MEMBER:
        return "Member";
    case CLASSID_NULL:
        return "Null";
    case CLASSID_PARAMETERASSIGN:
        return "ParameterAssign";
    case CLASSID_PARAMETER:
        return "Parameter";
    case CLASSID_POINTER:
        return "Pointer";
    case CLASSID_PORTASSIGN:
        return "PortAssign";
    case CLASSID_PORT:
        return "Port";
    case CLASSID_PROCEDURECALL:
        return "ProcedureCall";
    case CLASSID_PROCEDURE:
        return "Procedure";
    case CLASSID_RANGE:
        return "Range";
    case CLASSID_REAL:
        return "Real";
    case CLASSID_REALVALUE:
        return "RealValue";
    case CLASSID_RECORD:
        return "Record";
    case CLASSID_RECORDVALUEALT:
        return "RecordValueAlt";
    case CLASSID_RECORDVALUE:
        return "RecordValue";
    case CLASSID_REFERENCE:
        return "Reference";
    case CLASSID_RETURN:
        return "Return";
    case CLASSID_SIGNAL:
        return "Signal";
    case CLASSID_SIGNED:
        return "Signed";
    case CLASSID_SLICE:
        return "Slice";
    case CLASSID_STATE:
        return "State";
    case CLASSID_STATETABLE:
        return "StateTable";
    case CLASSID_STRING:
        return "String";
    case CLASSID_STRINGVALUE:
        return "StringValue";
    case CLASSID_SWITCHALT:
        return "SwitchAlt";
    case CLASSID_SWITCH:
        return "Switch";
    case CLASSID_SYSTEM:
        return "System";
    case CLASSID_TIME:
        return "Time";
    case CLASSID_TIMEVALUE:
        return "TimeValue";
    case CLASSID_TRANSITION:
        return "Transition";
    case CLASSID_TYPEDEF:
        return "TypeDef";
    case CLASSID_TYPEREFERENCE:
        return "TypeReference";
    case CLASSID_TYPETPASSIGN:
        return "TypeTPAssign";
    case CLASSID_TYPETP:
        return "TypeTP";
    case CLASSID_UNSIGNED:
        return "Unsigned";
    case CLASSID_VALUESTATEMENT:
        return "ValueStatement";
    case CLASSID_VALUETPASSIGN:
        return "ValueTPAssign";
    case CLASSID_VALUETP:
        return "ValueTP";
    case CLASSID_VARIABLE:
        return "Variable";
    case CLASSID_VIEW:
        return "View";
    case CLASSID_VIEWREFERENCE:
        return "ViewReference";
    case CLASSID_WAIT:
        return "Wait";
    case CLASSID_WHENALT:
        return "WhenAlt";
    case CLASSID_WHEN:
        return "When";
    case CLASSID_WHILE:
        return "While";
    case CLASSID_WITHALT:
        return "WithAlt";
    case CLASSID_WITH:
        return "With";
    default:;
    }
    return "";
}

std::string caseSemanticsToString(const CaseSemantics c)
{
    switch (c) {
    case CASE_LITERAL:
        return "CASE_LITERAL";
    case CASE_X:
        return "CASE_X";
    case CASE_Z:
        return "CASE_Z";
    default:;
    }

    messageError("Unexpected case semantics", nullptr, nullptr);
}

CaseSemantics caseSemanticsFromString(const std::string &c)
{
    if (c == "CASE_LITERAL")
        return CASE_LITERAL;
    else if (c == "CASE_X")
        return CASE_X;
    else if (c == "CASE_Z")
        return CASE_Z;

    messageError(std::string("Unexpected case semantics: ") + c, nullptr, nullptr);
}

} // namespace hif
