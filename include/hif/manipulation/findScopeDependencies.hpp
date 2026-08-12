/// @file findScopeDependencies.hpp
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

/// @brief Visits the given object root to determine scope dependencies
/// between objects in its subtree. Only DesignUnit, LibraryDef and System
/// are considered.
/// @param root The root of the tree (usually the system object).
/// @param scopeRelations The map storing pointers to scope objects in
/// relation with the list of objects representing their scope to @p root.

void findScopeDependencies(hif::System *root, std::map<hif::Scope *, std::list<hif::Scope *>> &scopeRelations);

} // namespace manipulation
} // namespace hif
