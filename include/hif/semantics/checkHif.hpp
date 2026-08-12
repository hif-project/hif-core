/// @file checkHif.hpp
/// @brief Provides utilities to validate the correctness of HIF descriptions based on semantics.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{
namespace semantics
{

/// @brief Options to customize the HIF validation checks.
struct CheckOptions {
    bool checkAliases;               ///< Check for potential aliases, useful for invalid reads. Default: false.
    bool checkOnCopy;                ///< Perform checks on a copy of the tree. Default: false.
    bool checkSimplifiedTree;        ///< Check after simplifying the tree. Default: false.
    bool checkMatchOfSimplifiedTree; ///< Verify that the simplified and original trees match. Default: false.
    bool checkFlushingCaches;        ///< Perform checks after cache flushing. Default: false.
    bool checkStandardLibraryDefs;   ///< Include standard library definitions and views in the checks. Default: false.
    bool checkInstantiate;           ///< Validate instantiable objects. Default: false.
    bool checkSemanticTypeSymbols;   ///< Check for undeclared symbols in semantic types. Default: false.
    bool exitOnErrors;               ///< Stop checks after the first failure. Default: false.
    bool forceSingleView;            ///< Force a single view in the design unit regardless of semantics. Default: true.
    bool allowMultipleStates;        ///< Allow multiple states and transitions in state tables. Default: false.

    /// @brief Default constructor initializing options.
    CheckOptions();

    /// @brief Destructor.
    ~CheckOptions();

    /// @brief Copy constructor.
    /// @param o Options to copy from.
    CheckOptions(const CheckOptions &o);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param o Options to assign from.
    /// @return Reference to the assigned options.
    CheckOptions &operator=(const CheckOptions &o);
};

/// @brief Validates the HIF description for correctness based on HIF semantics.
/// @param o The node to start the validation from.
/// @param sem The semantics to use for the checks.
/// @param opt Optional configuration for the checks (default: CheckOptions()).
/// @return 0 if no errors are found, otherwise 1.

int checkHif(Object *o, ILanguageSemantics *sem, const CheckOptions &opt = CheckOptions());

/// @brief Validates the HIF description with stricter native semantics.
/// @details Native semantics disallow certain constructs (e.g., bitvectors, non-power-of-2 spans).
/// @param o The node to start the validation from.
/// @param sem The semantics to use for the checks.
/// @param opt Optional configuration for the checks (default: CheckOptions()).
/// @return 0 if no errors are found, otherwise 1.

int checkNativeHif(Object *o, ILanguageSemantics *sem, const CheckOptions &opt = CheckOptions());

} // namespace semantics
} // namespace hif
