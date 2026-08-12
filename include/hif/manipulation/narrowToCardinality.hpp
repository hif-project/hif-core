/// @file narrowToCardinality.hpp
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

/// @brief Given a value, gets the subvalue which matches the given cardinality.
/// For instance: <tt>sc_lv<9>b[10]</tt> with <tt>v = b[5][3]</tt> and
/// <tt>c = 1</tt>, returns <tt>b[5]</tt>.
///
/// @param v The value.
/// @param c The cardinality.
/// @param sem The semantics.
/// @param considerOnlyBits <tt>true</tt> if only bit types are to be considered,
///        <tt>false</tt> otherwise
/// @return The subvalue or nullptr in case of error.
///

Value *narrowToCardinality(
    Value *v,
    unsigned int c,
    hif::semantics::ILanguageSemantics *sem,
    bool considerOnlyBits = true);

} // namespace manipulation
} // namespace hif
