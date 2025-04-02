/// @file SimplifyOptions.hpp
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

/// @name Tree simplifying functions.
/// @{

/// @brief Struct of options for the <tt>simplify</tt> method.
/// @see hif::simplify
struct SimplifyOptions {
    enum Behavior : unsigned char {
        BEHAVIOR_CONSERVATIVE,
        BEHAVIOR_NORMAL,
        BEHAVIOR_AGGRESSIVE,
    };

    /// @brief Simplify constants by replacing them with their values.
    /// Default is false.
    bool simplify_constants;
    /// @brief Simplify define constants by replacing them with their values.
    /// Default is true.
    /// @note This flag is checked only when simplify_constants holds.
    bool simplify_defines;
    /// @brief Simplify parameters by replacing them with their default values.
    /// Default is false.
    bool simplify_parameters;
    /// @brief Simplify template parameters by replacing them with their default values.
    /// Default is false.
    bool simplify_template_parameters;
    /// @brief Simplify CTC template parameters by replacing them with their default values.
    /// Default is true.
    /// @note This flag is checked only when simplify_template_parameters holds.
    bool simplify_ctc_template_parameters;
    /// @brief Simplify non CTC template parameters by replacing them with their default values.
    /// Default is true.
    /// @note This flag is checked only when simplify_template_parameters holds.
    bool simplify_non_ctc_template_parameters;
    /// @brief Simplify statements in the code. Default is false.
    ///E.g. unrolls loops with constant iterations.
    bool simplify_statements;
    /// @brief Simplify generate constructs by expanding them. Default is false.
    /// @warning Do not use this flag to expand generate constructs, but
    /// please rely on the <tt>expandGenerates</tt> function.
    /// @see expandGenerates
    bool simplify_generates;
    /// @brief Simplify declarations. Default is false.
    /// @warning To be removed.
    bool simplify_declarations;
    /// @brief Simplify semantic types. Default is false.
    bool simplify_semantics_types;
    /// @brief Simplify typereferences by expanding with their base type. Default is false.
    bool simplify_typereferences;
    /// @brief Simplify function call in order to replace them with ConstValues.
    /// Default is false.
    bool simplify_functioncalls;
    /// @brief If <tt>false</tt>, copy the tree to be simplified, simplify the copied tree and
    /// return it. If <tt>true</tt>, simplify in place. Default is true.
    bool replace_result;
    /// @brief Enables or disables some simplifications. Default is <tt>BEHAVIOR_NORMAL</tt>.
    Behavior behavior;
    ///@brief Objects are not simplified if their referenced declarations are not
    /// sub-nodes of this node. nullptr means always simplify.
    Object *context;
    ///@brief If set, simplify symbols only if are declared under this root.
    Object *root;

    SimplifyOptions();
    ~SimplifyOptions();

    SimplifyOptions(const SimplifyOptions &);
    SimplifyOptions &operator=(const SimplifyOptions &);

    SimplifyOptions &setSimplifyConstants(bool yes);
    SimplifyOptions &setSimplifyDefines(bool yes);
    SimplifyOptions &setSimplifyParameters(bool yes);
    SimplifyOptions &setSimplifyTemplateParameters(bool yes);
    SimplifyOptions &setSimplifyCtcTemplateParameters(bool yes);
    SimplifyOptions &setSimplifyNonCtcTemplateParameters(bool yes);
    SimplifyOptions &setSimplifyStatements(bool yes);
    SimplifyOptions &setSimplifyGenerates(bool yes);
    SimplifyOptions &setSimplifyDeclarations(bool yes);
    SimplifyOptions &setSimplifySemanticsTypes(bool yes);
    SimplifyOptions &setSimplifyTypereferences(bool yes);
    SimplifyOptions &setSimplifyFunctioncalls(bool yes);
    SimplifyOptions &setReplaceResult(bool yes);
    SimplifyOptions &setBehavior(const Behavior b);
    SimplifyOptions &setContext(Object *ctx);
    SimplifyOptions &setRoot(Object *r);
};

} // namespace manipulation
} // namespace hif
