/// @file Signed.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Signed.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Range.hpp"

namespace hif
{

Signed::Signed()
{
    // ntd
}

Signed::~Signed()
{
    // ntd
}

ClassId Signed::getClassId() const { return CLASSID_SIGNED; }

int Signed::acceptVisitor(HifVisitor &vis) { return vis.visitSigned(*this); }

void Signed::_calculateFields()
{
    SimpleType::_calculateFields();

    _addField(_span);
}

std::string Signed::_getFieldName(const Object *child) const
{
    if (child == _span)
        return "span";
    return SimpleType::_getFieldName(child);
}

Range *Signed::setSpan(Range *x) { return setChild(_span, x); }

Object *Signed::toObject() { return this; }

} // namespace hif
