/// @file rebaseTypeSpan.hpp
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

/// @brief: Manage types having spans different from [N downto 0]
///
/// In source code types can have arbitrary span. A VHDL example:
/// @code
/// signal s : std_logic_vector(7 downto 4);

/// @endcode
///
/// It is translated in SystemC as:
/// @code
/// sc_lv<4> s;

/// @endcode
///
/// Thus, the length of the span is preserved.
/// The problem is that when a member or a slice of that signal is used, the
/// index ranges from 4 to 7 in the source language, while it ranges from
/// 0 to 3 in the destination language.
/// In order to preserve equivalence, the lower bound must be subtracted
/// from the indices of all members and slices of the signal, as well as
/// to the type span.
///
/// @warning If at least one span has been rebased, the semantic types
/// on the resulting tree must be recomputed.
///
/// @param root The tree from which rebasing is to be performed.
/// @param refLang The reference semantics.
/// @param currentOnly Rebase only the span of given object.
/// @return <tt>true</tt> if at least one span has been rebased, <tt>false</tt> otherwise.
///
/// @{

bool rebaseTypeSpan(Object *root, hif::semantics::ILanguageSemantics *refLang, bool currentOnly);

/// @}

} // namespace manipulation
} // namespace hif
