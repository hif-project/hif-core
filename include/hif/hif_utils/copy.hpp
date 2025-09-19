/// @file copy.hpp
/// @brief Utilities for copying HIF objects and object lists.
/// @details
/// Provides functionality to create deep or shallow copies of HIF objects and
/// lists of objects. Copy behavior can be customized through the `CopyOptions`
/// struct.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Struct for customizing copy behavior of HIF objects.
/// @details
/// The `CopyOptions` struct defines various options to customize how HIF
/// objects are copied. This includes whether child objects, comments, code
/// info, and semantic types should be copied. It also allows specifying a
/// user-defined function to process objects during the copy.
struct CopyOptions {
    /// @brief Function pointer type for user-defined copy processing.
    typedef Object *(*UserFunction)(Object *, Object *, void *);

    /// @brief If true, copies the semantic type of `TypedObject`s. Default is `false`.
    bool copySemanticsTypes;

    /// @brief If true, aliases any associated declaration pointers. Default is `true`.
    bool copyDeclarations;

    /// @brief If true, performs a deep copy, including all child objects. Default is `true`.
    bool copyChildObjects;

    /// @brief If true, copies object properties. Default is `true`.
    bool copyProperties;

    /// @brief If true, copies code info associated with the object. Default is `true`.
    bool copyCodeInfos;

    /// @brief If true, copies comments associated with the object. Default is `true`.
    bool copyComments;

    /// @brief Pointer to a user-defined function to be applied during the copy. Default is `nullptr`.
    UserFunction userFunction;

    /// @brief Pointer to user data to be passed to the user function. Default is `nullptr`.
    void *userData;

    /// @brief Default constructor.
    CopyOptions();

    /// @brief Destructor.
    ~CopyOptions();

    /// @brief Copy constructor.
    /// @param o The `CopyOptions` object to copy.
    CopyOptions(const CopyOptions &o);

    /// @brief Assignment operator.
    /// @param o The `CopyOptions` object to assign.
    /// @return A reference to this object.
    CopyOptions &operator=(const CopyOptions &o);
};

/// @brief Creates a copy of a HIF object.
/// @details
/// This function creates a copy of the specified object according to the
/// provided copy options. The default behavior includes deep copying of the
/// object and its properties.
/// @param obj The object to copy.
/// @param opt The copy options.
/// @return A pointer to the copied object.

Object *copy(const Object *obj, const CopyOptions &opt = CopyOptions());

/// @brief Creates a copy of a HIF object (templated version).
/// @details
/// This templated function creates a copy of a specific HIF object type. It
/// ensures type safety by casting the result to the specified type.
/// @tparam T The type of the object to copy.
/// @param obj The object to copy.
/// @param opt The copy options.
/// @return A pointer to the copied object of type `T`.
template <typename T> T *copy(const T *obj, const CopyOptions &opt = CopyOptions())
{
    return static_cast<T *>(copy(static_cast<const Object *>(obj), opt));
}

/// @brief Copies a list of HIF objects.
/// @details
/// This function copies a list of HIF objects from a source list to a
/// destination list. Copy behavior can be customized with the `CopyOptions`
/// parameter.
/// @param src The source list of objects to copy.
/// @param dest The destination list to populate with copies.
/// @param opt The copy options.

void copy(const BList<Object> &src, BList<Object> &dest, const CopyOptions &opt = CopyOptions());

/// @brief Copies a list of HIF objects (templated version).
/// @details
/// This templated function allows copying a list of specific object types while
/// maintaining type safety. Internally, it uses the non-templated version of
/// the function.
/// @tparam T The type of the objects in the list.
/// @param src The source list of objects to copy.
/// @param dest The destination list to populate with copies.
/// @param opt The copy options.
template <class T> void copy(const BList<T> &src, BList<T> &dest, const CopyOptions &opt = CopyOptions())
{
    const BList<Object> *s = reinterpret_cast<const BList<Object> *>(&src);
    BList<Object> *d       = reinterpret_cast<BList<Object> *>(&dest);
    copy(*s, *d, opt);
}

} // namespace hif
