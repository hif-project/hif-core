/// @file compareValues.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{
namespace semantics
{

/// @name Value comparison stuff.
///
///	@{

/// @brief Enumeration for value comparison result.
enum class CompareResult : unsigned char {
    UNKNOWN,
    GREATER,
    EQUAL,
    LESS,
};

/// @brief Compares two values.
///
/// @param v1 The first value.
/// @param v2 The second value.
/// @param refSem The reference semantics.
/// @param simplify If <tt>true</tt> simplifies the values before
/// performing comparison.
/// @return The comparison result between the two values.

CompareResult compareValues(Value *v1, Value *v2, hif::semantics::ILanguageSemantics *refSem, bool simplify);

/// @}

} // namespace semantics
} // namespace hif
