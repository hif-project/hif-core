/// @file IfAlt.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Action.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/IfAlt.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

IfAlt::IfAlt()
    : actions()
    , _condition(nullptr)
{
    _setBListParent(actions);
}

IfAlt::~IfAlt() { delete (_condition); }

ClassId IfAlt::getClassId() const { return CLASSID_IFALT; }

int IfAlt::acceptVisitor(HifVisitor &vis) { return vis.visitIfAlt(*this); }

void IfAlt::_calculateFields()
{
    Alt::_calculateFields();

    _addField(_condition);
    _addBList(actions);
}

std::string IfAlt::_getFieldName(const Object *child) const
{
    if (child == _condition)
        return "condition";
    return Alt::_getFieldName(child);
}

std::string IfAlt::_getBListName(const BList<Object> &list) const
{
    if (&list == &actions.toOtherBList<Object>())
        return "actions";
    return Alt::_getBListName(list);
}

Value *IfAlt::getCondition() const { return _condition; }

Value *IfAlt::setCondition(Value *x) { return setChild(_condition, x); }

} // namespace hif
