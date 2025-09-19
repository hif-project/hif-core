/// @file isInTree.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Checks whether it is possible to reach the System object starting
/// from the passed object.
/// @param obj The object to check.
/// @return <tt>true</tt> if System is reachable.
///

bool isInTree(const Object *obj);

} // namespace hif
