/// @file getNearestParent.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Finds the nearest parent of a given type of an object.
/// This function will go up the Hif tree starting from a given object
/// until it reaches an object of a given type or the root of the Hif
/// tree.
///
/// Call example:
/// @code
/// hif::Type result = getNearestParent< hif::Type >( staringObj);
/// @endcode
///
/// @tparam T The type of the object to be returned.
/// @param object The starting point of the function search.
/// @param matchStarting If <tt>true</tt> match also the passed object.
/// Default is <tt>false</tt>.
/// @return The wanted Hif object or nullptr if it has not been found.
///
template <typename T> T *getNearestParent(Object *object, const bool matchStarting = false);

} // namespace hif
