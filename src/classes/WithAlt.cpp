/// @file WithAlt.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/WithAlt.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

WithAlt::WithAlt()
    : conditions()
    , _value(nullptr)
{
    _setBListParent(conditions);
}

WithAlt::~WithAlt() { delete (_value); }

Value *WithAlt::getValue() const { return _value; }

Value *WithAlt::setValue(Value *x) { return setChild(_value, x); }

ClassId WithAlt::getClassId() const { return CLASSID_WITHALT; }

int WithAlt::acceptVisitor(HifVisitor &vis) { return vis.visitWithAlt(*this); }

void WithAlt::_calculateFields()
{
    Alt::_calculateFields();

    _addField(_value);
    _addBList(conditions);
}

std::string WithAlt::_getFieldName(const Object *child) const
{
    if (child == _value)
        return "value";
    return Alt::_getFieldName(child);
}

std::string WithAlt::_getBListName(const BList<Object> &list) const
{
    if (&list == &conditions.toOtherBList<Object>())
        return "conditions";
    return Alt::_getBListName(list);
}

} // namespace hif
