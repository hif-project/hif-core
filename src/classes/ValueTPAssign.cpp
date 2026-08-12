/// @file ValueTPAssign.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/ValueTPAssign.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

ValueTPAssign::ValueTPAssign()
    : _value(nullptr)
{
    // ntd
}

ValueTPAssign::~ValueTPAssign() { delete (_value); }

ClassId ValueTPAssign::getClassId() const { return CLASSID_VALUETPASSIGN; }

int ValueTPAssign::acceptVisitor(HifVisitor &vis) { return vis.visitValueTPAssign(*this); }

void ValueTPAssign::_calculateFields()
{
    TPAssign::_calculateFields();

    _addField(_value);
}

std::string ValueTPAssign::_getFieldName(const Object *child) const
{
    if (child == _value)
        return "value";
    return TPAssign::_getFieldName(child);
}

Value *ValueTPAssign::getValue() const { return _value; }

Value *ValueTPAssign::setValue(Value *x) { return setChild(_value, x); }

Object *ValueTPAssign::toObject() { return this; }

} // namespace hif
