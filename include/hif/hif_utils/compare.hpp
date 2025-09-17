/// @file compare.hpp
/// @brief Utilities for comparing HIF objects and structures.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Compares two objects according to their type and their children nodes.
/// It provides a total ordering on Hif objects, which is useful for sorting methods
/// employed by the <tt>simplify</tt> function.
/// @return -1 if @p obj1 < @p obj2, 0 if @p obj1 == @p obj2, 1 if @p obj1 > @p obj2.
///

int compare(Object *obj1, Object *obj2);

} // namespace hif
