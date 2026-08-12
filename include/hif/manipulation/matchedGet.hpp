/// @file matchedGet.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Gets the object (from the newParent) in the same position of @p oldObj
/// in the @p oldParent. @p oldParent must be nullptr or the direct parent of @p oldObj.
/// @param newParent The new parent.
/// @param oldObj The object to be matched.
/// @param oldParent Optional old parent. Can be nullptr.
/// @return The matched object, or nullptr.

Object *matchedGet(Object *newParent, Object *oldObj, Object *oldParent = nullptr);

} // namespace manipulation
} // namespace hif
