/// @file Switch.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/SwitchAlt.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/Switch.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

Switch::Switch()
    : alts()
    , defaults()
    , _condition(nullptr)
    , _caseSemantics(CASE_LITERAL)
{
    _setBListParent(alts);
    _setBListParent(defaults);
}

Switch::~Switch() { delete (_condition); }

Value *Switch::getCondition() const { return _condition; }

Value *Switch::setCondition(Value *x) { return setChild(_condition, x); }

CaseSemantics Switch::getCaseSemantics() const { return _caseSemantics; }

void Switch::setCaseSemantics(const CaseSemantics c) { _caseSemantics = c; }

ClassId Switch::getClassId() const { return CLASSID_SWITCH; }

int Switch::acceptVisitor(HifVisitor &vis) { return vis.visitSwitch(*this); }

void Switch::_calculateFields()
{
    Action::_calculateFields();

    _addField(_condition);
    _addBList(alts);
    _addBList(defaults);
}

std::string Switch::_getFieldName(const Object *child) const
{
    if (child == _condition)
        return "condition";
    return Action::_getFieldName(child);
}

std::string Switch::_getBListName(const BList<Object> &list) const
{
    if (&list == &alts.toOtherBList<Object>())
        return "alts";
    if (&list == &defaults.toOtherBList<Object>())
        return "defaults";
    return Action::_getBListName(list);
}

} // namespace hif
