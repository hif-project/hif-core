/// @file mapToNative.hpp
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
/// @brief Maps all types of objects in subtree starting by @p v
/// in a native type that can be used for example as template parameter
/// of functions.
///
/// @param v The starting object.
/// @param sem The current tree semantics.
/// @param checkSem The semantics used to check and map native types.
///

void mapToNative(Object *v, hif::semantics::ILanguageSemantics *sem, hif::semantics::ILanguageSemantics *checkSem);

} // namespace manipulation
} // namespace hif
