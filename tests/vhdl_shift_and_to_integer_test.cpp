/// @file vhdl_shift_and_to_integer_test.cpp
/// @brief
/// Copyright (c) 2026, Electronic Systems Design (ESD) Group,
/// University of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.
///
/// Regression (hif-backend#94): a Verilog shift by a vector amount regenerated
/// as invalid VHDL, `to_integer(to_unsigned(a)) sll to_integer(to_unsigned(n))`,
/// in three independent ways.
///
///   1. The cast of a std_logic_vector to an integer built
///      `to_integer(to_unsigned(x))`. to_unsigned converts an integer *to* an
///      unsigned and needs a size; the vector has to be reinterpreted instead,
///      `to_integer(unsigned(x))`.
///   2. The shifted operand was suggested the type of the shift *count*, an
///      integer, so `sll` ended up between two integers. The vector keeps its
///      own width and becomes unsigned (or signed).
///   3. VHDLAnalysis accepted a shift whose count is a signed/unsigned vector.
///      `sll` takes an integer, and accepting it let the count's integer cast be
///      simplified away, leaving `unsigned(a) sll n`.
///
/// This checks each at the semantics level, where it lives. The whole excursion
/// is hif-regression's `combinational/v2vhdl_shift_amount`.

#include <hif/hif.hpp>
#include <iostream>

using namespace hif;
using namespace hif::semantics;

namespace
{

auto makeVector(std::int64_t msb, bool isSigned) -> Bitvector *
{
    auto *t = new Bitvector();
    t->setSpan(new Range(msb, 0));
    t->setSigned(isSigned);
    t->setLogic(true);
    t->setResolved(true);
    return t;
}

auto makeUnsigned(std::int64_t msb) -> Unsigned *
{
    auto *t = new Unsigned();
    t->setSpan(new Range(msb, 0));
    return t;
}

auto makeInt() -> Int *
{
    auto *t = new Int();
    t->setSpan(new Range(31, 0));
    t->setSigned(true);
    return t;
}

auto expect(bool condition, const std::string &message) -> bool
{
    if (!condition) {
        std::cerr << "FAIL: " << message << "\n";
    }
    return condition;
}

/// 1. std_logic_vector -> integer is `to_integer(unsigned(x))`.
auto checkVectorToIntegerCast() -> bool
{
    auto *sem = VHDLSemantics::getInstance();
    // explicitCast adds the libraries it needs to the nearest scope, so the value
    // has to sit in a design.
    auto *system = new System();
    auto *unit   = new DesignUnit();
    unit->setName("test_unit");
    auto *view = new View();
    view->setName("behav");
    view->setContents(new Contents());
    unit->views.push_back(view);
    system->designUnits.push_back(unit);
    auto *globalAction = new GlobalAction();
    view->getContents()->setGlobalAction(globalAction);
    auto *value = new BitvectorValue("00000000");
    value->setType(makeVector(7, false));
    auto *assign = new Assign();
    assign->setLeftHandSide(new Identifier("y"));
    assign->setRightHandSide(value);
    globalAction->actions.push_back(assign);

    Bitvector *vectorType = makeVector(7, false);
    Int *intType          = makeInt();

    Value *result = sem->explicitCast(value, intType, vectorType);
    bool ok       = expect(result != nullptr, "explicitCast(vector -> integer) returned nullptr");
    if (ok) {
        auto *call = dynamic_cast<FunctionCall *>(result);
        ok         = expect(call != nullptr, "the cast is not a function call");
        if (call != nullptr) {
            ok = expect(call->getName() == "to_integer", "the outer call is not to_integer: " + call->getName()) && ok;
            ok = expect(call->parameterAssigns.size() == 1, "to_integer should have one argument") && ok;
            if (call->parameterAssigns.size() == 1) {
                auto *argument = call->parameterAssigns.front()->getValue();
                auto *cast     = dynamic_cast<Cast *>(argument);
                ok             = expect(cast != nullptr, "the argument of to_integer is not a type conversion (a "
                                                         "call to to_unsigned is the defect)") &&
                     ok;
                if (cast != nullptr) {
                    ok = expect(
                             dynamic_cast<Unsigned *>(cast->getType()) != nullptr,
                             "the vector is not reinterpreted as unsigned") &&
                         ok;
                    auto *span = typeGetSpan(cast->getType(), sem);
                    ok         = expect(span != nullptr && spanGetBitwidth(span, sem) == 8, "the unsigned lost the vector's width") && ok;
                }
            }
        }
    }
    delete result;
    delete system;
    delete vectorType;
    delete intType;
    return ok;
}

/// 2. The shifted vector's suggestion is derived from the vector itself.
auto checkShiftedOperandSuggestion() -> bool
{
    auto *sem             = VHDLSemantics::getInstance();
    Bitvector *vectorType = makeVector(7, false);
    Int *countCast        = makeInt();
    Expression dummy;

    Type *first = sem->getSuggestedTypeForOp(countCast, op_sll, vectorType, &dummy, true);
    bool ok     = expect(first != nullptr, "no suggestion for the shifted operand");
    if (first != nullptr) {
        ok = expect(dynamic_cast<Unsigned *>(first) != nullptr, "the shifted vector is not suggested as unsigned (an "
                                                                 "integer here is the defect)") &&
             ok;
        auto *span = typeGetSpan(first, sem);
        ok         = expect(span != nullptr && spanGetBitwidth(span, sem) == 8, "the suggestion lost the vector's width") && ok;
    }

    Bitvector *signedVector = makeVector(3, true);
    Type *second            = sem->getSuggestedTypeForOp(countCast, op_sra, signedVector, &dummy, true);
    ok = expect(second != nullptr && dynamic_cast<Signed *>(second) != nullptr, "a signed vector is not suggested as signed") && ok;

    // A value that is already signed or unsigned keeps its type: an arithmetic
    // shift of a signed one must not become a shift of an unsigned one.
    Signed *alreadySigned = new Signed();
    alreadySigned->setSpan(new Range(7, 0));
    Type *third = sem->getSuggestedTypeForOp(countCast, op_sra, alreadySigned, &dummy, true);
    ok          = expect(third != nullptr && dynamic_cast<Signed *>(third) != nullptr,
                         "a signed operand is not kept signed for an arithmetic shift") &&
         ok;
    delete third;
    delete alreadySigned;

    delete first;
    delete second;
    delete vectorType;
    delete signedVector;
    delete countCast;
    return ok;
}

/// 3. VHDL shifts take an integer count, not a vector.
auto checkShiftCountMustBeInteger() -> bool
{
    auto *sem = VHDLSemantics::getInstance();
    Unsigned *shifted = makeUnsigned(7);
    Unsigned *count   = makeUnsigned(2);
    Int *integerCount = makeInt();
    Expression dummy;

    auto byVector = sem->getExprType(shifted, count, op_sll, &dummy);
    bool ok       = expect(byVector.returnedType == nullptr, "VHDLAnalysis accepts a shift by an unsigned vector");

    auto byInteger = sem->getExprType(shifted, integerCount, op_sll, &dummy);
    ok             = expect(byInteger.returnedType != nullptr, "VHDLAnalysis rejects a shift by an integer") && ok;

    delete shifted;
    delete count;
    delete integerCount;
    return ok;
}

} // namespace

auto main() -> int
{
    bool ok = true;
    ok      = checkVectorToIntegerCast() && ok;
    ok      = checkShiftedOperandSuggestion() && ok;
    ok      = checkShiftCountMustBeInteger() && ok;
    if (!ok) {
        return 1;
    }
    std::cout << "vhdl_shift_and_to_integer test passed.\n";
    return 0;
}
