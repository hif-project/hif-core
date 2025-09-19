/// @file Range.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>

#include "hif/HifVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/classes/IntValue.hpp"
#include "hif/classes/Range.hpp"
#include "hif/classes/Type.hpp"

namespace hif
{

Range::Range()
    : _direction(dir_upto)
    , _leftBound(nullptr)
    , _rightBound(nullptr)
    , _type(nullptr)
{
    // ntd
}

Range::Range(Value *lbound_, Value *rbound_, const RangeDirection dir_)
    : _direction(dir_)
    , _leftBound(nullptr)
    , _rightBound(nullptr)
    , _type(nullptr)
{
    setLeftBound(lbound_);
    setRightBound(rbound_);
}

Range::~Range()
{
    delete (_leftBound);
    delete (_rightBound);
    delete (_type);
}

RangeDirection Range::getDirection() const { return _direction; }

void Range::setDirection(const RangeDirection x) { _direction = x; }

Type *Range::getType() const { return _type; }

Type *Range::setType(Type *x) { return setChild(_type, x); }

Type *Range::getSemanticType() const { return _semanticsType; }

Type *Range::setSemanticType(Type *t)
{
    Type *ret = setChild(_semanticsType, t);
    if (t != nullptr) {
        messageError("Try to set the semantics type of a range.\nThis is not allowed!", nullptr, nullptr);
    }
    return ret;
}

Value *Range::getRightBound() const { return _rightBound; }

Value *Range::setRightBound(Value *x) { return setChild(_rightBound, x); }

Value *Range::getLeftBound() const { return _leftBound; }

Value *Range::setLeftBound(Value *x) { return setChild(_leftBound, x); }

ClassId Range::getClassId() const { return CLASSID_RANGE; }

int Range::acceptVisitor(HifVisitor &vis) { return vis.visitRange(*this); }

void Range::swapBounds()
{
    _direction = _direction == dir_upto ? dir_downto : dir_upto;
    Value *l   = setLeftBound(setRightBound(nullptr));
    setRightBound(l);
}

void Range::_calculateFields()
{
    Value::_calculateFields();

    _addField(_leftBound);
    _addField(_rightBound);
}

std::string Range::_getFieldName(const Object *child) const
{
    if (child == _leftBound)
        return "leftBound";
    if (child == _rightBound)
        return "rightBound";
    return Value::_getFieldName(child);
}

} // namespace hif
