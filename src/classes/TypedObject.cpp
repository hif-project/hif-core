/// @file TypedObject.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/TypedObject.hpp"
#include "hif/classes/Type.hpp"

namespace hif
{

TypedObject::TypedObject()
    : _semanticsType(nullptr)
{
    // ntd
}

TypedObject::~TypedObject() { delete _semanticsType; }

Type *TypedObject::getSemanticType() const { return _semanticsType; }

Type *TypedObject::setSemanticType(Type *t) { return setChild(_semanticsType, t); }

void TypedObject::_calculateFields() { Object::_calculateFields(); }

} // namespace hif
