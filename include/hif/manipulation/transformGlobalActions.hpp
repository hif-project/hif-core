/// @file transformGlobalActions.hpp
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

/// @brief Transforms global actions into corresponding processes (state tables).
///
/// @param o The object from which to start the manipulation.
/// @param sem The reference semantics.
/// @param list The list of generated state tables.
/// @param addVariablesInSensitivity If true, add also variables inside sensitivity
///        list (Default: false).
///

void transformGlobalActions(
    Object *o,
    std::set<StateTable *> &list,
    semantics::ILanguageSemantics *sem,
    bool addVariablesInSensitivity = false);
} // namespace manipulation
} // namespace hif
