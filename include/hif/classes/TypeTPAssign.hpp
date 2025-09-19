/// @file TypeTPAssign.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/TPAssign.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/ISymbol.hpp"

namespace hif
{

/// @brief Type template parameter assignment.
///
/// @details
/// This class represents a type template parameter assignment (i.e., a type
/// template argument/actual parameter).
///
/// @see TypeTP
class TypeTPAssign : public TPAssign, public features::TemplateSymbolIf<TypeTP>
{
public:
    /// @brief Constructor.
    TypeTPAssign();

    /// @brief Destructor.
    virtual ~TypeTPAssign();

    /// @brief Returns the type assigned to the type template paramter.
    /// @return The type assigned to the type template parameter.
    Type *getType() const;

    /// @brief Sets the type assigned to the type template parameter.
    /// @param n The type to be assigned to the type template parameter.
    /// @return The type previously assigned to the type template parameter.
    Type *setType(Type *n);

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

    /// @brief Returns this object as hif::Object.
    /// @return This object as hif::Object.
    virtual Object *toObject();

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    ///@brief The type assigned to the type template parameter.
    Type *_type;

    // K: disabled
    TypeTPAssign(const TypeTPAssign &);
    TypeTPAssign &operator=(const TypeTPAssign &);
};

} // namespace hif
