/// @file SwitchAlt.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Action.hpp"
#include "hif/classes/Value.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/SwitchAlt.hpp"

namespace hif
{

SwitchAlt::SwitchAlt()
    : conditions()
    , actions()
{
    _setBListParent(conditions);
    _setBListParent(actions);
}

SwitchAlt::~SwitchAlt()
{
    // ntd
}

ClassId SwitchAlt::getClassId() const { return CLASSID_SWITCHALT; }

int SwitchAlt::acceptVisitor(HifVisitor &vis) { return vis.visitSwitchAlt(*this); }

void SwitchAlt::_calculateFields()
{
    Alt::_calculateFields();

    _addBList(conditions);
    _addBList(actions);
}

std::string SwitchAlt::_getBListName(const BList<Object> &list) const
{
    if (&list == &conditions.toOtherBList<Object>())
        return "conditions";
    if (&list == &actions.toOtherBList<Object>())
        return "actions";
    return Alt::_getBListName(list);
}

} // namespace hif
