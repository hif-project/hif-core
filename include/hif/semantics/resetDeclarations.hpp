/// @file resetDeclarations.hpp
/// @brief
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

/// @brief Options for get/update declaration methods.
struct ResetDeclarationsOptions {
    ResetDeclarationsOptions();
    virtual ~ResetDeclarationsOptions();

    /// @brief Copy constructor.
    /// @param other The object to copy from.
    ResetDeclarationsOptions(const ResetDeclarationsOptions &other);

    /// @brief Assignment operator.
    /// @param other The object to assign from.
    /// @return Reference to this object.
    ResetDeclarationsOptions &operator=(ResetDeclarationsOptions other);

    /// @brief Swaps the contents of this object with another.
    /// @param other The object to swap with.
    void swap(ResetDeclarationsOptions &other);

    /// @brief Resets declarations only of current symbol.
    /// Default is false.
    bool onlyCurrent;

    /// @brief If a declaration is visible from the symbol scope, reset it.
    /// Default is false.
    bool onlyVisible;

    /// @brief Resets only symbols related to declaration signatures.
    /// For example, in case of function calls, the declarations of ParameterAssigns
    /// will be reset, but the ParameterAssigns values will be skipped.
    /// Default is false.
    bool onlySignatures;

    /// @brief The semantics used by getDeclaration (if any).
    /// Default is HIF.
    ILanguageSemantics *sem;
};

/// @brief Resets the declaration member of objects that have it
/// starting from @p o.
/// Declaration members will be set to nullptr.
///
/// @param o The starting root object.
/// @param opt The options.
///

void resetDeclarations(Object *o, const ResetDeclarationsOptions &opt = ResetDeclarationsOptions());

/// @brief Resets the declaration member of objects that have it
/// starting from @p o.
/// Declaration members will be set to nullptr.
///
/// @param o The starting list object.
/// @param opt The options.
///

void resetDeclarations(BList<Object> &o, const ResetDeclarationsOptions &opt = ResetDeclarationsOptions());

/// @brief Resets the declaration member of objects that have it
/// starting from @p o.
/// Declaration members will be set to nullptr.
///
/// @param o The starting list object.
/// @param opt The options.
///
template <typename T>
void resetDeclarations(BList<T> &o, const ResetDeclarationsOptions &opt = ResetDeclarationsOptions());
} // namespace semantics
} // namespace hif
