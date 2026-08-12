/// @file RealValue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/RealValue.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

RealValue::RealValue()
    : _value(0.0)
{
    // ntd
}

RealValue::RealValue(const double d)
    : _value(d)
{
    // ntd
}

RealValue::~RealValue()
{
    // ntd
}

double RealValue::getValue() const { return _value; }

void RealValue::setValue(const double d) { _value = d; }

ClassId RealValue::getClassId() const { return CLASSID_REALVALUE; }

int RealValue::acceptVisitor(HifVisitor &vis) { return vis.visitRealValue(*this); }

void RealValue::_calculateFields() { ConstValue::_calculateFields(); }

} // namespace hif
