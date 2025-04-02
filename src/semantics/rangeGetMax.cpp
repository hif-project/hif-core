/// @file rangeGetMax.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/rangeGetMax.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

Range *_rangeGetMinMax(Range *r1, Range *r2, ILanguageSemantics *sem, const bool getMax)
{
    messageAssert(r1 != nullptr && r2 != nullptr, "Unexpected parameters", nullptr, sem);

    // if the ranges are equal return a copy of one
    if (hif::equals(r1, r2))
        return hif::copy(r1);

    if (r1->getLeftBound() == nullptr || r1->getRightBound() == nullptr || r2->getLeftBound() == nullptr ||
        r2->getRightBound() == nullptr) {
        return nullptr;
    }

    std::uint64_t span1 = spanGetBitwidth(r1, sem);
    std::uint64_t span2 = spanGetBitwidth(r2, sem);

    if (span1 != 0 && span2 != 0) {
        if (getMax) {
            return (span1 > span2) ? hif::copy(r1) : hif::copy(r2);
        } else {
            return (span1 < span2) ? hif::copy(r1) : hif::copy(r2);
        }
    }

    // at least one range is an expression

    // calculating result direction
    RangeDirection dir1      = r1->getDirection();
    RangeDirection dir2      = r2->getDirection();
    RangeDirection dirResult = dir_downto;
    if (dir1 == dir2)
        dirResult = dir1;

    // getting spans
    Value *expr1 = spanGetSize(r1, sem);
    Value *expr2 = spanGetSize(r2, sem);

    // calculating the greater span
    hif::HifFactory f(sem);

    Operator oper    = (getMax) ? op_gt : op_lt;
    Expression *expr = f.expression(expr1, oper, expr2);
    When *w          = f.when(
        f.whenalt(expr, f.expression(hif::copy(expr1), op_minus, f.intval(1))),
        f.expression(hif::copy(expr2), op_minus, f.intval(1)));

    // creating the resulting range
    Range *resultRange = new Range();
    resultRange->setDirection(dirResult);
    if (dirResult == dir_upto) {
        resultRange->setLeftBound(new IntValue(0));
        resultRange->setRightBound(w);
    } else // dir_result == dir_downto
    {
        resultRange->setLeftBound(w);
        resultRange->setRightBound(new IntValue(0));
    }

    resultRange = hif::manipulation::simplify(resultRange, sem);
    messageDebugAssert(resultRange->getLeftBound() != nullptr, "Wrong left bound", resultRange, sem);
    messageDebugAssert(resultRange->getRightBound() != nullptr, "Wrong right bound", resultRange, sem);
    return resultRange;
}

} // namespace

Range *rangeGetMax(Range *a, Range *b, ILanguageSemantics *refLang) { return _rangeGetMinMax(a, b, refLang, true); }

Range *rangeGetMin(Range *a, Range *b, ILanguageSemantics *refLang) { return _rangeGetMinMax(a, b, refLang, false); }

} // namespace semantics
} // namespace hif
