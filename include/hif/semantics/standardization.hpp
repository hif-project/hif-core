/// @file standardization.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/hif.hpp"

namespace hif
{
namespace semantics
{

/// @brief A map from Cast pointers to Type pointers.
typedef std::map<Cast *, Type *> CastMap;

/// @brief This function invoke HifStdVistor used to standardize HIF code.
///
/// @param o The root object from which start standardization.
/// @param source The source semantics.
/// @param dest The destination semantics.
/// @return The standardized object.
///
System *standardizeHif(
    System *o,
    hif::semantics::ILanguageSemantics *source,
    hif::semantics::ILanguageSemantics *dest,
    CastMap &castMap);
/// @brief Map the symbols (and objects) contained in the tree starting from root
/// and involved in the standard libraries with respect to the destination semantics.
///
/// @param o The root tree to manage.
/// @param srcSem The source semantics.
/// @param destSem The destination semantics.
void mapStandardSymbols(Object *o, ILanguageSemantics *srcSem, ILanguageSemantics *destSem);
/// @brief Explicit casts and bool conversions.
///
/// @param o The tree to manage.
/// @param source The source semantics.
/// @param dest The semantics to be used to typing the tree and to check
///        condition validity.
///
void manageCasts(Object *o, ILanguageSemantics *source, ILanguageSemantics *dest, CastMap &castMap);
/// @brief Used as return type for type precision comparisons
///
enum precision_type_t : unsigned char {
    LESS_PRECISION,        ///@< first operand is less precise
    GREATER_PRECISION,     ///@< first operand is more precise
    EQUAL_PRECISION,       ///@< first operand is equally precise
    UNCOMPARABLE_PRECISION ///@< comparison not supported
};
/// @brief This function compare the precision of two types.
///
/// If type t1 has LESS_PRECISION than t2, is safe to cast a variable of
/// type t1 to type t2. For example, a bit_vector (3 downto 0) has less
/// precision than a bit_vector (4 downto 0).
///
/// @param t1 the first type
/// @param t2 the second type
/// @param sem The reference semantics
/// @return the comparison between the two types
///
precision_type_t comparePrecision(Type *t1, Type *t2, hif::semantics::ILanguageSemantics *sem);
} // namespace semantics
} // namespace hif
