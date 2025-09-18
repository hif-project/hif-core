/// @file ILanguageSemantics.hpp
/// @brief File containing the definition of the ILanguageSemantics class.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <iostream>

#include "hif/HifFactory.hpp"
#include "hif/application_utils/portability.hpp"
#include "hif/classes/ConstValue.hpp"
#include "hif/classes/Declaration.hpp"
#include "hif/classes/Object.hpp"
#include "hif/classes/Type.hpp"
#include "hif/classes/Value.hpp"
#include "hif/hif_utils/equals.hpp"
#include "hif/manipulation/sortParameters.hpp"

namespace hif
{
namespace semantics
{

/// @brief This class is a virtual class to support semantic of a specific HDL.
/// In consists in a set of methods to retrieve informations about HIF objects
/// according to a specific HDL semantic.
class ILanguageSemantics
{
public:
    /// @brief Enum for supported languages.
    enum SupportedLanguages : unsigned char {
        VHDL,    ///< VHDL semantics.
        Verilog, ///< Verilog semantics.
        SystemC, ///< SystemC semantics.
        HIF      ///< HIF semantics.
    };

    /// @brief Enum for mapping standard symbols between source and target semantics.
    enum MapCases : unsigned char {
        UNSUPPORTED, ///< Symbol is not supported by either semantics; an error is expected.
        SIMPLIFIED,  ///< Symbol is transformed into a simplified construct.
        MAP_KEEP,    ///< Symbol is mapped to another symbol; original declaration is kept.
        MAP_DELETE,  ///< Symbol is mapped to another symbol; original declaration is removed.
        UNKNOWN      ///< Symbol is unsupported in the target semantics.
    };

    /// @brief Struct representing information about the type of an expression.
    struct ExpressionTypeInfo {
        Type *returnedType;       ///< The type returned by the operation.
        Type *operationPrecision; ///< Precision applied to operands in the operation.

        /// @brief Default constructor.
        ExpressionTypeInfo();

        /// @brief Copy constructor.
        ExpressionTypeInfo(const ExpressionTypeInfo &t);

        /// @brief Destructor.
        virtual ~ExpressionTypeInfo();

        /// @brief Assignment operator.
        ExpressionTypeInfo &operator=(const ExpressionTypeInfo &t);
    };

    /// @name Mapping Standard Symbols
    /// @{
    typedef std::pair<std::string, std::string> KeySymbol; ///< Input symbol mapping key.
    typedef std::list<std::string> LibraryList;            ///< Libraries associated with a symbol.

    /// @brief Represents a mapped standard symbol.
    struct ValueSymbol {
        LibraryList libraries;    ///< Libraries required for the symbol.
        std::string mappedSymbol; ///< The mapped symbol name.
        MapCases mapAction;       ///< Action to perform on the symbol.

        /// @brief Default constructor.
        ValueSymbol();

        /// @brief Destructor.
        ~ValueSymbol();

        /// @brief Copy constructor.
        ValueSymbol(const ValueSymbol &other);

        /// @brief Assignment operator.
        ValueSymbol &operator=(const ValueSymbol &other);
    };

    typedef std::map<KeySymbol, ValueSymbol> StandardSymbols;        ///< Map of standard symbols.
    typedef std::map<std::string, std::string> StandardLibraryFiles; ///< Map of library names to filenames.
    /// @}

    /// @brief Struct representing the semantic options.
    struct SemanticOptions {
        enum ForConditionType { RANGE, EXPRESSION, RANGE_AND_EXPRESSION }; ///< Allowed types for `for` conditions.

