/// @file analyzePrecisionType.hpp
/// @brief Analyze and determine the precision type of HIF objects.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"
#include "hif/semantics/analyzePrecisionType.hpp"

namespace hif
{
namespace semantics
{
/// @brief Struct representing input for analyzePrecisionType() function.
///
struct AnalyzeParams {
    /// @brief Type definition for a list of Type pointers.
    typedef std::list<Type *> TypeList;
    AnalyzeParams();
    ~AnalyzeParams();
    /// @brief Copy constructor.
    /// @param t The object to copy from.
    AnalyzeParams(const AnalyzeParams &t);
    /// @brief Assignment operator.
    /// @param t The object to assign from.
    /// @return Reference to this object.
    AnalyzeParams &operator=(const AnalyzeParams &t);

    /// @brief Operation to perform on operands.
    Operator operation;

    /// @brief List of Type pointer containing the type of the operands
    /// involved in the analyzed expression.
    TypeList operandsType;

    /// @brief Pointer to the original object to analyze.
    Object *startingObj;
};
/// @brief Function to get informations about eventual casts to do on
/// operands of an assignment, an aggregate and all the other objects
/// with multiple Value.
/// This function behaves like the analyzeExprType, but it uses the
/// precision instead of the returned type as intermediate value for the
/// computation. The operation considered is the equality.
/// @param params struct of AnalyzeParams type containing analysis
/// parameters. The operation is ignored. In this function is used
/// only the equality.
/// @param sem The semantics.
/// @return The result precision, nullptr if cannot be determinated.
///
Type *analyzePrecisionType(const AnalyzeParams &params, ILanguageSemantics *sem);
/// @brief Utility wrapper to general analyzePrecisionType().
/// @param o The object.
/// @param sem The semantics.
/// @return The result precision, nullptr if cannot be determinated.
///
Type *analyzePrecisionType(With *o, ILanguageSemantics *sem);

/// @brief Utility wrapper to general analyzePrecisionType().
/// @param o The object.
/// @param sem The semantics.
/// @return The result precision, nullptr if cannot be determinated.
///
Type *analyzePrecisionType(Switch *o, ILanguageSemantics *sem);
} // namespace semantics
} // namespace hif
