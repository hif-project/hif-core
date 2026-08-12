/// @file getNearestCommonParent.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/getNearestCommonParent.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

Object *getNearestCommonParent(Object *obj1, Object *obj2)
{
    if (obj1 == nullptr || obj2 == nullptr)
        return nullptr;
    std::set<Object *> parent1Set;
    Object *current = obj1;
    while (current != nullptr) {
        parent1Set.insert(current);
        current = current->getParent();
    }

    current = obj2;
    while (current != nullptr) {
        if (parent1Set.find(current) != parent1Set.end()) {
            return current;
        }

        current = current->getParent();
    }

    return nullptr;
}

} // namespace hif
