/// @file integer_generic_test.cpp
/// @brief
/// Copyright (c) 2026, Electronic Systems Design (ESD) Group,
/// University of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.
///
/// Regression (hif-backend#93): hif2vhdl wrote a Verilog `parameter W = 5` as
/// `W : signed(31 downto 0) := "000...101"`, and every port bound that used it
/// as `to_integer(W - "000...1")`. vhdl2hif accepts that file, hif2verilog then
/// aborts on it, so the round trip was not closed for any parameterized design.
///
/// verilog2hif types an untyped Verilog parameter as a 32-bit signed bit vector,
/// which is what a Verilog parameter is not: it is an integer. Standardizing
/// toward VHDL now retypes such a generic as an integer, together with the
/// constants it is combined with, so the bound reads `W - 1`.
///
/// fixtures/verilog_parameter.hif.xml is the HIF verilog2hif produces for
/// `module m #(parameter W = 5) (input [W-1:0] a, output [W-1:0] y)` with source
/// positions removed. The whole round trip is hif-regression's
/// `parameterized/v2vhdl_parameter_generic`.

#include <hif/hif.hpp>
#include <iostream>

using namespace hif;
using namespace hif::semantics;

namespace
{

auto expect(bool condition, const std::string &message) -> bool
{
    if (!condition) {
        std::cerr << "FAIL: " << message << "\n";
    }
    return condition;
}

} // namespace

auto main(int argc, char **argv) -> int
{
    if (argc < 2) {
        std::cerr << "usage: integer_generic_test <fixture.hif.xml>\n";
        return 2;
    }
    Object *read = hif::readFile(argv[1]);
    auto *system = dynamic_cast<System *>(read);
    if (system == nullptr) {
        std::cerr << "FAIL: could not read fixture: " << argv[1] << "\n";
        return 1;
    }

    standardizeDescription(system, HIFSemantics::getInstance(), VHDLSemantics::getInstance());

    // The design's own generic; the standard libraries added by standardization
    // carry template parameters of their own.
    hif::HifTypedQuery<ValueTP> query;
    std::list<ValueTP *> allGenerics;
    hif::search(allGenerics, system, query);
    std::list<ValueTP *> generics;
    for (auto *candidate : allGenerics) {
        if (hif::getNearestParent<DesignUnit>(candidate) != nullptr &&
            hif::getNearestParent<DesignUnit>(candidate)->getName() == "v2vhdl_parameter_generic" &&
            candidate->getName() == "W") {
            generics.push_back(candidate);
        }
    }

    bool ok = expect(generics.size() == 1, "expected exactly one generic W in the design unit");
    if (generics.size() == 1) {
        ValueTP *generic = generics.front();
        ok = expect(dynamic_cast<Int *>(generic->getType()) != nullptr, "the generic is not an integer (a signed "
                                                                         "bit vector is the defect)") &&
             ok;
        ok = expect(dynamic_cast<IntValue *>(generic->getValue()) != nullptr, "the generic's default is not an integer literal") && ok;
        auto *value = dynamic_cast<IntValue *>(generic->getValue());
        ok = expect(value != nullptr && value->getValue() == 5, "the generic's default is not 5") && ok;
    }

    // No bit-string literal may be left in the arithmetic on the generic: that
    // is `W - "000...1"`, which VHDL cannot type.
    hif::HifTypedQuery<Expression> expressionQuery;
    std::list<Expression *> expressions;
    hif::search(expressions, system, expressionQuery);
    for (auto *expression : expressions) {
        if (!hif::operatorIsArithmetic(expression->getOperator())) {
            continue;
        }
        ok = expect(
                 dynamic_cast<BitvectorValue *>(expression->getValue1()) == nullptr &&
                     dynamic_cast<BitvectorValue *>(expression->getValue2()) == nullptr,
                 "arithmetic on the generic still has a bit-string literal operand") &&
             ok;
    }

    delete system;
    if (!ok) {
        return 1;
    }
    std::cout << "integer_generic test passed.\n";
    return 0;
}
