/// @file checkConflictingName.hpp
/// @brief Check for conflicting names in HIF symbol tables.
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

/// @brief Checks whether the given name matches a declaration into given scope.
/// @param n The name to be checked.
/// @param scope The scope where to perform the check.
/// @return <tt>true</tt> if the name is conflicting, <tt>false</tt> otherwise.
///

bool checkConflictingName(const std::string& n, Scope *scope);

} // namespace manipulation
} // namespace hif
