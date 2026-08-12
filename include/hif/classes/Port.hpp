/// @file Port.hpp
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

///	@brief Port declaration.
///
/// @details
/// This class represents the declaration of a RTL port.
/// It consists of the name of the port, its type, its direction and its
/// optional initial value.
class Port : public DataDeclaration
{
public:
    /// @brief Constructor.
    /// The type is set to nullptr and the direction to <tt>dir_none</tt>.
    Port();

    /// @brief Destructor.
    virtual ~Port();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the direction of the port.
    /// @return The direction of the port.
    PortDirection getDirection() const;

    /// @brief Sets the direction of the port.
    /// @param d The direction of the port to be set.
    void setDirection(const PortDirection d);

    /// @brief Returns whether this is a wrapper for an object with
    /// similar features, which does not have a matching data model.
    /// @return <tt>true</tt> if this is a wrapper, <tt>false</tt> otherwise.
    bool isWrapper() const;

    /// @brief Sets whether this is actually a wrapper for an object with
    /// similar features, which does not have a matching data model.
    /// @param wrapper <tt>true</tt> if this is a wrapper, <tt>false</tt> otherwise.
    void setWrapper(bool wrapper);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    ///	@brief The direction of the port.
    PortDirection _direction;

    /// @brief Tells whether this is actually a wrapper for an object with
    /// similar features, which does not have a matching data model (e.g.,
    /// Verilog-AMS natures or disciplines).
    bool _isWrapper;

    // K: disabled
    Port(const Port &);
    Port &operator=(const Port &);
};

} // namespace hif
