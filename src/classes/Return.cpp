/// @file Return.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Return.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

Return::Return()
    : _value(nullptr)
{
    // ntd
}

Return::~Return() { delete (_value); }

Value *Return::getValue() const { return _value; }

Value *Return::setValue(Value *x) { return setChild(_value, x); }

ClassId Return::getClassId() const { return CLASSID_RETURN; }

int Return::acceptVisitor(HifVisitor &vis) { return vis.visitReturn(*this); }

void Return::_calculateFields()
{
    Action::_calculateFields();

    _addField(_value);
}

std::string Return::_getFieldName(const Object *child) const
{
    if (child == _value)
        return "value";
    return Action::_getFieldName(child);
}

} // namespace hif
