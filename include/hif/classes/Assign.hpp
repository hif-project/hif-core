/// @file Assign.hpp
/// @brief Assignment statement for HIF objects.
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

///	@brief Assignment statement.
/// @details
/// This class represents an assignment statement.
/// It describes assignments to variables, signals or ports.
class Assign : public Action
{
public:
    /// @brief Constructor.
    Assign();

    /// @brief Destructor.
    virtual ~Assign();

    Assign(const Assign &)                     = delete;
    auto operator=(const Assign &) -> Assign & = delete;

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the the right-hand side of the assignment.
    /// @return The right-hand side of the assignment.
    Value *getRightHandSide() const;

    /// @brief Sets the right-hand side of the assignment.
    /// @param v The new right-hand side of the assignment.
    /// @return The old right-hand side of the assignment if it is different
    /// from the new one, nullptr otherwise.
    Value *setRightHandSide(Value *v);

    /// @brief Returns the left-hand side of the assignment.
    /// @return The left-hand side of the assignment.
    Value *getLeftHandSide() const;

    /// @brief Sets the left-hand side of the assignment.
    /// @param v The new left-hand side of the assignment.
    /// @return The old left-hand side of the assignment if it is different
    /// from the new one, nullptr otherwise.
    Value *setLeftHandSide(Value *v);

    /// @brief Returns the delay after which the assignment will be performed.
    /// @return The delay of the assignment, or <tt>nullptr</tt> in case the
    /// assignment is not delayed.
    Value *getDelay() const;

    /// @brief Sets the delay after which the assignment will be performed.
    /// @param tv The delay.
    /// @return The old delay of the assignment if it is different from the
    /// new one, nullptr otherwise.
    Value *setDelay(Value *tv);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of the given child relative to this object.
    /// @param child Pointer to the child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    /// @brief The source (i.e., the right-hand side) of the assignment.
    Value *_leftHandSide;
    /// @brief The target (i.e., the left-hand side) of the assignment.
    Value *_rightHandSide;
    /// @brief The assignment delay.
    Value *_delay;
};

} // namespace hif
