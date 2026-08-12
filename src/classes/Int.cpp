/// @file Int.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Int.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Range.hpp"

namespace hif
{

Int::Int()
    : _isSigned(true)
{
    // ntd
}

Int::~Int()
{
    // ntd
}

ClassId Int::getClassId() const { return CLASSID_INT; }

int Int::acceptVisitor(HifVisitor &vis) { return vis.visitInt(*this); }

void Int::_calculateFields()
{
    SimpleType::_calculateFields();

    _addField(_span);
}

std::string Int::_getFieldName(const Object *child) const
{
    if (child == _span)
        return "span";
    return SimpleType::_getFieldName(child);
}

Range *Int::setSpan(Range *x) { return setChild(_span, x); }

bool Int::isSigned() const { return (_isSigned); }

void Int::setSigned(bool sign) { _isSigned = sign; }

Object *Int::toObject() { return this; }

} // namespace hif
