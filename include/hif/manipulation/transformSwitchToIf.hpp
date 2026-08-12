/// @file transformSwitchToIf.hpp
/// @brief Provides functionality to transform Switch objects into If objects
/// and With objects into When objects.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/semantics/HIFSemantics.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Options for transforming case-based constructs (e.g., Switch to If).
struct TransformCaseOptions {
    bool fixCondition;             ///< Move condition into a support variable. Default: false.
    bool fixSignalOrPortCondition; ///< Introduce support variable for signal/port conditions. Default: false.
    bool splitCases;               ///< Split cases with multiple choices into separate If alternatives. Default: false.
    bool simplify;                 ///< Simplify the resulting object. Default: true.
    Variable *createdVariable;     ///< Pointer to the created support condition variable, if any.
    Assign *createdAssign;         ///< Pointer to the created condition variable assignment, if any.

    /// @brief Default constructor initializing options.
    TransformCaseOptions();

    /// @brief Destructor.
    ~TransformCaseOptions();

    /// @brief Copy constructor.
    /// @param other Options to copy from.
    TransformCaseOptions(const TransformCaseOptions &other);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param other Options to assign from.
    /// @return Reference to the assigned options.
    TransformCaseOptions &operator=(TransformCaseOptions other);

    /// @brief Swaps the contents of two TransformCaseOptions objects.
    /// @param other Options to swap with.
    void swap(TransformCaseOptions &other);
};

/// @brief Converts a `Switch` object into a corresponding `If` object.
/// @details Ensures side effects on the switch condition are safely handled.
/// Requires the `Switch` object to be part of a HIF tree.
/// @param s The starting `Switch` object.
/// @param opt Options controlling the transformation.
/// @param sem Pointer to the language semantics interface (default: HIFSemantics instance).
/// @return The transformed `If` object.

If *transformSwitchToIf(
    Switch *s,
    const TransformCaseOptions &opt         = TransformCaseOptions(),
    hif::semantics::ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance());

/// @brief Converts a `With` object into a corresponding `When` object.
/// @details Ensures side effects on the with condition are safely handled.
/// Requires the `With` object to be part of a HIF tree.
/// @param w The starting `With` object.
/// @param opt Options controlling the transformation.
/// @param sem Pointer to the language semantics interface (default: HIFSemantics instance).
/// @return The transformed `When` object.

Value *transformWithToWhen(
    With *w,
    const TransformCaseOptions &opt         = TransformCaseOptions(),
    hif::semantics::ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance());

} // namespace manipulation
} // namespace hif
