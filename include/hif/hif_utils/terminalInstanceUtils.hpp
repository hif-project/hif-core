/// @file terminalInstanceUtils.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{

/// @brief This function returns the instance of the given object.
/// This function operates on Hif objects having an <tt>instance</tt> field.
/// For example with my_namespace::my_class::my_static_const, return my_namespace.
/// @param obj The object from which the search will start.
/// @return The terminal instance if found, nullptr otherwise.
///

Object *getTerminalInstance(Object *obj);

} // namespace hif
