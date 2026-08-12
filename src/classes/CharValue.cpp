/// @file CharValue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/CharValue.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

CharValue::CharValue()
    : _value(0)
{
    // ntd
}

CharValue::CharValue(const char c)
    : _value(c)
{
    // ntd
}

CharValue::~CharValue()
{
    // ntd
}

ClassId CharValue::getClassId() const { return CLASSID_CHARVALUE; }

int CharValue::acceptVisitor(HifVisitor &vis) { return vis.visitCharValue(*this); }

void CharValue::_calculateFields() { ConstValue::_calculateFields(); }

char CharValue::getValue() const { return _value; }

void CharValue::setValue(const char x) { _value = x; }

} // namespace hif
