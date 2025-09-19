/// @file explicitCallsParameters.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/manipulation/sortParameters.hpp"
#include "hif/semantics/referencesUtils.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Explicts all parameters of given subprograms refs.
/// @param subprograms The list of subprograms to be fixed.
/// @param refMap The references map.
/// @param sem The semantics.
/// @param kind The explicit kind (@see sortParameters). Default is ALL.
void explicitCallsParameters(
    std::list<SubProgram *> &subprograms,
    hif::semantics::ReferencesMap &refMap,
    hif::semantics::ILanguageSemantics *sem,
    SortMissingKind kind = SortMissingKind::ALL);

/// @brief Explicts all parameters of given subprograms refs.
/// @param subprograms The set of subprograms to be fixed.
/// @param refMap The references map.
/// @param sem The semantics.
/// @param kind The explicit kind (@see sortParameters). Default is ALL.
void explicitCallsParameters(
    std::set<SubProgram *> &subprograms,
    hif::semantics::ReferencesMap &refMap,
    hif::semantics::ILanguageSemantics *sem,
    SortMissingKind kind = SortMissingKind::ALL);

} // namespace manipulation
} // namespace hif
