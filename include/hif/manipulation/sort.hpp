/// @file sort.hpp
/// @brief
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

/// @brief Options related to sort method.
struct SortOptions {
    SortOptions();
    ~SortOptions();

    /// @brief Indicates whether to recursively proceed with the visit
    /// on children nodes. Default is true.
    bool sortChildren;

    /// @brief <tt>true</tt> if parameters sorting methods must be called to sort
    /// PortAssigns, ParameterAssigns, etc. Default is false.
    bool sortParameters;
};

/// @brief Sorts the given object according with passed options.
///
/// @param obj The object to be sorted.
/// @param sem The reference semantics.
/// @param opt The sort options.
/// @return <tt>True</tt> if at least sort has been performed, <tt>false</tt> otherwise.

bool sort(Object *obj, hif::semantics::ILanguageSemantics *sem, const SortOptions &opt = SortOptions());

} // namespace manipulation
} // namespace hif
