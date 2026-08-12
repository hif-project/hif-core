/// @file Reference.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Reference.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Reference::Reference()
{
    // ntd
}

Reference::~Reference()
{
    // ntd
}

ClassId Reference::getClassId() const { return CLASSID_REFERENCE; }

int Reference::acceptVisitor(HifVisitor &vis) { return vis.visitReference(*this); }

void Reference::_calculateFields() { CompositeType::_calculateFields(); }

} // namespace hif
