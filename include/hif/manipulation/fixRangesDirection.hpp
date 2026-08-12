/// @file fixRangesDirection.hpp
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

/// @brief Fixes the problem caused by span upto.
/// E.g., in SystemC they don't exists.
/// The original span is transformed into the common [N downto 0].
/// A vhdl example:
/// @code
/// signal s : std_logic_vector(0 to 12);

/// @endcode
///
/// Is changed to:
/// @code
/// signal s : std_logic_vector(12 downto 0);

/// @endcode
///
/// @param o The system object.
/// @param sem The semantics.
///

void fixRangesDirection(hif::System *o, hif::semantics::ILanguageSemantics *sem);

} // namespace manipulation
} // namespace hif
