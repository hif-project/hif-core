/// @file expandGenerates.hpp
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

/// Starting from @p root, expands all generate constructs.
/// @param root The starting object from which to start the code manipulation.
/// @param refLang The reference semantics.
/// @return <tt>true</tt> if at least one expansion has been performed.
///

bool expandGenerates(Object *root, hif::semantics::ILanguageSemantics *refLang);

} // namespace manipulation
} // namespace hif
