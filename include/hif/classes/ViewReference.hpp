/// @file ViewReference.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/ReferencedType.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/ISymbol.hpp"

namespace hif
{

/// @brief Reference to a view of a design unit.
///
/// @details
/// This class represents a reference to a view of a design unit.
/// A view is referenced by its name and by the name of the design unit
/// where the view is defined.
///
/// @see View, DesignUnit
class ViewReference : public ReferencedType, public features::TemplateSymbolIf<View>
{
public:
    /// @brief The list of all template parameters assignments needed to instance the
    /// referenced view.
    ///
    BList<TPAssign> templateParameterAssigns;

    /// @brief Constructor.
    /// The names of the view and the design unit are initialized to NAME_NONE.
    ViewReference();

    /// @brief Destructor.
    virtual ~ViewReference();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the name of the design unit of the referenced view.
    /// @return The name of the design unit of the referenced view.
    std::string getDesignUnit() const;

    /// @brief Sets the name of the design unit of the referenced view.
    /// @warning @p n must be different from nullptr.
    /// @warning This method only changes the name of the design unit in the
    /// calling view reference. It does not modify the name in the design unit
    /// itself or in any other view reference.
    /// @param n The name of the design unit.
    void setDesignUnit(const std::string& n);

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
    /// @brief The name of the design unit of the referenced view.
    std::string _unitname;

    // K: disabled
    ViewReference(const ViewReference &);
    ViewReference &operator=(const ViewReference &);
};

} // namespace hif
