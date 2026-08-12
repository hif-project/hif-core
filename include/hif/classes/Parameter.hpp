/// @file Parameter.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/DataDeclaration.hpp"
#include "hif/hifEnums.hpp"

namespace hif
{

///	@brief Parameter declaration.
///
/// @details
/// This class represents the declaration of a parameter in a subprogram.
class Parameter : public DataDeclaration
{
public:
    /// @brief Constructor.
    /// The type is set to nullptr and the direction to <tt>dir_none</tt>.
    Parameter();

    /// @brief Destructor.
    virtual ~Parameter();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the direction of the paramter.
    /// @return The direction of the paramter.
    PortDirection getDirection() const;

    /// @brief Sets the direction of the parameter.
    /// @param d The direction of the parameter to be set.
    void setDirection(PortDirection d);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief The direction of the parameter.
    PortDirection _direction;

    // K: disabled.
    Parameter(const Parameter &);
    Parameter &operator=(const Parameter &);
};

} // namespace hif
