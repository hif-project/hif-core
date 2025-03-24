/// @file simplify.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <memory>

/////////////////////////////////////////
// HIF library includes
/////////////////////////////////////////

#include "hif/BiVisitor.hpp"
#include "hif/manipulation/simplify.hpp"
#include "hif/manipulation/simplifyExpression.hpp"
#include "hif/semantics/standardization.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#    pragma clang diagnostic ignored "-Wunused-template"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#    pragma GCC diagnostic ignored "-Wunsafe-loop-optimizations"
#    if __GNUC__ >= 5
#        pragma GCC diagnostic ignored "-Wduplicated-cond"
#        pragma GCC diagnostic ignored "-Wnull-dereference"
#    endif
#endif

/////////////////////////////////////////
// Debug macros
/////////////////////////////////////////

#ifndef NDEBUG
//#define DEBUG_EXPR_SCREEN
#    ifdef DEBUG_EXPR_SCREEN
long long unsigned int __expressionId = 0ULL;
#    endif
#endif

/////////////////////////////////////////
// Configuration macros
/////////////////////////////////////////
// Upper bound to choose whether simplify or not for statements.
#define FOR_UNROLL_THRESHOLD      1000
// Upper bound to choose whether unroll aggregate alts.
#define AGGREGATE_UROLL_THRESHOLD 128

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

#if (defined _MSC_VER)
double log2(double d) { return log(d) / log(2.0); }
#else
#    pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

// ////////////////////////////////////////////////////////////////////
// Simplify visitor.
// ////////////////////////////////////////////////////////////////////
class SimplifyVisitor : public GuideVisitor
{
public:
    SimplifyVisitor(const SimplifyOptions &opt, hif::semantics::ILanguageSemantics *sem);
    ~SimplifyVisitor();

    virtual int visitAggregate(Aggregate &o);
    virtual int visitAggregateAlt(AggregateAlt &o);
    virtual int visitArray(Array &o);
    virtual int visitAssign(Assign &o);
    virtual int visitBitValue(BitValue &o);
    virtual int visitBitvectorValue(BitvectorValue &o);
    virtual int visitBoolValue(BoolValue &o);
    virtual int visitBreak(Break &o);
    virtual int visitCast(Cast &o);
    virtual int visitCharValue(CharValue &o);
    virtual int visitConst(Const &o);
    virtual int visitContinue(Continue &o);
    virtual int visitEnumValue(EnumValue &o);
    virtual int visitExpression(Expression &o);
    virtual int visitFieldReference(FieldReference &o);
    virtual int visitFile(File &o);
    virtual int visitFor(For &o);
    virtual int visitForGenerate(ForGenerate &o);
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitIdentifier(Identifier &o);
    virtual int visitIf(If &o);
    virtual int visitIfAlt(IfAlt &o);
    virtual int visitIfGenerate(IfGenerate &o);
    virtual int visitInstance(Instance &o);
    virtual int visitIntValue(IntValue &o);
    virtual int visitLibrary(Library &);
    virtual int visitMember(Member &o);
    virtual int visitParameter(Parameter &o);
    virtual int visitParameterAssign(ParameterAssign &);
    virtual int visitPointer(Pointer &o);
    virtual int visitPort(Port &o);
    virtual int visitPortAssign(PortAssign &);
    virtual int visitProcedureCall(ProcedureCall &);
    virtual int visitRange(Range &o);
    virtual int visitRealValue(RealValue &o);
    virtual int visitRecord(Record &o);
    virtual int visitReference(Reference &o);
    virtual int visitReturn(Return &o);
    virtual int visitSignal(Signal &o);
    virtual int visitSlice(Slice &o);
    virtual int visitSwitch(Switch &o);
    virtual int visitString(String &o);
    virtual int visitStringValue(StringValue &o);
    virtual int visitTypeReference(TypeReference &);
    virtual int visitTypeTPAssign(TypeTPAssign &);
    virtual int visitValueStatement(ValueStatement &o);
    virtual int visitValueTP(ValueTP &o);
    virtual int visitValueTPAssign(ValueTPAssign &);
    virtual int visitVariable(Variable &o);
    virtual int visitViewReference(ViewReference &);
    virtual int visitWhen(When &o);
    virtual int visitWhenAlt(WhenAlt &o);
    virtual int visitWhile(While &o);
    virtual int visitWith(With &o);

    /// @brief Cleaning objects in BLists to avoid double delete.
    virtual int AfterVisit(Object &o);

    /// @brief Return the result.
    /// @return The result.
    ///
    Object *getResult();

    /// @brief Set the initial result.
    /// @param o The object to set to current result.
    ///
    void setResult(Object *o);

private:
    /// @brief Store the options given to simplify
    SimplifyOptions _opt;

    /// @brief Store the options given to sort method.
    SortOptions _sortOpt;

    /// @brief Store the semantics given to simplify
    hif::semantics::ILanguageSemantics *_sem;

    /// @brief The factory.
    HifFactory _factory;

    /// @brief store the result of simplify algorithm
    Object *_result;

    /// @brief Collects objects to be deleted, but which belongs to a BList.
    Trash _trash;

    typedef std::set<Declaration *> SelfSet;
    SelfSet _selfSet;

    typedef std::set<std::string> StringSet;
    StringSet _expressionKeys;

    // disabled
    SimplifyVisitor(const SimplifyVisitor &);
    SimplifyVisitor &operator=(const SimplifyVisitor &);

    /// Check the value can be considered a boolean.
    bool _checkBooleanValue(Value *v);

    /// Gets the value of the boolean value.
    bool _getBooleanValue(Value *v);

    /// @brief Resize given string to given size.
    void _resize(std::string &s, const char c, std::size_t size);

    /// @brief Try to transform the given value to an int value. This
    /// function is useful to get the index of some Hif object e.g. hif::Member
    /// @param v The value that represent the index.
    /// @return The int value that represent the index if possible,
    /// nullptr otherwise.
    IntValue *_getIndex(Value *v);

    /// @brief Checks that given object is not creating a self loop to assignment.
    /// E.g. vtpa v = v +1
    /// @param o The reference to be checked.
    /// @param decl The reference declaration.
    /// @return True if a self loop is detected.
    bool _isSelfAssignment(Object *o, DataDeclaration *decl);

    /// @brief Checks that given object is not creating a self loop initialization.
    /// E.g. vtp v = v +1
    /// @param o The reference to be checked.
    /// @param decl The reference declaration.
    /// @return True if a self loop is detected.
    bool _isSelfInitialization(Object *o, DataDeclaration *decl);
    /// @brief Overloaded of _isSelfInitialization() for TypeTPs.
    bool _isSelfInitialization(Object *o, TypeTP *decl);

    /// @brief Eventually add a cast to the value if its type cannon be
    /// safely substituted w.r.t. the given type.
    /// @param v The value
    /// @param t The type
    Value *_makeSubstituible(Value *v, Type *t);

    /// @brief Simplify field refrence if the prefix is a record value.
    /// @param o The FieldReference
    bool _simplifyRecordValueFieldReference(FieldReference *o);

    bool _pushFieldReference(FieldReference *o);

    /// @brief Removes potential useless cast on return value if any.
    bool _simplifyReturnUselessCasts(Return *o);

    /// @brief Removes potential casts on value statement.
    bool _simplifyValueStatementUselessCasts(ValueStatement *o);

    /// @brief Removes while if condition if false and is safe.
    bool _simplifyUselessWhile(While *o);

    /// @brief Handle simplification of template parameters assigns
    bool _simplifyTemplateParametersAssigns(FunctionCall *fc);
    /// @brief Transform function calls in constvalue if possible
    bool _simplifyFunctionCalls(FunctionCall *o);
    /// @brief Tries to unroll iterated concats
    bool _simplifyIteratedConcat(FunctionCall *o);
    /// @name For simplifications management
    /// @{

    template <typename T> bool _simplifyUselessFor(T *o);
    bool _simplifyConstantLoopFor(For *o);

    /// @}

    /// @name Member simplifications management
    /// @{

    bool _simplifySingleBitMember(Member *o);
    bool _simplifyBitvectorValueMember(Member *o);
    bool _simplifyAggregateMember(Member *o);
    bool _simplifySliceMember(Member *o);
    bool _simplifyBitwiseExpressionMember(Member *o);
    bool _simplifyConcatMember(Member *o);
    bool _simplifyUnrollAggregate(Member *o);
    bool _simplifyMemberInternalCast(Member *o);

    /// @}

    /// @name Simplify names methods
    /// @{

    /// @brief If option simplify_constants is enable, this function simplify
    /// the names replacing them with the declaration initial value.
    /// The same with template parameters and option simplify_template_parameters.
    /// @param o The object to simplify.
    /// @return True if the object has been simplified.
    bool _simplifyNames(Object *o);
    bool _simplifyNamesOfConstants(Object *o, Declaration *decl);
    bool _simplifyNamesOfTemplateParameters(Object *o, Declaration *decl);
    bool _simplifyNamesOfParameters(Object *o, Declaration *decl);
    bool _simplifyNamesOfTypeReferences(Object *o, Declaration *decl);

    /// @}

    /// @name Perform the replace operation with some checks, storing also
    /// the new _result of simplify algorithm.
    /// @{

    bool _replace(Object *from, Object *&to);

    template <class T1, class T2> bool _replace(T1 *from, T2 *&to);

    template <class T> bool _replace(T *from, BList<T> &to);

    void _replaceWithList(Object *from, BList<Object> &to);

    /// @}

    /// @name Expression management methods
    /// @{

    /// @brief Return true if the passed operator is commutative.
    /// NOTE: handle only operators used by balance expression function.
    /// @param o The input operator.
    /// @return true if the given operator is commutative.
    bool _isAllowedCommutative(Operator o);

    /// @brief Return the inverse of the given operator.
    /// NOTE: handle only operators used by balance expression function.
    /// @param o The input operator.
    /// @return The inverse of input operator.
    Operator _getAllowedInverseOp(Operator o);

    /// @brief This function tries to rebalance the tree of the given expression
    /// to simplify eventual const values.
    /// @param e The original expression.
    /// @return The simplified expression or nullptr.
    Expression *_getRebalancedExpressions(Expression *e);

    /// @brief In case of nested expression consisting in multiplication for a constant,
    /// checks if it is possible to simplify two equal sub-trees.
    /// E.g. [[v *2] - v] --> [v * [2-1]]
    bool _simplifyMultiplication(Expression *e);

    /// @brief If operands have bitwidth 1 we can map aritmetic operators
    /// in logic and bitwise
    bool _simplifyArithBitOperation(Expression *e);

    /// @brief If the sub-expression owns the same operator of the current one,
    /// simplify them and swap operands of sub-expression.
    /// E.g. of considered cases:
    /// Unary:
    /// (A + (+ B)) -> (A + B)
    /// (A + (- B)) -> (A - B)
    /// (A - (- B)) -> (A + B)
    /// Binary:
    /// (A - (B - C)) -> (A + (C - B))
    /// (A / (B / C)) -> (A * (C / B))
    /// @return True If the manipulation succeeded.
    bool _pushDownOperators(Expression *o);

    /// @brief If the sub-expression has a constants try to move it upper
    /// in the tree.
    /// @return True If the manipulation succeeded.
    bool _pushUpConstants(Expression *o);

    /// @brief Given the expression, it tries to swap nested branches.
    /// E.g. (a + b) - a => (a - a) + b
    ///
    /// @return True If the manipulation succeeded.
    bool _sortBranches(Expression *o);

    /// @brief Try to put the Expression in a form suitable for method
    /// _getRebalancedExpressions.
    /// @return True If the manipulation succeeded.
    bool _linearizeExpressionTree(Expression *o);

    /// @brief Returns true if the key of given expression in already present
    /// into the key list.
    bool _isAlreadySimplified(Expression *e);

    /// @brief Adds the given key to the expression key list.
    void _addExpressionKey(const std::string &key);

    /// @brief Removes the given key to the expression key list.
    void _removeExpressionKey(const std::string &key);

    bool _doSimplifyExpression(Expression *o);

    /// @brief Simplify expression with op_none
    bool _simplifyOpNone(Expression *o);

    /// @brief Try to (recursively) remove cast from expression operands
    /// if this does not compromise the expression operation precision
    /// and the returned type.
    /// @return True if is simplified
    bool _simplifyOperandCasts(Expression *o);

    /// @brief Try to collapse branches with two whens
    bool _simplifyDoubleWhen(Expression *o);

    /// @brief Try to simplify bitwise expressions when constvalues are involved
    bool _simplifyBitwiseConstants(Expression *o);

    /// @brief Check if the given expression tree is one of the researched type.
    /// In this case set the others parameters and return true.
    /// The researched cases are:
    /// case1) expr(op1)[ expr(op2)[ cv2, v ], cv1 ]
    /// case2) expr(op1)[ expr(op2)[ v, cv2 ], cv1 ]
    /// case3) expr(op1)[ cv1, expr(op2)[ cv2, v ] ]
    /// case4) expr(op1)[ cv1, expr(op2)[ v, cv2 ] ]
    ///
    /// @param e The input expression.
    /// @param v The only value present in expression tree, that can be present
    /// only in the sub expression.
    /// @param cv1 The most external const value, that is the const value
    /// child of input expression.
    /// @param cv2 The internal const value, that is the const value present
    /// in the sub expression.
    /// @param oper1 The operator of most external expression.
    /// @param oper2 The operator of sub expression.
    /// @param is_left1 True if the value <tt>v</tt> is on the left side (Op1)
    /// of the most external expression.
    /// @param is_left2 True if the value <tt>v</tt> is on the left side (Op1)
    /// of the sub expression.
    /// @return True if the tree matched with one of researched cases.
    ///
    bool _getNestedExpressionOperands(
        Expression *e,
        Value *&v,
        Value *&cv1,
        Value *&cv2,
        Operator &oper1,
        Operator &oper2,
        bool &is_left1,
        bool &is_left2);

    /// @brief Support method to retrive the information about given general expression.
    bool _getNestedConstExpressionOperands(
        Expression *e,
        Value *&v,
        Value *&cv1,
        Value *&cv2,
        Operator &oper1,
        Operator &oper2,
        bool &is_left1,
        bool &is_left2);

    /// @brief Support method to retrive the information about given expression
    /// checking if it has equals subtrees.
    bool _getNestedEqualsSubtreesOperands(
        Expression *e,
        Value *&v,
        Value *&cv1,
        Value *&cv2,
        Operator &oper1,
        Operator &oper2,
        bool &is_left1,
        bool &is_left2);

    /// @brief Support method to retrive the information about given expression
    /// getting nested single constant.
    bool _getNestedSingleConstantOperands(
        Expression *e,
        Value *&v,
        Value *&cv1,
        Value *&cv2,
        Operator &oper1,
        Operator &oper2,
        bool &is_left1,
        bool &is_left2);

    /// @}

    /// @brief If option is enable, the declaration member is simplified.
    /// This is done because guide visitor does not go into this child node.
    /// @param o The Object to manage.
    template <typename T> void _simplifyDeclaration(T *o);

    /// @name If option is enable, the semantics type member is simplified.
    /// This is done because guide visitor does not go into this child node.
    /// @param o The Object to manage.
    /// @{

    template <typename T> void _simplifySemanticsType(T *o);

    template <typename T> void _simplifyBaseType(T *o);

    /// @}

    /// @brief In case of negative Int/RealValue, tries to push the minus
    /// inside value of the object.
    template <typename T> bool _fixNegativeValue(T *o);

    /// @brief Eventually remove the syntactic type.
    /// @param o The const value to check.
    void _fixSyntacticType(ConstValue *o);

    /// @brief Checks assignability since in case of generates port binding value
    /// type could have changed.
    void _fixPortAssignAssignability(PortAssign *o);

    /// @brief Fix downto span removing it since is the policy choose in HIF.
    void _fixStringSpanInformation(String *o);

    /// @name Cast-related functions.
    /// Function to remove useless casts or manage particular casts of
    /// objects like aggregate, switch alts etc.
    /// @{

    /// @brief Move potential casts from target to source of assign,
    bool _fixRightHandSideCasts(Assign *o);

    /// @brief Tries to remove source casts if this operation preserves
    /// the correctness.
    bool _simplifyLeftHandSideCasts(
        Type *targetType,
        Value *source,
        Object *src,
        const bool checkPrecision = false,
        const hif::Operator oper  = op_assign);

    bool _ensureAssignability(DataDeclaration *decl);
    bool _ensureAssignability(Assign *ass);

    /// @brief Remove assigns to slices having null ranges. Assumes no side effects on the right hand side
    bool _removeNullRangeAssign(Assign *o);
    /// @brief Remove nested cast only if is useless.
    /// the situation is this: T1(T2(T3))
    /// where Tx(Ty) represent a cast from type Ty to type Tx.
    /// The cast T2 () is useful only if T1 > T2 && T2 < T3 because it
    /// loose precision
    /// in the case that two precisions are not comparable, we safely
    /// maintain the internal cast.
    /// @return true if operation is done.
    bool _removeUselessNestedCast(Cast *c);

    /// @brief Remove useless cast from T to T by using hif::equals.
    /// @param c The cast.
    /// @return true if operation is done.
    bool _removeSameTypeCast(Cast *c);

    /// @brief If op is a const value, try to call transform constant.
    /// @param c The cast.
    /// @return true if operation is done.
    bool _transformCastOfConstValue(Cast *c);

    /// @brief Checks whether removing the cast on constant is safe wrt
    /// the current semantics.
    /// Note, this involves parent which use constant without syntactic type.
    bool _canTransformConstantInBound(Cast *o);

    /// @brief If there is a cast of an aggregate (that must be of array type),
    /// this function pushes the cast to the aggregate elements.
    /// @param c The cast.
    /// @return true if operation is done.
    bool _explicitAggregateCast(Cast *c);

    /// @brief If there is a Cast to vector types involving a bit/bool, build the
    /// correspondent Aggregate.
    bool _transformCastToAggregate(Cast *c);

    /// @brief If there is a Cast to Bitvector type involving an Aggregate, build the
    /// correspondent BitvectorValue.
    bool _transformCastFromAggregateToBitvector(Cast *c);

    /// @brief If cast is to bitvector and value is concato of bit-arrays,
    /// push casts to concat operands, to try to further simplify the tree.
    bool _transformCastFromBitArrayConcat(Cast *c);

    /// @brief Transform an assign from array of bool to int into an equivalent expression
    bool _transformAssignFromArrayOfBoolToInt(Assign *o);

    /// @brief Transform a cast from array of bool to int into an equivalent expression
    bool _transformCastFromArrayOfBoolToInt(Cast *o);

    /// @brief Basic function to handle tranformation from array of bool to int
    bool
    _transformCastFromArrayOfBoolToInt(Value *toReplace, Value *internalExpr, Type *externalType, const bool recall);

    /// @brief If we want to cast an expression which is a multiplication with precision
    /// greater than 64 bits to a type that is less or equals 64 bits we try to simplify the
    /// expression
    bool _simplifyCastOfVectorMultiplication(Cast *c);

    /// @brief If there is a cast of a record value (that must be of record type),
    /// this function pushes the cast to the alt elements.
    /// @param c The cast.
    /// @return true if operation is done.
    bool _explicitRecordCast(Cast *c);

    /// @brief If there is a cast of a concat of values and the cast involves only a subset of those values
    /// we could simplify the cast value.
    /// @param c The cast..
    /// @return true if operation is done.
    bool _transformCastOfConcat(Cast *c);

    /// @brief If there is a cast from bitvector to array we replace it with the aggregate
    /// @param c The cast.
    /// @return true if operation is done.
    bool _fixCastFromBitvectorToArray(Cast *c);

    /// @brief Remove eventual useless cast added to the switch alt value(s)
    /// checking operation op_eq between them and switch value.
    /// @param o The switch.
    void _removeSwitchUselessCasts(Switch *o);

    bool _simplifyEmptyWith(With *o);

    /// @brief Remove eventual useless cast added to the with alt value(s)
    /// checking operation op_eq between them and switch expr.
    /// @param o The with.
    void _removeWithUselessCasts(With *o);

    /// @brief Support function for:
    /// _removeSwitchUselessCasts
    /// _removeWithUselessCasts.
    void _fixUselessConditionCast(BList<Value> &conditions, Type *refType);

    ///@}

    /// @name Bounds/Indexes management
    /// @{

    /// @brief Transform a null range into canonical form -1 downto 0.
    /// @param range The range to transform.
    void _fixNullRange(Range *range);

    /// @brief Checks whether the given range is a null range
    bool _isNullRange(Range *range);
    /// @brief Ensures that bound type is allowed according with current semantics.
    /// @param v The bound to be checked.
    void _fixBoundType(Value *v);

    /// @brief Check if bound can has not got the syntactical type.
    /// This is possible only if the corresponding type of the constant in
    /// given semantics is the same of bound type. If not this method create
    /// and add a cast of the bound to the mapped type of the bound type.
    /// @param bound The bound.
    void _handleBound(Value *bound);

    /// @}

    /// @name Generate-related functions.
    /// @{

    /// @brief Access point to expand for generate.
    bool _simplifyForGenerate(ForGenerate *o);

    /// @brief Access point to expand if generate.
    bool _simplifyIfGenerate(IfGenerate *o);

    /// @brief Check a condition, trying to resolve it as boolean const value.
    /// Note: must be called after GuideVisitor.
    /// @return
    ///     -1 if condition cannot be resolved as boolean value
    ///     0 if it can be resolved and it is evaluated as false
    ///     1 if it can be resolved and it is evaluated as true
    int _resolveBoolCondition(Value *condition);

    /// @brief Removes eventual useless casts of given condition.
    /// @param condition The condition
    /// @param o The object having the condition
    void _simplifyCondition(Value *condition, Object *o);

    /// @brief Determines how many iterations are performed by a for statement
    /// (which may be a For or ForGenerate).
    ///
    /// By now works for simple for-statements, e.g.:
    /// for (int i = 0; i < 32; ++i)
    ///
    /// Note: must be called after GuideVisitor.
    ///
    /// @param initDeclarations Refers to the same name field in For or ForGenerate.
    /// @param initValues Refers to the same name field in For or ForGenerate.
    /// @param condition Refers to the same name field in For or ForGenerate.
    /// @param steps Refers to the same name field in For or ForGenerate.
    /// @param loops The reference to the variable storing the resulting
    /// number of iterations.
    /// @param min The starting value.
    /// @param max The final value.
    /// @return true if has been able to calculate the number of iterations,
    /// false otherwise.
    ///
    bool _resolveForLoopBound(
        BList<DataDeclaration> &initDeclarations,
        BList<Action> &initValues,
        Value *condition,
        BList<Action> &steps,
        long long &loops,
        long long &min,
        long long &max);

    /// @brief Determines how many iterations are performed by a for statement
    /// (which may be a For or ForGenerate).
    /// Special case for Range condition.
    bool _resolveForLoopBound_rangeCase(Value *condition, long long &loops, long long &min, long long &max);

    /// @brief Determines the end value of a for statement (which may be
    /// a For or ForGenerate).
    Value *_resolveForLoopBound_getEndValue(Value *condition, const std::string &indexName, Expression *exprCond);

    /// @brief Determines the number of steps performed by a for statement
    /// (which may be a For or ForGenerate).
    Value *_resolveForLoopBound_getSteps(
        Value *condition,
        BList<Action> &steps,
        const std::string &indexName,
        Expression *exprCond);

    /// @brief Determines the number of iterations performed by a for statement
    /// (which may be a For or ForGenerate) basing on the extracted infos.
    bool _resolveForLoopBound_calculateIterations(
        long long &loops,
        long long &min,
        long long &max,
        Value *initVal,
        Expression *exprCond,
        Value *lastVal,
        Value *increment);

    bool _isLoopWithControlActions(Object *loop);

    /// @brief Declarations inside a Generate may shadow external declarations
    /// with the same name. A Generate may create more blocks, each one
    /// with its copy of the same declaration which is visible only inside
    /// that block.
    /// Main idea: expand the Generate in the parent object first (involving
    /// declarations, instances, stateTables, global actions), and then
    /// proceed with the visit of its inner generates.
    ///
    /// @param suffix is used by ForGenerate, passing a suffix that correspond
    ///     to a loop iteration, while IfGenerate just need fresh names to
    ///     avoid collision with other declarations.
    ///
    void _manageGenerateLocalStuff(Generate *o, std::string suffix, BaseContents *newLocation);

    ///@}

    /// @name Slice-related methods.
    /// @{

    /// @brief If the Slice bounds are equals, transform it to a Member.
    bool _simplifySliceToMember(Slice *o);

    /// @brief If the prefix is a constant and the range is a constant,
    /// calculates the resulting constant value.
    bool _simplifySliceConstantPrefix(Slice *o);

    /// @brief If the slice has the same span of the prefix type, removes it.
    bool _simplifyUselessSlice(Slice *o);

    /// @brief Simplify slices on ints.
    bool _simplifyIntSlice(Slice *o);

    /// @brief Pushes the slice inside every element of a concat
    bool _simplifyConstantConcatSlice(Slice *o);
    bool _simplifyNonConstantConcatSlice(Slice *o);
    void _getConcatElements(std::vector<Value *> &elements, Expression *expr);

    /// @brief Pushes the slice inside every element of an expression
    ///  with op_bor or op_band
    bool _simplifyBitwiseExpressionSlice(Slice *o);

    /// @brief Pushes the slice inside every element of an expression
    ///  with arithmentic operator
    bool _simplifyArithExpressionSlice(Slice *o);

    /// @brief Pushes the slice into the prefix cast
    bool _pushSliceIntoCast(Slice *o);
    bool _pushSliceIntoWhen(Slice *o);
    bool _pushSliceIntoWith(Slice *o);

    /// @brief Simplify slice with inside a cast
    bool _transformSliceToCast(Slice *o);
    /// @}

    /// @name Aggregate-related methods.
    /// @{

    /// @brief If the Aggregate type is Bit, and only others field is set,
    /// replace the Aggregate with others field value.
    bool _simplifyBitAggregate(Aggregate *obj);
    /// @brief Convert Aggregate to a BitvectorValue.
    bool _simplifyBitvectorAggregate(Aggregate *obj, Type *t);
    /// @brief Convert Aggregate to a String or a Char
    bool _simplifyStringAggregate(Aggregate *obj, Type *t);
    /// @brief Tries to remove others field.
    bool _simplifyUnrollingAggregate(Aggregate *obj, Type *t);
    /// @brief Tries to removes alts equals to others or remove all alts
    /// creating others in case of all equals alts.
    bool _simplifyRollingAggregate(Aggregate *obj, Type *t);
    /// @brief Transform an aggregate made of member of same value to a slice
    bool _simplifyAggregateWithSameAlts(Aggregate *obj, Type *t);
    /// @brief If Aggregate is an expression of single bit for minbound index
    /// and zeros for other bits is replaced with a cast
    bool _simplifySigleBitExpressionAggregate(Aggregate *obj, Type *t);
    /// @brief Replace fixed width vector aggregate with a concat.
    bool _simplifyAggregateToConcat(Aggregate *obj, Type *t);
    ///@}

    /// @name Simplify referenced assign source mathods
    /// @{

    template <typename T> bool _simplifyReferencedAssignList(BList<T> &list);

    /// @}

    /// @name Simplify constant conditional constructs
    /// @{

    bool _simplifyConstantCondition(If *o);
    bool _simplifyConstantCondition(When *o);
    bool _simplifyConstantCondition(With *o);
    bool _simplifyConstantCondition(Switch *o);

    /// @}

    /// @name Simplify merging cases stuff
    /// @{

    bool _equalsAltsBody(IfAlt *refAlt, IfAlt *currentAlt);
    bool _equalsAltAndDefaultBody(IfAlt *refAlt, If *o);
    void _replaceCaseWithDefault(If *o);
    bool _equalsAltsBody(WhenAlt *refAlt, WhenAlt *currentAlt);
    bool _equalsAltAndDefaultBody(WhenAlt *refAlt, When *o);
    void _replaceCaseWithDefault(When *o);
    bool _equalsAltsBody(SwitchAlt *refAlt, SwitchAlt *currentAlt);
    bool _equalsAltAndDefaultBody(SwitchAlt *refAlt, Switch *o);
    void _replaceCaseWithDefault(Switch *o);
    bool _equalsAltsBody(WithAlt *refAlt, WithAlt *currentAlt);
    bool _equalsAltAndDefaultBody(WithAlt *refAlt, With *o);
    void _replaceCaseWithDefault(With *o);

    template <typename T> bool _simplifyMergingCases(T *o);

    bool _simplifyWhenBoolConstants(When *o);

    template <typename T> bool _simplifyUselessAlts(T *o);

    template <typename T> bool _simplifyMergingAlts(T *o);

    /// @}
};

SimplifyVisitor::SimplifyVisitor(const SimplifyOptions &opt, hif::semantics::ILanguageSemantics *sem)
    : _opt(opt)
    , _sortOpt()
    , _sem(sem)
    , _factory(sem)
    , _result(nullptr)
    , _trash()
    , _selfSet()
    , _expressionKeys()
{
    // ntd
}

SimplifyVisitor::~SimplifyVisitor() { _trash.clear(); }
// //////////////////////////
// public methods
// //////////////////////////

Object *SimplifyVisitor::getResult() { return _result; }

void SimplifyVisitor::setResult(Object *o) { _result = o; }

bool SimplifyVisitor::_checkBooleanValue(Value *v)
{
    BoolValue *boolVal = dynamic_cast<BoolValue *>(v);
    BitValue *bitVal   = dynamic_cast<BitValue *>(v);

    if (boolVal == nullptr && bitVal == nullptr)
        return false;

    if (boolVal != nullptr)
        return true;

    if (bitVal->getValue() == bit_h || bitVal->getValue() == bit_one || bitVal->getValue() == bit_l ||
        bitVal->getValue() == bit_zero)
        return true;

    return false;
}

bool SimplifyVisitor::_getBooleanValue(Value *v)
{
    BoolValue *boolVal = dynamic_cast<BoolValue *>(v);
    BitValue *bitVal   = dynamic_cast<BitValue *>(v);

    if (boolVal == nullptr && bitVal == nullptr)
        return false;

    if (boolVal != nullptr)
        return boolVal->getValue();

    if (bitVal->getValue() == bit_h || bitVal->getValue() == bit_one)
        return true;

    return false;
}
// //////////////////////////
// Utility methods
// //////////////////////////

void SimplifyVisitor::_resize(std::string &s, const char c, std::size_t size)
{
    if (s.size() >= size)
        return;
    std::string tmp;
    tmp.resize(size - s.size(), c);
    s += tmp;
}

IntValue *SimplifyVisitor::_getIndex(Value *v)
{
    ConstValue *index = dynamic_cast<ConstValue *>(v);
    if (index == nullptr)
        return nullptr;

    HifFactory f;
    // Here we just want to convert the representation from any kind of value
    // to an IntValue. Therefore, we MUST always use the HIF semantics,
    // since the actual semantics could not have a Int type (e.g. Verilog).
    // After conversion, we roll back the syntactic type to the original type.
    f.setSemantics(hif::semantics::HIFSemantics::getInstance());
    Type *myInt             = f.integer();
    ConstValue *transformed = transformConstant(index, myInt, _sem);
    delete myInt;
    delete transformed->setType(hif::copy(index->getType()));
    IntValue *iv = dynamic_cast<IntValue *>(transformed);
    return iv;
}

bool SimplifyVisitor::_isSelfAssignment(Object *o, DataDeclaration *decl)
{
    if (_selfSet.find(decl) != _selfSet.end())
        return true;
    ReferencedAssign *ref = hif::getNearestParent<ReferencedAssign>(o);
    if (ref == nullptr)
        return false;
    Declaration *ddref = hif::semantics::getDeclaration(ref, _sem);

    if (ddref == decl) {
        _selfSet.insert(decl);
        return true;
    }
    return false;
}

bool SimplifyVisitor::_isSelfInitialization(Object *o, DataDeclaration *decl)
{
    if (_selfSet.find(decl) != _selfSet.end())
        return true;
    DataDeclaration *ddref = hif::getNearestParent<DataDeclaration>(o);

    if (ddref == decl) {
        _selfSet.insert(decl);
        return true;
    }
    return false;
}
bool SimplifyVisitor::_isSelfInitialization(Object *o, TypeTP *decl)
{
    if (_selfSet.find(decl) != _selfSet.end())
        return true;
    TypeTP *ddref = hif::getNearestParent<TypeTP>(o);

    if (ddref == decl) {
        _selfSet.insert(decl);
        return true;
    }
    return false;
}

Value *SimplifyVisitor::_makeSubstituible(Value *v, Type *t)
{
    if (v == nullptr || t == nullptr)
        return hif::copy(v);

    Type *vt = hif::semantics::getSemanticType(v, _sem);
    if (vt == nullptr)
        return hif::copy(v);

    Type *bt = t;
    if ((dynamic_cast<TypeReference *>(vt) != nullptr) != (dynamic_cast<TypeReference *>(bt) != nullptr)) {
        vt = hif::semantics::getBaseType(vt, false, _sem);
        bt = hif::semantics::getBaseType(t, false, _sem);
    }

    hif::EqualsOptions opt;
    opt.checkFieldsInitialvalue = false;
    if (hif::equals(bt, vt, opt))
        return hif::copy(v);

    Cast *c = new Cast();
    c->setValue(hif::copy(v));
    c->setType(hif::copy(t));
    return c;
}

