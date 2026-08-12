/// @file Wait.hpp
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

/// @brief Wait statement.
///
/// @details
/// This class represents a wait statement, which is used to suspend the
/// execution of a process.
class Wait : public Action
{
public:
    /// @brief List of signals, ports and events on which to wait as OR list.
    BList<Value> sensitivity;

    /// @brief List of signals, ports and events on which to wait as OR list.
    BList<Value> sensitivityPos;

    /// @brief List of signals, ports and events on which to wait as OR list.
    BList<Value> sensitivityNeg;

    /// @brief List of actions to be executed when wait() is completed
    BList<Action> actions;

    /// @brief Constructor.
    Wait();

    /// @brief Destructor.
    virtual ~Wait();

    /// @brief Returns the condition of the wait statement.
    /// @return The condition of the wait statement.
    Value *getCondition();

    /// @brief Sets the condition of the wait statement.
    /// @param condition The condition of the wait statement.
    /// @return The old condition of the wait statement.
    Value *setCondition(Value *condition);

    /// @brief Sets the time value of the wait statement.
    /// @param tv The time value of the wait statement to be set.
    /// @return The old time value of the time statement.
    Value *setTime(Value *tv);

    /// @brief Returns the time value of the wait statement.
    /// @return The time value of the wait statement.
    Value *getTime();

    /// @brief Sets the repetitions of the wait statement.
    /// @param v The repetitions of the wait statement to be set.
    /// @return The old repetitions of the wait statement.
    Value *setRepetitions(Value *v);

    /// @brief Returns the repetitions of the wait statement.
    /// @return The repetitions of the wait statement.
    Value *getRepetitions();

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

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief The condition of the wait statement.
    Value *_condition;

    /// @brief The time value of the wait statement.
    Value *_timeValue;

    /// @brief The repetitions of the wait statement.
    Value *_repetitions;

    // K: disabled
    Wait(const Wait &);
    Wait &operator=(const Wait &);
};

} // namespace hif
