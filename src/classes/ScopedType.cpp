/// @file ScopedType.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/ScopedType.hpp"

namespace hif
{

ScopedType::ScopedType()
    : _isConstexpr(false)
{
    // ntd
}

ScopedType::~ScopedType()
{
    // ntd
}

void ScopedType::_calculateFields() { Type::_calculateFields(); }

void ScopedType::setConstexpr(bool v) { _isConstexpr = v; }

bool ScopedType::isConstexpr() { return _isConstexpr; }

} // namespace hif
