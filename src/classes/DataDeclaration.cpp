/// @file DataDeclaration.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/DataDeclaration.hpp"
#include "hif/classes/Range.hpp"
#include "hif/classes/State.hpp"
#include "hif/classes/Type.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

DataDeclaration::DataDeclaration()
    : _type(nullptr)
    , _value(nullptr)
    , _range(nullptr)
{
    // ntd
}

DataDeclaration::~DataDeclaration()
{
    delete _type;
    delete _value;
    delete _range;
}

Type *DataDeclaration::getType() const { return _type; }

Type *DataDeclaration::setType(Type *x) { return setChild(_type, x); }

Value *DataDeclaration::getValue() const { return _value; }

Value *DataDeclaration::setValue(Value *x) { return setChild(_value, x); }

Range *DataDeclaration::getRange() const { return _range; }

Range *DataDeclaration::setRange(Range *x) { return setChild(_range, x); }

void DataDeclaration::_calculateFields()
{
    Declaration::_calculateFields();
    _addField(_type);
    _addField(_value);
    _addField(_range);
}

std::string DataDeclaration::_getFieldName(const Object *child) const
{
    if (child == _type)
        return "type";
    if (child == _value)
        return "value";
    if (child == _range)
        return "range";
    return Declaration::_getFieldName(child);
}

} // namespace hif
