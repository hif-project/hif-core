/// @file TypeReference.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/features/ISymbol.hpp"

#include "hif/classes/BList.hpp"
#include "hif/classes/ReferencedType.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Reference to a user-defined type.
///
/// @details
/// This class represents a reference to a user-defined type
/// (i.e., a TypeDef object).
/// A list of ranges can be optionally specified in order to restrict the
/// possible values of the referenced type.
class TypeReference : public ReferencedType, public features::TemplateSymbolIf<TypeDeclaration>
{
public:
    /// @brief List of template arguments (i.e., assignments to template formal parameters).
    BList<TPAssign> templateParameterAssigns;

    /// @brief List of the optional range restrictions on the referenced type.
    BList<Range> ranges;

    /// @brief Constructor.
    TypeReference();

    /// @brief Destructor.
    virtual ~TypeReference();

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

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    // K: disabled
    TypeReference(const TypeReference &);
    TypeReference &operator=(const TypeReference &);
};

} // namespace hif
