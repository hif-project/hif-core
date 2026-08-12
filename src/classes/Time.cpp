/// @file Time.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Time.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Time::Time()
{
    // ntd
}

Time::~Time()
{
    // ntd
}

ClassId Time::getClassId() const { return CLASSID_TIME; }

int Time::acceptVisitor(HifVisitor &vis) { return vis.visitTime(*this); }

void Time::_calculateFields() { SimpleType::_calculateFields(); }

} // namespace hif
