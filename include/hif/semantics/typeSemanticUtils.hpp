/// @file typeSemanticUtils.hpp
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

/// @brief Function that check if a type is a Signed.
///
/// @param type actual type
/// @param refLang The semantics.
/// @return <tt>true</tt> if <tt>type</tt> is a signed type,
///	0 otherwise
///

bool isSignedType(Type *type, ILanguageSemantics *refLang);

/// @brief Check if a type is an Unsigned.
///
/// @param type actual type
/// @param refLang The semantics.
/// @return <tt>true</tt> if <tt>type</tt> is a unsigned type,
/// false otherwise
///

bool isUnsignedType(Type *type, ILanguageSemantics *refLang);

/// @brief Check if a type is an Bit.
///
/// @param type actual type
/// @param refLang The semantics.
/// @return <tt>true</tt> if <tt>type</tt> is a bit type (with or without logic flag),
/// false otherwise
/// @attention not check logic flag, use isLogicBit if you want check logic flag.
///

bool isBitType(Type *type, ILanguageSemantics *refLang);

/// @brief Check if a type is an Array of Bit.
///
/// @param type actual type
/// @param refLang The semantics.
/// @return <tt>true</tt> if <tt>type</tt> is an array of bit type (with or without logic flag),
/// false otherwise
/// @attention not check logic flag check only packed flag, use isLogicBitVector if you
/// want check logic flag.
///

bool isBitVectorType(Type *type, ILanguageSemantics *refLang);

/// @brief Check if a type is a logic Bit.
///
/// @param type actual type
/// @param refLang The semantics.
/// @return <tt>true</tt> if <tt>type</tt> is a logic bit type (with logic flag set),
/// false otherwise
///

bool isLogicBitType(Type *type, ILanguageSemantics *refLang);

/// @brief Check if a type is an Array of logic Bit.
///
/// @param type actual type
/// @param refLang The semantics.
/// @return <tt>true</tt> if <tt>type</tt> is an array of logic bit type (with logic flag set),
/// false otherwise
///

bool isLogicVectorType(Type *type, ILanguageSemantics *refLang);

/// @brief Check if given type is a vector type (with range).
///
/// @param type given type.
/// @param refLang The semantics.
/// @return <tt>true</tt> if <tt>type</tt> is a vector type,
/// false otherwise.
///

bool isVectorType(Type *type, ILanguageSemantics *refLang);

/// @brief Check if a type is represented on a single Bit.
///
/// @param type Actual type
/// @param sem The semantics.
/// @return <tt>true</tt> if <tt>type</tt> is a bit, false otherwise
///

bool isSingleBitType(Type *type, ILanguageSemantics *sem);

/// @brief Checks if a type is a subtype of another.
///
///	@param t1 first type
/// @param t2 second type
/// @param refLang The semantics.
/// @param compareSpan If true compare the eventual span of types.
/// @param considerOpacity If true consider opacity of types.
/// @return <tt>true</tt> if <tt>t1</tt> is a subtype of <tt>t2</tt>.
///

bool isSubType(
    Type *t1,
    Type *t2,
    ILanguageSemantics *refLang,
    bool compareSpan     = true,
    bool considerOpacity = false);

/// @brief Checks if <tt>t</tt> is set as a semantics type of a typedObject.
///
/// @param t The Type to analyse.
/// @return true if <tt>t</tt> is set as a semantics type of a typedObject,
/// false otherwise.
///

bool isSemanticsType(const Type *t);

} // namespace semantics
} // namespace hif
