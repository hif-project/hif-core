/// @file Parameter.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Parameter.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Parameter::Parameter()
    : _direction(dir_none)
{
    // ntd
}

Parameter::~Parameter()
{
    // ntd
}

PortDirection Parameter::getDirection() const { return _direction; }

void Parameter::setDirection(PortDirection x) { _direction = x; }

ClassId Parameter::getClassId() const { return CLASSID_PARAMETER; }

int Parameter::acceptVisitor(HifVisitor &vis) { return vis.visitParameter(*this); }

void Parameter::_calculateFields() { DataDeclaration::_calculateFields(); }

} // namespace hif
