/// @file rangeGetShiftedToZero.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/rangeGetShiftedToZero.hpp"

#include "hif/application_utils/application_utils.hpp"
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

Range *rangeGetShiftedToZero(Range *range, hif::semantics::ILanguageSemantics *refLang)
{
    if (range->getLeftBound() == nullptr || range->getRightBound() == nullptr) {
        Range *ret = new Range();
        ret->setDirection(range->getDirection());
        return ret;
    }

    hif::HifFactory f;
    f.setSemantics(refLang);
    Value *span = hif::semantics::spanGetSize(range, refLang);
    if (range->getDirection() == dir_upto) {
        return new Range(new IntValue(0), f.expression(span, op_minus, f.intval(1)), dir_upto);
    } else if (range->getDirection() == dir_downto) {
        return new Range(f.expression(span, op_minus, f.intval(1)), new IntValue(0), dir_downto);
    } else {
        messageError("Unexpected range direction", range, refLang);
    }
}

} // namespace manipulation
} // namespace hif
