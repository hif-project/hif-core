/// @file rangeGetBitwidth.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/semantics/HIFSemantics.hpp"

namespace hif
{
namespace semantics
{

/// @brief Computes the bitwidth used to represent values belonging to
/// the given range @p r.
/// The bitwidth is obtained by computing the logarithm of
/// max(abs(left), abs(right)) to base 2.
///
/// @param r The range of which the biwidth is to be computed.
/// @param sem The reference semantics.
/// @return Bitwidth to represent values belonging to @p r if statically
/// computable, 0 otherwise.
///

unsigned int rangeGetBitwidth(Range *r, ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance());

} // namespace semantics
} // namespace hif
