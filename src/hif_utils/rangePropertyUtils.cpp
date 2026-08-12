/// @file rangePropertyUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/rangePropertyUtils.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

Value *rangeGetMaxBound(Range *r)
{
    if (r->getDirection() == dir_downto)
        return r->getLeftBound();
    else if (r->getDirection() == dir_upto)
        return r->getRightBound();

    return nullptr;
}

Value *rangeGetMinBound(Range *r)
{
    if (r->getDirection() == dir_downto)
        return r->getRightBound();
    else if (r->getDirection() == dir_upto)
        return r->getLeftBound();

    return nullptr;
}

Value *rangeSetMaxBound(Range *r, Value *v)
{
    if (r->getDirection() == dir_downto)
        return r->setLeftBound(v);
    else if (r->getDirection() == dir_upto)
        return r->setRightBound(v);

    return nullptr;
}

Value *rangeSetMinBound(Range *r, Value *v)
{
    if (r->getDirection() == dir_upto)
        return r->setLeftBound(v);
    else if (r->getDirection() == dir_downto)
        return r->setRightBound(v);

    return nullptr;
}

bool rangeIsValue(Range *r)
{
    Alt *alt = dynamic_cast<Alt *>(r->getParent());
    return (alt != nullptr);
}

} // namespace hif
