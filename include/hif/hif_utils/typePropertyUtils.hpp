/// @file typePropertyUtils.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @name Type utility stuff.
/// Methods that help to set or get specific field of Hif types.
///
///	@{

/// @brief Return if the generic type has flag constexpr set to true.
/// @param t The type from which take constexpr flag.
/// @param refLang The semantics.
/// @return True if constexpr flag is true, false otherwise.
///

bool typeIsConstexpr(Type *t, hif::semantics::ILanguageSemantics *refLang);

/// @brief Return if the generic type has flag signed set to true.
/// @param to The typeObject from which take signed flag.
/// @param refLang The semantics.
/// @param skip Skip recursion on TypeReferences.
/// @return True if signed flag is true, false otherwise.
///

bool typeIsSigned(Type *to, hif::semantics::ILanguageSemantics *refLang, const bool skip = false);

/// @brief Return if the generic type has flag logic set to true.
/// @param to The typeObject from which take logic flag.
/// @param refLang The semantics.
/// @return True if logic flag is true, false otherwise.
///

bool typeIsLogic(Type *to, hif::semantics::ILanguageSemantics *refLang);

/// @brief Return if the generic type has flag resolved set to true.
/// @param to The typeObject from which take resolved flag.
/// @param refLang The semantics.
/// @return True if resolved flag is true, false otherwise.
///

bool typeIsResolved(Type *to, hif::semantics::ILanguageSemantics *refLang);

/// @brief The function retrieves the range specified into
///	the Type given as parameter (if such Type provides a range),
/// nullptr otherwise.
/// @param to The Type analyzed to retrieve the range.
/// @param sem The reference semantics.
/// @param manageStrings If true return also the spanInformation of String objects.
/// Default is false.
/// @return the Range provided by to, nullptr otherwise.
///

Range *typeGetSpan(Type *to, hif::semantics::ILanguageSemantics *sem, const bool manageStrings = false);

/// @brief Set constexpr flag of type if the type has this flag.
/// @param type The typeObject.
/// @param v the value to set.
/// @return <tt>true</tt> if the property has been successfully set,
/// <tt>false</tt> otherwise.
///

bool typeSetConstexpr(Type *type, const bool v);

/// @brief Set sign flag of type if the type has this flag.
/// @param type The typeObject.
/// @param sign the value to set.
/// @param refLang The semantics.
/// @return <tt>true</tt> if the property has been successfully set,
/// <tt>false</tt> otherwise.
///

bool typeSetSigned(Type *type, const bool sign, hif::semantics::ILanguageSemantics *refLang);

/// @brief Set logic flag of type if the type has this flag.
/// @param type The typeObject.
/// @param logic the value to set.
/// @param refLang The semantics.
/// @return <tt>true</tt> if the property has been successfully set,
/// <tt>false</tt> otherwise.
///

bool typeSetLogic(Type *type, const bool logic, hif::semantics::ILanguageSemantics *refLang);

/// @brief Set resolved flag of type if the type has this flag.
/// @param type The typeObject.
/// @param resolved the value to set.
/// @param refLang The semantics.
/// @return <tt>true</tt> if the property has been successfully set,
/// <tt>false</tt> otherwise.
///

bool typeSetResolved(Type *type, const bool resolved, hif::semantics::ILanguageSemantics *refLang);
/// @brief Function that set range given as parameter to type,
///	if type allowed range specification.
///	@param to type to set the range
/// @param ro the range to set
/// @param refLang The semantics.
/// @param deleteIfNotSet delete passed if not set.
/// @param manageStrings If true return also the spanInformation of String objects.
/// Default is false.
/// @return <tt>true</tt> if the property has been successfully set,
/// <tt>false</tt> otherwise.
///

bool typeSetSpan(
    Type *to,
    Range *ro,
    hif::semantics::ILanguageSemantics *refLang,
    const bool deleteIfNotSet = false,
    const bool manageStrings  = false);

/// @brief Function that first find base type of <tt>to</tt>
/// and in a second time find the cardinality of the canonicalized <tt>t</tt>.
/// For example, given sc_lv<3> a[5] returns: 2.
///
/// @param type starting Type pointer.
/// @param refLang The semantics.
/// @param considerOnlyBits <tt>true</tt> if only bit types are to be considered,
///        <tt>false</tt> otherwise.
/// @return cardinality of <tt>t</tt>:
///	- 0 if base type of <tt>t</tt> is not an Array type.
/// - n if base type of <tt>t</tt> is an Array type of depth n.
///

unsigned int
typeGetCardinality(Type *type, hif::semantics::ILanguageSemantics *refLang, const bool considerOnlyBits = true);

/// @brief Returns the inner type of mono/multi-dimensional composite type.
///
/// @param t The Type to analyze.
/// @param refLang The semantics.
/// @param depth The requested depth, if any (default = -1).
/// @return The inner type of in case of composite, nullptr otherwise.
///

Type *typeGetNestedType(Type *t, hif::semantics::ILanguageSemantics *refLang, std::int64_t depth = -1);

/// @brief Checks whether givent type has symbols referring to templates.
///
/// @param t The Type to analyze.
/// @param sem The semantics.
/// @return True if type depends on templates.
///

bool typeDependsOnTemplates(Type *t, hif::semantics::ILanguageSemantics *sem);

///	@}

} // namespace hif
