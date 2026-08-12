/// @file Field.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Field.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Field::Field()
    : _direction(hif::dir_none)
{
    // ntd
}

Field::~Field()
{
    // ntd
}

PortDirection Field::getDirection() const { return _direction; }

void Field::setDirection(const PortDirection d) { _direction = d; }

ClassId Field::getClassId() const { return CLASSID_FIELD; }

int Field::acceptVisitor(HifVisitor &vis) { return vis.visitField(*this); }

void Field::_calculateFields() { DataDeclaration::_calculateFields(); }

} // namespace hif
