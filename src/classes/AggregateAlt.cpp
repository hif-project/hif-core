/// @file AggregateAlt.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/AggregateAlt.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

AggregateAlt::AggregateAlt()
    : indices()
    , _value(nullptr)
{
    _setBListParent(indices);
}

AggregateAlt::~AggregateAlt() { delete (_value); }

ClassId AggregateAlt::getClassId() const { return CLASSID_AGGREGATEALT; }

int AggregateAlt::acceptVisitor(HifVisitor &vis) { return vis.visitAggregateAlt(*this); }

void AggregateAlt::_calculateFields()
{
    Alt::_calculateFields();
    _addField(_value);
    _addBList(indices);
}

std::string AggregateAlt::_getFieldName(const Object *child) const
{
    if (child == _value)
        return "value";
    return Alt::_getFieldName(child);
}

std::string AggregateAlt::_getBListName(const BList<Object> &list) const
{
    if (&list == &indices.toOtherBList<Object>())
        return "indices";
    return Alt::_getBListName(list);
}

Value *AggregateAlt::getValue() const { return _value; }

Value *AggregateAlt::setValue(Value *x) { return setChild(_value, x); }

} // namespace hif
