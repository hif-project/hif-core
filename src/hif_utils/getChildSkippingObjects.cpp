/// @file getChildSkippingObjects.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/getChildSkippingObjects.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

Value *getChildSkippingCasts(Value *o)
{
    Cast *c = dynamic_cast<Cast *>(o);

    if (c == nullptr)
        return o;
    Value *v = c->getValue();

    while (v != nullptr && (dynamic_cast<Cast *>(v) != nullptr)) {
        c = static_cast<Cast *>(v);
        v = c->getValue();
    }

    return v;
}

} // namespace hif
