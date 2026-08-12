/// @file Instance.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/PortAssign.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/Instance.hpp"
#include "hif/classes/ReferencedType.hpp"

namespace hif
{

Instance::Instance()
    : portAssigns()
    , _referencedType(nullptr)
    , _initialValue(nullptr)
{
    _setBListParent(portAssigns);
}

Instance::~Instance() { delete _referencedType; }

ClassId Instance::getClassId() const { return CLASSID_INSTANCE; }

int Instance::acceptVisitor(HifVisitor &vis) { return vis.visitInstance(*this); }

void Instance::_calculateFields()
{
    Value::_calculateFields();

    _addField(_referencedType);
    _addField(_initialValue);
    _addBList(portAssigns);
}

std::string Instance::_getFieldName(const Object *child) const
{
    if (child == _referencedType)
        return "referencedType";
    if (child == _initialValue)
        return "initialValue";
    return Value::_getFieldName(child);
}

std::string Instance::_getBListName(const BList<Object> &list) const
{
    if (&list == &portAssigns.toOtherBList<Object>())
        return "portAssigns";
    return Value::_getBListName(list);
}

ReferencedType *Instance::getReferencedType() const { return _referencedType; }

ReferencedType *Instance::setReferencedType(ReferencedType *x) { return setChild(_referencedType, x); }

Value *Instance::getValue() const { return _initialValue; }

Value *Instance::setValue(Value *x) { return setChild(_initialValue, x); }

Object *Instance::toObject() { return this; }

} // namespace hif
