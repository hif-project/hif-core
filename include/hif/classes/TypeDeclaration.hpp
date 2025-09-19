/// @file TypeDeclaration.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Scope.hpp"

namespace hif
{

/// @brief Type declaration.
///
/// @details
/// This class is a base class for type declarations (i.e., type definitions
/// and type template parameters).
///
/// @see TypeDef, TypeTP
class TypeDeclaration : public Scope
{
public:
    /// @brief Constructor.
    TypeDeclaration();

    /// @brief Destructor.
    virtual ~TypeDeclaration();

    /// @brief Returns the type declared.
    /// @return The type declared.
    Type *getType() const;

    /// @brief Sets the type declared.
    /// @param t The type to be set.
    /// @return The old type declared.
    Type *setType(Type *t);

protected:
    /// @brief The type declared.
    Type *_type;

    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    // K: disabled
    TypeDeclaration(const TypeDeclaration &);
    TypeDeclaration &operator=(const TypeDeclaration &);
};

} // namespace hif
