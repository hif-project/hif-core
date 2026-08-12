/// @file With.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/WithAlt.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/With.hpp"

namespace hif
{

With::With()
    : alts()
    , _condition(nullptr)
    , _defaultvalue(nullptr)
    , _caseSemantics(CASE_LITERAL)
{
    _setBListParent(alts);
}

With::~With()
{
    delete (_condition);
    delete (_defaultvalue);
}

Value *With::getCondition() const { return _condition; }

Value *With::setCondition(Value *x) { return setChild(_condition, x); }

Value *With::getDefault() const { return _defaultvalue; }

Value *With::setDefault(Value *x) { return setChild(_defaultvalue, x); }

CaseSemantics With::getCaseSemantics() const { return _caseSemantics; }

void With::setCaseSemantics(const CaseSemantics c) { _caseSemantics = c; }

ClassId With::getClassId() const { return CLASSID_WITH; }

int With::acceptVisitor(HifVisitor &vis) { return vis.visitWith(*this); }

void With::_calculateFields()
{
    Value::_calculateFields();

    _addField(_condition);
    _addField(_defaultvalue);
    _addBList(alts);
}

std::string With::_getFieldName(const Object *child) const
{
    if (child == _condition)
        return "condition";
    if (child == _defaultvalue)
        return "default";
    return Value::_getFieldName(child);
}

std::string With::_getBListName(const BList<Object> &list) const
{
    if (&list == &alts.toOtherBList<Object>())
        return "alts";
    return Value::_getBListName(list);
}

} // namespace hif
