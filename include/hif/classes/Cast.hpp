/// @file Cast.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

/// @brief Explicit cast.
/// @details
/// This class represents an explicit cast.
/// It contains the operand to be cast, and the type to which the operand
/// to be cast.
class Cast : public Value
{
public:
    /// @brief Constructor.
    Cast();

    /// @brief Destructor.
    virtual ~Cast();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the type to which the operand is to be cast.
    /// @return The type to which the operand is to be cast.
    Type *getType() const;

    /// @brief Sets the type to which the operand is to be cast.
    /// @param t The type to which the operand is to be cast.
    /// @return The old type of the cast if it is different from the new one,
    /// nullptr otherwise.
    Type *setType(Type *t);

    /// @brief Returns the operand to be cast.
    /// @return The operand to be cast.
    Value *getValue() const;

    /// @brief Sets the operand to be cast.
    /// @param v The operand to be cast.
    /// @return The old operand of the cast operand if it is different from the
    /// new one, nullptr otherwise.
    Value *setValue(Value *v);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    // K: disabled
    Cast(const Cast &);
    Cast &operator=(const Cast &);

    /// @brief The type to which the operand is to be cast.
    Type *_type;

    /// @brief The operand to be cast.
    Value *_value;
};

} // namespace hif
