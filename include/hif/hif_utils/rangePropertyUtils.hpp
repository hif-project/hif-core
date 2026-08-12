/// @file rangePropertyUtils.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Returns the upper bound of a range, depending on its direction.
///
/// @param r The range of which the upper bound is to be computed.
/// @return The upper bound of @p r.
///

Value *rangeGetMaxBound(Range *r);

/// @brief Returns the lower bound of a range, depending on its direction.
///
/// @param r The range of which the lower bound is to be computed.
/// @return The lower bound of @p r.
///

Value *rangeGetMinBound(Range *r);

/// @brief Sets the upper bound of a range, depending on its direction.
///
/// @param r The range of which the upper bound is to be computed.
/// @param v The value to set.
/// @return The old upper bound of @p r.
///

Value *rangeSetMaxBound(Range *r, Value *v);

/// @brief Sets the lower bound of a range, depending on its direction.
///
/// @param r The range of which the lower bound is to be computed.
/// @param v The value to set.
/// @return The old lower bound of @p r.
///

Value *rangeSetMinBound(Range *r, Value *v);

/// @brief Returns whether a given range @p r must be considered as a true span
/// or range or as a generic value.
/// For example, if @p r is the value inside a switch alternative, the function
/// returns <tt>true</tt>.
///
/// @param r The range to be checked.
/// @return <tt>true</tt> if @p r must be considered as a generic value.
///

bool rangeIsValue(Range *r);

} // namespace hif
