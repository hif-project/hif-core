/// @file getChildSkippingObjects.hpp
/// @brief Retrieve child objects in HIF trees, skipping specified nodes.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Returns the first child object different from a cast.
/// Note: The function navigates into cast value node (not the type).
/// @param o The starting object.
/// @return The first child object different from cast.
///

Value *getChildSkippingCasts(Value *o);

} // namespace hif
