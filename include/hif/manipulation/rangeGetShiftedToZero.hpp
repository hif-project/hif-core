/// @file rangeGetShiftedToZero.hpp
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

/// @brief Shifts a span to zero by preserving the direction.
///
/// @param range The input span.
/// @param refLang The reference semantics.
/// @return The span shifted to zero.
///

Range *rangeGetShiftedToZero(Range *range, hif::semantics::ILanguageSemantics *refLang);

} // namespace manipulation
} // namespace hif
