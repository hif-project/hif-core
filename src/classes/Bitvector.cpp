/// @file Bitvector.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Bitvector.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Range.hpp"

namespace hif
{

Bitvector::Bitvector()
    : _isLogic(false)
    , _isResolved(false)
    , _isSigned(false)
{
    // ntd
}

Bitvector::~Bitvector()
{
    // ntd
}

ClassId Bitvector::getClassId() const { return CLASSID_BITVECTOR; }

int Bitvector::acceptVisitor(HifVisitor &vis) { return vis.visitBitvector(*this); }

void Bitvector::_calculateFields()
{
    SimpleType::_calculateFields();
    _addField(_span);
}

std::string Bitvector::_getFieldName(const Object *child) const
{
    if (child == _span)
        return "span";
    return SimpleType::_getFieldName(child);
}

Range *Bitvector::setSpan(Range *x) { return setChild(_span, x); }

bool Bitvector::isLogic() const { return _isLogic; }

void Bitvector::setLogic(bool logic) { _isLogic = logic; }

bool Bitvector::isResolved() const { return _isResolved; }

void Bitvector::setResolved(bool resolved) { _isResolved = resolved; }

bool Bitvector::isSigned() const { return _isSigned; }

void Bitvector::setSigned(bool sign) { _isSigned = sign; }

Object *Bitvector::toObject() { return this; }

} // namespace hif
