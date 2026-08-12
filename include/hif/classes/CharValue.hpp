/// @file CharValue.hpp
/// @brief Value type for representing characters in HIF.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/ConstValue.hpp"

namespace hif
{

/// @brief Char value.
/// @details
/// This class represents a value of type char (i.e., single character).
/// By default this value is assigned to '0'.
class CharValue : public ConstValue
{
public:
    /// @brief Constructor.
    CharValue();

    /// @brief Constructor.
    /// @param c The char value to be assigned. Default is 0.
    CharValue(const char c);

    /// @brief Destructor.
    virtual ~CharValue();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the char value.
    /// @return The char value.
    char getValue() const;

    /// @brief Sets the char value.
    /// @param c The char value to be set.
    void setValue(const char c);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief The actual char value.
    char _value;
};

} // namespace hif
