/// @file IntValue.hpp
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

/// @brief Integer value.
/// @details
/// This class represents a value of integer type.
/// By default this value is assigned to 0.
class IntValue : public ConstValue
{
public:
    /// @brief Constructor.
    IntValue() = default;

    /// @brief Constructor.
    /// @param value The integer value to be assigned. Default is 0.
    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    IntValue(T value)
        : _value(static_cast<std::int64_t>(value))
    {
        // Nothing to do here.
    }

    /// @brief Destructor.
    virtual ~IntValue() = default;

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the integer value.
    /// @return The integer value.
    template <typename T = std::int64_t, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    auto getValue() const -> T
    {
        return static_cast<T>(_value);
    }

    /// @brief Sets the integer value.
    /// @param value The integer value to be set.
    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0> void setValue(T value)
    {
        _value = static_cast<std::int64_t>(value);
    }

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief The actual integer value.
    std::int64_t _value;
};

} // namespace hif
