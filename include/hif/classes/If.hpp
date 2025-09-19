/// @file If.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Action.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief If statement.
///
/// @details
/// This class represents an if statement. It consists of a list of possible
/// alternatives (IfAlt), corresponding to the <tt>if</tt> and <tt>else if</tt>
/// branches of the statement, and of a default list of actions corresponding
/// to the <tt>else</tt> branch.
///
/// @see IfAlt
class If : public Action
{
public:
    /// @brief The alt type.
    typedef IfAlt AltType;

    /// @brief List of alternatives representing the different branches.
    BList<IfAlt> alts;

    /// @brief Default list of actions, to be executed when no condition
    /// in alternatives is matched.
    BList<Action> defaults;

    /// @brief Constructor.
    If();

    /// @brief Destructor.
    virtual ~If();

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
