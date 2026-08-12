/// @file instanceUtils.hpp
/// @brief Provides utilities for instantiating objects in the HIF model.
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

/// @name Instantiation-stuff: methods to instantiate objects.
/// @warning All returned objects are by aliasing. Thus, if you need to manipulate them,
/// you must make a copy.
/// @{

/// @brief Flushes the cache used to optimize performance with instantiations.

void flushInstanceCache();

/// @brief Options for controlling instantiation behavior.
struct InstantiateOptions {
    /// @brief Default constructor initializing options.
    InstantiateOptions();

    /// @brief Destructor.
    ~InstantiateOptions();

    /// @brief Copy constructor.
    /// @param other Options to copy from.
    InstantiateOptions(const InstantiateOptions &other);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param other Options to assign from.
    /// @return Reference to the assigned options.
    InstantiateOptions &operator=(const InstantiateOptions &other);

    bool replace;          ///< Replace the result if true. Default: false.
    bool onlySignature;    ///< Instantiate only the signature, skipping cache. Default: false.
    SubProgram *candidate; ///< Candidate method for instantiation, if applicable.
};

/// @brief Instantiates a ViewReference, resolving template parameters.
/// @param instance The view reference to instantiate.
/// @param ref_sem The semantics to use (default: HIFSemantics instance).
/// @param opt Options for instantiation (default: InstantiateOptions()).
/// @return The instantiated view.

View *instantiate(
    ViewReference *instance,
    hif::semantics::ILanguageSemantics *ref_sem = hif::semantics::HIFSemantics::getInstance(),
    const InstantiateOptions &opt               = InstantiateOptions());

/// @brief Instantiates a ProcedureCall, resolving template parameters.
/// @param originalCall The call to instantiate.
/// @param ref_sem The semantics to use (default: HIFSemantics instance).
/// @param opt Options for instantiation (default: InstantiateOptions()).
/// @return The instantiated subprogram signature.

SubProgram *instantiate(
    ProcedureCall *originalCall,
    hif::semantics::ILanguageSemantics *ref_sem = hif::semantics::HIFSemantics::getInstance(),
    const InstantiateOptions &opt               = InstantiateOptions());

/// @brief Instantiates a FunctionCall, resolving template parameters.
/// @param originalCall The call to instantiate.
/// @param ref_sem The semantics to use (default: HIFSemantics instance).
/// @param opt Options for instantiation (default: InstantiateOptions()).
/// @return The instantiated function signature.

Function *instantiate(
    FunctionCall *originalCall,
    hif::semantics::ILanguageSemantics *ref_sem = hif::semantics::HIFSemantics::getInstance(),
    const InstantiateOptions &opt               = InstantiateOptions());

/// @brief Instantiates a TypeReference, resolving template parameters.
/// @param instance The type reference to instantiate.
/// @param ref_sem The semantics to use (default: HIFSemantics instance).
/// @param opt Options for instantiation (default: InstantiateOptions()).
/// @return The instantiated typedef.

TypeReference::DeclarationType *instantiate(
    TypeReference *instance,
    hif::semantics::ILanguageSemantics *ref_sem = hif::semantics::HIFSemantics::getInstance(),
    const InstantiateOptions &opt               = InstantiateOptions());

/// @brief Instantiates a PortAssign, resolving template parameters.
/// @param instance The port assignment to instantiate.
/// @param ref_sem The semantics to use (default: HIFSemantics instance).
/// @param opt Options for instantiation (default: InstantiateOptions()).
/// @return The instantiated port.

Port *instantiate(
    PortAssign *instance,
    hif::semantics::ILanguageSemantics *ref_sem = hif::semantics::HIFSemantics::getInstance(),
    const InstantiateOptions &opt               = InstantiateOptions());

/// @brief Instantiates a ParameterAssign, resolving template parameters.
/// @param instance The parameter assignment to instantiate.
/// @param ref_sem The semantics to use (default: HIFSemantics instance).
/// @param opt Options for instantiation (default: InstantiateOptions()).
/// @return The instantiated parameter.

Parameter *instantiate(
    ParameterAssign *instance,
    hif::semantics::ILanguageSemantics *ref_sem = hif::semantics::HIFSemantics::getInstance(),
    const InstantiateOptions &opt               = InstantiateOptions());

/// @brief Instantiates a FieldReference, resolving template parameters.
/// @param instance The field reference to instantiate.
/// @param ref_sem The semantics to use (default: HIFSemantics instance).
/// @param opt Options for instantiation (default: InstantiateOptions()).
/// @return The instantiated field reference.

FieldReference::DeclarationType *instantiate(
    FieldReference *instance,
    hif::semantics::ILanguageSemantics *ref_sem = hif::semantics::HIFSemantics::getInstance(),
    const InstantiateOptions &opt               = InstantiateOptions());

/// @brief Instantiates an Instance, resolving template parameters.
/// @param instance The instance to instantiate.
/// @param ref_sem The semantics to use (default: HIFSemantics instance).
/// @param opt Options for instantiation (default: InstantiateOptions()).
/// @return The instantiated entity.

Instance::DeclarationType *instantiate(
    Instance *instance,
    hif::semantics::ILanguageSemantics *ref_sem = hif::semantics::HIFSemantics::getInstance(),
    const InstantiateOptions &opt               = InstantiateOptions());

/// @brief Instantiates a ValueTPAssign, resolving template parameters.
/// @param instance The ValueTPAssign to instantiate.
/// @param ref_sem The semantics to use (default: HIFSemantics instance).
/// @param opt Options for instantiation (default: InstantiateOptions()).
/// @return The instantiated ValueTP.

ValueTPAssign::DeclarationType *instantiate(
    ValueTPAssign *instance,
    hif::semantics::ILanguageSemantics *ref_sem = hif::semantics::HIFSemantics::getInstance(),
    const InstantiateOptions &opt               = InstantiateOptions());

/// @brief Instantiates a TypeTPAssign, resolving template parameters.
/// @param instance The TypeTPAssign to instantiate.
/// @param ref_sem The semantics to use (default: HIFSemantics instance).
/// @param opt Options for instantiation (default: InstantiateOptions()).
/// @return The instantiated TypeTP.

TypeTPAssign::DeclarationType *instantiate(
    TypeTPAssign *instance,
    hif::semantics::ILanguageSemantics *ref_sem = hif::semantics::HIFSemantics::getInstance(),
    const InstantiateOptions &opt               = InstantiateOptions());

/// @brief Instantiates a ReferencedType, resolving template parameters.
/// @param instance The ReferencedType to instantiate.
/// @param ref_sem The semantics to use (default: HIFSemantics instance).
/// @param opt Options for instantiation (default: InstantiateOptions()).
/// @return The instantiated scope.

Scope *instantiate(
    ReferencedType *instance,
    hif::semantics::ILanguageSemantics *ref_sem = hif::semantics::HIFSemantics::getInstance(),
    const InstantiateOptions &opt               = InstantiateOptions());

/// @brief Checks if an object is stored in the instance cache.
/// @param obj The object to look for.
/// @return True if the object is in the cache, false otherwise.

bool isInCache(Object *obj);

/// @brief Adds a declaration to the cache, which is cleared by flushInstanceCache().
/// @param o The declaration to add.

void addInCache(Declaration *o);

/// @}

} // namespace manipulation
} // namespace hif
