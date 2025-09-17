/// @file aggregateUtils.hpp
/// @brief Utility functions for manipulating HIF aggregate objects.
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

/// @brief Tries to transform the given aggregate removing its alts whenever is
/// possibile. It tries two simplifications:
/// 1) Removing of alts having same value of others (if any) and according to
/// the <tt>atLeastOne</tt> argument.
/// 2) Removing of all alts by adding an others clause if there is not already
/// one and all alts values are equal.
/// @note This methods directly manipulates the given aggregate @p o.
/// @param obj The aggregate to be managed.
/// @param atLeastOne If <tt>true</tt> remove all alts compatible with the
/// others clause, otherwise remove only if all alts are compatible.
/// @param sem The semantics.
/// @return <tt>true</tt> if changes on the given aggregate are performed.
///

bool transformAggregateRollingAlts(Aggregate *obj, const bool atLeastOne, hif::semantics::ILanguageSemantics *sem);
/// @brief Tries to transform the given aggregate by expanding its others
/// clause value for each missing alt.
/// This can be done only if the span is statically computable.
/// Expansion is to be carried out only if size of the span does not exceed the
/// given threshold.
/// @note This methods directly manipulates the given aggregate @p o.
/// @param obj The aggregate to be managed.
/// @param threshold The threshold the size of the span is to be compared with.
/// @param sem The semantics.
/// @param force If <tt>true</tt> force unrolling by expanding symbols.
/// @return <tt>true</tt> if changes on the given aggregate are performed.
///

bool transformAggregateUnrollingAlts(
    Aggregate *obj,
    unsigned long long threshold,
    hif::semantics::ILanguageSemantics *sem,
    const bool force = false);
} // namespace manipulation
} // namespace hif
