/// @file TimeValue.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/ConstValue.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Time value.
///
/// @details
/// This class represents a time value.
/// By default this value is assigned to <tt>0 ns</tt>.
class TimeValue : public ConstValue
{
public:
    /// @brief Enumeration of the supported time units.
    enum TimeUnit : unsigned char {
        time_fs = 0, // femtosecond
        time_ps,     // picosecond
        time_ns,     // nanosecond
        time_us,     // microsecond
        time_ms,     // millisecond
        time_sec,    // second
        time_min,    // minute
        time_hr      // hour
    };

    /// @brief Constructor.
    /// The default value is set to <tt>0 ns</tt>.
    TimeValue();

    /// @brief Destructor.
    virtual ~TimeValue();

    /// @brief Returns true if this time value is less than <tt>other</tt>.
    /// @param other The const reference to the other time value.
    /// @return True if this time value is less than <tt>other</tt>.
    bool operator<(const TimeValue &other) const;

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the time unit of the time value.
    /// @return The time unit of the time value.
    TimeUnit getUnit() const;

    /// @brief Sets the time unit of the time value.
    /// @param u The time unit of the time value to be set.
    void setUnit(TimeUnit u);

    /// @brief Returns the value of the time value.
    /// @return The value of the time value.
    double getValue() const;

    /// @brief Sets the value of the time value.
    /// @param v The value of the time value to be set.
    /// @return The old value of the time value.
    double setValue(const double v);

    /// @brief Converts the TimeValue to the given unit.
    /// @param u The given time unit.
    void changeUnit(const TimeUnit u);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

    /// @brief Return string representation of given time unit.
    /// @param t The time unit.
    /// @return The string representation of given time unit.
    static std::string timeUnitToString(const TimeUnit t);

    /// @brief Return the time unit for given string.
    /// @param s The string.
    /// @return The time unit for given string.
    static TimeUnit timeUnitFromString(const std::string &s);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief The time unit in which the time value is expressed.
    TimeUnit _unit;

    /// @brief The actual value of the time value.
    double _value;

    // K: disabled.
    TimeValue(const TimeValue &);
    TimeValue &operator=(const TimeValue &);
};

} // namespace hif
