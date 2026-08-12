/// @file removeStandardMethods.hpp
/// @brief Provides functionality to remove or replace standard method calls.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/hif.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Options for removing or handling standard methods.
struct RemoveStandardMethodsOptions {
    bool allowStdLib;           ///< Allow use of C/C++ standard library methods. Default: true.
    bool allowSystemC;          ///< Allow use of SystemC methods. Default: true.
    bool allowSupportLibrary;   ///< Allow use of support library methods. Default: true.
    bool useBinaryLogic;        ///< Assume binary logic (2-value) instead of multi-value logic. Default: false.
    bool useSystemCEvent;       ///< Allow calls to SystemC `event()` method. Default: true.
    bool preferScMethodFlavour; ///< Use SystemC `pf_method` process flavour for events. Default: false (uses `pf_hdl`).
    bool allowSystemCAMS;       ///< Allow SystemC AMS-related methods. Default: false.
    bool printBitvectorsAsIntegers; ///< Print bitvectors as integers in display functions. Default: false.

    /// @brief Default constructor initializing options.
    RemoveStandardMethodsOptions();

    /// @brief Destructor.
    ~RemoveStandardMethodsOptions();

    /// @brief Copy constructor.
    /// @param other Options to copy from.
    RemoveStandardMethodsOptions(const RemoveStandardMethodsOptions &other);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param other Options to assign from.
    /// @return Reference to the assigned options.
    RemoveStandardMethodsOptions &operator=(RemoveStandardMethodsOptions other);

    /// @brief Swaps the contents of two RemoveStandardMethodsOptions objects.
    /// @param other Options to swap with.
    void swap(RemoveStandardMethodsOptions &other);
};

/// @brief Removes calls to standard methods from the HIF object tree.
/// @details Eliminates or replaces standard methods such as `event()`, `rising_edge`,
/// and `to_x01` based on the provided options. Ensures compatibility with custom semantics
/// and binary logic if specified.
/// @param root The starting object for the operation.
/// @param sem Pointer to the language semantics interface (default: HIFSemantics instance).
/// @param opt Options for removing standard methods (default: RemoveStandardMethodsOptions()).
/// @return True if at least one fix has been applied, false otherwise.

bool removeStandardMethods(
    Object *root,
    hif::semantics::ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance(),
    const RemoveStandardMethodsOptions &opt = RemoveStandardMethodsOptions());

} // namespace manipulation
} // namespace hif
