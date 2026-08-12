/// @file ViewReference.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/TPAssign.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/ViewReference.hpp"

namespace hif
{

ViewReference::ViewReference()
    : templateParameterAssigns()
    , _unitname(NameTable::getInstance()->none())
{
    _setBListParent(templateParameterAssigns);
}

ViewReference::~ViewReference()
{
    // ntd
}

ClassId ViewReference::getClassId() const { return CLASSID_VIEWREFERENCE; }

int ViewReference::acceptVisitor(HifVisitor &vis) { return vis.visitViewReference(*this); }

void ViewReference::_calculateFields()
{
    ReferencedType::_calculateFields();

    _addBList(templateParameterAssigns);
}

std::string ViewReference::_getBListName(const BList<Object> &list) const
{
    if (&list == &templateParameterAssigns.toOtherBList<Object>())
        return "templateParameterAssigns";
    return ReferencedType::_getBListName(list);
}

std::string ViewReference::getDesignUnit() const { return _unitname; }

void ViewReference::setDesignUnit(const std::string &x) { _unitname = x; }

Object *ViewReference::toObject() { return this; }

} // namespace hif
