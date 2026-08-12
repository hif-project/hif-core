/// @file Aggregate.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Aggregate.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Aggregate::Aggregate()
    : alts()
    , _others(nullptr)
{
    _setBListParent(alts);
}

Aggregate::~Aggregate() { delete (_others); }

ClassId Aggregate::getClassId() const { return CLASSID_AGGREGATE; }

int Aggregate::acceptVisitor(HifVisitor &vis) { return vis.visitAggregate(*this); }

void Aggregate::_calculateFields()
{
    Value::_calculateFields();
    _addField(_others);
    _addBList(alts);
}

std::string Aggregate::_getFieldName(const Object *child) const
{
    if (child == _others)
        return "others";
    return Value::_getFieldName(child);
}

std::string Aggregate::_getBListName(const BList<Object> &list) const
{
    if (&list == &alts.toOtherBList<Object>())
        return "alts";
    return Value::_getBListName(list);
}

Value *Aggregate::getOthers() const { return _others; }

Value *Aggregate::setOthers(Value *x) { return setChild(_others, x); }

} // namespace hif
