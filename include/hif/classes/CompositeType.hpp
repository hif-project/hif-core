/// @file CompositeType.hpp
/// @brief Base class for composite data types in HIF.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Type.hpp"

namespace hif
{

///	@brief Abstract class for composite type.
/// @details
/// This class represents a composite data type.
/// Composite data types are data types which can be built by
/// exploiting primitive data types and other composite types.
/// This is a base class for all composite types.
///
/// @see Array, Record.
class CompositeType : public Type
{
public:
    /// @brief Constructor.
    CompositeType();

    /// @brief Destructor.
    virtual ~CompositeType();

    /// @brief Returns the composite type.
    /// @return The composite type.
    Type *getType() const;

    /// @brief Sets the composite type.
    /// @param t The new composite type to be set.
    /// @return The old composite type if it is different
    /// from the new one, nullptr otherwise.
    Type *setType(Type *t);

    /// @brief Returns the composite base type.
    /// @param considerOpacity <tt>true</tt> if opacity is to be considered, <tt>false</tt> otherwise.
    /// @return The composite base type.
    Type *getBaseType(const bool considerOpacity) const;

    /// @brief Sets the composite base type.
    /// @param t The new composite type to be set.
    /// @param considerOpacity <tt>true</tt> if opacity is to be considered, <tt>false</tt> otherwise.
    /// @return The old composite base type if it is different
    /// from the new one, nullptr otherwise.
    Type *setBaseType(Type *t, const bool considerOpacity);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    virtual std::string _getFieldName(const Object *child) const;

    /// @brief The composite type.
    Type *_type;

    /// @brief The composite base type.
    Type *_baseType;

    /// @brief The composite base type considering opacity.
    Type *_baseOpaqueType;

private:
    // K: disabled
    CompositeType(const CompositeType &);
    CompositeType &operator=(const CompositeType &);
};

} // namespace hif
