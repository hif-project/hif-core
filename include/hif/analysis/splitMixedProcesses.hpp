/// @file splitMixedProcesses.hpp
/// @brief Provides functionality for splitting mixed processes into pure
/// synchronous and asynchronous processes.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/analysis/analysisTypes.hpp"
#include "hif/classes/classes.hpp"

namespace hif
{

namespace analysis
{

/// @brief Splits mixed processes into pure synchronous and pure asynchronous
/// processes.
/// @details This function analyzes and transforms processes classified as mixed
/// into separate pure synchronous and asynchronous processes. It uses the
/// provided reference semantics and options to guide the transformation
/// process. The results are stored in the given process map for further
/// analysis or modifications.
///
/// @param map The map where results are stored, with keys as processes and values as their classification.
/// @param sem The reference semantics, defining how processes are interpreted and classified.
/// @param opt The analysis options, allowing customization of the splitting process. Default: AnalyzeProcessOptions().
/// @return True if the splitting process succeeds; false otherwise.

auto splitMixedProcesses(
    AnalyzeProcessOptions::ProcessMap &map,
    hif::semantics::ILanguageSemantics *sem,
    const AnalyzeProcessOptions &opt = AnalyzeProcessOptions()) -> bool;

} // namespace analysis

} // namespace hif