        bool port_inNoInitialValue;         ///< Ensure input ports have no initial values.
        bool port_outInitialValue;          ///< Ensure output ports have initial values.
        bool dataDeclaration_initialValue;  ///< Ensure declarations have initial values.
        bool scopedType_insideTypedef;      ///< Ensure scoped types are inside typedefs.
        bool int_bitfields;                 ///< Treat native ints as bitfields.
        bool designUnit_uniqueView;         ///< Ensure a design unit has only one view.
        bool for_implictIndex;              ///< Ensure `for` loops have a single implicit index declaration.
        ForConditionType for_conditionType; ///< Allowed type for `for` conditions.
        bool generates_isNoAllowed;         ///< Disallow `generate` constructs.
        bool after_isNoAllowed;             ///< Disallow `after` constructs.
        bool with_isNoAllowed;              ///< Disallow `with` constructs.
        bool globact_isNoAllowed;           ///< Disallow global actions.
        bool valueStatement_isNoAllowed;    ///< Disallow value statements.
        bool case_isOnlyLiteral;            ///< Restrict case constructs to literals only.
        bool lang_signPortNoBitAccess;      ///< Disallow bit access on signals and ports.
        bool lang_hasDontCare;              ///< Indicate language supports "don't care" values.
        bool lang_has9logic;                ///< Indicate language has 9 logic values (false means 4 values).
        bool waitWithActions;               ///< Allow waits to embrace actions.
        hif::manipulation::SortMissingKind::type lang_sortKind; ///< Sorting strategy for missing parameters.

        SemanticOptions();                                    ///< Default constructor.
        ~SemanticOptions();                                   ///< Destructor.
        SemanticOptions(const SemanticOptions &t);            ///< Copy constructor.
        SemanticOptions &operator=(const SemanticOptions &t); ///< Assignment operator.
    };

    /// @brief Returns the current semantic options.
    /// @return SemanticOptions object.
    const SemanticOptions &getSemanticsOptions() const;

    /// @brief Virtual destructor for polymorphic deletion.
    virtual ~ILanguageSemantics() = 0;

    /// @brief Returns the name of the semantics.
    virtual std::string getName() const = 0;

    /// @name Type management methods.
    ///@{

    ///
    /// @brief Function that given a couple of Type and an operation to do on them
    /// returns a struct containing informations about the returned type of the operation
    /// and the precision applied to the operands. Pure virtual method.
    ///
    /// @param op1Type Type pointer to the first operand involved in the operation.
    ///
    /// @param op2Type Type pointer to the second operand involved in the operation.
    ///
    /// @param operation op enumeration entry representing the operation to do.
    ///
    /// @param sourceObj pointer to starting value from which is starts analysis.
    ///
    /// @return type_info_t ExpressionTypeInfo containing informations about the operation.
    ///
    virtual ExpressionTypeInfo getExprType(Type *op1Type, Type *op2Type, Operator operation, Object *sourceObj) = 0;
    /// @brief Function to change the semantic checks mode.
    /// Usually, they are more permessive inside frontends,
    /// whilst backends are more strict.
    ///
    /// @param v The mode.
    ///
    void setStrictTypeChecks(const bool v);

    /// @brief Function to get the semantic checks mode.
    ///
    /// @return The mode.
    ///
    bool getStrictTypeChecks() const;

