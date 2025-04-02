/// @file getOtherOperandType.hpp
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

/// Given a Value returns the type of the other operand involved in the operation.
/// @param o The starting value.
/// @param refSem The reference semantics.
/// @param considerOverloading <tt>true</tt> if type must be retrieved via context
/// checking in case of overloaded methods. Default is <tt>false</tt>.
/// @param looseTypeChecks if <tt>true</tt> don't raise typing errors.
/// This is userfull during parsing fixes since the tree could be not typable.
/// Default is <tt>false</tt>.
/// @return The other operand type, or nullptr in case of error.

Type *getOtherOperandType(
    Object *o,
    ILanguageSemantics *refSem,
    bool considerOverloading = false,
    bool looseTypeChecks     = false);

/// @brief Returns the direction of the other operand involved in the operation.
/// @param o The starting value.
/// @param sem The reference semantics.
/// @return The direction.
///

hif::RangeDirection getOtherOperandDirection(Object *o, ILanguageSemantics *sem);
} // namespace semantics
} // namespace hif
