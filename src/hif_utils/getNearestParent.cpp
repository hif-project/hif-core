/// @file getNearestParent.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/getNearestParent.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

template <typename T> T *getNearestParent(Object *object, bool matchStarting)
{
    if (object == nullptr) {
        assert(false);
        return nullptr;
    }

    auto current = object;

    if (!matchStarting) {
        current = object->getParent();
    }

    while (current) {
        if (dynamic_cast<T *>(current))
            break;
        current = current->getParent();
    }

    return static_cast<T *>(current);
}

/// @brief Defines a template method for retrieving the nearest parent object of a specific type.
#define HIF_TEMPLATE_METHOD(T) T *getNearestParent<T>(Object *, bool)

HIF_INSTANTIATE_METHOD()

#undef HIF_TEMPLATE_METHOD

} // namespace hif
