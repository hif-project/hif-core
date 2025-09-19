/// @file simplify.hpp
/// @brief Provides functionality to simplify HIF object trees.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/manipulation/SimplifyOptions.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Maps types to their simplified counterparts.
template <class T> struct SimplifiedType {
    typedef Object type; ///< Default simplification type is `Object`.
};

/// @brief Specialization for `Value`. Simplifies to `Value`.
template <> struct SimplifiedType<Value> {
    typedef Value type; ///< The simplified type is `Value`.
};

/// @brief Specialization for `Cast`. Simplifies to `Value`.
template <> struct SimplifiedType<Cast> {
    typedef Value type; ///< The simplified type is `Value`.
};

/// @brief Specialization for `Expression`. Simplifies to `Value`.
template <> struct SimplifiedType<Expression> {
    typedef Value type; ///< The simplified type is `Value`.
};

/// @brief Specialization for `When`. Simplifies to `Value`.
template <> struct SimplifiedType<When> {
    typedef Value type; ///< The simplified type is `Value`.
};

/// @brief Specialization for `With`. Simplifies to `Value`.
template <> struct SimplifiedType<With> {
    typedef Value type; ///< The simplified type is `Value`.
};

/// @brief Specialization for `Range`. Simplifies to `Range`.
template <> struct SimplifiedType<Range> {
    typedef Range type; ///< The simplified type is `Range`.
};

/// @brief Specialization for `If`. Simplifies to `Action`.
template <> struct SimplifiedType<If> {
    typedef Action type; ///< The simplified type is `Action`.
};

/// @brief Specialization for `Switch`. Simplifies to `Action`.
template <> struct SimplifiedType<Switch> {
    typedef Action type; ///< The simplified type is `Action`.
};

/// @brief Specialization for `For`. Simplifies to `Action`.
template <> struct SimplifiedType<For> {
    typedef Action type; ///< The simplified type is `Action`.
};

/// @brief Specialization for `IfGenerate`. Simplifies to `Action`.
template <> struct SimplifiedType<IfGenerate> {
    typedef Action type; ///< The simplified type is `Action`.
};

/// @brief Specialization for `ForGenerate`. Simplifies to `Declaration`.
template <> struct SimplifiedType<ForGenerate> {
    typedef Declaration type; ///< The simplified type is `Declaration`.
};

/// @brief Specialization for `FieldReference`. Simplifies to `Value`.
template <> struct SimplifiedType<FieldReference> {
    typedef Value type; ///< The simplified type is `Value`.
};

/// @brief Specialization for `Type`. Simplifies to `Type`.
template <> struct SimplifiedType<Type> {
    typedef Type type; ///< The simplified type is `Type`.
};

/// @brief Simplifies a single object.
/// @param o The object to simplify.
/// @param refSem Pointer to the current semantics.
/// @param opt Options for simplification.
/// @return The simplified object.

Object *simplify(Object *o, hif::semantics::ILanguageSemantics *refSem, const SimplifyOptions &opt = SimplifyOptions());

/// @brief Simplifies a list of objects.
/// @param o The list of objects to simplify.
/// @param refSem Pointer to the current semantics.
/// @param opt Options for simplification.

void simplify(
    BList<Object> &o,
    hif::semantics::ILanguageSemantics *refSem,
    const SimplifyOptions &opt = SimplifyOptions());

/// @brief Simplifies a list of typed objects.
/// @param o The list of typed objects to simplify.
/// @param refSem Pointer to the current semantics.
/// @param opt Options for simplification.
template <typename T>
void simplify(BList<T> &o, hif::semantics::ILanguageSemantics *refSem, const SimplifyOptions &opt = SimplifyOptions());

/// @brief Simplifies a typed object.
/// @param o The object to simplify.
/// @param refSem Pointer to the current semantics.
/// @param opt Options for simplification.
/// @return The simplified object.
template <typename T>
typename SimplifiedType<T>::type *
simplify(T *o, hif::semantics::ILanguageSemantics *refSem, const SimplifyOptions &opt = SimplifyOptions());

/// @brief Simplifies and returns a copy of an object with aggressive simplifications.
/// @param o The object to simplify.
/// @param refSem Pointer to the current semantics.
/// @return A new, aggressively simplified object.

Object *getAggressiveSimplified(Object *o, hif::semantics::ILanguageSemantics *refSem);

/// @brief Simplifies and returns a copy of a typed object with aggressive simplifications.
/// @param o The object to simplify.
/// @param refSem Pointer to the current semantics.
/// @return A new, aggressively simplified object.
template <typename T>
typename SimplifiedType<T>::type *getAggressiveSimplified(T *o, hif::semantics::ILanguageSemantics *refSem);

} // namespace manipulation
} // namespace hif
