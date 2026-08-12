/// @file ScopedType.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Type.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Abstract class for types containing declarations.
///
/// @details
/// This class is an abstract class for types which contain declarations.
///
/// @see Enum, Record
class ScopedType : public Type
{
public:
    /// @brief Returns the <tt>constexpr</tt> attribute.
    /// @return The <tt>constexpr</tt> attribute.
    bool isConstexpr();

    /// @brief Sets the <tt>constexpr</tt> attribute.
    /// @param v The <tt>constexpr</tt> attribute to be set.
    void setConstexpr(bool v);

    /// @brief Destructor.
    virtual ~ScopedType();

protected:
    /// @brief Constructor.
    ScopedType();

    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief Flag to store the <tt>constexpr</tt> attribute.
    bool _isConstexpr;
};

} // namespace hif
