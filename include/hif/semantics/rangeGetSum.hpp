/// @file rangeGetSum.hpp
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

/// Returns a new span that is the sum between the given spans
/// @p a and @p b.
///
/// @param a The first range to be compared.
/// @param b The second range to be compared.
/// @param refLang The reference semantics.
/// @return The span that is the sum between @p a and @p b.
///

Range *rangeGetSum(Range *a, Range *b, ILanguageSemantics *refLang);

} // namespace semantics
} // namespace hif
