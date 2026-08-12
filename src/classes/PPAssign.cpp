/// @file PPAssign.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>

#include "hif/application_utils/Log.hpp"
#include "hif/classes/PPAssign.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

PPAssign::PPAssign()
    : _value(nullptr)
    , _direction(dir_none)
{
    // ntd
}

PPAssign::~PPAssign() { delete (_value); }

void PPAssign::setDirection(const PortDirection d)
{
    if (d != dir_none) {
        messageError(
            "Try to set direction to port or parameter assign."
            "\nThis may be useful only for partial-designs but this is not supported yet!",
            nullptr, nullptr);
    }
    _direction = d;
}

PortDirection PPAssign::getDirection() const { return _direction; }

Value *PPAssign::getValue() const { return _value; }

Value *PPAssign::setValue(Value *x) { return setChild(_value, x); }

void PPAssign::_calculateFields()
{
    ReferencedAssign::_calculateFields();

    _addField(_value);
}

std::string PPAssign::_getFieldName(const Object *child) const
{
    if (child == _value)
        return "value";
    return ReferencedAssign::_getFieldName(child);
}

} // namespace hif
