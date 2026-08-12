/// @file Expression.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Value.hpp"
#include "hif/hifEnums.hpp"

namespace hif
{

/// @brief Unary or binary expression.
///
/// @details
/// This class represents a unary or binary expression.
/// An expression consists of an operator and one or two operands.
class Expression : public Value
{
public:
    /// @brief Constructor.
    Expression();

    /// @brief Constructor.
    /// @param o The operator. Default value is <tt>op_none</tt>.
    /// @param op1 The first operand. Default value is <tt>nullptr</tt>.
    /// @param op2 The second operand. Default value is <tt>nullptr</tt>.
    Expression(Operator o, Value *op1, Value *op2 = nullptr);

    /// @brief Destructor.
    virtual ~Expression();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the operator of the expression.
    /// @return The operator of the expression.
    Operator getOperator() const;

    /// @brief Sets the operator of the expression.
    /// @param o The operand of the expression to be set.
    void setOperator(const Operator o);

    /// @brief Returns the first operand of the expression.
    /// @return The first operand of the expression.
    Value *getValue1() const;

    /// @brief Sets the first operand of the expression.
    /// @param v The first operand of the expression to be set.
    /// @return The old first operand of the expression if it is different
    /// from the new one, nullptr otherwise.
    Value *setValue1(Value *v);

    /// @brief Returns the second operand of the expression.
    /// @return The second operand of the expression.
    Value *getValue2() const;

    /// @brief Sets the second operand of the expression.
    /// @param v The second operand of the expression to be set.
    /// @return The old second operand of the expression if it is different
    /// from the new one, nullptr otherwise.
    Value *setValue2(Value *v);

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

private:
    /// @brief The operator of the expression.
    Operator _operator;

    /// @brief The first operand of the expression.
    Value *_value1;

    /// @brief The second operand of the expression.
    Value *_value2;

    /// @brief Reference to the object where the expression is declared.
    Object *_declaration;

    // K: disabled
    Expression(const Expression &);
    Expression &operator=(const Expression &);
};

} // namespace hif