bool SimplifyVisitor::_simplifyRecordValueFieldReference(FieldReference *o)
{
    RecordValue *recVal = dynamic_cast<RecordValue *>(o->getPrefix());
    if (recVal == nullptr)
        return false;

    RecordValueAlt *agra                  = nullptr;
    FieldReference::DeclarationType *decl = hif::semantics::getDeclaration(o, _sem);

    Field *f = dynamic_cast<Field *>(decl);
    if (f != nullptr) {
        // Normal case
        Record *rec = dynamic_cast<Record *>(f->getParent());
        messageAssert(rec != nullptr, "Unexpcted field parent", f->getParent(), _sem);
        auto pos = rec->fields.getPosition(f);
        messageAssert(pos != rec->fields.size(), "Unmatched field position", f, _sem);

        agra = recVal->alts.at(pos);
    } else {
        // Case of simplified prefix to record value
        // Sanity check
        Type *prefixType = hif::semantics::getSemanticType(recVal, _sem);
        messageDebugAssert(prefixType != nullptr, "Cannot type record value", recVal, _sem);
        Type *prefixBase = hif::semantics::getBaseType(prefixType, false, _sem);
        Record *rec      = dynamic_cast<Record *>(prefixBase);
        messageAssert(rec != nullptr, "Cannot type or type is not a record", recVal, _sem);
        for (BList<RecordValueAlt>::iterator i = recVal->alts.begin(); i != recVal->alts.end(); ++i) {
            if ((*i)->getName() != o->getName())
                continue;
            agra = (*i);
            break;
        }
    }

    messageAssert(agra != nullptr, "Field not found", nullptr, _sem);

    Type *agraType = hif::objectGetType(decl);
    Value *res     = _makeSubstituible(agra->getValue(), agraType);
    _replace(o, res);
    res->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_pushFieldReference(FieldReference *o)
{
    Value *prefix = o->getPrefix();
    With *wi      = dynamic_cast<With *>(prefix);
    When *wh      = dynamic_cast<When *>(prefix);
    if (wi == nullptr && wh == nullptr)
        return false;

    if (wi != nullptr) {
        if (wi->getDefault() != nullptr) {
            Value *def = _factory.fieldRef(wi->getDefault(), o->getName());
            wi->setDefault(def);
        }
        for (BList<WithAlt>::iterator it = wi->alts.begin(); it != wi->alts.end(); ++it) {
            Value *wAltVal = (*it)->getValue();
            Value *v       = _factory.fieldRef(wAltVal, o->getName());
            (*it)->setValue(v);
        }
    } else if (wh != nullptr) {
        if (wh->getDefault() != nullptr) {
            Value *def = _factory.fieldRef(wh->getDefault(), o->getName());
            wh->setDefault(def);
        }
        for (BList<WhenAlt>::iterator it = wh->alts.begin(); it != wh->alts.end(); ++it) {
            Value *wAltVal = (*it)->getValue();
            Value *v       = _factory.fieldRef(wAltVal, o->getName());
            (*it)->setValue(v);
        }
    }

    hif::semantics::resetTypes(prefix, false);
    _replace(o, prefix);
    prefix->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifyReturnUselessCasts(Return *o)
{
    if (o->getValue() == nullptr)
        return false;

    Function *f = hif::getNearestParent<Function>(o);
    messageAssert(f != nullptr, "Cannot find parent function", o, _sem);

    Type *tt = f->getType();

    // NOTE: use assign operator since is implicitelly converted.
    // ref. design: vhdl/ips/mep_sp_ram
    return _simplifyLeftHandSideCasts(tt, o->getValue(), o);
}

bool SimplifyVisitor::_simplifyValueStatementUselessCasts(ValueStatement *o)
{
    Value *value = o->getValue();
    if (dynamic_cast<Cast *>(value) == nullptr)
        return false;

    Value *internalValue = hif::copy(hif::getChildSkippingCasts(value));
    delete o->setValue(internalValue);

    return true;
}

bool SimplifyVisitor::_simplifyUselessWhile(While *o)
{
    int boolCond = _resolveBoolCondition(o->getCondition());
    if (boolCond != 0)
        return false;

    // in case of while(false) simply remove it
    if (!o->isDoWhile()) {
        _trash.insert(o);
        return true;
    }
    if (_isLoopWithControlActions(o))
        return 0;

    // in case of do-while(false) the actions can be moved only if:
    // - there are not breaks without label
    // - there are not breaks with label that is the same of the while.
    hif::HifTypedQuery<Break> q;
    std::list<Break *> result;
    hif::search(result, o, q);

    bool canBeSimplified = true;
    for (std::list<Break *>::iterator i = result.begin(); i != result.end(); ++i) {
        Break *b = *i;
        if (b->getName() == NameTable::getInstance()->none() || b->getName() == o->getName()) {
            canBeSimplified = false;
            break;
        }
    }

    if (!canBeSimplified)
        return false;

    _replaceWithList(o, o->actions);
    return true;
}

bool SimplifyVisitor::_simplifyTemplateParametersAssigns(FunctionCall *fc)
{
    if (fc->templateParameterAssigns.empty())
        return false;

    // Skipping loop when getSemanticType requires to simplify the same scope.
    // E.g. class constructors with template parameters.
    FunctionCall::DeclarationType *f = hif::semantics::getDeclaration(fc, _sem);
    messageAssert(f != nullptr, "Declaration not found", fc, _sem);
    if (hif::isSubNode(fc->templateParameterAssigns.front(), f))
        return false;

    _simplifyReferencedAssignList(fc->templateParameterAssigns);
    //_simplifyReferencedAssignList(fc->parameterAssigns);
    return true;
}

bool SimplifyVisitor::_simplifyFunctionCalls(FunctionCall *o)
{
    if (!_opt.simplify_functioncalls)
        return false;
    Function *fun = hif::copy(hif::manipulation::instantiate(o, _sem));
    if (fun == nullptr)
        return false;
    const bool ok = hif::manipulation::sortParameters(
        o->parameterAssigns, fun->parameters, true, hif::manipulation::SortMissingKind::ALL, _sem);
    if (!ok) {
        delete fun;
        return false;
    }
    hif::manipulation::SimplifyOptions opts;
    opts.simplify_constants           = true;
    opts.simplify_defines             = true;
    opts.simplify_parameters          = true;
    opts.simplify_template_parameters = true;
    opts.simplify_statements          = true;
    opts.simplify_generates           = true;
    opts.simplify_functioncalls       = true;
    opts.behavior                     = SimplifyOptions::BEHAVIOR_AGGRESSIVE;
    hif::manipulation::simplify(fun, _sem, opts);
    State *state = fun->getStateTable()->states.front();
    if (state->actions.size() != 1) {
        delete fun;
        return false;
    }
    Return *ret = dynamic_cast<Return *>(state->actions.front());
    if (ret == nullptr) {
        delete fun;
        return false;
    }
    Value *v = ret->getValue();
    messageAssert(v != nullptr, "Return statement without value", ret, _sem);
    // Check whether the returned value has some references to local declarations
    hif::semantics::ReferencesMap refMap;
    hif::semantics::getAllReferences(refMap, _sem, v);
    for (hif::semantics::ReferencesMap::iterator it = refMap.begin(); it != refMap.end(); ++it) {
        Declaration *decl = it->first;
        if (isSubNode(decl, fun)) {
            delete fun;
            return false;
        }
    }
    // Replace
    _replace(o, v);
    delete fun;
    return true;
}

bool SimplifyVisitor::_simplifyIteratedConcat(FunctionCall *o)
{
    std::string fcName(o->getName());
    if (fcName != "hif_verilog_iterated_concat")
        return false;

    if (o->templateParameterAssigns.empty())
        return false;
    if (o->parameterAssigns.empty())
        return false;

    Value *param1 = dynamic_cast<ValueTPAssign *>(o->templateParameterAssigns.front())->getValue();
    Value *param2 = o->parameterAssigns.at(0)->getValue();
    if (param1 == nullptr || param2 == nullptr)
        return false;

    IntValue *times = dynamic_cast<IntValue *>(param1);
    if (times == nullptr)
        return false;
    Value *ret = param2;
    for (long long i = 1; i < times->getValue(); ++i) {
        Expression *expr = _factory.expression(hif::copy(param2), hif::op_concat, ret);
        ret              = expr;
    }
    _replace(o, ret);
    ret->acceptVisitor(*this);
    return true;
}

template <typename T> bool SimplifyVisitor::_simplifyUselessFor(T *o)
{
    int boolCond = _resolveBoolCondition(o->getCondition());
    if (boolCond != 0)
        return false;
    if (_isLoopWithControlActions(o))
        return false;

    _trash.insert(o);
    return true;
}

bool SimplifyVisitor::_simplifyConstantLoopFor(For *o)
{
    // If only 1 loop is requested, move actions outside for statement.
    long long iter, min, max;
    if (!_resolveForLoopBound(o->initDeclarations, o->initValues, o->getCondition(), o->stepActions, iter, min, max)) {
        return 0;
    }

    if (iter > 1 && !_opt.simplify_statements)
        return 0;
    if (_isLoopWithControlActions(o))
        return 0;

    // TODO: in this case, we have to:
    // - check that assigns are direct assigns to names (no slices, members,
    //   etc, since supporting them seems too expensive)
    // - enrich the following unrolling method
    // - set the for loop var to the value of the last iteration, since it could
    //   be used elsewhere
    // Ref design: verilog/openCores/RS_dec + ddt
    // TODO: add also a simplify that moves the for loop index from global to local
    // e.g.: int i; for (i = 0, ...) --> for (int i = 0; ...)
    if (!o->initValues.empty())
        return 0;

    // Heuristic to choose whether simplify for statemente.
    long long threshold = o->forActions.size() * iter;
    if (threshold > FOR_UNROLL_THRESHOLD)
        return 0;

    BList<Action> result;

    if (dynamic_cast<Range *>(o->getCondition()) != nullptr) {
        messageAssert(o->initDeclarations.size() == 1, "Invalid for loop.", o, _sem);
        DataDeclaration *dd = o->initDeclarations.front();
        if (dd->getValue() == nullptr) {
            // cannot simplify.
            // ref. design: vhdl/openCores/LPF3x8
            return 0;
        }
    }

    for (long long counter = 0; counter < iter; ++counter) {
        BList<Action> acts;
        hif::copy(o->forActions, acts);
        for (BList<DataDeclaration>::iterator i = o->initDeclarations.begin(); i != o->initDeclarations.end(); ++i) {
            DataDeclaration *dd = *i;
            hif::semantics::ReferencesSet refs;
            hif::semantics::getReferences(*i, refs, _sem, acts);
            for (hif::semantics::ReferencesSet::iterator j = refs.begin(); j != refs.end(); ++j) {
                Object *ref = *j;
                ref->replace(hif::copy(dd->getValue()));
                delete ref;
            }
        }

        result.merge(acts);
        // Updating initial value w.r.t. current iteration
        for (BList<DataDeclaration>::iterator i = o->initDeclarations.begin(); i != o->initDeclarations.end(); ++i) {
            DataDeclaration *dd = *i;
            for (BList<Action>::iterator j = o->stepActions.begin(); j != o->stepActions.end(); ++j) {
                Assign *a = dynamic_cast<Assign *>(*j);
                messageDebugAssert(a != nullptr, "Unexpected step action", *j, _sem);
                if (a == nullptr)
                    continue;

                Identifier *target = dynamic_cast<Identifier *>(a->getLeftHandSide());
                messageDebugAssert(target != nullptr, "Unexpected step assign target", *j, _sem);
                if (target == nullptr)
                    continue;

                if (target->getName() != dd->getName())
                    continue;

                Value *src = hif::copy(a->getRightHandSide());
                hif::semantics::ReferencesSet refs;
                hif::semantics::getReferences(dd, refs, _sem, src);
                for (hif::semantics::ReferencesSet::iterator k = refs.begin(); k != refs.end(); ++k) {
                    Object *ref = *k;
                    ref->replace(hif::copy(dd->getValue()));
                    delete ref;
                }

                dd->setValue(src);
            }
        }
    }
    o->forActions.clear();
    o->forActions.merge(result);
    GuideVisitor::visitList(o->forActions);
    _replaceWithList(o, o->forActions);
    return true;
}

bool SimplifyVisitor::_simplifySingleBitMember(Member *o)
{
    // Can be a single bit, with a member of zero, e.g.:
    // bit/bool b; b[0]
    // bv<1> bv; bv[0]
    Type *prefixBaseType = hif::semantics::getBaseType(o->getPrefix(), false, _sem);
    if (dynamic_cast<Bit *>(prefixBaseType) != nullptr || dynamic_cast<Bool *>(prefixBaseType) != nullptr) {
        Value *prefix = o->setPrefix(nullptr);
        _replace(o, prefix);
        return true;
    } else if (
        hif::semantics::isVectorType(prefixBaseType, _sem) && !hif::manipulation::isInLeftHandSide(o) &&
        hif::semantics::typeGetSpanBitwidth(prefixBaseType, _sem) == 1) {
        Type *t          = hif::semantics::getSemanticType(o, _sem);
        Type *prefixType = hif::semantics::getSemanticType(o->getPrefix(), _sem);
        if (t == nullptr)
            return false;
        Value *value = o->getPrefix();
        if (!hif::objectIsInSensitivityList(o)) {
            if (!_sem->isCastAllowed(t, prefixType))
                return false;
            Cast *c = new Cast();
            c->setValue(value);
            c->setType(hif::copy(t));
            _replace(o, c);
            value = c;
        } else {
            _replace(o, value);
        }
        value->acceptVisitor(*this);
        return true;
    }

    return false;
}

bool SimplifyVisitor::_simplifyBitvectorValueMember(Member *o)
{
    BitvectorValue *bvPrefix = dynamic_cast<BitvectorValue *>(o->getPrefix());
    if (bvPrefix == nullptr)
        return false;

    // it may be a name simplified.
    IntValue *iv = _getIndex(o->getIndex());
    if (iv == nullptr)
        return 0;

    // example:
    // - before simplify names: n = sc_lv("01zx0"); n[2];
    // - after simplify: sc_lv("01zx0")[2]
    // - after this simplify: z.
    Type *memType = hif::semantics::getSemanticType(o, _sem);
    if (memType == nullptr) {
        delete iv;
        return 0;
    }

    std::string val(bvPrefix->getValue());

    Bit *bitType = dynamic_cast<Bit *>(hif::semantics::getBaseType(memType, false, _sem));
    messageAssert(bitType != nullptr, "Unexpected non-bit memType", memType, _sem);

    Range *r = hif::typeGetSpan(hif::semantics::getSemanticType(bvPrefix, _sem), _sem);
    if (r == nullptr) {
        delete iv;
        return false;
    }

    Type *t = memType;
    t->setTypeVariant(Type::NATIVE_TYPE);
    char bitValue = '0';
    if (r->getDirection() == dir_upto) {
        bitValue = val[static_cast<long unsigned>(iv->getValue())];
    } else if (r->getDirection() == dir_downto) {
        bitValue = val[static_cast<long unsigned>(static_cast<long long>(val.length()) - iv->getValue() - 1)];
    } else {
        messageError("Unexpected range direction", r, _sem);
    }
    BitValue *bval = new BitValue();
    bval->setValue(bitValue);
    bval->setType(hif::copy(t));
    hif::typeSetConstexpr(bval->getType(), true);

    _replace(o, bval);

    bval->acceptVisitor(*this);
    delete iv;
    return true;
}

bool SimplifyVisitor::_simplifyAggregateMember(Member *o)
{
    Aggregate *agPrefix = dynamic_cast<Aggregate *>(o->getPrefix());
    if (agPrefix == nullptr)
        return false;

    // it may be a name simplified.
    IntValue *iv = _getIndex(o->getIndex());

    // example:
    // - before simplify names: n = {10,20,30}; n[2];
    // - after simplify: {10,20,30}[2]
    // - after this simplify: 30.
    Value *res          = nullptr;
    bool canCheckOthers = true;
    for (BList<AggregateAlt>::iterator i = agPrefix->alts.begin(); i != agPrefix->alts.end(); ++i) {
        IntValue *intv = nullptr;
        for (BList<Value>::iterator j = (*i)->indices.begin(); j != (*i)->indices.end(); ++j) {
            if (!hif::equals(*j, o->getIndex())) {
                intv = _getIndex(*j);
                if (intv == nullptr || iv == nullptr) {
                    canCheckOthers = false;
                    continue;
                }
                if (intv->getValue() != iv->getValue())
                    continue;
            }

            res = hif::copy((*i)->getValue());
            break;
        }
        if (res != nullptr)
            break;
    }

    if (res == nullptr && canCheckOthers) {
        res = hif::copy(agPrefix->getOthers());
    }

    if (res == nullptr) {
        delete iv;
        return 0;
    }

    _replace(o, res);
    res->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifySliceMember(Member *o)
{
    Slice *slice = dynamic_cast<Slice *>(o->getPrefix());
    if (slice == nullptr)
        return false;

    if (_sem->isSliceTypeRebased()) {
        // Then the index is from zero:
        Value *min = hif::rangeGetMinBound(slice->getSpan());
        Expression *newIndex =
            _factory.expression(o->setIndex(nullptr), hif::op_plus, hif::manipulation::assureSyntacticType(min, _sem));
        o->setIndex(newIndex);
        o->setPrefix(slice->setPrefix(nullptr));
    } else {
        // Then the slice is useless:
        o->setPrefix(slice->setPrefix(nullptr));
    }
    delete slice;
    o->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifyBitwiseExpressionMember(Member *o)
{
    // (a_128 & b_128)[42] -> (a_128[42] & b_128[42])
    Value *prefix = o->getPrefix();
    if (dynamic_cast<Expression *>(prefix) == nullptr)
        return false;
    Expression *expr = static_cast<Expression *>(prefix);
    if (!hif::operatorIsBitwise(expr->getOperator()))
        return false;
    Value *index = o->getIndex();
    Member *m1   = _factory.member(expr->getValue1(), index);
    expr->setValue1(m1);
    if (expr->getValue2() != nullptr) {
        Member *m2 = _factory.member(expr->getValue2(), hif::copy(index));
        expr->setValue2(m2);
    }
    _replace(o, expr);
    expr->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifyConcatMember(Member *o)
{
    // (a_64,b_64,c_64)[100] -> b_64[35]
    Expression *prefix = dynamic_cast<Expression *>(o->getPrefix());
    if (prefix == nullptr)
        return false;
    if (prefix->getOperator() != op_concat)
        return false;
    Type *prefixType = hif::semantics::getSemanticType(prefix, _sem);
    if (prefixType == nullptr)
        return false;
    Range *prefixSpan = hif::typeGetSpan(prefixType, _sem);
    if (prefixSpan == nullptr)
        return false;
    const bool isPrefixDownto = prefixSpan->getDirection() == hif::dir_downto;

    std::vector<Value *> concatElements;
    _getConcatElements(concatElements, prefix);
    std::vector<long long> concatBW;
    for (std::vector<Value *>::iterator it = concatElements.begin(); it != concatElements.end(); ++it) {
        Type *t                      = hif::semantics::getSemanticType(*it, _sem);
        unsigned long long elementBw = hif::semantics::typeGetSpanBitwidth(t, _sem);
        if (elementBw == 0)
            return false;
        concatBW.push_back(static_cast<long long>(elementBw));
    }

    IntValue *index = dynamic_cast<IntValue *>(o->getIndex());
    if (index == nullptr)
        return false;
    long long memberVal = index->getValue();
    Value *selected     = nullptr;

    if (isPrefixDownto) {
        std::vector<Value *>::reverse_iterator v_it    = concatElements.rbegin();
        std::vector<long long>::reverse_iterator bw_it = concatBW.rbegin();

        for (; v_it != concatElements.rend() && bw_it != concatBW.rend(); ++v_it, ++bw_it) {
            if (memberVal < *bw_it) {
                selected = hif::copy(*v_it);
                break;
            }

            memberVal = memberVal - *bw_it;
        }
    } else {
        std::vector<Value *>::iterator v_it    = concatElements.begin();
        std::vector<long long>::iterator bw_it = concatBW.begin();

        for (; v_it != concatElements.end() && bw_it != concatBW.end(); ++v_it, ++bw_it) {
            if (memberVal < *bw_it) {
                selected = hif::copy(*v_it);
                break;
            }

            memberVal = memberVal - *bw_it;
        }
    }

    if (selected == nullptr)
        return false;

    if (dynamic_cast<Array *>(prefixType) != nullptr) {
        _replace(o, selected);
        selected->acceptVisitor(*this);
    } else {
        Member *ret = _factory.member(selected, new IntValue(memberVal));
        _replace(o, ret);
        ret->acceptVisitor(*this);
    }

    return true;
}

bool SimplifyVisitor::_simplifyUnrollAggregate(Member *o)
{
    // Aggr<T>[i] ->
    // with(i)
    // case 0: aggr<T>[0]
    // case 1: aggr<T>[1]
    // ... case n-1: aggr<T>[n-1]
    // default: aggr<T>[n]
    Identifier *id = dynamic_cast<Identifier *>(o->getIndex());
    if (id == nullptr)
        return false;
    // ref design: vhdl/ips/mephisto_core + ddt
    DataDeclaration *idDecl = hif::semantics::getDeclaration(id, _sem);
    if (dynamic_cast<Variable *>(idDecl) == nullptr && dynamic_cast<Signal *>(idDecl) == nullptr)
        return false;

    Aggregate *aggr = dynamic_cast<Aggregate *>(o->getPrefix());
    if (aggr == nullptr)
        return false;
    Type *semType         = hif::semantics::getSemanticType(o->getIndex(), _sem);
    Type *baseType        = hif::semantics::getBaseType(aggr, false, _sem);
    Range *span           = hif::typeGetSpan(baseType, _sem);
    unsigned long long bw = hif::semantics::spanGetBitwidth(span, _sem);
    if (bw == 0)
        return false;

    Value *minBound = hif::rangeGetMinBound(span);
    With *w         = new With();
    w->setCondition(hif::manipulation::assureSyntacticType(o->getIndex(), _sem));
    std::size_t altSize = aggr->alts.size();
    messageAssert(altSize >= 2, "Wrong number of alts.", o, _sem);
    for (std::size_t i = 0; i < altSize - 2; ++i) {
        WithAlt *wa  = new WithAlt();
        Value *index = _factory.cast(
            hif::copy(semType),
            _factory.expression(
                hif::manipulation::assureSyntacticType(hif::copy(minBound), _sem), hif::op_plus, _factory.intval(i)));
        wa->conditions.push_back(index);
        wa->setValue(_factory.member(hif::copy(o->getPrefix()), hif::copy(index)));
    }
    w->setDefault(_factory.member(
        hif::copy(o->getPrefix()),
        _factory.cast(
            hif::copy(semType), _factory.expression(
                                    hif::manipulation::assureSyntacticType(hif::copy(minBound), _sem), hif::op_plus,
                                    _factory.intval(altSize - 1)))));

    _replace(o, w);
    w->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifyMemberInternalCast(Member *o)
{
    auto memberType = hif::semantics::getSemanticType(o, _sem);
    if (memberType == nullptr)
        return false;
    auto cast = dynamic_cast<Cast *>(o->getPrefix());
    if (cast == nullptr)
        return false;
    auto value = cast->getValue();

    auto castType = hif::semantics::getBaseType(cast, false, _sem);
    if (!hif::semantics::isVectorType(castType, _sem))
        return false;
    auto valueType = hif::semantics::getBaseType(cast->getValue(), false, _sem);
    if (!hif::semantics::isVectorType(valueType, _sem))
        return false;

    // Ensuring the cast does not extends:
    const auto comp = hif::semantics::comparePrecision(castType, valueType, _sem);
    if (comp != hif::semantics::LESS_PRECISION && comp != hif::semantics::EQUAL_PRECISION)
        return false;

    // Check types are vectors or arrays:
    const auto castIsVector  = hif::semantics::isVectorType(castType, _sem);
    const auto castIsArray   = dynamic_cast<Array *>(castType) != nullptr;
    const auto valueIsVector = hif::semantics::isVectorType(valueType, _sem);
    const auto valueIsArray  = dynamic_cast<Array *>(valueType) != nullptr;
    if (!(castIsVector || castIsArray) || !(valueIsVector || valueIsArray))
        return false;

    auto castSpan  = hif::typeGetSpan(castType, _sem);
    auto valueSpan = hif::typeGetSpan(valueType, _sem);
    if (castSpan == nullptr || valueSpan == nullptr)
        return false;

    if (castIsVector && valueIsVector) {
        // Ref designs:
        // - openCores_aes_original_verilog2hif_hif2sc
        // - openCores_uart_vhdl2hif_hr
        EqualsOptions eqOpts;
        eqOpts.checkSpans         = false;
        eqOpts.checkConstexprFlag = false;
        eqOpts.checkLogicFlag     = false;
        eqOpts.checkSignedFlag    = false;
        eqOpts.checkResolvedFlag  = false;
        eqOpts.checkStringSpan    = false;
        eqOpts.handleVectorTypes  = true;

        if (!hif::equals(castType, valueType, eqOpts))
            return false;
    }

    // Substitute to check types:
    o->setPrefix(value);
    auto newType           = _sem->getMemberSemanticType(o);
    const bool sameBitType = hif::equals(newType, memberType);

    // Ensuring same dir, or rebasing index:
    const bool sameDir = castSpan->getDirection() == valueSpan->getDirection();
    if (!sameDir) {
        auto castMax = rangeGetMaxBound(castSpan);
        o->setIndex(_factory.expression(
            hif::manipulation::assureSyntacticType(hif::copy(castMax), _sem), hif::op_minus, o->getIndex()));
    }

    // Ensuring cast does not rebases the semantic type,
    // or we need to rebase:
    auto castMin  = rangeGetMinBound(castSpan);
    auto valueMin = rangeGetMinBound(valueSpan);
    if (!hif::equals(castMin, valueMin)) {
        auto rebase = _factory.expression(
            hif::manipulation::assureSyntacticType(hif::copy(castMin), _sem), hif::op_minus,
            hif::manipulation::assureSyntacticType(hif::copy(valueMin), _sem));
        o->setIndex(
            _factory.expression(hif::manipulation::assureSyntacticType(o->getIndex(), _sem), hif::op_minus, rebase));
    }

    Value *recall = o;
    if (!sameBitType) {
        auto c = new Cast();
        c->setType(hif::copy(memberType));
        o->replace(c);
        c->setValue(o);
        recall = c;
    }

    delete cast;
    recall->acceptVisitor(*this);
    return true;
}
bool SimplifyVisitor::_simplifyNames(Object *o)
{
    if (!_opt.simplify_constants && !_opt.simplify_template_parameters && !_opt.simplify_parameters &&
        !_opt.simplify_typereferences) {
        return false;
    }

    // Getting symbol declaration
    Declaration *decl = hif::semantics::getDeclaration(o, _sem);
    if (decl == nullptr)
        return false;

    // Checking context
    if (_opt.context != nullptr) {
        hif::semantics::DeclarationOptions dopt;
        dopt.location     = _opt.context;
        dopt.forceRefresh = true;

        Declaration *contextDecl = nullptr;

        FieldReference *fr  = dynamic_cast<FieldReference *>(o);
        DataDeclaration *dd = dynamic_cast<DataDeclaration *>(decl);
        if (fr != nullptr && dd != nullptr) {
            // Is a fieldref to constant or variable.
            // Since librearyDefs are global, prefixed symbols are always visible.
            // Therefore here we want to check visibility without considering
            // prefix library.
            // Reference design: mephisto_core.
            Identifier id(fr->getName());
            contextDecl = hif::semantics::getDeclaration(&id, _sem, dopt);
        } else {
            contextDecl = hif::semantics::getDeclaration(o, _sem, dopt);
            hif::semantics::setDeclaration(o, decl);
        }

        // Context declaration could be not the original one,
        // so the correct decl is the original!
        if (contextDecl != nullptr && contextDecl == decl)
            return false;
    }

    // Checkig root
    if (_opt.root != nullptr) {
        if (!hif::isSubNode(decl, _opt.root))
            return false;
    }

    if (_simplifyNamesOfConstants(o, decl))
        return true;
    if (_simplifyNamesOfTemplateParameters(o, decl))
        return true;
    if (_simplifyNamesOfParameters(o, decl))
        return true;
    if (_simplifyNamesOfTypeReferences(o, decl))
        return true;

    return false;
}

bool SimplifyVisitor::_simplifyNamesOfConstants(Object *o, Declaration *decl)
{
    if (!_opt.simplify_constants)
        return false;

    hif::EqualsOptions opt;
    opt.assureSameSymbolDeclarations = true;

    Const *c = dynamic_cast<Const *>(decl);
    Type *st = hif::objectGetType(decl);
    if (c != nullptr && c->getValue() != nullptr && (!c->isDefine() || _opt.simplify_defines)) {
        Value *copy = _makeSubstituible(c->getValue(), st);
        if (hif::equals(copy, o, opt)) {
            delete copy;
            return false;
        }

        _replace(o, copy);
        copy->acceptVisitor(*this);
        return true;
    }

    EnumValue *ev = dynamic_cast<EnumValue *>(decl);
    if (ev != nullptr && ev->getValue() != nullptr) {
        Value *copy = _makeSubstituible(ev->getValue(), st);
        if (hif::equals(copy, o, opt)) {
            delete copy;
            return false;
        }

        _replace(o, copy);
        copy->acceptVisitor(*this);
        return true;
    }

    return false;
}

bool SimplifyVisitor::_simplifyNamesOfTemplateParameters(Object *o, Declaration *decl)
{
    if (!_opt.simplify_template_parameters)
        return false;

    hif::EqualsOptions opt;
    opt.assureSameSymbolDeclarations = true;

    ValueTP *vtp = dynamic_cast<ValueTP *>(decl);
    if (vtp != nullptr && vtp->getValue() != nullptr) {
        const auto isCtc = vtp->isCompileTimeConstant();
        if (isCtc && !_opt.simplify_ctc_template_parameters)
            return false;
        if (!isCtc && !_opt.simplify_non_ctc_template_parameters)
            return false;

        if (_isSelfInitialization(o, vtp)) {
            // cannot simplify to avoid infinite loop.
            return false;
        }

        // WARNING: this fix may be not correct to avoid infinite loop.
        // In fact the check should be smarter since constructors
        // have to substitute its template parameters.
        // Reference designs: crc, dig_proc2, uart_vhdl (c++)
        // if (_isSelfAssignment(o, vtp))
        // {
        //     // cannot simplify to avoid infinite loop.
        //     return false;
        // }
        Value *copy = _makeSubstituible(vtp->getValue(), vtp->getType());
        if (hif::equals(copy, o, opt)) {
            delete copy;
            return false;
        }

        _replace(o, copy);
        copy->acceptVisitor(*this);
        return true;
    }

    TypeTP *ttp = dynamic_cast<TypeTP *>(decl);
    if (ttp != nullptr && ttp->getType() != nullptr) {
        if (_isSelfInitialization(o, ttp)) {
            // cannot simplify to avoid infinite loop.
            return false;
        }

        Type *copy = hif::copy(ttp->getType());
        if (hif::equals(copy, o, opt)) {
            delete copy;
            return false;
        }

        _replace(o, copy);
        copy->acceptVisitor(*this);
        return true;
    }

    return false;
}

bool SimplifyVisitor::_simplifyNamesOfParameters(Object *o, Declaration *decl)
{
    if (!_opt.simplify_parameters)
        return false;

    hif::EqualsOptions opt;
    opt.assureSameSymbolDeclarations = true;

    Parameter *p = dynamic_cast<Parameter *>(decl);
    if (p == nullptr || p->getValue() == nullptr)
        return false;

    if (_isSelfInitialization(o, p)) {
        // cannot simplify to avoid infinite loop.
        return false;
    }

    Value *copy = _makeSubstituible(p->getValue(), p->getType());
    if (hif::equals(copy, o, opt)) {
        delete copy;
        return false;
    }

    _replace(o, copy);
    copy->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifyNamesOfTypeReferences(Object *o, Declaration *decl)
{
    if (!_opt.simplify_typereferences)
        return false;

    TypeDef *td = dynamic_cast<TypeDef *>(decl);
    if (td == nullptr)
        return false;

    Type *t = hif::semantics::getBaseType(dynamic_cast<TypeReference *>(o), false, _sem, true);
    messageAssert(t != nullptr, "Base type not found.", o, _sem);

    hif::semantics::updateDeclarations(t, _sem);
    Type *copy = hif::copy(t);
    _replace(o, copy);
    copy->acceptVisitor(*this);

    return true;
}

bool SimplifyVisitor::_replace(Object *from, Object *&to)
{
    if (to == nullptr)
        return false;

    if (from == _result)
        _result = to;

    if (from == to)
        return true;
    if (from->getParent() == nullptr) {
        delete from;
        return false;
    }

    from->replace(to);
    delete from;
    return true;
}

template <class T1, class T2> bool SimplifyVisitor::_replace(T1 *from, T2 *&to)
{
    Object *loc         = to;
    const bool replaced = _replace(static_cast<Object *>(from), loc);
    to                  = static_cast<T2 *>(loc);
    return replaced;
}

template <class T> bool SimplifyVisitor::_replace(T *from, BList<T> &to)
{
    return _replace(static_cast<Object *>(from), to);
}

void SimplifyVisitor::_replaceWithList(Object *from, BList<Object> &to)
{
    if (from == _result) {
        // If is the object to be simplified remove it since cannot return the list.
        _result = nullptr;
    }

    if (from->getParent() == nullptr) {
        // If unrelated cannot replace.
        delete from;
        return;
    }

    if (to.empty()) {
        // Replace with empty list => replace with nothing.
        // Insert in trash to remove..
        _trash.insert(from);
        return;
    }

    from->replaceWithList(to);
    delete from;
}

bool SimplifyVisitor::_isAllowedCommutative(Operator o)
{
    switch (o) {
    case op_plus:
    case op_minus:
    case op_mult:
    case op_div:
    case op_band:
    case op_bor:
    case op_bxor:
    case op_xor:
        return hif::operatorIsCommutative(o);
    default: // error
        break;
    }
    return false;
}

Operator SimplifyVisitor::_getAllowedInverseOp(Operator o)
{
    switch (o) {
    case op_plus:
    case op_minus:
    case op_mult:
    case op_div:
        return hif::operatorGetInverse(o);
    default: // error
        break;
    }
    return op_none;
}

Expression *SimplifyVisitor::_getRebalancedExpressions(Expression *e)
{
    Type *torig = hif::semantics::getSemanticType(e, _sem);
    if (torig == nullptr) {
#if 0
        hif::semantics::resetTypes(e);
        torig = hif::semantics::getSemanticType(e, _sem);
#endif
        messageError("Cannot type the expr (1).", e, _sem);
    }

    Value *v       = nullptr;
    Value *cv1     = nullptr;
    Value *cv2     = nullptr;
    Operator oper1 = e->getOperator();
    Operator oper2 = e->getOperator();
    bool left1     = false;
    bool left2     = false;
    if (!_getNestedExpressionOperands(e, v, cv1, cv2, oper1, oper2, left1, left2)) {
        // not searched cases
        return nullptr;
    }

    Expression *e1 = new Expression();
    Expression *e2 = new Expression();
    Operator op1;
    Operator op2;

    if (left1 && !left2) // Case 1
    {
        // original tree:
        // expr(op1)[ expr(op2)[ cv2, v ], cv1 ]
        //
        // transformed tree:
        // expr(op1)[ expr(op2)[ cv2, cv1 ], v ]
        //
        // operation table (the same with * and /):
        // | oper1 | oper2 |  op1  |  op2  |
        // |   +   |   +   |   +   |   +   |
        // |   +   |   -   |   -   |   +   |
        // |   -   |   +   |   +   |   -   |
        // |   -   |   -   |   -   |   -   |

        // No possible division by zero

        e1->setValue1(e2);
        e1->setValue2(hif::copy(v));
        e2->setValue1(hif::copy(cv2));
        e2->setValue2(hif::copy(cv1));

        op1 = oper2;
        op2 = oper1;
    } else if (left1 && left2) // Case 2
    {
        // original tree:
        //  expr(op1)[ expr(op2)[ v, cv2 ], cv1 ]
        //
        // transformed tree:
        // expr(op1)[ v, expr(op2)[ cv2, cv1 ] ]
        //
        // operation table (the same with * and /):
        // | oper1 | oper2 |  op1  |  op2  |
        // |   +   |   +   |   +   |   +   |
        // |   +   |   -   |   -   |   -   |
        // |   -   |   +   |   +   |   -   |
        // |   -   |   -   |   -   |   +   |

        // Special cases to avoid /0:
        // |   *   |   /   |   *   |   /   |
        // expr(op1)[ v, expr(op2)[ cv1, cv2 ] ]

        if (oper1 == op_mult && oper2 == op_div) {
            // special case management
            e1->setValue1(hif::copy(v));
            e1->setValue2(e2);
            e2->setValue1(hif::copy(cv1));
            e2->setValue2(hif::copy(cv2));

            op1 = op_mult;
            op2 = op_div;
        } else {
            e1->setValue1(hif::copy(v));
            e1->setValue2(e2);
            e2->setValue1(hif::copy(cv2));
            e2->setValue2(hif::copy(cv1));

            op1 = oper2;
            op2 = (_isAllowedCommutative(oper1) ? oper2 : _getAllowedInverseOp(oper2));
        }
    } else if (!left1 && !left2) // Case 3
    {
        // original tree:
        // expr(op1)[ cv1, expr(op2)[ cv2, v ] ]
        //
        // transformed tree:
        // expr(op1)[ expr(op2)[ cv1, cv2 ], v ]
        //
        // operation table (the same with * and /):
        // | oper1 | oper2 |  op1  |  op2  |
        // |   +   |   +   |   +   |   +   |
        // |   +   |   -   |   -   |   +   |
        // |   -   |   +   |   -   |   -   |
        // |   -   |   -   |   +   |   -   |

        // No possible division by zero

        e1->setValue1(e2);
        e1->setValue2(hif::copy(v));
        e2->setValue1(hif::copy(cv1));
        e2->setValue2(hif::copy(cv2));

        op1 = (oper1 == oper2) ? (_isAllowedCommutative(oper1) ? oper1 : _getAllowedInverseOp(oper1))
                               : (_isAllowedCommutative(oper1) ? oper2 : oper1);
        op2 = oper1;
    } else //if (!left1 && left2) // Case 4
    {
        // original tree:
        // expr(op1)[ cv1, expr(op2)[ v, cv2 ] ]
        //
        // transformed tree:
        // expr(op1)[ expr(op2)[ cv1, cv2 ], v ]
        //
        // operation table (the same with * and /):
        // | oper1 | oper2 |  op1  |  op2  |
        // |   +   |   +   |   +   |   +   |
        // |   +   |   -   |   +   |   -   |
        // |   -   |   +   |   -   |   -   |
        // |   -   |   -   |   -   |   +   |

        // No possible division by zero

        e1->setValue1(e2);
        e1->setValue2(hif::copy(v));
        e2->setValue1(hif::copy(cv1));
        e2->setValue2(hif::copy(cv2));

        op1 = oper1;
        op2 = (_isAllowedCommutative(oper1) ? oper2 : _getAllowedInverseOp(oper2));
    }

    // set operators
    e1->setOperator(op1);
    e2->setOperator(op2);

    // Other optimization: if left is v and op1 is commutative,
    // ==> place v as right.
    if ((left1 && left2) && _isAllowedCommutative(e1->getOperator())) // cases 2
    {
        Value *tmp = e1->getValue1();
        e1->setValue1(e1->getValue2());
        e1->setValue2(tmp);
    }

    // check that operation is allowed in semantics
    Type *t = hif::semantics::getSemanticType(e1, _sem);
    if (t == nullptr) {
        messageDebug("Cannot type the expr (2).", e1, _sem);
        messageError("Original expression:", e, _sem);
    }

    // simplify the new nested expression
    Value *v2 = simplifyExpression(e2, _sem, _opt);
    if (v2 != nullptr) {
        _replace(e2, v2); // performs delete
    } else if (
        dynamic_cast<ConstValue *>(e2->getValue1()) != nullptr &&
        dynamic_cast<ConstValue *>(e2->getValue2()) != nullptr) {
        // Avoid infinite loop since paired const values may be not simplified
        // by simplifyExpression (e.g. division by interges).
        // Ref. design: vhdl/gaisler/can_oc
        delete e1;
        return nullptr;
    }
    // Must perform this visit since new nearest subtrees could be
    // simplified more, and are different from the original ones.
    // Right-pushed const could require to be fixed too.
    GuideVisitor::visitExpression(*e1);
    Expression *ret = _getRebalancedExpressions(e1);
    if (ret != nullptr) {
        delete e1;
        return ret;
    }

    // TODO: check if it is required to set the original semantics type
    // to the new expression

    return e1;
}

bool SimplifyVisitor::_simplifyMultiplication(Expression *e)
{
    // E.g. [[v *2] - v] --> [v * [2-1]]

    Expression *e1 = dynamic_cast<Expression *>(e->getValue1());
    if (e1 == nullptr)
        return false;

    if (e1->getOperator() != op_mult || (e->getOperator() != op_plus && e->getOperator() != op_minus))
        return false;

    ConstValue *cv = dynamic_cast<ConstValue *>(e1->getValue2());
    if (cv == nullptr)
        return false;

    if (!hif::equals(e1->getValue1(), e->getValue2()))
        return false;

    const hif::Operator inner = e->getOperator();

    e->setValue1(e->setValue2(nullptr));
    e->setOperator(op_mult);

    e->setValue2(e1);
    e1->setOperator(inner);
    delete e1->setValue1(cv);
    e1->setValue2(_factory.intval(1));

    hif::semantics::resetTypes(e, false);
    hif::semantics::resetTypes(e1, false);

    e->acceptVisitor(*this);

    return true;
}

bool SimplifyVisitor::_simplifyArithBitOperation(Expression *e)
{
    if (!hif::operatorIsArithmetic(e->getOperator()))
        return false;
    if (e->getValue2() == nullptr)
        return false;

    Type *t1 = hif::semantics::getSemanticType(e->getValue1(), _sem);
    Type *t2 = hif::semantics::getSemanticType(e->getValue2(), _sem);

    hif::semantics::ILanguageSemantics::ExpressionTypeInfo info = _sem->getExprType(t1, t2, e->getOperator(), e);
    if (hif::typeIsLogic(info.returnedType, _sem) || hif::typeIsLogic(info.operationPrecision, _sem))
        return false;
    unsigned long long returnSize    = hif::semantics::typeGetSpanBitwidth(info.returnedType, _sem);
    unsigned long long precisionSize = hif::semantics::typeGetSpanBitwidth(info.operationPrecision, _sem);
    if (returnSize != 1 || precisionSize != 1)
        return false;

    const bool isBoolean =
        dynamic_cast<Bool *>(hif::semantics::getBaseType(info.operationPrecision, false, _sem)) != nullptr;

    // a + b, a - b -> a xor b
    // a * b -> a and b
    // a / b, a^b -> a
    hif::Operator op = e->getOperator();
    if (op == hif::op_plus || op == hif::op_minus) {
        if (isBoolean)
            e->setOperator(hif::op_xor);
        else
            e->setOperator(hif::op_bxor);
        return true;
    } else if (op == hif::op_mult) {
        if (isBoolean)
            e->setOperator(hif::op_and);
        else
            e->setOperator(hif::op_band);
        return true;
    } else if (op == hif::op_div || op == hif::op_pow) {
        Value *v = hif::copy(e->getValue1());
        _replace(e, v);
        return true;
    }

    return false;
}

bool SimplifyVisitor::_pushDownOperators(Expression *o)
{
    Expression *e2 = dynamic_cast<Expression *>(o->getValue2());
    if (e2 == nullptr)
        return false;

    // unary
    if (e2->getValue2() == nullptr) {
        if (o->getOperator() == e2->getOperator() && o->getOperator() == op_plus) {
            Value *v = e2->setValue1(nullptr);
            o->setValue2(v);
            delete e2;
            return true;
        } else if (o->getOperator() == e2->getOperator() && o->getOperator() == op_minus) {
            // Note: this case is suitable also for op_div, but we are not
            // considering it since it cannot be unary.
            Value *v = e2->setValue1(nullptr);
            o->setValue2(v);
            o->setOperator(op_plus);
            delete e2;
            return true;
        } else if (o->getOperator() == hif::operatorGetInverse(e2->getOperator())) {
            // Since the only operators that is both unary and binary are + and -
            Value *v = e2->setValue1(nullptr);
            o->setValue2(v);
            o->setOperator(op_minus);
            delete e2;
            return true;
        }
        return false;
    }

    // binary
    if (o->getOperator() != e2->getOperator())
        return false;

    if (o->getOperator() != op_minus && o->getOperator() != op_div)
        return false;

    o->setOperator(hif::operatorGetInverse(o->getOperator()));
    Value *tmp = e2->setValue1(e2->getValue2());
    e2->setValue2(tmp);
    return true;
}

bool SimplifyVisitor::_pushUpConstants(Expression *o)
{
    Expression *e1  = dynamic_cast<Expression *>(o->getValue1());
    ConstValue *cv2 = dynamic_cast<ConstValue *>(o->getValue2());
    if (e1 == nullptr || cv2 != nullptr)
        return false;

    ConstValue *cv1_2 = dynamic_cast<ConstValue *>(e1->getValue2());
    ConstValue *cv1_1 = dynamic_cast<ConstValue *>(e1->getValue1());
    if (cv1_1 != nullptr || cv1_2 == nullptr)
        return false;

    if (o->getOperator() != op_plus && o->getOperator() != op_minus && o->getOperator() != op_mult &&
        o->getOperator() != op_div) {
        return false;
    }

    if (o->getOperator() != e1->getOperator() || hif::operatorGetInverse(o->getOperator()) != e1->getOperator()) {
        return false;
    }

    hif::Operator tmpOp = o->getOperator();
    o->setOperator(e1->getOperator());
    e1->setOperator(tmpOp);

    Value *tmpVal = o->setValue2(cv1_2);
    e1->setValue2(tmpVal);

    return true;
}

bool SimplifyVisitor::_sortBranches(Expression *o)
{
    if (o->getValue2() == nullptr)
        return false;
    Expression *e1 = dynamic_cast<Expression *>(o->getValue1());
    if (e1 == nullptr)
        return false;
    // Check to avoid to undo changes from getRebalancedTree().
    if (dynamic_cast<ConstValue *>(e1->getValue1()))
        return false;

    if (o->getOperator() != op_plus && o->getOperator() != op_minus && o->getOperator() != op_mult &&
        o->getOperator() != op_div) {
        return false;
    }

    if (o->getOperator() != e1->getOperator() && hif::operatorGetInverse(o->getOperator()) != e1->getOperator()) {
        return false;
    }

    Value *b1 = o->getValue2();
    Value *b2 = e1->getValue2();

    int cmp = hif::compare(b1, b2);
    if (cmp >= 0)
        return false;

    e1->setValue2(b1);
    o->setValue2(b2);

    hif::Operator tmpOp = o->getOperator();
    o->setOperator(e1->getOperator());
    e1->setOperator(tmpOp);

    // check expressions are still valid
    Expression *copy    = hif::copy(o);
    const bool isInTree = o->getParent() != nullptr;
    if (isInTree)
        o->replace(copy);
    Type *t = hif::semantics::getSemanticType(copy, _sem);
    if (isInTree)
        copy->replace(o);
    delete copy;

    if (t == nullptr) {
        // restore
        e1->setValue2(b2);
        o->setValue2(b1);

        tmpOp = o->getOperator();
        o->setOperator(e1->getOperator());
        e1->setOperator(tmpOp);

        return false;
    }

    return true;
}

bool SimplifyVisitor::_linearizeExpressionTree(Expression *o)
{
    Expression *e2 = dynamic_cast<Expression *>(o->getValue2());
    if (e2 == nullptr || e2->getValue2() == nullptr)
        return false;
    // only binary

    if (o->getOperator() != op_plus && e2->getOperator() != op_plus && o->getOperator() != op_minus &&
        e2->getOperator() != op_minus && o->getOperator() != op_mult && e2->getOperator() != op_mult &&
        o->getOperator() != op_div && e2->getOperator() != op_div)
        return false;

    ConstValue *e1 = dynamic_cast<ConstValue *>(o->getValue1());
    if (e1 != nullptr) {
        if (!hif::operatorIsCommutative(o->getOperator()))
            return false;

        Value *tmp = o->setValue1(o->getValue2());
        o->setValue2(tmp);
        return true;
    }

    if (o->getOperator() != hif::operatorGetInverse(e2->getOperator()) && o->getOperator() != e2->getOperator())
        return false;

    Operator op1  = o->getOperator();
    Operator op2  = e2->getOperator();
    Value *oLeft  = o->getValue1();
    Value *eLeft  = e2->getValue1();
    Value *eRight = e2->getValue2();

    e2->setValue1(oLeft);
    e2->setValue2(eLeft);
    o->setValue1(e2);
    o->setValue2(eRight);

    // op1=+* op2=+* --> op1=+* op2=+*
    // op1=+* op2=-/ --> op1=-/ op2=+*
    // op1=-/ op2=+* --> op1=-/ op2=-/

    if (op1 == op2) {
        // op_minus has been already pushed
        messageDebugAssert(op1 == op_plus || op1 == op_mult, "Unexpected op (1)", nullptr, _sem);
    } else if (op1 == op_plus || op1 == op_mult) {
        o->setOperator(op2);
        e2->setOperator(op1);
    } else {
        messageDebugAssert(op1 == op_minus || op1 == op_div, "Unexpcted op (2)", nullptr, _sem);
        e2->setOperator(op1);
    }
    return true;
}

bool SimplifyVisitor::_isAlreadySimplified(Expression *e)
{
    std::string key = hif::objectGetKey(e);
    return _expressionKeys.find(key) != _expressionKeys.end();
}
void SimplifyVisitor::_addExpressionKey(const std::string &key) { _expressionKeys.insert(key); }

void SimplifyVisitor::_removeExpressionKey(const std::string &key)
{
    StringSet::iterator i = _expressionKeys.find(key);
    messageAssert(i != _expressionKeys.end(), "Unexpected case", nullptr, nullptr);

    _expressionKeys.erase(i);
}

bool SimplifyVisitor::_doSimplifyExpression(Expression *o)
{
    if (_simplifyOpNone(o)) {
#ifdef DEBUG_EXPR_SCREEN
        std::clog << "[EXPR_" << __expressionId << "] - " << "SIMPLIFIED OP NONE. END VISIT!" << std::endl;
        --__expressionId;
#endif
        return true;
    }

    if (_simplifyOperandCasts(o)) {
#ifdef DEBUG_EXPR_SCREEN
        std::clog << "[EXPR_" << __expressionId << "] - " << "SIMPLIFIED OPERAND CASTS. END VISIT!" << std::endl;
        --__expressionId;
#endif
        return true;
    }

    if (_simplifyDoubleWhen(o)) {
#ifdef DEBUG_EXPR_SCREEN
        std::clog << "[EXPR_" << __expressionId << "] - " << "SIMPLIFIED DOUBLE WHEN. END VISIT!" << std::endl;
        --__expressionId;
#endif
        return true;
    }

    if (_simplifyBitwiseConstants(o)) {
#ifdef DEBUG_EXPR_SCREEN
        std::clog << "[EXPR_" << __expressionId << "] - " << "SIMPLIFIED BITWISE CONSTANTS. END VISIT!" << std::endl;
        --__expressionId;
#endif
        return true;
    }

    if (_simplifyArithBitOperation(o)) {
#ifdef DEBUG_EXPR_SCREEN
        std::clog << "[EXPR_" << __expressionId << "] - " << "SIMPLIFIED ARITHMETIC BIT OPERATION. END VISIT!"
                  << std::endl;
        --__expressionId;
#endif
        return true;
    }

    if (_simplifyMultiplication(o)) {
#ifdef DEBUG_EXPR_SCREEN
        std::clog << "[EXPR_" << __expressionId << "] - " << "SIMPLIFIED MULTIPLICATION. END VISIT!" << std::endl;
        --__expressionId;
#endif
        return true;
    }

    return false;
}

bool SimplifyVisitor::_simplifyOpNone(Expression *o)
{
    if (o->getOperator() != hif::op_none)
        return false;
    messageAssert(o->getValue2() == nullptr, "Unexpected value", o, _sem);
    Value *ret = hif::copy(o->getValue1());
    _replace(o, ret);
    ret->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifyOperandCasts(Expression *o)
{
    Type *originalType = hif::semantics::getSemanticType(o, _sem);
    Cast *castOp1      = dynamic_cast<Cast *>(o->getValue1());
    Cast *castOp2      = dynamic_cast<Cast *>(o->getValue2());

    if (castOp1 == nullptr && castOp2 == nullptr)
        return false;

    // Getting base type of operands
    Type *eT1     = hif::semantics::getSemanticType(o->getValue1(), _sem);
    Type *eT1base = hif::semantics::getBaseType(eT1, false, _sem);
    Type *eT2     = nullptr;
    Type *eT2base = nullptr;
    if (o->getValue2() != nullptr) {
        eT2     = hif::semantics::getSemanticType(o->getValue2(), _sem);
        eT2base = hif::semantics::getBaseType(eT2, false, _sem);
    }

    // Getting base type of operands without casts
    Type *subT1 = eT1base;
    if (castOp1 != nullptr) {
        Type *cValType = hif::semantics::getSemanticType(castOp1->getValue(), _sem);
        subT1          = hif::semantics::getBaseType(cValType, false, _sem);
    }

    Type *subT2 = eT2base;
    if (castOp2 != nullptr) {
        Type *cValType = hif::semantics::getSemanticType(castOp2->getValue(), _sem);
        subT2          = hif::semantics::getBaseType(cValType, false, _sem);
    }

    // TODO: check if truly required and in case enalbe
#if 0
    // Special case:
    // int32(a + 1LL) -1 is no simplified, but we wanto to!
    // so introduce a special case for constants.
    // NOTE: allowed only when trucating!
    // In case, let's consider the constant as of the same type of
    // internal cast expression
    if (hif::operatorIsArithmetic(o->getOperator()) && eT2base != nullptr)
    {
        ConstValue * cv1 = dynamic_cast<ConstValue *>(o->getValue1());
        ConstValue * cv2 = dynamic_cast<ConstValue *>(o->getValue2());
        if (eT1base->getClassId() == subT1->getClassId()
            && castOp1 != nullptr && cv2 != nullptr)
        {
            if (hif::semantics::comparePrecision(eT1base, subT1, _sem)
                == hif::semantics::LESS_PRECISION)
            {
                subT2 = subT1;
            }
        }
        else if (eT2base->getClassId() == subT2->getClassId()
                 && castOp1 != nullptr && cv1 != nullptr)
        {
            if (hif::semantics::comparePrecision(eT2base, subT2, _sem)
                == hif::semantics::LESS_PRECISION)
            {
                subT = subT2;
            }
        }
    }
#endif

    // Checking operation removing all operands casts.
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo simplifiedInfo =
        _sem->getExprType(subT1, subT2, o->getOperator(), o);

    if (simplifiedInfo.returnedType == nullptr) {
        // Cannot type. Try removing only cast on op1.
        simplifiedInfo = _sem->getExprType(subT1, eT2base, o->getOperator(), o);
        if (simplifiedInfo.returnedType != nullptr && castOp1 != nullptr) {
            // Can remove cast on op1.
            castOp2 = nullptr;
            subT2   = eT2base;
        } else {
            // Cannot type again. Try removing only cast on op2.
            simplifiedInfo = _sem->getExprType(eT1base, subT2, o->getOperator(), o);
            if (simplifiedInfo.returnedType != nullptr && castOp2 != nullptr) {
                // Can remove cast on op2.
                castOp1 = nullptr;
                subT1   = eT1base;
            } else {
                // info.returnedType could be != nullptr,
                // but casts cannot be removed.
                delete simplifiedInfo.returnedType;
                simplifiedInfo.returnedType = nullptr;
            }
        }
    }

    // Cannot remove cast, not allowed expression.
    if (simplifiedInfo.returnedType == nullptr)
        return false;

    // Found an expression that is still "valid" without a cast(s).
    // Checking now if is still equivalent.
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo origInfo =
        _sem->getExprType(eT1base, eT2base, o->getOperator(), o);

    bool canRemoveOnShift = false;
    const bool isSafe =
        _sem->canRemoveCastOnOperands(o, origInfo, simplifiedInfo, eT1base, eT2base, subT1, subT2, canRemoveOnShift);

    if (!isSafe) {
        // is not safe. Cannot remove casts.
        return false;
    }

    if (castOp1 == nullptr && castOp2 != nullptr && hif::operatorIsShift(o->getOperator()) && !canRemoveOnShift) {
        // cast only on value 2, but it is a shift and it cannot be removed!
        // ref design: verilog/v2sc/PRIMERUN
        return false;
    }

    // Is safe. Removing casts from original expressions

    if (castOp1 != nullptr) {
        delete o->setValue1(castOp1->setValue(nullptr));
    }

    // ref design: verilog/v2sc/PRIMERUN
    if (castOp2 != nullptr && (!hif::operatorIsShift(o->getOperator()) || canRemoveOnShift)) {
        delete o->setValue2(castOp2->setValue(nullptr));
    }

    bool retIsNotEqual = false;
    Value *ret         = nullptr;

    {
        originalType = hif::copy(originalType);
        hif::semantics::resetTypes(o, false);
        ret           = o;
        Type *retType = hif::semantics::getSemanticType(ret, _sem);
        retIsNotEqual = !hif::equals(retType, originalType);
    }

    if (retIsNotEqual) {
        Cast *nativeCast = new Cast();
        nativeCast->setType(originalType);
        nativeCast->setValue(hif::copy(o));
        _replace(o, nativeCast);
        ret = nativeCast;
    } else {
        delete originalType;
    }

    hif::semantics::getSemanticType(ret, _sem);
    ret->acceptVisitor(*this);

    return true;
}

bool SimplifyVisitor::_simplifyDoubleWhen(Expression *o)
{
    // When1 op Expr -->
    // (When1.alt1 op Expr) else (When1.def op Expr)
    //
    // When1 op When2 --> (if same alts conditions)
    // (When1.alt1 op When2.alt1) else (When1.def op When2.def)
    When *lWhen            = dynamic_cast<When *>(o->getValue1());
    When *rWhen            = dynamic_cast<When *>(o->getValue2());
    const hif::Operator op = o->getOperator();
    if (lWhen == nullptr && rWhen == nullptr)
        return false;

    if (o->getValue2() == nullptr) {
        messageAssert(lWhen != nullptr, "Unexpected case", nullptr, nullptr);
        Type *whenType = hif::semantics::getSemanticType(lWhen, _sem);
        messageAssert(whenType != nullptr, "Cannot type When", lWhen, _sem);

        for (BList<WhenAlt>::iterator it = lWhen->alts.begin(); it != lWhen->alts.end(); ++it) {
            WhenAlt *wa   = *it;
            Expression *e = _factory.expression(op, _factory.cast(hif::copy(whenType), wa->getValue()));
            wa->setValue(e);
        }
        if (lWhen->getDefault() != nullptr) {
            Expression *e = _factory.expression(op, _factory.cast(hif::copy(whenType), lWhen->getDefault()));
            lWhen->setDefault(e);
        }
        o->setValue1(nullptr);
        // ref design: verilog/openCores/or1200_top
        hif::semantics::resetTypes(lWhen, false);
        _replace(o, lWhen);
        lWhen->acceptVisitor(*this);
        return true;
    } else if (lWhen != nullptr && rWhen == nullptr) {
        Type *whenType = hif::semantics::getSemanticType(lWhen, _sem);
        messageAssert(whenType != nullptr, "Cannot type When", lWhen, _sem);

        for (BList<WhenAlt>::iterator it = lWhen->alts.begin(); it != lWhen->alts.end(); ++it) {
            WhenAlt *wa = *it;
            Expression *e =
                _factory.expression(_factory.cast(hif::copy(whenType), wa->getValue()), op, hif::copy(o->getValue2()));
            wa->setValue(e);
        }

        if (lWhen->getDefault() != nullptr) {
            Expression *e = _factory.expression(
                _factory.cast(hif::copy(whenType), lWhen->getDefault()), op, hif::copy(o->getValue2()));
            lWhen->setDefault(e);
        }
        o->setValue1(nullptr);
        // ref design: verilog/openCores/or1200_top
        hif::semantics::resetTypes(lWhen, false);
        _replace(o, lWhen);
        lWhen->acceptVisitor(*this);
        return true;
    } else if (lWhen == nullptr && rWhen != nullptr) {
        Type *whenType = hif::semantics::getSemanticType(rWhen, _sem);
        messageAssert(whenType != nullptr, "Cannot type When", rWhen, _sem);

        for (BList<WhenAlt>::iterator it = rWhen->alts.begin(); it != rWhen->alts.end(); ++it) {
            WhenAlt *wa = *it;
            Expression *e =
                _factory.expression(hif::copy(o->getValue1()), op, _factory.cast(hif::copy(whenType), wa->getValue()));
            wa->setValue(e);
        }

        if (rWhen->getDefault() != nullptr) {
            Expression *e = _factory.expression(
                hif::copy(o->getValue1()), op, _factory.cast(hif::copy(whenType), rWhen->getDefault()));
            rWhen->setDefault(e);
        }
        o->setValue2(nullptr);
        // ref design: verilog/openCores/or1200_top
        hif::semantics::resetTypes(rWhen, false);
        _replace(o, rWhen);
        rWhen->acceptVisitor(*this);
        return true;
    } else if (lWhen != nullptr && rWhen != nullptr) {
        Type *lWhenType = hif::semantics::getSemanticType(lWhen, _sem);
        messageAssert(lWhenType != nullptr, "Cannot type When", lWhen, _sem);
        Type *rWhenType = hif::semantics::getSemanticType(rWhen, _sem);
        messageAssert(rWhenType != nullptr, "Cannot type When", rWhen, _sem);

        if (lWhen->alts.size() != rWhen->alts.size())
            return false;
        BList<WhenAlt>::iterator lit = lWhen->alts.begin();
        BList<WhenAlt>::iterator rit = rWhen->alts.begin();
        for (; lit != lWhen->alts.end(); ++lit, ++rit) {
            WhenAlt *left  = *lit;
            WhenAlt *right = *rit;
            if (!hif::equals(left->getCondition(), right->getCondition()))
                return false;
        }
        const bool leftHasDefault  = lWhen->getDefault() != nullptr;
        const bool rightHasDefault = rWhen->getDefault() != nullptr;
        if (leftHasDefault != rightHasDefault)
            return false;
        lit = lWhen->alts.begin();
        rit = rWhen->alts.begin();
        for (; lit != lWhen->alts.end(); ++lit, ++rit) {
            WhenAlt *left  = *lit;
            WhenAlt *right = *rit;
            Expression *e  = _factory.expression(
                _factory.cast(hif::copy(lWhenType), left->getValue()), op,
                _factory.cast(hif::copy(rWhenType), right->getValue()));
            left->setValue(e);
        }
        if (leftHasDefault) {
            Expression *e = _factory.expression(
                _factory.cast(hif::copy(lWhenType), lWhen->getDefault()), op,
                _factory.cast(hif::copy(rWhenType), rWhen->getDefault()));
            lWhen->setDefault(e);
        }
        o->setValue1(nullptr);
        // ref design: verilog/openCores/or1200_top
        hif::semantics::resetTypes(lWhen, false);
        _replace(o, lWhen);
        lWhen->acceptVisitor(*this);
        return true;
    }
    return false;
}

bool SimplifyVisitor::_simplifyBitwiseConstants(Expression *o)
{
    // (val | -cv1 ) & (cv1) --> cv1
    // (val & -cv1) | (cv1) --> cv1
    Expression *innerExpr = dynamic_cast<Expression *>(o->getValue1());
    if (innerExpr == nullptr)
        return false;
    const Operator op1 = o->getOperator();
    const Operator op2 = innerExpr->getOperator();
    const bool case1   = op1 == hif::op_band && op2 == hif::op_bor;
    const bool case2   = op2 == hif::op_band && op1 == hif::op_bor;
    if (!case1 && !case2)
        return false;

    ConstValue *cv1 = dynamic_cast<ConstValue *>(o->getValue2());
    ConstValue *cv2 = dynamic_cast<ConstValue *>(innerExpr->getValue2());
    if (cv1 == nullptr || cv2 == nullptr)
        return false;

    IntValue *iv1        = dynamic_cast<IntValue *>(cv1);
    IntValue *iv2        = dynamic_cast<IntValue *>(cv2);
    BitvectorValue *bvv1 = dynamic_cast<BitvectorValue *>(cv1);
    BitvectorValue *bvv2 = dynamic_cast<BitvectorValue *>(cv2);

    if (iv1 != nullptr && iv2 != nullptr) {
        if (iv1->getValue() != -(iv2->getValue()))
            return false;
    } else if (bvv1 != nullptr && bvv2 != nullptr) {
        if (!bvv1->is01() || !bvv2->is01())
            return false;
        if (bvv1->getValue().size() > 64 || bvv2->getValue().size() > 64)
            return false;
        const std::uint64_t i1 = bvv1->getValueAsUnsigned();
        const int64_t i2       = bvv2->getValueAsSigned();
        if (i1 != std::uint64_t(-i2))
            return false;
    } else {
        return false;
    }

    Type *exprType = hif::semantics::getSemanticType(o, _sem);
    messageAssert(exprType != nullptr, "Cannot get semantic type of expression", o, _sem);
    Cast *c = _factory.cast(exprType, cv1);
    _replace(o, c);
    c->acceptVisitor(*this);
    return true;
}
bool SimplifyVisitor::_getNestedExpressionOperands(
    Expression *e,
    Value *&v,
    Value *&cv1,
    Value *&cv2,
    Operator &oper1,
    Operator &oper2,
    bool &is_left1,
    bool &is_left2)
{
    if (e->getValue2() == nullptr)
        return false;

    if (_getNestedConstExpressionOperands(e, v, cv1, cv2, oper1, oper2, is_left1, is_left2)) {
        return true;
    }

    if (_getNestedEqualsSubtreesOperands(e, v, cv1, cv2, oper1, oper2, is_left1, is_left2)) {
        return true;
    }

    if (_getNestedSingleConstantOperands(e, v, cv1, cv2, oper1, oper2, is_left1, is_left2)) {
        return true;
    }

    return false;
}

bool SimplifyVisitor::_getNestedConstExpressionOperands(
    Expression *e,
    Value *&v,
    Value *&cv1,
    Value *&cv2,
    Operator &oper1,
    Operator &oper2,
    bool &is_left1,
    bool &is_left2)
{
    if (e->getValue2() == nullptr)
        return false;
    Expression *e1 = dynamic_cast<Expression *>(e->getValue1());
    Expression *e2 = dynamic_cast<Expression *>(e->getValue2());
    cv1            = dynamic_cast<ConstValue *>(e->getValue1());
    cv2            = dynamic_cast<ConstValue *>(e->getValue2());
    if (e1 != nullptr && e2 != nullptr)
        return false;
    if (e1 == nullptr && e2 == nullptr)
        return false;
    if (cv1 == nullptr && cv2 == nullptr)
        return false;

    // set the most external const value on cv1
    if (cv2 != nullptr)
        cv1 = cv2;
    oper1 = e->getOperator();

    if (e1 != nullptr) {
        is_left1            = true;
        ConstValue *cvLeft  = dynamic_cast<ConstValue *>(e1->getValue1());
        ConstValue *cvRight = dynamic_cast<ConstValue *>(e1->getValue2());
        if (cvLeft == nullptr && cvRight == nullptr)
            return false;
        if (cvLeft != nullptr && cvRight != nullptr)
            return false;
        oper2    = e1->getOperator();
        is_left2 = cvLeft == nullptr ? true : false;
        cv2      = is_left2 ? cvRight : cvLeft;
        v        = is_left2 ? e1->getValue1() : e1->getValue2();
    } else // e2 != nullptr
    {
        is_left1            = false;
        ConstValue *cvLeft  = dynamic_cast<ConstValue *>(e2->getValue1());
        ConstValue *cvRight = dynamic_cast<ConstValue *>(e2->getValue2());
        if (cvLeft == nullptr && cvRight == nullptr)
            return false;
        if (cvLeft != nullptr && cvRight != nullptr)
            return false;
        oper2    = e2->getOperator();
        is_left2 = cvLeft == nullptr ? true : false;
        cv2      = is_left2 ? cvRight : cvLeft;
        v        = is_left2 ? e2->getValue1() : e2->getValue2();
    }

    if (hif::operatorIsArithmetic(oper1)) {
        // allowed operators are: plus, minus, mult, div.
        if (oper1 != op_plus && oper1 != op_minus && oper1 != op_mult && oper1 != op_div)
            return false;
        if (oper2 != op_plus && oper2 != op_minus && oper2 != op_mult && oper2 != op_div)
            return false;

        // check compatibility of operands.
        // accepted operation are:
        // plus - plus, plus - minus, minus - plus, minus - minus
        // mult - mult, mult - div, minus - div, minus - div
        if ((oper1 == op_plus || oper1 == op_minus) && (oper2 == op_mult || oper2 == op_div))
            return false;
        if ((oper2 == op_plus || oper2 == op_minus) && (oper1 == op_mult || oper1 == op_div))
            return false;
    } else if (hif::operatorIsBitwise(oper1) || oper1 == hif::op_xor) {
        if (oper1 != oper2)
            return false;
    } else {
        return false;
    }

    // Sanity check to avoid loops: if all are consts, do not rebalance!
    if (dynamic_cast<ConstValue *>(v) != nullptr)
        return false;

    return true;
}
bool SimplifyVisitor::_getNestedEqualsSubtreesOperands(
    Expression *e,
    Value *&v,
    Value *&cv1,
    Value *&cv2,
    Operator &oper1,
    Operator &oper2,
    bool &is_left1,
    bool &is_left2)
{
    // Checking for equal values.
    Expression *e1 = dynamic_cast<Expression *>(e->getValue1());
    Expression *e2 = dynamic_cast<Expression *>(e->getValue2());
    if (e1 == nullptr && e2 == nullptr)
        return false;
    oper1 = e->getOperator();

    bool ok = false;
    if (e2 != nullptr) {
        bool c1  = hif::equals(e->getValue1(), e2->getValue1());
        bool c2  = hif::equals(e->getValue1(), e2->getValue2());
        is_left1 = false;
        oper2    = e2->getOperator();
        if (c1) {
            v        = e2->getValue2();
            is_left2 = false;
            cv1      = e->getValue1();
            cv2      = e2->getValue1();
            ok       = true;
        } else if (c2) {
            v        = e2->getValue1();
            is_left2 = true;
            cv1      = e->getValue1();
            cv2      = e2->getValue2();
            ok       = true;
        }
    }

    // try with other branch
    if (e1 != nullptr && !ok) {
        bool c1  = hif::equals(e1->getValue1(), e->getValue2());
        bool c2  = hif::equals(e1->getValue2(), e->getValue2());
        is_left1 = true;
        oper2    = e1->getOperator();
        if (c1) {
            v        = e1->getValue2();
            is_left2 = false;
            cv1      = e->getValue2();
            cv2      = e1->getValue1();
            ok       = true;
        } else if (c2) {
            v        = e1->getValue1();
            is_left2 = true;
            cv1      = e->getValue2();
            cv2      = e1->getValue2();
            ok       = true;
        }
    }

    if (!ok)
        return false;
    if (hif::operatorIsArithmetic(oper1)) {
        // allowed operators are: plus, minus, mult, div.
        if (oper1 != op_plus && oper1 != op_minus && oper1 != op_mult && oper1 != op_div)
            return false;
        if (oper2 != op_plus && oper2 != op_minus && oper2 != op_mult && oper2 != op_div)
            return false;

        // check compatibility of operands.
        // accepted operation are:
        // plus - plus, plus - minus, minus - plus, minus - minus
        // mult - mult, mult - div, minus - div, minus - div
        if ((oper1 == op_plus || oper1 == op_minus) && (oper2 == op_mult || oper2 == op_div))
            return false;
        if ((oper2 == op_plus || oper2 == op_minus) && (oper1 == op_mult || oper1 == op_div))
            return false;
    } else if (hif::operatorIsBitwise(oper1) || oper1 == hif::op_xor) {
        if (oper1 != oper2)
            return false;
    } else {
        return false;
    }
    // Sanity check to avoid loops: if also the third sub-tree is equals,
    // do not rebalance the tree!
    if (hif::equals(v, cv1))
        return false;

    // Sanity check to avoid loop: if third subtree is an expression with
    // two equal branches, rebalance only if their weight is less
    if (dynamic_cast<Expression *>(v) != nullptr) {
        Expression *thirdExpr        = static_cast<Expression *>(v);
        const bool isAllowedOperator = thirdExpr->getOperator() == op_plus || thirdExpr->getOperator() == op_minus ||
                                       thirdExpr->getOperator() == op_mult || thirdExpr->getOperator() == op_div;
        if (isAllowedOperator) {
            const bool sameBranches = hif::equals(thirdExpr->getValue1(), thirdExpr->getValue2());
            if (sameBranches) {
                const bool isLess = hif::compare(cv1, thirdExpr->getValue1()) < 0;
                if (!isLess)
                    return false;
            }
        }
    }

    return true;
}
bool SimplifyVisitor::_getNestedSingleConstantOperands(
    Expression *e,
    Value *&v,
    Value *&cv1,
    Value *&cv2,
    Operator &oper1,
    Operator &oper2,
    bool &is_left1,
    bool &is_left2)
{
    // Checking for single const object:
    Expression *e1 = dynamic_cast<Expression *>(e->getValue1());
    Expression *e2 = dynamic_cast<Expression *>(e->getValue2());
    if (e1 == nullptr && e2 == nullptr)
        return false;
    oper1 = e->getOperator();
    oper2 = e->getOperator();

    bool ok = false;
    if (e2 != nullptr) {
        const bool c1 = dynamic_cast<ConstValue *>(e2->getValue1()) != nullptr;
        const bool c2 = dynamic_cast<ConstValue *>(e2->getValue2()) != nullptr;
        if (c1 && c2)
            return false; // constants are yet grouped
        is_left1 = false;
        oper2    = e2->getOperator();
        if (c2) {
            v        = e2->getValue2();
            is_left2 = false;
            cv1      = e->getValue1();
            cv2      = e2->getValue1();
            ok       = true;
        } else if (c1) {
            v        = e2->getValue1();
            is_left2 = true;
            cv1      = e->getValue1();
            cv2      = e2->getValue2();
            ok       = true;
        }
    }

    // try with other branch
    if (e1 != nullptr && !ok) {
        const bool c1 = dynamic_cast<ConstValue *>(e1->getValue1()) != nullptr;
        const bool c2 = dynamic_cast<ConstValue *>(e1->getValue2()) != nullptr;
        if (c1 && c2)
            return false; // constants are yet grouped
        is_left1 = true;
        oper2    = e1->getOperator();
        if (c2) {
            v        = e1->getValue2();
            is_left2 = false;
            cv1      = e->getValue2();
            cv2      = e1->getValue1();
            ok       = true;
        } else if (c1) {
            v        = e1->getValue1();
            is_left2 = true;
            cv1      = e->getValue2();
            cv2      = e1->getValue2();
            ok       = true;
        }
    }

    if (!ok)
        return false;
    if (hif::operatorIsArithmetic(oper1)) {
        // allowed operators are: plus, minus, mult, div.
        if (oper1 != op_plus && oper1 != op_minus && oper1 != op_mult && oper1 != op_div)
            return false;
        if (oper2 != op_plus && oper2 != op_minus && oper2 != op_mult && oper2 != op_div)
            return false;

        // check compatibility of operands.
        // accepted operation are:
        // plus - plus, plus - minus, minus - plus, minus - minus
        // mult - mult, mult - div, minus - div, minus - div
        if ((oper1 == op_plus || oper1 == op_minus) && (oper2 == op_mult || oper2 == op_div))
            return false;
        if ((oper2 == op_plus || oper2 == op_minus) && (oper1 == op_mult || oper1 == op_div))
            return false;
    } else if (hif::operatorIsBitwise(oper1) || oper1 == hif::op_xor) {
        if (oper1 != oper2)
            return false;
    } else {
        return false;
    }
    return true;
}

template <typename T> void SimplifyVisitor::_simplifyDeclaration(T * /*o*/)
{
    // if (!_opt.simplify_declarations) return;
    // typename T::DeclarationType * originalDecl =
    //     hif::semantics::getDeclaration(o, _sem, o, false, true);
    // if (originalDecl == nullptr) return;
    //
    // typename T::DeclarationType * decl = hif::copy(originalDecl);
    // const bool can_replace = originalDecl->getParent() != nullptr;
    // if (can_replace) hif::replace(originalDecl, decl);
    // decl->acceptVisitor(*this);
    // if (can_replace) hif::replace(decl, originalDecl);
    //
    // if (hif::equals(decl, originalDecl))
    // {
    //     hif::semantics::setDeclaration(o, originalDecl);
    //     delete decl;
    // }
}

template <typename T> void SimplifyVisitor::_simplifySemanticsType(T *o)
{
    if (_opt.simplify_semantics_types && o->getSemanticType() != nullptr) {
        o->getSemanticType()->acceptVisitor(*this);
    }
}

template <typename T> void SimplifyVisitor::_simplifyBaseType(T *o)
{
    if (!_opt.simplify_semantics_types)
        return;

    if (o->getBaseType(false) != nullptr) {
        o->getBaseType(false)->acceptVisitor(*this);
    }

    if (o->getBaseType(true) != nullptr) {
        o->getBaseType(true)->acceptVisitor(*this);
    }
}

template <typename T> bool SimplifyVisitor::_fixNegativeValue(T *o)
{
    if (o->getValue() >= 0)
        return false;

    Expression *p = dynamic_cast<Expression *>(o->getParent());
    if (p == nullptr)
        return false;

    if (p->getValue2() != o)
        return false;

    if (p->getOperator() != op_plus && p->getOperator() != op_minus)
        return false;

    p->setOperator(hif::operatorGetInverse(p->getOperator()));
    o->setValue(-o->getValue());
    return true;
}

void SimplifyVisitor::_fixSyntacticType(ConstValue *o)
{
    if (needSyntacticType(o))
        return;
    _handleBound(o);
}

void SimplifyVisitor::_fixPortAssignAssignability(PortAssign *o)
{
    // check assignability since in case of generates port binding value type
    // could have changed (ref design: openCoresFunbase/dm9ka_controller)
    Type *paT = hif::semantics::getSemanticType(o, _sem);
    messageAssert(paT != nullptr, "Cannot type port assign", o, _sem);
    // This can be a open bind.
    // Ref design: verilog/trusthub/aes_t100_tj
    if (o->getValue() == nullptr)
        return;
    Type *valType = hif::semantics::getSemanticType(o->getValue(), _sem);
    messageAssert(valType != nullptr, "Cannot type port assign value", o->getValue(), _sem);

    if (_sem->getExprType(paT, valType, op_bind, o).returnedType != nullptr)
        return;

    Cast *c = new Cast();
    c->setType(hif::copy(paT));
    c->setValue(o->setValue(nullptr));
    o->setValue(c);
}

void SimplifyVisitor::_fixStringSpanInformation(String *o)
{
    Range *spanInfo = o->getSpanInformation();

    if (spanInfo == nullptr)
        return;
    if (spanInfo->getDirection() != dir_upto)
        return;

    if (spanInfo->getLeftBound() != nullptr || spanInfo->getRightBound() != nullptr)
        return;

    delete o->setSpanInformation(nullptr);
}

bool SimplifyVisitor::_fixRightHandSideCasts(Assign *o)
{
    if (dynamic_cast<Cast *>(o->getLeftHandSide()) == nullptr)
        return false;

    Value *target = hif::getChildSkippingCasts(o->getLeftHandSide());
    Type *tt      = hif::semantics::getSemanticType(target, _sem);
    Type *ts      = hif::semantics::getSemanticType(o->getRightHandSide(), _sem);

    // Remove cast on target.
    target->replace(nullptr);
    _replace(o->getLeftHandSide(), target); // performs delete

    // Eventually push cast on source.
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo info = _sem->getExprType(tt, ts, op_assign, o);
    if (info.returnedType == nullptr) {
        Cast *c = new Cast();
        c->setType(hif::copy(tt));
        c->setValue(o->getRightHandSide());
        o->setRightHandSide(c);
    }

    return true;
}

bool SimplifyVisitor::_simplifyLeftHandSideCasts(
    Type *targetType,
    Value *source,
    Object *src,
    const bool checkPrecision,
    const hif::Operator oper)
{
    if (source == nullptr)
        return false;

    bool ret = false;

    while (dynamic_cast<Cast *>(source) != nullptr) {
        Cast *c  = static_cast<Cast *>(source);
        Type *ts = hif::semantics::getSemanticType(c->getValue(), _sem);

        if (targetType == nullptr || ts == nullptr) {
            return false;
        }

        // Operation not allowed?
        hif::semantics::ILanguageSemantics::ExpressionTypeInfo info = _sem->getExprType(targetType, ts, oper, src);
        if (info.returnedType == nullptr) {
            // cast is necessary
            return ret;
        }

        if (checkPrecision) {
            hif::semantics::precision_type_t precision = hif::semantics::comparePrecision(c->getType(), ts, _sem);

            // cast is needed to avoid template problems in case of implicit assignments.
            if (precision != hif::semantics::EQUAL_PRECISION) {
                return ret;
            }
        } else {
            // cast is necessary for span/sign extension
            if (!hif::semantics::canRemoveInternalCast(
                    targetType, c->getType(), ts, _sem, src, _opt.behavior == SimplifyOptions::BEHAVIOR_CONSERVATIVE)) {
                return ret;
            }
        }

        // Remove cast.
        Value *op = c->setValue(nullptr);
        c->replace(op);
        delete c;
        source = op;
        ret    = true;
    }

    return ret;
}

bool SimplifyVisitor::_ensureAssignability(DataDeclaration *decl)
{
    if (decl->getValue() == nullptr || decl->getType() == nullptr)
        return false;

    auto valueType = hif::semantics::getSemanticType(decl->getValue(), _sem);
    messageAssert(valueType != nullptr, "Cannot calculate semantic type", decl->getValue(), _sem);
    auto res = _sem->getExprType(decl->getType(), valueType, hif::op_assign, decl);
    if (res.returnedType != nullptr)
        return false;
    decl->setValue(_factory.cast(hif::copy(decl->getType()), decl->setValue(nullptr)));
    decl->getValue()->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_ensureAssignability(Assign *ass)
{
    if (ass->getLeftHandSide() == nullptr || ass->getRightHandSide() == nullptr)
        return false;

    auto targetType = hif::semantics::getSemanticType(ass->getLeftHandSide(), _sem);
    auto valueType  = hif::semantics::getSemanticType(ass->getRightHandSide(), _sem);
    messageAssert(targetType != nullptr, "Cannot calculate semantic type", ass->getLeftHandSide(), _sem);
    messageAssert(valueType != nullptr, "Cannot calculate semantic type", ass->getRightHandSide(), _sem);
    auto res = _sem->getExprType(targetType, valueType, hif::op_assign, ass);
    if (res.returnedType != nullptr)
        return false;
    ass->setRightHandSide(_factory.cast(hif::copy(targetType), ass->setRightHandSide(nullptr)));
    ass->getRightHandSide()->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_removeNullRangeAssign(Assign *o)
{
    Slice *sl = dynamic_cast<Slice *>(o->getLeftHandSide());
    if (sl == nullptr)
        return false;
    if (!_isNullRange(sl->getSpan()))
        return false;
    _trash.insert(o);
    return true;
}

bool SimplifyVisitor::_removeUselessNestedCast(Cast *o)
{
    if (dynamic_cast<Cast *>(o->getValue()) == nullptr)
        return false;

    // the situation is this: T1(T2(T3))
    // where Tx(Ty) represent a cast from type Ty to type Tx.
    // The cast T2 () is useful only if T1 > T2 && T2 < T3 because it
    // loose precision
    // in the case that two precisions are not comparable, we safely
    // maintain the internal cast

    Cast *internalCast = static_cast<Cast *>(o->getValue());
    Type *t1           = o->getType();
    Type *t2           = internalCast->getType();
    Type *t3           = hif::semantics::getSemanticType(internalCast->getValue(), _sem);
    if (t1 == nullptr || t2 == nullptr || t3 == nullptr) {
        messageError("Cannot type operator of internal cast", internalCast->getValue(), _sem);
    }

    if (!hif::semantics::canRemoveInternalCast(
            t1, t2, t3, _sem, o, _opt.behavior == SimplifyOptions::BEHAVIOR_CONSERVATIVE)) {
        return false;
    }

    // Else, remove it.

    o->setValue(internalCast->getValue());
    internalCast->setValue(nullptr);
    delete internalCast;

    return true;
}

bool SimplifyVisitor::_removeSameTypeCast(Cast *o)
{
    // remove useless cast from T to T
    Type *t      = o->getType();
    Type *opType = hif::semantics::getSemanticType(o->getValue(), _sem);

    // Checking same type up to instance (of referenced types) since some semantics
    // type could have no instance when the type declaration is visibile.
    // Ref. design: vhdl/gaisler/can_oc: vendor_library_type
    hif::EqualsOptions eqOpt;
    eqOpt.assureSameSymbolDeclarations = true;
    eqOpt.checkReferencedInstance      = false;
    eqOpt.checkFieldsInitialvalue      = false;
    eqOpt.checkConstexprFlag           = false;
    const bool sameType                = hif::equals(t, opType, eqOpt);

    // can be removed if same base type, AND in fcall/pcall prefix
    Type *tBase              = hif::semantics::getBaseType(t, true, _sem, true);
    Type *opBase             = hif::semantics::getBaseType(opType, true, _sem, true);
    const bool isFcallPrefix = dynamic_cast<FunctionCall *>(o->getParent()) != nullptr &&
                               static_cast<FunctionCall *>(o->getParent())->getInstance() == o;
    const bool isPcallPrefix = dynamic_cast<ProcedureCall *>(o->getParent()) != nullptr &&
                               static_cast<ProcedureCall *>(o->getParent())->getInstance() == o;
    // Ref. design: vhdl/custom/crc cast on ips_xfr_wait assignment, w/o error.
    hif::EqualsOptions eqOpt2;
    eqOpt2.checkTypeVariantField = false;
    const bool samePrefixType    = hif::equals(tBase, opBase, eqOpt2) && (isFcallPrefix || isPcallPrefix);

    if (!sameType && !samePrefixType)
        return false;

    if (dynamic_cast<Aggregate *>(o->getValue()) != nullptr) {
        Aggregate *agg = static_cast<Aggregate *>(o->getValue());
        if (agg->getOthers() != nullptr) {
            Type *otherType        = hif::semantics::getOtherOperandType(o, _sem);
            const bool sameAggType = hif::equals(opType, otherType, eqOpt);
            if (!sameAggType)
                return false;
        }
    }

    Value *v = o->getValue();
    o->setValue(nullptr);
    _replace(o, v);

    return true;
}

bool SimplifyVisitor::_transformCastOfConstValue(Cast *o)
{
    //  if op is a const value, try to call transform constant
    ConstValue *c = dynamic_cast<ConstValue *>(o->getValue());
    if (c == nullptr)
        return false;

    if (!_canTransformConstantInBound(o))
        return false;

    Value *v = hif::manipulation::transformValue(c, o->getType(), _sem, true);
    if (v == nullptr)
        return false;
    // This assures that in case of aggregate, the correct type is preserved.
    // @warning: in case of resetTypes() this is lost! So maybe we should
    // force a syntactic fix...
    if (v->getSemanticType() == nullptr)
        v->setSemanticType(hif::copy(o->getType()));

    _replace(o, v);

    // removing syntactic type if not necessary
    ConstValue *ct = dynamic_cast<ConstValue *>(v);
    if (ct != nullptr && !hif::manipulation::needSyntacticType(ct)) {
        delete ct->setType(nullptr);
    }

    v->acceptVisitor(*this); // fix new const value.
    return true;
}

bool SimplifyVisitor::_canTransformConstantInBound(Cast *o)
{
    ConstValue *c = dynamic_cast<ConstValue *>(o->getValue());
    if (c == nullptr)
        return true;
    if (hif::manipulation::needSyntacticType(o))
        return true;

    // TODO check if declaration should be considered
    Value *defV       = _sem->getTypeDefaultValue(o->getType(), nullptr);
    ConstValue *cDefV = dynamic_cast<ConstValue *>(defV);
    if (cDefV == nullptr) {
        delete defV;
        return true;
    }

    Type *t              = _sem->getTypeForConstant(cDefV);
    Type *allowed        = _sem->isTypeAllowedAsBound(t);
    const bool isAllowed = (allowed == nullptr);

    delete defV;
    delete t;
    delete allowed;

    return isAllowed;
}

bool SimplifyVisitor::_explicitAggregateCast(Cast *o)
{
    Aggregate *agr = dynamic_cast<Aggregate *>(o->getValue());
    if (agr == nullptr)
        return false;

    Array *arr = dynamic_cast<Array *>(hif::semantics::getBaseType(o->getType(), false, _sem));
    if (arr == nullptr)
        return false;

    // ok, fix cast!
    for (BList<AggregateAlt>::iterator it = agr->alts.begin(); it != agr->alts.end(); ++it) {
        Cast *c = new Cast();
        c->setValue((*it)->setValue(c));
        c->setType(hif::copy(arr->getType()));
    }

    if (agr->getOthers() != nullptr) {
        Cast *c = new Cast();
        c->setValue(agr->setOthers(c));
        c->setType(hif::copy(arr->getType()));
    }

    hif::semantics::resetTypes(agr, false);

    // if range are the same, remove cast!
    Array *agrType =
        dynamic_cast<Array *>(hif::semantics::getBaseType(hif::semantics::getSemanticType(agr, _sem), false, _sem));
    if (agrType == nullptr || hif::equals(arr->getSpan(), agrType->getSpan())) {
        o->setValue(nullptr);
        _replace(o, agr); // performs delete
    }

    // fix again aggregate
    agr->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_transformCastToAggregate(Cast * /*c*/)
{
    return false;
#if 0 // DISABLED
    // We have to make this change safer...
    // disabled at the moment
    if (hif::manipulation::isTargetOfAssign(c)) return false;
    Type* bt = hif::semantics::getBaseType(c->getType(), false, _sem);
    if (dynamic_cast<Bitvector*>(bt) == nullptr
        && dynamic_cast<Signed*>(bt) == nullptr
        && dynamic_cast<Unsigned*>(bt) == nullptr) return false;

    Type* t = hif::semantics::getBaseType(
                hif::semantics::getSemanticType(c->getOperator(), _sem),
                false, _sem);
    if (dynamic_cast<Bit*>(t) == nullptr && dynamic_cast<Bool*>(t) == nullptr) return false;

    Range* span = hif::typeGetSpan(c->getType(), _sem);
    unsigned long long size = hif::semantics::spanGetBitwidth(span, _sem);
    Value* lower = hif::rangeGetMinBound(span);

    Bit* bitT = new Bit();
    bitT->SetNative(hif::typeIsConstexpr(bt));
    bitT->SetResolved(hif::typeIsResolved(bt, _sem));
    bitT->SetLogic(hif::typeIsLogic(bt, _sem));

    hif::HifFactory f(_sem);

    BitValue * others = nullptr;
    if (size == 0 || size != 1) others = f.bitval(bit_zero, bitT);

    Cast* aggV = f.cast(hif::copy(bitT), c->SetOp(nullptr));
    Aggregate* aggr = f.aggregate(
                others,
                (
                    f.aggregatealt(hif::copy(lower), aggV)
                    ));

    c->replace(aggr);
    delete c;

    aggr->acceptVisitor(*this);

    return true;
#endif
}

bool SimplifyVisitor::_transformCastFromAggregateToBitvector(Cast *c)
{
    Aggregate *agg = dynamic_cast<Aggregate *>(c->getValue());
    if (agg == nullptr)
        return false;

    const bool res = _simplifyBitvectorAggregate(agg, c->getType());
    if (!res)
        return false;

    Value *bvv = c->setValue(nullptr);
    _replace(c, bvv);
    return true;
}

bool SimplifyVisitor::_transformCastFromBitArrayConcat(Cast *c)
{
    Expression *e = dynamic_cast<Expression *>(c->getValue());
    if (e == nullptr || e->getOperator() != op_concat)
        return false;

    Type *baseType = hif::semantics::getBaseType(c->getType(), false, _sem);
    Bitvector *bv  = dynamic_cast<Bitvector *>(baseType);
    Signed *sig    = dynamic_cast<Signed *>(baseType);
    Unsigned *usig = dynamic_cast<Unsigned *>(baseType);

    if (bv == nullptr && sig == nullptr && usig == nullptr)
        return false;

    Type *eType   = hif::semantics::getSemanticType(e, _sem);
    Array *eArray = dynamic_cast<Array *>(hif::semantics::getBaseType(eType, false, _sem, true));
    if (eArray == nullptr || dynamic_cast<Bit *>(eArray->getType()) == nullptr)
        return false;

    CopyOptions copt;
    copt.copyChildObjects = false;

    Type *eOp1Type = hif::semantics::getSemanticType(e->getValue1(), _sem);
    messageAssert(eOp1Type != nullptr, "Cannot type value 1", e->getValue1(), _sem);
    Type *eOp2Type = hif::semantics::getSemanticType(e->getValue2(), _sem);
    messageAssert(eOp2Type != nullptr, "Cannot type value 2", e->getValue2(), _sem);

    bool changed = false;

    Array *eOp1Base = dynamic_cast<Array *>(hif::semantics::getBaseType(eOp1Type, false, _sem));
    if (eOp1Base != nullptr && dynamic_cast<Bit *>(eOp1Base->getType())) {
        changed   = true;
        Type *bvr = hif::copy(baseType, copt);
        typeSetSpan(bvr, hif::copy(eOp1Base->getSpan()), _sem);
        Cast *cast = new Cast();
        cast->setType(bvr);
        cast->setValue(e->setValue1(cast));
    }

    Array *eOp2Base = dynamic_cast<Array *>(hif::semantics::getBaseType(eOp2Type, false, _sem));
    if (eOp2Base != nullptr && dynamic_cast<Bit *>(eOp2Base->getType())) {
        changed   = true;
        Type *bvr = hif::copy(baseType, copt);
        typeSetSpan(bvr, hif::copy(eOp2Base->getSpan()), _sem);
        Cast *cast = new Cast();
        cast->setType(bvr);
        cast->setValue(e->setValue2(cast));
    }

    if (!changed)
        return false;

    // Assuring the correct new semantic type.
    hif::semantics::resetTypes(e, false);

    c->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_transformAssignFromArrayOfBoolToInt(Assign *o)
{
    const bool ret = _transformCastFromArrayOfBoolToInt(
        o->getRightHandSide(), o->getRightHandSide(), hif::semantics::getSemanticType(o->getLeftHandSide(), _sem),
        false);
    if (!ret)
        return false;
    o->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_transformCastFromArrayOfBoolToInt(Cast *o)
{
    return _transformCastFromArrayOfBoolToInt(o, o->getValue(), hif::semantics::getSemanticType(o, _sem), true);
}

bool SimplifyVisitor::_transformCastFromArrayOfBoolToInt(
    Value *toReplace,
    Value *internalExpr,
    Type *externalType,
    const bool recall)
{
    Type *valueType      = hif::semantics::getSemanticType(internalExpr, _sem);
    Array *valueBaseType = dynamic_cast<Array *>(hif::semantics::getBaseType(valueType, false, _sem));

    if (dynamic_cast<Int *>(hif::semantics::getBaseType(externalType, false, _sem)) == nullptr)
        return false;
    if (valueBaseType == nullptr || (dynamic_cast<Bool *>(valueBaseType->getType()) == nullptr &&
                                     dynamic_cast<Bit *>(valueBaseType->getType()) == nullptr))
        return false;

    long long bw = static_cast<long long>(hif::semantics::typeGetSpanBitwidth(valueBaseType, _sem));
    if (bw == 0)
        return false;

    const bool isDownto = valueBaseType->getSpan()->getDirection() == hif::dir_downto;

    long long firstShift = isDownto ? 0 : (bw - 1);
    Value *expr =
        _factory.cast(hif::copy(externalType), _factory.member(hif::copy(internalExpr), new IntValue(firstShift)));
    for (long long i = 1; i < bw; ++i) {
        long long shift = isDownto ? i : (bw - i - 1);
        Value *tmp      = _factory.expression(
            _factory.cast(hif::copy(externalType), _factory.member(hif::copy(internalExpr), new IntValue(i))),
            hif::op_sll, _factory.intval(shift));

        expr = _factory.expression(tmp, hif::op_bor, expr);
    }

    _replace(toReplace, expr);
    if (recall)
        expr->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifyCastOfVectorMultiplication(Cast *c)
{
    // c_128 = a_64 * b_64
    // If we want to fit variables into integers we need to rewrite
    // this expression in order to compute c[127:64] and c[63:0]
    // An hint could be found in
    // http://stackoverflow.com/questions/1815367/multiplication-of-large-numbers-how-to-catch-overflow

    // But, since this was so brutal we first handle this special case
    // c_64 = cast<64>( a_64 * b_64) -> cast<64>( a[31:0] * b[31:0])
    if (dynamic_cast<Expression *>(c->getValue()) == nullptr)
        return false;
    Expression *e = static_cast<Expression *>(c->getValue());
    if (e->getOperator() != hif::op_mult)
        return false;
    Type *type1 = hif::semantics::getBaseType(e->getValue1(), false, _sem);
    Type *type2 = hif::semantics::getBaseType(e->getValue2(), false, _sem);
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo info;
    info = _sem->getExprType(type1, type2, e->getOperator(), e);
    if (dynamic_cast<Array *>(type1) != nullptr || dynamic_cast<Array *>(type2) != nullptr)
        return false;
    unsigned long long retBw  = hif::semantics::typeGetSpanBitwidth(info.returnedType, _sem);
    unsigned long long precBw = hif::semantics::typeGetSpanBitwidth(info.operationPrecision, _sem);
    if (retBw <= 64 && precBw <= 64)
        return false;

    unsigned long long typeBW = hif::semantics::typeGetSpanBitwidth(c->getType(), _sem);
    if (typeBW == 0 || typeBW > 64)
        return false;

    Type *t1        = hif::semantics::getSemanticType(e->getValue1(), _sem);
    Type *t2        = hif::semantics::getSemanticType(e->getValue2(), _sem);
    long long op1Bw = static_cast<long long>(hif::semantics::typeGetSpanBitwidth(t1, _sem));
    long long op2Bw = static_cast<long long>(hif::semantics::typeGetSpanBitwidth(t2, _sem));
    if (op1Bw == 0 || op2Bw == 0)
        return false;

    if (op1Bw == op2Bw) {
        Range *span = _factory.range(static_cast<long long>(typeBW) / 2 - 1, 0);
        e->setValue1(_factory.slice(e->getValue1(), span));
        e->setValue2(_factory.slice(e->getValue2(), hif::copy(span)));
    } else {
        if (op1Bw > op2Bw) {
            long long lBound = static_cast<long long>(typeBW) - op2Bw - 1;
            Range *rng       = _factory.range(lBound, 0);
            e->setValue1(_factory.slice(e->getValue1(), rng));
        } else {
            long long lBound = static_cast<long long>(typeBW) - op1Bw - 1;
            Range *rng       = _factory.range(lBound, 0);
            e->setValue2(_factory.slice(e->getValue2(), rng));
        }
    }

    hif::semantics::resetTypes(e, false);
    e->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_explicitRecordCast(Cast *o)
{
    RecordValue *recVal = dynamic_cast<RecordValue *>(o->getValue());
    if (recVal == nullptr)
        return false;

    Record *rec = dynamic_cast<Record *>(hif::semantics::getBaseType(o->getType(), false, _sem, false));

    if (rec == nullptr)
        return false;

    // ok, fix cast!

    // TODO call aggregate sort

    if ((rec->fields.size()) < (recVal->alts.size())) {
        messageError("fields and recVal size mismatch", nullptr, _sem);
    }

    BList<Field>::iterator jt          = rec->fields.begin();
    BList<RecordValueAlt>::iterator it = recVal->alts.begin();
    for (; it != recVal->alts.end(); ++it, ++jt) {
        Cast *c = new Cast();
        c->setValue(hif::copy((*it)->getValue()));
        c->setType(hif::copy((*jt)->getType()));
        _replace((*it)->getValue(), c); // performs delete
    }

    o->setValue(nullptr);
    _replace(o, recVal); // performs delete

    // fix again aggregate
    recVal->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_transformCastOfConcat(Cast *c)
{
    // cast<8>(a_8 , b_8) -> cast<8>(b_8)
    Expression *expr = dynamic_cast<Expression *>(c->getValue());
    if (expr == nullptr || expr->getOperator() != hif::op_concat)
        return false;

    hif::semantics::ILanguageSemantics::ExpressionTypeInfo info = _sem->getExprType(
        hif::semantics::getBaseType(expr->getValue1(), false, _sem),
        hif::semantics::getBaseType(expr->getValue2(), false, _sem), expr->getOperator(), expr);
    Type *retType = hif::semantics::getBaseType(info.returnedType, false, _sem);
    if (retType == nullptr)
        return false;

    unsigned long long castbw = 0;
    unsigned long long argbw  = 0;
    Type *castType            = hif::semantics::getBaseType(c->getType(), false, _sem);
    if (dynamic_cast<Array *>(castType) != nullptr) {
        Array *a       = static_cast<Array *>(castType);
        Value *totalbw = hif::semantics::typeGetTotalSpanSize(a, _sem);
        IntValue *ivbw = dynamic_cast<IntValue *>(totalbw);
        if (ivbw == nullptr) {
            delete totalbw;
            return false;
        }
        castbw = static_cast<unsigned long long>(ivbw->getValue());
        delete ivbw;
    } else {
        castbw = hif::semantics::typeGetSpanBitwidth(castType, _sem);
    }

    const bool isStringCast = dynamic_cast<String *>(castType) != nullptr;
    if (dynamic_cast<Array *>(retType) != nullptr) {
        Array *a      = static_cast<Array *>(retType);
        IntValue *tmp = dynamic_cast<IntValue *>(hif::semantics::typeGetTotalSpanSize(a, _sem));
        if (tmp == nullptr)
            return false;
        argbw = static_cast<unsigned long long>(tmp->getValue());
    } else if (isStringCast) {
        // Special case!
        // Cast to string of vectors: it is ok to pushcast on operands:
        if (!hif::semantics::isVectorType(retType, _sem))
            return false;

        expr->setValue1(_factory.cast(_factory.string(), expr->setValue1(nullptr)));
        expr->setValue2(_factory.cast(_factory.string(), expr->setValue2(nullptr)));
        _replace(c, expr);
        expr->acceptVisitor(*this);

        return true;
    } else {
        argbw = hif::semantics::typeGetSpanBitwidth(retType, _sem);
    }
    if (castbw == 0 || argbw == 0 || castbw == argbw)
        return false;

    // Retrive all elements inside concat
    std::vector<Value *> concatElements;
    _getConcatElements(concatElements, expr);
    std::vector<long long> concatBW;
    for (std::vector<Value *>::iterator it = concatElements.begin(); it != concatElements.end(); ++it) {
        Type *t                      = hif::semantics::getSemanticType(*it, _sem);
        unsigned long long elementBw = hif::semantics::typeGetSpanBitwidth(t, _sem);
        if (elementBw == 0)
            return false;
        concatBW.push_back(static_cast<long long>(elementBw));
    }

    // Take only the elements considered by the cast
    unsigned long long totalbw = 0;
    std::vector<Value *> saved;
    std::vector<Value *>::reverse_iterator v_it    = concatElements.rbegin();
    std::vector<long long>::reverse_iterator bw_it = concatBW.rbegin();
    for (; v_it != concatElements.rend() && bw_it != concatBW.rend(); ++v_it, ++bw_it) {
        totalbw += static_cast<unsigned long long>(*bw_it);
        saved.push_back(hif::copy(*v_it));
        if (totalbw >= castbw)
            break;
    }
    if (bw_it == concatBW.rend())
        --bw_it;

    if (totalbw == castbw && saved.size() == concatElements.size())
        return false;

    Value *v    = saved.at(0);
    Value *last = v;
    if (saved.size() > 1) {
        std::vector<long long>::reverse_iterator bw_jt = ++concatBW.rbegin();
        for (std::vector<Value *>::iterator it = ++(saved.begin()); it != saved.end() && bw_jt != concatBW.rend();
             ++it, ++bw_jt) {
            Value *el = *it;

            Range *r          = _factory.range(*bw_jt - 1, 0ll);
            Type *newCastType = hif::copy(retType);
            // ref. design openCores_generic_ahb_arbiter_verilog2hif_ddt
            newCastType->setTypeVariant(hif::Type::NATIVE_TYPE);
            hif::typeSetSpan(newCastType, r, _sem, true);
            Cast *cast = new Cast();
            cast->setType(newCastType);
            cast->setValue(el);
            last = cast;

            v = _factory.expression(cast, hif::op_concat, v);
        }
    }
    if (totalbw > castbw) {
        // Last element is sliced
        unsigned long long max = static_cast<unsigned long long>(*bw_it) - (totalbw - castbw) - 1;
        Type *lastType         = hif::semantics::getBaseType(last, false, _sem);
        Range *lastSpan        = hif::typeGetSpan(lastType, _sem);
        messageAssert(lastSpan != nullptr, "Span not found", lastType, _sem);
        Slice *sl = new Slice();
        sl->setSpan(_factory.range(
            _factory.expression(
                hif::copy(hif::manipulation::assureSyntacticType(hif::rangeGetMinBound(lastSpan), _sem)), hif::op_plus,
                _factory.intval(max)),
            hif::dir_downto, hif::copy(hif::manipulation::assureSyntacticType(hif::rangeGetMinBound(lastSpan), _sem))));
        if (lastSpan->getDirection() == hif::dir_upto)
            sl->getSpan()->swapBounds();
        last->replace(sl);
        sl->setPrefix(last);
    } else if (totalbw < castbw) {
        // Last element must be cast-extended
        unsigned long long max = castbw - totalbw + static_cast<unsigned long long>(*bw_it) - 1;
        Type *lastType         = hif::semantics::getBaseType(last, false, _sem);
        Range *lastSpan        = hif::typeGetSpan(lastType, _sem);
        messageAssert(lastSpan != nullptr, "Span not found", lastType, _sem);
        Range *r = _factory.range(
            _factory.expression(
                hif::copy(hif::manipulation::assureSyntacticType(hif::rangeGetMinBound(lastSpan), _sem)), hif::op_plus,
                _factory.intval(max)),
            hif::dir_downto, hif::copy(hif::manipulation::assureSyntacticType(hif::rangeGetMinBound(lastSpan), _sem)));
        if (lastSpan->getDirection() == hif::dir_upto)
            r->swapBounds();
        Type *newCastType = hif::copy(retType);
        hif::typeSetSpan(newCastType, r, _sem, true);
        Cast *cast = new Cast();
        cast->setType(newCastType);
        last->replace(cast);
        cast->setValue(last);

        if (dynamic_cast<Expression *>(v) != nullptr) {
            Expression *concatExpr   = static_cast<Expression *>(v);
            Type *v2CastType         = hif::copy(newCastType);
            unsigned long long v2Max = totalbw - static_cast<unsigned long long>(*bw_it) - 1;
            Range *v2Span            = _factory.range(v2Max, 0ull);
            hif::typeSetSpan(v2CastType, v2Span, _sem);
            concatExpr->setValue2(_factory.cast(v2CastType, concatExpr->getValue2()));
        }
    }
    delete c->setValue(v);
    c->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_fixCastFromBitvectorToArray(Cast *c)
{
    // cast<Array>(bv_128) ->
    // aggregate:
    // alt 0 bv[63:0]
    // alt 1 bv[127:64]
    Type *castType = hif::semantics::getBaseType(c->getType(), false, _sem);
    if (dynamic_cast<Array *>(castType) == nullptr)
        return false;
    Array *arr = static_cast<Array *>(castType);

    Value *val    = c->getValue();
    Type *valType = hif::semantics::getBaseType(hif::semantics::getSemanticType(val, _sem), false, _sem);
    if (valType == nullptr || dynamic_cast<Bitvector *>(valType) == nullptr)
        return false;

    Bitvector *bv = dynamic_cast<Bitvector *>(valType);
    long long bw  = static_cast<long long>(hif::semantics::spanGetBitwidth(hif::typeGetSpan(bv, _sem), _sem));
    if (bw == 0)
        return false;

    long long elements  = static_cast<long long>(hif::semantics::typeGetSpanBitwidth(arr, _sem));
    long long elementBw = static_cast<long long>(hif::semantics::typeGetSpanBitwidth(arr->getType(), _sem));

    if (elementBw == 0 || elementBw == 1)
        return false;
    if (bw != (elements * elementBw))
        return false;

    const bool bvIsDownto = bv->getSpan()->getDirection() == hif::dir_downto;
    Range *arrElementSpan = hif::typeGetSpan(arr->getType(), _sem);
    if (arrElementSpan == nullptr)
        return false;
    const bool arrIsDownto = arrElementSpan->getDirection() == hif::dir_downto;

    Aggregate *agg = new Aggregate();
    for (long long i = 0; i < elements; ++i) {
        AggregateAlt *alt = new AggregateAlt();

        Range *span = nullptr;
        if (arrIsDownto && bvIsDownto) {
            alt->indices.push_back(new IntValue(i));
            span = _factory.range((i + 1) * elementBw - 1, elementBw * i);
        } else if (arrIsDownto && !bvIsDownto) {
            alt->indices.push_back(new IntValue(elements - i - 1));
            span = _factory.range(elementBw * i, (i + 1) * elementBw - 1);
        } else if (!arrIsDownto && bvIsDownto) {
            alt->indices.push_back(new IntValue(elements - i - 1));
            span = _factory.range((i + 1) * elementBw - 1, elementBw * i);
        } else // !arrIsDownto && !bvIsDownto
        {
            alt->indices.push_back(new IntValue(i));
            span = _factory.range(elementBw * i, (i + 1) * elementBw - 1);
        }

        Value *v  = hif::copy(val);
        Slice *sl = _factory.slice(v, span);
        alt->setValue(sl);
        agg->alts.push_back(alt);
    }

    Cast *cc = _factory.cast(hif::copy(arr), agg);
    _replace(c, cc);
    agg->acceptVisitor(*this);
    return true;
}
void SimplifyVisitor::_removeSwitchUselessCasts(Switch *o)
{
    // Remove eventual useless cast added to the switch alt value(s)
    // checking operation op_eq in inner casts.

    Type *refType = hif::semantics::getSemanticType(o->getCondition(), _sem);
    messageAssert(refType != nullptr, "Cannot type switch value", o->getCondition(), _sem);

    for (BList<SwitchAlt>::iterator i = o->alts.begin(); i != o->alts.end(); ++i) {
        _fixUselessConditionCast((*i)->conditions, refType);
    }
}

bool SimplifyVisitor::_simplifyEmptyWith(With *o)
{
    if (!o->alts.empty())
        return false;
    auto def = o->getDefault();
    messageAssert(def != nullptr, "Unexpected With without default", o, _sem);
    _replaceCaseWithDefault(o);
    delete o;
    def->acceptVisitor(*this);
    return true;
}
void SimplifyVisitor::_removeWithUselessCasts(With *o)
{
    // Remove eventual useless cast added to the switch alt value(s)
    // checking operation op_eq in inner casts.

    Type *refType = hif::semantics::getSemanticType(o->getCondition(), _sem);
    messageAssert(refType != nullptr, "Cannot type with expr", o->getCondition(), _sem);

    for (BList<WithAlt>::iterator i = o->alts.begin(); i != o->alts.end(); ++i) {
        _fixUselessConditionCast((*i)->conditions, refType);
    }
}

void SimplifyVisitor::_fixUselessConditionCast(BList<Value> &conditions, Type *refType)
{
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo res;
    for (BList<Value>::iterator j = conditions.begin(); j != conditions.end(); ++j) {
        Value *current = (*j);

        while (dynamic_cast<Cast *>(current) != nullptr) {
            Cast *co     = static_cast<Cast *>(current);
            Type *opType = hif::semantics::getSemanticType(co->getValue(), _sem);
            messageAssert(opType != nullptr, "Cannot type cast value", co, _sem);

            res = _sem->getExprType(refType, opType, op_eq, co);

            if (res.returnedType == nullptr) {
                // not useless! Nothing else to do.
                break;
            }

            // remove useless cast
            current = co->getValue();
            co->setValue(nullptr);
            co->replace(current);
            delete co;
        }
    }
}

void SimplifyVisitor::_fixNullRange(Range *range)
{
    if (!_isNullRange(range))
        return;

    if (range->getDirection() == dir_downto) {
        delete range->setLeftBound(new IntValue(-1));
        delete range->setRightBound(new IntValue(0));
    } else if (range->getDirection() == dir_upto) {
        delete range->setLeftBound(new IntValue(0));
        delete range->setRightBound(new IntValue(-1));
    }
}

bool SimplifyVisitor::_isNullRange(Range *range)
{
    ConstValue *cv1 = dynamic_cast<ConstValue *>(hif::getChildSkippingCasts(range->getLeftBound()));
    ConstValue *cv2 = dynamic_cast<ConstValue *>(hif::getChildSkippingCasts(range->getRightBound()));

    if (cv1 == nullptr || cv2 == nullptr)
        return false;

    Range r;
    r.setDirection(range->getDirection());
    r.setLeftBound(hif::copy(cv1));
    r.setRightBound(hif::copy(cv2));
    range->replace(&r);
    unsigned long long size = hif::semantics::spanGetBitwidth(&r, _sem, false);
    r.replace(range);

    if (size != 0)
        return false;
    return true;
}

void SimplifyVisitor::_fixBoundType(Value *v)
{
    if (v == nullptr)
        return;
    Range *r = dynamic_cast<Range *>(v);
    if (r != nullptr) {
        _fixBoundType(r->getLeftBound());
        _fixBoundType(r->getRightBound());
        return;
    }

    ConstValue *cv = dynamic_cast<ConstValue *>(v);
    if (cv != nullptr) {
        // Const values must be already fine.
        // Warning: this avoids the following getSemanticType(),
        // which could generate an infinite loop.
        return;
    }

    // Fix of bound type is required since instantiate could simplify casts.
    // E.g.: int<W>(a) is valid in hif as bound,
    // but when simplified it can became int<3>(a) which is no more valid.
    // Ref design: verilog/openCores/top_openMSP430 + hr
    Type *t = hif::semantics::getSemanticType(v, _sem);
    messageAssert(t != nullptr, "Cannot type description", v, _sem);
    Type *good = _sem->isTypeAllowedAsBound(t);
    if (good == nullptr)
        return;
    Cast *c = new Cast();
    c->setType(good);
    v->replace(c);
    c->setValue(v);
    hif::manipulation::assureSyntacticType(v, _sem);
    c->acceptVisitor(*this);
}

void SimplifyVisitor::_handleBound(Value *bound)
{
    ConstValue *cv = dynamic_cast<ConstValue *>(bound);
    if (cv == nullptr) {
        // Not a cost value, nothing to do.
        return;
    }

    Type *boundType = cv->getType();
    if (boundType == nullptr) {
        // Assuming it was already good.
        return;
    }

    // TODO: should be correct, but does not work...
    // if (_sem->isTypeAllowedAsBound(cv->getType()))
    // {
    //     delete cv->setType(nullptr);
    //     return;
    // }

    Type *semType = _sem->getTypeForConstant(cv);
    messageAssert(semType != nullptr, "Cannot getTypeForConstant into semantics.", cv, _sem);

    hif::EqualsOptions opt;
    opt.checkSignedFlag    = false;
    opt.checkConstexprFlag = false;
    if (hif::equals(boundType, semType, opt)) {
        // Removing syntactic type.
        delete semType;
        delete cv->setType(nullptr);
    } else {
        // Setting cv syntactic type to semType
        cv->setType(semType);
        // Adding a cast of cv to boundType.
        Cast *c = new Cast();
        c->setType(boundType);
        bound->replace(c);
        c->setValue(cv);

        // Fix the eventual useless cast
        // ref. design: vhdl/custom/test_span_direction vhdl2hif
        c->acceptVisitor(*this);
    }
}

bool SimplifyVisitor::_simplifyForGenerate(ForGenerate *o)
{
    if (!_opt.simplify_generates)
        return 0;
    if (_isLoopWithControlActions(o))
        return 0;

    // Expanding eventual constants.
    messageDebugAssert(hif::getNearestParent<System>(o->getParent()) != nullptr, "Cannot find system", o, _sem);
    SimplifyOptions optBkp = _opt;

    // ref design: vhdl/openCoresFunbase/hibi_udp
    _opt.simplify_constants                   = true;
    _opt.simplify_template_parameters         = true;
    _opt.simplify_ctc_template_parameters     = true;
    _opt.simplify_non_ctc_template_parameters = true;
    _opt.simplify_generates                   = false;
    _opt.behavior                             = SimplifyOptions::BEHAVIOR_AGGRESSIVE;
    visitList(o->initDeclarations);
    visitList(o->initValues);
    visitList(o->stepActions);
    if (o->getCondition() != nullptr)
        o->getCondition()->acceptVisitor(*this);

    _opt = optBkp;

    // Getting number of iteration
    long long iter = 0;
    long long max  = 0;
    long long min  = 0;
    if (!_resolveForLoopBound(o->initDeclarations, o->initValues, o->getCondition(), o->stepActions, iter, min, max)) {
        messageDebugAssert(!_opt.simplify_generates, "Cannot resolve for generate loop bound", o, _sem);
        return false;
    }

    // support only one initial declaration at the moment.
    if (o->initDeclarations.size() != 1) {
        messageDebugAssert(!_opt.simplify_generates, "Multiple indexes in for generate not handled", o, _sem);
        return false;
    }

    if (iter <= 0) {
        _trash.insert(o);
        return true;
    }

    BaseContents *cnt = hif::getNearestParent<BaseContents>(o);
    messageAssert(cnt != nullptr, "Cannot find parent base contents", o, _sem);

    // Each replied component is marked with a suffix "_i" corresponding
    // to the i-th iteration, and also defines the level of nesting.
    // E.g.
    // for i = 0 to 5
    //      for j = 0 to 1
    // component_0_0
    // component_0_1
    // component_1_0
    // ...
    for (long long i = min; i <= max; ++i) {
        std::stringstream ss;
        ss << "_" << i;
        std::string suffix;
        ss >> suffix;

        // Creates a work copy of the current object.
        ForGenerate *copy = hif::copy(o);
        messageAssert(o->getParent() != nullptr, "parent not found", o, _sem);
        o->replace(copy);

        // Invalid old declarations references. TODO may be lighter
        // using MatchObject?
        hif::semantics::resetDeclarations(copy);

        // Substituting initial value with current iteration value:
        IntValue *intV         = _factory.intval(i);
        DataDeclaration *index = copy->initDeclarations.front();
        delete index->setValue(intV);

        Value *vSub = _makeSubstituible(intV, index->getType());

        // Substituting all references with current iteration value:
        hif::semantics::ReferencesSet refs;
        hif::semantics::getReferences(index, refs, _sem, copy);
        for (hif::semantics::ReferencesSet::iterator j = refs.begin(); j != refs.end(); ++j) {
            (*j)->replace(hif::copy(vSub));
        }

        delete vSub;

        // Calling guide visitor on copy to resolve eventual inner generates:
        GuideVisitor::visitForGenerate(*copy);

        // Expanding for:
        _manageGenerateLocalStuff(copy, suffix, cnt);

        // Restore object for further iterations.
        copy->replace(o);
        delete copy;
    }

    _trash.insert(o);

    return true;
}

bool SimplifyVisitor::_simplifyIfGenerate(IfGenerate *o)
{
    if (!_opt.simplify_generates)
        return false;

    // Expand eventual constants.
    messageDebugAssert(hif::getNearestParent<System>(o) != nullptr, "Cannot find system", o, _sem);

    SimplifyOptions optBkp                    = _opt;
    // ref design: vhdl/openCoresFunbase/hibi_udp
    _opt.simplify_constants                   = true;
    _opt.simplify_template_parameters         = true;
    _opt.simplify_ctc_template_parameters     = true;
    _opt.simplify_non_ctc_template_parameters = true;
    _opt.simplify_generates                   = false;
    _opt.behavior                             = SimplifyOptions::BEHAVIOR_AGGRESSIVE;
    if (o->getCondition() != nullptr)
        o->getCondition()->acceptVisitor(*this);
    _opt = optBkp;

    int boolCond = _resolveBoolCondition(o->getCondition());
    if (boolCond == -1) {
        messageDebugAssert(!_opt.simplify_generates, "Cannot resolve if generate condition", o->getCondition(), _sem);
        return false;
    } else if (boolCond == 0) {
        _trash.insert(o);
    } else // boolCond == 1
    {
        if (!_opt.simplify_generates)
            return 0;

        BaseContents *cnt = hif::getNearestParent<BaseContents>(o);
        messageAssert(cnt != nullptr, "parent base contents not found", o, _sem);

        GuideVisitor::visitIfGenerate(*o);

        _manageGenerateLocalStuff(o, "", cnt);
        _trash.insert(o);
    }

    return true;
}

int SimplifyVisitor::_resolveBoolCondition(Value *condition)
{
    // Condition must be simplified and convertible in boolean value,
    // otherwise do not simplify the statement!
    ConstValue *cv = dynamic_cast<ConstValue *>(condition);
    if (cv == nullptr)
        return -1;

    Bool bb;
    BoolValue *bv = dynamic_cast<BoolValue *>(hif::manipulation::transformConstant(cv, &bb, _sem));
    if (bv == nullptr)
        return -1;

    int ret = (bv->getValue()) ? 1 : 0;
    delete bv;

    return ret;
}

void SimplifyVisitor::_simplifyCondition(Value *condition, Object *o)
{
    Cast *c = dynamic_cast<Cast *>(condition);
    while (c != nullptr) {
        Type *opType = hif::semantics::getSemanticType(c->getValue(), _sem);
        if (opType == nullptr)
            return;
        if (!_sem->checkCondition(opType, o))
            return;

        Value *op = c->getValue();
        c->setValue(nullptr);
        _replace(c, op);

        c = dynamic_cast<Cast *>(op);
    }
}

bool SimplifyVisitor::_resolveForLoopBound(
    BList<DataDeclaration> &initDeclarations,
    BList<Action> &initValues,
    Value *condition,
    BList<Action> &steps,
    long long &loops,
    long long &min,
    long long &max)
{
    if (_opt.simplify_statements) {
        SimplifyOptions localOpts(_opt);
        localOpts.simplify_constants = true;
        simplify(initDeclarations, _sem, localOpts);
        simplify(initValues, _sem, localOpts);
        condition = simplify(condition, _sem, localOpts);
        simplify(steps, _sem, localOpts);
    }

    // Special management if condition is a range.
    if (dynamic_cast<Range *>(condition) != nullptr) {
        return _resolveForLoopBound_rangeCase(condition, loops, min, max);
    }

    // Assuming that loop is based on one index only, otherwise
    // we cannot understand which index is considered for iterate.
    messageDebugAssert(
        initDeclarations.empty() ^ initValues.empty(), "Unexpected for case (1)", initDeclarations.getParent(), _sem);
    if (!initDeclarations.empty() && initDeclarations.size() > 1)
        return false;
    if (!initValues.empty() && initValues.size() > 1)
        return false;

    // Determine index and initial value.
    std::string indexName;
    Value *initVal = nullptr;
    if (!initDeclarations.empty()) {
        indexName = initDeclarations.front()->getName();
        initVal   = initDeclarations.front()->getValue();
    } else {
        Assign *assInit = dynamic_cast<Assign *>(initValues.front());
        messageAssert(assInit != nullptr, "Unexpected for case (2)", initValues.front(), _sem);

        Identifier *index = dynamic_cast<Identifier *>(assInit->getLeftHandSide());
        messageAssert(index != nullptr, "Unexpected for case (3)", assInit, _sem);

        indexName = index->getName();
        initVal   = assInit->getRightHandSide();
    }

    if (indexName.empty() || initVal == nullptr) {
        messageError("Unexpected for case (4)", initValues.front(), _sem);
    }

    // Determine end value.
    Value *nestedCond    = hif::getChildSkippingCasts(condition);
    Expression *exprCond = dynamic_cast<Expression *>(nestedCond);
    if (exprCond == nullptr) {
        // In this case, it can be a fcall.
        // We are currently not able to simplify this.
        return false;
    }
    Value *lastVal = _resolveForLoopBound_getEndValue(condition, indexName, exprCond);
    if (lastVal == nullptr) {
        return false;
    }

    // Determine steps
    Value *increment = _resolveForLoopBound_getSteps(condition, steps, indexName, exprCond);
    messageAssert(increment != nullptr, "Unexpected for case (6)", condition->getParent(), _sem);

    // Calculate number of iterations.
    const bool ret = _resolveForLoopBound_calculateIterations(loops, min, max, initVal, exprCond, lastVal, increment);
    delete increment;
    return ret;
}

bool SimplifyVisitor::_resolveForLoopBound_rangeCase(Value *condition, long long &loops, long long &min, long long &max)
{
    Range *r = static_cast<Range *>(condition);
    loops    = static_cast<long long>(hif::semantics::spanGetBitwidth(r, _sem));
    if (loops == 0)
        return false;
    IntValue *iLeft  = dynamic_cast<IntValue *>(r->getLeftBound());
    IntValue *iRight = dynamic_cast<IntValue *>(r->getRightBound());
    if (r->getDirection() == dir_downto) {
        if (iRight == nullptr)
            return false;
        min = iRight->getValue();
        max = min + loops - 1;
    } else if (r->getDirection() == dir_upto) {
        if (iLeft == nullptr)
            return false;
        min = iLeft->getValue();
        max = min + loops - 1;
    } else
        return false;

    return true;
}
Value *SimplifyVisitor::_resolveForLoopBound_getEndValue(
    Value * /*condition*/,
    const std::string &indexName,
    Expression *exprCond)
{
    Value *lastVal = nullptr;

    Identifier *tgt = dynamic_cast<Identifier *>(hif::getChildSkippingCasts(exprCond->getValue1()));
    if (tgt == nullptr || tgt->getName() != indexName)
        return nullptr;

    lastVal = exprCond->getValue2();
    return lastVal;
}
Value *SimplifyVisitor::_resolveForLoopBound_getSteps(
    Value *condition,
    BList<Action> &steps,
    const std::string &indexName,
    Expression *exprCond)
{
    Value *increment = nullptr;

    hif::HifFactory f;
    f.setSemantics(_sem);

    if (steps.empty()) {
        messageAssert(
            dynamic_cast<ForGenerate *>(condition->getParent()) != nullptr, "Unexpected parent", condition, _sem);
        increment = f.intval(1);
        return increment;
    }

    if (steps.size() > 1) {
        messageError("Unexpected steps objects", nullptr, _sem);
    }

    // Checking that increment step is like: i = i + val
    Assign *ao = dynamic_cast<Assign *>(steps.front());
    messageAssert(ao != nullptr, "Unexpected step object", steps.front(), _sem);

    Identifier *target = dynamic_cast<Identifier *>(hif::getChildSkippingCasts(ao->getLeftHandSide()));
    if (target == nullptr || target->getName() != indexName) {
        messageError("Unexpected assign target", ao->getLeftHandSide(), _sem);
    }

    Expression *expr    = dynamic_cast<Expression *>(hif::getChildSkippingCasts(ao->getRightHandSide()));
    FunctionCall *fcall = dynamic_cast<FunctionCall *>(hif::getChildSkippingCasts(ao->getRightHandSide()));
    if (expr == nullptr && fcall == nullptr) {
        messageError("Unexpected assign source", ao->getRightHandSide(), _sem);
    }

    if (expr != nullptr) {
        Identifier *op1 = dynamic_cast<Identifier *>(hif::getChildSkippingCasts(expr->getValue1()));
        if (op1 == nullptr || op1->getName() != indexName) {
            messageError("Unexpected expr op1", expr, _sem);
        }

        increment = expr->getValue2();

        // TODO check: this should make the expression always typeable
        // Value * current = source;
        // increment = source->getValue2();
        // while(current != nullptr && current->getParent() != ao)
        // {
        //     Cast * cc = dynamic_cast< Cast *>(current);
        //     if (cc == nullptr) break;
        //     Cast * c = new Cast();
        //     c->SetOp(increment);
        //     c->SetType(hif::copy(cc->getType()));
        //     increment = c;
        //     current = dynamic_cast<Value*>(current->getParent());
        // }

        // Sanity checks:
        if (expr->getOperator() == op_plus && (exprCond->getOperator() != op_lt && exprCond->getOperator() != op_le)) {
            return nullptr;
        }
        if (expr->getOperator() == op_minus && (exprCond->getOperator() != op_gt && exprCond->getOperator() != op_ge)) {
            return nullptr;
        }
    }

    return hif::copy(increment);
}

bool SimplifyVisitor::_resolveForLoopBound_calculateIterations(
    long long &loops,
    long long &min,
    long long &max,
    Value *initVal,
    Expression *exprCond,
    Value *lastVal,
    Value *increment)
{
    hif::HifFactory f;
    f.setSemantics(_sem);

    Value *minBound      = nullptr;
    Expression *loopExpr = new Expression();

    // Assuming that a <, <= operation correspond to an increment in step
    // action(s), and vice-versa.
    if (exprCond->getOperator() == op_lt) {
        loopExpr->setValue1(hif::copy(lastVal));
        loopExpr->setValue2(hif::copy(initVal));
        loopExpr->setOperator(op_minus);
        minBound = hif::copy(initVal);
    } else if (exprCond->getOperator() == op_le) {
        Expression *sub = new Expression();
        sub->setValue1(hif::copy(lastVal));
        sub->setValue2(hif::copy(initVal));
        sub->setOperator(op_minus);

        loopExpr->setValue1(sub);
        loopExpr->setValue2(f.intval(1));
        loopExpr->setOperator(op_plus);
        minBound = hif::copy(initVal);
    } else if (exprCond->getOperator() == op_gt) {
        loopExpr->setValue1(hif::copy(initVal));
        loopExpr->setValue2(hif::copy(lastVal));
        loopExpr->setOperator(op_minus);
        minBound = hif::copy(lastVal);
    } else if (exprCond->getOperator() == op_ge) {
        Expression *sub = new Expression();
        sub->setValue1(hif::copy(initVal));
        sub->setValue2(hif::copy(lastVal));
        sub->setOperator(op_minus);

        loopExpr->setValue1(sub);
        loopExpr->setValue2(f.intval(1));
        loopExpr->setOperator(op_plus);
        minBound = hif::copy(lastVal);
    } else {
        //messageDebugAssert(false, "Unexpected expr condition", exprCond, nullptr);
        delete loopExpr;
        return false;
    }

    Expression *e = new Expression();
    e->setValue1(loopExpr);
    e->setValue2(hif::copy(increment));
    e->setOperator(op_div);

    if (hif::semantics::getSemanticType(e, _sem) == nullptr) {
        delete minBound;
        delete e;
        return false;
    }

    SimplifyOptions opt;
    Value *res          = simplify(e, _sem, opt);
    IntValue *ivo       = dynamic_cast<IntValue *>(res);
    BitvectorValue *bvo = dynamic_cast<BitvectorValue *>(res);

    if (bvo != nullptr) {
        Int *intType    = _factory.integer(new Range(63, 0), true, true);
        ConstValue *cvo = hif::manipulation::transformConstant(bvo, intType, _sem);
        delete intType;
        if (cvo != nullptr) {
            ivo = dynamic_cast<IntValue *>(cvo);
            if (ivo == nullptr)
                delete cvo;
        }
        delete bvo;
        res = ivo;
    }

    if (ivo == nullptr) {
        delete minBound;
        delete res;
        return false;
    }
    loops = ivo->getValue();
    delete ivo;

    // Setting min and max:
    Value *resMin          = simplify(minBound, _sem, opt);
    IntValue *ivoMin       = dynamic_cast<IntValue *>(resMin);
    BitvectorValue *bvoMin = dynamic_cast<BitvectorValue *>(resMin);

    if (bvoMin != nullptr) {
        Int *intType    = _factory.integer(new Range(63, 0), true, true);
        ConstValue *cvo = hif::manipulation::transformConstant(bvoMin, intType, _sem);
        delete intType;
        if (cvo != nullptr) {
            ivoMin = dynamic_cast<IntValue *>(cvo);
            if (ivoMin == nullptr)
                delete cvo;
        }
        delete bvoMin;
        resMin = ivoMin;
    }

    if (ivoMin == nullptr) {
        delete resMin;
        return false;
    }

    min = ivoMin->getValue();
    max = min + loops - 1;
    delete ivoMin;

    return true;
}

bool SimplifyVisitor::_isLoopWithControlActions(Object *loop)
{
    hif::HifTypedQuery<Continue> query0;
    hif::HifTypedQuery<Break> query1;
    query0.setNextQueryType(&query1);
    typedef hif::HifTypedQuery<Object>::Results Results;
    Results results;
    hif::search(results, loop, query0);
    for (Results::iterator it = results.begin(); it != results.end(); ++it) {
        Object *obj   = *it;
        Object *scope = hif::getMatchingScope(obj);
        if (loop == scope)
            return true;
    }
    return false;
}

void SimplifyVisitor::_manageGenerateLocalStuff(Generate *o, std::string suffix, BaseContents *newLocation)
{
    messageAssert(newLocation != nullptr, "Unexpected nullptr new location", nullptr, _sem);

    // Instances
    // This is necessary (before moving list "declarations") since the
    // instance declaration could be somewhere else.
    moveToScope(o, newLocation, _sem, suffix, o->instances);

    // Declarations and stateTables
    moveToScope(o, newLocation, _sem, suffix, o->declarations);
    moveToScope(o, newLocation, _sem, suffix, o->stateTables);

    // Global action
    // TODO search the name of instances inside global actions and replace
    // them with the new ones. Is it necessary? May use a map to collect
    // them during fix of instances.
    if (o->getGlobalAction() != nullptr) {
        moveToScope(o, newLocation, _sem, suffix, o->getGlobalAction());
    }
}

bool SimplifyVisitor::_simplifySliceToMember(Slice *o)
{
    Range *span = o->getSpan();
    if (!hif::equals(span->getLeftBound(), span->getRightBound()))
        return false;

    Type *t = hif::semantics::getSemanticType(o, _sem);
    messageAssert(t != nullptr, "Cannot type slice.", o, _sem);

    Member *m = new Member();
    m->setPrefix(o->setPrefix(nullptr));
    m->setIndex(span->setLeftBound(nullptr));

    Cast *c = new Cast();
    c->setValue(m);
    c->setType(hif::copy(t));

    _replace(o, c);
    c->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifySliceConstantPrefix(Slice *o)
{
    // it may be a name simplified.
    IntValue *il = _getIndex(o->getSpan()->getLeftBound());
    IntValue *ir = _getIndex(o->getSpan()->getRightBound());
    if (il == nullptr || ir == nullptr) {
        delete il;
        delete ir;
        return false;
    }

    BitvectorValue *bvPrefix = dynamic_cast<BitvectorValue *>(o->getPrefix());
    Aggregate *agPrefix      = dynamic_cast<Aggregate *>(o->getPrefix());

    if (bvPrefix != nullptr) {
        // example:
        // - before simplify names: n = sc_lv("01zx0"); n.range(2,4);
        // - after simplify: sc_lv("01zx0").range(2,4)
        // - after this simplify: zx0.
        Expression *expr = nullptr;
        if (o->getSpan()->getDirection() == dir_downto) {
            expr = _factory.expression(
                hif::copy(bvPrefix), op_sra, hif::manipulation::assureSyntacticType(hif::copy(ir), _sem));
        } else if (o->getSpan()->getDirection() == dir_upto) {
            expr = _factory.expression(
                hif::copy(bvPrefix), op_sla, hif::manipulation::assureSyntacticType(hif::copy(il), _sem));
        }
        Cast *c = _factory.cast(hif::copy(hif::semantics::getSemanticType(o, _sem)), expr);

        _replace(o, c);
        c->acceptVisitor(*this);
    } else if (agPrefix != nullptr) {
        // TODO: not implemented yet
        delete il;
        delete ir;
        return false;
    } else {
        delete il;
        delete ir;
        return false;
    }

    delete il;
    delete ir;
    return true;
}

bool SimplifyVisitor::_simplifyUselessSlice(Slice *o)
{
    Value *prefix = o->getPrefix();

    Type *prefixType = hif::semantics::getBaseType(hif::semantics::getSemanticType(prefix, _sem), false, _sem);
    messageAssert(prefixType != nullptr, "Cannot type prefix", prefix, _sem);

    Value *typeSpan  = hif::semantics::typeGetSpanSize(prefixType, _sem);
    Value *sliceSpan = hif::semantics::spanGetSize(o->getSpan(), _sem);

    const bool equalsSpanSize = hif::equals(typeSpan, sliceSpan);
    delete typeSpan;
    delete sliceSpan;
    if (!equalsSpanSize)
        return false;

    o->setPrefix(nullptr);
    _replace(o, prefix);

    return true;
}

bool SimplifyVisitor::_simplifyIntSlice(Slice *o)
{
    hif::manipulation::LeftHandSideOptions lopt;
    lopt.sem = _sem;
    if (hif::manipulation::isInLeftHandSide(o, lopt))
        return false;
    Value *prefix   = o->getPrefix();
    Type *sliceType = hif::semantics::getSemanticType(prefix, _sem);
    messageAssert(sliceType != nullptr, "Cannot type slice", o, _sem);

    Type *prefixType = hif::semantics::getBaseType(hif::semantics::getSemanticType(prefix, _sem), false, _sem);
    messageAssert(prefixType != nullptr, "Cannot type prefix", prefix, _sem);

    Int *intType = dynamic_cast<Int *>(prefixType);
    if (intType == nullptr)
        return false;

    if (o->getSpan()->getDirection() != hif::dir_downto) {
        messageError("Unexpected span direction for integer slice", o, _sem);
    }

    // Translating: e.slice(l downto r) --> int<l, r>(e >> r)
    Value *min = hif::rangeGetMinBound(o->getSpan());
    min->replace(nullptr);
    Value *v = _factory.cast(
        hif::copy(sliceType),
        _factory.expression(o->setPrefix(nullptr), hif::op_sra, hif::manipulation::assureSyntacticType(min, _sem)));
    _replace(o, v);
    v->acceptVisitor(*this);

    return true;
}

bool SimplifyVisitor::_simplifyConstantConcatSlice(Slice *o)
{
    // (a_64,b_64,c_64)[100:0] -> b_64[35:0],c_64
    Expression *prefix = dynamic_cast<Expression *>(o->getPrefix());
    if (prefix == nullptr)
        return false;
    if (prefix->getOperator() != op_concat)
        return false;
    Type *prefixType = hif::semantics::getSemanticType(prefix, _sem);
    if (prefixType == nullptr)
        return false;
    Range *prefixSpan = hif::typeGetSpan(prefixType, _sem);
    if (prefixSpan == nullptr)
        return false;
    const bool isPrefixDownto = prefixSpan->getDirection() == hif::dir_downto;
    IntValue *minBoundVal     = dynamic_cast<IntValue *>(hif::rangeGetMinBound(prefixSpan));
    if (minBoundVal == nullptr)
        return false;
    long long minBound = minBoundVal->getValue();

    std::vector<Value *> concatElements;
    _getConcatElements(concatElements, prefix);
    std::vector<long long> concatBW;
    for (std::vector<Value *>::iterator it = concatElements.begin(); it != concatElements.end(); ++it) {
        Type *t                      = hif::semantics::getSemanticType(*it, _sem);
        unsigned long long elementBw = hif::semantics::typeGetSpanBitwidth(t, _sem);
        if (elementBw == 0)
            return false;
        concatBW.push_back(static_cast<long long>(elementBw));
    }

    Range *span          = o->getSpan();
    IntValue *rightBound = dynamic_cast<IntValue *>(span->getRightBound());
    IntValue *leftBound  = dynamic_cast<IntValue *>(span->getLeftBound());
    if (rightBound == nullptr || leftBound == nullptr)
        return false;
    long long rBound = rightBound->getValue() - minBound;
    long long lBound = leftBound->getValue() - minBound;
    std::vector<Value *> toConcat;
    long long remainder = 0;

    if (isPrefixDownto) {
        std::vector<Value *>::reverse_iterator v_it    = concatElements.rbegin();
        std::vector<long long>::reverse_iterator bw_it = concatBW.rbegin();
        for (; v_it != concatElements.rend() && bw_it != concatBW.rend(); ++v_it, ++bw_it) {
            if (lBound < 0)
                break;
            if (rBound < *bw_it) {
                long long newLBound = lBound >= *bw_it ? *bw_it - 1 : lBound;
                if (newLBound - rBound + 1 == *bw_it) {
                    // Covers all the Value's bits
                    toConcat.push_back(hif::copy(*v_it));
                } else {
                    Range *rng = _factory.range(newLBound, rBound);
                    Slice *s   = _factory.slice(hif::copy(*v_it), rng);
                    toConcat.push_back(s);
                }
            }
            rBound = rBound - *bw_it;
            if (rBound < 0)
                rBound = 0;
            lBound = lBound - *bw_it;
        }
        remainder = lBound + 1;
    } else {
        std::vector<Value *>::iterator v_it    = concatElements.begin();
        std::vector<long long>::iterator bw_it = concatBW.begin();
        for (; v_it != concatElements.end() && bw_it != concatBW.end(); ++v_it, ++bw_it) {
            if (rBound < 0)
                break;
            if (lBound < *bw_it) {
                long long newRBound = rBound >= *bw_it ? *bw_it - 1 : rBound;
                if (newRBound - lBound + 1 == *bw_it) {
                    // Covers all the Value's bits
                    toConcat.push_back(hif::copy(*v_it));
                } else {
                    Range *rng = _factory.range(newRBound, lBound);
                    Slice *s   = _factory.slice(hif::copy(*v_it), rng);
                    toConcat.push_back(s);
                }
            }
            lBound = lBound - *bw_it;
            if (lBound < 0)
                lBound = 0;
            rBound = rBound - *bw_it;
        }
        remainder = rBound + 1;
    }

    if (remainder > 0) {
        // if remainder is not zero we have a slice greater than actual bits
        // This is fine in Verilog since it extends with X
        if (!hif::semantics::isVectorType(prefixType, _sem)) {
            messageError("Unsupported case", prefixType, _sem);
        }
        BitvectorValue *bvv = _factory.bitvectorval(
            std::string(std::string::size_type(remainder), 'X'), _factory.bitvector(new Range(remainder - 1, 0)));
        toConcat.push_back(bvv);
    }

    if (toConcat.empty()) {
        // If toConcat is empty then the slice is outside
        // the span of prefix. This is fine in Verilog designs
        // and returns all X
        // ref. design mios_ChkSlaveFrame_verilog2hif_ddt
        if (!hif::semantics::isVectorType(prefixType, _sem)) {
            messageError("Unsupported case", prefixType, _sem);
        }
        long long bw;
        if (isPrefixDownto)
            bw = leftBound->getValue() - rightBound->getValue() + 1;
        else
            bw = rightBound->getValue() - leftBound->getValue() + 1;
        BitvectorValue *bvv = _factory.bitvectorval(
            std::string(std::string::size_type(bw), 'X'), _factory.bitvector(new Range(bw - 1, 0)));
        Cast *c = _factory.cast(hif::copy(hif::semantics::getSemanticType(o, _sem)), bvv);
        _replace(o, c);
        c->acceptVisitor(*this);
        return true;
    } else if (toConcat.size() == 1) {
        Value *ret = toConcat.front();
        o->replace(ret);
        delete o;
        return true;
    } else if (toConcat.size() == 2) {
        Expression *ret = _factory.expression(toConcat.back(), op_concat, toConcat.front());
        o->replace(ret);
        delete o;
        return true;
    }
    Expression *ret = _factory.expression(toConcat[1], op_concat, toConcat[0]);
    for (unsigned int i = 2; i < toConcat.size(); ++i) {
        Expression *expr = _factory.expression(toConcat[i], op_concat, ret);
        ret              = expr;
    }
    o->replace(ret);
    delete o;
    return true;
}

bool SimplifyVisitor::_simplifyNonConstantConcatSlice(Slice *o)
{
    Expression *expr = dynamic_cast<Expression *>(o->getPrefix());
    if (expr == nullptr || expr->getOperator() != hif::op_concat)
        return false;

    // (a, b)[l, r] -->
    // when l <= |b| -1 then
    //    b[l, r]
    // elsewhen r >= |b|
    //    a[l - |b|, r - |b|]
    // else
    //    a[l - |b|, 0], b[|b|-1, r]
    // checks:
    //  - right bound always zero

    Type *exprType      = hif::semantics::getSemanticType(expr, _sem);
    Value *exprMinBound = hif::rangeGetMinBound(hif::typeGetSpan(exprType, _sem));
    if (dynamic_cast<IntValue *>(exprMinBound) == nullptr || static_cast<IntValue *>(exprMinBound)->getValue() != 0ll)
        return false;

    Value *a         = expr->getValue1();
    Value *b         = expr->getValue2();
    Type *aType      = hif::semantics::getSemanticType(a, _sem);
    Type *bType      = hif::semantics::getSemanticType(b, _sem);
    Range *aSpan     = hif::typeGetSpan(aType, _sem);
    Range *bSpan     = hif::typeGetSpan(bType, _sem);
    Value *aMinBound = hif::rangeGetMinBound(aSpan);
    Value *bMinBound = hif::rangeGetMinBound(bSpan);
    IntValue *aiv    = dynamic_cast<IntValue *>(aMinBound);
    messageAssert(aiv != nullptr, "Unable to compute bound", a, _sem);
    IntValue *biv = dynamic_cast<IntValue *>(bMinBound);
    messageAssert(biv != nullptr, "Unable to compute bound", b, _sem);

    if (aiv == nullptr || biv == nullptr)
        return false;

    Range *span  = o->getSpan();
    Value *bSize = hif::semantics::typeGetSpanSize(bType, _sem);
    if (bSize == nullptr)
        return false;
    Type *sliceType = hif::semantics::getSemanticType(o, _sem);
    When *ret       = new When();
    WhenAlt *wa0    = new WhenAlt();
    ret->alts.push_back(wa0);
    wa0->setCondition(_factory.expression(
        hif::manipulation::assureSyntacticType(hif::copy(span->getLeftBound()), _sem), hif::op_le,
        _factory.expression(bSize, hif::op_minus, _factory.intval(1))));
    wa0->setValue(_factory.cast(hif::copy(sliceType), _factory.slice(hif::copy(b), hif::copy(span))));
    WhenAlt *wa1 = new WhenAlt();
    ret->alts.push_back(wa1);
    wa1->setCondition(_factory.expression(
        hif::manipulation::assureSyntacticType(hif::copy(span->getRightBound()), _sem), hif::op_ge, hif::copy(bSize)));
    wa1->setValue(_factory.cast(
        hif::copy(sliceType),
        _factory.slice(
            hif::copy(a), _factory.range(
                              _factory.expression(
                                  hif::manipulation::assureSyntacticType(hif::copy(span->getLeftBound()), _sem),
                                  hif::op_minus, hif::copy(bSize)),
                              hif::dir_downto,
                              _factory.expression(
                                  hif::manipulation::assureSyntacticType(hif::copy(span->getRightBound()), _sem),
                                  hif::op_minus, hif::copy(bSize))))));
    ret->setDefault(_factory.cast(
        hif::copy(sliceType),
        _factory.expression(
            _factory.slice(
                hif::copy(a), _factory.range(
                                  _factory.expression(
                                      hif::manipulation::assureSyntacticType(hif::copy(span->getLeftBound()), _sem),
                                      hif::op_minus, hif::copy(bSize)),
                                  hif::dir_downto, new IntValue(0))),
            hif::op_concat,
            _factory.slice(
                hif::copy(b),
                _factory.range(
                    _factory.expression(hif::copy(bSize), hif::op_minus, _factory.intval(1)), hif::dir_downto,
                    hif::manipulation::assureSyntacticType(hif::copy(span->getRightBound()), _sem))))));
    _replace(o, ret);
    ret->acceptVisitor(*this);
    return true;
}

void SimplifyVisitor::_getConcatElements(std::vector<Value *> &elements, Expression *expr)
{
    if (expr->getOperator() != hif::op_concat) {
        elements.push_back(expr);
        return;
    }

    Value *v1 = expr->getValue1();
    Type *t1  = hif::semantics::getBaseType(hif::semantics::getSemanticType(v1, _sem), false, _sem);
    Value *v2 = expr->getValue2();
    Type *t2  = hif::semantics::getBaseType(hif::semantics::getSemanticType(v2, _sem), false, _sem);

    if (dynamic_cast<Expression *>(v1) == nullptr) {
        if (dynamic_cast<Array *>(t1) == nullptr) {
            elements.push_back(v1);
        } else {
            Array *a       = static_cast<Array *>(t1);
            long long size = static_cast<long long>(hif::semantics::typeGetSpanBitwidth(a, _sem));
            for (long long i = 0; i < size; ++i) {
                Member *mem = _factory.member(hif::copy(v1), new IntValue(i));
                elements.push_back(mem);
            }
        }
    } else {
        Expression *e1 = static_cast<Expression *>(v1);
        _getConcatElements(elements, e1);
    }

    if (dynamic_cast<Expression *>(v2) == nullptr) {
        if (dynamic_cast<Array *>(t2) == nullptr) {
            elements.push_back(v2);
        } else {
            Array *a       = static_cast<Array *>(t2);
            long long size = static_cast<long long>(hif::semantics::typeGetSpanBitwidth(a, _sem));
            for (long long i = 0; i < size; ++i) {
                Member *mem = _factory.member(hif::copy(v2), new IntValue(i));
                elements.push_back(mem);
            }
        }
    } else {
        Expression *e2 = static_cast<Expression *>(v2);
        _getConcatElements(elements, e2);
    }
}

bool SimplifyVisitor::_simplifyBitwiseExpressionSlice(Slice *o)
{
    // (a_128 & b_128)[63:0] -> (a_128[63:0] & b_128[63:0])
    Value *prefix = o->getPrefix();
    if (dynamic_cast<Expression *>(prefix) == nullptr)
        return false;
    Expression *expr = static_cast<Expression *>(prefix);
    if (!hif::operatorIsBitwise(expr->getOperator()))
        return false;

    Type *origType = hif::semantics::getSemanticType(o, _sem);
    if (origType == nullptr)
        return false;

    Range *rng   = o->getSpan();
    Slice *newV1 = _factory.slice(expr->getValue1(), rng);
    expr->setValue1(newV1);
    if (expr->getValue2() != nullptr) {
        Slice *newV2 = _factory.slice(expr->getValue2(), hif::copy(rng));
        expr->setValue2(newV2);
    }
    Value *newValue = expr;
    hif::semantics::resetTypes(newValue, false);
    o->replace(newValue);
    Type *newType = hif::semantics::getSemanticType(newValue, _sem);
    messageAssert(newType != nullptr, "Cannot type value", newType, _sem);
    if (!hif::equals(origType, newType)) {
        Cast *c = new Cast();
        c->setType(hif::copy(origType));
        newValue->replace(c);
        c->setValue(newValue);
        newValue = c;
    }
    delete o;
    newValue->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifyArithExpressionSlice(Slice *o)
{
    // (a + b)[l, r] -->
    // a[l, r] + b[l, r] + carry
    // where carry = ((b[r,0] > 0 && a[r,0] > INT_MAX - b[r,0])
    //              ||(b[r,0] < 0 && a[r,0] < INT_MIN - b[r,0]))
    // In case of unsigned: carry = a[r,0] > UINT_MAX - b[r,0]
    Value *prefix = o->getPrefix();
    if (dynamic_cast<Expression *>(prefix) == nullptr)
        return false;
    Expression *expr = static_cast<Expression *>(prefix);
    if (expr->getOperator() != hif::op_plus || expr->getValue2() == nullptr)
        return false;

    Type *type     = hif::semantics::getSemanticType(o, _sem);
    Type *baseType = hif::semantics::getBaseType(type, false, _sem);
    if (!hif::semantics::isVectorType(baseType, _sem))
        return false;

    Value *value1 = expr->getValue1();
    Value *value2 = expr->getValue2();

    Type *type1 = hif::semantics::getSemanticType(value1, _sem);
    Type *type2 = hif::semantics::getSemanticType(value2, _sem);
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo info =
        _sem->getExprType(type1, type2, expr->getOperator(), expr);
    if (hif::typeIsSigned(info.operationPrecision, _sem))
        return false;

    expr->setValue1(_factory.slice(_factory.cast(hif::copy(info.operationPrecision), value1), hif::copy(o->getSpan())));

    expr->setValue2(_factory.slice(_factory.cast(hif::copy(info.operationPrecision), value2), hif::copy(o->getSpan())));
    hif::semantics::resetTypes(expr, false);

    Value *minBound = hif::rangeGetMinBound(o->getSpan());
    if (dynamic_cast<IntValue *>(minBound) != nullptr && static_cast<IntValue *>(minBound)->getValue() == 0) {
        // no carry:
        _replace(o, expr);
        expr->acceptVisitor(*this);
        return true;
    }

    Range *r_0 = _factory.range(
        _factory.expression(
            hif::manipulation::assureSyntacticType(hif::copy(minBound), _sem), hif::op_minus, _factory.intval(1)),
        hif::dir_downto, new IntValue(0));
    if (o->getSpan()->getDirection() == hif::dir_upto)
        r_0->swapBounds();

    Value *carry = _factory.expression(
        _factory.slice(_factory.cast(hif::copy(info.operationPrecision), hif::copy(value1)), r_0), hif::op_gt,
        _factory.expression(
            _factory.slice(
                _factory.expression(
                    hif::op_bnot, _factory.cast(hif::copy(info.operationPrecision), _factory.intval(0))),
                hif::copy(r_0)),
            hif::op_minus,
            _factory.slice(_factory.cast(hif::copy(info.operationPrecision), hif::copy(value2)), hif::copy(r_0))));

    Expression *newExpr = _factory.expression(expr, hif::op_plus, _factory.cast(hif::copy(type), carry));

    _replace(o, newExpr);
    newExpr->acceptVisitor(*this);

    return true;
}
bool SimplifyVisitor::_pushSliceIntoCast(Slice *o)
{
    // ((cast_16)val)[9:0] -> (cast_10)(val[9:0])
    Cast *prefix = dynamic_cast<Cast *>(o->getPrefix());
    if (prefix == nullptr)
        return false;
    Value *value    = prefix->getValue();
    Type *valueType = hif::semantics::getBaseType(value, false, _sem);
    if (!hif::semantics::isVectorType(valueType, _sem) && dynamic_cast<Array *>(valueType) == nullptr)
        return false;
    Type *sliceType = hif::semantics::getBaseType(o, false, _sem);

    IntValue *castMinBound = dynamic_cast<IntValue *>(hif::rangeGetMinBound(hif::typeGetSpan(prefix->getType(), _sem)));
    if (castMinBound == nullptr || castMinBound->getValue() != 0)
        return false;
    IntValue *valueMinBound = dynamic_cast<IntValue *>(hif::rangeGetMinBound(hif::typeGetSpan(valueType, _sem)));
    if (valueMinBound == nullptr || valueMinBound->getValue() != 0)
        return false;
    IntValue *valueMaxBound = dynamic_cast<IntValue *>(hif::rangeGetMaxBound(hif::typeGetSpan(valueType, _sem)));
    if (valueMaxBound == nullptr)
        return false;
    IntValue *sliceMaxBound = dynamic_cast<IntValue *>(hif::rangeGetMaxBound(o->getSpan()));
    if (sliceMaxBound == nullptr)
        return false;
    IntValue *sliceMinBound = dynamic_cast<IntValue *>(hif::rangeGetMinBound(o->getSpan()));
    if (sliceMinBound == nullptr)
        return false;

    const bool valueIsSigned = hif::typeIsSigned(valueType, _sem);

    if (!valueIsSigned) {
        if (valueMaxBound->getValue() < sliceMinBound->getValue()) {
            // we are taking just the xtension part of an unsigned extension
            // --> all zeros!
            Value *res = _factory.cast(hif::copy(sliceType), _factory.intval(0));
            _replace(o, res);
            res->acceptVisitor(*this);
            return true;
        }
        // we are taking some some bits of value
        if (sliceMaxBound->getValue() > valueMaxBound->getValue()) {
            // taking also some zeros of extension: just reduce the slice!
            Range *rng = hif::typeGetSpan(sliceType, _sem);
            if (rng == nullptr)
                return false;
            hif::typeSetSpan(prefix->getType(), hif::copy(rng), _sem);
            o->replace(prefix);
            o->setPrefix(value);
            sliceMaxBound->replace(hif::copy(valueMaxBound));
            delete sliceMaxBound;
            prefix->setValue(o);
            hif::semantics::resetTypes(o, false);
            hif::semantics::resetTypes(prefix, false);
            prefix->acceptVisitor(*this);
            return true;
        }

        // all bits inside the value
        Range *rng = hif::typeGetSpan(sliceType, _sem);
        if (rng == nullptr)
            return false;
        hif::typeSetSpan(prefix->getType(), hif::copy(rng), _sem);
        o->replace(prefix);
        o->setPrefix(value);
        prefix->setValue(o);
        hif::semantics::resetTypes(o, false);
        hif::semantics::resetTypes(prefix, false);
        prefix->acceptVisitor(*this);
        return true;
    }

    // case with sign!
    if (valueMaxBound->getValue() < sliceMinBound->getValue()) {
        // we are taking just the xtension part of a signed extension
        // --> all zeros or ones!
        Member *sign     = _factory.member(hif::copy(value), hif::copy(valueMaxBound));
        // Before going on, we must be usre that concat on single bits is available:
        // ref design: vhdl/ips/mephisto_core
        Type *memberType = _sem->getMemberSemanticType(sign);
        if (memberType == nullptr)
            return false;
        hif::semantics::ILanguageSemantics::ExpressionTypeInfo info =
            _sem->getExprType(memberType, memberType, op_concat, nullptr);
        if (info.returnedType == nullptr) {
            delete sign;
            return false;
        }

        unsigned long long sliceBw =
            static_cast<unsigned long long>(sliceMaxBound->getValue() - sliceMinBound->getValue() + 1);
        Value *concat = sign;
        for (unsigned long long i = 1; i < sliceBw; ++i) {
            concat = _factory.expression(concat, hif::op_concat, hif::copy(sign));
        }

        _replace(o, concat);
        concat->acceptVisitor(*this);
        return true;
    }

    // we are taking some some bits of value
    if (sliceMaxBound->getValue() > valueMaxBound->getValue()) {
        // taking also some bits of extension: reduced slice + cast for sign extension
        Range *rng = hif::typeGetSpan(sliceType, _sem);
        if (rng == nullptr)
            return false;
        hif::typeSetSpan(prefix->getType(), hif::copy(rng), _sem);
        o->replace(prefix);
        o->setPrefix(value);
        sliceMaxBound->replace(hif::copy(valueMaxBound));
        delete sliceMaxBound;
        prefix->setValue(o);
        hif::semantics::resetTypes(o, false);
        hif::semantics::resetTypes(prefix, false);
        prefix->acceptVisitor(*this);
        return true;
    }

    // all bits inside the value
    Range *rng = hif::typeGetSpan(sliceType, _sem);
    if (rng == nullptr)
        return false;
    hif::typeSetSpan(prefix->getType(), hif::copy(rng), _sem);
    o->replace(prefix);
    o->setPrefix(value);
    prefix->setValue(o);
    hif::semantics::resetTypes(o, false);
    hif::semantics::resetTypes(prefix, false);
    prefix->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_pushSliceIntoWhen(Slice *o)
{
    // ((when c then v1 else v3)[9:0] --> when c then v1[9:0] else v2[9:0]
    When *prefix = dynamic_cast<When *>(o->getPrefix());
    if (prefix == nullptr)
        return false;
    Type *whenType     = hif::semantics::getSemanticType(prefix, _sem);
    Type *whenBaseType = hif::semantics::getBaseType(whenType, false, _sem);
    if (!hif::semantics::isVectorType(whenBaseType, _sem) && dynamic_cast<Array *>(whenBaseType) == nullptr)
        return false;
    Range *span = o->getSpan();
    for (BList<WhenAlt>::iterator it = prefix->alts.begin(); it != prefix->alts.end(); ++it) {
        WhenAlt *alt = *it;
        alt->setValue(_factory.slice(_factory.cast(hif::copy(whenType), alt->setValue(nullptr)), hif::copy(span)));
    }

    if (prefix->getDefault() != nullptr) {
        prefix->setDefault(
            _factory.slice(_factory.cast(hif::copy(whenType), prefix->setDefault(nullptr)), hif::copy(span)));
    }

    _replace(o, prefix);
    hif::semantics::resetTypes(prefix, false);
    prefix->acceptVisitor(*this);

    return true;
}

bool SimplifyVisitor::_pushSliceIntoWith(Slice *o)
{
    // ((with c case a: v1 default: v2)[9:0] --> with c case a: v1[9:0] default v2[9:0]
    With *prefix = dynamic_cast<With *>(o->getPrefix());
    if (prefix == nullptr)
        return false;
    Type *withType     = hif::semantics::getSemanticType(prefix, _sem);
    Type *withBaseType = hif::semantics::getBaseType(withType, false, _sem);
    if (!hif::semantics::isVectorType(withBaseType, _sem) && dynamic_cast<Array *>(withBaseType) == nullptr)
        return false;
    Range *span = o->getSpan();
    for (BList<WithAlt>::iterator it = prefix->alts.begin(); it != prefix->alts.end(); ++it) {
        WithAlt *alt = *it;
        alt->setValue(_factory.slice(_factory.cast(hif::copy(withType), alt->setValue(nullptr)), hif::copy(span)));
    }

    if (prefix->getDefault() != nullptr) {
        prefix->setDefault(
            _factory.slice(_factory.cast(hif::copy(withType), prefix->setDefault(nullptr)), hif::copy(span)));
    }

    _replace(o, prefix);
    hif::semantics::resetTypes(prefix, false);
    prefix->acceptVisitor(*this);

    return true;
}
bool SimplifyVisitor::_transformSliceToCast(Slice *o)
{
    // Verilog and VHDL always take the rightmost bits when casting
    // (both when extending or trunkating)
    // Therefore this semplification can be done only when direction is downto!
    if (o->getSpan()->getDirection() != hif::dir_downto)
        return false;
    ObjectSensitivityOptions opts;
    opts.checkAll = true;
    if (hif::objectIsInSensitivityList(o, opts))
        return false;

    // ref design: verilog/v2sc/shift, vhdl/openCoresFunbase/hibi_udp
    hif::manipulation::LeftHandSideOptions leftOpts;
    leftOpts.considerPortassigns = true;
    leftOpts.sem                 = _sem;
    if (hif::manipulation::isInLeftHandSide(o, leftOpts))
        return false;

    Value *value    = o->getPrefix();
    //if (dynamic_cast<Cast*>(value) == nullptr) return false;
    Type *valueType = hif::semantics::getSemanticType(value, _sem);
    if (valueType == nullptr)
        return false;
    // Manipulation performed only on vectors and integers (avoid arrays)
    // ... other types? Strings?
    if (!hif::semantics::isVectorType(valueType, _sem) && dynamic_cast<Int *>(valueType) == nullptr)
        return false;

    Type *sliceType = hif::semantics::getSemanticType(o, _sem);
    if (sliceType == nullptr)
        return false;

    Range *valueSpan = hif::typeGetSpan(valueType, _sem);
    if (valueSpan == nullptr)
        return false;
    Range *sliceSpan    = o->getSpan();
    Range *semSliceSpan = hif::typeGetSpan(sliceType, _sem);
    if (semSliceSpan == nullptr)
        return false;

    if (!hif::equals(hif::rangeGetMinBound(valueSpan), hif::rangeGetMinBound(sliceSpan)))
        return false;
    if (!hif::equals(hif::rangeGetMinBound(semSliceSpan), hif::rangeGetMinBound(sliceSpan)))
        return false;

    Cast *newCast = new Cast();
    Type *newType = hif::copy(sliceType);
    // ref. design openCores_generic_ahb_arbiter_verilog2hif_ddt
    newType->setTypeVariant(hif::Type::NATIVE_TYPE);
    newCast->setType(newType);
    newCast->setValue(value);
    _replace(o, newCast);
    newCast->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifyBitAggregate(Aggregate *obj)
{
    Type *t = hif::semantics::getSemanticType(obj, _sem);
    messageAssert(t != nullptr, "Cannot type aggregate", obj, _sem);

    Bit *bT = dynamic_cast<Bit *>(t);
    if (bT == nullptr)
        return false;

    // At the moment, the only managed case is an initial value Aggregate with only 'others' specified
    if (!obj->alts.empty() || obj->getOthers() == nullptr) {
        messageDebugAssert(obj->alts.empty(), "Unsupported case(1)", obj->getParent(), nullptr);
        messageDebugAssert((obj->getOthers() != nullptr), "Unsupported case(2)", obj->getParent(), nullptr);
        return false;
    }

    Value *v = obj->setOthers(nullptr);
    obj->replace(v);
    delete obj;

    return true;
}

bool SimplifyVisitor::_simplifyBitvectorAggregate(Aggregate *obj, Type *t)
{
    messageAssert(t != nullptr, "Type is null", obj, _sem);

    // Check the base type: ref. design: vhdl/ips/mephisto_core
    Type *bt = hif::semantics::getBaseType(t, false, _sem);
    if (!hif::semantics::isVectorType(bt, _sem))
        return false;

    // If the span is not constant, cannot determine how many elements.
    unsigned long long aggSize = hif::semantics::typeGetSpanBitwidth(bt, _sem);
    if (aggSize == 0 && obj->alts.empty() && obj->getOthers() != nullptr)
        return false;

    // Value initialization.
    std::string bvValue;
    char newBit;

    if (obj->getOthers() != nullptr) {
        BitValue *bv = dynamic_cast<BitValue *>(obj->getOthers());
        if (bv == nullptr)
            return false;
        newBit = bv->toString()[0];
    } else {
        Bit b;
        b.setLogic(hif::typeIsLogic(bt, _sem));
        Value *tdf = _sem->getTypeDefaultValue(&b, nullptr); // FIXME:
        // for Verilog, this should check declaration and pass it to getTypeDefaultValue
        // as base case, we can consider if aggr is init.val of a declaration and pass it

        BitValue *bv = dynamic_cast<BitValue *>(tdf);
        messageAssert(bv != nullptr, "Unexpected default value type", tdf, _sem);

        newBit = bv->toString()[0];
        delete tdf;
    }

    if (aggSize == 0)
        aggSize = 1;
    _resize(bvValue, newBit, static_cast<size_t>(aggSize));

    for (BList<AggregateAlt>::iterator altIt(obj->alts.begin()); altIt != obj->alts.end(); ++altIt) {
        AggregateAlt *alt = *altIt;
        for (BList<Value>::iterator indIt(alt->indices.begin()); indIt != alt->indices.end(); ++indIt) {
            Value *index = *indIt;
            if (dynamic_cast<IntValue *>(index) != nullptr) {
                IntValue *ind = static_cast<IntValue *>(index);
                BitValue *bv  = dynamic_cast<BitValue *>(alt->getValue());
                if (bv == nullptr)
                    return false; // Maybe expression returning bit.
                _resize(bvValue, newBit, static_cast<size_t>(ind->getValue() + 1));
                bvValue[static_cast<long unsigned int>(ind->getValue())] = bv->toString()[0];
            } else if (dynamic_cast<Range *>(index) != nullptr) {
                Range *ind = static_cast<Range *>(index);

                // If the span is not constant, cannot determine how many elements.
                unsigned long long size = hif::semantics::spanGetBitwidth(ind, _sem);
                if (size == 0)
                    return false;

                IntValue *lbV = dynamic_cast<IntValue *>(ind->getLeftBound());
                IntValue *rbV = dynamic_cast<IntValue *>(ind->getRightBound());
                if (lbV == nullptr && rbV == nullptr)
                    return false;

                long long lb;
                long long rb;
                if (lbV != nullptr) {
                    lb = lbV->getValue();
                    rb = lb + static_cast<long long>(size) - 1;
                } else // if (rbV != nullptr)
                {
                    rb = rbV->getValue();
                    lb = rb + static_cast<long long>(size) - 1;
                }

                // Swap bounds if needed
                if (lb > rb)
                    std::swap(lb, rb);

                // Assigning value
                if (dynamic_cast<BitvectorValue *>(alt->getValue()) != nullptr) {
                    BitvectorValue *bvv = static_cast<BitvectorValue *>(alt->getValue());
                    std::string str_bvv(bvv->getValue());

                    if (size != str_bvv.size()) {
                        // TODO to be developed (padding)
                        messageError("Case not managed yet", alt->getValue(), _sem);
                    }

                    Bitvector *bvvType = dynamic_cast<Bitvector *>(hif::semantics::getSemanticType(bvv, _sem));
                    messageAssert(bvvType != nullptr, "Unexpected bit vector value type", bvv, _sem);

                    if (bvvType->getSpan()->getDirection() == dir_downto) {
                        str_bvv = std::string(str_bvv.rbegin(), str_bvv.rend());
                    }

                    for (long long k = lb; k <= rb; ++k) {
                        _resize(bvValue, newBit, static_cast<long unsigned int>(k) + 1);
                        bvValue[static_cast<long unsigned int>(k)] = str_bvv[static_cast<long unsigned int>(k)];
                    }
                } else if (dynamic_cast<BitValue *>(alt->getValue()) != nullptr) {
                    BitValue *bv = static_cast<BitValue *>(alt->getValue());

                    for (long long k = lb; k <= rb; ++k) {
                        _resize(bvValue, newBit, static_cast<long unsigned int>(k) + 1);
                        bvValue[static_cast<long unsigned int>(k)] = bv->toString()[0];
                    }
                } else
                    return false; // Maybe expression...
            } else
                return false; // Maybe expression...
        }
    }

    std::string ret;
    ret = bvValue;

    Type *aggType = hif::semantics::getSemanticType(obj, _sem);
    messageAssert(aggType != nullptr, "Cannot type description", obj, _sem);
    Range *aggSpan = hif::typeGetSpan(aggType, _sem);
    messageAssert(aggSpan != nullptr, "Cannot get type span", aggType, _sem);
    if (aggSpan->getDirection() == dir_downto) {
        std::reverse(ret.begin(), ret.end());
    }

    BitvectorValue *newV = nullptr;
    newV                 = new BitvectorValue(ret);
    Type *sType          = hif::copy(bt); // need to copy since _replace performs delete
    const bool replaced  = _replace(obj, newV);
    hif::manipulation::assureSyntacticType(newV, _sem, sType);
    delete sType; // delete since assureSyntacticType copy it
    if (!replaced)
        delete newV;
    return true;
}

bool SimplifyVisitor::_simplifyStringAggregate(Aggregate *obj, Type *t)
{
    String *s = dynamic_cast<String *>(t);
    Char *c   = dynamic_cast<Char *>(t);
    if (s == nullptr && c == nullptr)
        return false;

    if (c != nullptr) {
        messageAssert(obj->alts.size() == 1 || obj->getOthers() != nullptr, "Unexpected aggregate", obj, _sem);

        Value *v = nullptr;
        if (obj->alts.empty())
            v = obj->setOthers(nullptr);
        else
            v = obj->alts.front()->setValue(nullptr);

        _replace(obj, v);

        return true;
    }

    messageAssert(obj->alts.empty(), "TODO: to manage std::string aggregate with alts", obj, _sem);

    Value *v      = obj->getOthers();
    CharValue *cv = dynamic_cast<CharValue *>(v);
    messageAssert(cv != nullptr, "Unexpected aggregate", obj, _sem);

    std::string val;
    if (cv->getValue() == '\0') {
        val = "";
    } else {
        unsigned long long bw = hif::semantics::spanGetBitwidth(s->getSpanInformation(), _sem);
        if (bw == 0)
            return false;
        std::string ss(static_cast<size_t>(bw), cv->getValue());
        val = ss;
    }
    StringValue *tt = new StringValue();
    tt->setValue(val);
    String *synt = hif::copy(s);
    hif::typeSetConstexpr(synt, true);
    tt->setType(synt);

    _replace(obj, tt);

    return true;
}

bool SimplifyVisitor::_simplifyUnrollingAggregate(Aggregate *obj, Type *t)
{
    // Works only in mixed case!
    if (obj->getOthers() == nullptr)
        return false;

    if (!transformAggregateUnrollingAlts(obj, AGGREGATE_UROLL_THRESHOLD, _sem))
        return false;

    // assuring same type as original
    t = hif::copy(t);
    hif::semantics::resetTypes(obj);
    Type *newType = hif::semantics::getSemanticType(obj, _sem);
    messageAssert(newType != nullptr, "Cannot type the unrolled aggregate", obj, _sem);
    if (!hif::equals(newType, t)) {
        Cast *c = new Cast();
        c->setType(t);
        obj->replace(c);
        c->setValue(obj);

        c->acceptVisitor(*this);
    } else {
        delete t;
        obj->acceptVisitor(*this);
    }

    return true;
}

bool SimplifyVisitor::_simplifyRollingAggregate(Aggregate *obj, Type *t)
{
    if (obj->alts.size() <= 1)
        return false;
    if (obj->getOthers() == nullptr)
        return false;

    if (!hif::manipulation::transformAggregateRollingAlts(obj, true, _sem))
        return false;

    // assuring same type as original
    t = hif::copy(t);
    hif::semantics::resetTypes(obj);
    Type *newType = hif::semantics::getSemanticType(obj, _sem);
    messageAssert(newType != nullptr, "Cannot type the rolled aggregate", obj, _sem);
    if (!hif::equals(newType, t)) {
        Cast *c = new Cast();
        c->setType(t);
        obj->replace(c);
        c->setValue(obj);

        c->acceptVisitor(*this);
    } else {
        delete t;
        obj->acceptVisitor(*this);
    }

    return true;
}

bool SimplifyVisitor::_simplifyAggregateWithSameAlts(Aggregate *obj, Type *t)
{
    // aggregate [0]-> {a[0]} [1]->{a[1]} .... [n]->{a[n]}
    // => a[0:n]
    if (obj->alts.empty())
        return false;
    if (obj->getOthers() != nullptr)
        return false;
    typedef std::map<long long, long long> Indexes;
    Indexes indexes;
    Value *prefix = nullptr;
    // Create a map <aggregate index, member index>
    for (BList<AggregateAlt>::iterator it = obj->alts.begin(); it != obj->alts.end(); ++it) {
        AggregateAlt *alt = *it;
        if (alt->indices.size() != 1)
            return false;
        Value *index       = alt->indices.front();
        Value *altValue    = alt->getValue();
        IntValue *indexVal = dynamic_cast<IntValue *>(index);
        Range *indexRng    = dynamic_cast<Range *>(index);
        if (indexVal == nullptr && indexRng == nullptr)
            return false;

        if (indexVal != nullptr) {
            Member *mem = dynamic_cast<Member *>(altValue);
            if (mem == nullptr)
                return false;
            IntValue *memIndex = dynamic_cast<IntValue *>(mem->getIndex());
            if (memIndex == nullptr)
                return false;
            if (prefix == nullptr) {
                prefix = mem->getPrefix();
            } else {
                if (!hif::equals(prefix, mem->getPrefix()))
                    return false;
            }
            indexes[indexVal->getValue()] = memIndex->getValue();
        } else if (indexRng != nullptr) {
            IntValue *indexLeft  = dynamic_cast<IntValue *>(indexRng->getLeftBound());
            IntValue *indexRight = dynamic_cast<IntValue *>(indexRng->getRightBound());
            if (indexLeft == nullptr || indexRight == nullptr)
                return false;

            Slice *sl = dynamic_cast<Slice *>(altValue);
            if (sl == nullptr)
                return false;
            IntValue *left  = dynamic_cast<IntValue *>(sl->getSpan()->getLeftBound());
            IntValue *right = dynamic_cast<IntValue *>(sl->getSpan()->getRightBound());
            if (left == nullptr || right == nullptr)
                return false;

            const bool isRngDownto   = indexLeft->getValue() > indexRight->getValue();
            const bool isValueDownto = left->getValue() > right->getValue();
            long long rngBw          = isRngDownto ? (indexLeft->getValue() - indexRight->getValue() + 1)
                                                   : (indexRight->getValue() - indexLeft->getValue() + 1);
            long long valueBw =
                isValueDownto ? (left->getValue() - right->getValue() + 1) : (right->getValue() - left->getValue() + 1);
            if (rngBw != valueBw)
                return false;

            if (prefix == nullptr) {
                prefix = sl->getPrefix();
            } else {
                if (!hif::equals(prefix, sl->getPrefix()))
                    return false;
            }

            long long rngMinBound   = isRngDownto ? indexRight->getValue() : indexLeft->getValue();
            long long valueMinBound = isValueDownto ? right->getValue() : left->getValue();
            long long valueMaxBound = isValueDownto ? left->getValue() : right->getValue();
            const bool isSameDir    = isValueDownto == isRngDownto;
            long long valueStart    = isSameDir ? valueMinBound : valueMaxBound;
            long long i             = rngMinBound;
            long long j             = valueStart;
            for (; i != rngMinBound + rngBw - 1; ++i) {
                indexes[i] = j;
                if (isSameDir)
                    j += 1;
                else
                    j -= 1;
            }
        }
    }

    // Check if we can collapse all aggregate alts
    bool isSameDir = true;
    if (indexes.size() > 1) {
        Indexes::iterator it = indexes.begin();
        Indexes::iterator jt = it;
        ++jt;
        const bool isIndexDownto = (it->first < jt->first);
        const bool isValueDownto = (it->second < jt->second);
        isSameDir                = isIndexDownto == isValueDownto;
    }

    Indexes::iterator it   = indexes.begin();
    long long currentIndex = it->first;
    long long currentValue = it->second;
    ++it;
    for (; it != indexes.end(); ++it) {
        if (currentIndex + 1 != it->first)
            return false;
        if (isSameDir) {
            if (currentValue + 1 != it->second)
                return false;
        } else {
            if (currentValue - 1 != it->second)
                return false;
        }
        currentIndex = it->first;
        currentValue = it->second;
    }
    // Collapse aggregate
    long long min = indexes.begin()->second;
    long long max = (--indexes.end())->second;

    Type *valueType = hif::semantics::getBaseType(prefix, false, _sem);
    if (valueType == nullptr)
        return false;
    Range *typeRange = hif::typeGetSpan(valueType, _sem);
    if (typeRange == nullptr)
        return false;
    const bool isTypeDownto = typeRange->getDirection() == hif::dir_downto;

    Range *rng = nullptr;
    if (isTypeDownto)
        rng = new Range(max, min);
    else
        rng = new Range(min, max);

    Slice *newValue = _factory.slice(prefix, rng);
    Cast *c         = _factory.cast(hif::copy(t), newValue);
    _replace(obj, c);
    c->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifySigleBitExpressionAggregate(Aggregate *obj, Type *t)
{
    Type *baseType = hif::semantics::getBaseType(t, false, _sem);
    if (!hif::semantics::isVectorType(baseType, _sem))
        return false;
    if (obj->alts.size() != 1)
        return false;
    if (obj->alts.front()->indices.size() != 1)
        return false;
    if (dynamic_cast<Range *>(obj->alts.front()->indices.front()) != nullptr)
        return false;
    Range *span = hif::typeGetSpan(baseType, _sem);
    if (span == nullptr)
        return false;
    Value *minBound = hif::rangeGetMinBound(span);
    if (!hif::equals(minBound, obj->alts.front()->indices.front()))
        return false;
    unsigned long long bw = hif::semantics::spanGetBitwidth(span, _sem);
    if (bw != 1) {
        if (dynamic_cast<BitValue *>(obj->getOthers()) == nullptr)
            return false;
        if (static_cast<BitValue *>(obj->getOthers())->getValue() != hif::bit_zero)
            return false;
    }

    Type *castType = hif::copy(t);
    Cast *c        = _factory.cast(castType, obj->alts.front()->getValue());
    _replace(obj, c);
    c->acceptVisitor(*this);
    return true;
}

bool SimplifyVisitor::_simplifyAggregateToConcat(Aggregate *obj, Type *t)
{
    // Agg{ 0-> expr1, 1->expr2} --> if is vector type:
    // concat expr1, expr2
    Type *baseType = hif::semantics::getBaseType(t, false, _sem);
    if (!hif::semantics::isVectorType(baseType, _sem))
        return false;
    Range *span = hif::typeGetSpan(baseType, _sem);
    if (span == nullptr)
        return false;
    long long bw = static_cast<long long>(hif::semantics::spanGetBitwidth(span, _sem));
    if (bw == 0)
        return false;
    IntValue *minBound = dynamic_cast<IntValue *>(hif::rangeGetMinBound(span));
    if (minBound == nullptr)
        return false;

    typedef std::map<long long, Value *> Indexes;
    Indexes indexes;

    // Create a map <aggregate index, alt value>
    long long minIndexValue = std::numeric_limits<long long>::max();
    for (BList<AggregateAlt>::iterator it = obj->alts.begin(); it != obj->alts.end(); ++it) {
        AggregateAlt *alt = *it;
        Value *altValue   = alt->getValue();
        for (BList<Value>::iterator jt = alt->indices.begin(); jt != alt->indices.end(); ++jt) {
            Value *index = *jt;

            IntValue *indexVal = dynamic_cast<IntValue *>(index);
            Range *indexRng    = dynamic_cast<Range *>(index);
            if (indexVal == nullptr && indexRng == nullptr)
                return false;

            if (indexVal != nullptr) {
                long long ii = indexVal->getValue();
                if (ii < minIndexValue)
                    minIndexValue = ii;
                indexes[ii] = altValue;
            } else if (indexRng != nullptr) {
                IntValue *minIndex = dynamic_cast<IntValue *>(hif::rangeGetMinBound(indexRng));
                IntValue *maxIndex = dynamic_cast<IntValue *>(hif::rangeGetMaxBound(indexRng));
                if (minIndex == nullptr || maxIndex == nullptr)
                    return false;
                if (minIndex->getValue() < minIndexValue)
                    minIndexValue = minIndex->getValue();

                for (long long ii = minIndex->getValue(); ii <= maxIndex->getValue(); ++ii) {
                    indexes[ii] = altValue;
                }
            } else {
                messageError("Unexpected case", nullptr, _sem);
            }
        }
    }

    // Checking if we can collapse all aggregate alts,
    // and creating resulting concat
    Value *concat       = nullptr;
    const bool isDownto = span->getDirection() == hif::dir_downto;
    for (long long ii = minBound->getValue(); ii < minBound->getValue() + bw; ++ii) {
        Indexes::iterator it = indexes.find(ii);
        if (it == indexes.end()) {
            if (obj->getOthers() == nullptr) {
                delete concat;
                return false;
            }

            if (concat == nullptr)
                concat = hif::copy(obj->getOthers());
            else if (isDownto)
                concat = _factory.expression(hif::copy(obj->getOthers()), hif::op_concat, concat);
            else
                concat = _factory.expression(concat, hif::op_concat, hif::copy(obj->getOthers()));

            continue;
        }

        Value *value = it->second;
        if (concat == nullptr)
            concat = hif::copy(value);
        else if (isDownto)
            concat = _factory.expression(hif::copy(value), hif::op_concat, concat);
        else
            concat = _factory.expression(concat, hif::op_concat, hif::copy(value));
    }

    Cast *c = _factory.cast(hif::copy(t), concat);
    _replace(obj, c);
    c->acceptVisitor(*this);
    return true;
}

template <typename T> bool SimplifyVisitor::_simplifyReferencedAssignList(BList<T> &list)
{
    for (typename BList<T>::iterator i = list.begin(); i != list.end(); ++i) {
        ReferencedAssign *ra = *i;
        if (dynamic_cast<TypeTPAssign *>(ra) != nullptr)
            continue;
        hif::Operator op = hif::op_assign;

        bool checkPrecision = false;
        Value *v            = nullptr;
        if (dynamic_cast<ValueTPAssign *>(ra) != nullptr) {
            v  = static_cast<ValueTPAssign *>(ra)->getValue();
            op = hif::op_conv;
        } else if (dynamic_cast<PortAssign *>(ra) != nullptr) {
            v  = static_cast<PortAssign *>(ra)->getValue();
            op = hif::op_bind;
        } else if (dynamic_cast<ParameterAssign *>(ra) != nullptr) {
            v              = static_cast<ParameterAssign *>(ra)->getValue();
            op             = hif::op_conv;
            checkPrecision = true;
        }

        Type *tt = hif::semantics::getSemanticType(ra, _sem);
        messageAssert(tt != nullptr, "Cannot type reference assign", *i, _sem);

        _simplifyLeftHandSideCasts(tt, v, ra, checkPrecision, op);
    }

    return true;
}

bool SimplifyVisitor::_simplifyConstantCondition(If *o)
{
    // Check all alternatives. If alternative condition result false, mark
    // the alternative to be removed. If it results true, replace the
    // if-statement with that alternative. If all alternatives result false,
    // replace the if-statement with the default case.
    bool allFalse = true;

    for (BList<IfAlt>::iterator i = o->alts.begin(); i != o->alts.end();) {
        IfAlt *alt   = *i;
        int boolCond = _resolveBoolCondition(alt->getCondition());

        if (boolCond == 0) {
            i = i.erase();
        } else if (boolCond == 1) {
            if (allFalse) {
                _replaceWithList(o, alt->actions);
                return true;
            } else {
                o->defaults.clear();
                o->defaults.merge(alt->actions);
                for (BList<IfAlt>::iterator j = i; j != o->alts.end();) {
                    j = j.erase();
                }

                return true;
            }
        } else {
            allFalse = false;
            ++i;
        }
    }

    // If all alts have been marked to be removed, check the presence of a
    // default behavior (and replace if statement with its content) and
    // delete if statement.
    if (!allFalse)
        return false;

    _replaceWithList(o, o->defaults);

    return true;
}

bool SimplifyVisitor::_simplifyConstantCondition(When *o)
{
    Bool b;
    bool constTrue = false;
    for (BList<WhenAlt>::iterator i = o->alts.begin(); i != o->alts.end();) {
        if (constTrue) {
            i = i.erase();
            continue;
        }

        WhenAlt *wa      = *i;
        ConstValue *cond = dynamic_cast<ConstValue *>(wa->getCondition());
        if (cond == nullptr) {
            ++i;
            continue;
        }
        BoolValue *bv = dynamic_cast<BoolValue *>(transformConstant(cond, &b, _sem));
        if (bv == nullptr) {
            ++i;
            continue;
        }

        const bool val = bv->getValue();
        delete bv;
        if (val) {
            constTrue = true;
            ++i;
        } else {
            i = i.erase();
        }
    }

    if (constTrue) {
        delete o->setDefault(nullptr);
    }

    if (o->alts.empty()) {
        // substitute with default
        messageDebugAssert(o->getDefault() != nullptr, "Expected default value", o, _sem);
        Value *v = hif::copy(o->getDefault());
        _replace(o, v);
        return true;
    }

    if (o->alts.size() == 1 && constTrue) {
        // substitute with remaining alt
        Value *v = hif::copy(o->alts.back()->getValue());
        _replace(o, v);
        return true;
    }

    return false;
}

bool SimplifyVisitor::_simplifyConstantCondition(With *o)
{
    ConstValue *cv = dynamic_cast<ConstValue *>(o->getCondition());
    if (cv == nullptr)
        return false;

    Value *withv  = nullptr;
    bool allConst = true;
    for (BList<WithAlt>::iterator i = o->alts.begin(); i != o->alts.end(); ++i) {
        WithAlt *wa = *i;
        for (BList<Value>::iterator j = wa->conditions.begin(); j != wa->conditions.end(); ++j) {
            Value *cond        = *j;
            ConstValue *condCv = dynamic_cast<ConstValue *>(cond);
            allConst &= (condCv != nullptr);
            if (!hif::equals(o->getCondition(), cond))
                continue;
            withv = wa->getValue();
            break;
        }
        if (withv != nullptr)
            break;
    }

    if (withv != nullptr) {
        Value *v = hif::copy(withv);
        _replace(o, v);
        return true;
    }

    // If it is not found but it's a const, the only match is the default value
    if (allConst) {
        Value *v = hif::copy(o->getDefault());
        _replace(o, v);
        return true;
    }

    return false;
}

bool SimplifyVisitor::_simplifyConstantCondition(Switch *o)
{
    ConstValue *cv = dynamic_cast<ConstValue *>(o->getCondition());
    if (cv == nullptr)
        return false;

    SwitchAlt *constAlt = nullptr;
    bool allConst       = true;
    for (BList<SwitchAlt>::iterator i = o->alts.begin(); i != o->alts.end(); ++i) {
        SwitchAlt *alt = *i;
        for (BList<Value>::iterator j = alt->conditions.begin(); j != alt->conditions.end(); ++j) {
            Value *cond        = *j;
            ConstValue *condCv = dynamic_cast<ConstValue *>(cond);
            allConst &= (condCv != nullptr);
            if (!hif::equals(o->getCondition(), cond))
                continue;
            constAlt = alt;
            break;
        }
        if (constAlt != nullptr)
            break;
    }

    if (constAlt != nullptr) {
        _replaceWithList(o, constAlt->actions);
        return true;
    }

    // If it is not found but it's a const, the only match is the default value
    if (allConst) {
        _replaceWithList(o, o->defaults);
        return true;
    }

    return false;
}

bool SimplifyVisitor::_equalsAltsBody(IfAlt *refAlt, IfAlt *currentAlt)
{
    return hif::equals(refAlt->actions, currentAlt->actions);
}

bool SimplifyVisitor::_equalsAltAndDefaultBody(IfAlt *refAlt, If *o)
{
    return hif::equals(refAlt->actions, o->defaults);
}

void SimplifyVisitor::_replaceCaseWithDefault(If *o) { _replaceWithList(o, o->defaults); }

bool SimplifyVisitor::_equalsAltsBody(WhenAlt *refAlt, WhenAlt *currentAlt)
{
    return hif::equals(refAlt->getValue(), currentAlt->getValue());
}

bool SimplifyVisitor::_equalsAltAndDefaultBody(WhenAlt *refAlt, When *o)
{
    return hif::equals(refAlt->getValue(), o->getDefault());
}

void SimplifyVisitor::_replaceCaseWithDefault(When *o) { o->replace(o->setDefault(nullptr)); }

bool SimplifyVisitor::_equalsAltsBody(SwitchAlt *refAlt, SwitchAlt *currentAlt)
{
    return hif::equals(refAlt->actions, currentAlt->actions);
}

bool SimplifyVisitor::_equalsAltAndDefaultBody(SwitchAlt *refAlt, Switch *o)
{
    return hif::equals(refAlt->actions, o->defaults);
}

void SimplifyVisitor::_replaceCaseWithDefault(Switch *o) { _replaceWithList(o, o->defaults); }

bool SimplifyVisitor::_equalsAltsBody(WithAlt *refAlt, WithAlt *currentAlt)
{
    return hif::equals(refAlt->getValue(), currentAlt->getValue());
}

bool SimplifyVisitor::_equalsAltAndDefaultBody(WithAlt *refAlt, With *o)
{
    return hif::equals(refAlt->getValue(), o->getDefault());
}

void SimplifyVisitor::_replaceCaseWithDefault(With *o) { o->replace(o->setDefault(nullptr)); }

bool SimplifyVisitor::_simplifyWhenBoolConstants(When *o)
{
    if (o->alts.size() != 1 || o->getDefault() == nullptr)
        return false;

    Type *type     = hif::semantics::getSemanticType(o, _sem);
    Type *baseType = hif::semantics::getBaseType(type, false, _sem);
    messageAssert(baseType != nullptr, "Cannot type When", o, _sem);
    // Canot simplify, since the value could hold also X or Z.
    // Ref design: verilog/openCores/i2cSlaveTop
    const bool isLogic = hif::typeIsLogic(baseType, _sem);
    if (isLogic)
        return false;

    // TODO: can simplify also bv<1> ?
    Bit *bitType   = dynamic_cast<Bit *>(baseType);
    Bool *boolType = dynamic_cast<Bool *>(baseType);
    if (bitType == nullptr && boolType == nullptr)
        return false;

    Value *value1   = o->alts.front()->getValue();
    Value *value2   = o->getDefault();
    ConstValue *cv1 = dynamic_cast<ConstValue *>(value1);
    ConstValue *cv2 = dynamic_cast<ConstValue *>(value2);
    if (cv1 == nullptr && cv2 == nullptr)
        return false;
    const bool isCase1 = _checkBooleanValue(cv1);
    const bool isCase2 = _checkBooleanValue(cv2);
    if (!isCase1 && !isCase2)
        return false;

    // NOTE: remember that 'X' in condition means false!
    //       --> c behaves as a boolean for following semplifications
    // CASE 1
    // c ? cv1 : v -->
    // cv1 == true --> c || v
    // cv1 == false --> !c && v
    //
    // CASE 2
    // c ? v : cv2 -->
    // cv2 == true --> v || !c
    // cv2 == false --> c && v

    // ref design: vhdl/agilent/built_in_fir
    const bool isBitwise  = (bitType != nullptr && _sem->hasBitwiseOperationsOnBits(bitType->isLogic()));
    const bool constValue = isCase1 ? _getBooleanValue(cv1) : _getBooleanValue(cv2);

    const bool negateC = (isCase1 && !constValue) || (!isCase1 && constValue);
    const bool useAnd  = !constValue;

    const hif::Operator opNot = isBitwise ? hif::op_bnot : hif::op_not;
    const hif::Operator opAnd = isBitwise ? hif::op_band : hif::op_and;
    const hif::Operator opOr  = isBitwise ? hif::op_bor : hif::op_or;

    const hif::Operator exprOp = useAnd ? opAnd : opOr;

    Value *v = isCase1 ? value2 : value1;

    // We should check whether When is logic_ternary:
    // - ternary? --> as documented
    // - !ternary? --> required also call to explicitBoolConversion
    // But for non-logic values, the two behaviors are the same.
    // So no need to manage this case...
    // Ref design: verilog/openCores/i2cSlaveTop
    Value *c = nullptr;
    if (o->isLogicTernary() || !isLogic) {
        c = _factory.cast(hif::copy(type), o->alts.front()->getCondition());
    } else {
        // Explicit bool conversion required! Since hidden conversions could apply.
        // E.g. (x > y) @type(logic_bit) --> (x > y) == '1'
        // Ref design: vhdl/ips/mephisto_core
        c = _factory.cast(hif::copy(type), _sem->explicitBoolConversion(o->alts.front()->getCondition()));
    }
    if (negateC)
        c = _factory.expression(opNot, c);

    Expression *expr = _factory.expression(c, exprOp, v);

    _replace(o, expr);
    expr->acceptVisitor(*this);

    return true;
}

template <typename T> bool SimplifyVisitor::_simplifyUselessAlts(T *o)
{
    typedef std::set<std::string> CaseSet;
    CaseSet caseSet;
    typedef typename T::AltType AltType;
    for (typename BList<AltType>::iterator i = o->alts.begin(); i != o->alts.end();) {
        AltType *currentAlt = *i;
        for (BList<Value>::iterator j = currentAlt->conditions.begin(); j != currentAlt->conditions.end();) {
            Value *condition                = *j;
            const std::string &conditionKey = hif::objectGetKey(condition);
            if (caseSet.find(conditionKey) == caseSet.end()) {
                caseSet.insert(conditionKey);
                ++j;
                continue;
            }

            j = j.erase();
        }

        if (!currentAlt->conditions.empty()) {
            ++i;
            continue;
        }

        i = i.erase();
    }

    return o->alts.empty();
}

template <typename T> bool SimplifyVisitor::_simplifyMergingCases(T *o)
{
    typedef typename T::AltType AltType;
    AltType *refAlt = nullptr;
    hif::Trash localTrash;
    bool isFirst = false;
    for (typename BList<AltType>::iterator i = o->alts.begin(); i != o->alts.end(); ++i) {
        AltType *currentAlt = *i;

        if (refAlt != nullptr && _equalsAltsBody(refAlt, currentAlt)) {
            if (isFirst) {
                Value *newCurrentCond = _sem->explicitBoolConversion(currentAlt->getCondition());
                messageAssert(
                    newCurrentCond != nullptr, "Cannot determine new current condition (2)", currentAlt->getCondition(),
                    _sem);
                delete currentAlt->setCondition(newCurrentCond);
                isFirst = false;
            }

            Value *refAltCond     = refAlt->getCondition();
            Value *currentAltCond = currentAlt->getCondition();

            // Checking types. Allowed only if both bits or both bools.
            Type *refAltCondType =
                hif::semantics::getBaseType(hif::semantics::getSemanticType(refAltCond, _sem), false, _sem);
            Type *currentAltCondType =
                hif::semantics::getBaseType(hif::semantics::getSemanticType(currentAltCond, _sem), false, _sem);

            Bool *refBool = dynamic_cast<Bool *>(refAltCondType);
            Bit *refBit   = dynamic_cast<Bit *>(refAltCondType);
            if (refBool == nullptr && refBit == nullptr) {
                isFirst = true;
                refAlt  = currentAlt;
                continue;
            }

            Bool *currentBool = dynamic_cast<Bool *>(currentAltCondType);
            Bit *currentBit   = dynamic_cast<Bit *>(currentAltCondType);
            if (currentBool == nullptr && currentBit == nullptr) {
                isFirst = true;
                refAlt  = currentAlt;
                continue;
            }

            // ok merge
            Value *newCurrentCond = _sem->explicitBoolConversion(currentAltCond);
            messageAssert(
                newCurrentCond != nullptr, "Cannot determine new current condition (1)", currentAltCond, _sem);

            refAlt->setCondition(_factory.expression(refAltCond, op_or, newCurrentCond));
            localTrash.insert(currentAlt);
        } else {
            isFirst = true;
            refAlt  = currentAlt;
        }
    }

    if (refAlt != nullptr && _equalsAltAndDefaultBody(refAlt, o)) {
        localTrash.insert(refAlt);
    }

    localTrash.clear();
    if (o->alts.empty()) {
        _replaceCaseWithDefault(o);
        return true;
    }

    // only simplified, object is good
    return false;
}

template <typename T> bool SimplifyVisitor::_simplifyMergingAlts(T *o)
{
    typedef typename T::AltType AltType;
    hif::Trash localTrash;

    if (o->getCaseSemantics() != hif::CASE_LITERAL) {
        // only near branches can be safely merged
        AltType *refAlt = nullptr;
        for (typename BList<AltType>::iterator i = o->alts.begin(); i != o->alts.end(); ++i) {
            AltType *currentAlt = *i;
            if (refAlt != nullptr && _equalsAltsBody(refAlt, currentAlt)) {
                // merge condition
                BList<Value> &currAltCond = currentAlt->conditions;
                refAlt->conditions.merge(currAltCond);
                localTrash.insert(currentAlt);
            } else {
                refAlt = currentAlt;
            }
        }

        if (refAlt != nullptr && _equalsAltAndDefaultBody(refAlt, o)) {
            localTrash.insert(refAlt);
        }
    } else {
        // more aggressive simplify:
        for (typename BList<AltType>::iterator i = o->alts.rbegin(); i != o->alts.rend(); --i) {
            AltType *refAlt = *i;
            if (_equalsAltAndDefaultBody(refAlt, o)) {
                localTrash.insert(refAlt);
                continue;
            }

            typename BList<AltType>::iterator j = i;
            --j;
            for (; j != o->alts.rend(); --j) {
                AltType *currentAlt = *j;
                if (_equalsAltsBody(refAlt, currentAlt)) {
                    // merge condition
                    BList<Value> &refAltCond = refAlt->conditions;
                    currentAlt->conditions.merge(refAltCond);
                    localTrash.insert(refAlt);
                    break;
                }
            }
        }
    }

    localTrash.clear();
    if (o->alts.empty()) {
        _replaceCaseWithDefault(o);
        return true;
    }

    // only simplified, object is good
    return false;
}

// //////////////////////////
// Visits methods
// //////////////////////////

int SimplifyVisitor::AfterVisit(Object &)
{
    _trash.clear();
    return 0;
}

int SimplifyVisitor::visitAggregate(Aggregate &o)
{
    GuideVisitor::visitAggregate(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);

    Type *t = hif::semantics::getSemanticType(&o, _sem);
    if (t == nullptr)
        return 0;

    if (_simplifyBitvectorAggregate(&o, t))
        return 0;
    if (_simplifyBitAggregate(&o))
        return 0;
    if (_simplifyStringAggregate(&o, t))
        return 0;
    if (_simplifyUnrollingAggregate(&o, t))
        return 0;
    if (_simplifyRollingAggregate(&o, t))
        return 0;
    if (_simplifyAggregateWithSameAlts(&o, t))
        return 0;
    if (_simplifySigleBitExpressionAggregate(&o, t))
        return 0;
    if (_simplifyAggregateToConcat(&o, t))
        return 0;

    return 0;
}

int SimplifyVisitor::visitAggregateAlt(AggregateAlt &o)
{
    GuideVisitor::visitAggregateAlt(o);
    for (BList<Value>::iterator it(o.indices.begin()); it != o.indices.end(); ++it) {
        _fixBoundType(*it);
        _handleBound(*it);
    }
    return 0;
}

int SimplifyVisitor::visitArray(Array &o)
{
    GuideVisitor::visitArray(o);
    _simplifyBaseType(&o);
    return 0;
}

int SimplifyVisitor::visitAssign(Assign &o)
{
    GuideVisitor::visitAssign(o);
    _fixRightHandSideCasts(&o);
    Type *tt = hif::semantics::getSemanticType(o.getLeftHandSide(), _sem);
    _simplifyLeftHandSideCasts(tt, o.getRightHandSide(), &o);
    if (_removeNullRangeAssign(&o))
        return 0;
    if (_transformAssignFromArrayOfBoolToInt(&o))
        return 0;

    _ensureAssignability(&o);
    return 0;
}

int SimplifyVisitor::visitBitValue(BitValue &o)
{
    GuideVisitor::visitBitValue(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);
    _fixSyntacticType(&o);
    return 0;
}

int SimplifyVisitor::visitBitvectorValue(BitvectorValue &o)
{
    GuideVisitor::visitBitvectorValue(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);
    _fixSyntacticType(&o);
    return 0;
}

int SimplifyVisitor::visitBoolValue(BoolValue &o)
{
    GuideVisitor::visitBoolValue(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);
    _fixSyntacticType(&o);
    return 0;
}

int SimplifyVisitor::visitBreak(Break &o)
{
    GuideVisitor::visitBreak(o);
    Object *scope = hif::getMatchingScope(&o);
    if (scope != nullptr)
        return 0;
    _trash.insert(&o);
    return 0;
}

int SimplifyVisitor::visitCast(Cast &o)
{
    GuideVisitor::visitCast(o);

    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);

    // Some cases are included in:
    // - visit of Assign
    // - visit of Expression
    // - visit of IfAlt
    // - visit of Switch
    // - visit of DataDeclaration

    // Removing of nested cast
    _removeUselessNestedCast(&o);

    // Removing of current cast
    if (_removeSameTypeCast(&o))
        return 0;
    if (_transformCastOfConstValue(&o))
        return 0;
    if (_explicitAggregateCast(&o))
        return 0;
    if (_transformCastToAggregate(&o))
        return 0;
    if (_explicitRecordCast(&o))
        return 0;
    if (_transformCastFromAggregateToBitvector(&o))
        return 0;
    if (_transformCastOfConcat(&o))
        return 0;
    if (_fixCastFromBitvectorToArray(&o))
        return 0;
    // NOTE: enabled after fix of TransforConstant
    // ref design: vhdl/custom/test_span_direction (broken if enabled)
    if (_transformCastFromBitArrayConcat(&o))
        return 0;
    if (_transformCastFromArrayOfBoolToInt(&o))
        return 0;
    if (_simplifyCastOfVectorMultiplication(&o))
        return 0;

    return 0;
}

int SimplifyVisitor::visitCharValue(CharValue &o)
{
    GuideVisitor::visitCharValue(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);
    _fixSyntacticType(&o);
    return 0;
}

int SimplifyVisitor::visitConst(Const &o)
{
    GuideVisitor::visitConst(o);
    _simplifyLeftHandSideCasts(o.getType(), o.getValue(), &o);
    return 0;
}

int SimplifyVisitor::visitContinue(Continue &o)
{
    GuideVisitor::visitContinue(o);
    Object *scope = hif::getMatchingScope(&o);
    if (scope != nullptr)
        return 0;
    _trash.insert(&o);
    return 0;
}

int SimplifyVisitor::visitEnumValue(EnumValue &o)
{
    GuideVisitor::visitEnumValue(o);

    // loop caused by visit of enumvalue type declaration is avoided
    // inside the visit of typeref.

    return 0;
}

int SimplifyVisitor::visitExpression(Expression &o)
{
#ifdef DEBUG_EXPR_SCREEN
    ++__expressionId;
    std::clog << "[EXPR_" << __expressionId << "] - " << "01. - Original expr" << std::endl;
    hif::writeFile(std::clog, &o, false);
    std::clog << std::endl;
#endif

    GuideVisitor::visitExpression(o);

#ifdef DEBUG_EXPR_SCREEN
    std::clog << "[EXPR_" << __expressionId << "] - " << "02. After guide visitor" << std::endl;
    hif::writeFile(std::clog, &o, false);
    std::clog << std::endl;
#endif

    hif::manipulation::sort(&o, _sem, _sortOpt);

#ifdef DEBUG_EXPR_SCREEN
    std::clog << "[EXPR_" << __expressionId << "] - " << "03. After sort" << std::endl;
    hif::writeFile(std::clog, &o, false);
    std::clog << std::endl;
#endif

    Type *ot = hif::semantics::getSemanticType(&o, _sem);
    _simplifySemanticsType(&o);

    if (_doSimplifyExpression(&o))
        return 0;

    if (_isAlreadySimplified(&o)) {
#ifdef DEBUG_EXPR_SCREEN
        std::clog << "[EXPR_" << __expressionId << "] - " << "ALREADY SIMPLIFIED. END VISIT!" << std::endl;
        --__expressionId;
#endif
        return 0;
    }

    std::string expressionKey = hif::objectGetKey(&o);

    // //////////////////////////
    // Series of simplifications
    // on original expr that
    // may coexist..
    // //////////////////////////
    Expression *refExpression = _getRebalancedExpressions(&o);
    const bool isRebalanced   = (refExpression != nullptr);
    if (!isRebalanced) {
        refExpression = &o;
    }

#ifdef DEBUG_EXPR_SCREEN
    std::clog << "[EXPR_" << __expressionId << "] - " << "04. After _getRebalancedExpressions" << std::endl;
    hif::writeFile(std::clog, refExpression, false);
    std::clog << std::endl;
#endif

    const bool isChangedOperators = _pushDownOperators(refExpression);
    const bool isLinearized       = _linearizeExpressionTree(refExpression);
    const bool isSortedBranches   = _sortBranches(refExpression);

    bool isSorted = false;
    isSorted |= hif::manipulation::sort(refExpression->getValue1(), _sem, _sortOpt);
    if (refExpression->getValue2() != nullptr) {
        isSorted |= hif::manipulation::sort(refExpression->getValue2(), _sem, _sortOpt);
    }
    isSorted |= hif::manipulation::sort(refExpression, _sem, _sortOpt);

#ifdef DEBUG_EXPR_SCREEN
    std::clog << "[EXPR_" << __expressionId << "] - " << "05. After push/linearize/sort" << std::endl;
    std::clog << "   - isChangedOperators: " << isChangedOperators << std::endl;
    std::clog << "   - isLinearized: " << isLinearized << std::endl;
    std::clog << "   - isSortedBranches: " << isSortedBranches << std::endl;
    std::clog << "   - isSorted: " << isSorted << std::endl;
    hif::writeFile(std::clog, refExpression, false);
    std::clog << std::endl;
#endif

    // //////////////////////////
    // Try to simplify expression
    // //////////////////////////
    Value *simplified       = simplifyExpression(refExpression, _sem, _opt);
    const bool isSimplified = (simplified != nullptr);
    if (!isSimplified) {
        simplified = refExpression;
    } else if (isRebalanced) {
        delete refExpression;
    }

#ifdef DEBUG_EXPR_SCREEN
    std::clog << "[EXPR_" << __expressionId << "] - " << "06. After _simplifyExpression" << std::endl;
    hif::writeFile(std::clog, simplified, false);
    std::clog << std::endl;
#endif
    Type *vt = hif::semantics::getSemanticType(simplified, _sem);
    hif::EqualsOptions opt;
    opt.checkConstexprFlag = false;
    opt.checkSignedFlag    = false;
    opt.checkResolvedFlag  = false;
    const bool equals      = hif::equals(ot, vt);
    const bool equalsOpt   = hif::equals(ot, vt, opt);
    if (!equals && (equalsOpt || dynamic_cast<Expression *>(simplified) == nullptr)) {
        Value *op = (simplified == &o) ? hif::copy(simplified) : simplified;
        Cast *c   = new Cast();
        c->setValue(op);
        c->setType(hif::copy(ot));
        simplified = c;
    } else if (!equals && !equalsOpt) {
        // Manipulation seems unsafe: avoid this simplify.
        if (simplified != &o)
            delete simplified;
#ifdef DEBUG_EXPR_SCREEN
        std::clog << "NOT EQUALS! ABORT SIMPLIFICATION! END VISIT!" << std::endl;
        std::clog << "    - equals: " << equals << std::endl;
        std::clog << "    - equalsOpt: " << equalsOpt << std::endl;
        --__expressionId;
#endif
        return 0;
    }

#ifdef DEBUG_EXPR_SCREEN
    std::clog << "[EXPR_" << __expressionId << "] - " << "07. After potential cast adding" << std::endl;
    hif::writeFile(std::clog, simplified, false);
    std::clog << std::endl;
#endif

    _replace(&o, simplified);

    const bool possibleLoop = (isLinearized && isRebalanced) || (isSorted && isLinearized);

    if (possibleLoop)
        _addExpressionKey(expressionKey);

    // Need an extra simplification.
    // TODO Check, is computationally expansive?
    // Reference design: vhdl/custom/test_span_direction
    if (isSimplified || isChangedOperators || isLinearized || (isSortedBranches && !isRebalanced) ||
        (!isSortedBranches && isRebalanced)) {
        simplified->acceptVisitor(*this);
        if (possibleLoop)
            _removeExpressionKey(expressionKey);
    }

#ifdef DEBUG_EXPR_SCREEN
    std::clog << "[EXPR_" << __expressionId << "] - " << "END VISIT EXPRESSION!" << std::endl;
    --__expressionId;
#endif

    return 0;
}

int SimplifyVisitor::visitFieldReference(FieldReference &o)
{
    GuideVisitor::visitFieldReference(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifyDeclaration(&o);
    _simplifySemanticsType(&o);

    if (_simplifyNames(&o))
        return 0;
    if (_simplifyRecordValueFieldReference(&o))
        return 0;
    if (_pushFieldReference((&o)))
        return 0;

    return 0;
}

int SimplifyVisitor::visitFile(File &o)
{
    GuideVisitor::visitFile(o);
    _simplifyBaseType(&o);
    return 0;
}

int SimplifyVisitor::visitFor(For &o)
{
    GuideVisitor::visitFor(o);

    _simplifyCondition(o.getCondition(), &o);

    if (_simplifyUselessFor(&o))
        return 0;
    if (_simplifyConstantLoopFor(&o))
        return 0;

    return 0;
}

int SimplifyVisitor::visitForGenerate(ForGenerate &o)
{
    // A pre-visit is performed in order to simplify inner expressions,
    // without exploding the ForGenerate.
    bool restore            = _opt.simplify_generates;
    _opt.simplify_generates = false;
    GuideVisitor::visitForGenerate(o);
    _opt.simplify_generates = restore;

    _simplifyCondition(o.getCondition(), &o);

    if (_simplifyUselessFor(&o))
        return 0;
    if (_simplifyForGenerate(&o))
        return 0;

    return 0;
}

int SimplifyVisitor::visitFunctionCall(FunctionCall &o)
{
    GuideVisitor::visitFunctionCall(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifyDeclaration(&o);
    _simplifySemanticsType(&o);

    _simplifyTemplateParametersAssigns(&o);
    if (_simplifyFunctionCalls(&o))
        return 0;
    if (_simplifyIteratedConcat(&o))
        return 0;

    return 0;
}

int SimplifyVisitor::visitIdentifier(Identifier &o)
{
    GuideVisitor::visitIdentifier(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifyDeclaration(&o);
    _simplifySemanticsType(&o);
    if (_simplifyNames(&o))
        return 0;
    return 0;
}

int SimplifyVisitor::visitIntValue(IntValue &o)
{
    GuideVisitor::visitIntValue(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);
    _fixSyntacticType(&o);
    _fixNegativeValue(&o);
    return 0;
}

int SimplifyVisitor::visitIf(If &o)
{
    GuideVisitor::visitIf(o);
    if (_simplifyConstantCondition(&o))
        return 0;
    if (_simplifyMergingCases(&o))
        return 0;
    return 0;
}

int SimplifyVisitor::visitIfAlt(IfAlt &o)
{
    GuideVisitor::visitIfAlt(o);
    _simplifyCondition(o.getCondition(), &o);
    return 0;
}

int SimplifyVisitor::visitIfGenerate(IfGenerate &o)
{
    // A pre-visit is performed in order to simplify inner expressions,
    // without exploding the ForGenerate.
    bool restore            = _opt.simplify_generates;
    _opt.simplify_generates = false;
    GuideVisitor::visitIfGenerate(o);
    _opt.simplify_generates = restore;

    _simplifyCondition(o.getCondition(), &o);
    if (_simplifyIfGenerate(&o))
        return 0;
    return 0;
}

int SimplifyVisitor::visitInstance(Instance &o)
{
    GuideVisitor::visitInstance(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);
    _simplifyReferencedAssignList(o.portAssigns);
    return 0;
}

int SimplifyVisitor::visitLibrary(Library &o)
{
    GuideVisitor::visitLibrary(o);
    //_simplifyDeclaration(&o);
    return 0;
}

int SimplifyVisitor::visitMember(Member &o)
{
    GuideVisitor::visitMember(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);

    _fixBoundType(o.getIndex());
    _handleBound(o.getIndex());

    if (_simplifyMemberInternalCast(&o))
        return 0;
    if (_simplifySingleBitMember(&o))
        return 0;
    if (_simplifyBitvectorValueMember(&o))
        return 0;
    if (_simplifyAggregateMember(&o))
        return 0;
    if (_simplifySliceMember(&o))
        return 0;
    if (_simplifyBitwiseExpressionMember(&o))
        return 0;
    if (_simplifyConcatMember(&o))
        return 0;
    if (_simplifyUnrollAggregate(&o))
        return 0;

    return 0;
}

int SimplifyVisitor::visitParameter(Parameter &o)
{
    GuideVisitor::visitParameter(o);
    _simplifyLeftHandSideCasts(o.getType(), o.getValue(), &o);
    return 0;
}

int SimplifyVisitor::visitParameterAssign(ParameterAssign &o)
{
    GuideVisitor::visitParameterAssign(o);
    _simplifyDeclaration(&o);
    _simplifySemanticsType(&o);
    return 0;
}

int SimplifyVisitor::visitPointer(Pointer &o)
{
    GuideVisitor::visitPointer(o);
    _simplifyBaseType(&o);
    return 0;
}

int SimplifyVisitor::visitPort(Port &o)
{
    GuideVisitor::visitPort(o);
    _simplifyLeftHandSideCasts(o.getType(), o.getValue(), &o);
    return 0;
}

int SimplifyVisitor::visitPortAssign(PortAssign &o)
{
    GuideVisitor::visitPortAssign(o);
    _simplifyDeclaration(&o);
    _simplifySemanticsType(&o);
    _fixPortAssignAssignability(&o);
    return 0;
}

int SimplifyVisitor::visitProcedureCall(ProcedureCall &o)
{
    GuideVisitor::visitProcedureCall(o);
    _simplifyDeclaration(&o);

    //_simplifyReferencedAssignList(o.parameterAssigns);
    _simplifyReferencedAssignList(o.templateParameterAssigns);
    return 0;
}

int SimplifyVisitor::visitRange(Range &o)
{
    GuideVisitor::visitRange(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);

    // fix null range.
    _fixNullRange(&o);

    // fix syntactic types.
    _fixBoundType(o.getLeftBound());
    _fixBoundType(o.getRightBound());
    _handleBound(o.getLeftBound());
    _handleBound(o.getRightBound());
    return 0;
}

int SimplifyVisitor::visitRealValue(RealValue &o)
{
    GuideVisitor::visitRealValue(o);
    _simplifySemanticsType(&o);
    _fixSyntacticType(&o);
    _fixNegativeValue(&o);
    return 0;
}

int SimplifyVisitor::visitRecord(Record &o)
{
    GuideVisitor::visitRecord(o);
    _simplifyBaseType(&o);
    return 0;
}

int SimplifyVisitor::visitReference(Reference &o)
{
    GuideVisitor::visitReference(o);
    _simplifyBaseType(&o);
    return 0;
}

int SimplifyVisitor::visitReturn(Return &o)
{
    GuideVisitor::visitReturn(o);
    if (_simplifyReturnUselessCasts(&o))
        return 0;
    return 0;
}

int SimplifyVisitor::visitSignal(Signal &o)
{
    GuideVisitor::visitSignal(o);
    _simplifyLeftHandSideCasts(o.getType(), o.getValue(), &o);
    _ensureAssignability(&o);
    return 0;
}
int SimplifyVisitor::visitSlice(Slice &o)
{
    GuideVisitor::visitSlice(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);

    if (_simplifySliceToMember(&o))
        return 0;
    if (_simplifySliceConstantPrefix(&o))
        return 0;
    if (_simplifyUselessSlice(&o))
        return 0;
    if (_simplifyIntSlice(&o))
        return 0;
    if (_simplifyBitwiseExpressionSlice(&o))
        return 0;
    if (_simplifyArithExpressionSlice(&o))
        return 0;
    if (_simplifyConstantConcatSlice(&o))
        return 0;
    if (_simplifyNonConstantConcatSlice(&o))
        return 0;
    if (_pushSliceIntoCast(&o))
        return 0;
    if (_pushSliceIntoWhen(&o))
        return 0;
    if (_pushSliceIntoWith(&o))
        return 0;
    if (_transformSliceToCast(&o))
        return 0;
    return 0;
}
int SimplifyVisitor::visitSwitch(Switch &o)
{
    GuideVisitor::visitSwitch(o);

    _removeSwitchUselessCasts(&o);
    if (_simplifyConstantCondition(&o))
        return 0;
    if (_simplifyUselessAlts(&o))
        return 0;
    if (_simplifyMergingAlts(&o))
        return 0;

    return 0;
}

int SimplifyVisitor::visitString(String &o)
{
    GuideVisitor::visitString(o);

    _fixStringSpanInformation(&o);
    return 0;
}

int SimplifyVisitor::visitStringValue(StringValue &o)
{
    GuideVisitor::visitStringValue(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);
    _fixSyntacticType(&o);
    return 0;
}
int SimplifyVisitor::visitTypeReference(TypeReference &o)
{
    GuideVisitor::visitTypeReference(o);
    //_simplifyDeclaration(&o);
    _simplifyReferencedAssignList(o.templateParameterAssigns);
    if (_simplifyNames(&o))
        return 0;
    return 0;
}
int SimplifyVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    GuideVisitor::visitTypeTPAssign(o);
    _simplifyDeclaration(&o);
    return 0;
}

int SimplifyVisitor::visitValueStatement(ValueStatement &o)
{
    GuideVisitor::visitValueStatement(o);
    if (_simplifyValueStatementUselessCasts(&o))
        return 0;
    return 0;
}

int SimplifyVisitor::visitValueTP(ValueTP &o)
{
    GuideVisitor::visitValueTP(o);
    _simplifyLeftHandSideCasts(o.getType(), o.getValue(), &o);
    return 0;
}
int SimplifyVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    GuideVisitor::visitValueTPAssign(o);
    _simplifyDeclaration(&o);
    _simplifySemanticsType(&o);
    return 0;
}
int SimplifyVisitor::visitVariable(Variable &o)
{
    GuideVisitor::visitVariable(o);
    _simplifyLeftHandSideCasts(o.getType(), o.getValue(), &o);
    return 0;
}
int SimplifyVisitor::visitViewReference(ViewReference &o)
{
    GuideVisitor::visitViewReference(o);
    //_simplifyDeclaration(&o);
    return 0;
}
int SimplifyVisitor::visitWhen(When &o)
{
    GuideVisitor::visitWhen(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);

    if (_simplifyConstantCondition(&o))
        return 0;
    if (_simplifyMergingCases(&o))
        return 0;
    if (_simplifyWhenBoolConstants(&o))
        return 0;
    return 0;
}
int SimplifyVisitor::visitWhenAlt(WhenAlt &o)
{
    GuideVisitor::visitWhenAlt(o);
    _simplifyCondition(o.getCondition(), &o);
    return 0;
}
int SimplifyVisitor::visitWhile(While &o)
{
    GuideVisitor::visitWhile(o);
    _simplifyCondition(o.getCondition(), &o);

    if (_simplifyUselessWhile(&o))
        return 0;
    return 0;
}
int SimplifyVisitor::visitWith(With &o)
{
    if (_simplifyEmptyWith(&o))
        return 0;

    GuideVisitor::visitWith(o);
    hif::manipulation::sort(&o, _sem, _sortOpt);
    _simplifySemanticsType(&o);

    _removeWithUselessCasts(&o);
    if (_simplifyConstantCondition(&o))
        return 0;
    if (_simplifyUselessAlts(&o))
        return 0;
    if (_simplifyMergingAlts(&o))
        return 0;

    return 0;
}

} // namespace

// /////////////////////////////////////////////////////////////////////////
// SimplifyOptions
// /////////////////////////////////////////////////////////////////////////
SimplifyOptions::SimplifyOptions()
    : simplify_constants(false)
    , simplify_defines(true)
    , simplify_parameters(false)
    , simplify_template_parameters(false)
    , simplify_ctc_template_parameters(true)
    , simplify_non_ctc_template_parameters(true)
    , simplify_statements(false)
    , simplify_generates(false)
    , simplify_declarations(false)
    , simplify_semantics_types(false)
    , simplify_typereferences(false)
    , simplify_functioncalls(false)
    , replace_result(true)
    , behavior(BEHAVIOR_NORMAL)
    , context(nullptr)
    , root(nullptr)
{
    // ntd
}

SimplifyOptions::~SimplifyOptions()
{
    // ntd
}

SimplifyOptions::SimplifyOptions(const SimplifyOptions &o)
    : simplify_constants(o.simplify_constants)
    , simplify_defines(o.simplify_defines)
    , simplify_parameters(o.simplify_parameters)
    , simplify_template_parameters(o.simplify_template_parameters)
    , simplify_ctc_template_parameters(o.simplify_ctc_template_parameters)
    , simplify_non_ctc_template_parameters(o.simplify_non_ctc_template_parameters)
    , simplify_statements(o.simplify_statements)
    , simplify_generates(o.simplify_generates)
    , simplify_declarations(o.simplify_declarations)
    , simplify_semantics_types(o.simplify_semantics_types)
    , simplify_typereferences(o.simplify_typereferences)
    , simplify_functioncalls(o.simplify_functioncalls)
    , replace_result(o.replace_result)
    , behavior(o.behavior)
    , context(o.context)
    , root(o.root)
{
    // ntd
}

SimplifyOptions &SimplifyOptions::operator=(const SimplifyOptions &o)
{
    if (this == &o)
        return *this;

    simplify_constants                   = o.simplify_constants;
    simplify_defines                     = o.simplify_defines;
    simplify_parameters                  = o.simplify_parameters;
    simplify_template_parameters         = o.simplify_template_parameters;
    simplify_ctc_template_parameters     = o.simplify_ctc_template_parameters;
    simplify_non_ctc_template_parameters = o.simplify_non_ctc_template_parameters;
    simplify_statements                  = o.simplify_statements;
    simplify_generates                   = o.simplify_generates;
    simplify_declarations                = o.simplify_declarations;
    simplify_semantics_types             = o.simplify_semantics_types;
    simplify_typereferences              = o.simplify_typereferences;
    simplify_functioncalls               = o.simplify_functioncalls;
    replace_result                       = o.replace_result;
    behavior                             = o.behavior;
    context                              = o.context;
    root                                 = o.root;

    return *this;
}

SimplifyOptions &SimplifyOptions::setSimplifyConstants(const bool yes)
{
    simplify_constants = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setSimplifyDefines(const bool yes)
{
    simplify_defines = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setSimplifyParameters(const bool yes)
{
    simplify_parameters = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setSimplifyTemplateParameters(const bool yes)
{
    simplify_template_parameters = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setSimplifyCtcTemplateParameters(const bool yes)
{
    simplify_ctc_template_parameters = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setSimplifyNonCtcTemplateParameters(const bool yes)
{
    simplify_non_ctc_template_parameters = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setSimplifyStatements(const bool yes)
{
    simplify_statements = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setSimplifyGenerates(const bool yes)
{
    simplify_generates = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setSimplifyDeclarations(const bool yes)
{
    simplify_declarations = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setSimplifySemanticsTypes(const bool yes)
{
    simplify_semantics_types = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setSimplifyTypereferences(const bool yes)
{
    simplify_typereferences = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setSimplifyFunctioncalls(const bool yes)
{
    simplify_functioncalls = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setReplaceResult(const bool yes)
{
    replace_result = yes;
    return *this;
}

SimplifyOptions &SimplifyOptions::setBehavior(const SimplifyOptions::Behavior b)
{
    this->behavior = b;
    return *this;
}

SimplifyOptions &SimplifyOptions::setContext(Object *ctx)
{
    this->context = ctx;
    return *this;
}

SimplifyOptions &SimplifyOptions::setRoot(Object *r)
{
    this->root = r;
    return *this;
}

// ///////////////////////////////////////////////////////////////////
// simplify methods
// ///////////////////////////////////////////////////////////////////

Object *simplify(Object *obj, hif::semantics::ILanguageSemantics *refSem, const SimplifyOptions &opt)
{
    if (obj == nullptr)
        return nullptr;

    hif::application_utils::initializeLogHeader("HIF", "Simplify");
    messageAssert(!opt.simplify_declarations, "Symplify declaration not supported yet", nullptr, refSem);

    SimplifyVisitor v(opt, refSem);
    if (!opt.replace_result) {
        Object *o = hif::copy(obj);
        v.setResult(o);
        o->acceptVisitor(v);

        hif::application_utils::restoreLogHeader();
        return v.getResult();
    }

    v.setResult(obj);
    obj->acceptVisitor(v);

    hif::application_utils::restoreLogHeader();
    return v.getResult();
}

Object *getAggressiveSimplified(Object *o, hif::semantics::ILanguageSemantics *refSem)
{
    if (o == nullptr)
        return nullptr;
    SimplifyOptions sopt;
    sopt.simplify_constants           = true;
    sopt.simplify_template_parameters = true;
    sopt.simplify_defines             = true;
    //sopt.replace_result = false;
    //sopt.simplify_typereferences = true;

    Object *copy          = hif::copy(o);
    const bool canReplace = o->getParent() != nullptr;
    if (canReplace)
        o->replace(copy);
    Object *ret = simplify(copy, refSem, sopt);
    if (canReplace)
        ret->replace(o);
    return ret;
}

void simplify(BList<Object> &o, hif::semantics::ILanguageSemantics *refSem, const SimplifyOptions &opt)
{
    for (BList<Object>::iterator i = o.begin(); i != o.end(); ++i) {
        simplify(*i, refSem, opt);
    }
}

template <typename T>
typename SimplifiedType<T>::type *simplify(T *o, hif::semantics::ILanguageSemantics *refSem, const SimplifyOptions &opt)
{
    return dynamic_cast<typename SimplifiedType<T>::type *>(simplify(static_cast<Object *>(o), refSem, opt));
}

template <typename T> void simplify(BList<T> &o, hif::semantics::ILanguageSemantics *refSem, const SimplifyOptions &opt)
{
    simplify(o.template toOtherBList<Object>(), refSem, opt);
}

template <typename T>
typename SimplifiedType<T>::type *getAggressiveSimplified(T *o, hif::semantics::ILanguageSemantics *refSem)
{
    return dynamic_cast<typename SimplifiedType<T>::type *>(getAggressiveSimplified(static_cast<Object *>(o), refSem));
}

#define HIF_TEMPLATE_METHOD(T)                                                                                         \
    SimplifiedType<T>::type *simplify<T>(T * o, hif::semantics::ILanguageSemantics * refSem, const SimplifyOptions &opt)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

#define HIF_TEMPLATE_METHOD(T)                                                                                         \
    void simplify<T>(BList<T> & o, hif::semantics::ILanguageSemantics * refSem, const SimplifyOptions &opt)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

#define HIF_TEMPLATE_METHOD(T)                                                                                         \
    SimplifiedType<T>::type *getAggressiveSimplified<T>(T * o, hif::semantics::ILanguageSemantics * refSem)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

} // namespace manipulation
} // namespace hif
