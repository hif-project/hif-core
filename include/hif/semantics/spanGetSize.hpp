/// @file spanGetSize.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/manipulation/simplify.hpp"

namespace hif
{
namespace semantics
{

/// @brief Extracts the span of a type @p t and it returns the span size
/// by calling the spanGetSize function.
///
/// @param t The type from which the span is to be extracted.
/// @param refLang The reference semantics.
/// @return The span size.
///

Value *typeGetSpanSize(Type *t, ILanguageSemantics *refLang);

/// @brief Returns the size of span @p r.
/// The size of the span @p r is computed as (r'high - r'low + 1).
/// If the size of the span can not be computed, nullptr will be returned.
///
/// @param r The span of which the size is to be computed.
/// @param sem The reference semantics.
/// @param simplify If true simplify given range.
/// @param opts Simplify options.
/// @return The size of the span @p r computed as (r'high - r'low + 1).
///

Value *spanGetSize(
    Range *r,
    ILanguageSemantics *sem,
    bool simplify                            = true,
    const hif::manipulation::SimplifyOptions &opts = hif::manipulation::SimplifyOptions());

/// @brief Extracts the span of a type @p t.
/// If it's an array extracts the sum of every element's span
///
/// @param t The type from which the span is to be extracted.
/// @param sem The reference semantics.
/// @return The span size.
///

Value *typeGetTotalSpanSize(Type *t, ILanguageSemantics *sem);

} // namespace semantics
} // namespace hif
