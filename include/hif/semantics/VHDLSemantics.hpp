/// @file VHDLSemantics.hpp
/// @brief Header file of VHDLSemantics class.
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

///	@brief This class is an implementation virtual class ILanguageSemantics.
/// It implements VHDL semantics.
class VHDLSemantics : public ILanguageSemantics
{
public:
    /// @brief Returns the name of the semantics.
    /// @return The name of the semantics.
    virtual std::string getName() const;

    ///
    /// @brief Function that given a couple of Type and an operation to do on them
    /// returns a struct containing informations about the operation according
    /// with VHDL semantics.
    ///
    /// @param op1Type Type pointer to the first operand involved in the operation.
    ///
    /// @param op2Type Type pointer to the second operand involved in the operation.
    ///
    /// @param operation op enumeration entry representing the operation to do.
    ///
    /// @param sourceObj pointer to starting value from which is starts analysis.
    ///
    /// @return ExpressionTypeInfo struct containing informations about the operation
    /// according with VHDL semantics.
    ///
    virtual ExpressionTypeInfo getExprType(Type *op1Type, Type *op2Type, Operator operation, Object *sourceObj);
    ///
    /// @brief Function that given a Type returns a Value pointer to the
    /// default value associated to the type according with VHDL semantics.
    ///
    /// @param t Type pointer to the type where the default value have to be established.
    /// @param d Declaration to which the value refers to.
    ///
    /// @return Value pointer to the type default value.
    ///
    virtual Value *getTypeDefaultValue(Type *t, Declaration *d);
    ///
    /// @brief Function that given a Type returns a Type pointer to the
    /// correspondent VHDL type (if exists) according VHDL language.
    ///
    /// @param t Type pointer to the type to analyze.
    ///
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
    /// VHDL requirements (opportunely setting flags of the type).
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

    /// @brief Checks whether the operations on given bit type are logic or bitwise.
    /// E.g. in Verilog b1 & b2 is a bitwise operation.
    /// In VHDL b1 and b2 is bitwise if type is logic, logical and otherwise.
    ///
    /// @param isLogic If the type is logic.
    /// @return True if operators are bitwise, false if they are logical.
    ///
    virtual bool hasBitwiseOperationsOnBits(bool isLogic) const;

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
    /// @return The instance.
    static VHDLSemantics *getInstance();
    ///
    /// @brief This function returns true if the given operator is supported by
    /// the semantics, false otherwise.
    /// @param operation The operator to check.
    /// @return True if supported.
    bool isSupported(Operator operation);

    /// @brief Checks whether a name is forbidden in the current semantics.
    /// @param decl The declaration to check.
    /// @return True if forbidden.
    virtual bool isForbiddenName(Declaration *decl);

    /// @brief Check if slice type is rebased.
    /// @return True if slice type is rebased.
    virtual bool isSliceTypeRebased();

    /// @brief Check if syntactic type is rebased.
    /// @return True if syntactic type is rebased.
    virtual bool isSyntacticTypeRebased();

    /// @name Flags management.
    /// @{

    /// @brief Set if Semantics have to use PSL relaxed checks.
    /// @param v The value to set.
    void setUsePsl(bool v);

    /// @brief Get if Semantics have use PSL relaxed checks.
    /// @return True if using PSL.
    bool usingPsl();

    /// @}

    /// @name Standard packages
    /// @{

    /// @brief Get the IEEE math complex package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getIeeeMathComplexPackage(bool hifFormat = false);

    /// @brief Get the IEEE math real package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getIeeeMathRealPackage(bool hifFormat = false);

    /// @brief Get the IEEE numeric bit package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getIeeeNumericBitPackage(bool hifFormat = false);

    /// @brief Get the IEEE numeric std package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getIeeeNumericStdPackage(bool hifFormat = false);

    /// @brief Get the IEEE std logic 1164 package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getIeeeStdLogic1164Package(bool hifFormat = false);

    /// @brief Get the IEEE std logic arith package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getIeeeStdLogicArithPackage(bool hifFormat = false);

    /// @brief Get the IEEE std logic arith ex package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getIeeeStdLogicArithExPackage(bool hifFormat = false);

    /// @brief Get the IEEE std logic misc package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getIeeeStdLogicMiscPackage(bool hifFormat = false);

    /// @brief Get the IEEE std logic signed package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getIeeeStdLogicSignedPackage(bool hifFormat = false);

    /// @brief Get the IEEE std logic text IO package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getIeeeStdLogicTextIOPackage(bool hifFormat = false);

    /// @brief Get the IEEE std logic unsigned package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getIeeeStdLogicUnsignedPackage(bool hifFormat = false);

    /// @brief Get the standard package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getStandardPackage(bool hifFormat = false);

    /// @brief Get the text IO package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getTextIOPackage(bool hifFormat = false);

    /// @brief Get the PSL standard package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The package.
    LibraryDef *getPSLStandardPackage(bool hifFormat = false);

    /// @brief Get the eventual library def matching the given name.
    /// @param n The name.
    /// @return The library def or nullptr.
    virtual LibraryDef *getStandardLibrary(const std::string &n);

    /// @brief Return True if the given library is native for the semantics.
    /// @param n The library name.
    /// @param hifFormat Whether to use HIF format.
    /// @return True if native.
    virtual bool isNativeLibrary(const std::string &n, bool hifFormat = false);

    /// @brief Starting from system adds all required standard packeges.
    /// @param s The system.
    virtual void addStandardPackages(System *s);

    /// @brief Map an input symbol into the corresponding output one.
    /// @param decl The declaration.
    /// @param key The key symbol.
    /// @param value The value symbol.
    /// @param srcSem The source semantics.
    /// @return The map cases.
    virtual MapCases
    mapStandardSymbol(Declaration *decl, KeySymbol &key, ValueSymbol &value, ILanguageSemantics *srcSem);

    /// @brief Returns the mapped symbol w.r.t. the current semantics.
    /// @param key The key symbol.
    /// @param s The object.
    /// @return The mapped object.
    virtual Object *getSimplifiedSymbol(KeySymbol &key, Object *s);

    /// @brief Returns true if no namespaces is needed for given library name.
    /// @param n The library name.
    /// @param isLibInclusion Whether it's a library inclusion.
    /// @return True if standard inclusion.
    virtual bool isStandardInclusion(const std::string &n, bool isLibInclusion);

    /// @brief Returns the event method name w.r.t. current semantics.
    /// @param hifFormat Whether to use HIF format.
    /// @return The event method name.
    virtual std::string getEventMethodName(bool hifFormat = false);

    /// @brief Returns <tt>true</tt> if the given call is an event call w.r.t.
    /// the current semantics, <tt>false</tt> otherwise.
    /// @param call The function call.
    /// @return True if event call.
    virtual bool isEventCall(FunctionCall *call);

    /// @}

private:
    VHDLSemantics();
    virtual ~VHDLSemantics();

    // disabled
    VHDLSemantics(const VHDLSemantics &);
    VHDLSemantics &operator=(const VHDLSemantics &);

    /// Initialization methods.
    void _initForbiddenNames();
    void _initStandardSymbols();
    void _initStandardFilenames();

    typedef std::list<hif::SubProgram *> SubList;
    typedef std::map<std::string, SubList> DeclarationMap;
    DeclarationMap _stdMap;
    bool _usingPsl;
};
} // namespace semantics
} // namespace hif
