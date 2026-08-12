/// @file Continue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Continue.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Continue::Continue()
{
    // ntd
}

Continue::~Continue()
{
    // ntd
}

ClassId Continue::getClassId() const { return CLASSID_CONTINUE; }

int Continue::acceptVisitor(HifVisitor &vis) { return vis.visitContinue(*this); }

void Continue::_calculateFields() { Action::_calculateFields(); }

Object *Continue::toObject() { return this; }

} // namespace hif
