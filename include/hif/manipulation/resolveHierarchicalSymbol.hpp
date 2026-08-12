/// @file resolveHierarchicalSymbol.hpp
/// @brief
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

/// @brief Resolve a given symbol against the HIF tree.
///
/// Symbols must be expressed as: top[.sub[(arch)]][.name]
/// e.g.
/// b17.u1.clock (where u1 is instance of b15)
///
/// @param symbol The symbol to be resolved.
/// @param system The HIF tree root.
/// @param sem The semantics to be used (default: HIF).
/// @param instancePath Pointer to the list where to store the instances traversed
/// along the hierarchical path, or <tt>nullptr</tt> in case this is not needed.
/// @return The object corresponding to the given symbol if resolved,
/// nullptr in case of error.
///

Object *resolveHierarchicalSymbol(
    const std::string &symbol,
    hif::System *system,
    hif::semantics::ILanguageSemantics *sem  = hif::semantics::HIFSemantics::getInstance(),
    std::list<hif::Instance *> *instancePath = nullptr);

} // namespace manipulation
} // namespace hif
