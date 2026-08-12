/// @file TypeTPAssign.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/TypeTPAssign.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Type.hpp"

namespace hif
{

TypeTPAssign::TypeTPAssign()
    : _type(nullptr)
{
    // ntd
}

TypeTPAssign::~TypeTPAssign() { delete _type; }

ClassId TypeTPAssign::getClassId() const { return CLASSID_TYPETPASSIGN; }

int TypeTPAssign::acceptVisitor(HifVisitor &vis) { return vis.visitTypeTPAssign(*this); }

void TypeTPAssign::_calculateFields()
{
    TPAssign::_calculateFields();

    _addField(_type);
}

std::string TypeTPAssign::_getFieldName(const Object *child) const
{
    if (child == _type)
        return "type";
    return TPAssign::_getFieldName(child);
}

Type *TypeTPAssign::getType() const { return _type; }

Type *TypeTPAssign::setType(Type *x) { return setChild(_type, x); }

Object *TypeTPAssign::toObject() { return this; }

} // namespace hif
