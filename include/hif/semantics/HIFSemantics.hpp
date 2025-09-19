/// @file HIFSemantics.hpp
/// @brief Core semantics and rules for HIF language constructs.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/semantics/ILanguageSemantics.hpp"

namespace hif
{
namespace semantics
{

/// @brief This class implements the HIF semantics.
///
/// It is an implementation of the ILanguageSemantics interface.
class HIFSemantics : public ILanguageSemantics
{
public:
    /// @brief Returns the name of the semantics.
    /// @return The name of the semantics.
    virtual std::string getName() const;
    /// @brief This function returns a struct containing informations about
    /// cast to be applied to the operands, in order to make the given
    /// operation HIF compliant.
    /// compliant.
    ///
    /// @param op1Type Type pointer to the first operand involved in the
    ///   operation.
    ///
    /// @param op2Type Type pointer to the second operand involved in the
    ///   operation.
    ///
    /// @param operation the operation applied to the operands.
    ///
    /// @param sourceObj pointer to starting value from which is starts analysis.
    ///
    /// @return the struct containing informations about the return type and
    ///   the type required to be casted the operands if they are different.
    virtual ExpressionTypeInfo getExprType(Type *op1Type, Type *op2Type, Operator operation, Object *sourceObj);
    ///
    /// @brief Function that given a Type returns a Value pointer
    ///   to the default value associated to the type according with HIF
    ///   semantics.
    ///
    /// @param t Type pointer to the type where the default value have to
    ///   be established.
    /// @param d Declaration to which the value refers to.
    ///
    /// @return Value pointer to the type default value.
    ///
    virtual Value *getTypeDefaultValue(Type *t, Declaration *d);
    /// @brief Function that given a Type returns a Type pointer to
    ///   the correspondent HIF type (if exists) according HIF language.
    ///
    /// @param t Type pointer to the type to analyze.
    /// @return Type pointer to the correspondent type if exists, nullptr otherwise.
    ///
    virtual Type *getMapForType(Type *t);
    virtual Operator getMapForOperator(Operator srcOperation, Type *srcT1, Type *srcT2, Type *dstT1, Type *dstT2);
    /// @brief Function that given a type and an operation to do with
    /// operators of that type returns a pointer to a Type
    /// representing the type that operands have to be cast into to
    /// obtain a valid operation in target SystemC (if possible).
    ///
    /// @param t Type pointer to the operands type.
    /// @param operation op entry enumeration representing operation to do
    /// @param opType The type of operand to be casted.
    /// @param startingObject The startingObject.
    /// @param isOp1 True if it is the type for op1.
    ///
    /// @return Type pointer to the suggested type to obtain
    /// a valid operation in current semantics  (if possible).
    ///
    virtual Type *getSuggestedTypeForOp(Type *t, Operator operation, Type *opType, Object *startingObject, bool isOp1);
    ///
    /// @brief Function that given a ConstValue returns a Type
    /// pointer representing the type to associate to the constant according
    /// HIF requirements (opportunely setting flags of the type).
    ///
    /// @param c ConstValue pointer to the constant to analyze.
    ///
    /// @return Type pointer to the constant type.
    ///
    virtual Type *getTypeForConstant(ConstValue *c);
    ///
    /// @brief Function that given Type representing a valid guard condition.
    /// Checks if its type is compatible with target language requirements.
    ///
    /// @param t Type pointer to type of the condition to analyze.
    /// @param o The object containing the condition.
    ///
    /// @return true if the type of the condition is compatible with target
    /// language requirements, false otherwise.
    ///
    virtual bool checkCondition(Type *t, Object *o);
    ///
    /// @brief Function that given a Value representing a valid guard
    /// condition in VHDL returns an expression representing the explanation
    /// of the guard as boolean condition (with == operator).
    ///
    /// @param c Value pointer to the condition to analyze.
    ///
    /// @return Value pointer to the created expression.
    ///
    virtual Value *explicitBoolConversion(Value *c);
    ///
    /// @brief Function that given a Value and a type which the given
    /// object have to be cast into, returns a Value representing the
    /// explicit cast (it may be a nested cast, a function call, a procedure
    /// call or a constant) according with target language semantics.
    ///
    /// @param valueToCast Value pointer to the value to cast.
    /// @param castType Type pointer to the type which the given
    /// Value have to be cast.
    /// @param srcType The type of value in source semantics.
    ///
    /// @return Value pointer to the explicit cast.
    ///
    virtual Value *explicitCast(Value *valueToCast, Type *castType, Type *srcType);
    /// @brief Function that given a real value returns the correspondent
    /// int value according to semantics rules (e.g., VHDL rounds while
    /// SystemC truncates).
    /// @param v The value to convert.
    /// @return The converted value.
    virtual std::int64_t transformRealToInt(const double v);
    /// @name Semantic checks methods
    ///@{

