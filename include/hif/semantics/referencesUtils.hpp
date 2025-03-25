/// @file referencesUtils.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

#include <functional>
#include <set>

namespace hif
{
namespace semantics
{

/// @name References management.
/// @{

class GetReferencesOptions;

/// @brief Set for getAllreferences().
using ReferencesSet = std::set<Object *>;
/// @brief Map for getAllReferences().
using ReferencesMap = std::map<Declaration *, ReferencesSet>;

/// @brief Options of method getAllReferences() / getReferences().
struct GetReferencesOptions {
    /// @brief If true unreferenced declarations are also collected.
    bool include_unreferenced = false;

    /// @brief If true it raises an error when declarations are not found.
    bool error = true;

    /// @brief If true skip standard declarations and declarations inside standard LibraryDefs.
    bool skip_standard_declarations = false;

    /// @brief If true returns only first found reference.
    bool only_first = false;

    /// @brief If set, object is collected only if the method returns true.
    std::function<bool(Object *, ILanguageSemantics *, const GetReferencesOptions &)> check_object_method = nullptr;
};

/// @brief Returns all references to declaration @p decl starting from the
/// @p root subtree. If @p root is nullptr, references will be searched in
/// the whole Hif tree.
/// @warning This function is computationally heavy.
/// @warning This function sets declaration members since it uses the
/// getDeclaration method.
///
/// @param decl The declaration of which references are to be found.
/// @param list The list where to store all found references.
/// @param refSem The reference semantics.
/// @param root The starting root object.
/// @param opt The given options.
///

void getReferences(
    Declaration *decl,
    ReferencesSet &list,
    ILanguageSemantics *refSem,
    Object *root                    = nullptr,
    const GetReferencesOptions &opt = GetReferencesOptions());

/// @brief Returns all references to declaration @p decl starting from the
/// @p root subtree. If @p root is nullptr, references will be searched in
/// the whole Hif tree.
/// @warning This function is computationally heavy.
/// @warning This function sets declaration members since it uses the
/// getDeclaration method.
///
/// @param decl The declaration of which references are to be found.
/// @param list The list where to store all found references.
/// @param refSem The reference semantics.
/// @param root The starting root object.
/// @param opt The given options.
///

void getReferences(
    Declaration *decl,
    ReferencesSet &list,
    ILanguageSemantics *refSem,
    BList<Object> &root,
    const GetReferencesOptions &opt = GetReferencesOptions());

/// @brief Returns all references to declaration @p decl starting from the
/// @p root subtree. If @p root is nullptr, references will be searched in
/// the whole Hif tree.
/// @warning This function is computationally heavy.
/// @warning This function sets declaration members since it uses the
/// getDeclaration method.
///
/// @param decl The declaration of which references are to be found.
/// @param list The list where to store all found references.
/// @param refSem The reference semantics.
/// @param root The starting root object.
/// @param opt The given options.
///
template <typename T>
void getReferences(
    Declaration *decl,
    ReferencesSet &list,
    ILanguageSemantics *refSem,
    BList<T> &root,
    const GetReferencesOptions &opt = GetReferencesOptions());

/// @brief Returns all references of all declarations starting from the
/// @p root subtree.
/// @warning This function is computationally heavy.
/// @warning This function sets declaration members since it uses the
/// getDeclaration method.
/// @warning Collected declarations are only those that can be referenced.
///
/// @param refMap The result map of references.
/// @param refSem The reference semantics.
/// @param root The starting root object.
/// @param opt The given options.
///

void getAllReferences(
    ReferencesMap &refMap,
    ILanguageSemantics *refSem,
    Object *root,
    const GetReferencesOptions &opt = GetReferencesOptions());

/// @brief Returns all references of all declarations starting from the
/// @p root subtree.
/// @warning This function is computationally heavy.
/// @warning This function sets declaration members since it uses the
/// getDeclaration method.
/// @warning Collected declarations are only those that can be referenced.
///
/// @param refMap The result map of references.
/// @param refSem The reference semantics.
/// @param root The starting root object.
/// @param opt The given options.
///

void getAllReferences(
    ReferencesMap &refMap,
    ILanguageSemantics *refSem,
    BList<Object> &root,
    const GetReferencesOptions &opt = GetReferencesOptions());

/// @brief Returns all references of all declarations starting from the
/// @p root subtree.
/// @warning This function is computationally heavy.
/// @warning This function sets declaration members since it uses the
/// getDeclaration method.
/// @warning Collected declarations are only those that can be referenced.
///
/// @param refMap The result map of references.
/// @param refSem The reference semantics.
/// @param root The starting root object.
/// @param opt The given options.
///
template <typename T>
void getAllReferences(
    ReferencesMap &refMap,
    ILanguageSemantics *refSem,
    BList<T> &root,
    const GetReferencesOptions &opt = GetReferencesOptions());

/// @}

} // namespace semantics
} // namespace hif
