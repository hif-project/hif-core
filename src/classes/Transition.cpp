/// @file Transition.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Transition.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Expression.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

Transition::Transition()
    : enablingLabelList()
    , enablingList()
    , updateLabelList()
    , updateList()
    , _name(NameTable::getInstance()->none())
    , _prevName(NameTable::getInstance()->none())
    , _priority(0)
    , _enablingLabelOrMode(false)
{
    _setBListParent(enablingLabelList);
    _setBListParent(enablingList);
    _setBListParent(updateLabelList);
    _setBListParent(updateList);
}

Transition::~Transition()
{
    // ntd
}

std::string Transition::getName() const { return _name; }

void Transition::setName(const std::string &n) { _name = n; }

std::string Transition::getPrevName() const { return _prevName; }

void Transition::setPrevName(const std::string &n) { _prevName = n; }

void Transition::setPriority(const priority_t p) { _priority = p; }

Transition::priority_t Transition::getPriority() const { return _priority; }

void Transition::enablingListToExpression()
{
    if (enablingList.empty() || enablingList.size() == 1)
        return;
    Value *v = enablingList.front();
    BList<Value>::iterator i(enablingList.begin());
    i = i.erase();
    while (i != enablingList.end()) {
        Value *vv     = *i;
        Expression *e = new Expression();
        if (_enablingLabelOrMode)
            e->setOperator(op_or);
        else
            e->setOperator(op_and);
        e->setValue1(v);
        e->setValue2(vv);
        v = e;
        i = i.erase();
    }
    enablingList.push_back(v);
}

bool Transition::getEnablingOrCondition() const { return _enablingLabelOrMode; }

void Transition::_calculateFields()
{
    Action::_calculateFields();

    _addBList(enablingLabelList);
    _addBList(enablingList);
    _addBList(updateLabelList);
    _addBList(updateList);
}

std::string Transition::_getBListName(const BList<Object> &list) const
{
    if (&list == &enablingLabelList.toOtherBList<Object>())
        return "enablingLabelList";
    if (&list == &enablingList.toOtherBList<Object>())
        return "enablingList";
    if (&list == &updateLabelList.toOtherBList<Object>())
        return "updateLabelList";
    if (&list == &updateList.toOtherBList<Object>())
        return "updateList";
    return Action::_getBListName(list);
}

void Transition::setEnablingOrCondition(bool flag) { _enablingLabelOrMode = flag; }

ClassId Transition::getClassId() const { return CLASSID_TRANSITION; }

int Transition::acceptVisitor(HifVisitor &vis) { return vis.visitTransition(*this); }

} // namespace hif
