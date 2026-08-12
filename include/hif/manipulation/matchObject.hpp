/// @file matchObject.hpp
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

/// @brief Match object options.
struct MatchObjectOptions {
    /// @brief If <tt>true</tt>, relaxes the matching criterion.
    bool matchStructure;

    /// @brief If <tt>true</tt>, skips references when matching objects.
    bool skipReferences;

    MatchObjectOptions();
    ~MatchObjectOptions();
};

/// @brief Given a pattern object in a reference tree, returns the corresponding
/// object in the same position in the matched tree.
///
/// The @p matchStructure parameter can be used to relax the pattern matching
/// criterion. For example, an object in the range of an array can be matched
/// by an object in a bit vector type having the same range.
///
/// @param pattern The pattern object to search for.
/// @param referenceTree The reference HIF tree.
/// @param matchedTree The matched HIF tree.
/// @param sem The reference semantics. Default value is HIF semantics.
/// @param opt The match object options.
/// @return The corresponding object in the same position in the matched tree.
///

Object *matchObject(
    Object *pattern,
    Object *referenceTree,
    Object *matchedTree,
    hif::semantics::ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance(),
    const MatchObjectOptions &opt           = MatchObjectOptions());

} // namespace manipulation
} // namespace hif
