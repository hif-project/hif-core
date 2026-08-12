/// @file objectGetKey.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief Given a object returns a key string of the object.
///
/// @param obj The object.
/// @return The key string of the object.
///

std::string objectGetKey(Object *obj);

} // namespace hif
