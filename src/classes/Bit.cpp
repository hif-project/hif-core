/// @file Bit.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Bit.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Bit::Bit()
    : _isLogic(false)
    , _isResolved(false)
{
    // ntd
}

Bit::~Bit()
{
    // ntd
}

ClassId Bit::getClassId() const { return CLASSID_BIT; }

int Bit::acceptVisitor(HifVisitor &vis) { return vis.visitBit(*this); }

void Bit::_calculateFields() { SimpleType::_calculateFields(); }

bool Bit::isLogic() const { return _isLogic; }

void Bit::setLogic(bool logic) { _isLogic = logic; }

bool Bit::isResolved() const { return _isResolved; }

void Bit::setResolved(bool resolved) { _isResolved = resolved; }

} // namespace hif
