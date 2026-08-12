/// @file rangeGetSum.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/rangeGetSum.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

} // namespace

Range *rangeGetSum(Range *r1, Range *r2, ILanguageSemantics *refLang)
{
    hif::HifFactory f;
    f.setSemantics(refLang);

    Range *ret = nullptr;

    // calculating result direction
    RangeDirection dir1       = r1->getDirection();
    RangeDirection dir2       = r2->getDirection();
    RangeDirection dir_result = dir_downto;
    if (dir1 == dir2)
        dir_result = dir1;

    if (r1->getLeftBound() == nullptr || r1->getRightBound() == nullptr || r2->getLeftBound() == nullptr ||
        r2->getRightBound() == nullptr) {
        Range *retInfo = new Range();
        retInfo->setDirection(dir_result);
        return retInfo;
    }

    std::uint64_t span1 = spanGetBitwidth(r1, refLang);
    std::uint64_t span2 = spanGetBitwidth(r2, refLang);

    if (span1 != 0 && span2 != 0) {
        if (dir_result == dir_upto) {
            ret = f.range(0, static_cast<std::int64_t>(span1 + span2 - 1));
        } else {
            ret = f.range(static_cast<std::int64_t>(span1 + span2 - 1), 0);
        }
        return ret;
    }

    // at least one range is an expression

    // getting spans
    Value *expr1 = spanGetSize(r1, refLang);
    Value *expr2 = spanGetSize(r2, refLang);

    Expression *e = f.expression(f.expression(expr1, op_plus, expr2), op_minus, f.intval(1));

    if (dir_result == dir_upto) {
        ret = new Range(new IntValue(0), e, dir_upto);
    } else {
        ret = new Range(e, new IntValue(0), dir_downto);
    }

    ret = hif::manipulation::simplify(ret, refLang);
    return ret;
}

} // namespace semantics
} // namespace hif
