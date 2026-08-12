/// @file getMatchingScope.hpp
/// @brief Find the matching scope for a given HIF object.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Return the eventual matching scope or nullptr
/// @param o The starting object
/// @return the matching scope to witch the object refers

Object *getMatchingScope(Break *o);

/// @brief Return the eventual matching scope or nullptr
/// @param o The starting object
/// @return the matching scope to witch the object refers

Object *getMatchingScope(Continue *o);

/// @brief Return the eventual matching scope or nullptr
/// @param o The starting object
/// @return the matching scope to witch the object refers

Object *getMatchingScope(Object *o);
} // namespace hif
