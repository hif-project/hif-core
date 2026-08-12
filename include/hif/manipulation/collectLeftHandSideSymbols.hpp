/// @file collectLeftHandSideSymbols.hpp
/// @brief Collect left-hand side symbols from HIF assignment statements.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/manipulation/assignUtils.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Returns all symbols which are targets of a given assignment.
/// For example: <tt>(a[i], b) = c</tt> will collect <tt>a</tt> and <tt>b</tt>.
///
/// @param obj The assignment for which symbols are to be collected.
/// @param opt The LeftHandSideOptions used in check.
/// @return The list of collected symbols.
///

std::list<Value *> collectLeftHandSideSymbols(Assign *obj, const LeftHandSideOptions &opt = LeftHandSideOptions());

} // namespace manipulation
} // namespace hif
