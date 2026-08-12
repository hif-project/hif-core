/// @file getDeclarationTotalInstances.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <map>

#include "hif/application_utils/portability.hpp"
#include "hif/classes/classes.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Maps a scope to its total instance count.
typedef std::map<hif::Scope *, std::uint64_t> TotalInstanceMap;

/// @brief Counts the total number of subinstances for a top-level declaration.
/// @param scope The scope containing the top-level declaration.
/// @param sem The language semantics interface used for analysis.
/// @return A map associating each declaration scope with its occurrence count.

TotalInstanceMap getDeclarationTotalInstances(hif::Scope *scope, hif::semantics::ILanguageSemantics *sem);

} // namespace manipulation
} // namespace hif
