/// @file DeclarationOptions.hpp
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
struct DeclarationOptions {
    void swap(DeclarationOptions &other);

    /// @brief The object from which to start the search. If it is
    /// set to nullptr, start the search from symbol.
    Object *location = nullptr;

    /// @brief If <tt>true</tt> and the declaration member of symbol
    /// is already set, it is ignored and the declaration is re-computed.
    /// Default is false.
    bool forceRefresh = false;

    /// @brief If <tt>true</tt> returns the declaration only if
    /// it is already set in the declaration member of symbol.
    /// Default is false.
    bool dontSearch = false;

    /// @brief If <tt>true</tt> and a declaration can not found, raise an error.
    /// Default is false.
    bool error = false;

    /// @brief If <tt>true</tt> don't raise typing errors.
    /// This is userfull during parsing fixes since the tree could be not typable.
    /// Default is <tt>false</tt>.
    bool looseTypeChecks = false;
};

} // namespace semantics
} // namespace hif
