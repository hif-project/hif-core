/// @file mergeTrees.hpp
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
/// @brief Options for mergeTrees().
struct MergeTreesOptions {
    /// @brief Identify if print addictional infos. Default is false.
    bool printInfos;

    /// @brief Identify if is a description from ipxact. Default is false.
    bool isIpxact;

    /// @brief Forces to merge compultational branches.
    bool mergeBranches;
};

/// @brief This method takes a list of trees and compose them to form a single
/// tree merging the most of information from all of them.
/// Passed trees are deleted by the method.
/// @param partialTrees The list of trees to merge.
/// @param sem The reference semantics.
/// @param opt The options.
/// @return The merged tree

Object *mergeTrees(
    std::list<Object *> &partialTrees,
    semantics::ILanguageSemantics *sem,
    const MergeTreesOptions &opt = MergeTreesOptions());

} // namespace manipulation
} // namespace hif
