/// @file removeUnusedDeclarations.hpp
/// @brief Provides functionality for removing unused declarations.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/semantics/HIFSemantics.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Options for controlling the removal of unused declarations.
struct RemoveUnusedDeclarationOptions {
    /// @name Traits
    /// @{

    typedef std::set<Declaration *> DeclarationSet;           ///< Set of declarations.
    typedef std::set<Object *> ObjectSet;                     ///< Set of objects.
    typedef std::map<Declaration *, ObjectSet> ReferencesMap; ///< Map of declarations to their references.

    /// @}
    /// @name Configuration options
    /// @{

    bool skipStandardLibs;  ///< Skip standard library definitions and views. Default: false.
    bool skipStandardViews; ///< Skip standard views outside standard library definitions. Default: false.
    bool removeAll;         ///< Remove all unused declarations. Default: true.
    bool removeConstants;   ///< Remove unused constants. Default: false.
    bool removeParams;      ///< Remove unused parameters. Default: false.
    bool removeSubPrograms; ///< Remove unused subprograms. Default: false.
    bool removeTypeDefs;    ///< Remove unused typedefs. Default: false.
    bool removeTypeTPs;     ///< Remove unused TypeTPs. Default: false.
    bool removeValueTPs;    ///< Remove unused ValueTPs. Default: false.
    bool removeVariables;   ///< Remove unused variables. Default: false.
    bool removeView;        ///< Remove unused views. Default: false.
    bool removeAlias;       ///< Remove unused aliases. Default: false.
    bool removeEnumValue;   ///< Remove unused enum values. Default: false.
    bool removeField;       ///< Remove unused fields. Default: false.
    bool removeLibraryDef;  ///< Remove unused library definitions. Default: false.
    bool removePort;        ///< Remove unused ports. Default: false.
    bool removeSignal;      ///< Remove unused signals. Default: false.
    bool removeStateTable;  ///< Remove unused state tables. Default: false.

    /// @}
    /// @name Optional info
    /// @{

    View *top;                          ///< Reference to the top module of the design.
    DeclarationSet *declarationsTarget; ///< Optional list of declarations to check.
    ReferencesMap *allReferencesMap;    ///< Optional map of all references.

    /// @}

    /// @brief Default constructor initializing options.
    RemoveUnusedDeclarationOptions();

    /// @brief Destructor.
    ~RemoveUnusedDeclarationOptions();

    /// @brief Copy constructor.
    /// @param other Options to copy from.
    RemoveUnusedDeclarationOptions(const RemoveUnusedDeclarationOptions &other);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param other Options to assign from.
    /// @return Reference to the assigned options.
    RemoveUnusedDeclarationOptions &operator=(const RemoveUnusedDeclarationOptions &other);
};

/// @brief Removes unused declarations from the HIF tree.
/// @details Analyzes references in the tree and removes declarations that are not referenced.
/// @param root The root of the system to analyze.
/// @param sem Pointer to the language semantics interface (default: HIFSemantics instance).
/// @param options Options controlling the behavior of the operation (default: RemoveUnusedDeclarationOptions()).
/// @return True if unused declarations were successfully removed, false otherwise.

bool removeUnusedDeclarations(
    System *root,
    hif::semantics::ILanguageSemantics *sem       = hif::semantics::HIFSemantics::getInstance(),
    const RemoveUnusedDeclarationOptions &options = RemoveUnusedDeclarationOptions());

} // namespace manipulation
} // namespace hif
