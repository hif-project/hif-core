/// @file VHDLSemantics_init.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/VHDLSemantics.hpp"
namespace hif
{
namespace semantics
{
void VHDLSemantics::_initForbiddenNames()
{
    // Forbidden tab (i.e. reserved words) are cabled
    _forbiddenNames.insert("process");
}
void VHDLSemantics::_initStandardFilenames() {}
void VHDLSemantics::_initStandardSymbols()
{
    // TODO map the following libraries
    // * VHDL standard, ieee__std_logic_1164, ieee__std_logic_arith
    // * SC sc_core, tlm, hdtlib

    const char *hif_verilog_standard[] = {"hif_verilog_standard"};

    _standardSymbols[_makeKey("hif_verilog_standard", "hif_verilog_standard")] =
        _makeValue(hif_verilog_standard, 1, "hif_verilog_standard", MAP_KEEP);
}
} // namespace semantics
} // namespace hif
