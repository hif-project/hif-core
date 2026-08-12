/// @file propagateSymbols.hpp
/// @brief Provides utilities for propagating symbols in a hierarchical design.
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

/// @brief Options for controlling the propagation of symbols.
struct PropagationOptions {
    /// @name Traits
    /// @{

    using DeclarationTargets = std::list<Object *>;            ///< List of objects to propagate.
    using ObjectSet          = std::set<Object *>;             ///< Set of unique objects.
    using ViewSet            = std::set<hif::View *>;          ///< Set of unique views.
    using ViewMap            = std::map<hif::View *, ViewSet>; ///< Map tracking view dependencies.

    /// @}
    /// @name Configuration options
    /// @{

    bool allSignals;         ///< Propagate all signals of specified modules. Default: false.
    bool allPorts;           ///< Propagate all ports of specified modules. Default: false.
    bool allVariables;       ///< Propagate variables, creating support signals. Default: false.
    bool inputPorts;         ///< Propagate input and inout ports. Default: false.
    bool includeNotRtlTypes; ///< Propagate non-RTL types (e.g., File, Pointer). Default: false.
    bool skipArrays;         ///< Skip propagation of array symbols. Default: false.

    /// @}
    /// @name Data to be propagated
    /// @{

    DeclarationTargets declarationTargets; ///< List of declarations to propagate.

    /// @}
    /// @name Optional infos
    /// @{

    System *system; ///< Optional system reference.
    ViewMap *smm;   ///< Tracks view dependencies from parent to children.
    ViewMap *pmm;   ///< Tracks view dependencies from children to parents.

    /// @}

    /// @brief Default constructor initializing options.
    PropagationOptions();

    /// @brief Destructor.
    ~PropagationOptions();

    /// @brief Copy constructor.
    /// @param other Options to copy from.
    PropagationOptions(const PropagationOptions &other);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param other Options to assign from.
    /// @return Reference to the assigned options.
    PropagationOptions &operator=(const PropagationOptions &other);
};

/// @brief Propagates ports and signals of submodules to the top level.
/// @details Ensures proper visibility of symbols by propagating them based on
/// the specified options. Handles dependencies between views and detects errors
/// during propagation.
/// @param options Configuration options for the propagation.
/// @param sem Pointer to the language semantics interface (default: HIFSemantics instance).
/// @return True if propagation succeeds, false if an error occurs.

bool propagateSymbols(
    PropagationOptions &options,
    hif::semantics::ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance());

} // namespace manipulation
} // namespace hif
