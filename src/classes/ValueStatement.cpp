/// @file ValueStatement.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/ValueStatement.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

ValueStatement::ValueStatement()
    : _value(nullptr)
{
    // ntd
}

ValueStatement::~ValueStatement() { delete (_value); }

Value *ValueStatement::getValue() const { return _value; }

Value *ValueStatement::setValue(Value *x) { return setChild(_value, x); }

ClassId ValueStatement::getClassId() const { return CLASSID_VALUESTATEMENT; }

int ValueStatement::acceptVisitor(HifVisitor &vis) { return vis.visitValueStatement(*this); }

void ValueStatement::_calculateFields()
{
    Action::_calculateFields();

    _addField(_value);
}

std::string ValueStatement::_getFieldName(const Object *child) const
{
    if (child == _value)
        return "value";
    return Action::_getFieldName(child);
}

} // namespace hif
