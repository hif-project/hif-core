/// @file getBaseType.hpp
/// @brief
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

/// @brief Canonicalize <tt>t</tt>, that is, it replaces typedefs with their definitions.
///	It returns the base type of \p t if \p t is a type reference, \p t
///	itself otherwise.
/// NOTE: Typerefs declarations are instantiated.
///
/// @param t actual Type
/// @param consider_opacity if <tt>true</tt> method consider the opacity
/// 	of typeDefObjects.
/// @param refSem The reference semantics
/// @param compositeRecurse Tells whether visit a composite type deeply.
/// @return <tt>t</tt> if it is already a base type, or the definition of <tt>t</tt> if it
/// is a TypeReference.
///

Type *getBaseType(Type *t, bool consider_opacity, ILanguageSemantics *refSem, bool compositeRecurse = true);

/// @brief Try to find the base type of the Value v according with
/// semantics passed as parameter.
///
/// @warning If during manipulation the reference semantics changes
/// its necessary to call resetType() function to calculate the correct
/// type. This function, in fact, calculate the type of <tt>v</tt> only
/// if it's not already set.
///
/// @param v the starting TypedObject
/// @param considerOpacity if <tt>true</tt> method consider the opacity
/// 	of typeDefObjects.
/// @param refSem The reference semantics
/// @param compositeRecurse Tells whether visit a composite type deeply.
/// @param error If true, rise error if can not type a typed object.
///
/// @return type of <tt>v</tt> if it's possible to establish it, nullptr otherwise
///

Type *getBaseType(
    TypedObject *v,
    bool considerOpacity,
    ILanguageSemantics *refSem,
    bool compositeRecurse = true,
    bool error            = false);

} // namespace semantics
} // namespace hif
