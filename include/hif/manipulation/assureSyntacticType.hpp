/// @file assureSyntacticType.hpp
/// @brief Ensure syntactic type correctness for HIF objects.
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

/// @brief Checks whether the passed value needs a syntactic type with
/// respect to its parent.
/// @param v The value to be checked.
/// @return <tt>true</tt> if @p v needs a syntactic type, or it does
/// not have a parent.

bool needSyntacticType(Value *v);
/// @brief Checks whether the passed value is a ConstValue, and in that
/// case it sets the syntactic type, with this priority:
/// * if already present, do nothing;
/// * if passed as @p suggestedType, set it;
/// * otherwise, retrieve the type from semantics @p sem.
/// @param v The value to be checked.
/// @param sem The reference semantics.
/// @param suggestedType The type to set, if specified.
/// Otherwise, retrieve it from semantics @p sem.
/// @return The checked value.
///

Value *assureSyntacticType(Value *v, hif::semantics::ILanguageSemantics *sem, Type *suggestedType = nullptr);

} // namespace manipulation
} // namespace hif
