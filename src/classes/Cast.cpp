/// @file Cast.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Cast.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Type.hpp"

namespace hif
{

Cast::Cast()
    : _type(nullptr)
    , _value(nullptr)
{
    // ntd
}

Cast::~Cast()
{
    delete (_type);
    delete (_value);
}

ClassId Cast::getClassId() const { return CLASSID_CAST; }

int Cast::acceptVisitor(HifVisitor &vis) { return vis.visitCast(*this); }

void Cast::_calculateFields()
{
    Value::_calculateFields();
    _addField(_type);
    _addField(_value);
}

std::string Cast::_getFieldName(const Object *child) const
{
    if (child == _type)
        return "type";
    if (child == _value)
        return "value";
    return Value::_getFieldName(child);
}

Type *Cast::getType() const { return _type; }

Type *Cast::setType(Type *x) { return setChild(_type, x); }

Value *Cast::getValue() const { return _value; }

Value *Cast::setValue(Value *x) { return setChild(_value, x); }

} // namespace hif
