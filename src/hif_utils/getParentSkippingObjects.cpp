/// @file getParentSkippingObjects.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/getParentSkippingObjects.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

Object *getParentSkippingCasts(Value *o) { return getParentSkippingClass<Cast>(o); }

template <class T> Object *getParentSkippingClass(Object *o)
{
    if (o->getParent() == nullptr)
        return nullptr;

    Object *p = o->getParent();

    while (p != nullptr && (dynamic_cast<T *>(p) != nullptr)) {
        p = p->getParent();
    }

    return p;
}

/// @brief Helper macro to instantiate the `getParentSkippingClass` method template for all HIF object types.
/// @details This macro generates explicit template instantiations for the
/// `Object::getParentSkippingClass` method template. It ensures that all necessary
/// specializations are available for linking across all HIF object types.
/// @param T The type of the object whose parent is to be retrieved.
#define HIF_TEMPLATE_METHOD(T) Object *getParentSkippingClass<T>(Object *)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

} // namespace hif
