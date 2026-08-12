/// @file Field.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/DataDeclaration.hpp"

namespace hif
{

/// @brief Field declaration (in a Record).
///
/// @details
/// This class represents the declaration of field within the declaration
/// of a data structure (a Record).
///
/// @see Record
class Field : public DataDeclaration
{
public:
    /// @brief Constructor.
    Field();

    /// @brief Destructor.
    virtual ~Field();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

    /// @brief Returns the direction of the field when representing a port.
    /// @return The direction.
    PortDirection getDirection() const;

    /// @brief Sets the direction of the field when representing a port.
    /// @param d The direction to set.
    void setDirection(const PortDirection d);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    ///	@brief The direction of the field reference when representing a port.
    PortDirection _direction;

    // K: disabled
    Field(const Field &);
    Field &operator=(const Field &);
};

} // namespace hif
