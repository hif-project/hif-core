/// @file backendManipulations.hpp
/// @brief Utilities for manipulating and transforming HIF backend data.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{
namespace backends
{

/// @brief Given a method call, assures that all its actual parameters are
/// assignable to the declaration formal parameters.
/// @param call The method call.
/// @param sem The reference semantics.
/// @param shiftToZero True if actual parameters must be shifted to zero.
/// @param skip The number of parameters to not check (starting from the method
///             first parameter).
/// @param removeCastOnOutputs when true remove cast on outputs. Default is false
void makeParametersAssignable(
    ProcedureCall *call,
    hif::semantics::ILanguageSemantics *sem,
    bool shiftToZero,
    unsigned int skip              = 0,
    bool removeCastOnOutputs = false);

/// @brief Given a method call, assures that all its actual parameters are
/// assignable to the declaration formal parameters.
/// @param call The method call.
/// @param sem The reference semantics.
/// @param shiftToZero True if actual parameters must be shifted to zero.
/// @param skip The number of parameters to not check (starting from the method
///             first parameter).
/// /// @param removeCastOnOutputs when true remove cast on outputs. Default is false
void makeParametersAssignable(
    FunctionCall *call,
    hif::semantics::ILanguageSemantics *sem,
    bool shiftToZero,
    unsigned int skip              = 0,
    bool removeCastOnOutputs = false);

/// @brief Given a value, assures that its original type is preserved
/// for fully substitutability.
///
/// @param v The value to be checked.
/// @param valueType The current value type.
/// @param originalType The type to be preserved.
/// @return True if a cast has been added.
///
bool addEventualCast(Value *v, Type *valueType, Type *originalType);

/// @brief Introduces a library and the matching LibraryDef into given scope and system.
/// - The scope is used as starting object to get the actual scope into which add the library.
/// - The given name of the library must be complete. E.g. "hif_systemc_sc_core".
/// @param libName The library name.
/// @param scope The scope into which add the library. If nullptr library is not added.
/// @param system The system into which add the LibraryDef.
/// @param sem The reference semantics.
/// @param standard If <tt>true</tt> the inclusion is created as standard.
/// @return True if the library has been added.
bool addHifLibrary(
    const std::string &libName,
    Object *scope,
    System *system,
    hif::semantics::ILanguageSemantics *sem,
    bool standard = false);

/// @brief Calculate the include path from given <tt>where</tt> scope to
/// <tt>toInclude</tt> scope.
/// @param where The starting scope.
/// @param toInclude The scope to be reached.
/// @param cppHeaderExtension The c++ header extension.
/// @param sem The semantics.
/// @return The include path.
/// @todo Added language scope strings and separators.
std::string calculateIncludePath(
    Scope *where,
    Scope *toInclude,
    const std::string &cppHeaderExtension,
    hif::semantics::ILanguageSemantics *sem);

} // namespace backends
} // namespace hif
