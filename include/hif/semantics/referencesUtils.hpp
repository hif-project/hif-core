/// @file referencesUtils.hpp
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

/// @name References management.
/// @{

/// @brief Set for getAllreferences().
typedef std::set<Object *> ReferencesSet;
/// @brief Map for getAllReferences().
typedef std::map<Declaration *, ReferencesSet> ReferencesMap;

/// @brief Options of method getAllReferences() / getReferences().
struct GetReferencesOptions {
    /// @brief Function pointer type for custom object collection methods.
    typedef bool (*CollectObjectMethod)(Object *, ILanguageSemantics *, const GetReferencesOptions &);

    /// @brief If <tt>true</tt> unreferenced declarations
    /// are also collected. default = false
    bool includeUnreferenced;

    /// @brief If <tt>true</tt> it raises an error when declarations are
    /// not found. default = true
    bool error;

    /// @brief If <tt>true</tt> skip standard declarations and declarations
    /// inside standard LibraryDefs. default = false
    bool skipStandardDeclarations;

    /// @brief If <tt>true</tt> returns only first found reference.
    /// Default is false.
    bool onlyFirst;

    /// @brief If set, object is collected only if the method returns true.
    CollectObjectMethod collectObjectMethod;

    GetReferencesOptions();
    /// @brief Constructor with parameters.
    /// @param iu Include unreferenced flag.
    /// @param e Error flag.
    /// @param ssd Skip standard declarations flag.
    GetReferencesOptions(const bool iu, const bool e, const bool ssd);
    ~GetReferencesOptions();
    /// @brief Copy constructor.
    /// @param other The object to copy from.
    GetReferencesOptions(const GetReferencesOptions &other);
    /// @brief Assignment operator.
    /// @param other The object to assign from.
    /// @return Reference to this object.
    GetReferencesOptions &operator=(const GetReferencesOptions &other);
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
