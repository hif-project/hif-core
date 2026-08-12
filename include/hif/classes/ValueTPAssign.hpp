/// @file ValueTPAssign.hpp
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

/// @brief Value template parameter assignment.
///
/// @details
/// This class represents a value template parameter assignment (i.e., a
/// value template argument/actual parameter).
///
/// @see ValueTP
class ValueTPAssign : public TPAssign, public features::TemplateSymbolIf<ValueTP>
{
public:
    /// @brief Constructor.
    ValueTPAssign();

    /// @brief Destructor.
    virtual ~ValueTPAssign();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Sets the value assigned to the value template parameter.
    /// @param v The value to be assigned to the value template parameter.
    /// @return The value previously assigned to the value template parameter.
    Value *setValue(Value *v);

    /// @brief Returns the value assigned to the value template paramter.
    /// @return The value of the template parameter.
    Value *getValue() const;

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
    /// @brief The value assigned to the value template parameter.
    Value *_value;

    // K: disabled
    ValueTPAssign(const ValueTPAssign &);
    ValueTPAssign &operator=(const ValueTPAssign &);
};

} // namespace hif
