/// @file SimpleType.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Type.hpp"

namespace hif
{

/// @brief Abstract class for simple types.
///
/// @details
/// This class is an abstract class for simple types (i.e., their definition
/// does not involve other types).
class SimpleType : public Type
{
public:
    /// @brief Constructor.
    SimpleType();

    SimpleType(const SimpleType &)            = delete;
    SimpleType &operator=(const SimpleType &) = delete;

    /// @brief Destructor.
    virtual ~SimpleType();

    /// @brief Returns the <tt>constexpr</tt> attribute.
    /// @return The <tt>constexpr</tt> attribute.
    ///
    bool isConstexpr() const;

    /// @brief Sets the <tt>constexpr</tt> attribute.
    /// @param flag The <tt>constexpr</tt> attribute to be set.
    void setConstexpr(bool flag);

protected:
    /// @brief Flag to store the <tt>constexpr</tt>.
    /// Default value is false.
    bool _isConstexpr;

    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();
};

} // namespace hif
