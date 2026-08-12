/// @file flattenDesign.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/semantics/ILanguageSemantics.hpp"

namespace hif
{
namespace manipulation
{

/// @brief flattenDesign() options.
struct FlattenDesignOptions {
    /// @brief Verbose output flag.
    bool verbose;
    /// @brief The name of the top-level design unit.
    std::string topLevelName;
    /// @brief The set of names of root design units, as hierarchical names.
    std::set<std::string> rootDUs;
    /// @brief The set of names of root instances.
    /// Their format is the hierarchical name.
    std::set<std::string> rootInstances;
};

/// @brief Flattens the given system description (i.e., removes its hierarchy).
/// Selective flattening is available by specifying root design units and/or instances
/// in the dedicated fields of the options structure.
/// @param system The system object to be flattened.
/// @param sem The reference semantics.
/// @param opt The options.

void flattenDesign(
    hif::System *system,
    hif::semantics::ILanguageSemantics *sem,
    const FlattenDesignOptions &opt = FlattenDesignOptions());

} // namespace manipulation
} // namespace hif