    /// @brief Function that given a Type checks whether it is suitable
    /// as bound of a Range
    ///
    /// @param t The type to be checked.
    /// @return nullptr if allowed, a suitable fresh type otherwise.
    ///
    virtual Type *isTypeAllowedAsBound(Type *t);

    /// @brief Function that given the target type and the source type
    /// of a Cast checks whether it is valid a direct conversion with
    /// respect to semantics.
    ///
    /// @param target The target type to be checked.
    /// @param source The source type to be checked.
    /// @return True if allowed, False otherwise.
    ///
    virtual bool isCastAllowed(Type *target, Type *source);

    /// @brief Returns true if given type is allowed in the semantics.
    ///
    /// @param t The type to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isTypeAllowed(Type *t);

    /// @brief Returns true if given range direction is allowed in the semantics.
    ///
    /// @param r The range to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isRangeDirectionAllowed(RangeDirection r);

    /// @brief Returns true if given type is allowed as switch or with
    /// value in the semantics.
    ///
    /// @param t The type to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isTypeAllowedAsCase(Type *t);

    /// @brief Returns true if given type is allowed as port type.
    ///
    /// @param t The type to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isTypeAllowedAsPort(Type *t);

    /// @brief Returns true if given language id is allowed in the semantics.
    ///
    /// @param id The language id to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isLanguageIdAllowed(LanguageID id);

    /// @brief Returns true if given value is allowed for port binding
    /// in the semantics.
    ///
    /// @param o The value to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isValueAllowedInPortBinding(Value *o);

    /// @brief Returns true if given const value syntactic type is allowed
    /// for the const value in the semantics.
    ///
    /// @param cv The value to be checked.
    /// @param synType The syntactic type to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isTypeAllowedForConstValue(ConstValue *cv, Type *synType);

    ///@}
    /// @name Template related stuff
    ///@{

    /// @brief Checks if type is allowed as template type w.r.t.
    ///semantics.
    /// @param t The type to check.
    /// @return True if is allowed.
    ///
    virtual bool isTemplateAllowedType(Type *t);
    /// @brief Return the mapped of given type that is allowed as type
    /// in template parameter w.r.t. semantics.
    /// @param t The type to map.
    /// @return The allowed type.
    ///
    virtual Type *getTemplateAllowedType(Type *t);

    ///@}
    ///
    /// Singleton stuff.
    ///
    /// @brief Function thats return an instance to VHDLSemantic class.
    /// @return The instance of HIFSemantics.
    static HIFSemantics *getInstance();
    /// @brief Checks whether a name is forbidden in the current semantics.
    /// @param decl The declaration to check.
    /// @return true if the name is forbidden, false otherwise.
    virtual bool isForbiddenName(Declaration *decl);

    /// @brief Returns true when semantics type of slice must be rebased.
    /// @return true if slice type must be rebased.
    virtual bool isSliceTypeRebased();

    /// @brief Returns true when syntactic type must be rebased.
    /// @return true if syntactic type must be rebased.
    virtual bool isSyntacticTypeRebased();

    virtual Type *getMemberSemanticType(Member *m);

