/// @file Entity.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Parameter.hpp"
#include "hif/classes/Port.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/Entity.hpp"

namespace hif
{

Entity::Entity()
    : parameters()
    , ports()
{
    _setBListParent(parameters);
    _setBListParent(ports);
    setName("Entity");
}

Entity::~Entity()
{
    // ntd
}

ClassId Entity::getClassId() const { return CLASSID_ENTITY; }

int Entity::acceptVisitor(HifVisitor &vis) { return vis.visitEntity(*this); }

void Entity::_calculateFields()
{
    Scope::_calculateFields();
    _addBList(parameters);
    _addBList(ports);
}

std::string Entity::_getBListName(const BList<Object> &list) const
{
    if (&list == &parameters.toOtherBList<Object>())
        return "parameters";
    if (&list == &ports.toOtherBList<Object>())
        return "ports";
    return Scope::_getBListName(list);
}

} // namespace hif