    ///
    /// @brief Function that given a Type returns a Value
    /// pointer to the default value associated to the type.
    /// Pure virtual method.
    ///
    /// @param t Type pointer to the type where the default value
    /// have to be established.
    /// @param d Declaration to which the value refers to.
    ///
    /// @return Type pointer to the correspondent type if exists, nullptr otherwise.
    ///
    virtual Value *getTypeDefaultValue(Type *t, Declaration *d)                                                   = 0;
    ///
    /// @brief Function that given a Type returns a Type pointer
    /// to the correspondent type in the target language (if exists).
    /// Pure virtual method.
    ///
    /// @param t Type pointer to the type to analyze.
    ///
    /// @return Type pointer to the correspondent fresh type.
    ///
    virtual Type *getMapForType(Type *t)                                                                          = 0;
    /// @brief Function that given the operator and the Types of expression
    /// in the source semantics, returns the (eventually changed) operator
    /// in the target language (if exists).
    ///
    /// @param srcOperation The operator.
    /// @param srcT1 The type of the first operand in source semantics.
    /// @param srcT2 The type of the second operand in source semantics.
    /// @param dstT1 The type of the first operand in destination semantics.
    /// @param dstT2 The type of the second operand in destination semantics.
    /// @return Type pointer to the correspondent fresh type.
    ///
    virtual Operator getMapForOperator(Operator srcOperation, Type *srcT1, Type *srcT2, Type *dstT1, Type *dstT2) = 0;
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
    virtual Type *
    getSuggestedTypeForOp(Type *t, Operator operation, Type *opType, Object *startingObject, const bool isOp1) = 0;
    /// @brief Function that given a ConstValue returns a Type
    /// pointer representing the type to associate to the constant according
    /// with target language requirements (opportunely setting flags of the type).
    /// Pure virtual method.
    ///
    /// @param c ConstValue pointer to the constant to analyze.
    ///
    /// @return Type pointer to the constant type.
    ///
    virtual Type *getTypeForConstant(ConstValue *c)                                                            = 0;
    /// @brief Checks if given type is compatible with language requirements
    /// as condition.
    ///
    /// @param t Type of the condition to analyze.
    /// @param o The object containing the condition.
    ///
    /// @return true if the type of the condition is compatible with target
    /// language requirements, false otherwise.
    ///
    virtual bool checkCondition(Type *t, Object *o)                                                            = 0;
    /// @brief Function that given a Value representing a valid guard
    /// condition in the target language returns an expression representing
    /// the explanation of the guard as boolean condition (with == operator).
    /// Pure virtual method.
    ///
    /// @param c Value pointer to the condition to analyze.
    ///
    /// @return Value pointer to the created expression.
    ///
    virtual Value *explicitBoolConversion(Value *c)                                                            = 0;
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
    virtual Value *explicitCast(Value *valueToCast, Type *castType, Type *srcType)                             = 0;
    /// @brief Function that given a real value returns the correspondent
    /// int value according to semantics rules (e.g., VHDL rounds while
    /// SystemC truncates).
    /// @param v The value to convert.
    /// @return The converted value.
    virtual long long transformRealToInt(const double v)                                                       = 0;

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

    ///@}
    /// @name Semantic checks methods
    ///@{

    /// @brief Function that given a Type checks whether it is suitable
    /// as bound of a Range
    ///
    /// @param t The type to be checked.
    /// @return nullptr if allowed, a suitable fresh type otherwise.
    ///
    virtual Type *isTypeAllowedAsBound(Type *t) = 0;

    /// @brief Function that given the target type and the source type
    /// of a Cast checks whether it is valid a direct conversion with
    /// respect to semantics.
    ///
    /// @param target The target type to be checked.
    /// @param source The source type to be checked.
    /// @return True if allowed, False otherwise.
    ///
    virtual bool isCastAllowed(Type *target, Type *source) = 0;

    /// @brief Returns true if given type is allowed in the semantics.
    ///
    /// @param t The type to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isTypeAllowed(Type *t) = 0;

    /// @brief Returns true if given range direction is allowed in the semantics.
    ///
    /// @param r The range to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isRangeDirectionAllowed(RangeDirection r) = 0;

    /// @brief Returns true if given type is allowed as switch or with
    /// value in the semantics.
    ///
    /// @param t The type to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isTypeAllowedAsCase(Type *t) = 0;

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
    virtual bool isTypeAllowedAsPort(Type *t) = 0;

    /// @brief Returns true if given language id is allowed in the semantics.
    ///
    /// @param id The language id to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isLanguageIdAllowed(LanguageID id) = 0;

    /// @brief Returns true if given value is allowed for port binding
    /// in the semantics.
    ///
    /// @param o The value to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isValueAllowedInPortBinding(Value *o) = 0;

