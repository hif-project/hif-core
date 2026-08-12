/// @file Wait.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/TimeValue.hpp"
#include "hif/classes/Value.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/Wait.hpp"

namespace hif
{

Wait::Wait()
    : Action()
    , sensitivity()
    , sensitivityPos()
    , sensitivityNeg()
    , actions()
    , _condition(nullptr)
    , _timeValue(nullptr)
    , _repetitions(nullptr)
{
    _setBListParent(sensitivity);
    _setBListParent(sensitivityPos);
    _setBListParent(sensitivityNeg);
    _setBListParent(actions);
}

Wait::~Wait()
{
    delete _condition;
    delete _timeValue;
    delete _repetitions;
}

Value *Wait::getCondition() { return _condition; }

Value *Wait::setCondition(Value *condition) { return setChild(_condition, condition); }

Value *Wait::setTime(Value *tv) { return setChild(_timeValue, tv); }

Value *Wait::getTime() { return _timeValue; }

Value *Wait::setRepetitions(Value *v) { return setChild(_repetitions, v); }

Value *Wait::getRepetitions() { return _repetitions; }

ClassId Wait::getClassId() const { return CLASSID_WAIT; }

int Wait::acceptVisitor(HifVisitor &vis) { return vis.visitWait(*this); }

void Wait::_calculateFields()
{
    Action::_calculateFields();

    _addField(_condition);
    _addField(_timeValue);
    _addField(_repetitions);
    _addBList(sensitivity);
    _addBList(sensitivityPos);
    _addBList(sensitivityNeg);
    _addBList(actions);
}

std::string Wait::_getFieldName(const Object *child) const
{
    if (child == _condition)
        return "condition";
    if (child == _timeValue)
        return "timeValue";
    if (child == _repetitions)
        return "repetitions";
    return Action::_getFieldName(child);
}

std::string Wait::_getBListName(const BList<Object> &list) const
{
    if (&list == &sensitivity.toOtherBList<Object>())
        return "sensitivity";
    if (&list == &sensitivityPos.toOtherBList<Object>())
        return "sensitivityPos";
    if (&list == &sensitivityNeg.toOtherBList<Object>())
        return "sensitivityNeg";
    if (&list == &actions.toOtherBList<Object>())
        return "actions";
    return Action::_getBListName(list);
}

} // namespace hif
