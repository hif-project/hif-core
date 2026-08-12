/// @file operatorUtils.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"

namespace hif
{
/// @brief Checks whether the passed operator is arithmetic.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is arithmetic, <tt>false</tt> otherwise.

bool operatorIsArithmetic(const hif::Operator oper);

/// @brief Checks whether the passed operator is binary.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is binary, <tt>false</tt> otherwise.

bool operatorIsBinary(const hif::Operator oper);

/// @brief Checks whether the passed operator is commutative.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is commutative, <tt>false</tt> otherwise.

bool operatorIsCommutative(const hif::Operator oper);

/// @brief Checks whether the passed operator is logical.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is logical, <tt>false</tt> otherwise.

bool operatorIsLogical(const hif::Operator oper);

/// @brief Checks whether the passed operator is logical bitwise.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is logical bitwise, <tt>false</tt> otherwise.

bool operatorIsBitwise(const hif::Operator oper);

/// @brief Checks whether the passed operator is a reduce one.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is a reduce operator, <tt>false</tt> otherwise.

bool operatorIsReduce(const hif::Operator oper);

/// @brief Checks whether the passed operator is relational.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is relational, <tt>false</tt> otherwise.

bool operatorIsRelational(const hif::Operator oper);

/// @brief Checks whether the passed operator is equality.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is equality, <tt>false</tt> otherwise.

bool operatorIsEquality(const hif::Operator oper);

/// @brief Checks whether the passed operator is a rotate one.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is a rotate operator, <tt>false</tt> otherwise.

bool operatorIsRotate(const hif::Operator oper);

/// @brief Checks whether the passed operator is a shift one.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is a shift operator, <tt>false</tt> otherwise.

bool operatorIsShift(const hif::Operator oper);

/// @brief Checks whether the passed operator is an address one.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is an address operator, <tt>false</tt> otherwise.

bool operatorIsAddress(const hif::Operator oper);

/// @brief Checks whether the passed operator is unary.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is unary, <tt>false</tt> otherwise.

bool operatorIsUnary(const hif::Operator oper);

/// @brief Checks whether the passed operator is an assignment, i.e. op_assign, op_conv, op_bind.
/// @param oper The operator.
/// @return <tt>true</tt> if the operator is an assignment, <tt>false</tt> otherwise.

bool operatorIsAssignment(const hif::Operator oper);

/// @brief Returns the inverse operator of the passed one (if it exists).
/// @param oper The operator.
/// @return The inverse operator (if it exists), <tt>op_none</tt> otherwise.

hif::Operator operatorGetInverse(const hif::Operator oper);

/// @brief Returns the conversion from logic to bitwise and vice versa.
/// @param oper The operator.
/// @return The converted operator (if it exists), <tt>op_none</tt> otherwise.

hif::Operator operatorGetLogicBitwiseConversion(const hif::Operator oper);

} // namespace hif
