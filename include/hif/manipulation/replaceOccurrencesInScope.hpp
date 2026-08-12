/// @file replaceOccurrencesInScope.hpp
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

/// @brief Finds all the references of @p decl in the sub-tree starting from @p root,
/// and replaces them with a copy of @p to.
///
/// @param decl The declaration the references of which are to be found.
/// @param to The object to perform replacement with.
/// @param refLang The reference semantics.
/// @param root The sub-tree from which to start the search.
///

void replaceOccurrencesInScope(
    Declaration *decl,
    Object *to,
    hif::semantics::ILanguageSemantics *refLang,
    Object *root);

} // namespace manipulation
} // namespace hif
