/// @file Member.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Member.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Member::Member()
    : _index(nullptr)
{
    // ntd
}

Member::~Member() { delete (_index); }

ClassId Member::getClassId() const { return CLASSID_MEMBER; }

int Member::acceptVisitor(HifVisitor &vis) { return vis.visitMember(*this); }

Value *Member::getIndex() const { return _index; }

Value *Member::setIndex(Value *v) { return setChild(_index, v); }

void Member::_calculateFields()
{
    PrefixedReference::_calculateFields();

    _addField(_index);
}

std::string Member::_getFieldName(const Object *child) const
{
    if (child == _index)
        return "index";
    return PrefixedReference::_getFieldName(child);
}

} // namespace hif
