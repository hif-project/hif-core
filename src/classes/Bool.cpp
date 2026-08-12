/// @file Bool.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Bool.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Bool::Bool()
{
    // ntd
}

Bool::~Bool()
{
    // ntd
}

ClassId Bool::getClassId() const { return CLASSID_BOOL; }

int Bool::acceptVisitor(HifVisitor &vis) { return vis.visitBool(*this); }

void Bool::_calculateFields() { SimpleType::_calculateFields(); }

} // namespace hif