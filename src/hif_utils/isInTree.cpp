/// @file isInTree.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/isInTree.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

bool isInTree(const Object *obj)
{
    if (obj) {
        auto current = obj;
        while (current->getParent()) {
            current = current->getParent();
        }
        return (dynamic_cast<const System *>(current) != nullptr);
    }
    return false;
}

} // namespace hif
