/// @file rangeGetIncremented.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/rangeGetIncremented.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

} // namespace

Range *rangeGetIncremented(Range *range, hif::semantics::ILanguageSemantics *refLang, std::int64_t increment)
{
    Range *resultRange  = nullptr;
    std::int64_t rangeSize = static_cast<std::int64_t>(hif::semantics::spanGetBitwidth(range, refLang));

    if (rangeSize > 0) {
        std::int64_t newSize = rangeSize - 1LL + increment;
        if (range->getDirection() == dir_downto)
            // the range is [ range_length - 1 + 1 ; 0 ]
            // simplified in [ range_length ; 0 ]
            resultRange = new Range(newSize, 0);
        else
            // it is the opposite
            resultRange = new Range(0, newSize);
    } else {
        // if a size is equal to zero means that the range bound is a
        // general expression.
        // check range bounds
        if (range->getLeftBound() == nullptr || range->getRightBound() == nullptr) {
            return nullptr;
        }

        // the range becomes [ upper + 1 ; lower ]
        // build the upper and the lower bound
        Expression *upperBoundResult = new Expression();
        Value *lowerBoundResult      = nullptr;
        hif::HifFactory f(refLang);
        if (range->getDirection() == dir_downto) {
            Value *bound   = hif::copy(range->getLeftBound());
            ConstValue *cv = dynamic_cast<ConstValue *>(bound);
            if (cv != nullptr)
                cv->setType(refLang->getTypeForConstant(cv));
            upperBoundResult->setValue1(bound);
            upperBoundResult->setValue2(f.intval(increment));
            upperBoundResult->setOperator(op_plus);
            lowerBoundResult = hif::copy(range->getRightBound());
            resultRange      = new Range(upperBoundResult, lowerBoundResult, dir_downto);
        } else {
            Value *bound   = hif::copy(range->getRightBound());
            ConstValue *cv = dynamic_cast<ConstValue *>(bound);
            if (cv != nullptr)
                cv->setType(refLang->getTypeForConstant(cv));
            upperBoundResult->setValue1(bound);
            upperBoundResult->setValue2(f.intval(increment));
            upperBoundResult->setOperator(op_plus);
            lowerBoundResult = hif::copy(range->getLeftBound());
            resultRange      = new Range(lowerBoundResult, upperBoundResult, dir_upto);
        }
    }

    resultRange = simplify(resultRange, refLang);
    return resultRange;
}

} // namespace manipulation
} // namespace hif
