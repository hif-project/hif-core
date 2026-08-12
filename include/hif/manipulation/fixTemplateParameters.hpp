/// @file fixTemplateParameters.hpp
/// @brief Handles adjustments and fixes for template parameters.
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

/// @brief Options for fixing template parameter-related issues.
struct FixTemplateOptions {
    /// @brief Default constructor initializing options to default values.
    FixTemplateOptions();

    /// @brief Destructor.
    ~FixTemplateOptions();

    /// @brief Copy constructor.
    /// @param o Options to copy from.
    FixTemplateOptions(const FixTemplateOptions &o);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param o Options to assign from.
    /// @return Reference to the assigned options.
    FixTemplateOptions &operator=(FixTemplateOptions o);

    /// @brief Swaps the contents of two FixTemplateOptions objects.
    /// @param o Options to swap with.
    void swap(FixTemplateOptions &o);

    bool useHdtLib;                               ///< Use hdtlib types. Default: false.
    hif::semantics::ILanguageSemantics *checkSem; ///< Semantics interface for checks.
    bool setConstExpr;                            ///< Set PROPERTY_CONSTEXPR if true. Default: false.
    bool fixStandardDeclarations;                 ///< Fix standard library declarations. Default: false.
};

/// @brief Fixes template parameter-related issues in the system object.
/// @details Adjusts template parameters and resolves potential conflicts or
/// inconsistencies based on the provided options and semantics.
/// @param o Pointer to the system object.
/// @param sem Pointer to the language semantics interface.
/// @param opt Options specifying the fixes to apply (default: FixTemplateOptions()).

void fixTemplateParameters(
    hif::System *o,
    hif::semantics::ILanguageSemantics *sem,
    const FixTemplateOptions &opt = FixTemplateOptions());

} // namespace manipulation
} // namespace hif
