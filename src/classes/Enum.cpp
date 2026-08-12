/// @file Enum.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/EnumValue.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/Enum.hpp"

namespace hif
{

Enum::Enum()
    : values()
{
    _setBListParent(values);
}

Enum::~Enum()
{
    // ntd
}

ClassId Enum::getClassId() const { return CLASSID_ENUM; }

int Enum::acceptVisitor(HifVisitor &vis) { return vis.visitEnum(*this); }

void Enum::_calculateFields()
{
    ScopedType::_calculateFields();
    _addBList(values);
}

std::string Enum::_getBListName(const BList<Object> &list) const
{
    if (&list == &values.toOtherBList<Object>())
        return "values";
    return ScopedType::_getBListName(list);
}

} // namespace hif
