/// @file matchPatternInTree.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/hif_utils/hif_utils.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Searches subtree(s) matching @p pattern inside tree and returns the list
/// of nodes which are roots of pattern occurrences in the tree.
/// Note: the pattern is matched by checking subtrees w.r.t. the given equals options @p opt.
///
/// @param pattern The pattern object to search.
/// @param tree The tree from which to start the search.
/// @param resulList List on which search results are stored.
/// @param opt The equals options used to perform the equality check.
///

void matchPatternInTree(Object *pattern, Object *tree, std::list<Object *> &resulList, const hif::EqualsOptions &opt);

} // namespace manipulation
} // namespace hif
