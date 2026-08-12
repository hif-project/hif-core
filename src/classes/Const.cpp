/// @file Const.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Const.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Const::Const()
    : _isInstance(true)
    , _isDefine(false)
    , _isStandard(false)
{
    // ntd
}

Const::~Const()
{
    // ntd
}

bool Const::isInstance() const { return _isInstance; }

void Const::setInstance(bool instance) { _isInstance = instance; }

ClassId Const::getClassId() const { return CLASSID_CONST; }

int Const::acceptVisitor(HifVisitor &vis) { return vis.visitConst(*this); }

void Const::_calculateFields() { DataDeclaration::_calculateFields(); }

void Const::setDefine(bool define) { _isDefine = define; }

bool Const::isDefine() const { return _isDefine; }

bool Const::isStandard() const { return _isStandard; }

void Const::setStandard(bool standard) { _isStandard = standard; }

} // namespace hif
