/// @file fixNestedDeclarations.hpp
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

/// @brief Fix declarations nested in other declaration. This is not allowed
/// is SystemC.
/// @param o The system object.
/// @param sem The semantics.

void fixNestedDeclarations(hif::System *o, hif::semantics::ILanguageSemantics *sem);

} // namespace manipulation
} // namespace hif
