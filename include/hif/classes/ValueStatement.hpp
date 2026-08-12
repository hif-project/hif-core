/// @file ValueStatement.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Action.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

///	@brief Call to a procedure.
///
/// @details
/// This class represents a call to a procedure.
///
/// @see Procedure
class ValueStatement : public Action
{
public:
    /// @brief Constructor.
    ValueStatement();

    /// @brief Destructor.
    virtual ~ValueStatement();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the statement value.
    /// @return The statement value.
    Value *getValue() const;

    /// @brief Sets the statement value.
    /// @param n The statement value to be set.
    /// @return The previous statement value, or nullptr if none.
    Value *setValue(Value *n);

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
    ///@brief The calling object.
    Value *_value;

    // K: disabled
    ValueStatement(const ValueStatement &);
    ValueStatement &operator=(const ValueStatement &);
};

} // namespace hif
