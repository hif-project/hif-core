/// @file renameInScope.hpp
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

/// @brief Renames a declaration in the scope containing it.
///
/// @param decl The declaration to be renamed.
/// @param refSem The reference semantics.
/// @return bool <tt>true</tt> if the declaration has been successfully renamed.
///

bool renameInScope(Declaration *decl, hif::semantics::ILanguageSemantics *refSem);

} // namespace manipulation
} // namespace hif
