/// @file fixAssignments.hpp
/// @brief Provides functionality for fixing assignment-related issues in the tree structure.
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

/// @brief Options for fixing assignment-related issues.
struct FixAssignmentOptions {
    bool fixAssigns;               ///< Fix Assign nodes by adding casts if needed. Default: false.
    bool fixParameterAssigns;      ///< Fix ParameterAssign values within parameter types. Default: false.
    bool fixDataDeclarationsValue; ///< Fix initial values in data declarations to match their types. Default: false.

    /// @brief Default constructor initializing options to false.
    FixAssignmentOptions();

    /// @brief Destructor.
    ~FixAssignmentOptions();

    /// @brief Copy constructor.
    /// @param o Options to copy from.
    FixAssignmentOptions(const FixAssignmentOptions &o);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param o Options to assign from.
    /// @return Reference to the assigned options.
    FixAssignmentOptions &operator=(FixAssignmentOptions o);

    /// @brief Swaps the contents of two FixAssignmentOptions objects.
    /// @param o Options to swap with.
    void swap(FixAssignmentOptions &o);

    /// @brief Checks if any fixes are enabled.
    /// @return True if any fix option is enabled, false otherwise.
    bool hasSomethingToFix() const;
};

/// @brief Fixes the tree by adding casts or correcting assignments as per the options.
/// @param root Pointer to the root of the tree.
/// @param sem Pointer to the language semantics interface.
/// @param opt Fix options to apply (default: FixAssignmentOptions()).
/// @return True if any fixes were applied, false otherwise.

bool fixAssignments(
    Object *root,
    hif::semantics::ILanguageSemantics *sem,
    const FixAssignmentOptions &opt = FixAssignmentOptions());

} // namespace manipulation
} // namespace hif
