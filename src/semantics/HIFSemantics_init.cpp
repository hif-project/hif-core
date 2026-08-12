/// @file HIFSemantics_init.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/HIFSemantics.hpp"
namespace hif
{
namespace semantics
{

void HIFSemantics::_initForbiddenNames()
{
    // ntd
}

void HIFSemantics::_initStandardFilenames()
{
    // ntd
}

void HIFSemantics::_initStandardSymbols()
{
    //const char * hif_vhdl_ieee_math_complex[] = {"hif_vhdl_ieee_math_complex"};
    //const char * hif_vhdl_ieee_math_real[] = {"hif_vhdl_ieee_math_real"};
    //const char * hif_vhdl_ieee_numeric_bit[] = {"hif_vhdl_ieee_numeric_bit"};
    const char *hif_vhdl_ieee_numeric_std[]     = {"hif_vhdl_ieee_numeric_std"};
    const char *hif_vhdl_ieee_std_logic_1164[]  = {"hif_vhdl_ieee_std_logic_1164"};
    const char *hif_vhdl_ieee_std_logic_arith[] = {"hif_vhdl_ieee_std_logic_arith"};
    //const char * hif_vhdl_ieee_std_logic_arith_ex[] = {"hif_vhdl_ieee_std_logic_arith_ex"};
    const char *hif_vhdl_ieee_std_logic_misc[]  = {"hif_vhdl_ieee_std_logic_misc"};
    //const char * hif_vhdl_ieee_std_logic_signed[] = {"hif_vhdl_ieee_std_logic_signed"};
    //const char * hif_vhdl_ieee_std_logic_textio[] = {"hif_vhdl_ieee_std_logic_textio"};
    //const char * hif_vhdl_ieee_std_logic_unsigned[] = {"hif_vhdl_ieee_std_logic_unsigned"};
    const char *hif_vhdl_standard[]             = {"hif_vhdl_standard"};
    //const char * hif_vhdl_textio[] = {"hif_vhdl_std_textio"};

    const char *hif_verilog_standard[]      = {"hif_verilog_standard"};
    const char *hif_verilog_vams_standard[] = {"hif_verilog_vams_standard"};

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_ieee_math_complex
    // /////////////////////////////////////////////////////////////////////////

    // TODO

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_ieee_math_real
    // /////////////////////////////////////////////////////////////////////////

    // TODO

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_ieee_numeric_bit
    // /////////////////////////////////////////////////////////////////////////

    // TODO

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_ieee_numeric_std
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("ieee_numeric_std", "shift_left")] =
        _makeValue(hif_vhdl_ieee_numeric_std, 1, "hif_vhdl_shift_left", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_numeric_std", "shift_right")] =
        _makeValue(hif_vhdl_ieee_numeric_std, 1, "hif_vhdl_shift_right", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_numeric_std", "rotate_left")] =
        _makeValue(hif_vhdl_ieee_numeric_std, 1, "hif_vhdl_rotate_left", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_numeric_std", "rotate_right")] =
        _makeValue(hif_vhdl_ieee_numeric_std, 1, "hif_vhdl_rotate_right", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_numeric_std", "resize")] =
        _makeValue(hif_vhdl_ieee_numeric_std, 1, "hif_vhdl_resize", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_numeric_std", "to_signed")] =
        _makeValue(hif_vhdl_ieee_numeric_std, 1, "hif_vhdl_to_signed", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_numeric_std", "to_std_logic_vector")] =
        _makeValue(hif_vhdl_ieee_numeric_std, 1, "hif_vhdl_to_std_logic_vector", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_numeric_std", "to_unsigned")] =
        _makeValue(hif_vhdl_ieee_numeric_std, 1, "hif_vhdl_to_unsigned", SIMPLIFIED);

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_ieee_std_logic_1164
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("ieee_std_logic_1164", "to_stdulogic")] =
        _makeValue(hif_vhdl_ieee_std_logic_1164, 1, "hif_vhdl_to_stdulogic", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_std_logic_1164", "to_stdlogicvector")] =
        _makeValue(hif_vhdl_ieee_std_logic_1164, 1, "hif_vhdl_to_stdlogicvector", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_std_logic_1164", "to_stdulogicvector")] =
        _makeValue(hif_vhdl_ieee_std_logic_1164, 1, "hif_vhdl_to_stdulogicvector", SIMPLIFIED);

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_ieee_std_logic_arith
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("ieee_std_logic_arith", "conv_integer")] =
        _makeValue(hif_vhdl_ieee_std_logic_arith, 1, "hif_vhdl_conv_integer", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_std_logic_arith", "conv_signed")] =
        _makeValue(hif_vhdl_ieee_std_logic_arith, 1, "hif_vhdl_conv_signed", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_std_logic_arith", "conv_unsigned")] =
        _makeValue(hif_vhdl_ieee_std_logic_arith, 1, "hif_vhdl_conv_unsigned", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_std_logic_arith", "conv_std_logic_vector")] =
        _makeValue(hif_vhdl_ieee_std_logic_arith, 1, "hif_vhdl_conv_std_logic_vector", SIMPLIFIED);

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_ieee_std_logic_arith_ex
    // /////////////////////////////////////////////////////////////////////////

    // TODO

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_ieee_std_logic_misc
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("ieee_std_logic_misc", "and_reduce")] =
        _makeValue(hif_vhdl_ieee_std_logic_misc, 1, "hif_vhdl_and_reduce", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_std_logic_misc", "nand_reduce")] =
        _makeValue(hif_vhdl_ieee_std_logic_misc, 1, "hif_vhdl_nand_reduce", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_std_logic_misc", "or_reduce")] =
        _makeValue(hif_vhdl_ieee_std_logic_misc, 1, "hif_vhdl_or_reduce", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_std_logic_misc", "nor_reduce")] =
        _makeValue(hif_vhdl_ieee_std_logic_misc, 1, "hif_vhdl_nor_reduce", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_std_logic_misc", "xor_reduce")] =
        _makeValue(hif_vhdl_ieee_std_logic_misc, 1, "hif_vhdl_xor_reduce", SIMPLIFIED);
    _standardSymbols[_makeKey("ieee_std_logic_misc", "xnor_reduce")] =
        _makeValue(hif_vhdl_ieee_std_logic_misc, 1, "hif_vhdl_xnor_reduce", SIMPLIFIED);

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_ieee_std_logic_signed
    // /////////////////////////////////////////////////////////////////////////

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_ieee_std_logic_textio
    // /////////////////////////////////////////////////////////////////////////

    // TODO

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_ieee_std_logic_unsigned
    // /////////////////////////////////////////////////////////////////////////

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_standard
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("standard", "base")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_base", SIMPLIFIED); // FIXME unsupported

    _standardSymbols[_makeKey("standard", "left")]  = _makeValue(hif_vhdl_standard, 1, "hif_vhdl_left", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "right")] = _makeValue(hif_vhdl_standard, 1, "hif_vhdl_right", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "high")]  = _makeValue(hif_vhdl_standard, 1, "hif_vhdl_high", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "low")]   = _makeValue(hif_vhdl_standard, 1, "hif_vhdl_low", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "ascending")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_ascending", SIMPLIFIED);

    _standardSymbols[_makeKey("standard", "image")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_image", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "value")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_value", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "pos")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_pos", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "val")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_val", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "succ")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_succ", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "pred")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_pred", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "leftof")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_leftof", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "rightof")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_rightof", SIMPLIFIED); // FIXME unsupported

    _standardSymbols[_makeKey("standard", "range")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_range", SIMPLIFIED); // FIXME simplify
    _standardSymbols[_makeKey("standard", "reverse_range")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_reverse_range", SIMPLIFIED); // FIXME simplify
    _standardSymbols[_makeKey("standard", "length")] = _makeValue(hif_vhdl_standard, 1, "hif_vhdl_length", SIMPLIFIED);

    _standardSymbols[_makeKey("standard", "delayed")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_delayed", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "stable")] = _makeValue(hif_vhdl_standard, 1, "hif_vhdl_stable", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "quiet")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_quiet", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "transaction")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_transaction", SIMPLIFIED); // FIXME unsupported

    _standardSymbols[_makeKey("standard", "active")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_active", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "last_event")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_last_event", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "last_active")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_last_active", SIMPLIFIED); // FIXME unsupported

    _standardSymbols[_makeKey("standard", "driving")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_driving", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "driving_value")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_driving_value", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "simple_name")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_simple_name", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "instance_name")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_instance_name", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "path_name")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_path_name", SIMPLIFIED); // FIXME unsupported
    _standardSymbols[_makeKey("standard", "foreign")] =
        _makeValue(hif_vhdl_standard, 1, "hif_vhdl_foreign", SIMPLIFIED); // FIXME unsupported

    // /////////////////////////////////////////////////////////////////////////
    // vhdl_textio
    // /////////////////////////////////////////////////////////////////////////

    // TODO

    // /////////////////////////////////////////////////////////////////////////
    // verilog_standard
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("standard", "iterated_concat")] =
        _makeValue(hif_verilog_standard, 1, "hif_verilog_iterated_concat", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "_system_fclose")] =
        _makeValue(hif_verilog_standard, 1, "hif_verilog__system_fclose", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "_system_fflush")] =
        _makeValue(hif_verilog_standard, 1, "hif_verilog__system_fflush", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "_system_fopen")] =
        _makeValue(hif_verilog_standard, 1, "hif_verilog__system_fopen", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "_system_random")] =
        _makeValue(hif_verilog_standard, 1, "hif_verilog__system_random", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "_system_readmemb")] =
        _makeValue(hif_verilog_standard, 1, "hif_verilog__system_readmemb", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "_system_readmemh")] =
        _makeValue(hif_verilog_standard, 1, "hif_verilog__system_readmemh", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "_system_feof")] =
        _makeValue(hif_verilog_standard, 1, "hif_verilog__system_feof", SIMPLIFIED);
    _standardSymbols[_makeKey("standard", "_system_fscanf")] =
        _makeValue(hif_verilog_standard, 1, "hif_verilog__system_fscanf", SIMPLIFIED);

