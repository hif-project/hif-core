/// @file Pointer.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Pointer.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Pointer::Pointer()
{
    // ntd
}

Pointer::~Pointer()
{
    // ntd
}

ClassId Pointer::getClassId() const { return CLASSID_POINTER; }

int Pointer::acceptVisitor(HifVisitor &vis) { return vis.visitPointer(*this); }

void Pointer::_calculateFields() { CompositeType::_calculateFields(); }

} // namespace hif
