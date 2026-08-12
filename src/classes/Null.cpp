/// @file Null.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Null.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Null::Null()
{
    // ntd
}

Null::~Null()
{
    // ntd
}

ClassId Null::getClassId() const { return CLASSID_NULL; }

int Null::acceptVisitor(HifVisitor &vis) { return vis.visitNull(*this); }

void Null::_calculateFields() { Action::_calculateFields(); }

} // namespace hif
