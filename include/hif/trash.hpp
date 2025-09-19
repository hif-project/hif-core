/// @file trash.hpp
/// @brief Utilities for handling and managing unused or temporary HIF objects.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <set>

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Trash.
class Trash
{
public:
    /// @brief The type used to hold the objects in trash.
    using TrashHolder = std::set<Object *>;

    Trash();

    /// @brief The destructors automatically calls clear().
    ~Trash();

    /// @brief Deletes all object inside the trash, possibly removing them
    /// from the tree.
    /// @param where The current point inside the tree. Used to avoid to delete
    ///        a parrent of current object.
    void clear(Object *where = nullptr);

    /// @brief Empties the trash, without deleting the objects.
    void reset();

    /// @brief Add given object in trash.
    /// @param object The object.
    void insert(Object *object);

    /// @brief Add given list objects in trash. All list elements are removed.
    /// @param list The list
    void insert(BList<Object> &list);

    /// @brief Add given list objects in trash. All list elements are removed.
    /// @param list The list
    template <typename T> void insert(BList<T> &list);

    /// @brief Check whether given object is in trash.
    /// @param o The object.
    /// @return True if the object is in trash.
    bool contains(Object *o) const;

    /// @brief Removes a root object from the trash, if contained, without
    /// deleting it.
    /// @param o The object.
    /// @return True if the object was in cache.
    bool remove(Object *o);

private:
    TrashHolder _holder;

    Trash(const Trash &other);
    Trash &operator=(const Trash &other);
};
} // namespace hif
