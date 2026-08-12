/// @file FunctionCall.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/ParameterAssign.hpp"
#include "hif/classes/TPAssign.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/FunctionCall.hpp"
#include "hif/classes/Type.hpp"

namespace hif
{

FunctionCall::FunctionCall()
    : templateParameterAssigns()
    , parameterAssigns()
    , _instance(nullptr)
{
    _setBListParent(templateParameterAssigns);
    _setBListParent(parameterAssigns);
}

FunctionCall::~FunctionCall() { delete _instance; }

ClassId FunctionCall::getClassId() const { return CLASSID_FUNCTIONCALL; }

int FunctionCall::acceptVisitor(HifVisitor &vis) { return vis.visitFunctionCall(*this); }

void FunctionCall::_calculateFields()
{
    Value::_calculateFields();

    _addField(_instance);
    _addBList(templateParameterAssigns);
    _addBList(parameterAssigns);
}

std::string FunctionCall::_getFieldName(const Object *child) const
{
    if (child == _instance)
        return "instance";
    return Value::_getFieldName(child);
}

std::string FunctionCall::_getBListName(const BList<Object> &list) const
{
    if (&list == &templateParameterAssigns.toOtherBList<Object>())
        return "templateParameterAssigns";
    if (&list == &parameterAssigns.toOtherBList<Object>())
        return "parameterAssigns";
    return Value::_getBListName(list);
}

Value *FunctionCall::getInstance() const { return _instance; }

Value *FunctionCall::setInstance(Value *x) { return setChild(_instance, x); }

Object *FunctionCall::toObject() { return this; }

} // namespace hif
