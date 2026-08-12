/// @file Contents.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Library.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/Contents.hpp"

namespace hif
{

Contents::Contents()
    : libraries()
{
    _setBListParent(libraries);
    setName("Architecture");
}

Contents::~Contents()
{
    // ntd
}

ClassId Contents::getClassId() const { return CLASSID_CONTENTS; }

int Contents::acceptVisitor(HifVisitor &vis) { return vis.visitContents(*this); }

void Contents::_calculateFields()
{
    BaseContents::_calculateFields();
    _addBList(libraries);
}

std::string Contents::_getBListName(const BList<Object> &list) const
{
    if (&list == &libraries.toOtherBList<Object>())
        return "libraries";
    return BaseContents::_getBListName(list);
}

} // namespace hif
