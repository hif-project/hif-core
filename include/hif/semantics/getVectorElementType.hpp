/// @file getVectorElementType.hpp
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

/// @brief Returns the type of elements in case of vector or array types.
///
/// @param t The Type to analyze.
/// @param refLang The semantics.
/// @return The type of elements in case of vector or array types, nullptr otherwise.
///

Type *getVectorElementType(Type *t, ILanguageSemantics *refLang);

} // namespace semantics
} // namespace hif
