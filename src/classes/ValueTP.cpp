/// @file ValueTP.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/ValueTP.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

ValueTP::ValueTP()
    : _isCompileTimeConstant(true)
{
    // ntd
}

ValueTP::~ValueTP()
{
    // ntd
}

ClassId ValueTP::getClassId() const { return CLASSID_VALUETP; }

bool ValueTP::isCompileTimeConstant() const { return _isCompileTimeConstant; }

void ValueTP::setCompileTimeConstant(bool compileTimeConstant) { _isCompileTimeConstant = compileTimeConstant; }

int ValueTP::acceptVisitor(HifVisitor &vis) { return vis.visitValueTP(*this); }

void ValueTP::_calculateFields() { DataDeclaration::_calculateFields(); }

} // namespace hif
