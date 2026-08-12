/// @file Entity.hpp
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

/// @brief Entity of a view.
///
/// @details
/// This class represents the entity of a view (i.e., the definition of its
/// interface in terms of RTL ports and parameters).
class Entity : public Scope
{
public:
    /// @brief List of parameters defined in the interface.
    BList<Parameter> parameters;

    /// @brief List of of ports defined in the interface.
    BList<Port> ports;

    /// @brief Constructor.
    Entity();

    /// @brief Destructor.
    virtual ~Entity();

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
};

} // namespace hif
