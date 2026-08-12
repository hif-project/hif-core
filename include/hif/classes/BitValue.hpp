/// @file BitValue.hpp
/// @brief Value type for representing single bits in HIF.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/ConstValue.hpp"

namespace hif
{

/// @brief Bit value.
/// @details
/// This class represents a value of bit type.
/// By default this value is assigned to '0'.
class BitValue : public ConstValue
{
public:
    /// @brief Constructor.
    BitValue();

    /// @brief Constructor.
    /// @param b The bit value to be assigned. Default is bit_zero.
    BitValue(const BitConstant b);

    /// @brief Destructor.
    virtual ~BitValue();

    BitValue(const BitValue &)                     = delete;
    auto operator=(const BitValue &) -> BitValue & = delete;

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the bit value.
    /// @return The bit value.
    BitConstant getValue() const;

    /// @brief Sets the bit value.
    /// @param b The bit value to be set.
    void setValue(const BitConstant b);

    /// @brief Sets the bit value.
    /// @param b Character representing the new bit value.
    void setValue(const char b);

    /// @brief Returns the bit value as a string.
    /// @return The bit value as a string.
    std::string toString() const;

    /// @brief Returns true if contains 0 or 1.
    /// @return true if contains 0 or 1.
    bool is01() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief The actual bit value.
    BitConstant _value;
};

} // namespace hif
