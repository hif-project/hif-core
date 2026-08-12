/// @file comparePrecision.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>

#include "hif/hif.hpp"
#include "hif/semantics/standardization.hpp"

//#define DEBUG_MSG

namespace hif
{
namespace semantics
{

namespace
{ // unnamed

bool _getBooleanValue(Value *v)
{
    BoolValue *boolVal = dynamic_cast<BoolValue *>(v);
    BitValue *bitVal   = dynamic_cast<BitValue *>(v);

    if (boolVal == nullptr && bitVal == nullptr)
        return false;

    if (boolVal != nullptr)
        return boolVal->getValue();

    if (bitVal->getValue() == bit_h || bitVal->getValue() == bit_one)
        return true;

    return false;
}
} // End unnamed namespace.
precision_type_t comparePrecision(Type *t1, Type *t2, hif::semantics::ILanguageSemantics *sem)
{
    //if types are equal unless range, check the span size
    if (t1 == nullptr || t2 == nullptr)
        return UNCOMPARABLE_PRECISION;
    Range *span1 = hif::typeGetSpan(t1, sem);
    Range *span2 = hif::typeGetSpan(t2, sem);
    if (span1 == nullptr && span2 == nullptr) {
        // E.g. Time vs Time
        return EQUAL_PRECISION;
    } else if (span1 == nullptr || span2 == nullptr) {
        return UNCOMPARABLE_PRECISION;
    }

    Value *spanSize1 = hif::semantics::spanGetSize(span1, sem);
    Value *spanSize2 = hif::semantics::spanGetSize(span2, sem);
    if (hif::equals(spanSize1, spanSize2)) {
        delete spanSize1;
        delete spanSize2;
        return EQUAL_PRECISION;
    }

    hif::HifFactory fact(sem);

    Expression *exp = fact.expression(spanSize1, op_minus, spanSize2);

    // Forcing considering sign since asking for greater than zero must
    // not return always true.
    // E.g.: 32 - 64 >= 0  -->  returns true since constvalue 64 and 32 are unsigned.
    Type *exprType = hif::copy(hif::semantics::getSemanticType(exp, sem));
    messageAssert(exprType != nullptr, "Cannot type expression", exp, sem);
    hif::typeSetSigned(exprType, true, sem);
    Cast *signedExpr = fact.cast(exprType, exp);

    Value *prec = hif::manipulation::simplify(signedExpr, sem);

    Expression *less    = fact.expression(prec, op_le, fact.intval(0));
    Expression *greater = fact.expression(hif::copy(prec), op_ge, fact.intval(0));

    Value *lessPrec    = hif::manipulation::simplify(less, sem);
    Value *greaterPrec = hif::manipulation::simplify(greater, sem);

    bool lessBool    = _getBooleanValue(lessPrec);
    bool greaterBool = _getBooleanValue(greaterPrec);

    delete lessPrec;
    delete greaterPrec;

    if (!lessBool && !greaterBool) {
        return UNCOMPARABLE_PRECISION;
    } else if (lessBool && greaterBool) {
        return EQUAL_PRECISION;
    } else if (lessBool) {
        return LESS_PRECISION;
    } else // greaterBool
    {
        return GREATER_PRECISION;
    }
}

} // namespace semantics
} // namespace hif
