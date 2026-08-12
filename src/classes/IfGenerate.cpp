/// @file IfGenerate.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/IfGenerate.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

IfGenerate::IfGenerate()
    : _condition(nullptr)
{
    // ntd
}

IfGenerate::~IfGenerate() { delete (_condition); }

ClassId IfGenerate::getClassId() const { return CLASSID_IFGENERATE; }

int IfGenerate::acceptVisitor(HifVisitor &vis) { return vis.visitIfGenerate(*this); }

void IfGenerate::_calculateFields()
{
    Generate::_calculateFields();

    _addField(_condition);
}

std::string IfGenerate::_getFieldName(const Object *child) const
{
    if (child == _condition)
        return "condition";
    return Generate::_getFieldName(child);
}

Value *IfGenerate::getCondition() const { return _condition; }

Value *IfGenerate::setCondition(Value *x) { return setChild(_condition, x); }

} // namespace hif
