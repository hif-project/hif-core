/// @file Unsigned.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Unsigned.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Range.hpp"

namespace hif
{

Unsigned::Unsigned()
    : SimpleType()
{
    // ntd
}

Unsigned::~Unsigned()
{
    // ntd
}

ClassId Unsigned::getClassId() const { return CLASSID_UNSIGNED; }

int Unsigned::acceptVisitor(HifVisitor &vis) { return vis.visitUnsigned(*this); }

void Unsigned::_calculateFields()
{
    SimpleType::_calculateFields();

    _addField(_span);
}

std::string Unsigned::_getFieldName(const Object *child) const
{
    if (child == _span)
        return "span";
    return SimpleType::_getFieldName(child);
}

Range *Unsigned::setSpan(Range *x) { return setChild(_span, x); }

Object *Unsigned::toObject() { return this; }

} // namespace hif
