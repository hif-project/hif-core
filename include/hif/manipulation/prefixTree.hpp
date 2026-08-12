/// @file prefixTree.hpp
/// @brief Provides functionality for adding prefixes to symbols in the HIF tree.
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

/// @brief Options for controlling the prefixing of symbols in the HIF tree.
struct PrefixTreeOptions {
    bool recursive;    ///< Apply prefixing recursively if true. Default: true.
    bool strictChecks; ///< Perform strict checks (e.g., raise errors for missing declarations). Default: true.
    bool skipPrefixingIfSameScope; ///< Skip prefixing symbols if they belong to the same scope. Default: true.
    bool setContainingView;        ///< Include the containing view as part of the prefix if true. Default: false.

    /// @brief Default constructor initializing options.
    PrefixTreeOptions();

    /// @brief Destructor.
    ~PrefixTreeOptions();

    /// @brief Copy constructor.
    /// @param o Options to copy from.
    PrefixTreeOptions(const PrefixTreeOptions &o);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param o Options to assign from.
    /// @return Reference to the assigned options.
    PrefixTreeOptions &operator=(PrefixTreeOptions o);

    /// @brief Swaps the contents of two PrefixTreeOptions objects.
    /// @param o Options to swap with.
    void swap(PrefixTreeOptions &o);
};

/// @brief Adds prefixes to matched symbols starting from the given root object.
/// @details Modifies symbols to include prefixes based on their scope, library,
/// or containing view as per the provided options. Operates recursively or non-recursively
/// based on the options.
/// @param root The root object to start the prefixing from.
/// @param sem Pointer to the language semantics interface.
/// @param opt Options controlling the prefixing behavior (default: PrefixTreeOptions()).

void prefixTree(
    Object *root,
    hif::semantics::ILanguageSemantics *sem,
    const PrefixTreeOptions &opt = PrefixTreeOptions());

} // namespace manipulation
} // namespace hif
