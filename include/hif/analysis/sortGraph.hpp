/// @file sortGraph.hpp
/// @brief Provides utilities for sorting graphs of objects.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/analysis/analysisTypes.hpp"
#include "hif/classes/classes.hpp"

namespace hif
{

namespace analysis
{

/// @brief Sorts a graph of objects.
/// @details This function performs a sort on a graph of objects, optionally
/// starting from the leaves, and produces a sorted list of keys. A stable sort
/// can also be achieved by providing a prior sorting list.
///
/// @param graph The input graph, represented as a pair of maps for parent-to-child and child-to-parent relationships.
/// @param list The output list of sorted keys.
/// @param fromLeaves True if traversal should begin from the leaves; otherwise, starts from the roots.
/// @param stableList (Optional) A prior sorted list to ensure stable sorting.
///

void sortGraph(
    Types<Object, Object>::Graph &graph,
    Types<Object, Object>::List &list,
    bool fromLeaves,
    Types<Object, Object>::List *stableList = nullptr);

/// @brief Sorts a graph of objects.
/// @details This templated function performs a sort on a generic graph of
/// key-value pairs, optionally starting from the leaves. It supports stable
/// sorting by leveraging a prior sorted list.
///
/// @tparam KEY The type of keys in the graph.
/// @tparam VALUE The type of values in the graph.
/// @param graph The input graph, represented as a pair of maps for parent-to-child and child-to-parent relationships.
/// @param list The output list of sorted keys.
/// @param fromLeaves True if traversal should begin from the leaves; otherwise, starts from the roots.
/// @param stableList (Optional) A prior sorted list to ensure stable sorting.
template <class KEY, class VALUE>
void sortGraph(
    typename Types<KEY, VALUE>::Graph &graph,
    typename Types<KEY, VALUE>::List &list,
    bool fromLeaves,
    typename Types<KEY, VALUE>::List *stableList = nullptr)
{
    auto *g = reinterpret_cast<typename Types<Object, Object>::Graph *>(&graph);
    auto *l = reinterpret_cast<typename Types<Object, Object>::List *>(&list);
    auto *s = reinterpret_cast<typename Types<Object, Object>::List *>(stableList);
    sortGraph(*g, *l, fromLeaves, s);
}

} // namespace analysis

} // namespace hif
