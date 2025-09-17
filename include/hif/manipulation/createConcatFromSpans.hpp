/// @file createConcatFromSpans.hpp
/// @brief Create concatenated objects from HIF span definitions.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/analysis/analyzeSpans.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Given a set of indeces, creates a concat expression.
/// @param spanType The type of the span represented by the given indeces.
/// @param indexMap The input indeces and matching values.
/// @param sem The reference semantics.
/// @param others Optional value for indeces not pushed into indexMap.
/// @return The result, or nullptr in case of error.

Value *createConcatFromSpans(
    Type *spanType,
    const hif::analysis::IndexMap &indexMap,
    hif::semantics::ILanguageSemantics *sem,
    Value *others);

} // namespace manipulation
} // namespace hif
