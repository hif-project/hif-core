/// @file transformIfToWhen.hpp
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

/// @brief Transforms an If object into a corresponding When object.
/// @warning It currently works only if branches contain single return statements.
/// @param ifo The starting if object.
/// @param sem The reference semantics.
/// @return The corresponding When, or nullptr if unable to translate.
///

When *transformIfToWhen(If *ifo, hif::semantics::ILanguageSemantics *sem);

} // namespace manipulation
} // namespace hif
