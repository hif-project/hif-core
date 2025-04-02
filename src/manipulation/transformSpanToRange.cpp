/// @file transformSpanToRange.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cmath>

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/manipulation/transformSpanToRange.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

} // namespace

Range *transformSpanToRange(Range *r, hif::semantics::ILanguageSemantics *refLang, bool isSigned)
{
    messageDebugAssert(r != nullptr, "Unexpected nullptr range", nullptr, refLang);

    Range *range = new Range();
    range->setDirection(dir_upto);

    Value *spanDimension = hif::semantics::spanGetSize(r, refLang);
    if (dynamic_cast<IntValue *>(spanDimension) != nullptr) {
        IntValue *i = static_cast<IntValue *>(spanDimension);
        if (isSigned) {
            range->setRightBound(
                new IntValue(static_cast<std::int64_t>(pow(2, static_cast<double>(i->getValue()) - 1) - 1)));
            range->setLeftBound(new IntValue(static_cast<std::int64_t>(-pow(2, static_cast<double>(i->getValue()) - 1))));
        } else {
            range->setRightBound(new IntValue(static_cast<std::int64_t>(pow(2, static_cast<double>(i->getValue())) - 1)));
            range->setLeftBound(new IntValue(0));
        }
    } else {
        // Signed: range = [ 0, 2^(span-1) - 1 ]
        // Unsigned: range = [ 0, 2^(span-1) ]
        hif::HifFactory factory;
        factory.setSemantics(refLang);

        Value *exponent = nullptr;
        if (isSigned) {
            exponent = factory.expression(hif::copy(spanDimension), op_minus, factory.intval(1));
        } else {
            exponent = hif::copy(spanDimension);
        }

        range->setLeftBound(factory.intval(0));
        range->setRightBound(
            factory.expression(factory.expression(factory.intval(2), op_pow, exponent), op_minus, factory.intval(1)));
    }

    delete spanDimension;

    range = hif::manipulation::simplify(range, refLang);

    return range;
}

} // namespace manipulation
} // namespace hif
