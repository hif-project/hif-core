/// @file Variable.hpp
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

/// @brief Variable declaration.
///
/// @details
/// This class represents the declaration of a variable.
/// It consists of the name of the variable, its type and its initial value.
class Variable : public DataDeclaration
{
public:
    /// @brief Constructor.
    Variable();

    /// @brief Destructor.
    virtual ~Variable();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns whether the variable is an instance of a design unit.
    /// @return <tt>true</tt> if the variable is an instance of a design unit, <tt>false</tt> otherwise.
    bool isInstance() const;

    /// @brief Sets whether the variable is an instance of a design unit.
    /// @param instance <tt>true</tt> if the variable is an instance of a design unit, <tt>false</tt> otherwise.
    void setInstance(bool instance);

    /// @brief Returns whether this is a standard declaration.
    /// @return <tt>true</tt> if this is a standard declaration, <tt>false</tt> otherwise.
    bool isStandard() const;

    /// @brief Sets whether this is a standard declaration.
    /// @param standard <tt>true</tt> if this is a standard declaration, <tt>false</tt> otherwise.
    void setStandard(bool standard);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief Stores whether the variable is an instance of a design unit.
    /// Default value is <tt>true</tt>.
    bool _isInstance;

    /// @brief Distinguishes between a normal declaration (i.e., part of design)
    /// and a standard one (i.e., part of the language).
    bool _isStandard;

    // K: disabled
    Variable(const Variable &);
    Variable &operator=(const Variable &);
};

} // namespace hif
