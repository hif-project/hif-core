/// @file Alias.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Alias.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Alias::Alias()
    : _isStandard(false)
{
    // ntd
}

Alias::~Alias() {}

ClassId Alias::getClassId() const { return CLASSID_ALIAS; }

int Alias::acceptVisitor(HifVisitor &vis) { return vis.visitAlias(*this); }

void Alias::_calculateFields() { DataDeclaration::_calculateFields(); }

bool Alias::isStandard() const { return _isStandard; }

void Alias::setStandard(bool standard) { _isStandard = standard; }

} // namespace hif
