/// @file ReferencedType.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/ReferencedType.hpp"

namespace hif
{

ReferencedType::ReferencedType()
    : _instance(nullptr)
{
    // ntd
}

ReferencedType::~ReferencedType() { delete _instance; }

ReferencedType *ReferencedType::getInstance() const { return _instance; }

void ReferencedType::_calculateFields()
{
    Type::_calculateFields();

    _addField(_instance);
}

std::string ReferencedType::_getFieldName(const Object *child) const
{
    if (child == _instance)
        return "instance";
    return Type::_getFieldName(child);
}

ReferencedType *ReferencedType::setInstance(ReferencedType *x) { return setChild(_instance, x); }

} // namespace hif
