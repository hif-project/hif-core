/// @file RealValue.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/ConstValue.hpp"

namespace hif
{

/// @brief Real value.
///
/// @details
/// This class represents a value of real type.
/// The underlying data type used to store the value is <tt>double</tt>.
class RealValue : public ConstValue
{
public:
    /// @brief Constructor.
    RealValue();

    /// @brief Constructor.
    /// @param d The real value to be assigned. Default is 0.0.
    RealValue(const double d);

    /// @brief Destructor.
    virtual ~RealValue();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the real value.
    /// @return The real value.
    double getValue() const;

    /// @brief Sets the real value.
    /// @param d The real value to be set.
    void setValue(const double d);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief The actual real value (stored as a <tt>double</tt>).
    double _value;
};

} // namespace hif
