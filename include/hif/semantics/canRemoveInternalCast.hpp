/// @file canRemoveInternalCast.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

namespace hif
{
class Object;
class Type;

namespace semantics
{
class ILanguageSemantics;
} // namespace semantics

} // namespace hif

namespace hif
{
namespace semantics
{
/// @brief Checks whether a cast of type t2 can be removed.
/// Works also for assign casts, passing the lefthandside type as t1
/// and the assignment as o.
/// @param t1 The external cast type.
/// @param t2 The nested cast type to be checked for erasing.
/// @param t3 The inner value type.
/// @param sem The reference semantics.
/// @param o The considered object (i.e. the external cast).
/// @param conservativeBehavior True for safer behavior. Default is false.
/// @return True when the nedted cast can be removed.
bool canRemoveInternalCast(
    Type *t1,
    Type *t2,
    Type *t3,
    ILanguageSemantics *sem,
    Object *o,
    bool conservativeBehavior = false);

} // namespace semantics
} // namespace hif
