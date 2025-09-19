/// @file SystemCSemantics.hpp
/// @brief Semantics and rules for SystemC language constructs in HIF.
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
/// @brief This class implements the SystemC semantics.
///
/// It is an implementation of the ILanguageSemantics interface.
class SystemCSemantics : public ILanguageSemantics
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
    /// @param srcObj pointer to starting value from which is starts analysis.
    ///
    /// @return the struct containing informations about the return type and
    ///   the type required to be casted the operands if they are different.
    virtual ExpressionTypeInfo getExprType(Type *op1Type, Type *op2Type, Operator operation, Object *srcObj);
    /// @brief Function that given a Type returns a Value pointer
    ///   to the default value associated to the type according with SystemC
    ///   semantics.
    ///
    /// @param type Type pointer to the type where the default value have to
    ///   be established.
    /// @param d Declaration to which the value refers to.
    ///
    /// @return Value pointer to the type default value.
    ///
    virtual Value *getTypeDefaultValue(Type *type, Declaration *d);
    /// @brief Function that given a Type returns a Type pointer to
    ///   the correspondent SystemC type (if exists) according SystemC language.
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
    /// @brief Function that given a ConstValue returns a Type
    /// pointer representing the type to associate to the constant according
    /// SystemC requirements (opportunely setting flags of the type).
    ///
    /// @param c ConstValue pointer to the constant to analyze.
    ///
    /// @return Type pointer to the constant type.
    ///
    virtual Type *getTypeForConstant(ConstValue *c);
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
    /// @brief Function that given a Value representing a valid guard
    /// condition in SystemC returns an expression representing the explanation
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

    /// @brief Function that given a member returns its semantic type
    /// according to semantics rules (e.g., SystemC with Bitvector prefix type
    /// returns sc_proxy).
    /// @param m The member.
    /// @return The semantic type.
    virtual Type *getMemberSemanticType(Member *m);

    /// @brief Function that given a slice returns its semantic type
    /// according to semantics rules (e.g., SystemC with Bitvector prefix type
    /// returns sc_proxy).
    /// @param s The slice.
    /// @return The semantic type.
    virtual Type *getSliceSemanticType(Slice *s);
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

    /// @brief Returns true if given type is allowed for the return value
    /// of a function.
    ///
    /// @param type The type to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isTypeAllowedAsReturn(Type *type);

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
    /// Singleton stuff.
    ///
    /// @brief Function thats return an instance to SystemCSemantics class.
    /// @return The instance.
    static SystemCSemantics *getInstance();
    /// @brief This function returns true if the given operator is supported by
    /// the semantics, false otherwise.
    /// @param operation The operator to check.
    /// @return True if supported.
    bool isSupported(Operator operation);

    /// @brief Checks whether a name is forbidden in the current semantics.
    /// @param decl The declaration to check.
    /// @return True if forbidden.
    virtual bool isForbiddenName(Declaration *decl);

    /// @brief Returns true when semantics type of slice must be rebased.
    /// @return true if slice type must be rebased.
    virtual bool isSliceTypeRebased();

    /// @brief Returns true when syntactic type must be rebased.
    /// @return true if syntactic type must be rebased.
    virtual bool isSyntacticTypeRebased();

    /// @name Flags management.
    /// @{

    /// @brief Set if Semantics have to use hdtlib library types.
    /// @param v The value to set.
    void setUseHdtlib(bool v);

    /// @brief Get if Semantics have use hdtlib library types.
    /// @return True if using hdtlib.
    bool usingHdtlib();

    /// @brief Set if Semantics have to keep bit deprecated bit type
    /// @param v The value to set.
    void setKeepBit(bool v);

    /// @brief Get if Semantics have to keep bit deprecated bit type
    /// @return True if keeping bit.
    bool getKeepBit();

    /// @brief Set if Semantics have to preserve resolved types.
    /// @param v The value to set.
    void setResolvedTypes(bool v);

    /// @brief Get if Semantics have to preserve resolved types.
    /// @return True if preserving resolved types.
    bool getResolvedTypes();
    /// @}
    /// @name Standard libraries.
    /// @{

    /// @brief Get the standard package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The standard package.
    LibraryDef *getStandardPackage(bool hifFormat = false);

    /// @brief Get the new package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The new package.
    LibraryDef *getNewPackage(bool hifFormat = false);

    /// @brief Get the SystemVue model builder.
    /// @param hifFormat Whether to use HIF format.
    /// @return The SystemVue model builder.
    LibraryDef *getSystemVueModelBuilder(bool hifFormat = false);

    /// @brief Get the C math package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The C math package.
    LibraryDef *getCMathPackage(bool hifFormat = false);

    /// @brief Get the C stdio package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The C stdio package.
    LibraryDef *getCStdIOPackage(bool hifFormat = false);

    /// @brief Get the C stdlib package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The C stdlib package.
    LibraryDef *getCStdLibPackage(bool hifFormat = false);

    /// @brief Get the C time package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The C time package.
    LibraryDef *getCTimePackage(bool hifFormat = false);

    /// @brief Get the ddt clib package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The ddt clib package.
    LibraryDef *getDdtClibPackage(bool hifFormat = false);

    /// @brief Get the hdtlib package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The hdtlib package.
    LibraryDef *getHdtlibPackage(bool hifFormat = false);

    /// @brief Get the iostream package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The iostream package.
    LibraryDef *getIOStreamPackage(bool hifFormat = false);

    /// @brief Get the sc ams eln package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The sc ams eln package.
    LibraryDef *getScAmsELNPackage(bool hifFormat = false);

    /// @brief Get the sc core package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The sc core package.
    LibraryDef *getScCorePackage(bool hifFormat = false);

    /// @brief Get the string package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The string package.
    LibraryDef *getStringPackage(bool hifFormat = false);

    /// @brief Get the cstring package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The cstring package.
    LibraryDef *getCStringPackage(bool hifFormat = false);

    /// @brief Get the c stddef package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The c stddef package.
    LibraryDef *getCStdDefPackage(bool hifFormat = false);

    /// @brief Get the systemc extensions package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The systemc extensions package.
    LibraryDef *getSystemcExtensionsPackage(bool hifFormat = false);

    /// @brief Get the tlm package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The tlm package.
    LibraryDef *getTlmPackage(bool hifFormat = false);

    /// @brief Get the vector package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The vector package.
    LibraryDef *getVectorPackage(bool hifFormat = false);

    /// @brief Get the tlm utils.
    /// @param hifFormat Whether to use HIF format.
    /// @return The tlm utils.
    LibraryDef *getTlmUtils(bool hifFormat = false);

    /// @brief Get the sc dt package.
    /// @param hifFormat Whether to use HIF format.
    /// @return The sc dt package.
    LibraryDef *getScDtPackage(bool hifFormat = false);

    /// @brief Get the eventual LibraryDef matching the given name.
    /// @param n The name.
    /// @return The LibraryDef or nullptr.
    virtual LibraryDef *getStandardLibrary(const std::string &n);

    /// @brief Return True if the given library is native for the semantics.
    /// @param n The library name.
    /// @param hifFormat Whether to use HIF format.
    /// @return True if native.
    virtual bool isNativeLibrary(const std::string &n, bool hifFormat = false);

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

    /// @brief Returns the namespace used by given library name.
    /// E.g. for "iostream" returns "std"
    /// @param n The library name.
    /// @return The namespace.
    std::string getLibraryNamespace(const std::string &n);

    /// @}
private:
    SystemCSemantics();
    virtual ~SystemCSemantics();

    // disabled
    SystemCSemantics(const SystemCSemantics &);
    SystemCSemantics &operator=(const SystemCSemantics &);

    /// Initialization methods.
    void _initForbiddenNames();
    void _initStandardSymbols();
    void _initStandardFilenames();

    // Utility methods

    // Generic stub for HifAggregateArray, HifAggregateBitVector, HifAggregateLogicVector
    DesignUnit *_makeHifAggregateDU(
        HifFactory &factory,
        const char *duName,
        Type *aggregateType,
        Type *parameterType,
        bool hifFormat);

    /// @name Methods used by canRemoveCastOnOperands.
    /// @{

    virtual bool _checkConcatCasts(
        Expression *e,
        Type *castT1,
        Type *castT2,
        Type *subT1,
        Type *subT2,
        ExpressionTypeInfo &exprInfo,
        ExpressionTypeInfo &info);

    /// @}
    typedef std::list<hif::SubProgram *> SubList;
    typedef std::map<std::string, SubList> DeclarationMap;
    DeclarationMap _stdMap;

    bool _useHdtlib;
    bool _keepBit;
    bool _resolvedTypes;
};

} // namespace semantics
} // namespace hif
