/// @file standardizeDescription.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/application_utils.hpp"
#include "hif/classes/classes.hpp"

namespace hif
{
namespace semantics
{

/// @brief Function to get standardize an HIF description. This is the entry point
/// for standardization. This function introduces the necessaries cast and then
/// simply, where is possible, to obtain a standard HIF description.
///
/// @param root Object pointer to the root of the tree which have to be standardize.
/// @param source_sem The semantics of destination language.
/// @param dest_sem The semantics of source language.
/// @param parentFileManager The parent step file manager if any.
///

void standardizeDescription(
    System *&root,
    ILanguageSemantics *source_sem,
    ILanguageSemantics *dest_sem,
    hif::application_utils::StepFileManager *parentFileManager = nullptr);
} // namespace semantics
} // namespace hif
