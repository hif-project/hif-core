/// @file systemCManipulation.hpp
/// @brief Provides utilities for adapting HIF objects to SystemC constructs.
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

/// @brief Options for mapping VHDL `last_value` attributes to SystemC.
struct LastValueOptions {
    /// @brief Default constructor initializing options.
    LastValueOptions();

    /// @brief Destructor.
    ~LastValueOptions();

    /// @brief Copy constructor.
    /// @param other Options to copy from.
    LastValueOptions(const LastValueOptions &other);

    /// @brief Copy assignment operator using copy-and-swap idiom.
    /// @param other Options to assign from.
    /// @return Reference to the assigned options.
    LastValueOptions &operator=(LastValueOptions other);

    /// @brief Swaps the contents of two LastValueOptions objects.
    /// @param other Options to swap with.
    void swap(LastValueOptions &other);

    bool replaceRisingFallingEdge; ///< Replace `last_value` in rising/falling edges. Default: true.
    bool inlineLastValue; ///< Inline `last_value` as a function instead of using an external library. Default: false.
};

/// @brief Replaces references to the VHDL `last_value` attribute with equivalent SystemC code.
/// @details This function traverses the HIF tree and replaces `last_value` with appropriate
/// SystemC constructs based on the options provided.
/// @param root The root of the HIF object tree.
/// @param opts Options controlling the transformation (default: LastValueOptions()).
/// @return True if at least one `last_value` reference was found and replaced.

bool mapLastValueToSystemC(Object *root, const LastValueOptions &opts = LastValueOptions());

/// @brief Adds a constructor to a SystemC module, accepting the mandatory module instance name.
/// @details The constructor is added to the provided module view if it does not already exist.
/// @param view The module to which the constructor is added.
/// @param sem The language semantics to use.
/// @return True if the constructor was added successfully.

bool addModuleConstructor(View *view, hif::semantics::ILanguageSemantics *sem);

} // namespace manipulation
} // namespace hif
