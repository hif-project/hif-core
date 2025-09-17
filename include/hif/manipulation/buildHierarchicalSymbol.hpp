/// @file buildHierarchicalSymbol.hpp
/// @brief Build hierarchical symbol names for HIF objects.
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
/// @brief The definition style used by method buildHierarchicalSymbol.
struct DefinitionStyle {
    enum Type : unsigned char {
        HIF,
        VHDL,
        VERILOG,
        SYSTEMC,
    };
};

/// @brief Build a hierarchical symbol against the HIF tree.
///
/// Symbols will be expressed as: top[.sub[(arch)]][.name]
/// e.g.
/// b17.u1.clock (where u1 is instance of b15)
///
/// @param obj The object to be represented as symbol.
/// @param sem The semantics to be used (default: HIF).
/// @param style The style used to build hierarchical names.
/// @return The symbol corresponding to the given object.
///

std::string buildHierarchicalSymbol(
    Object *obj,
    hif::semantics::ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance(),
    DefinitionStyle::Type style             = DefinitionStyle::HIF);

} // namespace manipulation
} // namespace hif
