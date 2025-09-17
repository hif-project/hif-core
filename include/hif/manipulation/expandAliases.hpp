/// @file expandAliases.hpp
/// @brief Expand alias objects to their referenced HIF objects.
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

/// Starting from @p root, expands all aliases by replacing them with
/// their corresponding values.
/// @param root The starting object from which to start the code manipulation.
/// @param refLang The reference semantics.
///

void expandAliases(Object *root, hif::semantics::ILanguageSemantics *refLang);

} // namespace manipulation
} // namespace hif
