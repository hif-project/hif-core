/// @file BaseContents.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Generate.hpp"
#include "hif/classes/Instance.hpp"
#include "hif/classes/StateTable.hpp"

#include "hif/classes/BaseContents.hpp"
#include "hif/classes/GlobalAction.hpp"

namespace hif
{

BaseContents::BaseContents()
    : declarations()
    , stateTables()
    , instances()
    , generates()
    , _globalAction(nullptr)
{
    _setBListParent(declarations);
    _setBListParent(stateTables);
    _setBListParent(instances);
    _setBListParent(generates);
}

BaseContents::~BaseContents() { delete (_globalAction); }

GlobalAction *BaseContents::getGlobalAction() const { return _globalAction; }

void BaseContents::_calculateFields()
{
    Scope::_calculateFields();
    _addField(_globalAction);
    _addBList(declarations);
    _addBList(stateTables);
    _addBList(instances);
    _addBList(generates);
}

std::string BaseContents::_getFieldName(const Object *child) const
{
    if (child == _globalAction)
        return "globalAction";
    return Scope::_getFieldName(child);
}

std::string BaseContents::_getBListName(const BList<Object> &list) const
{
    if (&list == &declarations.toOtherBList<Object>())
        return "declarations";
    if (&list == &stateTables.toOtherBList<Object>())
        return "stateTables";
    if (&list == &instances.toOtherBList<Object>())
        return "instances";
    if (&list == &generates.toOtherBList<Object>())
        return "generates";
    return Scope::_getBListName(list);
}

GlobalAction *BaseContents::setGlobalAction(GlobalAction *x) { return setChild(_globalAction, x); }

} // namespace hif
