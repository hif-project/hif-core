/// @file Return.hpp
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

/// @brief Return statement.
///
/// @details
/// This class represents a return statement, which terminates the execution
/// of a subprogram. In case of a function, a return value must be specified.
/// In case of a procedure, a return value must not be specified.
class Return : public Action
{
public:
    /// @brief Returned type by member GetDeclaration
    typedef Function DeclarationType;

    /// @brief Constructor.
    Return();

    /// @brief Destructor.
    virtual ~Return();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the returned value.
    /// @return The returned value.
    Value *getValue() const;

    /// @brief Sets the returned value.
    /// @param n The returned value to be set.
    /// @return The previous returned value, or nullptr if none.
    Value *setValue(Value *n);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object to get the name for.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    /// @brief The returned value.
    Value *_value;

    // K: disabled
    Return(const Return &);
    Return &operator=(const Return &);
};

} // namespace hif