    /// @name Standard packages
    /// @{

    /// @brief Gets the standard package.
    /// @return The standard package LibraryDef.
    LibraryDef *getStandardPackage();

    /// @brief Get the eventual LibraryDef matching the given name.
    /// @param name The name.
    /// @return The LibraryDef or nullptr.
    virtual LibraryDef *getStandardLibrary(const std::string &name);

    /// @brief Return True if the given library is native for the semantics.
    /// @param name The library name.
    /// @param hifFormat Whether to use HIF format.
    /// @return True if the library is native, false otherwise.
    virtual bool isNativeLibrary(const std::string &name, bool hifFormat = false);

    /// @brief Map an input symbol into the corresponding output one.
    /// @param decl The declaration.
    /// @param key The key symbol.
    /// @param value The value symbol.
    /// @param srcSem The source semantics.
    /// @return The mapping case.
    virtual MapCases
    mapStandardSymbol(Declaration *decl, KeySymbol &key, ValueSymbol &value, ILanguageSemantics *srcSem);

    /// @brief Returns the mapped symbol w.r.t. the current semantics.
    /// @param key The key symbol.
    /// @param s The object.
    /// @return The simplified symbol.
    virtual Object *getSimplifiedSymbol(KeySymbol &key, Object *s);

    /// @brief Returns the event method name w.r.t. current semantics.
    /// @param hifFormat True if HIF format.
    /// @return The event method name.
    virtual std::string getEventMethodName(bool hifFormat = false);

    /// @brief Returns <tt>true</tt> if the given call is an event call w.r.t.
    /// the current semantics, <tt>false</tt> otherwise.
    /// @param call The function call to check.
    /// @return True if event call, false otherwise.
    virtual bool isEventCall(FunctionCall *call);

    /// @}

private:
    HIFSemantics();
    virtual ~HIFSemantics();

    // disabled
    HIFSemantics(const HIFSemantics &);
    HIFSemantics &operator=(const HIFSemantics &);

    /// Initialization methods.
    void _initForbiddenNames();
    void _initStandardSymbols();
    void _initStandardFilenames();

    /// Mapping methods (libraries)
    Object *_getSimplifiedSymbolIeeeMathComplex(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolIeeeMathReal(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolIeeeNumericBit(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolIeeeNumericStd(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolIeeeStdLogic1164(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolIeeeStdLogicArith(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolIeeeStdLogicArithEx(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolIeeeStdLogicMisc(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolIeeeStdLogicSigned(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolIeeeStdLogicTextIO(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolIeeeStdLogicUnsigned(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolStandard(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolVamsStandard(KeySymbol &key, Object *s);
    Object *_getSimplifiedSymbolStdTextIO(KeySymbol &key, Object *s);

    /// Mapping methods (functions)
    Object *_getSimplifiedSymbol_left(Object *s);
    Object *_getSimplifiedSymbol_right(Object *s);
    Object *_getSimplifiedSymbol_high(Object *s);
    Object *_getSimplifiedSymbol_low(Object *s);
    Object *_getSimplifiedSymbol_length(Object *s);
    Object *_getSimplifiedSymbol_stable(Object *s);
    Object *_getSimplifiedSymbol_ascending(Object *s);
    Object *_getSimplifiedSymbol_iteratedConcat(Object *s);
    Object *
    _getSimplifiedSymbol_withVerilogIntegers(Object *s, bool intReturnedType, const std::vector<int> &intParamIndexes);

    /// Splitting cases.
    LibraryDef *_getVHDLStandardLibrary(const std::string &name);
    LibraryDef *_getVerilogStandardLibrary(const std::string &name);
    LibraryDef *_getSystemCStandardLibrary(const std::string &name);
    LibraryDef *_getHIFStandardLibrary(const std::string &name);

    typedef std::list<hif::SubProgram *> SubList;
    typedef std::map<std::string, SubList> DeclarationMap;
    DeclarationMap _stdMap;
};

} // namespace semantics
} // namespace hif
