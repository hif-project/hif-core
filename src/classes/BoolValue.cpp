/// @file BoolValue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/BoolValue.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

BoolValue::BoolValue()
    : _value(false)
{
    // ntd
}

BoolValue::BoolValue(bool b)
    : _value(b)
{
    // ntd
}

BoolValue::~BoolValue()
{
    // ntd
}

ClassId BoolValue::getClassId() const { return CLASSID_BOOLVALUE; }

int BoolValue::acceptVisitor(HifVisitor &vis) { return vis.visitBoolValue(*this); }

void BoolValue::_calculateFields() { ConstValue::_calculateFields(); }

bool BoolValue::getValue() const { return _value; }

void BoolValue::setValue(bool x) { _value = x; }

} // namespace hif
