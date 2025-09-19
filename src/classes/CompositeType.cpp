/// @file CompositeType.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/CompositeType.hpp"

namespace hif
{

CompositeType::CompositeType()
    : _type(nullptr)
    , _baseType(nullptr)
    , _baseOpaqueType(nullptr)
{
    // ntd
}

CompositeType::~CompositeType()
{
    delete _type;
    delete _baseType;
    delete _baseOpaqueType;
}

Type *CompositeType::getType() const { return _type; }

Type *CompositeType::setType(Type *x) { return setChild(_type, x); }

Type *CompositeType::getBaseType(bool considerOpacity) const
{
    return considerOpacity ? _baseOpaqueType : _baseType;
}

Type *CompositeType::setBaseType(Type *x, bool considerOpacity)
{
    if (considerOpacity) {
        return setChild(_baseOpaqueType, x);
    } else {
        return setChild(_baseType, x);
    }
}

void CompositeType::_calculateFields()
{
    Type::_calculateFields();
    _addField(_type);
}

std::string CompositeType::_getFieldName(const Object *child) const
{
    if (child == _type)
        return "type";
    return Type::_getFieldName(child);
}

} // namespace hif
