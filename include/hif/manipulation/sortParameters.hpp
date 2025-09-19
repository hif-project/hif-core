/// @file sortParameters.hpp
/// @brief Provides utilities for sorting parameters and handling missing parameters.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{
namespace semantics
{
class ILanguageSemantics;

}
} // namespace hif

namespace hif
{
namespace manipulation
{

/// @name Parameter Sorting and Missing Parameter Handling
/// @{

/// @brief Retrieves the implicitly assigned object for a template parameter.
/// @param tp The template parameter declaration.
/// @param formalParameterType The reference tree for template parameter references.
/// @param actualParameterType The tree to match against the formal parameter type.
/// @param sem Pointer to the language semantics interface.
/// @param hasCandidate If true, suppresses errors for unresolved matches.
/// @return The matched object or nullptr if no match is found.
Object *getImplicitTemplate(
    Declaration *tp,
    Object *formalParameterType,
    Object *actualParameterType,
    hif::semantics::ILanguageSemantics *sem,
    bool hasCandidate);

/// @brief Specifies how to handle missing parameters during sorting.
struct SortMissingKind {
    /// @brief Enumeration of strategies for handling missing parameters.
    enum type : unsigned char {
        NOTHING, ///< Do not add missing parameters.
        ALL,     ///< Add all missing parameters.
        LIMITED  ///< Add missing parameters until the last assigned parameter; others use default values.
    };
};

/// @brief Sorts actual parameters to align with formal parameters.
/// @details Resolves positional or named binding of arguments, adding missing parameters as needed.
/// @param actualParams List of actual parameters.
/// @param formalParams List of formal parameters.
/// @param set_formal_names If true, sets the names of formal parameters in actual parameters.
/// @param missingType Specifies how to handle missing parameters.
/// @param refSem Pointer to the reference semantics interface.
/// @param hasCandidate If true, relaxes some checks for implicit parameters.
/// @return True if sorting is successful, false if unresolved parameters are found.

bool sortParameters(
    BList<ParameterAssign> &actualParams,
    BList<Parameter> &formalParams,
    bool set_formal_names,
    const SortMissingKind::type missingType,
    hif::semantics::ILanguageSemantics *refSem,
    bool hasCandidate = false);

/// @brief Sorts actual template parameters to align with formal template parameters.
/// @details Resolves positional or named binding of template arguments.
/// @param actualParams List of actual template parameters.
/// @param formalParams List of formal template parameters.
/// @param set_formal_names If true, sets the names of formal template parameters in actual template parameters.
/// @param missingType Specifies how to handle missing template parameters.
/// @param refSem Pointer to the reference semantics interface.
/// @param hasCandidates If true, relaxes some checks for implicit parameters.
/// @return True if sorting is successful, false if unresolved parameters are found.

bool sortParameters(
    BList<TPAssign> &actualParams,
    BList<Declaration> &formalParams,
    bool set_formal_names,
    const SortMissingKind::type missingType,
    hif::semantics::ILanguageSemantics *refSem,
    bool hasCandidates = false);

/// @brief Sorts port bindings to align with formal port definitions.
/// @details Resolves positional or named binding of port connections.
/// @param actualParams List of actual port bindings.
/// @param formalParams List of formal port definitions.
/// @param set_formal_names If true, sets the formal port names in actual port bindings.
/// @param missingType Specifies how to handle missing ports.
/// @param refSem Pointer to the reference semantics interface.
/// @return True if sorting is successful, false if unresolved ports are found.

bool sortParameters(
    BList<PortAssign> &actualParams,
    BList<Port> &formalParams,
    bool set_formal_names,
    const SortMissingKind::type missingType,
    hif::semantics::ILanguageSemantics *refSem);

/// @}
/// End of parameter sorting utilities.

} // namespace manipulation
} // namespace hif
