/// @file BiVisitor.hpp
/// @brief Provides interfaces for MonoVisitor and BiVisitor templates.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Base class for single-object visitors.
/// @details
/// The MonoVisitor class provides a mechanism for visiting objects of
/// different types derived from a common base class (Object). It defines
/// a method `callMap` that dynamically dispatches calls to the appropriate
/// `map` method in the derived visitor class based on the runtime class
/// ID of the object.
/// @tparam Child The derived visitor class that implements specific `map`
/// methods for each object type.
template <class Child> class MonoVisitor
{
public:
    /// @brief Default constructor.
    MonoVisitor();

    /// @brief Virtual destructor to ensure proper cleanup in derived classes.
    virtual ~MonoVisitor() = 0;

    /// @brief Dispatches the object to the appropriate map function in the
    /// derived class.
    /// @details
    /// This function dynamically resolves the runtime type of the object
    /// and calls the corresponding `map` method implemented in the derived
    /// class.
    /// @param o1 The object to visit.
    void callMap(Object *o1);

private:
    /// @brief Copy constructor (deleted).
    MonoVisitor(const MonoVisitor<Child> &);

    /// @brief Assignment operator (deleted).
    MonoVisitor<Child> &operator=(const MonoVisitor<Child> &);
};

/// @brief Base class for dual-object visitors.
/// @details
/// The BiVisitor class extends `MonoVisitor` to support visiting pairs of
/// objects. It provides a mechanism for dispatching calls to appropriate
/// `map` methods based on the runtime class IDs of two objects. This is
/// useful for operations that involve relationships or interactions between
/// two objects.
/// @tparam Child The derived visitor class that implements specific `map`
/// methods for object pairs.
template <class Child> class BiVisitor : public MonoVisitor<Child>
{
public:
    using MonoVisitor<Child>::callMap;

    /// @brief Default constructor.
    BiVisitor();

    /// @brief Virtual destructor to ensure proper cleanup in derived classes.
    virtual ~BiVisitor() = 0;

    /// @brief Dispatches the object pair to the appropriate map function in the
    /// derived class.
    /// @details
    /// This function dynamically resolves the runtime types of the two objects
    /// and calls the corresponding `map` method implemented in the derived
    /// class.
    /// @param o1 The first object to visit.
    /// @param o2 The second object to visit.
    void callMap(Object *o1, Object *o2);

private:
    /// @brief Helper function to rebind object pairs to their specific types
    ///        and dispatch to the derived class's `map` function.
    ///
    /// @tparam T The type of the first object in the pair.
    /// @param o1 The first object, already cast to type T.
    /// @param o2 The second object to visit.
    template <typename T> void _rebind(T *o1, Object *o2);

    /// @brief Copy constructor (deleted).
    BiVisitor(const BiVisitor<Child> &);

    /// @brief Assignment operator (deleted).
    BiVisitor<Child> &operator=(const BiVisitor<Child> &);
};

} // namespace hif

#include "BiVisitor.i.hpp"
