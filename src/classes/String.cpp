/// @file String.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/String.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Range.hpp"

namespace hif
{

String::String()
    : _spanInformation(nullptr)
{
    // ntd
}

String::~String() { delete _spanInformation; }

Range *String::setSpanInformation(Range *r) { return setChild(_spanInformation, r); }

Range *String::getSpanInformation() const { return _spanInformation; }

ClassId String::getClassId() const { return CLASSID_STRING; }

int String::acceptVisitor(HifVisitor &vis) { return vis.visitString(*this); }

void String::_calculateFields()
{
    SimpleType::_calculateFields();

    _addField(_spanInformation);
}

std::string String::_getFieldName(const Object *child) const
{
    if (child == _spanInformation)
        return "spanInformation";
    return SimpleType::_getFieldName(child);
}

Object *String::toObject() { return this; }

} // namespace hif
