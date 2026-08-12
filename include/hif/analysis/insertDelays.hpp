/// @file insertDelays.hpp
/// @brief Defines utilities for inserting delays into design units.
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

/// @brief Stores properties of a delay to be injected into a design unit.
/// @details This structure encapsulates the delay's characteristics, including
/// the port to inject, the number of clock or delta cycles, and whether it
/// represents a half-clock delay.
struct DelayProperties {
    Port *port              = nullptr; ///< The port where the delay will be injected.
    std::size_t clockCycles = 0;       ///< Number of clock cycles of delay.
    std::size_t deltas      = 0;       ///< Number of delta cycles of delay.
    bool halfClock          = false;   ///< Indicates if this is a half-clock delay.
};

/// @brief Stores delay information associated with a specific design unit.
/// @details This structure contains the details of delays to be injected into a
/// design unit, including the reference clock, reset, and working edges.
struct DelayInfos {
    using DelayMap    = std::map<Port *, DelayProperties>; ///< Map of ports to delay properties.
    using WorkingEdge = ProcessInfos::WorkingEdge;         ///< Alias for the working edge type.
    using ResetPhase  = ProcessInfos::ResetPhase;          ///< Alias for the reset phase type.

    View *view              = nullptr;                ///< The view to be injected with delays.
    Port *clock             = nullptr;                ///< The reference clock for the delays.
    Port *reset             = nullptr;                ///< The reference reset for the delays.
    WorkingEdge workingEdge = ProcessInfos::NO_EDGE;  ///< The clock's working edge.
    ResetPhase resetPhase   = ProcessInfos::NO_PHASE; ///< The reset phase.
    DelayMap delayProperties;                         ///< Map of delays to be injected.
};

/// @brief List of delay information to be applied to design units.
using DelayList = std::list<DelayInfos>;

/// @brief Injects delays into specified design units.
/// @details This function takes a list of delays and applies them to the specified design units
/// based on the provided reference semantics. It supports clock cycles, delta cycles, and
/// half-clock delays.
/// @param delays The list of delays to be injected.
/// @param sem The reference semantics used for applying the delays.
/// @return True if the delays were successfully applied, false otherwise.

auto insertDelays(DelayList &delays, hif::semantics::ILanguageSemantics *sem) -> bool;

} // namespace analysis

} // namespace hif