    /// @brief Returns true if given const value syntactic type is allowed
    /// for the const value in the semantics.
    ///
    /// @param cv The value to be checked.
    /// @param synType The syntactic type to be checked.
    /// @return true if is allowed.
    ///
    virtual bool isTypeAllowedForConstValue(ConstValue *cv, Type *synType) = 0;

    /// @brief Checks whether the cast on operands can be removed safetely.
    ///
    /// @param e The original expression.
    /// @param origInfo The original expression Info.
    /// @param simplifiedInfo The info without casts.
    /// @param castT1 The type of first operand cast.
    /// @param castT2 The type of second operand cast.
    /// @param subT1 The type of first operand without cast.
    /// @param subT2 The type of second operand without cast.
    /// @param canRemoveOnShift In case of shift operation, indicates wheter
    ///        the possible cast on 2nd operand can be removed.
    /// @return <tt>true</tt> if cast can be removed.
    ///
    bool canRemoveCastOnOperands(
        Expression *e,
        ExpressionTypeInfo &origInfo,
        ExpressionTypeInfo &simplifiedInfo,
        Type *castT1,
        Type *castT2,
        Type *subT1,
        Type *subT2,
        bool &canRemoveOnShift);

    /// @brief Checks whether the operations on given bit type are logic or bitwise.
    /// E.g. in Verilog b1 & b2 is a bitwise operation.
    /// In VHDL b1 and b2 is bitwise if type is logic, logical and otherwise.
    ///
    /// @param isLogic If the type is logic.
    /// @return True if operators are bitwise, false if they are logical.
    ///
    virtual bool hasBitwiseOperationsOnBits(const bool isLogic) const;

    ///@}
    /// @name Template related stuff
    ///@{

    /// @brief Checks if type is allowed as template type w.r.t.
    ///semantics.
    /// @param t The type to check.
    /// @return True if is allowed.
    ///
    virtual bool isTemplateAllowedType(Type *t) = 0;

    /// @brief Return the fresh mapped type w.r.t. given type that is allowed as type
    /// in template parameter w.r.t. semantics.
    /// @param t The type to map.
    /// @return The allowed type.
    ///
    virtual Type *getTemplateAllowedType(Type *t) = 0;

    ///@}
    /// @name Singleton design pattern.
    ///@{

    ///
    /// @brief Function thats return an instance of the required language semantics class.
    ///
    /// @param lang enumeration entry representing the required language.
    ///
    /// @return ILanguageSemantics object reference to the class that implements
    /// the semantics of the required language.
    ///
    static ILanguageSemantics *getInstance(SupportedLanguages lang);
    ///@}

    /// @brief Checks whether a name is forbidden in the current semantics.
    virtual bool isForbiddenName(Declaration *decl) = 0;

    /// @name General support methods.
    /// @{

    /// @brief Gets the context precision for the given object.
    /// @param o The object to get precision for.
    /// @return The range representing the precision.
    Range *getContextPrecision(Object *o);

    /// @brief Returns true when semantics type of slice must be rebased.
    virtual bool isSliceTypeRebased() = 0;

    /// @brief Returns true when syntactic type must be rebased.
    virtual bool isSyntacticTypeRebased() = 0;

    /// @}
    /// @name Standard packages
    /// @{

    /// @brief Get the eventual LibraryDef matching the given name.
    /// @param n The name.
    /// @return The LibraryDef or nullptr.
    virtual LibraryDef *getStandardLibrary(const std::string &n) = 0;

    /// @brief Starting from system adds all required standard packages.
    /// @param s The system.
    virtual void addStandardPackages(System *s);

    /// @brief Return True if the given library is native for the semantics.
    virtual bool isNativeLibrary(const std::string &n, const bool hifFormat = false) = 0;

    /// @brief Map an input symbol into the corresponding output one.
    /// @return True if map succeed.
    virtual MapCases
    mapStandardSymbol(Declaration *decl, KeySymbol &key, ValueSymbol &value, ILanguageSemantics *srcSem) = 0;

