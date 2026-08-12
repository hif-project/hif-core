/// @file analyzeProcesses.hpp
/// @brief Provides functionality to classify and analyze processes in a tree
/// structure.
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

/// @brief Classifies all processes inside a given root node.
/// @param root The root of the hierarchical tree to analyze.
/// @param map The map where analysis results will be stored.
/// @param sem The reference semantics for process classification.
/// @param opt The analysis options.
/// @return True if the analysis succeeds, false otherwise.

auto analyzeProcesses(
    Object *root,
    AnalyzeProcessOptions::ProcessMap &map,
    hif::semantics::ILanguageSemantics *sem,
    const AnalyzeProcessOptions &opt = AnalyzeProcessOptions()) -> bool;

} // namespace analysis

} // namespace hif
