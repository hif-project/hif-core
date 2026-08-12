/// @file RecordValueAlt.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/RecordValueAlt.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

RecordValueAlt::RecordValueAlt()
    : _value(nullptr)
{
    // ntd
}

RecordValueAlt::~RecordValueAlt() { delete _value; }

ClassId RecordValueAlt::getClassId() const { return CLASSID_RECORDVALUEALT; }

int RecordValueAlt::acceptVisitor(HifVisitor &vis) { return vis.visitRecordValueAlt(*this); }

void RecordValueAlt::_calculateFields()
{
    Alt::_calculateFields();

    _addField(_value);
}

std::string RecordValueAlt::_getFieldName(const Object *child) const
{
    if (child == _value)
        return "value";
    return Alt::_getFieldName(child);
}

Value *RecordValueAlt::getValue() const { return _value; }

Value *RecordValueAlt::setValue(Value *x) { return setChild(_value, x); }

Object *RecordValueAlt::toObject() { return this; }

} // namespace hif
