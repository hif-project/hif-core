/// @file GlobalAction.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Object.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief List of global actions.
///
/// @details
/// This class represents a list of global actions (i.e., concurrent statements)
/// in a Contents object.
class GlobalAction : public Object
{
public:
    /// @brief List of global actions.
    BList<Action> actions;

    /// @brief Constructor.
    GlobalAction();

    /// Destructor.
    virtual ~GlobalAction();

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
