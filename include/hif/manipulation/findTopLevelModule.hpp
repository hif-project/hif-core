/// @file findTopLevelModule.hpp
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

/// @brief A set of pointers to `View` objects, representing top-level modules.
typedef std::set<View *> ViewSet;

/// @brief Options for configuring the behavior of top-level module finding functions.
struct FindTopOptions {
    /// @brief Enables verbose logging. Default is `false`.
    bool verbose;

    /// @brief Specifies the expected name of the top-level design unit.
    /// If a different name is found, an error is raised. Default is an empty string.
    std::string topLevelName;

    /// @brief A pointer to a map storing sub-module views, used for performance optimization
    /// if `findViewDependencies()` has already been called. Default is `nullptr`.
    std::map<hif::View *, std::set<hif::View *>> *smm;

    /// @brief A pointer to a map storing parent-module views, used for performance optimization
    /// if `findViewDependencies()` has already been called. Default is `nullptr`.
    std::map<hif::View *, std::set<hif::View *>> *pmm;

    /// @brief If `true`, the method uses heuristics to determine the top-level module when
    /// more than one candidate is found. Default is `false`.
    bool useHeuristics;

    /// @brief If `true`, raises an error if more than one top-level design unit is found. Default is `false`.
    bool checkAtMostOne;

    /// @brief If `true`, raises an error if no top-level design unit is found. Default is `false`.
    bool checkAtLeastOne;

    /// @brief Default constructor initializing all options to their default values.
    FindTopOptions();

    /// @brief Destructor.
    ~FindTopOptions();

    /// @brief Copy constructor.
    /// @param other The `FindTopOptions` instance to copy.
    FindTopOptions(const FindTopOptions &other);

    /// @brief Assignment operator.
    /// @param other The `FindTopOptions` instance to assign.
    /// @return A reference to this `FindTopOptions` instance.
    FindTopOptions &operator=(const FindTopOptions &other);
};

/// @brief Identifies the top-level modules in a given system description.
/// @details Traverses the system description starting from the root object
/// and applies the given options to determine the top-level views.
///
/// @param root The root `System` object to analyze.
/// @param sem A pointer to the reference semantics.
/// @param opt The options specifying the criteria for determining top-level views.
/// @return A set of pointers to the identified top-level views.

ViewSet findTopLevelModules(
    hif::System *root,
    hif::semantics::ILanguageSemantics *sem,
    const FindTopOptions &opt = FindTopOptions());

/// @brief Identifies the single top-level module in a given system description.
/// @details Similar to `findTopLevelModules`, but expects to find exactly one top-level view
/// (based on the provided options). Returns a single pointer instead of a set.
///
/// @param root The root `System` object to analyze.
/// @param sem A pointer to the reference semantics.
/// @param opt The options specifying the criteria for determining the top-level view.
/// @return A pointer to the identified top-level view, or `nullptr` if no valid view is found.

View *findTopLevelModule(
    hif::System *root,
    hif::semantics::ILanguageSemantics *sem,
    const FindTopOptions &opt = FindTopOptions());

} // namespace manipulation
} // namespace hif
