/// @file getSemanticType.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/manipulation/prefixTree.hpp"
#include "hif/semantics/HIFSemantics.hpp"

namespace hif
{
namespace semantics
{

/// @brief Try to find the type of the Value v according with
/// semantics passed as parameter. The default semantics is HIF.
///
/// @warning If during manipulation the reference semantics changes
/// its necessary to call resetType() function to calculate the correct
/// type. This function, in fact, calculate the type of <tt>v</tt> only
/// if it's not already set.
///
/// @param v the starting TypedObject
/// @param ref_sem The reference semantics.
/// @param error If true, rise error if can not type a typed object.
///
/// @return type of <tt>v</tt> if it's possible to establish it, nullptr otherwise
///

Type *
getSemanticType(TypedObject *v, ILanguageSemantics *ref_sem = HIFSemantics::getInstance(), bool error = false);

/// @brief Starting from given <tt>root</tt> node, type all object that has
/// semantics type. If option <tt>error</tt> is true, rise error if can not type
/// a typed object.
///
/// @param root The root node.
/// @param ref_sem The reference semantics.
/// @param error If true, rise error if can not type a typed object.
///

void typeTree(Object *root, ILanguageSemantics *ref_sem = HIFSemantics::getInstance(), bool error = false);

/// @brief Starting from given <tt>root</tt> node, type all object that has
/// semantics type. If option <tt>error</tt> is true, rise error if can not type
/// a typed object.
///
/// @param root The root node.
/// @param ref_sem The reference semantics.
/// @param error If true, rise error if can not type a typed object.
///

void typeTree(BList<Object> &root, ILanguageSemantics *ref_sem = HIFSemantics::getInstance(), bool error = false);

/// @brief Starting from given <tt>root</tt> node, type all object that has
/// semantics type. If option <tt>error</tt> is true, rise error if can not type
/// a typed object.
///
/// @param root The root node.
/// @param ref_sem The reference semantics.
/// @param error If true, rise error if can not type a typed object.
///
template <typename T>
void typeTree(BList<T> &root, ILanguageSemantics *ref_sem = HIFSemantics::getInstance(), bool error = false);

/// @brief Flushes the cache of semantic types.

void flushTypeCacheEntries();

/// @brief Given an object, check if it is in type cache.
/// @param obj The object to be checked.
/// @return True if object is in cache.

bool isInTypeCache(Object *obj);

/// @brief Given an object, it is added into the type cache.
/// @param obj The object to be inserted.

void addInTypeCache(Object *obj);

/// @brief Given a type returns the a copy of it with all prefixed symbols.
/// @param t The type to be prefixed.
/// @param sem The semantics.
/// @param opt The optional prefix tree options.
/// @param context The optional context to be used for prefix the symbols.
/// If not passed type symbols is used also how context. When the context is given,
/// internal symbols are simplified w.r.t. it.
/// @return A copy of given type with all prefixed symbols.

Type *getPrefixedType(
    Type *t,
    ILanguageSemantics *sem,
    const hif::manipulation::PrefixTreeOptions &opt = hif::manipulation::PrefixTreeOptions(),
    Object *context                                 = nullptr);

} // namespace semantics
} // namespace hif
