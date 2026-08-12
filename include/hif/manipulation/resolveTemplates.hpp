/// @file resolveTemplates.hpp
/// @brief Provides functionality to resolve template declarations.
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

/// @brief Options for resolving template declarations.
struct ResolveTempalteOptions {
    View *top;                          ///< Pointer to the top-level HIF view.
    bool removeUnreferenced;            ///< Remove unreferenced template declarations. Default: false.
    bool constvalueBounds;              ///< Enforce constant values as span bounds. Default: false.
    bool allowLhsSlices;                ///< Allow non-constant slices in LHS of assignments. Default: false.
    bool allowRhsSlices;                ///< Allow non-constant slices in RHS of assignments. Default: false.
    bool removeInstantiatedViews;       ///< Remove instantiated views. Default: false.
    bool removeInstantiatedSubPrograms; ///< Remove instantiated subprograms. Default: false.
    bool removeInstantiatedTypeDefs;    ///< Remove instantiated typedefs. Default: false.
    bool forceNonCTCInstantiation; ///< Force instantiation of views with non-compile-time-constants. Default: false.

    /// @brief Default constructor initializing options.
    ResolveTempalteOptions();

    /// @brief Destructor.
    ~ResolveTempalteOptions();

    /// @brief Copy constructor.
    /// @param other Options to copy from.
    ResolveTempalteOptions(const ResolveTempalteOptions &other);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param other Options to assign from.
    /// @return Reference to the assigned options.
    ResolveTempalteOptions &operator=(ResolveTempalteOptions other);

    /// @brief Swaps the contents of two ResolveTempalteOptions objects.
    /// @param other Options to swap with.
    void swap(ResolveTempalteOptions &other);
};

/// @brief Resolves template declarations in the HIF system.
/// @details This function analyzes the system object, resolving template declarations
/// and handling unused or instantiated templates based on the provided options.
/// @param system Pointer to the HIF System object.
/// @param sem Pointer to the reference semantics interface (default: HIFSemantics instance).
/// @param opt Options for resolving templates (default: ResolveTempalteOptions()).
/// @return True if at least one fix was applied, false otherwise.

bool resolveTemplates(
    hif::System *system,
    hif::semantics::ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance(),
    const ResolveTempalteOptions &opt       = ResolveTempalteOptions());

} // namespace manipulation
} // namespace hif
