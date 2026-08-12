/// @file Slice.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Slice.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Range.hpp"

namespace hif
{

Slice::Slice()
    : _span(nullptr)
{
    // ntd
}

Slice::~Slice() { delete (_span); }

Range *Slice::getSpan() const { return _span; }

Range *Slice::setSpan(Range *x) { return setChild(_span, x); }

ClassId Slice::getClassId() const { return CLASSID_SLICE; }

int Slice::acceptVisitor(HifVisitor &vis) { return vis.visitSlice(*this); }

void Slice::_calculateFields()
{
    PrefixedReference::_calculateFields();

    _addField(_span);
}

std::string Slice::_getFieldName(const Object *child) const
{
    if (child == _span)
        return "span";
    return PrefixedReference::_getFieldName(child);
}

} // namespace hif
