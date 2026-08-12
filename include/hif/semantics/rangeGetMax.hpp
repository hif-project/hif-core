/// @file rangeGetMax.hpp
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

/// Returns a new range representing the maximum between the given ranges
/// @p a and @p b.
///
/// @param a The first range to be compared.
/// @param b The second range to be compared
/// @param refLang The reference semantics.
/// @return The maximum between @p a and @p b.
///

Range *rangeGetMax(Range *a, Range *b, ILanguageSemantics *refLang);

/// Returns a new range representing the minimum between the given ranges
/// @p a and @p b.
///
/// @param a The first range to be compared.
/// @param b The second range to be compared
/// @param refLang The reference semantics.
/// @return The minimum between @p a and @p b.
///

Range *rangeGetMin(Range *a, Range *b, ILanguageSemantics *refLang);

} // namespace semantics
} // namespace hif
