/// @file addUniqueObject.hpp
/// @brief Add unique objects to HIF containers, avoiding duplicates.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/hif_utils/equals.hpp"

namespace hif
{
namespace manipulation
{

/// @brief The addUniqueObject options.
struct AddUniqueObjectOptions {
    AddUniqueObjectOptions();
    ~AddUniqueObjectOptions();

    /// @brief Copy constructor.
    /// @param other The object to copy from.
    AddUniqueObjectOptions(const AddUniqueObjectOptions &other);

    /// @brief Assignment operator.
    /// @param other The object to assign from.
    /// @return Reference to this object.
    AddUniqueObjectOptions &operator=(AddUniqueObjectOptions other);

    /// @brief Swaps the contents of this object with another.
    /// @param other The object to swap with.
    void swap(AddUniqueObjectOptions &other);

    /// @brief If set, the object is inserted in the desired position.
    /// Otherwise, the object is inserted at the end of the list.
    unsigned int position;

    /// @brief If <tt>true</tt> copy the object when inserted.
    bool copyIfUnique;

    /// @brief If <tt>true</tt> delete the object when not inserted.
    bool deleteIfNotAdded;

    /// @brief The options used to check whether the object is already present.
    EqualsOptions equalsOptions;
};

/// @brief Inserts an object into given list only if it is
/// not stored in the list yet w.r.t. given options.
///
/// @param obj The object to insert.
/// @param list The list where to insert.
/// @param opt The options.
/// @return <tt>true</tt> if the object has been inserted, <tt>false</tt> otherwise.

bool addUniqueObject(Object *obj, BList<Object> &list, const AddUniqueObjectOptions &opt = AddUniqueObjectOptions());

/// @brief Inserts an object into given list only if it is
/// not stored in the list yet w.r.t. given options.
///
/// @param obj The object to insert.
/// @param list The list where to insert.
/// @param opt The options.
/// @return <tt>true</tt> if the object has been inserted, <tt>false</tt> otherwise.
template <typename T>
bool addUniqueObject(Object *obj, BList<T> &list, const AddUniqueObjectOptions &opt = AddUniqueObjectOptions());

/// @brief Searchs the nearest suitable list for given object and insert the object
/// into it only if it is not stored in the list yet w.r.t. given options.
///
/// @param obj The object to insert.
/// @param startingObject The object from which start the search of suitable list.
/// @param opt The options.
/// @return <tt>true</tt> if the object has been inserted, <tt>false</tt> otherwise.

bool addUniqueObject(Object *obj, Object *startingObject, const AddUniqueObjectOptions &opt = AddUniqueObjectOptions());

} // namespace manipulation
} // namespace hif
