/// @file FieldReference.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/PrefixedReference.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/INamedObject.hpp"
#include "hif/features/ISymbol.hpp"

namespace hif
{

/// @brief Field reference (i.e., access to a field).
///
/// @details
/// This class represents a field reference, which is an access to a field
/// of a data structure (a Record) or of a class variable (in C++).
///
/// @see Field
class FieldReference : public PrefixedReference,
                       public features::TemplateSymbolIf<Declaration>,
                       public features::INamedObject
{
public:
    /// @brief Constructor.
    FieldReference();

    /// @brief Destructor.
    virtual ~FieldReference();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

    /// @brief Checks whether given declaration is fine for a FieldReference.
    /// In fact, FieldReference DeclarationType is too generic, due to
    /// DataDeclaration and TypeDef.
    /// E.g. we exclude SubPrograms.
    /// @param d The declaration to be checked.
    /// @return True if given declaration is not fine for a FieldReference.
    static bool isNotAllowedDeclaration(Declaration *d);

    /// @brief Returns this object as hif::Object.
    /// @return This object as hif::Object.
    virtual Object *toObject();

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    // K: disabled.
    FieldReference(const FieldReference &);
    FieldReference &operator=(const FieldReference &);
};

} // namespace hif
