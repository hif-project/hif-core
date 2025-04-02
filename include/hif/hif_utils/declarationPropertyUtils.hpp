/// @file declarationPropertyUtils.hpp
/// @brief Utilities for managing HIF declaration properties.
/// @details
/// Provides functionality to check declaration properties such as standard
/// compliance, instance status, and parameter dependencies. Includes
/// customizable options for flexibility.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Options for checking if a declaration is part of a standard component.
/// @details
/// Includes flags for enabling or disabling checks for specific languages and libraries.
struct DeclarationIsPartOfStandardOptions {
    /// @brief Default constructor.
    DeclarationIsPartOfStandardOptions();

    /// @brief Destructor.
    ~DeclarationIsPartOfStandardOptions();

    /// @brief Copy constructor.
    DeclarationIsPartOfStandardOptions(const DeclarationIsPartOfStandardOptions &other);

    /// @brief Assignment operator.
    DeclarationIsPartOfStandardOptions &operator=(DeclarationIsPartOfStandardOptions other);

    /// @brief Swaps the contents with another options object.
    void swap(DeclarationIsPartOfStandardOptions &other);

    /// @brief Resets all flags to defaults (all disabled).
    void reset();

    /// @brief Checks if all allow flags are enabled.
    bool areAllAllowFlagsSet() const;

    /// @brief Enables or disables C/C++ language checks.
    /// @param enable Flag to enable or disable.
    void enableCppFamily(bool enable);

    /// @brief Enables or disables SystemC RTL/TLM checks.
    /// @param enable Flag to enable or disable.
    void enableSystemCFamily(bool enable);

    /// @brief Enables or disables SystemC RTL/TLM/AMS checks.
    /// @param enable Flag to enable or disable.
    void enableSystemCAMSFamily(bool enable);

    /// @brief Enables or disables VHDL language checks.
    /// @param enable Flag to enable or disable.
    void enableVhdlFamily(bool enable);

    /// @brief Enables or disables Verilog language checks.
    /// @param enable Flag to enable or disable.
    void enableVerilogFamily(bool enable);

    /// Flags for language and library checks.
    bool dontCheckStandardViews; ///< Do not check standard views. Default: false.
    bool allowSystemCRTL;        ///< Allow SystemC RTL libraries. Default: true.
    bool allowSystemCTLM;        ///< Allow SystemC TLM libraries. Default: true.
    bool allowSystemCAMS;        ///< Allow SystemC AMS libraries. Default: true.
    bool allowC;                 ///< Allow C libraries. Default: true.
    bool allowCpp;               ///< Allow C++ libraries. Default: true.
    bool allowCppExtraLibs;      ///< Allow non-standard C++ libraries. Default: true.
    bool allowVhdl;              ///< Allow VHDL libraries. Default: true.
    bool allowVhdlPSL;           ///< Allow VHDL PSL libraries. Default: true.
    bool allowVerilog;           ///< Allow Verilog libraries. Default: true.
    bool allowVerilogAMS;        ///< Allow Verilog AMS libraries. Default: true.

    hif::semantics::ILanguageSemantics *sem; ///< Language semantics to use. Default: HIF.
};

/// @brief Checks if a declaration is an instance.
/// @param obj The declaration to check.
/// @param scope The scope to use, if applicable.
/// @return True if the declaration is an instance.

bool declarationIsInstance(Declaration *obj, Object *scope);

/// @brief Checks if a declaration is part of a standard component.
/// @param decl The declaration to check.
/// @param opts Options for customizing the check.
/// @return True if the declaration is part of a standard component.

bool declarationIsPartOfStandard(
    Declaration *decl,
    const DeclarationIsPartOfStandardOptions &opts = DeclarationIsPartOfStandardOptions());

/// @brief Checks if the declaration of a symbol is part of a standard component.
/// @param symb The symbol to check.
/// @param sem The language semantics to use.
/// @param opts Options for customizing the check.
/// @return True if the symbol's declaration is part of a standard component.

bool declarationIsPartOfStandard(
    Object *symb,
    hif::semantics::ILanguageSemantics *sem,
    const DeclarationIsPartOfStandardOptions &opts = DeclarationIsPartOfStandardOptions());

/// @brief Type for parameters influencing signature return types.
/// @see signatureDependsOnActualParameters
typedef std::set<Parameter *> DependingParameters;

/// @brief Checks if a signature depends on actual parameters.
/// @details Determines if the signature's return type is influenced by the actual parameters.
/// @param decl The signature to check.
/// @param dependingParams Set of parameters influencing the signature.
/// @return True if the signature depends on actual parameters.

bool signatureDependsOnActualParameters(SubProgram *decl, DependingParameters &dependingParams);

} // namespace hif
