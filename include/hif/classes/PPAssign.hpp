/// @file PPAssign.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/ReferencedAssign.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/hifEnums.hpp"

namespace hif
{

/// @brief Abstract class for parameter or port assignments.
///
/// @details
/// This class is an abstract class for parameter assignments
/// (i.e., arguments) or port assignments (i.e., port bindings).
///
/// @see ParameterAssign, PortAssign
class PPAssign : public ReferencedAssign
{
public:
    /// @brief Constructor.
    PPAssign();

    /// @brief Destructor.
    virtual ~PPAssign();

    /// @brief Returns the direction of the parameter or port assignment.
    /// @return The direction of the parameter or port assignment.
    PortDirection getDirection() const;

    /// @brief Sets the direction of the parameter or port assignment.
    /// @param d The direction of the parameter or port assignment to be set.
    void setDirection(const PortDirection d);

    /// @brief Returns the value of the parameter or port assignment.
    /// It represents the argument passed as parameter or the value bound
    /// to a port.
    /// @return The value of the parameter or port assignment.
    Value *getValue() const;

    /// @brief Sets the value of the parameter or port assignment.
    /// @param v The value of the parameter or port assignment to be set.
    /// @return The old value of the parameter or port assignment if it is
    /// different from the new one, nullptr otherwise.
    Value *setValue(Value *v);

protected:
    /// @brief The value of the parameter or port assignment.
    Value *_value;

    /// @brief The direction of the parameter or port assignment.
    PortDirection _direction;

    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    // K: disabled.
    PPAssign(const PPAssign &);
    PPAssign &operator=(const PPAssign &);
};

} // namespace hif
