/// @file Port.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Port.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Port::Port()
    : _direction(dir_none)
    , _isWrapper(false)
{
    // ntd
}

Port::~Port()
{
    // ntd
}

PortDirection Port::getDirection() const { return _direction; }

void Port::setDirection(const PortDirection x) { _direction = x; }

ClassId Port::getClassId() const { return CLASSID_PORT; }

int Port::acceptVisitor(HifVisitor &vis) { return vis.visitPort(*this); }

void Port::_calculateFields() { DataDeclaration::_calculateFields(); }

bool Port::isWrapper() const { return _isWrapper; }

void Port::setWrapper(bool wrapper) { _isWrapper = wrapper; }

} // namespace hif
