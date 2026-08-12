/// @file StateTable.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Value.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/State.hpp"
#include "hif/classes/StateTable.hpp"

namespace hif
{

StateTable::StateTable()
    : declarations()
    , sensitivity()
    , sensitivityPos()
    , sensitivityNeg()
    , states()
    , edges()
    , _entryState(NameTable::getInstance()->none())
    , _flavour(pf_hdl)
    , _dontInitialize(false)
    , _isStandard(false)
{
    _setBListParent(declarations);
    _setBListParent(sensitivity);
    _setBListParent(sensitivityPos);
    _setBListParent(sensitivityNeg);
    _setBListParent(states);
    _setBListParent(edges);
}

StateTable::~StateTable()
{
    // ntd
}

State *StateTable::getEntryState() { return findState(_entryState); }

void StateTable::setEntryState(State *s)
{
    if (s == nullptr)
        return;
    _entryState = s->getName();
}

std::string StateTable::getEntryStateName() { return _entryState; }

void StateTable::setEntryStateName(const std::string &s) { _entryState = s; }

State *StateTable::findState(const std::string &name)
{
    for (BList<State>::iterator i = states.begin(); i != states.end(); ++i) {
        if ((*i)->getName() == name) {
            return *i;
        }
    }
    return nullptr;
}

void StateTable::setFlavour(ProcessFlavour f) { _flavour = f; }

ProcessFlavour StateTable::getFlavour() const { return _flavour; }

void StateTable::setDontInitialize(bool dontInitialize) { _dontInitialize = dontInitialize; }

bool StateTable::getDontInitialize() const { return _dontInitialize; }

ClassId StateTable::getClassId() const { return CLASSID_STATETABLE; }

int StateTable::acceptVisitor(HifVisitor &vis) { return vis.visitStateTable(*this); }

bool StateTable::isStandard() const { return _isStandard; }

void StateTable::setStandard(bool standard) { _isStandard = standard; }

void StateTable::_calculateFields()
{
    Scope::_calculateFields();

    _addBList(declarations);
    _addBList(sensitivity);
    _addBList(sensitivityPos);
    _addBList(sensitivityNeg);
    _addBList(states);
    _addBList(edges);
}

std::string StateTable::_getBListName(const BList<Object> &list) const
{
    if (&list == &declarations.toOtherBList<Object>())
        return "declarations";
    if (&list == &sensitivity.toOtherBList<Object>())
        return "sensitivity";
    if (&list == &sensitivityPos.toOtherBList<Object>())
        return "sensitivityPos";
    if (&list == &sensitivityNeg.toOtherBList<Object>())
        return "sensitivityNeg";
    if (&list == &states.toOtherBList<Object>())
        return "states";
    if (&list == &edges.toOtherBList<Object>())
        return "edges";
    return Scope::_getBListName(list);
}

} // namespace hif
