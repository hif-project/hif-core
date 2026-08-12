/// @file Char.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Char.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Char::Char()
{
    // ntd
}

Char::~Char()
{
    // ntd
}

ClassId Char::getClassId() const { return CLASSID_CHAR; }

int Char::acceptVisitor(HifVisitor &vis) { return vis.visitChar(*this); }

void Char::_calculateFields() { SimpleType::_calculateFields(); }

} // namespace hif
