/// @file TypeVisitor.hpp
/// @brief Declares the `TypeVisitor` class for analyzing and simplifying types in HIF constructs.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/GuideVisitor.hpp"
#include "hif/HifFactory.hpp"

namespace hif
{
namespace semantics
{

/// @class TypeVisitor
/// @brief Visitor class for analyzing and managing types in HIF constructs.
class TypeVisitor : public GuideVisitor
{
public:
    /// @brief Constructor.
    /// @param ref Pointer to the language semantics object.
    /// @param error Flag to enable error reporting.
    TypeVisitor(ILanguageSemantics *ref, bool error);

    /// @brief Deleted copy constructor to prevent copying.
    TypeVisitor(const TypeVisitor &v) = delete;

    /// @brief Deleted assignment operator to prevent copying.
    TypeVisitor &operator=(const TypeVisitor &v) = delete;

    /// @brief Virtual destructor.
    virtual ~TypeVisitor();

    // Overridden visit methods for different HIF objects.
    virtual int visitAggregate(Aggregate &o);
    virtual int visitBitValue(BitValue &o);
    virtual int visitBitvectorValue(BitvectorValue &o);
    virtual int visitBoolValue(BoolValue &o);
    virtual int visitCast(Cast &o);
    virtual int visitCharValue(CharValue &o);
    virtual int visitExpression(Expression &o);
    virtual int visitFieldReference(FieldReference &o);
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitIdentifier(Identifier &o);
    virtual int visitInstance(Instance &o);
    virtual int visitIntValue(IntValue &o);
    virtual int visitMember(Member &o);
    virtual int visitParameterAssign(ParameterAssign &o);
    virtual int visitPortAssign(PortAssign &o);
    virtual int visitRealValue(RealValue &o);
    virtual int visitRecordValue(RecordValue &o);
    virtual int visitSlice(Slice &o);
    virtual int visitStringValue(StringValue &o);
    virtual int visitTimeValue(TimeValue &o);
    virtual int visitTypeTPAssign(TypeTPAssign &o);
    virtual int visitValueTPAssign(ValueTPAssign &o);
    virtual int visitWhen(When &o);
    virtual int visitWith(With &o);

    ILanguageSemantics *_sem; ///< Pointer to the language semantics interface.
    bool _error;              ///< Flag indicating whether to report errors.
    hif::HifFactory _factory; ///< Factory for creating HIF-related objects.

    /// @brief Determines the type of a constant value.
    /// @details Checks semantics type, syntactic type, and contextual inference.
    /// @param o Pointer to the constant value object.
    /// @return True if the type is determined using `getTypeForConstant`.
    bool _getTypeForConstant(ConstValue *o);

    /// @brief Updates the semantic type of an object using cached declarations.
    /// @param obj The object whose type needs updating.
    /// @param onlySignature If true, updates only the signature.
    void _updateCachedDeclarations(TypedObject *obj, bool onlySignature = false);

    /// @brief Simplifies a type and adds it to the cache.
    /// @param o The type to simplify.
    /// @param simplified If true, assumes the type is already simplified.
    void _simplify(Type *o, bool simplified = false);

    /// @brief Determines the type of a parameter assignment.
    /// @param o Pointer to the parameter assignment object.
    /// @param checkAllCandidates If true, checks all possible candidates.
    /// @param looseTypeChecks If true, performs less strict type checks.
    void _getTypeOfParameterAssign(
        ParameterAssign *o,
        bool checkAllCandidates = false,
        bool looseTypeChecks    = false);

    /// @brief Determines the type of a port assignment.
    /// @param o Pointer to the port assignment object.
    void _getTypeOfPortAssign(PortAssign *o);

    /// @brief Determines the type of a value template parameter assignment.
    /// @param o Pointer to the value template parameter assignment object.
    void _getTypeOfValueTPAssign(ValueTPAssign *o);

    /// @brief Creates parent scope references for a type.
    /// @param o The object whose scope is to be referenced.
    /// @param rt The referenced type to attach scopes to.
    /// @return The modified referenced type.
    ReferencedType *_makeParentScopes(Object *o, ReferencedType *rt);

    /// @name Aggregate Support Methods
    /// @{

    /// @brief Checks if an aggregate is a constant expression.
    /// @param o Pointer to the aggregate object.
    /// @return True if the aggregate is a constant expression.
    bool _aggregateIsConstExpr(Aggregate *o);

    /// @brief Retrieves the internal type of an aggregate.
    /// @param o Pointer to the aggregate object.
    /// @return The internal type of the aggregate.
    Type *_aggregateGetInternalType(Aggregate *o);

    /// @brief Retrieves the minimum bound of a value.
    /// @param v Pointer to the value object.
    /// @param min Reference to store the minimum bound.
    /// @return True if the minimum bound is successfully determined.
    bool _getBoundMin(Value *v, std::int64_t &min);

    /// @brief Checks if an aggregate is auto-determinate.
    /// @param o Pointer to the aggregate object.
    /// @param sizeExpr Reference to store the size expression.
    /// @param size Reference to store the size.
    /// @param min Reference to store the minimum bound.
    /// @return True if the aggregate is auto-determinate.
    bool _aggregateIsAutoDeterminate(Aggregate *o, Value *&sizeExpr, std::int64_t &size, std::int64_t &min);

    /// @brief Calculates the size of an aggregate.
    /// @param sizeExpr Reference to the size expression.
    /// @param size The calculated size.
    /// @param min The minimum bound.
    /// @return True if the size is successfully calculated.
    bool _aggregateCalulateSize(Value *&sizeExpr, std::int64_t size, std::int64_t min);

    /// @}
};

} // namespace semantics
} // namespace hif