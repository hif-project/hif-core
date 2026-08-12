/// @file verilog_clog2_registration_test.cpp
/// @brief
/// Copyright (c) 2026, Electronic Systems Design (ESD) Group,
/// University of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.
///
/// Regression for a real gap: $clog2 is a documented, standard IEEE 1364
/// math function, but VerilogSemantics::getStandardPackage() only ever had
/// a `//$clog2` comment marking where it belonged - never actually
/// registered, unlike $time/$stime/$random (same file, same section shape)
/// which are. Any expression using $clog2 (typically in a port range or
/// localparam, e.g. `input [$clog2(N)-1:0] sel`) crashes
/// FixDescription_1::_fixParameterNames in hif-frontend with "Unable to
/// get call declaration", because after the $ -> _system rename there is
/// nothing to find.
///
/// This test targets the registration directly (not the full hif-frontend
/// parse pipeline, which lives in a different repo): $clog2 becomes
/// `_system_clog2` after FixDescription_1's system-call rename, so a
/// correctly-registered SubProgram by that name, taking exactly one
/// parameter, must exist in Verilog's standard library.

#include <hif/hif.hpp>
#include <iostream>

using namespace hif;
using namespace hif::semantics;

auto main() -> int
{
    LibraryDef *standard = VerilogSemantics::getInstance()->getStandardLibrary("standard");
    if (standard == nullptr) {
        std::cerr << "FAIL: VerilogSemantics standard library not found\n";
        return 1;
    }

    SubProgram *clog2 = nullptr;
    for (BList<Declaration>::iterator it(standard->declarations.begin()); it != standard->declarations.end(); ++it) {
        auto *sub = dynamic_cast<SubProgram *>(*it);
        if (sub != nullptr && sub->getName() == "_system_clog2") {
            clog2 = sub;
            break;
        }
    }

    if (clog2 == nullptr) {
        std::cerr << "FAIL: _system_clog2 is not registered in Verilog's standard library\n";
        return 1;
    }

    if (clog2->parameters.size() != 1) {
        std::cerr << "FAIL: expected _system_clog2 to take exactly 1 parameter, got " << clog2->parameters.size()
                   << "\n";
        return 1;
    }

    // getStandardLibrary("standard") builds its declarations with
    // hifFormat=false, where the shared `integerBV` helper type used by the
    // sibling $time/$stime/$random registrations is a 32-bit Bitvector, not
    // an Int - match that existing convention for consistency.
    auto *clog2Function = dynamic_cast<Function *>(clog2);
    if (clog2Function == nullptr) {
        std::cerr << "FAIL: expected _system_clog2 to be a Function (has a return type)\n";
        return 1;
    }
    if (dynamic_cast<Bitvector *>(clog2Function->getType()) == nullptr) {
        std::cerr << "FAIL: expected _system_clog2 to return a Bitvector, matching $time/$stime/$random\n";
        return 1;
    }

    std::cout << "PASS\n";
    return 0;
}
