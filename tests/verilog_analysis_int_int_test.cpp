/// @file verilog_analysis_int_int_test.cpp
/// @brief
/// Copyright (c) 2026, Electronic Systems Design (ESD) Group,
/// University of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.
///
/// Regression for a real gap: VerilogAnalysis (src/semantics/VerilogSemantics.cpp)
/// has no map(Int*, Int*) overload at all, so any binary expression combining
/// two Int-typed operands under Verilog semantics silently fails to type
/// (getSemanticType returns nullptr). This surfaces for real in hif-backend's
/// unresolved_parameter regression, where hif-frontend's standard-library
/// `iterated_concat` subprogram computes its range as
/// `(size + 1) * times - 1 downto 0` - all Int-Int arithmetic.
///
/// Expected rule set, matching Verilog's own sibling map(Bitvector*, Bitvector*)
/// in the same file (used as the in-house reference, not VHDL's Int-Int map,
/// which hardcodes a fixed 32-bit result - correct for VHDL's fixed-width
/// `integer` type, wrong for Verilog's width-carrying Int usage here):
///   - Arithmetic/bitwise: result is Int, signed only if BOTH operands are
///     signed (i.e. unsigned if either is unsigned - IEEE 1364 5.5.1), span
///     is the max of the two operand spans.
///   - Relational operators: result is a 1-bit type, not Int.

#include <hif/hif.hpp>
#include <iostream>

using namespace hif;
using namespace hif::semantics;

namespace
{

auto makeIntValue(std::int64_t msb, bool isSigned) -> IntValue *
{
    auto *t = new Int();
    t->setSpan(new Range(msb, 0));
    t->setSigned(isSigned);
    auto *v = new IntValue(0);
    v->setType(t);
    return v;
}

auto checkIntResult(const std::string &label, Value *v1, Value *v2, Operator op, std::uint64_t expectedSpan, bool expectedSigned) -> bool
{
    Expression expr(op, v1, v2);
    Type *t = getSemanticType(&expr, VerilogSemantics::getInstance());
    expr.setValue1(nullptr);
    expr.setValue2(nullptr);

    if (t == nullptr) {
        std::cerr << "FAIL(" << label << "): getSemanticType returned nullptr\n";
        return false;
    }
    auto *intType = dynamic_cast<Int *>(t);
    if (intType == nullptr) {
        std::cerr << "FAIL(" << label << "): expected Int result type\n";
        return false;
    }
    const std::uint64_t actualSpan = spanGetBitwidth(intType->getSpan(), VerilogSemantics::getInstance());
    bool ok                        = true;
    if (actualSpan != expectedSpan) {
        std::cerr << "FAIL(" << label << "): expected span " << expectedSpan << ", got " << actualSpan << "\n";
        ok = false;
    }
    if (intType->isSigned() != expectedSigned) {
        std::cerr << "FAIL(" << label << "): expected signed=" << expectedSigned << ", got " << intType->isSigned()
                   << "\n";
        ok = false;
    }
    return ok;
}

auto checkRelationalResult(const std::string &label, Value *v1, Value *v2, Operator op) -> bool
{
    Expression expr(op, v1, v2);
    Type *t = getSemanticType(&expr, VerilogSemantics::getInstance());
    expr.setValue1(nullptr);
    expr.setValue2(nullptr);

    if (t == nullptr) {
        std::cerr << "FAIL(" << label << "): getSemanticType returned nullptr\n";
        return false;
    }
    if (dynamic_cast<Int *>(t) != nullptr) {
        std::cerr << "FAIL(" << label << "): expected a 1-bit result, got Int\n";
        return false;
    }
    return true;
}

} // namespace

auto main() -> int
{
    bool ok = true;

    // Unsigned(8) + Unsigned(4) -> Int, unsigned, span = max(8,4) = 8
    ok &= checkIntResult(
        "unsigned_plus_unsigned", makeIntValue(7, false), makeIntValue(3, false), op_plus, 8, false);

    // Signed(8) + Unsigned(4) -> Int, UNSIGNED (not both signed), span = 8
    ok &= checkIntResult(
        "signed_plus_unsigned", makeIntValue(7, true), makeIntValue(3, false), op_plus, 8, false);

    // Signed(8) + Signed(4) -> Int, signed, span = 8
    ok &= checkIntResult("signed_plus_signed", makeIntValue(7, true), makeIntValue(3, true), op_plus, 8, true);

    // Unsigned(4) * Unsigned(8) -> Int, unsigned, span = max(4,8) = 8
    ok &= checkIntResult(
        "unsigned_times_unsigned", makeIntValue(3, false), makeIntValue(7, false), op_mult, 8, false);

    // Relational: Unsigned(4) < Unsigned(8) -> 1-bit result, not Int
    ok &= checkRelationalResult("less_than", makeIntValue(3, false), makeIntValue(7, false), op_lt);

    if (ok) {
        std::cout << "PASS\n";
        return 0;
    }
    return 1;
}
