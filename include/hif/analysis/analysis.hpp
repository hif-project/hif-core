/// @file analysis.hpp
/// @brief Main include for HIF analysis utilities and algorithms.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

namespace hif
{
/// @brief Wraps methods to analyze and change design characteristics.
namespace analysis
{
} // namespace analysis
} // namespace hif

#include "hif/analysis/analysisTypes.hpp"
#include "hif/analysis/analyzeProcesses.hpp"
#include "hif/analysis/analyzeSpans.hpp"
#include "hif/analysis/insertDelays.hpp"
#include "hif/analysis/sortGraph.hpp"
#include "hif/analysis/splitMixedProcesses.hpp"
