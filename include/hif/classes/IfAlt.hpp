/// @file IfAlt.hpp
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

/// @brief If statement alternative.
///
/// @details
/// This class represents an alternative for an if statement (i.e., a branch
/// of an if statement, except the <tt>else</tt> one).
/// It consists of a boolean condition and a list of actions to be executed
/// when the condition is true.
///
/// @see If
class IfAlt : public Alt
{
public:
    /// @brief List of actions to be executed if the condition is matched.
    BList<Action> actions;

    /// @brief Constructor.
    IfAlt();

    /// @brief Destructor.
    virtual ~IfAlt();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the condition of the if alternative.
    /// @return The condition of the if alternative.
    Value *getCondition() const;

    /// @brief Sets the condition of the if alternative.
    /// @param b The condition of the if alternative to be set.
    /// @return The old condition of the if alternative if it is different
    /// from the new one, nullptr otherwise.
    Value *setCondition(Value *b);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    // K: disabled
    IfAlt(const IfAlt &);
    IfAlt &operator=(const IfAlt &);

    /// @brief Condition of the if alternative.
    Value *_condition;
};

} // namespace hif
