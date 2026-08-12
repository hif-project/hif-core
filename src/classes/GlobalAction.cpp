/// @file GlobalAction.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Action.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/GlobalAction.hpp"

namespace hif
{

GlobalAction::GlobalAction()
    : actions()
{
    _setBListParent(actions);
}

GlobalAction::~GlobalAction()
{
    // ntd
}

ClassId GlobalAction::getClassId() const { return CLASSID_GLOBALACTION; }

int GlobalAction::acceptVisitor(HifVisitor &vis) { return vis.visitGlobalAction(*this); }

void GlobalAction::_calculateFields()
{
    Object::_calculateFields();
    _addBList(actions);
}

std::string GlobalAction::_getBListName(const BList<Object> &list) const
{
    if (&list == &actions.toOtherBList<Object>())
        return "actions";
    return Object::_getBListName(list);
}

} // namespace hif
