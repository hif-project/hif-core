/// @file Const.hpp
/// @brief Constant declaration for HIF data structures.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/DataDeclaration.hpp"

namespace hif
{

///	@brief Constant declaration.
///
/// @details
/// This class represents the declaration of a constant.
/// It includes the name of the constant, its type and its value.
class Const : public DataDeclaration
{
public:
    /// @brief Constructor.
    Const();

    /// @brief Destructor.
    virtual ~Const();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns whether this constant must be treated as a class member or not.
    /// @return <tt>true</tt> if this constant must be treated as a class member.
    bool isInstance() const;

    /// @brief Sets whether this constant must be treated as a class member.
    /// @param instance <tt>true</tt> if this constant must be treated as a class member,
    /// <tt>false</tt> otherwise.
    void setInstance(const bool instance);

    /// @brief Sets whether this constant must be treated as a define.
    /// @param define <tt>true</tt> if this constant must be treated as a define,
    /// <tt>false</tt> otherwise.
    void setDefine(const bool define);

    /// @brief Returns whether this constant must be treated as a define.
    /// @return <tt>true</tt> if this constant must be treated as a define,
    /// <tt>false</tt> otherwise.
    bool isDefine() const;

    /// @brief Returns whether this is a standard declaration.
    /// @return <tt>true</tt> if this is a standard declaration, <tt>false</tt> otherwise.
    bool isStandard() const;

    /// @brief Sets whether this is a standard declaration.
    /// @param standard <tt>true</tt> if this is a standard declaration, <tt>false</tt> otherwise.
    void setStandard(const bool standard);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief Flag to store whether this constant is an instance of a design unit.
    /// Default value is <tt>true</tt>.
    bool _isInstance;

    /// @brief Flag to store whether this constant must be treated as a define.
    /// Default value is <tt>false</tt>.
    bool _isDefine;

    /// @brief Distinguishes between a normal declaration (i.e., part of design)
    /// and a standard one (i.e., part of the language).
    bool _isStandard;

    // K: disabled
    Const(const Const &);
    Const &operator=(const Const &);
};

} // namespace hif