    /// @brief Map a library name in the correspondent header file name.
    /// @param n The library name.
    /// @return The corresponding header file name.
    std::string mapStandardFilename(const std::string &n);

    /// @brief Returns the mapped symbol w.r.t. the current semantics.
    virtual Object *getSimplifiedSymbol(KeySymbol &key, Object *s) = 0;

    /// @brief Returns true if no namespaces is needed for given library name.
    virtual bool isStandardInclusion(const std::string &n, const bool isLibInclusion);

    /// @brief Creates a copy of the declaration renaming it adding
    /// the given suffix, and return the fresh new declaration.
    /// @tparam T The type of the declaration.
    /// @param decl The declaration to copy.
    /// @param suffix The suffix to add.
    /// @return The new suffixed declaration.
    template <typename T> T *getSuffixedCopy(T *decl, const std::string &suffix);

    /// @brief Returns the actual library filename (which could be different
    /// from the library name set).
    /// @param n The library name.
    /// @return The actual library filename.
    virtual std::string getStandardFilename(const std::string &n);

    /// @brief Returns the event method name w.r.t. current semantics.
    virtual std::string getEventMethodName(const bool hifFormat = false) = 0;

    /// @brief Returns <tt>true</tt> if the given call is an event call w.r.t.
    /// the current semantics, <tt>false</tt> otherwise.
    virtual bool isEventCall(FunctionCall *call) = 0;

    /// @}

    /// @brief Returns whether native semantics are being used.
    /// @return true if native semantics are used, false otherwise.
    bool useNativeSemantics() const;

    /// @brief Sets whether to use native semantics.
    /// @param b true to use native semantics, false otherwise.
    void setUseNativeSemantics(const bool b);

    /// @brief Creates a HIF-compatible name from the given name.
    /// @param reqName The requested name.
    /// @return The HIF-compatible name.
    std::string makeHifName(const std::string &reqName) const;

protected:
    ILanguageSemantics();

    /// @brief Factory.
    hif::HifFactory _factory;

    /// @brief Factory with semantics HIF (used to build standard hif library)
    hif::HifFactory _hifFactory;

    /// @brief Reference to name table.
    hif::NameTable *_nameT;

    /// @brief The semantics checks mode.
    bool _strictChecking;

    /// @brief Forbidden names.
    std::set<std::string> _forbiddenNames;

    /// @brief The semantic check options.
    SemanticOptions _semanticOptions;

    /// @name Standard packages
    /// @{

    /// @brief Wrapper for a string with possibility to add a prefix "hif_"
    /// @param reqName The requested name.
    /// @param hifFormat If true, add "hif_" prefix.
    /// @return The name with optional prefix.
    std::string _makeHifName(const std::string &reqName, const bool hifFormat) const;

    /// @brief Wrapper for Enum creation with possibility to add a prefix "hif_".
    /// @param enumName The enum name.
    /// @param values The enum values.
    /// @param size The number of values.
    /// @param hifFormat If true, add "hif_" prefix.
    /// @return The created TypeDef.
    TypeDef *_makeEnum(const char *enumName, const char *values[], size_t size, const bool hifFormat);

    /// @brief Create a SubProgram with at most one parameter.
    /// @param n The name.
    /// @param retType The return type.
    /// @param paramType The parameter type.
    /// @param paramValue The parameter value.
    /// @param unsupported Whether unsupported.
    /// @param hifFormat Whether to use HIF format.
    /// @return The created SubProgram.
    SubProgram *_makeAttribute(
        const std::string &n,
        Type *retType,
        Type *paramType,
        Value *paramValue,
        const bool unsupported,
        const bool hifFormat);

    /// @brief Create a SubProgram with two parameters.
    /// @param n The name.
    /// @param retType The return type.
    /// @param param1Type The first parameter type.
    /// @param param1Value The first parameter value.
    /// @param param2Type The second parameter type.
    /// @param param2Value The second parameter value.
    /// @param unsupported Whether unsupported.
    /// @param hifFormat Whether to use HIF format.
    /// @return The created SubProgram.
    SubProgram *_makeBinaryAttribute(
        const std::string &n,
        Type *retType,
        Type *param1Type,
        Value *param1Value,
        Type *param2Type,
        Value *param2Value,
        const bool unsupported,
        const bool hifFormat);

