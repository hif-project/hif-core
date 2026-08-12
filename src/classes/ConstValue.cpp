/// @file ConstValue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/ConstValue.hpp"
#include "hif/classes/Type.hpp"

namespace hif
{

ConstValue::ConstValue()
    : Value()
    , _type(nullptr)
{
    // ntd
}

void ConstValue::_calculateFields()
{
    Value::_calculateFields();
    _addField(_type);
}

std::string ConstValue::_getFieldName(const Object *child) const
{
    if (child == _type)
        return "type";
    return Value::_getFieldName(child);
}

ConstValue::~ConstValue() { delete _type; }

Type *ConstValue::getType() const { return _type; }

Type *ConstValue::setType(Type *x) { return setChild(_type, x); }

} // namespace hif
