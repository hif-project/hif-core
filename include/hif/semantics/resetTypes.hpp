/// @file resetTypes.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{
namespace semantics
{

/// @brief Reset the type calculated for every Value of the
/// subtree HIF with root <tt>root</tt>.
///
/// @param root of the subtree
/// @param recursive If true, reset types of sub nodes.
///

void resetTypes(Object *root, const bool recursive = true);

/// @brief Reset the type calculated for every Value of the
/// subtree HIF with root <tt>root</tt>.
///
/// @param root of the subtree
/// @param recursive If true, reset types of sub nodes.
///

void resetTypes(BList<Object> &root, const bool recursive = true);

/// @brief Reset the type calculated for every Value of the
/// subtree HIF with root <tt>root</tt>.
///
/// @param root of the subtree
/// @param recursive If true, reset types of sub nodes.
///
template <typename T> void resetTypes(BList<T> &root, const bool recursive = true);
} // namespace semantics
} // namespace hif
