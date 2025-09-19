/// @file ConstValue.hpp
/// @brief Value type for representing constants in HIF.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/Value.hpp"

namespace hif
{

/// @brief Base class for constant values.
/// @details
/// This class represents a base class for constant values.
class ConstValue : public Value
{
public:
    /// Destructor.
    virtual ~ConstValue() = 0;

    /**
     * @brief Sets the syntactic type of the constant value.
     * @param t The syntactic type of the constant value.
     * @return The previous syntactic type.
     */
    Type *setType(Type *t);

    /// @brief Returns the syntactic type of the constant value.
    /// @return The syntactic type of the constant value.
    Type *getType() const;

protected:
    /// Disabled constructor.
    ConstValue();

    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    /// The syntactic type of the constant value.
    Type *_type;

    /// @name Disabled.
    //@{

    ConstValue(ConstValue &);

    ConstValue &operator=(ConstValue &);

    //@}
};

} // namespace hif
