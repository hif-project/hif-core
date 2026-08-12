/// @file If.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/IfAlt.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/If.hpp"

namespace hif
{

If::If()
    : alts()
    , defaults()
{
    _setBListParent(alts);
    _setBListParent(defaults);
}

If::~If()
{
    // ntd
}

ClassId If::getClassId() const { return CLASSID_IF; }

int If::acceptVisitor(HifVisitor &vis) { return vis.visitIf(*this); }

void If::_calculateFields()
{
    Action::_calculateFields();
    _addBList(alts);
    _addBList(defaults);
}

std::string If::_getBListName(const BList<Object> &list) const
{
    if (&list == &alts.toOtherBList<Object>())
        return "alts";
    if (&list == &defaults.toOtherBList<Object>())
        return "defaults";
    return Action::_getBListName(list);
}

} // namespace hif
