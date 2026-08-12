/// @file renameForbiddenNames.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/semantics/ILanguageSemantics.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Checks whether a name conflicts with a reserved keyword according
/// to semantics @p sem. If a conflict is found, renaming is performed.
///
/// @param root The subtree in which names are to be checked.
/// @param sem The reference semantics.
///

void renameForbiddenNames(Object *root, hif::semantics::ILanguageSemantics *sem);

} // namespace manipulation
} // namespace hif