    // /////////////////////////////////////////////////////////////////////////
    // verilog_ams_standard
    // /////////////////////////////////////////////////////////////////////////

    _standardSymbols[_makeKey("vams_standard", "cross")] =
        _makeValue(hif_verilog_vams_standard, 1, "hif_verilog_cross", SIMPLIFIED);
    _standardSymbols[_makeKey("vams_standard", "_system_driver_count")] =
        _makeValue(hif_verilog_vams_standard, 1, "hif_verilog__system_driver_count", SIMPLIFIED);
    _standardSymbols[_makeKey("vams_standard", "_system_driver_state")] =
        _makeValue(hif_verilog_vams_standard, 1, "hif_verilog__system_driver_state", SIMPLIFIED);
    _standardSymbols[_makeKey("vams_standard", "_system_driver_strength")] =
        _makeValue(hif_verilog_vams_standard, 1, "hif_verilog__system_driver_strength", SIMPLIFIED);
    _standardSymbols[_makeKey("vams_standard", "_system_driver_next_state")] =
        _makeValue(hif_verilog_vams_standard, 1, "hif_verilog__system_driver_next_state", SIMPLIFIED);
    _standardSymbols[_makeKey("vams_standard", "_system_driver_next_strength")] =
        _makeValue(hif_verilog_vams_standard, 1, "hif_verilog__system_driver_next_strength", SIMPLIFIED);
    _standardSymbols[_makeKey("vams_standard", "_system_driver_type")] =
        _makeValue(hif_verilog_vams_standard, 1, "hif_verilog__system_driver_type", SIMPLIFIED);
}

} // namespace semantics
} // namespace hif
