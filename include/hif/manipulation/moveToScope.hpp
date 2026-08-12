/// @file moveToScope.hpp
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

/// @name Move content from a scope to another one.
//@{

/// @brief Moves the contents of a scope into another scope. Could work on a
/// single object or on the entire contents object.
/// @param oldScope The old scope.
/// @param newScope The new scope.
/// @param sem The reference semantics.
/// @param suffix The suffix to append to the future object(s) name. If not
///     specified, the current name will be used as prefix of the future name.
/// @param obj If specified, it means to move only that object. If not, it moves
///     the entire oldScope contents.

void moveToScope(
    Scope *oldScope,
    Scope *newScope,
    hif::semantics::ILanguageSemantics *sem,
    const std::string &suffix,
    Object *obj = nullptr);

/// @brief Particular case used to deal with For declarations.
/// @param oldScope The old for scope.
/// @param newScope The new scope.
/// @param sem The reference semantics.
/// @param suffix The suffix to append to the future object(s) name.
/// @param obj If specified, it means to move only that object.

void moveToScope(
    For *oldScope,
    Scope *newScope,
    hif::semantics::ILanguageSemantics *sem,
    const std::string &suffix,
    Object *obj = nullptr);

/// @brief (Wrapper) Moves all the elements of a list contained in a scope
/// into the corresponding list of another scope.
/// @param oldScope The old scope.
/// @param newScope The new scope.
/// @param sem The reference semantics.
/// @param suffix The suggested suffix for renaming.
/// @param list The list to move.
template <typename T>
void moveToScope(
    Scope *oldScope,
    Scope *newScope,
    hif::semantics::ILanguageSemantics *sem,
    const std::string &suffix,
    BList<T> &list)
{
    typename BList<T>::iterator it = list.begin();
    while (it != list.end()) {
        moveToScope(oldScope, newScope, sem, suffix, *it);
        it = list.begin();
    }
}

//@}

} // namespace manipulation
} // namespace hif
