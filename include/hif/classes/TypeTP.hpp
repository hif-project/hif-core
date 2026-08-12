/// @file TypeTP.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/TypeDeclaration.hpp"

namespace hif
{

/// @brief Type template parameter.
///
/// @details
/// This class represents the declaration of a type template parameter
/// (i.e., a template parameter the arguments of which are types).
class TypeTP : public TypeDeclaration
{
public:
    /// @brief Constructor.
    TypeTP();

    /// @brief Destructor.
    virtual ~TypeTP();

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
