/// @file DesignUnit.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/View.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/DesignUnit.hpp"

namespace hif
{

DesignUnit::DesignUnit()
    : views()
{
    _setBListParent(views);
}

DesignUnit::~DesignUnit()
{
    // ntd
}

ClassId DesignUnit::getClassId() const { return CLASSID_DESIGNUNIT; }

int DesignUnit::acceptVisitor(HifVisitor &vis) { return vis.visitDesignUnit(*this); }

void DesignUnit::_calculateFields()
{
    Scope::_calculateFields();
    _addBList(views);
}

std::string DesignUnit::_getBListName(const BList<Object> &list) const
{
    if (&list == &views.toOtherBList<Object>())
        return "views";
    return Scope::_getBListName(list);
}

} // namespace hif
