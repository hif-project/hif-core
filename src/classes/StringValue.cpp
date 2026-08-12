/// @file StringValue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/StringValue.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

StringValue::StringValue()
    : _text()
    , _isPlain(false)
{
    // ntd
}

StringValue::StringValue(const std::string &t)
    : _text(t)
    , _isPlain(false)
{
    // ntd
}

StringValue::~StringValue()
{
    // ntd
}

void StringValue::setValue(const std::string &text) { _text = text; }

void StringValue::setPlain(bool plain) { _isPlain = plain; }

bool StringValue::isPlain() const { return _isPlain; }

ClassId StringValue::getClassId() const { return CLASSID_STRINGVALUE; }

int StringValue::acceptVisitor(HifVisitor &vis) { return vis.visitStringValue(*this); }

void StringValue::_calculateFields() { ConstValue::_calculateFields(); }

std::string StringValue::getValue() const { return _text; }

} // namespace hif
