/// @file TypeReference.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Range.hpp"
#include "hif/classes/TPAssign.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/TypeReference.hpp"
namespace hif
{

TypeReference::TypeReference()
    : templateParameterAssigns()
    , ranges()
{
    _setBListParent(templateParameterAssigns);
    _setBListParent(ranges);
}

TypeReference::~TypeReference()
{
    // ntd
}

ClassId TypeReference::getClassId() const { return CLASSID_TYPEREFERENCE; }

int TypeReference::acceptVisitor(HifVisitor &vis) { return vis.visitTypeReference(*this); }

void TypeReference::_calculateFields()
{
    ReferencedType::_calculateFields();

    _addBList(templateParameterAssigns);
    _addBList(ranges);
}

std::string TypeReference::_getBListName(const BList<Object> &list) const
{
    if (&list == &templateParameterAssigns.toOtherBList<Object>())
        return "templateParameterAssigns";
    if (&list == &ranges.toOtherBList<Object>())
        return "range";
    return ReferencedType::_getBListName(list);
}

Object *TypeReference::toObject() { return this; }

} // namespace hif
