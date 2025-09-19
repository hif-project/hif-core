/// @file Contents.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/BaseContents.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

///	@brief Contents of a view.
///
/// @details
/// This class represents the contents of a view, which describe the internal
/// structure of a view in detail.
/// Contents contain a list of local declarations (variables, constants, type
/// definitions, procedures and functions).
/// Contents is used to describe the internal structure of a view in detail.
/// It can contain a list of local declarations (variables, constants, type definitions, procedures
/// and functions definition).
/// Contents can also contain generate statements, instances of sub-components
/// and concurrent actions (i.e., global actions).
/// Contents represent the architecture of a design unit in VHDL.
class Contents : public BaseContents
{
public:
    /// @brief List of references to used libraries.
    BList<Library> libraries;

    /// @brief Constructor.
    Contents();

    /// @brief Destructor.
    virtual ~Contents();

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

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    // K: disabled
    Contents(const Contents &);
    Contents &operator=(const Contents &);
};

} // namespace hif
