/// @file Real.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Real.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Range.hpp"

namespace hif
{

Real::Real()
{
    // ntd
}

Real::~Real()
{
    // ntd
}

Range *Real::setSpan(Range *x) { return setChild(_span, x); }

ClassId Real::getClassId() const { return CLASSID_REAL; }

int Real::acceptVisitor(HifVisitor &vis) { return vis.visitReal(*this); }

void Real::_calculateFields()
{
    SimpleType::_calculateFields();

    _addField(_span);
}

std::string Real::_getFieldName(const Object *child) const
{
    if (child == _span)
        return "span";
    return SimpleType::_getFieldName(child);
}

Object *Real::toObject() { return this; }

} // namespace hif
