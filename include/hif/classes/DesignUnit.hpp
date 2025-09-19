/// @file DesignUnit.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Scope.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Design unit.
///
/// @details
/// This class represents a design unit (i.e., a design unit in VHDL, a module
/// in Verilog and SystemC, a class C++).
/// It is possible to define several views (i.e., implementations of the design
/// unit) for a design unit in a HIF description.
///
/// @see View
class DesignUnit : public Scope
{
public:
    /// @brief List of views of the design unit.
    BList<View> views;

    /// @brief Constructor.
    /// The name of the design unit is set to <tt>NAME_NONE</tt>.
    DesignUnit();

    /// @brief Destructor.
    virtual ~DesignUnit();

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

    /**
     * @brief Returns the name of given BList w.r.t. this.
     * @param list The BList to get the name for.
     * @return The name of the BList.
     */
    virtual std::string _getBListName(const BList<Object> &list) const;
};

} // namespace hif
