/// @file matchTrees.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Collects matches between the reference tree and the matched tree.
/// @details Visits the entire reference tree, identifying matches with the matched tree.
/// Children of unmatched objects are excluded from the collection.
/// @param referenceTree The root of the reference tree to be visited.
/// @param matchedTree The root of the tree to match against the reference tree.
/// @param matched A map storing matched objects, associating reference objects to their matched counterparts.
/// @param unmatched A map storing unmatched objects, associating reference objects to null or unmatched counterparts.

void matchTrees(
    Object *referenceTree,
    Object *matchedTree,
    std::map<Object *, Object *> &matched,
    std::map<Object *, Object *> &unmatched);

} // namespace manipulation
} // namespace hif
