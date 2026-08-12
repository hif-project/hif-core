/// @file Expression.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Expression.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Expression::Expression()
    : _operator(op_none)
    , _value1(nullptr)
    , _value2(nullptr)
    , _declaration(nullptr)
{
    // ntd
}

Expression::Expression(Operator o, Value *op1, Value *op2)
    : _operator(o)
    , _value1(nullptr)
    , _value2(nullptr)
    , _declaration(nullptr)
{
    setValue1(op1);
    setValue2(op2);
}

Expression::~Expression()
{
    delete (_value1);
    delete (_value2);
}

ClassId Expression::getClassId() const { return CLASSID_EXPRESSION; }

int Expression::acceptVisitor(HifVisitor &vis) { return vis.visitExpression(*this); }

void Expression::_calculateFields()
{
    Value::_calculateFields();
    _addField(_value1);
    _addField(_value2);
}

std::string Expression::_getFieldName(const Object *child) const
{
    if (child == _value1)
        return "value1";
    if (child == _value2)
        return "value2";
    return Value::_getFieldName(child);
}

Operator Expression::getOperator() const { return _operator; }

void Expression::setOperator(const Operator x) { _operator = x; }

Value *Expression::getValue1() const { return _value1; }

Value *Expression::setValue1(Value *x) { return setChild(_value1, x); }

Value *Expression::getValue2() const { return _value2; }

Value *Expression::setValue2(Value *x) { return setChild(_value2, x); }

} // namespace hif
