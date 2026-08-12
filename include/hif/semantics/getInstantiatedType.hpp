/// @file getInstantiatedType.hpp
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
/// @brief Try to find the instantiated type of the Type t according with
/// semantics passed as parameter. The default semantics is HIF.
///
/// @param t the starting Type
/// @param ref_sem The reference semantics.
///
/// @return a fresh instantiated <tt>t</tt> if it's possible to establish it, nullptr otherwise
///

Type *getInstantiatedType(Type *t, ILanguageSemantics *ref_sem = HIFSemantics::getInstance());
} // namespace semantics
} // namespace hif
