/// @file Function.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Function.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Type.hpp"

namespace hif
{

Function::Function()
    : _type(nullptr)
{
    // ntd
}

Function::~Function() { delete (_type); }

ClassId Function::getClassId() const { return CLASSID_FUNCTION; }

int Function::acceptVisitor(HifVisitor &vis) { return vis.visitFunction(*this); }

void Function::_calculateFields()
{
    SubProgram::_calculateFields();
    _addField(_type);
}

std::string Function::_getFieldName(const Object *child) const
{
    if (child == _type)
        return "type";
    return SubProgram::_getFieldName(child);
}

Type *Function::getType() const { return _type; }

Type *Function::setType(Type *x) { return setChild(_type, x); }

} // namespace hif
