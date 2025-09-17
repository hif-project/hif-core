/// @file HifNodeRef.hpp
/// @brief Reference wrapper for HIF nodes, enabling safe and efficient access.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Wrapper used to store a temporary reference to a node in the tree.
/// The passed object is removed from the tree.
/// Example code:
/// @code
/// HifNodeRef ref(o);
/// ref.replace(factory.xxx(xxx, o, xxx));
/// @endcode
class HifNodeRef
{
public:
    /// @brief Constructor.
    /// @param o the object from which take the reference.
    explicit HifNodeRef(Object *o);

    /// @brief Destructor.
    ~HifNodeRef();

    /// @brief Method to replace the reference with another object.
    bool replace(Object *o);

private:
    /// @brief The placeholder.
    Bit ref;

    HifNodeRef(const HifNodeRef &);
    HifNodeRef &operator=(const HifNodeRef &);
};

} // namespace hif
