/// @file moveDeclaration.hpp
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

/// @brief Moves a declaration into another scope.
/// This function:
///     - removes declaration from old scope;
///     - moves declaration into newScope;
///     - renames declaration with a fresh name (avoid collisions);
///     - substitutes all occurrences with the fresh name starting from subtree.
/// @param decl Declaration object to be moved.
/// @param newScope Scope where the declaration should be moved.
/// @param subtree Subtree where the declaration is used.
/// @param refSem The reference semantics.
/// @param suffix The suffix to append to the future declaration name. If not
///     specified, declaration name will be used as prefix of the future name.
/// @param pos The position within the suitable list in which the declaration will be moved.
/// @return the (eventually updated) name of declaration if it has been
///     successfully moved, nullptr otherwise.
///

std::string moveDeclaration(
    Declaration *decl,
    Scope *newScope,
    Object *subtree,
    hif::semantics::ILanguageSemantics *refSem,
    const std::string &suffix,
    int pos = -1);

} // namespace manipulation
} // namespace hif
