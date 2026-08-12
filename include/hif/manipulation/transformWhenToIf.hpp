/// @file transformWhenToIf.hpp
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

/// @brief Transforms the given when to an if statement.
/// The given When is not deleted, but its children are moved.
/// The translation works by checking the parent, which must be an assign
/// or a Return.
///
/// @param wo The when object.
/// @param sem The reference semantics.
/// @return The If object or nullptr in case of error..
///

If *transformWhenToIf(When *wo, hif::semantics::ILanguageSemantics *sem);

} // namespace manipulation
} // namespace hif
