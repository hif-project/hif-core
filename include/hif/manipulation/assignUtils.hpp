/// @file assignUtils.hpp
/// @brief Utilities for handling assignment targets in HIF objects.
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

/// @brief Options for identifying assignment targets.
struct LeftHandSideOptions {
    /// @brief Default constructor initializing options.
    LeftHandSideOptions();

    /// @brief Destructor.
    ~LeftHandSideOptions();

    /// @brief Copy constructor.
    /// @param o Options to copy from.
    LeftHandSideOptions(const LeftHandSideOptions &o);

    /// @brief Copy assignment operator.
    /// @param o Options to assign from.
    /// @return Reference to the assigned options.
    LeftHandSideOptions &operator=(const LeftHandSideOptions &o);

    hif::semantics::ILanguageSemantics *sem; ///< Semantics to use for checks.

    bool considerRecord;      ///< Include record accesses as assignment targets. Default: false.
    bool considerPortassigns; ///< Include port assignments as targets based on direction. Default: false.
};

/// @brief Checks if a given object is a target of an assignment.
/// @details Determines if the object appears on the left-hand side of an assignment.
/// @param obj The object to check.
/// @param opt Options controlling the check (default: LeftHandSideOptions()).
/// @return True if the object is a target of an assignment, false otherwise.

bool isInLeftHandSide(Object *obj, const LeftHandSideOptions &opt = LeftHandSideOptions());

/// @brief Retrieves the target of an assignment (left-hand side).
/// @param obj The object to analyze.
/// @return The target of the assignment.
HIF_DEPRECATED("Useless.") Value *getLeftHandSide(Value *obj);

} // namespace manipulation
} // namespace hif
