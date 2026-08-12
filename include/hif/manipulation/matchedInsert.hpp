/// @file matchedInsert.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/manipulation/matchedInsertType.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Inserts @p newObj inside @p newParent as the same field matched
/// by @p oldObj inside @p oldParent.
///
/// @param newObj The new object to be inserted.
/// @param newParent The parent object where to insert @p newObj.
/// @param oldObj The old object from which to get the position inside @p oldParent.
/// @param oldParent The old parent object to be matched.
/// @param type The insert type. Default is  <tt>TYPE_ERROR</tt>.
/// @return <tt>true</tt> if there are no errors, <tt>false</tt> otherwise.
///

bool matchedInsert(
    Object *newObj,
    Object *newParent,
    Object *oldObj,
    Object *oldParent                  = nullptr,
    MatchedInsertType type = MatchedInsertType::TYPE_ERROR);

} // namespace manipulation
} // namespace hif
