/// @file WhenAlt.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/WhenAlt.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

WhenAlt::WhenAlt()
    : _condition(nullptr)
    , _value(nullptr)
{
    // ntd
}

WhenAlt::~WhenAlt()
{
    delete (_condition);
    delete (_value);
}

Value *WhenAlt::getCondition() const { return _condition; }

Value *WhenAlt::setCondition(Value *x) { return setChild(_condition, x); }

Value *WhenAlt::getValue() const { return _value; }

Value *WhenAlt::setValue(Value *x) { return setChild(_value, x); }

ClassId WhenAlt::getClassId() const { return CLASSID_WHENALT; }

int WhenAlt::acceptVisitor(HifVisitor &vis) { return vis.visitWhenAlt(*this); }

void WhenAlt::_calculateFields()
{
    Alt::_calculateFields();

    _addField(_condition);
    _addField(_value);
}

std::string WhenAlt::_getFieldName(const Object *child) const
{
    if (child == _condition)
        return "condition";
    if (child == _value)
        return "value";
    return Alt::_getFieldName(child);
}

} // namespace hif
