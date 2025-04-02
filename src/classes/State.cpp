/// @file State.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstring>

#include "hif/HifVisitor.hpp"
#include "hif/classes/State.hpp"
#include "hif/classes/StateTable.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

State::State()
    : actions()
    , invariants()
    , _priority(0)
    , _atomic(false)
{
    _setBListParent(actions);
    _setBListParent(invariants);
}

State::~State()
{
    // ntd
}

State::EdgeList_t State::getInEdges()
{
    EdgeList_t ret;
    StateTable *st = dynamic_cast<StateTable *>(getParent());
    if (st != nullptr) {
        for (BList<Transition>::iterator i = st->edges.begin(); i != st->edges.end(); ++i) {
            if (getName() == (*i)->getName()) {
                ret.push_back(*i);
            }
        }
    }
    return ret;
}

State::EdgeList_t State::getOutEdges()
{
    EdgeList_t ret;
    StateTable *st = dynamic_cast<StateTable *>(getParent());
    if (st != nullptr) {
        for (BList<Transition>::iterator i = st->edges.begin(); i != st->edges.end(); ++i) {
            if (getName() == (*i)->getPrevName()) {
                ret.push_back(*i);
            }
        }
    }
    return ret;
}

void State::setPriority(const priority_t p) { _priority = p; }

State::priority_t State::getPriority() const { return _priority; }

void State::setAtomic(bool v) { _atomic = v; }

bool State::isAtomic() const { return _atomic; }

void State::_calculateFields()
{
    Declaration::_calculateFields();

    _addBList(actions);
    _addBList(invariants);
}

std::string State::_getBListName(const BList<Object> &list) const
{
    if (&list == &actions.toOtherBList<Object>())
        return "actions";
    if (&list == &invariants.toOtherBList<Object>())
        return "invariants";
    return Declaration::_getBListName(list);
}

ClassId State::getClassId() const { return CLASSID_STATE; }

int State::acceptVisitor(HifVisitor &vis) { return vis.visitState(*this); }

} // namespace hif
