/// @file TypedObject.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Object.hpp"

namespace hif
{

/// @brief Base class for objects having a semantic type.
///
/// @details
/// This class is a base class for objects which can have a semantic type.
class TypedObject : public Object
{
public:
    /// @brief Constructor.
    TypedObject();

    /// @brief Destructor.
    virtual ~TypedObject();

    /// @brief Returns the semantic type.
    /// @return The semantic type.
    virtual Type *getSemanticType() const;

    /// @brief Sets the semantic type.
    /// @param t The semantic type to be set.
    /// @return The old semantic type.
    virtual Type *setSemanticType(Type *t);

protected:
    /// @brief The semantic type.
    Type *_semanticsType;

    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    // K: disabled.
    TypedObject(const TypedObject &);
    TypedObject &operator=(const TypedObject &);
};

} // namespace hif
