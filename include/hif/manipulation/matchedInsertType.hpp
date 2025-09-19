/// @file matchedInsertType.hpp
/// @brief Defines insertion behavior types for object manipulation.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Specifies insertion behavior when handling objects in HIF structures.
enum class MatchedInsertType : unsigned char {
    TYPE_DELETE,       ///< Deletes existing objects before insertion.
    TYPE_ERROR,        ///< Raises an error if the object already exists.
    TYPE_ONLY_REPLACE, ///< Replaces existing objects without deletion.
    TYPE_EXPAND        ///< Inserts at a position in a list, shifting subsequent elements.
};

} // namespace manipulation
} // namespace hif
