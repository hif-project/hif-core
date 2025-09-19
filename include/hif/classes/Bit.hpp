/// @file Bit.hpp
/// @brief Bit type definition for HIF data structures.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/SimpleType.hpp"

namespace hif
{

///	@brief Bit type.
/// @details
/// This class represents a bit type.
/// The <tt>logic</tt> attribute is used to distinguish between logic type
/// (4-value logic) and bit type (2-value logic).
/// The <tt>resolved</tt> attribute is used to distinguish between resolved and
/// unresolved bit types.
class Bit : public SimpleType
{
public:
    /// @brief Constructor.
    Bit();

    /// @brief Destructor.
    virtual ~Bit();

    Bit(const Bit &)                     = delete;
    auto operator=(const Bit &) -> Bit & = delete;

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the <tt>logic</tt> attribute.
    /// @return The <tt>logic</tt> attribute.
    bool isLogic() const;

    /// @brief Sets the <tt>logic</tt> attribute.
    /// @param logic The <tt>logic</tt> attribute to be set.
    void setLogic(bool logic);

    /// @brief Sets the <tt>resolved</tt> attribute.
    /// @return The <tt>resolved</tt> attribute.
    bool isResolved() const;

    /// @brief Set the <tt>resolved</tt> attribute.
    /// @param resolved The <tt>resolved</tt> attribute to be set.
    void setResolved(bool resolved);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief Flag to store the <tt>logic</tt> attribute.
    bool _isLogic;

    /// @brief Flag to store the <tt>resolved</tt> attribute.
    bool _isResolved;
};

} // namespace hif
