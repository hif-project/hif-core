/// @file Action.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Action.hpp"

namespace hif
{

Action::Action()
    : Object()
{
}

Action::~Action() {}

void Action::_calculateFields() { Object::_calculateFields(); }

} // namespace hif
