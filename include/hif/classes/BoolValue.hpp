/// @file BoolValue.hpp
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

/// @brief Boolean value.
/// @details
/// This class represents a boolean value.
/// By default this value is assigned to <tt>false</tt>.
class BoolValue : public ConstValue
{
public:
    /// @brief Constructor.
    BoolValue();

    /// @brief Constructor.
    /// @param b The boolean value to be assigned. Default is <tt>false</tt>.
    explicit BoolValue(bool b);

    /// @brief Destructor.
    virtual ~BoolValue();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the boolean value.
    /// @return The boolean value.
    bool getValue() const;

    /// @brief Sets the boolean value.
    /// @param b The boolean value to be set.
    void setValue(bool b);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief The actual boolean value.
    bool _value;
};

} // namespace hif
