/// @file setDeclaration.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{
namespace semantics
{

/// @brief Sets the declaration of an object.
/// This function works only with objects that have the SetDeclaration
/// function defined (which is typically private).
///
/// @param o The object of which the declaration has to be set.
/// @param decl The declaration to be set.
///

void setDeclaration(Object *o, Object *decl);

} // namespace semantics
} // namespace hif
