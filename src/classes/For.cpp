/// @file For.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/DataDeclaration.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/For.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

For::For()
    : initDeclarations()
    , initValues()
    , stepActions()
    , forActions()
    , _condition(nullptr)
{
    _setBListParent(initDeclarations);
    _setBListParent(initValues);
    _setBListParent(stepActions);
    _setBListParent(forActions);
}

For::~For() { delete (_condition); }

ClassId For::getClassId() const { return CLASSID_FOR; }

int For::acceptVisitor(HifVisitor &vis) { return vis.visitFor(*this); }

void For::_calculateFields()
{
    Action::_calculateFields();

    _addField(_condition);
    _addBList(initDeclarations);
    _addBList(initValues);
    _addBList(stepActions);
    _addBList(forActions);
}

std::string For::_getFieldName(const Object *child) const
{
    if (child == _condition)
        return "condition";
    return Action::_getFieldName(child);
}

std::string For::_getBListName(const BList<Object> &list) const
{
    if (&list == &initDeclarations.toOtherBList<Object>())
        return "initDeclarations";
    if (&list == &initValues.toOtherBList<Object>())
        return "initValues";
    if (&list == &stepActions.toOtherBList<Object>())
        return "stepActions";
    if (&list == &forActions.toOtherBList<Object>())
        return "forActions";
    return Action::_getBListName(list);
}

Value *For::getCondition() const { return _condition; }

Value *For::setCondition(Value *x) { return setChild(_condition, x); }

hif::Object *hif::For::toObject() { return this; }

} // namespace hif
