/// @file TypeTP.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/TypeTP.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

TypeTP::TypeTP()
{
    // ntd
}

TypeTP::~TypeTP()
{
    // ntd
}

ClassId TypeTP::getClassId() const { return CLASSID_TYPETP; }

int TypeTP::acceptVisitor(HifVisitor &vis) { return vis.visitTypeTP(*this); }

void TypeTP::_calculateFields() { TypeDeclaration::_calculateFields(); }

} // namespace hif
