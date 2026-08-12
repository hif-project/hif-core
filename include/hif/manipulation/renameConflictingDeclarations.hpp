/// @file renameConflictingDeclarations.hpp
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

/// @brief Renames declarations inside design units, to avoid name clashes.
///
/// @param root The tree to be fixed.
/// @param sem The semantics.
/// @param refMap Optional map of all references.
/// @param suffix Optional suffix for the renaming.
/// @return True if at least one renaming has been performed.
///

bool renameConflictingDeclarations(
    Object *root,
    hif::semantics::ILanguageSemantics *sem,
    std::map<Declaration *, std::set<Object *>> *refMap = nullptr,
    const std::string &suffix                           = "");

} // namespace manipulation
} // namespace hif
