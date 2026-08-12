/// @file When.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/WhenAlt.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/When.hpp"

namespace hif
{

When::When()
    : alts()
    , _defaultvalue(nullptr)
    , _logicTernary(false)
{
    _setBListParent(alts);
}

When::~When() { delete (_defaultvalue); }

Value *When::getDefault() const { return _defaultvalue; }

Value *When::setDefault(Value *x) { return setChild(_defaultvalue, x); }

bool When::isLogicTernary() const { return _logicTernary; }

void When::setLogicTernary(bool logicTernary) { _logicTernary = logicTernary; }

ClassId When::getClassId() const { return CLASSID_WHEN; }

int When::acceptVisitor(HifVisitor &vis) { return vis.visitWhen(*this); }

void When::_calculateFields()
{
    Value::_calculateFields();

    _addField(_defaultvalue);
    _addBList(alts);
}

std::string When::_getFieldName(const Object *child) const
{
    if (child == _defaultvalue)
        return "default";
    return Value::_getFieldName(child);
}

std::string When::_getBListName(const BList<Object> &list) const
{
    if (&list == &alts.toOtherBList<Object>())
        return "alts";
    return Value::_getBListName(list);
}

} // namespace hif
