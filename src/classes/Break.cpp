/// @file Break.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Break.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Break::Break()
    : _name(NameTable::getInstance()->none())
{
    // ntd
}

Break::~Break()
{
    // ntd
}

ClassId Break::getClassId() const { return CLASSID_BREAK; }

int Break::acceptVisitor(HifVisitor &vis) { return vis.visitBreak(*this); }

void Break::_calculateFields() { Action::_calculateFields(); }

Object *Break::toObject() { return this; }

} // namespace hif
