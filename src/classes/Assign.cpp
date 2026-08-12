/// @file Assign.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Assign.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/TimeValue.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

Assign::Assign()
    : _leftHandSide(nullptr)
    , _rightHandSide(nullptr)
    , _delay(nullptr)
{
    // ntd
}

Assign::~Assign()
{
    delete (_leftHandSide);
    delete (_rightHandSide);
    delete (_delay);
}

ClassId Assign::getClassId() const { return CLASSID_ASSIGN; }

int Assign::acceptVisitor(HifVisitor &vis) { return vis.visitAssign(*this); }

void Assign::_calculateFields()
{
    Action::_calculateFields();
    _addField(_leftHandSide);
    _addField(_rightHandSide);
    _addField(_delay);
}

std::string Assign::_getFieldName(const Object *child) const
{
    if (child == _leftHandSide)
        return "leftHandSide";
    if (child == _rightHandSide)
        return "rightHandSide";
    if (child == _delay)
        return "delay";
    return Action::_getFieldName(child);
}

Value *Assign::getRightHandSide() const { return _rightHandSide; }

Value *Assign::setRightHandSide(Value *x) { return setChild(_rightHandSide, x); }

Value *Assign::getLeftHandSide() const { return _leftHandSide; }

Value *Assign::setLeftHandSide(Value *x) { return setChild(_leftHandSide, x); }

Value *Assign::getDelay() const { return _delay; }

Value *Assign::setDelay(Value *tv) { return setChild(_delay, tv); }

} // namespace hif
