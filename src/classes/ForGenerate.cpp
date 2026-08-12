/// @file ForGenerate.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Action.hpp"
#include "hif/classes/DataDeclaration.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/ForGenerate.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

ForGenerate::ForGenerate()
    : initDeclarations()
    , initValues()
    , stepActions()
    , _condition(nullptr)
{
    _setBListParent(initDeclarations);
    _setBListParent(initValues);
    _setBListParent(stepActions);
}
ForGenerate::~ForGenerate() { delete (_condition); }

ClassId ForGenerate::getClassId() const { return CLASSID_FORGENERATE; }

int ForGenerate::acceptVisitor(HifVisitor &vis) { return vis.visitForGenerate(*this); }

void ForGenerate::_calculateFields()
{
    Generate::_calculateFields();

    _addField(_condition);
    _addBList(initDeclarations);
    _addBList(initValues);
    _addBList(stepActions);
}

std::string ForGenerate::_getFieldName(const Object *child) const
{
    if (child == _condition)
        return "condition";
    return Generate::_getFieldName(child);
}

std::string ForGenerate::_getBListName(const BList<Object> &list) const
{
    if (&list == &initDeclarations.toOtherBList<Object>())
        return "initDeclarations";
    if (&list == &initValues.toOtherBList<Object>())
        return "initValues";
    if (&list == &stepActions.toOtherBList<Object>())
        return "stepActions";
    return Generate::_getBListName(list);
}

Value *ForGenerate::getCondition() const { return _condition; }

Value *ForGenerate::setCondition(Value *x) { return setChild(_condition, x); }

} // namespace hif
