/// @file EnumValue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/EnumValue.hpp"
#include "hif/HifVisitor.hpp"
namespace hif
{

EnumValue::EnumValue()
    : DataDeclaration()
{
    // ntd
}

EnumValue::~EnumValue()
{
    // ntd
}

ClassId EnumValue::getClassId() const { return CLASSID_ENUMVALUE; }

int EnumValue::acceptVisitor(HifVisitor &vis) { return vis.visitEnumValue(*this); }

void EnumValue::_calculateFields() { DataDeclaration::_calculateFields(); }

} // namespace hif
