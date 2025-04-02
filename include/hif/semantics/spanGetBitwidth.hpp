/// @file spanGetBitwidth.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/manipulation/simplify.hpp"
#include "hif/semantics/HIFSemantics.hpp"

namespace hif
{
namespace semantics
{

/// @brief Computes the bitwidth used to represent values belonging to
/// the given span @p r.
/// It computes SUP - INF + 1.
///
/// @param r The span of which the bitwidth is to be computed.
/// @param sem The reference semantics.
/// @param simplify True if bounds must be simplified.
/// @param opts The simplify options.
/// @return The numebr of elements in the span @p r if statically computable,
/// 0 otherwise.

std::uint64_t spanGetBitwidth(
    Range *r,
    ILanguageSemantics *sem                        = HIFSemantics::getInstance(),
    const bool simplify                            = true,
    const hif::manipulation::SimplifyOptions &opts = hif::manipulation::SimplifyOptions());

/// @brief Function that return the size of <tt>t</tt>,
///	measured in number of bits.
///
/// @param type Type pointer.
/// @param sem The semantics.
/// @param simplify True if bounds must be simplified.
/// @param opts The simplify options.
/// @return size of <tt>t</tt> measured in number of bits if it is found, 0 otherwise.

std::uint64_t typeGetSpanBitwidth(
    Type *type,
    ILanguageSemantics *sem,
    const bool simplify                            = true,
    const hif::manipulation::SimplifyOptions &opts = hif::manipulation::SimplifyOptions());

} // namespace semantics
} // namespace hif
