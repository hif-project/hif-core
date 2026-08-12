/// @file getParentSkippingObjects.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Returns the first parent object different from a cast.
/// @param o The starting object.
/// @return The first parent object different from cast.
///

Object *getParentSkippingCasts(Value *o);

/// @brief Returns the first parent object different from @p T.
/// @param o The starting object.
/// @return The first parent object different from @p T.
///
template <typename T> Object *getParentSkippingClass(Object *o);

} // namespace hif