    /// @brief Create a SubProgram with three parameters.
    /// @param n The name.
    /// @param retType The return type.
    /// @param param1Type The first parameter type.
    /// @param param1Value The first parameter value.
    /// @param param2Type The second parameter type.
    /// @param param2Value The second parameter value.
    /// @param param3Type The third parameter type.
    /// @param param3Value The third parameter value.
    /// @param unsupported Whether unsupported.
    /// @param hifFormat Whether to use HIF format.
    /// @return The created SubProgram.
    SubProgram *_makeTernaryAttribute(
        const std::string &n,
        Type *retType,
        Type *param1Type,
        Value *param1Value,
        Type *param2Type,
        Value *param2Value,
        Type *param3Type,
        Value *param3Value,
        const bool unsupported,
        const bool hifFormat);

    /// @brief Create a SubProgram parameter.
    /// @param scope The scope.
    /// @param paramType The parameter type.
    /// @param paramValue The parameter value.
    /// @param paramIndex The parameter index.
    /// @param hifFormat Whether to use HIF format.
    void _makeAttributeParameter(
        SubProgram *scope,
        Type *paramType,
        Value *paramValue,
        const std::string &paramIndex,
        const bool hifFormat);
    /// @brief Adds a multiparameter function.
    /// @param ld The library definition.
    /// @param name The function name.
    /// @param factory The factory.
    /// @param hifFormat Whether to use HIF format.
    /// @param ret The return type.
    void
    _addMultiparamFunction(LibraryDef *ld, const char *name, hif::HifFactory &factory, const bool hifFormat, Type *ret);

    /// @brief Make an array of type @p t with span <tt>left downto right</tt>.
    /// @param index The suffix for left and right.
    /// @param t The array type.
    /// @return The built array.
    Array *_makeTemplateArray(const std::string &index, Type *t);

    /// @brief Check whether the given name is 'hif_' prefixed.
    /// @param n The name to check.
    /// @param unprefixed The unprefixed name output.
    /// @return true if prefixed.
    bool _isHifPrefixed(const std::string &n, std::string &unprefixed);

    /// @brief Create a StandardSymbols key.
    /// @param library The library name.
    /// @param symbol The symbol name.
    /// @return The key symbol.
    ILanguageSemantics::KeySymbol _makeKey(const char *library, const char *symbol);

    /// @brief Create a StandardSymbols value.
    /// @param library The library array.
    /// @param size The size of the library array.
    /// @param symbol The symbol name.
    /// @param action The map action.
    /// @return The value symbol.
    ILanguageSemantics::ValueSymbol
    _makeValue(const char *library[], unsigned int size, const char *symbol, ILanguageSemantics::MapCases action);

    /// @name Methods used by canRemoveCastOnOperands.
    /// @{

    /// @brief Checks concatenation casts for removal.
    /// @param e The expression.
    /// @param castT1 Cast type 1.
    /// @param castT2 Cast type 2.
    /// @param subT1 Sub type 1.
    /// @param subT2 Sub type 2.
    /// @param exprInfo Expression info.
    /// @param info Info.
    /// @return true if casts can be removed.
    virtual bool _checkConcatCasts(
        Expression *e,
        Type *castT1,
        Type *castT2,
        Type *subT1,
        Type *subT2,
        ExpressionTypeInfo &exprInfo,
        ExpressionTypeInfo &info);

