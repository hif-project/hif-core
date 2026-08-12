/// @file ProcedureCall.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/ParameterAssign.hpp"
#include "hif/classes/TPAssign.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/ProcedureCall.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

ProcedureCall::ProcedureCall()
    : templateParameterAssigns()
    , parameterAssigns()
    , _instance(nullptr)
{
    _setBListParent(templateParameterAssigns);
    _setBListParent(parameterAssigns);
}

ProcedureCall::~ProcedureCall() { delete _instance; }

ClassId ProcedureCall::getClassId() const { return CLASSID_PROCEDURECALL; }

int ProcedureCall::acceptVisitor(HifVisitor &vis) { return vis.visitProcedureCall(*this); }

void ProcedureCall::_calculateFields()
{
    Action::_calculateFields();

    _addField(_instance);
    _addBList(templateParameterAssigns);
    _addBList(parameterAssigns);
}

std::string ProcedureCall::_getFieldName(const Object *child) const
{
    if (child == _instance)
        return "instance";
    return Action::_getFieldName(child);
}

std::string ProcedureCall::_getBListName(const BList<Object> &list) const
{
    if (&list == &templateParameterAssigns.toOtherBList<Object>())
        return "templateParameterAssigns";
    if (&list == &parameterAssigns.toOtherBList<Object>())
        return "parameterAssigns";
    return Action::_getBListName(list);
}

Value *ProcedureCall::getInstance() const { return _instance; }

Value *ProcedureCall::setInstance(Value *x) { return setChild(_instance, x); }

Object *ProcedureCall::toObject() { return this; }

} // namespace hif
