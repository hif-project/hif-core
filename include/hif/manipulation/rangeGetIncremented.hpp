/// @file rangeGetIncremented.hpp
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

/// @brief Given a span @p range, returns a new span with the upper bound
/// incremented by one.
/// For example, if the span is (0 to 10), this function returns the span
/// (0 to 11).
///
/// @param range The input span.
/// @param refLang The reference semantics.
/// @param increment The increment. Default is 1.
/// @return The new span with the upper bound incremented by one.
///

Range *rangeGetIncremented(Range *range, hif::semantics::ILanguageSemantics *refLang, std::int64_t increment = 1);

} // namespace manipulation
} // namespace hif
