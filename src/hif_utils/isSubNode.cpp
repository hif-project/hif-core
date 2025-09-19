/// @file isSubNode.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/isSubNode.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

bool isSubNode(Object *obj, Object *parent, bool matchStarting)
{
    if (parent == nullptr)
        return false;
    if (obj == parent)
        return matchStarting;

    Object *current = obj;
    while (current != nullptr && current != parent) {
        current = current->getParent();
    }

    return (current == parent);
}
bool isSubNode(Object *obj, BList<Object> &parentList, bool matchStarting)
{
    for (BList<Object>::iterator i = parentList.begin(); i != parentList.end(); ++i) {
        if (isSubNode(obj, *i, matchStarting))
            return true;
    }

    return false;
}

template <typename T> bool isSubNode(Object *obj, BList<T> &parentList, bool matchStarting)
{
    BList<Object> *tmp = reinterpret_cast<BList<Object> *>(&parentList);
    return isSubNode(obj, *tmp, matchStarting);
}

/// @brief Helper macro to instantiate the `isSubNode` method template for all HIF object types.
/// @details This macro generates explicit template instantiations for the
/// `Object::isSubNode` method template. It ensures all necessary specializations
/// are available for linking across all HIF object types.
/// @param T The type of the objects being checked as subnodes.
#define HIF_TEMPLATE_METHOD(T) bool isSubNode<T>(Object *, BList<T> &, bool)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

} // namespace hif
