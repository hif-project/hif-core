/// @file With.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Value.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Selected signal assignment.
///
/// @details
/// This class represents a VHDL selected signal assignment
/// (<tt>with / select</tt> construct).
/// According to the value of the expression featured in the <tt>with</tt>
/// construct, a corresponding value is assigned to the item featured in
/// the <tt>select</tt> construct.
/// The item featured in the <tt>select</tt> construct is stored as target
/// of an Assign object in HIF.
/// Conversely, the value featured in the <tt>with</tt> construct is stored
/// as the <tt>with</tt> expression.
/// The list of alternatives (WithAlt) contain a specific value which is
/// to be taken on by the <tt>with</tt> expression, and an associated value.
/// Additionally, a default value can be specified.
///
/// @see WithAlt
class With : public Value
{
public:
    /// @brief The alts type.
    typedef WithAlt AltType;

    /// @brief List of alternatives of the selected signal assignment.
    BList<WithAlt> alts;

    /// @brief Constructor.
    With();

    /// @brief Destructor.
    virtual ~With();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the expression on which selection is performed.
    /// @return The expression on which selection is performed.
    Value *getCondition() const;

    /// @brief Sets the expression on which selection is performed.
    /// @param v The expression on which selection is to be performed.
    /// @return The expression on which selection was previously performed.
    Value *setCondition(Value *v);

    /// @brief Returns the default value of the selected signal assignment.
    /// @return The default value of the selected signal assignment.
    Value *getDefault() const;

    /// @brief Sets the default value of the selected signal assignment.
    /// @param v The default value of the selected signal assignment.
    /// @return The previous default value of the selected signal assignment.
    Value *setDefault(Value *v);

    /// @brief Returns the case semantics used in current With.
    /// @return The case semantics.
    CaseSemantics getCaseSemantics() const;

    /// @brief Sets the case semantics used in current With.
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
    /// @brief The expression on which selection is performed.
    Value *_condition;

    /// @brief The default value of the selected signal assignment.
    Value *_defaultvalue;

    /// @brief The case semantics.
    CaseSemantics _caseSemantics;

    // K: disabled
    With(const With &);
    With &operator=(const With &);
};

} // namespace hif
