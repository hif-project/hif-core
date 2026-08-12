/// @file bindOpenPortAssigns.hpp
/// @brief Bind open port assignments in HIF modules.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/semantics/HIFSemantics.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Adds a dummy signal attached to a port which is not bound.
/// It is useful since in Hif when a design unit is instantiated, ports not
/// bound are not allowed.
///
/// @param object The root object from which to search the instances.
/// @param sem The reference semantics.
///

void bindOpenPortAssigns(
    Object &object,
    hif::semantics::ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance());

} // namespace manipulation
} // namespace hif
