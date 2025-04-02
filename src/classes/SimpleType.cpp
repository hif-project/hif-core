/// @file SimpleType.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/SimpleType.hpp"

namespace hif
{

SimpleType::SimpleType()
    : _isConstexpr(false)
{
    // ntd
}

SimpleType::~SimpleType()
{
    // ntd
}

bool SimpleType::isConstexpr() const { return _isConstexpr; }

void SimpleType::setConstexpr(bool flag) { _isConstexpr = flag; }

void SimpleType::_calculateFields() { Type::_calculateFields(); }

} // namespace hif
