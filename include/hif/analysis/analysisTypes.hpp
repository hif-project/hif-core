/// @file analysisTypes.hpp
/// @brief Defines types and structures for analyzing processes and handling
/// template parameter fixes.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <list>
#include <map>
#include <set>

#include "hif/application_utils/portability.hpp"

namespace hif
{

namespace analysis
{

/// @brief Utility structure to define template-based typedefs for C++98
/// compatibility.
template <class KEY, class VALUE = KEY> struct Types {
    /// @brief Key type.
    using Key   = KEY;

    /// @brief Value type.
    using Value = VALUE;

    /// @brief Type for storing sets of objects.
    using Set   = std::set<Value *>;

    /// @brief Type for storing maps of objects.
    using Map   = std::map<Key *, Set>;

    /// @brief Type for storing lists of objects.
    using List  = std::list<Key *>;

    /// @brief Type for storing graphs of objects.
    /// First element maps children to parents,
    /// second element maps parents to children.
    using Graph = std::pair<Map, Map>;
};

/// @brief Infos extracted for any process.
///
struct ProcessInfos {
    /// @brief Enumeration of process kinds.
    enum ProcessKind : unsigned char {
        ASYNCHRONOUS,        ///< Asynchronous process.
        SYNCHRONOUS,         ///< Synchronous process.
        DERIVED_SYNCHRONOUS, ///< Derived synchronous process.
        MIXED,               ///< Mixed process.
        DERIVED_MIXED        ///< Derived mixed process.
    };

    /// @brief Enumeration of reset kinds.
    enum ResetKind : unsigned char {
        NO_RESET,                 ///< No reset.
        SYNCHRONOUS_RESET,        ///< Synchronous reset.
        ASYNCHRONOUS_RESET,       ///< Asynchronous reset.
        DERIVED_SYNCHRONOUS_RESET ///< Derived synchronous reset.
    };

    /// @brief Enumeration of working edges for processes.
    enum WorkingEdge : unsigned char {
        NO_EDGE,      ///< No edge.
        RISING_EDGE,  ///< Rising edge.
        FALLING_EDGE, ///< Falling edge.
        BOTH_EDGES    ///< Both edges.
    };

    /// @brief Enumeration of reset phases.
    enum ResetPhase : unsigned char {
        NO_PHASE,   ///< No reset phase.
        HIGH_PHASE, ///< High phase for reset.
        LOW_PHASE   ///< Low phase for reset.
    };

    /// @brief Enumeration of process styles.
    enum ProcessStyle : unsigned char {
        /// The body of the process odes not match other templates.
        NO_STYLE,
        /// Process kind:
        /// if (reset == )
        /// else if (clock.event && clock == )
        ///
        STYLE_1,
        /// Process kind:
        /// if (clock == )
        /// {
        ///     // Optional inner if:
        ///     if (reset)
        ///     {}
        ///     else
        ///     {}
        /// }
        ///
        STYLE_2,
        /// Process kind: SYNCH/DERIVED
        /// switch (state)
        /// {
        ///     case  ...
        /// }
        ///
        STYLE_3,
        /// Process kind: MIXED/SYNCH
        /// made by many inner sections, with one matching
        /// process style 1 or 2
        ///
        STYLE_4,
        /// Process kind:
        /// @(pos clock) a <= b;
        ///
        STYLE_5,
        /// Process kind:
        /// @(pos clock)
        ///   if (reset)
        ///   ...
        ///   else
        ///   ...
        ///
        STYLE_6
    };

    /// @brief TYpe for lists of declarations.
    using ReferredDeclarations = Types<DataDeclaration>::Set;

    /// @name Process Behavior
    /// @brief Attributes related to the behavior of a process.
    /// @{
    ProcessKind processKind;   ///< Kind of the process.
    ResetKind resetKind;       ///< Kind of the reset.
    WorkingEdge workingEdge;   ///< Working edge of the process.
    ResetPhase resetPhase;     ///< Reset phase of the process.
    ProcessStyle processStyle; ///< Style of the process.
    /// @}

    /// @name Sensitivities
    /// @brief Declarations and variables sensitive to various events.
    /// @{
    ReferredDeclarations risingSensitivity;  ///< Declarations sensitive to rising edges.
    ReferredDeclarations fallingSensitivity; ///< Declarations sensitive to falling edges.
    ReferredDeclarations sensitivity;        ///< General sensitivity list.
    /// @}

    /// @name I/O Ports and Variables
    /// @brief Ports and variables used as inputs and outputs in the process.
    /// @{
    ReferredDeclarations inputs;          ///< Input ports.
    ReferredDeclarations outputs;         ///< Output ports.
    ReferredDeclarations inputVariables;  ///< Input variables.
    ReferredDeclarations outputVariables; ///< Output variables.
    /// @}

    /// @name Signals
    /// @brief Clock and reset signals associated with the process.
    /// @{
    DataDeclaration *clock; ///< Clock signal.
    DataDeclaration *reset; ///< Reset signal.
    /// @}

    /// @brief Constructor.
    ProcessInfos();

    /// @brief Destructor.
    ~ProcessInfos();

    /// @brief Copy constructor.
    /// @param other The other instance to copy.
    ProcessInfos(const ProcessInfos &other);

    /// @brief Assignment operator.
    /// @param other The other instance to copy.
    /// @return a reference to this instance.
    auto operator=(const ProcessInfos &other) -> ProcessInfos &;

    /// @brief Gets the size of the sensitivity list.
    /// @return Number of elements in the sensitivity list.
    auto getSensitivitySize() const -> ReferredDeclarations::size_type;

    /// @brief Checks if a declaration is in the sensitivity list.
    /// @param v Declaration to check.
    /// @return True if in sensitivity list, false otherwise.
    auto isInSensitivity(ReferredDeclarations::value_type v) const -> bool;
};

/// @brief Options for analyzing processes.
struct AnalyzeProcessOptions {
    using ProcessMap = std::map<Object *, ProcessInfos>; ///< Map of processes and their infos.

    /// @name Configuration Options
    /// @{
    std::string clock;               ///< Main clock name (default: nullptr).
    std::string reset;               ///< Main reset name (default: nullptr).
    bool skip_standard_declarations; ///< Skip standard declarations (default: true).
    bool printWarnings;              ///< Print warnings (default: false).
    /// @}

    /// @brief Constructor.
    AnalyzeProcessOptions();

    /// @brief Destructor.
    ~AnalyzeProcessOptions();

    /// @brief Copy constructor.
    /// @param other The other instance to copy.
    AnalyzeProcessOptions(const AnalyzeProcessOptions &other);

    /// @brief Assignment operator.
    /// @param other The other instance to copy.
    /// @return a reference to this instance.
    auto operator=(const AnalyzeProcessOptions &other) -> AnalyzeProcessOptions &;
};

} // namespace analysis

} // namespace hif
