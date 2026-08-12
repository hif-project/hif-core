/// @file PortAssign.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>

#include "hif/HifVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/classes/PortAssign.hpp"

namespace hif
{

PortAssign::PortAssign()
    : _type(nullptr)
    , _partialBind(nullptr)
{
    // ntd
}
PortAssign::~PortAssign()
{
    delete _type;
    delete _partialBind;
}

Type *PortAssign::getType() const { return _type; }

Type *PortAssign::setType(Type *x)
{
    if (x != nullptr)
        reinterpret_cast<PortAssign *>(x)->_setParent(this);
    if (_type != x) {
        messageError(
            "Try to set type to port assign."
            "\nThis may be useful only for partial-designs but this is not supported yet!",
            nullptr, nullptr);
    }
    return nullptr;
}

Value *PortAssign::getPartialBind() const { return _partialBind; }

Value *PortAssign::setPartialBind(Value *pB) { return setChild(_partialBind, pB); }
ClassId PortAssign::getClassId() const { return CLASSID_PORTASSIGN; }

int PortAssign::acceptVisitor(HifVisitor &vis) { return vis.visitPortAssign(*this); }

void PortAssign::_calculateFields()
{
    PPAssign::_calculateFields();

    _addField(_type);
    _addField(_partialBind);
}

std::string PortAssign::_getFieldName(const Object *child) const
{
    if (child == _type)
        return "type";
    if (child == _partialBind)
        return "partialBind";
    return PPAssign::_getFieldName(child);
}

Object *PortAssign::toObject() { return this; }

} // namespace hif