    /// @brief Checks shift casts for removal.
    /// @param e The expression.
    /// @param castT1 Cast type 1.
    /// @param castT2 Cast type 2.
    /// @param subT1 Sub type 1.
    /// @param subT2 Sub type 2.
    /// @param exprInfo Expression info.
    /// @param info Info.
    /// @param canRemoveOnShift Whether shift cast can be removed.
    /// @return true if casts can be removed.
    virtual bool _checkShiftCasts(
        Expression *e,
        Type *castT1,
        Type *castT2,
        Type *subT1,
        Type *subT2,
        ExpressionTypeInfo &exprInfo,
        ExpressionTypeInfo &info,
        bool &canRemoveOnShift);

    /// @brief Checks arithmetic casts for removal.
    /// @param e The expression.
    /// @param castT1 Cast type 1.
    /// @param castT2 Cast type 2.
    /// @param subT1 Sub type 1.
    /// @param subT2 Sub type 2.
    /// @param origInfo Original info.
    /// @param simplifiedInfo Simplified info.
    /// @return true if casts can be removed.
    virtual bool _checkArithmeticCasts(
        Expression *e,
        Type *castT1,
        Type *castT2,
        Type *subT1,
        Type *subT2,
        ExpressionTypeInfo &origInfo,
        ExpressionTypeInfo &simplifiedInfo);

    /// @brief Checks relational casts for removal.
    /// @param e The expression.
    /// @param castT1 Cast type 1.
    /// @param castT2 Cast type 2.
    /// @param subT1 Sub type 1.
    /// @param subT2 Sub type 2.
    /// @param origInfo Original info.
    /// @param simplifiedInfo Simplified info.
    /// @return true if casts can be removed.
    virtual bool _checkRelationalCasts(
        Expression *e,
        Type *castT1,
        Type *castT2,
        Type *subT1,
        Type *subT2,
        ExpressionTypeInfo &origInfo,
        ExpressionTypeInfo &simplifiedInfo);

    /// @brief Checks bitwise casts for removal.
    /// @param e The expression.
    /// @param castT1 Cast type 1.
    /// @param castT2 Cast type 2.
    /// @param subT1 Sub type 1.
    /// @param subT2 Sub type 2.
    /// @param origInfo Original info.
    /// @param simplifiedInfo Simplified info.
    /// @return true if casts can be removed.
    virtual bool _checkBitwiseCasts(
        Expression *e,
        Type *castT1,
        Type *castT2,
        Type *subT1,
        Type *subT2,
        ExpressionTypeInfo &origInfo,
        ExpressionTypeInfo &simplifiedInfo);

    /// @brief Checks generic casts for removal.
    /// @param e The expression.
    /// @param castT1 Cast type 1.
    /// @param castT2 Cast type 2.
    /// @param subT1 Sub type 1.
    /// @param subT2 Sub type 2.
    /// @param origInfo Original info.
    /// @param simplifiedInfo Simplified info.
    /// @param precOpt Precision options.
    /// @param retOpt Return options.
    /// @return true if casts can be removed.
    virtual bool _checkGenericCasts(
        Expression *e,
        Type *castT1,
        Type *castT2,
        Type *subT1,
        Type *subT2,
        ExpressionTypeInfo &origInfo,
        ExpressionTypeInfo &simplifiedInfo,
        const hif::EqualsOptions &precOpt,
        const hif::EqualsOptions &retOpt);

    /// @}

    /// @brief Map of standard symbols for this semantics.
    StandardSymbols _standardSymbols;

    /// @brief Map for standard library file names.
    StandardLibraryFiles _standardFilenames;

    /// @brief Forces to allow only native c++ types
    /// e.g. bitvectors are not allowed
    bool _useNativeSemantics;

private:
    ILanguageSemantics(const ILanguageSemantics &);
    ILanguageSemantics &operator=(const ILanguageSemantics &);
};

} // namespace semantics
} // namespace hif

/// @brief Outputs the ILanguageSemantics object to an ostream.
/// @param o The output stream.
/// @param s The ILanguageSemantics object to output.
/// @return The output stream.
std::ostream &operator<<(std::ostream &o, const hif::semantics::ILanguageSemantics &s);
