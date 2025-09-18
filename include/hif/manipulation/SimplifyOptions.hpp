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
    /// @brief Enumeration for the simplification behavior levels.
    enum Behavior : unsigned char {
        /// @brief Conservative simplification behavior.
        BEHAVIOR_CONSERVATIVE,
        /// @brief Normal simplification behavior.
        BEHAVIOR_NORMAL,
        /// @brief Aggressive simplification behavior.
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

    /// @brief Default constructor.
    SimplifyOptions();
    /// @brief Default destructor.
    ~SimplifyOptions();

    /// @brief Copy constructor.
    /// @param other The SimplifyOptions to copy from.
    SimplifyOptions(const SimplifyOptions &other);
    /// @brief Copy assignment operator.
    /// @param other The SimplifyOptions to assign from.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &operator=(const SimplifyOptions &other);

    /// @brief Sets whether to simplify constants.
    /// @param yes True to enable constant simplification.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setSimplifyConstants(const bool yes);
    /// @brief Sets whether to simplify define constants.
    /// @param yes True to enable define constant simplification.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setSimplifyDefines(const bool yes);
    /// @brief Sets whether to simplify parameters.
    /// @param yes True to enable parameter simplification.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setSimplifyParameters(const bool yes);
    /// @brief Sets whether to simplify template parameters.
    /// @param yes True to enable template parameter simplification.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setSimplifyTemplateParameters(const bool yes);
    /// @brief Sets whether to simplify CTC template parameters.
    /// @param yes True to enable CTC template parameter simplification.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setSimplifyCtcTemplateParameters(const bool yes);
    /// @brief Sets whether to simplify non-CTC template parameters.
    /// @param yes True to enable non-CTC template parameter simplification.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setSimplifyNonCtcTemplateParameters(const bool yes);
    /// @brief Sets whether to simplify statements.
    /// @param yes True to enable statement simplification.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setSimplifyStatements(const bool yes);
    /// @brief Sets whether to simplify generate constructs.
    /// @param yes True to enable generate construct simplification.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setSimplifyGenerates(const bool yes);
    /// @brief Sets whether to simplify declarations.
    /// @param yes True to enable declaration simplification.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setSimplifyDeclarations(const bool yes);
    /// @brief Sets whether to simplify semantic types.
    /// @param yes True to enable semantic type simplification.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setSimplifySemanticsTypes(const bool yes);
    /// @brief Sets whether to simplify typereferences.
    /// @param yes True to enable typereference simplification.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setSimplifyTypereferences(const bool yes);
    /// @brief Sets whether to simplify function calls.
    /// @param yes True to enable function call simplification.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setSimplifyFunctioncalls(const bool yes);
    /// @brief Sets whether to replace the result in place.
    /// @param yes True to enable in-place replacement.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setReplaceResult(const bool yes);
    /// @brief Sets the simplification behavior.
    /// @param b The behavior level.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setBehavior(const Behavior b);
    /// @brief Sets the context object.
    /// @param ctx The context object.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setContext(Object *ctx);
    /// @brief Sets the root object.
    /// @param r The root object.
    /// @return Reference to this SimplifyOptions.
    SimplifyOptions &setRoot(Object *r);
};

} // namespace manipulation
} // namespace hif
