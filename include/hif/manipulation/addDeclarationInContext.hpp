/// @file addDeclarationInContext.hpp
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

/// @brief Add given @p newDecl in the same context of given @p context declaration.
/// @param newDecl The new declaration to add.
/// @param context The context.
/// @param before If <tt>true</tt> add declaration before, after otherwise.
/// @return <tt>true</tt> if added, <tt>false</tt> otherwise.

bool addDeclarationInContext(Declaration *newDecl, Declaration *context, bool before = false);

} // namespace manipulation
} // namespace hif
