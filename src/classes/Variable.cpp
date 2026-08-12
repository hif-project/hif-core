/// @file Variable.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Variable.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Variable::Variable()
    : _isInstance(true)
    , _isStandard(false)
{
    // ntd
}

Variable::~Variable()
{
    // ntd
}

bool Variable::isInstance() const { return _isInstance; }

void Variable::setInstance(bool instance) { _isInstance = instance; }

ClassId Variable::getClassId() const { return CLASSID_VARIABLE; }

int Variable::acceptVisitor(HifVisitor &vis) { return vis.visitVariable(*this); }

void Variable::_calculateFields() { DataDeclaration::_calculateFields(); }

bool Variable::isStandard() const { return _isStandard; }

void Variable::setStandard(bool standard) { _isStandard = standard; }

} // namespace hif
