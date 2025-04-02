/// @file While.hpp
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

/// @brief While loop.
///
/// @details
/// This class represents a while loop.
class While : public Action, public features::INamedObject
{
public:
    /// @brief List of actions that form the loop body.
    BList<Action> actions;

    /// @brief Constructor.
    While();

    /// @brief Destructor.
    virtual ~While();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the while loop condition.
    /// @return The while loop condition.
    Value *getCondition() const;

    /// @brief Sets the while loop condition.
    /// @param b The while loop condition to be set.
    /// @return The old while loop condition.
    Value *setCondition(Value *b);

    /// @brief Returns the <tt>doWhile</tt> attribute.
    /// @return The <tt>doWhile</tt> attribute.
    bool isDoWhile() const;

    /// @brief Sets the <tt>doWhile</tt> attribute.
    /// @param doWhile The <tt>doWhile</tt> attribute.
    void setDoWhile(bool doWhile);

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
    virtual std::string _getFieldName(const Object *child) const;

    /// @brief Returns the name of given BList w.r.t. this.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief The while loop condition.
    Value *_condition;

    /// @brief Identify if costruct is a while or a do-while.
    bool _doWhile;

    // K: disabled
    While(const While &);
    While &operator=(const While &);
};

} // namespace hif
