/// @file For.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Action.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/INamedObject.hpp"

namespace hif
{

/// @brief For loop.
///
/// @details
/// This class represents a for loop.
class For : public Action, public features::INamedObject
{
public:
    /// @brief List of for-loop-indices declarations.
    BList<DataDeclaration> initDeclarations;

    /// @brief List of actions performed in the initialization
    /// phase of the loop.
    BList<Action> initValues;

    /// @brief List of actions performed at the end of each iteration.
    BList<Action> stepActions;

    /// @brief List of actions that form the loop body.
    BList<Action> forActions;

    /// @brief Constructor.
    /// The name of the loop is set to <tt>NAME_NONE</tt>.
    For();

    /// @brief Destructor.
    virtual ~For();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the for loop condition.
    /// @return The for loop condition.
    virtual Value *getCondition() const;

    /// @brief Sets the for loop condition.
    /// @param cond The for loop condition to be set.
    /// @return The old for loop condition.
    virtual Value *setCondition(Value *cond);

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

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief The for loop condition.
    Value *_condition;

    // K: disabled
    For(const For &);
    For &operator=(const For &);
};

} // namespace hif
