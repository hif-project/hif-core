/// @file SemanticAnalysis.hpp
/// @brief File containing the declaration of the class used to analyze the type of the operands involved in current operation.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/manipulation/prefixTree.hpp"
#include "hif/semantics/ILanguageSemantics.hpp"

namespace hif
{
namespace semantics
{

/// @brief Class used to analyze the type of the operands involved in current operation.
class SemanticAnalysis
{
public:
    /// @brief Deleted copy constructor to prevent copying.
    SemanticAnalysis(const SemanticAnalysis &) = delete;

    /// @brief Deleted assignment operator to prevent copying.
    SemanticAnalysis &operator=(const SemanticAnalysis &) = delete;

    /// @brief Virtual destructor.
    virtual ~SemanticAnalysis() = 0;

    /// @brief Main function used to analyze the type of the operands involved
    /// in current operation.
    virtual void analyzeOperands(Type *op1Type, Type *op2Type);

    /// @brief Function used to get the result of the analysis.
    /// @return The result of the analysis.
    ILanguageSemantics::ExpressionTypeInfo getResult();

protected:
    /// @brief Constructor for initializing the analysis context.
    /// @param sem Pointer to the language semantics object.
    /// @param currOperator The operator for the current operation.
    /// @param srcObj The source object initiating the analysis.
    SemanticAnalysis(ILanguageSemantics *sem, Operator currOperator, Object *srcObj);

    ILanguageSemantics *_sem;                        ///< Pointer to the language semantics interface.
    Operator _currOperator;                          ///< The operator involved in the analysis.
    Object *_srcObj;                                 ///< The source object initiating the analysis.
    ILanguageSemantics::ExpressionTypeInfo _result;  ///< Stores the result of the analysis.
    HifFactory _factory;                             ///< Factory for creating HIF-related objects.
    hif::manipulation::PrefixTreeOptions _prefixOpt; ///< Options for managing prefix trees during analysis.

    /// @brief Analyzes a unary operation.
    /// @param op1 The operand.
    virtual void _callMap(Object *op1) = 0;

    /// @brief Analyzes a binary operation.
    /// @param op1 The first operand.
    /// @param op2 The second operand.
    virtual void _callMap(Object *op1, Object *op2) = 0;

    /// @brief Restores type references for result types and operation precision.
    /// @param tr The type reference to restore.
    /// @param base The base type to use for restoration.
    void _restoreTypeReference(TypeReference *tr, Type *base);

    /// @name Common unary mapping functions
    /// @{
    /// @brief Maps a type reference for unary operations.
    /// @param op The operand's type reference.
    void _map(TypeReference *op);
    /// @}

    /// @name Common binary mapping functions
    /// @{
    /// @brief Maps a type reference and a type for binary operations.
    /// @param op1 The first operand's type reference.
    /// @param op2 The second operand's type.
    void _map(TypeReference *op1, Type *op2);

    /// @brief Maps a type and a type reference for binary operations.
    /// @param op1 The first operand's type.
    /// @param op2 The second operand's type reference.
    void _map(Type *op1, TypeReference *op2);

    /// @brief Maps two type references for binary operations.
    /// @param op1 The first operand's type reference.
    /// @param op2 The second operand's type reference.
    /// @param checkOpacity Whether to check for opacity.
    void _map(TypeReference *op1, TypeReference *op2, bool checkOpacity);
    /// @}
};

} // namespace semantics
} // namespace hif
