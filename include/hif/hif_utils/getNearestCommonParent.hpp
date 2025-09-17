/// @file getNearestCommonParent.hpp
/// @brief Find the nearest common parent of two HIF objects.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Returns the nearest common parent.
/// @param obj1 The first Object.
/// @param obj2 The second Object.
/// @return The nearest common parent if present, nullptr otherwise.
///

Object *getNearestCommonParent(Object *obj1, Object *obj2);

} // namespace hif
