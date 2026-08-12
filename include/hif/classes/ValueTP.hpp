/// @file ValueTP.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/DataDeclaration.hpp"

namespace hif
{

/// @brief Value template parameter.
///
/// @details
/// This class represents the declaration of a value template parameter
/// (i.e., a template parameter the arguments of which are values).
class ValueTP : public DataDeclaration
{
public:
    /// @brief Constructor.
    ValueTP();

    /// @brief Destructor.
    virtual ~ValueTP();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns whether this is a value TP that must be resolved
    /// at compile time.
    /// @return <tt>true</tt> if this is a compile time constant, <tt>false</tt> otherwise.
    bool isCompileTimeConstant() const;

    /// @brief Sets whether this is a value TP that must be resolved
    /// at compile time.
    /// @param compileTimeConstant The value to set.
    void setCompileTimeConstant(bool compileTimeConstant);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Distinguishes between a value TP that must be resolved at compile
    /// time or not.
    bool _isCompileTimeConstant;
};

} // namespace hif
