/// @file BitvectorValue.hpp
/// @brief Value type for representing bitvectors in HIF.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <cstdint>
#include <cstdlib>
#include <vector>

#include "hif/application_utils/portability.hpp"
#include "hif/classes/ConstValue.hpp"
#include "hif/hifEnums.hpp"

namespace hif
{

/// @brief Bit vector value.
/// @details
/// This class represents a value of bit vector type.
/// By default this value is assigned to '0'.
class BitvectorValue : public ConstValue
{
public:
    /// @brief Constructor.
    BitvectorValue();

    /// @brief Constructor.
    /// @param sValue String representing the bit vector value. Default is "0".
    BitvectorValue(const std::string &sValue);

    /// @brief Destructor.
    virtual ~BitvectorValue();

    BitvectorValue(const BitvectorValue &)                     = delete;
    auto operator=(const BitvectorValue &) -> BitvectorValue & = delete;

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the bit vector value.
    /// @return The bit vector value.
    std::string getValue() const;

    /// @brief Sets the bit vector value.
    /// @param value String representing the bit vector value to be set.
    void setValue(const std::string &value);

    /// @brief Returns true if contains only 0 or 1.
    /// @return true if contains only 0 or 1.
    bool is01() const;

    /// @brief Changes content by transforming to 0/1.
    void to01();

    /// @brief Returs true if contains only X.
    /// @return true if contains only X.
    bool isX() const;

    /// @brief Returns the value as an unsigned integer.
    /// @return The value as std::uint64_t. Returns 0 in case of conversion error.
    std::uint64_t getValueAsUnsigned() const;

    /// @brief Returns the value as a signed integer.
    /// @return The value as std::int64_t. Returns 0 in case of conversion error.
    std::int64_t getValueAsSigned() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief Ensures that a string representing a bit vector value contains
    /// only legal values for bits.
    /// @return <tt>true</tt> if the check is successful, <tt>false</tt>
    /// if an error occurred.
    bool _handleValue(const std::string &val);

    /// @brief The bit vector value.
    std::vector<BitConstant> _value;
};

} // namespace hif
