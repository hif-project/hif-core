/// @file TypeDeclaration.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/TypeDeclaration.hpp"
#include "hif/classes/Type.hpp"

namespace hif
{

TypeDeclaration::TypeDeclaration()
    : Scope()
    , _type(nullptr)
{
    // ntd
}

TypeDeclaration::~TypeDeclaration() { delete (_type); }

Type *TypeDeclaration::getType() const { return _type; }

Type *TypeDeclaration::setType(Type *x) { return setChild(_type, x); }

void TypeDeclaration::_calculateFields()
{
    Scope::_calculateFields();

    _addField(_type);
}

std::string TypeDeclaration::_getFieldName(const Object *child) const
{
    if (child == _type)
        return "type";
    return Scope::_getFieldName(child);
}

} // namespace hif
