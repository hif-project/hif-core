/// @file fixInstanceBindings.hpp
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

/// @brief Options for fixInstanceBindings().
struct FixBindingOptions {
    /// @brief Binding with casts must be fixed. Default true.
    bool fixCasts;
    /// @brief Bindings with casts between logic vectors (i.e., lv, signed
    /// and unsigned) must be fixed. This flag is checked only if <tt>fixCasts
    /// </tt> is enabled. Default false.
    bool fixVectorCasts;
    /// @brief Binding with members must be fixed. Default true.
    bool fixMembers;
    /// @brief Binding with slices must be fixed. Default true.
    bool fixSlices;
    /// @brief Binding with constants. Default true.
    bool fixConstants;
    /// @brief Binding with other values must be fixed. Default true.
    bool fixOthers;
    /// @brief Allow bindings only with segnals and ports. Default false.
    bool allowOnlySignalPorts;
};

/// @brief Replaces port bindings with no name-to-name binding, inserting auxiliary signals.
/// This fix tries to avoid introducing extra delta cycles if it is possible.
/// Otherwise, global actions will be introduced, resulting in extra delta cycle(s).
/// @param root The root.
/// @param ref_sem The semantics to be used to check the bindings.
/// @param opt The fix binding options.
/// @return <tt>false</tt> in case of errors, <tt>true</tt> in case of success.
///

bool fixInstanceBindings(
    Object *root,
    hif::semantics::ILanguageSemantics *ref_sem = hif::semantics::HIFSemantics::getInstance(),
    const FixBindingOptions &opt                = FixBindingOptions());

} // namespace manipulation
} // namespace hif
