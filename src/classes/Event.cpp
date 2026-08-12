/// @file Event.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Event.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

hif::Event::Event()
{
    // ntd
}

Event::~Event()
{
    // ntd
}

ClassId Event::getClassId() const { return CLASSID_EVENT; }

int Event::acceptVisitor(HifVisitor &vis) { return vis.visitEvent(*this); }

void Event::_calculateFields() { SimpleType::_calculateFields(); }

} // namespace hif
