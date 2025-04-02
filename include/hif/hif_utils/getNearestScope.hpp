/// @file getNearestScope.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Finds the nearest scope of the object with selected features.
/// This function will go up the Hif tree starting from a given object
/// until it reaches a Scope object matching the given features.
///
/// @param o The starting point of the function search.
/// @param needDeclarationList Find a scope which has a list of declarations.
/// @param needLibraryList Find a scope which has a list of libraries.
/// @param needTemplates Find a scope which has a list of templates.
/// @return The wanted Hif object or nullptr if it has not been found.
///

Scope *getNearestScope(Object *o, bool needDeclarationList, bool needLibraryList, bool needTemplates);

} // namespace hif
