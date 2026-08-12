/// @file Null.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Action.hpp"

namespace hif
{

/// @brief Null statement (NOP).
///
/// @details
/// This class represents a null statement (i.e., a NOP).
/// It is an action that does nothing.
class Null : public Action
{
public:
    /// @brief Constructor.
    Null();

    /// @brief Destructor.
    virtual ~Null();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();
};

} // namespace hif
