/// @file IntValue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/IntValue.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

ClassId IntValue::getClassId() const { return CLASSID_INTVALUE; }

int IntValue::acceptVisitor(HifVisitor &vis) { return vis.visitIntValue(*this); }

void IntValue::_calculateFields() { ConstValue::_calculateFields(); }

} // namespace hif
