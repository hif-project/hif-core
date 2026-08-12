/// @file PrefixedReference.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/PrefixedReference.hpp"

namespace hif
{

PrefixedReference::PrefixedReference()
    : Value()
    , _prefix(nullptr)
{
    // ntd
}

PrefixedReference::~PrefixedReference() { delete (_prefix); }

Value *PrefixedReference::getPrefix() const { return _prefix; }

Value *PrefixedReference::setPrefix(Value *x) { return setChild(_prefix, x); }

void PrefixedReference::_calculateFields()
{
    Value::_calculateFields();

    _addField(_prefix);
}

std::string PrefixedReference::_getFieldName(const Object *child) const
{
    if (child == _prefix)
        return "prefix";
    return Value::_getFieldName(child);
}

} // namespace hif
