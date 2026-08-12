/// @file simplifyExpression.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/hif.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Simplify given expression.
/// @param e The expression to be simplified.
/// @param sem The semantics.
/// @param opt The simplify options.
/// @return The result of the simplification.
Value *simplifyExpression(Expression *e, hif::semantics::ILanguageSemantics *sem, SimplifyOptions opt);

} // namespace manipulation
} // namespace hif
