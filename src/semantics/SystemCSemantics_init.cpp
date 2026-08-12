/// @file SystemCSemantics_init.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/semantics/SystemCSemantics.hpp"

namespace hif
{
namespace semantics
{

void SystemCSemantics::_initForbiddenNames()
{
    // Forbidden tab (i.e. reserved words) are cabled
    _forbiddenNames.insert("case");
    _forbiddenNames.insert("char");
    _forbiddenNames.insert("const");
    _forbiddenNames.insert("default");
    _forbiddenNames.insert("delete");
    _forbiddenNames.insert("div");
    _forbiddenNames.insert("double");
    _forbiddenNames.insert("else");
    _forbiddenNames.insert("extern");
    _forbiddenNames.insert("float");
    _forbiddenNames.insert("for");
    _forbiddenNames.insert("if");
    _forbiddenNames.insert("int");
    _forbiddenNames.insert("long");
    _forbiddenNames.insert("log2"); // Necessary for test log2
    _forbiddenNames.insert("new");
    _forbiddenNames.insert("return");
    _forbiddenNames.insert("short");
    _forbiddenNames.insert("switch");
    _forbiddenNames.insert("this");
    _forbiddenNames.insert("unsigned");
    _forbiddenNames.insert("void");
    _forbiddenNames.insert("while");
    _forbiddenNames.insert("break");
    _forbiddenNames.insert("do");
    _forbiddenNames.insert("or");
    _forbiddenNames.insert("register");
    _forbiddenNames.insert("return");
    _forbiddenNames.insert("template");
    _forbiddenNames.insert("typename");
    _forbiddenNames.insert("while");
    _forbiddenNames.insert("xor");
}
void SystemCSemantics::_initStandardFilenames()
{
    _standardFilenames["sc_core"]               = "systemc";
    _standardFilenames["sc_dt"]                 = "systemc";
    _standardFilenames["tlm"]                   = "tlm.h";
    _standardFilenames["cmath"]                 = "cmath";
    _standardFilenames["cstdlib"]               = "cstdlib";
    _standardFilenames["ctime"]                 = "ctime";
    _standardFilenames["iostream"]              = "iostream";
    _standardFilenames["string"]                = "string";
    _standardFilenames["cstring"]               = "cstring";
    _standardFilenames["new"]                   = "new";
    _standardFilenames["cstddef"]               = "cstddef";
    _standardFilenames["vector"]                = "vector";
    _standardFilenames["cstdio"]                = "cstdio";
    _standardFilenames["sca_eln"]               = "systemc-ams.h";
    _standardFilenames["SystemVueModelBuilder"] = "SystemVue/ModelBuilder.h";
    _standardFilenames["tlm_utils"]             = "tlm_utils/tlm_quantumkeeper.h";
}
void SystemCSemantics::_initStandardSymbols()
{
    // Predefined libraries
    const char *hif_vhdl_ieee_math_complex[]       = {"hif_vhdl_ieee_math_complex", "cmath", "cstdlib", "standard"};
    const char *hif_vhdl_ieee_math_real[]          = {"hif_vhdl_ieee_math_real", "cmath", "cstdlib", "standard"};
    const char *hif_vhdl_ieee_numeric_bit[]        = {"hif_vhdl_ieee_numeric_bit"};
    const char *hif_vhdl_ieee_numeric_std[]        = {"hif_vhdl_ieee_numeric_std", "hif_systemc_extensions"};
    const char *hif_vhdl_ieee_std_logic_1164[]     = {"hif_vhdl_ieee_std_logic_1164"};
    const char *hif_vhdl_ieee_std_logic_arith[]    = {"hif_vhdl_ieee_std_logic_arith"};
    const char *hif_vhdl_ieee_std_logic_arith_ex[] = {"hif_vhdl_ieee_std_logic_arith_ex"};
    const char *hif_vhdl_ieee_std_logic_misc[]     = {"hif_vhdl_ieee_std_logic_misc"};
    const char *hif_vhdl_ieee_std_logic_signed[]   = {"hif_vhdl_ieee_std_logic_signed"};
    const char *hif_vhdl_ieee_std_logic_textio[]   = {"hif_vhdl_ieee_std_logic_textio"};
    const char *hif_vhdl_ieee_std_logic_unsigned[] = {"hif_vhdl_ieee_std_logic_unsigned"};
    const char *hif_vhdl_standard[]                = {"hif_vhdl_standard"};
    const char *hif_vhdl_textio[]                  = {"hif_vhdl_std_textio"};

    const char *hif_verilog_standard[] = {"hif_verilog_standard", "cstdio"};

    const char *standard[]                          = {"standard"};
    const char *sc_core[]                           = {"sc_core"};
    const char *sc_core_hif_vhdl_standard[]         = {"sc_core", "hif_vhdl_standard"};
    const char *cmath[]                             = {"cmath"};
    const char *cstdlib[]                           = {"cstdlib"};
    const char *cstdio[]                            = {"cstdio"};
    const char *hif_systemc_extensions[]            = {"hif_systemc_extensions"};
    const char *hif_systemc_SystemVueModelBuilder[] = {"SystemVueModelBuilder"};
    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_standard -> (SystemC) systemc, hif_vhdl_standard
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_standard")] =
        _makeValue(sc_core_hif_vhdl_standard, 2, "hif_vhdl_standard", MAP_KEEP);

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_base")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_base", SIMPLIFIED); // FIXME unsupported

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_left")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_left", SIMPLIFIED);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_right")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_right", SIMPLIFIED);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_high")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_high", SIMPLIFIED);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_low")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_low", SIMPLIFIED);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_ascending")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_ascending", SIMPLIFIED);

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_image")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_image", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_value")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_value", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_pos")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_pos", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_val")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_val", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_succ")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_succ", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_pred")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_pred", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_leftof")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_leftof", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_rightof")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_rightof", SIMPLIFIED); // FIXME unsupported

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_range")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_range", SIMPLIFIED); // FIXME simplify
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_reverse_range")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_reverse_range", SIMPLIFIED); // FIXME simplify
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_length")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_length", SIMPLIFIED);

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_time_units")] =
        _makeValue(sc_core, 1, "sc_time_unit", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_fs")]  = _makeValue(sc_core, 1, "SC_FS", MAP_KEEP);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_ps")]  = _makeValue(sc_core, 1, "SC_PS", MAP_KEEP);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_ns")]  = _makeValue(sc_core, 1, "SC_NS", MAP_KEEP);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_us")]  = _makeValue(sc_core, 1, "SC_US", MAP_KEEP);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_ms")]  = _makeValue(sc_core, 1, "SC_MS", MAP_KEEP);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_sec")] = _makeValue(sc_core, 1, "SC_SEC", MAP_KEEP);

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_delayed")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_delayed", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_stable")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_stable", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_quiet")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_quiet", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_transaction")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_transaction", SIMPLIFIED); // FIXME unsupported

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_event")] = _makeValue(sc_core, 1, "event", MAP_DELETE);

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_active")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_active", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_last_event")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_last_event", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_last_active")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_last_active", SIMPLIFIED); // FIXME unsupported

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_last_value")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_last_value", SIMPLIFIED);

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_driving")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_driving", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_driving_value")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_driving_value", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_simple_name")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_simple_name", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_instance_name")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_instance_name", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_path_name")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_path_name", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_foreign")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_foreign", SIMPLIFIED); // FIXME unsupported

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_severity_level")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_severity_level", MAP_KEEP);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_delay_length")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_delay_length", MAP_KEEP);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_file_open_kind")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_file_open_kind", MAP_KEEP);
    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_file_open_status")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_file_open_status", MAP_KEEP);

    _standardSymbols[_makeKey("hif_vhdl_standard", "hif_vhdl_now")] =
        _makeValue(sc_core, 1, "sc_time_stamp", MAP_DELETE);
    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_ieee_std_logic_1164 -> (SystemC) hif_vhdl_ieee_std_logic_1164
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_ieee_std_logic_1164", "hif_vhdl_ieee_std_logic_1164")] =
        _makeValue(hif_vhdl_ieee_std_logic_1164, 1, "hif_vhdl_ieee_std_logic_1164", MAP_KEEP);
    _standardSymbols[_makeKey("hif_vhdl_ieee_std_logic_1164", "hif_vhdl_to_x01z")] =
        _makeValue(hif_vhdl_ieee_std_logic_1164, 1, "hif_vhdl_to_x01z", SIMPLIFIED);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_ieee_std_logic_arith -> (SystemC) hif_vhdl_ieee_std_logic_arith
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_ieee_std_logic_arith", "hif_vhdl_ieee_std_logic_arith")] =
        _makeValue(hif_vhdl_ieee_std_logic_arith, 1, "hif_vhdl_ieee_std_logic_arith", MAP_KEEP);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_ieee_numeric_std -> (SystemC) hif_vhdl_ieee_numeric_std
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_ieee_numeric_std", "hif_vhdl_ieee_numeric_std")] =
        _makeValue(hif_vhdl_ieee_numeric_std, 2, "hif_vhdl_ieee_numeric_std", MAP_KEEP);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_ieee_numeric_bit -> (SystemC) hif_vhdl_ieee_numeric_bit
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_ieee_numeric_bit", "hif_vhdl_ieee_numeric_bit")] =
        _makeValue(hif_vhdl_ieee_numeric_bit, 1, "hif_vhdl_ieee_numeric_bit", MAP_KEEP);
    // TODO insert supported cases

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_ieee_std_logic_signed -> (SystemC) hif_vhdl_ieee_std_logic_signed
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_ieee_std_logic_signed", "hif_vhdl_ieee_std_logic_signed")] =
        _makeValue(hif_vhdl_ieee_std_logic_signed, 1, "hif_vhdl_ieee_std_logic_signed", MAP_KEEP);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_ieee_std_logic_unsigned -> (SystemC) hif_vhdl_ieee_std_logic_unsigned
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_ieee_std_logic_unsigned", "hif_vhdl_ieee_std_logic_unsigned")] =
        _makeValue(hif_vhdl_ieee_std_logic_unsigned, 1, "hif_vhdl_ieee_std_logic_unsigned", MAP_KEEP);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_ieee_math_real -> (SystemC) hif_vhdl_ieee_math_real
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_ieee_math_real")] =
        _makeValue(hif_vhdl_ieee_math_real, 4, "hif_vhdl_ieee_math_real", MAP_KEEP);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_ceil")]  = _makeValue(cmath, 1, "ceil", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_floor")] = _makeValue(cmath, 1, "floor", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_round")] = _makeValue(cmath, 1, "round", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_trunc")] = _makeValue(cmath, 1, "trunc", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_realmax")] =
        _makeValue(cmath, 1, "fmax", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_realmin")] =
        _makeValue(cmath, 1, "fmin", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_srand")] =
        _makeValue(cstdlib, 1, "srand", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_rand")] = _makeValue(cstdlib, 1, "rand", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_sqrt")] = _makeValue(cmath, 1, "sqrt", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_cbrt")] = _makeValue(cmath, 1, "cbrt", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_exp")]  = _makeValue(cmath, 1, "exp", MAP_DELETE);
    //_standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_log")] = _makeValue(cmath, 1, "log", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_sin")]  = _makeValue(cmath, 1, "sin", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_cos")]  = _makeValue(cmath, 1, "cos", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_tan")]  = _makeValue(cmath, 1, "tan", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_arcsin")] = _makeValue(cmath, 1, "asin", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_arccos")] = _makeValue(cmath, 1, "acos", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_arctan")] = _makeValue(cmath, 1, "atan", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_sinh")]   = _makeValue(cmath, 1, "sinh", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_cosh")]   = _makeValue(cmath, 1, "cosh", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_tanh")]   = _makeValue(cmath, 1, "tanh", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_arcsinh")] =
        _makeValue(cmath, 1, "asinh", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_arccosh")] =
        _makeValue(cmath, 1, "acosh", MAP_DELETE);
    _standardSymbols[_makeKey("hif_vhdl_ieee_math_real", "hif_vhdl_arctanh")] =
        _makeValue(cmath, 1, "atanh", MAP_DELETE);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_systemc_standard -> (SystemC) standard
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_systemc_standard", "hif_systemc_standard")] =
        _makeValue(standard, 1, "standard", MAP_KEEP);
    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_systemc_hif_systemc_extensions -> (SystemC) hif_systemc_extensions
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_systemc_hif_systemc_extensions", "hif_systemc_hif_systemc_extensions")] =
        _makeValue(hif_systemc_extensions, 1, "hif_systemc_extensions", MAP_KEEP);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_std_textio -> (SystemC) hif_vhdl_std_textio
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_std_textio", "hif_vhdl_std_textio")] =
        _makeValue(hif_vhdl_textio, 1, "hif_vhdl_std_textio", MAP_KEEP);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_ieee_math_complex -> (SystemC) hif_vhdl_ieee_math_complex
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_ieee_math_complex", "hif_vhdl_ieee_math_complex")] =
        _makeValue(hif_vhdl_ieee_math_complex, 4, "hif_vhdl_ieee_math_complex", MAP_KEEP);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_ieee_std_logic_arith_ex -> (SystemC) hif_vhdl_ieee_std_logic_arith_ex
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_ieee_std_logic_arith_ex", "hif_vhdl_ieee_std_logic_arith_ex")] =
        _makeValue(hif_vhdl_ieee_std_logic_arith_ex, 1, "hif_vhdl_ieee_std_logic_arith_ex", MAP_KEEP);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_ieee_std_logic_textio -> (SystemC) hif_vhdl_ieee_std_logic_textio
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_ieee_std_logic_textio", "hif_vhdl_ieee_std_logic_textio")] =
        _makeValue(hif_vhdl_ieee_std_logic_textio, 1, "hif_vhdl_ieee_std_logic_textio", MAP_KEEP);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_vhdl_ieee_std_logic_misc -> (SystemC) hif_vhdl_ieee_std_logic_misc
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_vhdl_ieee_std_logic_misc", "hif_vhdl_ieee_std_logic_misc")] =
        _makeValue(hif_vhdl_ieee_std_logic_misc, 1, "hif_vhdl_ieee_std_logic_misc", MAP_KEEP);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_verilog_standard -> (SystemC) hif_verilog_standard
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_verilog_standard", "hif_verilog_standard")] =
        _makeValue(hif_verilog_standard, 2, "hif_verilog_standard", MAP_KEEP);
    _standardSymbols[_makeKey("hif_verilog_standard", "hif_verilog__system_display")] =
        _makeValue(cstdio, 1, "printf", MAP_DELETE);

    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_systemc_SystemVueModelBuilder -> (SystemC) SystemVueModelBuilder
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("hif_systemc_SystemVueModelBuilder", "hif_systemc_SystemVueModelBuilder")] =
        _makeValue(hif_systemc_SystemVueModelBuilder, 1, "hif_systemc_SystemVueModelBuilder", MAP_KEEP);

#if 1
    // /////////////////////////////////////////////////////////////////////////
    // (HIF) hif_verilog_vams_* -> (SystemC) hif_verilog_vams_*
    // Only for debug purpose
    // /////////////////////////////////////////////////////////////////////////

    const char *hif_verilog_vams_standard[]      = {"hif_verilog_vams_standard"};
    const char *hif_verilog_vams_constants[]     = {"hif_verilog_vams_constants"};
    const char *hif_verilog_vams_disciplines[]   = {"hif_verilog_vams_disciplines"};
    const char *hif_verilog_vams_driver_access[] = {"hif_verilog_vams_driver_access"};
    _standardSymbols[_makeKey("hif_verilog_vams_standard", "hif_verilog_vams_standard")] =
        _makeValue(hif_verilog_vams_standard, 1, "hif_verilog_vams_standard", MAP_KEEP);
    _standardSymbols[_makeKey("hif_verilog_vams_constants", "hif_verilog_vams_constants")] =
        _makeValue(hif_verilog_vams_constants, 1, "hif_verilog_vams_constants", MAP_KEEP);
    _standardSymbols[_makeKey("hif_verilog_vams_disciplines", "hif_verilog_vams_disciplines")] =
        _makeValue(hif_verilog_vams_disciplines, 1, "hif_verilog_vams_disciplines", MAP_KEEP);
    _standardSymbols[_makeKey("hif_verilog_vams_driver_access", "hif_verilog_vams_driver_access")] =
        _makeValue(hif_verilog_vams_driver_access, 1, "hif_verilog_vams_driver_access", MAP_KEEP);
#endif
}
} // namespace semantics
} // namespace hif
