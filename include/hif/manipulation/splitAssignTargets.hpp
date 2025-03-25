/// @file splitAssignTargets.hpp
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

/// @brief options for splitAssignTargets().
struct SplitAssignTargetOptions {
    SplitAssignTargetOptions();
    ~SplitAssignTargetOptions();
    SplitAssignTargetOptions(const SplitAssignTargetOptions &other);
    SplitAssignTargetOptions &operator=(SplitAssignTargetOptions other);
    void swap(SplitAssignTargetOptions &other);

    /// @name General configuration.
    /// @{

    /// @brief Skip standard scopes. Default is true.
    bool skip_standard_declarations;
    /// @brief Split concat targets. Default is false.
    bool splitConcats;
    /// @brief Split target vectors, whose size is greater than given trashold. Default is false.
    bool splitVectors;
    /// @brief Split target integers, whose size is greater than given trashold. Default is false.
    bool splitIntegers;
    /// @brief Split target arrays. Default is false.
    bool splitArrays;
    /// @brief Enable fix for array of ports and signals used as parameters. Default is false.
    bool removeSignalPortArrayParameters;

    /// @}

    /// @name Special configuration options for concats.
    /// @{

    /// @brief Split also targets which are record values involved in concats. Default is false.
    bool splitRecordValueConcats;
    /// @brief When splitting concats, create support signal instead of variables. Default is false.
    bool createSignals;
    /// @brief Split also port bindings with concats. Default is false.
    bool splitPortassigns;

    /// @}

    /// @name Special configuration options for assign types.
    /// @{

    /// @brief Assign targets of given types are splitted in chunks of this maximum bitwidth. Default is 64.
    unsigned long long maxBitwidth;
    /// @brief Sets a custom maximum unrolling factor, in order to avoid HIF code explosion.
    /// Zero means force unrolling.
    unsigned long long unrollingUpperBound;

    /// @}
};

/// @brief Splits assignments having concatenations as their left-hand side into multiple
/// corresponding assignments (one for each concatenated item).
/// @param root The object.
/// @param sem The semantics.
/// @param opt The options
/// @return True when at least one split has been performed.

bool splitAssignTargets(
    Object *root,
    hif::semantics::ILanguageSemantics *sem,
    const SplitAssignTargetOptions &opt = SplitAssignTargetOptions());

} // namespace manipulation
} // namespace hif
