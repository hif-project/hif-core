/// @file Switch.hpp
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

/// @brief Switch statement.
///
/// @details
/// This class represents a switch statement.
/// It consists of the switch value, a list of possible case alternatives
/// (SwitchAlt) and an optional default list of actions.
///
/// @see SwitchAlt
class Switch : public Action
{
public:
    /// @brief The alts type.
    typedef SwitchAlt AltType;

    /// @brief The list of alternatives representing the different cases.
    BList<SwitchAlt> alts;

    /// @brief The default list of actions, to be executed when no case
    /// in the alternatives is matched.
    BList<Action> defaults;

    /// @brief Constructor.
    Switch();

    /// @brief Destructor.
    virtual ~Switch();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the switch value on which case selection is performed.
    /// @return The switch value.
    Value *getCondition() const;

    /// @brief Sets the switch value on which case selection is performed.
    /// @param v The switch value to be set.
    /// @return The old switch value, or nullptr if none.
    Value *setCondition(Value *v);

    /// @brief Returns the case semantics used in current Switch.
    /// @return The case semantics.
    CaseSemantics getCaseSemantics() const;

    /// @brief Sets the case semantics used in current Switch.
    /// @param c The case semantics to be set.
    void setCaseSemantics(const CaseSemantics c);

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
    /// @brief The switch value on which case selection is performed.
    Value *_condition;

    /// @brief The case semantics.
    CaseSemantics _caseSemantics;

    // K: disabled
    Switch(const Switch &);
    Switch &operator=(const Switch &);
};

} // namespace hif
