/// @file transformRangeToSpan.hpp
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

/// @brief Transforms a range into the corresponding span.
///
/// @param r The range to be converted.
/// @param refLang The reference semantics.
/// @param isSigned <tt>true</tt> if the values represented by the span
/// returned are signed, <tt>false</tt> if they are unsigned.
/// @return The corresponding span.
///

Range *transformRangeToSpan(Range *r, hif::semantics::ILanguageSemantics *refLang, bool isSigned = true);

} // namespace manipulation
} // namespace hif
