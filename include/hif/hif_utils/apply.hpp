/// @file apply.hpp
/// @brief Defines functions for applying operations to all objects within a HIF subtree.
/// @details Provides utility functions to traverse and apply user-defined operations
/// on all objects in a subtree or a list of subtrees in a HIF (Hardware Intermediate Format)
/// system description. These functions support custom operations via function pointers.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/GuideVisitor.hpp"
#include "hif/classes/classes.hpp"

namespace hif
{

/// @name Apply Functions
/// @brief Functions for applying operations to all objects in a subtree.
/// @details These functions allow you to apply a given function to all objects contained
/// in a subtree or a list of subtrees in a HIF description.
/// The user-defined function must have the following signature:
/// ```cpp
/// bool myFunction(Object *obj, void *data);
/// ```
/// - The first parameter is the current object being processed.
/// - The second parameter is user-defined data that can be passed for context or state.
/// - The function must return a `bool`:
///   - `true`: Continue applying the function to the object's children.
///   - `false`: Skip the object's children.
/// @{

namespace apply
{

template <typename Function, typename Data> class ApplyVisitor : public GuideVisitor
{
public:
    /// @brief Constructs an ApplyVisitor with a function and data.
    /// @param function The function to apply to each object.
    /// @param data User-defined data to pass to the function.
    ApplyVisitor(Function function, Data data)
        : GuideVisitor()
        , _function(function)
        , _data(data)
    {
        // ntd
    }

    virtual ~ApplyVisitor() = default;

    ApplyVisitor(const ApplyVisitor &)      = delete;
    ApplyVisitor &operator=(ApplyVisitor &) = delete;

    /// @brief Called before visiting an object.
    /// @param o The object being visited.
    /// @return True if the function returns false (to stop traversal), false otherwise.
    bool BeforeVisit(Object &o) { return !_function(&o, _data); }

private:
    Function _function;
    Data _data;
};

/// @brief Applies a function to all objects in a subtree.
/// @details Traverses the subtree rooted at `o`, applying the function `f` to each object.
/// If `f` returns `true` for an object, the traversal continues to the object's children.
/// If `f` returns `false`, the traversal skips that object's subtree.
///
/// @param o The root object of the subtree.
/// @param f The function to apply to each object in the subtree.
/// @param data Optional user-defined data to pass as the second parameter to `f`.

template <typename Function, typename Data> void visit(Object *o, Function f, Data data)
{
    if (o) {
        apply::ApplyVisitor<Function, Data> app(f, data);
        o->acceptVisitor(app);
    }
}

/// @brief Applies a function to all objects in subtrees within a list.
/// @details Iterates through the elements of the list `l`, applying `apply` to each element.
/// The function `f` is applied to all objects in the subtree of each element.
///
/// @tparam T The type of objects in the list. Must be derived from `Object`.
/// @param l A list of subtrees to process.
/// @param f The function to apply to each object in the subtrees.
/// @param data Optional user-defined data to pass as the second parameter to `f`.
template <class T, typename Function, typename Data> void visit(BList<T> &l, Function f, Data data)
{
    for (typename BList<T>::iterator i = l.begin(); i != l.end(); ++i) {
        hif::apply::visit(*i, f, data);
    }
}

/// @}

} // namespace apply

} // namespace hif
