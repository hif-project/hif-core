/// @file SwitchAlt.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Alt.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Switch statement alternative.
///
/// @details
/// This class represents an alternative for a switch statement
/// (i.e., a switch case).
/// It consists of a list of conditions and a list of actions to be executed
/// when one of the conditions is met.
///
/// @see Switch
class SwitchAlt : public Alt
{
public:
    /// @brief List of conditions.
    BList<Value> conditions;

    /// @brief List of actions to be executed if one of the conditions is met.
    BList<Action> actions;

    /// @brief Constructor.
    SwitchAlt();

    /// @brief Destructor.
    virtual ~SwitchAlt();

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
