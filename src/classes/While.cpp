/// @file While.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/While.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{
While::While()
    : actions()
    , _condition(nullptr)
    , _doWhile(false)
{
    _setBListParent(actions);
}

While::~While() { delete (_condition); }

Value *While::getCondition() const { return _condition; }

Value *While::setCondition(Value *x) { return setChild(_condition, x); }

bool While::isDoWhile() const { return _doWhile; }

void While::setDoWhile(bool doWhile) { _doWhile = doWhile; }

ClassId While::getClassId() const { return CLASSID_WHILE; }

int While::acceptVisitor(HifVisitor &vis) { return vis.visitWhile(*this); }

void While::_calculateFields()
{
    Action::_calculateFields();

    _addField(_condition);
    _addBList(actions);
}

std::string While::_getFieldName(const Object *child) const
{
    if (child == _condition)
        return "condition";
    return Action::_getFieldName(child);
}

std::string While::_getBListName(const BList<Object> &list) const
{
    if (&list == &actions.toOtherBList<Object>())
        return "action";
    return Action::_getBListName(list);
}

Object *While::toObject() { return this; }

} // namespace hif
