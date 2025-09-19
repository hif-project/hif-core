/// @file standardizeHif.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <iostream>

#include "hif/hif.hpp"
#include "hif/semantics/standardization.hpp"

// /////////////////////////////////////////////////////////////////////
// TODO
// /////////////////////////////////////////////////////////////////////
//
// Possible problems that could require a fix:
// - When changing a pointer inside the map, all pointed object sub-nodes
//   can became invalid. Therefore, there sould be a method to refresh such
//   pointers. Known method which could create this bug:
//   * _mapType
//   * VisitAssign, when removing casts on target
//   * _mapConstValue, when replacing syntactic type.
//
// /////////////////////////////////////////////////////////////////////

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

namespace hif
{
namespace semantics
{

namespace
{ // anon namespace

typedef std::map<Object *, Object *> TreeMap;

// ///////////////////////////////////////////////////////////////////
// Support Visitors
// ///////////////////////////////////////////////////////////////////
class MapDeleteVisitor : public GuideVisitor
{
public:
    MapDeleteVisitor(TreeMap &map, Object *oldTree, Object *newTree, hif::semantics::ILanguageSemantics *sem);
    virtual ~MapDeleteVisitor();

    virtual int AfterVisit(Object &o);

private:
    // Disabled.
    MapDeleteVisitor(const MapDeleteVisitor &);
    MapDeleteVisitor &operator=(const MapDeleteVisitor &);

    TreeMap *_map;
    Object *_oldTree;
    Object *_newTree;
    hif::semantics::ILanguageSemantics *_sem;
};

MapDeleteVisitor::MapDeleteVisitor(
    TreeMap &map,
    Object *oldTree,
    Object *newTree,
    hif::semantics::ILanguageSemantics *sem)
    : _map(&map)
    , _oldTree(oldTree)
    , _newTree(newTree)
    , _sem(sem)
{
    // ntd
}
MapDeleteVisitor::~MapDeleteVisitor()
{
    // ntd
}
int MapDeleteVisitor::AfterVisit(Object &o)
{
    TreeMap::iterator i = _map->find(&o);
    if (i == _map->end())
        return 0;
    Object *dst = i->second;
    _map->erase(i);

    if (_newTree == nullptr)
        return 0;

    // The following code is intended in the case of a type
    // that contains declarations children, i.e., Enum/EnumValue
    // and Record/RecordValue.

    Declaration *decl = dynamic_cast<Declaration *>(&o);
    if (decl == nullptr)
        return 0;

    // Passing HIF semantics, since matchObject expects to work on
    // two trees with the same semantics.
    Object *matched =
        hif::manipulation::matchObject(dst, _oldTree, _newTree, hif::semantics::HIFSemantics::getInstance());
    messageAssert(matched != nullptr, "Unable to match declaration in new tree.", dst, _sem);
    (*_map)[&o] = matched;
    return 0;
}
// //////////////////////////////////////////////////
// Std Visitor
// Class to standardize HIF produced by a "literal"
// translation of source semantics code according
// with destination semantics.
// //////////////////////////////////////////////////
class HifStdVisitor : public GuideVisitor
{
public:
    HifStdVisitor(hif::semantics::ILanguageSemantics *src, hif::semantics::ILanguageSemantics *dst, CastMap &castMap);

    virtual ~HifStdVisitor();

    /// @brief Public function to get the corresponding object of
    /// <tt>o</tt> into the destination tree.
    System *getResult(System *o);

    virtual int visitAggregate(Aggregate &o);
    virtual int visitAggregateAlt(AggregateAlt &o);
    virtual int visitAlias(Alias &o);
    virtual int visitArray(Array &o);
    virtual int visitAssign(Assign &o);
    virtual int visitBit(Bit &o);
    virtual int visitBitValue(BitValue &o);
    virtual int visitBitvector(Bitvector &o);
    virtual int visitBitvectorValue(BitvectorValue &o);
    virtual int visitBool(Bool &o);
    virtual int visitBoolValue(BoolValue &o);
    virtual int visitBreak(Break &o);
    virtual int visitCast(Cast &o);
    virtual int visitChar(Char &o);
    virtual int visitCharValue(CharValue &o);
    virtual int visitConst(Const &o);
    virtual int visitContents(Contents &o);
    virtual int visitContinue(Continue &o);
    virtual int visitDesignUnit(DesignUnit &o);
    virtual int visitEnum(Enum &o);
    virtual int visitEnumValue(EnumValue &o);
    virtual int visitEvent(Event &o);
    virtual int visitExpression(Expression &o);
    virtual int visitField(Field &o);
    virtual int visitFieldReference(FieldReference &o);
    virtual int visitFile(File &o);
    virtual int visitFor(For &o);
    virtual int visitForGenerate(ForGenerate &o);
    virtual int visitFunction(Function &o);
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitGlobalAction(GlobalAction &o);
    virtual int visitEntity(Entity &o);
    virtual int visitIdentifier(Identifier &o);
    virtual int visitIf(If &o);
    virtual int visitIfAlt(IfAlt &o);
    virtual int visitIfGenerate(IfGenerate &o);
    virtual int visitInstance(Instance &o);
    virtual int visitInt(Int &o);
    virtual int visitIntValue(IntValue &o);
    virtual int visitLibraryDef(LibraryDef &o);
    virtual int visitLibrary(Library &o);
    virtual int visitMember(Member &o);
    virtual int visitNull(Null &o);
    virtual int visitTransition(Transition &o);
    virtual int visitParameterAssign(ParameterAssign &o);
    virtual int visitParameter(Parameter &o);
    virtual int visitPointer(Pointer &o);
    virtual int visitPortAssign(PortAssign &o);
    virtual int visitPort(Port &o);
    virtual int visitProcedure(Procedure &o);
    virtual int visitProcedureCall(ProcedureCall &o);
    virtual int visitRange(Range &o);
    virtual int visitReal(Real &o);
    virtual int visitRealValue(RealValue &o);
    virtual int visitRecord(Record &o);
    virtual int visitRecordValue(RecordValue &o);
    virtual int visitRecordValueAlt(RecordValueAlt &o);
    virtual int visitReference(Reference &o);
    virtual int visitReturn(Return &o);
    virtual int visitSignal(Signal &o);
    virtual int visitSigned(Signed &o);
    virtual int visitSlice(Slice &o);
    virtual int visitState(State &o);
    virtual int visitString(String &o);
    virtual int visitStateTable(StateTable &o);
    virtual int visitSwitchAlt(SwitchAlt &o);
    virtual int visitSwitch(Switch &o);
    virtual int visitSystem(System &o);
    virtual int visitStringValue(StringValue &o);
    virtual int visitTime(Time &o);
    virtual int visitTimeValue(TimeValue &o);
    virtual int visitTypeDef(TypeDef &o);
    virtual int visitTypeReference(TypeReference &o);
    virtual int visitTypeTPAssign(TypeTPAssign &o);
    virtual int visitTypeTP(TypeTP &o);
    virtual int visitUnsigned(Unsigned &o);
    virtual int visitValueStatement(ValueStatement &o);
    virtual int visitValueTPAssign(ValueTPAssign &o);
    virtual int visitValueTP(ValueTP &o);
    virtual int visitVariable(Variable &o);
    virtual int visitView(View &o);
    virtual int visitViewReference(ViewReference &o);
    virtual int visitWait(Wait &o);
    virtual int visitWhen(When &o);
    virtual int visitWhenAlt(WhenAlt &o);
    virtual int visitWhile(While &o);
    virtual int visitWith(With &o);
    virtual int visitWithAlt(WithAlt &o);

protected:
    typedef std::map<Object *, Object *> TreeMap;

    /// @brief The source semantics.
    hif::semantics::ILanguageSemantics *_srcSem;

    /// @brief The destination semantics.
    hif::semantics::ILanguageSemantics *_dstSem;

    /// @brief The destination languange Hif factory.
    hif::HifFactory _dstFactory;

    /// @brief If true rebase types according with destination semantics.
    bool _canRebaseTypes;

    /// @brief Maps objects of source tree to corresponding objects
    /// in destination tree.
    TreeMap _treeMap;

    /// @brief Maps of destination tree cast with source object type.
    CastMap &_castMap;

    /// @brief The trash.
    hif::Trash _trash;

    /// @name Basic utilities.
    /// Methods that are useful for the mapping of objects from
    /// source tree to destination tree.
    ///
    ///@{

    /// @brief If not already created in map:
    /// - creates a copy of the object without children nodes adding
    /// it into destination map.
    /// - insert the new object in destination tree in the same
    /// position of original object in source tree.
    ///
    /// @param v The object of source tree.
    /// @return true if the operation was successful.
    ///
    template <typename T> bool _dstCopyObject(T *v);

    /// @brief Delete the corresponding object into map.
    /// and remove the original entry of the map.
    ///
    /// @param v The object of source tree.
    ///
    void _mapDelete(Object *v);

    /// @brief Delete the corresponding object into map and all its
    /// subtree. Remove also the original entries of the map.
    ///
    /// @param v The object of source tree.
    /// @param newTree Eventual new tree to which map source tree
    /// declarations.
    ///
    void _mapDeleteTree(Object *v, Object *newTree = nullptr);

    /// @brief Return the corresponding object into map.
    ///
    /// @param v The object of source tree.
    /// @return The corresponding object into map.
    ///
    Object *_mapGet(Object *v);

    /// @brief Set new couple into map.
    ///
    /// @param k The object of source tree.
    /// @param v The corresponding object to set into map.
    ///
    void _mapSet(Object *k, Object *v);

    /// @brief Check compatibility of object couple.
    ///
    void _mapCheck(Object *k, Object *v);

    /// @brief Return the corresponding object into map.
    /// This is only a template version of _mapGet.
    ///
    /// @param v The object of source tree.
    /// @return The corresponding object into map.
    ///
    template <typename T> T *_mapTypedGet(T *v);

    ///@}

    /// @name Standardization utilities.
    /// General methods that collect some functionality useful to
    /// the standardization of particular classes of objects.
    ///@{

    /// @brief If given data declaration has not got initial value,
    /// asks source semantics to give a valid default value and sets it.
    ///
    /// @param o The data declaration.
    ///
    void _assureInitialValue(DataDeclaration *o);

    /// @brief Assure that PPAssigns have set correctly the direction field.
    void _fixPPAssignSrcDirection(PPAssign *o);

    /// @brief Create the declaration of the corresponding object
    /// into map. This is done getting the declaration of the source
    /// object and mapping in destination semantics calling the
    /// standardization.
    /// If the declaration was already mapped it set the declaration
    /// of destination object to the destination declaration.
    ///
    /// @param obj The source object that must have a declaration.
    ///
    void _dstGetDeclaration(Object *obj);

    /// @brief Return the given type mapped in destination semantics.
    /// On given type is performed standardization
    ///
    /// @param o The type to map.
    /// @param fresh True if returned type must be fresh and not
    /// inserted into the map.
    ///
    Type *_dstGetType(Type *o, bool fresh);

    /// @brief Given a list this method perform analysis of precision
    /// with op_eq of conditions of all elements and given <tt>srcRef</tt>.
    /// If need a cast on conditions this function adds casts on each
    /// element condition of <tt>dstList</tt> and on condition of
    /// <tt>dstRef</tt>.
    ///
    /// @param srcList The list to check of source tree.
    /// @param dstList The corresponding list elements of
    /// destination tree.
    /// @param srcRef The additional object to check (that can be
    /// for example the default value of a switch).
    /// @param dstRef The corresponding additional object of map.
    ///
    template <typename T>
    void _dstPushCastToConditions(BList<T> &srcList, BList<T> &dstList, Value *srcRef, Value *dstRef);

    /// @brief This method check if each element of the given list
    /// and the <tt>dstRef</tt> need a cast. In this case it add
    /// cast on destination element
    ///
    /// @param srcList The list to check of source tree.
    /// @param dstList The corresponding list elements of
    /// destination tree.
    /// @param srcDefault The additional object to check (that can be
    /// for example the default value of a switch).
    /// @param dstDefault The corresponding additional object of map.
    /// @param srcCastType The type of the eventual cast.
    /// Must be fresh.
    ///
    template <typename T>
    void _dstPushCastToElements(
        BList<T> &srcList,
        BList<T> &dstList,
        Value *srcDefault,
        Value *dstDefault,
        Type *srcCastType);

    /// @brief This method adds a cast on <tt>dstVal</tt> of type
    /// <tt>t</tt> in destination tree.
    /// It also refresh the map link of <tt>srcVal</tt> with created
    /// cast.
    ///
    /// @param srcVal The source value.
    /// @param dstVal The destination value.
    /// @param t The type of the cast.
    ///
    void _dstReplaceWithCast(Value *srcVal, Value *dstVal, Type *t);

    /// @brief Used in case of concat expressions, this method
    /// preserves spans of the <tt>rangeType</tt> into <tt>castType</tt>.
    ///
    /// @param rangeType The destination object type.
    /// @param castType The cast type.
    ///
    void _maintainSpan(Type *rangeType, Type *castType);

    /// @brief Removes all external cast for each element of the
    /// given list.
    ///
    /// @param srcList The source list.
    /// @param dstList The destination list to manage.
    ///
    void _removeListCasts(BList<Value> &srcList, BList<Value> &dstList);

    /// @brief Check the compatibility of bound testing operation
    /// op_plus between bounds. If Operation is not possible, this
    /// method add a cast to a calculated type that is the type
    /// of bound with max precision.
    ///
    /// @param left The left bound of source tree.
    /// @param right The right bound of source tree.
    ///
    void _makeCompatibleBounds(Value *left, Value *right);

    /// @brief Check if destination bound can has not got the
    /// syntactical type. This is possible only if the corresponding
    /// type of the constant in destination semantics is the same
    /// of bound type. If not this method create and return a cast
    /// of the bound to the mapped type of the source bound in
    /// destination language.
    ///
    /// @param dstBound The destination bound.
    /// @return The destination value that can be the give dstBound
    /// or the created cast.
    ///
    Value *_dstHandleBound(Value *dstBound);

    /// @brief This method sorts the @p dstList declarations w.r.t. @p srcList.
    ///
    /// @param srcList The source list.
    /// @param dstList The destination list.
    ///
    void _sortDeclarations(BList<Declaration> &srcList, BList<Declaration> &dstList);

    ///@}

    /// @name Mapping utilities.
    /// Methods used by standardizaion of corresponding class or type
    /// of nodes.
    ///@{

    /// @brief Manage assigns.
    /// It removes eventual casts from target and adds cast on the
    /// source if necessary.
    ///
    /// @param o The assign object.
    ///
    void _mapAssign(Assign *o);

    /// @brief Manage conditions.
    /// If needed add a cast to mapped bool on the condition.
    ///
    /// @param o The condition.
    ///
    void _mapCondition(Value *o);

    /// @brief Manage const values.
    /// Check if the mapped type is good w.r.t. desination semantics.
    /// If not, it tries to transform constant to mapped type and, if
    /// transform constant is not possibile, it add a cast to
    /// mapped type.
    ///
    /// @param o The const value.
    ///
    void _mapConstValue(ConstValue *o);

    /// @brief Manage expressions.
    /// It asks semantics algorithm if operands or result need
    /// a cast and eventually add them.
    ///
    /// @param o The expression.
    ///
    void _mapExpression(Expression *o);

    /// @brief Standardizes the given index value.
    /// It builds a standard interger w.r.t. destination semantics
    /// and add a cast if given index type is not compatible.
    ///
    /// @param o The index.
    ///
    void _mapIndex(Value *o);

    /// @brief Manage assign between declaration and initial value.
    /// It ask the semantics if operation assign is valid.
    /// If not it add a cast of the destination initial value
    /// to destination declaration type.
    ///
    /// @param o The data declaration from which give the initial value.
    ///
    void _mapInitialValue(DataDeclaration *o);

    /// @brief Manage referenced assigns.
    /// It checks if operation assign is valid w.r.t. the destination
    /// semantics. If not, add a cast of destination actual element
    /// to destination formal element.
    ///
    /// @param o The referenced assign object.
    ///
    template <typename T> void _mapReferencedAssign(T *o, const hif::Operator op);

    /// @brief Manage referenced assign list of Type TP and Value TP.
    /// For each ValueTPAssign it call _mapReferencedAssign function.
    ///
    /// @param list The tp assign list.
    ///
    void _mapReferencedAssignList(BList<TPAssign> &list);

    /// @brief Manage referenced assign list of Parameter Assigns.
    /// For each element it call _mapReferencedAssign function.
    ///
    /// @param list The parameter assign list.
    ///
    void _mapReferencedAssignList(BList<ParameterAssign> &list);

    /// @brief Manage referenced assign list of Port Assign.
    /// For each element it call _mapReferencedAssign function.
    ///
    /// @param list The port assign list.
    ///
    void _mapReferencedAssignList(BList<PortAssign> &list);

    /// @brief Manage the returns.
    /// It search the function declaration and checks the compatibility
    /// of its return type with the value of given return.
    /// If assign is not valid it adds a cast of destination value
    /// to destination function returned type.
    ///
    /// @param o The return object.
    ///
    void _mapReturn(Return *o);

    /// @brief Manage mapping of Types.
    /// It gets and replace the corresponding type w.r.t.
    /// destination semantics.
    ///
    /// @param o The source type.
    ///
    void _mapType(Type *o);

    /// @brief Manage general values.
    /// It gets the source type and map it into destination semantics.
    /// It checks that type with the type of de destination value.
    /// If they are not compatible, it adds a cast to destination
    /// values.
    ///
    /// @param o The source value.
    ///
    void _mapValue(Value *o);

    /// @brief Entry point for managing slices.
    /// @param o The Slice.
    void _mapSlice(Slice *o);

    /// @brief Manage slices.
    /// It gets the source and the destination type of prefix and
    /// fix slice bounds w.r.t. isSyntacticTypeRebased and isSliceTypeRebased
    /// of source and destination semantics
    ///
    /// @param o The slice.
    ///
    void _mapSliceSpan(Slice *o);

    /// @brief Entry point for managing members.
    /// @param o The Member.
    void _mapMember(Member *o);

    /// @brief Manage member indexes.
    /// It gets the source and the destination type of prefix and
    /// fix member index w.r.t. isSyntacticTypeRebased and isSliceTypeRebased
    /// of source and destination semantics
    ///
    /// @param o The Member.
    ///
    void _mapMemberIndex(Member *o);

    ///@}

    /// @name Mapping utilities.
    ///@{

    /// @brief Struct representing out of analyzePrecisionType function.
    ///
    struct AnalyzeInfo {
        AnalyzeInfo();
        ~AnalyzeInfo();
        AnalyzeInfo(const AnalyzeInfo &t);
        AnalyzeInfo &operator=(const AnalyzeInfo &t);

        /// @brief Type pointer representing the type in which operand 1 have
        /// to be eventually cast to be compliant with destination language semantics.
        Type *operandType1;

        /// @brief Type pointer representing the type in which operand 2 have
        /// to be eventually cast to be compliant with destination language semantics.
        Type *operandType2;

        /// @brief Type pointer representing the type in which result have
        /// to be eventually cast to be compliant with destination language semantics.
        Type *resultType;

        /// @brief The result operator according to eventual changes on the
        /// operand types.
        Operator resultOperator;

        /// @brief Flag to indicate if the expression analysis returns an error.
        bool error;
    };

    /// @brief Function to get information about eventual casts to do on operands or
    /// on result of an expression which works on the given type to be compliant
    /// with the destination semantics.
    ///
    AnalyzeInfo _analyzeExprType(const AnalyzeParams &params);

    /// @brief Function to get informations about eventual casts to do on
    ///  operands of an assignment, an aggregate and all the other objects
    ///  with multiple Value.
    /// This function behaves like the analyzeExprType, but it uses the
    /// precision instead of the returned type as intermediate value for the
    /// computation. The operation considered is the equality.
    ///
    /// @param params struct of analyze_params_t type containing analysis
    ///  parameters. The operation is ignored. In this function is used
    ///  only the equality.
    ///
    /// @return struct of analayze_info_t containing informations about
    ///  eventual cast to do to be destination language's compliant.
    ///
    AnalyzeInfo _analyzePrecisionType(const AnalyzeParams &params);

    /// @brief Function to perform semantics analysis algorithm
    ///
    void _performSemanticsAlgorithm(Type *t1, Type *t2, Operator operation, AnalyzeInfo &result, Object *startingObj);

    /// @brief Function to control that, given two types, a cast is
    /// need on operands or on the result basing on type length.
    ///
    void _handleLength(Type *source, Type *dest, Type *resultType, AnalyzeInfo &result, Operator operation);

    /// @brief TODO
    void _typeSetPrecision(Type *&result, Type *precision);

    ///@}
    bool _isTypedRange(Range *tSpan, hif::semantics::ILanguageSemantics *sem);

    bool _isTop(Value *o);

    void _fillCastMap(Cast *key, Type *t);

private:
    // disabled
    HifStdVisitor &operator=(const HifStdVisitor &v);
    HifStdVisitor(const HifStdVisitor &v);
};
HifStdVisitor::HifStdVisitor(ILanguageSemantics *src, ILanguageSemantics *dst, CastMap &castMap)
    : GuideVisitor(0)
    , _srcSem(src)
    , _dstSem(dst)
    , _dstFactory(dst)
    , _canRebaseTypes(true)
    , _treeMap()
    , _castMap(castMap)
    , _trash()
{
    // Nothing to do.
}
HifStdVisitor::~HifStdVisitor() { _trash.clear(); }
System *HifStdVisitor::getResult(System *o) { return _mapTypedGet(o); }
// //////////////////////////////////////////////////
// StdVisitor support methods
// //////////////////////////////////////////////////

template <typename T> bool HifStdVisitor::_dstCopyObject(T *v)
{
    Object *o = _mapGet(v);

    // Setting expand mode for declarations.
    hif::manipulation::MatchedInsertType type;
    if (dynamic_cast<Declaration *>(v) != nullptr) {
        type = hif::manipulation::MatchedInsertType::TYPE_EXPAND;
    } else {
        type = hif::manipulation::MatchedInsertType::TYPE_ERROR;
    }

    if (o != nullptr) {
        // already standardized (probably by _getTreeDeclaration)
        // Insert into parent and do not standardize again!
        if (v->getParent() == nullptr)
            return false;
        if (o->getParent() != nullptr)
            return false;
        if (isSemanticsType(dynamic_cast<Type *>(v)))
            return false;
        Object *oParent = _mapGet(v->getParent());
        bool result     = hif::manipulation::matchedInsert(o, oParent, v, v->getParent(), type);

        messageDebugIfFails(result, "o = ", o, _dstSem);
        messageDebugIfFails(result, "oParent = ", oParent, _dstSem);
        messageDebugIfFails(result, "v = ", v, _srcSem);
        messageDebugIfFails(result, "vParent = ", v->getParent(), _srcSem);
        messageAssert(result, "Matched insert fails (1).", v, _srcSem);
        return false;
    }

    hif::CopyOptions opt;
    opt.copyChildObjects   = false;
    opt.copySemanticsTypes = false;
    opt.copyDeclarations   = false;
    T *dstObj              = hif::copy(v, opt);
    _mapSet(v, dstObj);
    if (v->getParent() == nullptr)
        return true;
    Object *newParent = _mapGet(v->getParent());
    if (newParent == nullptr) {
        DesignUnit *duDecl  = dynamic_cast<DesignUnit *>(v->getParent());
        LibraryDef *libDecl = dynamic_cast<LibraryDef *>(v->getParent());
        View *viewDecl      = dynamic_cast<View *>(v->getParent());
        Enum *enumDecl      = dynamic_cast<Enum *>(v->getParent());
        Record *recDecl     = dynamic_cast<Record *>(v->getParent());
        TypeDef *tdDecl     = dynamic_cast<TypeDef *>(v->getParent());

        if (duDecl == nullptr && libDecl == nullptr && viewDecl == nullptr && enumDecl == nullptr &&
            recDecl == nullptr && tdDecl == nullptr)
            return true;

        // Forcing full copy:
        // E.g.: visitInstance --> visit its decl (entity)
        // --> _dstCopyObject() --> if (! parent already mapped)
        // --> visitView() --> no visit entity children (ports)
        // --> visit content has refs to ports
        // --> ports decls are inserted not in order
        // Ref design: verilog/openCores/aes when aes is parsed before its submodules.
        // E.g.2: visitIdentifier --> visitEnumValue
        // --> if (!parentAlreadyMapped) --> visitTypeRef() --> VisitEnum
        // --> maptype() --> deleteTree(i.e. enum and enumValue and typeref)
        // --> map typeref
        // Ref design: verilog/openCores/aes
        _mapDelete(v);

        v->getParent()->acceptVisitor(*this);
        newParent = _mapGet(v->getParent());
        messageAssert(newParent != nullptr, "Unexpected nullptr parent.", v, _srcSem);
        return false;
    }
    if (isSemanticsType(dynamic_cast<Type *>(v)))
        return true;
    bool result = hif::manipulation::matchedInsert(dstObj, newParent, v, v->getParent(), type);

    messageAssert(result, "Matched insert fails (2).", v, _srcSem);
    return true;
}

void HifStdVisitor::_mapDelete(Object *v)
{
    TreeMap::iterator i = _treeMap.find(v);
    if (i == _treeMap.end())
        return;
    Object *o = i->second;
    _treeMap.erase(i);
    delete o;
}
void HifStdVisitor::_mapDeleteTree(Object *v, Object *newTree)
{
    Object *dst = _mapGet(v);
    if (dst == nullptr)
        return;
    MapDeleteVisitor vis(_treeMap, dst, newTree, _dstSem);
    v->acceptVisitor(vis);

    hif::HifTypedQuery<Cast> q;
    std::list<Cast *> castList;
    hif::search(castList, dst, q);
    for (std::list<Cast *>::iterator i = castList.begin(); i != castList.end(); ++i) {
        Cast *c              = *i;
        CastMap::iterator it = _castMap.find(c);
        if (it == _castMap.end())
            continue;
        delete it->second;
        _castMap.erase(it);
    }

    delete dst;
}
Object *HifStdVisitor::_mapGet(Object *v)
{
    TreeMap::iterator i = _treeMap.find(v);
    if (i == _treeMap.end())
        return nullptr;

    Object *k = i->second;
    _mapCheck(k, v);
    return k;
}
void HifStdVisitor::_mapCheck(Object *k, Object *v)
{
    messageAssert(
        (dynamic_cast<Action *>(k) != nullptr) == (dynamic_cast<Action *>(v) != nullptr), "Action not matched", k,
        _srcSem);
    messageAssert(
        (dynamic_cast<Alt *>(k) != nullptr) == (dynamic_cast<Alt *>(v) != nullptr), "Alt not matched", k, _srcSem);
    messageAssert(
        (dynamic_cast<Declaration *>(k) != nullptr) == (dynamic_cast<Declaration *>(v) != nullptr),
        "Declaration not matched", k, _srcSem);
    messageAssert(
        (dynamic_cast<GlobalAction *>(k) != nullptr) == (dynamic_cast<GlobalAction *>(v) != nullptr),
        "GlobalAction not matched", k, _srcSem);
    messageAssert(
        (dynamic_cast<Type *>(k) != nullptr) == (dynamic_cast<Type *>(v) != nullptr), "Type not matched", k, _srcSem);
    messageAssert(
        (dynamic_cast<TypedObject *>(k) != nullptr) == (dynamic_cast<TypedObject *>(v) != nullptr),
        "TypedObject not matched", k, _srcSem);
}
void HifStdVisitor::_mapSet(Object *k, Object *v)
{
    _mapCheck(k, v);
    _treeMap[k] = v;
}
template <typename T> T *HifStdVisitor::_mapTypedGet(T *v) { return dynamic_cast<T *>(_mapGet(v)); }
void HifStdVisitor::_assureInitialValue(DataDeclaration *o)
{
    if (o->getValue() != nullptr)
        return;

    Value *vo = _srcSem->getTypeDefaultValue(o->getType(), o);
    o->setValue(vo);

    hif::manipulation::prefixTree(vo, _srcSem);
    //messageAssert ( vo != nullptr, "Missing initial value.", o, _srcSem );
}
void HifStdVisitor::_fixPPAssignSrcDirection(PPAssign * /*o*/)
{
    // At the moment PPAssign cannot have the direction set.
    // Declaration * decl = getDeclaration(o, _srcSem);
    // messageAssert(decl != nullptr, "Declaration not found", o, _srcSem);
    // Port * port = dynamic_cast<Port *>(decl);
    // Parameter * param = dynamic_cast<Parameter *>(decl);
    // messageAssert(port != nullptr || param != nullptr, "Unexpected declaration type.", decl, _srcSem);
    // if (port != nullptr) o->setDirection(port->getDirection());
    // else o->setDirection(param->getDirection());
}
void HifStdVisitor::_dstGetDeclaration(Object *obj)
{
    Object *dstObj = _mapGet(obj);
    messageAssert(dstObj != nullptr, "Object not found in map", obj, _dstSem);

    Declaration *decl  = getDeclaration(obj, _srcSem);
    bool declIsInCache = hif::manipulation::isInCache(decl);

    if (dynamic_cast<Instance *>(obj) != nullptr) {
        Instance *ii      = static_cast<Instance *>(obj);
        TypeReference *tr = dynamic_cast<TypeReference *>(ii->getReferencedType());
        ViewReference *vr = dynamic_cast<ViewReference *>(ii->getReferencedType());
        Library *l        = dynamic_cast<Library *>(ii->getReferencedType());
        if (tr != nullptr) {
            // Here, trust the declaration returned by getDeclaration. A TypeRef
            // to a ViewRef is already managed there.
            if (decl == nullptr)
                return;
        } else if (vr != nullptr) {
            messageAssert(decl != nullptr, "Missing source declaration.", obj, _srcSem);
        } else if (l != nullptr) {
            messageAssert(decl == nullptr, "Found declaration for instance of a library.", obj, _srcSem);
            return;
        }
    }

    messageAssert(decl != nullptr, "Declaration not found.", obj, _srcSem);

    Object *dstDecl = _mapGet(decl);
    if (dstDecl == nullptr) {
        // Not yet mapped.
        // In case of std function call,
        // we perform parameter assign mapping by
        // using the standard src signature mapped in dst sem,
        // and then, after guide visitor, we perform the actual
        // call mapping.
        decl->acceptVisitor(*this);
        dstDecl = _mapGet(decl);
        messageAssert(dstDecl != nullptr, "Expected mapped declaration", decl, _dstSem);

        if (declIsInCache) {
            // since original declaration is in cache (that is for example
            // in case of instantiate) add the mapped declaration in cache too.
            Declaration *dstDeclaration = dynamic_cast<Declaration *>(dstDecl);
            messageAssert(dstDeclaration != nullptr, "Expected declaration type", dstDecl, _dstSem);
            hif::manipulation::addInCache(dstDeclaration);
        }
    }

    messageAssert(dstDecl != nullptr, "Declaration not found in destination tree", obj, _dstSem);
    setDeclaration(dstObj, dstDecl);
}
Type *HifStdVisitor::_dstGetType(Type *o, bool /*fresh*/)
{
    Type *t = nullptr;

    bool restore    = _canRebaseTypes;
    _canRebaseTypes = false;

    t = _mapTypedGet(o);
    if (t != nullptr)
        return hif::copy(t);
    o->acceptVisitor(*this);
    t = hif::copy(_mapTypedGet(o));
    _mapDeleteTree(o);

    _canRebaseTypes = restore;

    return t;
}
template <typename T>
void HifStdVisitor::_dstPushCastToConditions(BList<T> &srcList, BList<T> &dstList, Value *srcRef, Value *dstRef)
{
    std::list<Type *> altTypes;

    for (typename BList<T>::iterator i = srcList.begin(); i != srcList.end(); ++i) {
        for (BList<Value>::iterator j = (*i)->conditions.begin(); j != (*i)->conditions.end(); ++j) {
            if (dynamic_cast<Range *>(*j) != nullptr) {
                Range *r = static_cast<Range *>(*j);

                Type *altTypeLeft = getSemanticType(r->getLeftBound(), _srcSem);
                messageAssert(altTypeLeft != nullptr, "Cannot type left bound of condition", r, _srcSem);
                altTypes.push_back(altTypeLeft);

                Type *altTypeRight = getSemanticType(r->getRightBound(), _srcSem);
                messageAssert(altTypeRight != nullptr, "Cannot type right bound of condition", r, _srcSem);
                altTypes.push_back(altTypeRight);
            } else {
                Type *altType = getSemanticType(*j, _srcSem);
                messageAssert(altType != nullptr, "Cannot type condition", *j, _srcSem);
                altTypes.push_back(altType);
            }
        }
    }
    altTypes.push_back(getSemanticType(srcRef, _srcSem));
    messageDebugAssert(!altTypes.empty(), "Unexpected empty alts types", nullptr, _srcSem);

    AnalyzeParams altsParams;
    altsParams.operation    = op_case_eq;
    altsParams.operandsType = altTypes;
    altsParams.startingObj  = srcRef->getParent();

    AnalyzeInfo altsInfo = _analyzePrecisionType(altsParams);

    messageAssert(!altsInfo.error, "Error during type analysis.", srcList.getParent(), _srcSem);

    if (altsInfo.operandType1 == nullptr)
        return;

    messageAssert(
        srcList.size() == dstList.size(), "Source and destination list must have same size (1).", srcList.getParent(),
        _srcSem);

    typename BList<T>::iterator is = srcList.begin();
    for (typename BList<T>::iterator id = dstList.begin(); id != dstList.end(); ++id, ++is) {
        T *srcAlt = *is;
        T *dstAlt = *id;
        messageAssert(
            srcAlt->conditions.size() == dstAlt->conditions.size(),
            "Source and destination list must have same size (2).", srcList.getParent(), _srcSem);

        BList<Value>::iterator js = srcAlt->conditions.begin();
        for (BList<Value>::iterator jd = dstAlt->conditions.begin(); jd != dstAlt->conditions.end(); ++jd, ++js) {
            Value *srcAltCond = *js;
            Value *dstAltCond = *jd;
            if (dynamic_cast<Range *>(dstAltCond) != nullptr) {
                Range *srcAltRangeCond      = static_cast<Range *>(srcAltCond);
                Value *srcAltRangeCondLeft  = srcAltRangeCond->getLeftBound();
                Value *srcAltRangeCondRight = srcAltRangeCond->getRightBound();
                Range *dstAltRangeCond      = static_cast<Range *>(dstAltCond);
                Value *dstAltRangeCondLeft  = dstAltRangeCond->getLeftBound();
                Value *dstAltRangeCondRight = dstAltRangeCond->getRightBound();

                Type *dstAltRangeCondLeftType             = getSemanticType(dstAltRangeCondLeft, _dstSem);
                Type *dstAltRangeCondRightType            = getSemanticType(dstAltRangeCondRight, _dstSem);
                ILanguageSemantics::ExpressionTypeInfo eL = _dstSem->getExprType(
                    altsInfo.operandType1, dstAltRangeCondLeftType, op_case_eq, dstAltRangeCondLeft);
                ILanguageSemantics::ExpressionTypeInfo eR = _dstSem->getExprType(
                    altsInfo.operandType1, dstAltRangeCondRightType, op_case_eq, dstAltRangeCondRight);
                if (eL.returnedType == nullptr) {
                    _dstReplaceWithCast(srcAltRangeCondLeft, dstAltRangeCondLeft, hif::copy(altsInfo.operandType1));
                    hif::manipulation::assureSyntacticType(dstAltRangeCondLeft, _dstSem);
                }
                if (eR.returnedType == nullptr) {
                    _dstReplaceWithCast(srcAltRangeCondRight, dstAltRangeCondRight, hif::copy(altsInfo.operandType1));
                    hif::manipulation::assureSyntacticType(dstAltRangeCondRight, _dstSem);
                }
            } else {
                Type *dstAltCondType = hif::semantics::getSemanticType(dstAltCond, _dstSem);
                ILanguageSemantics::ExpressionTypeInfo e =
                    _dstSem->getExprType(altsInfo.operandType1, dstAltCondType, op_case_eq, dstAltCond);
                if (e.returnedType == nullptr) {
                    _dstReplaceWithCast(srcAltCond, dstAltCond, hif::copy(altsInfo.operandType1));
                }
            }
        }
    }

    _dstReplaceWithCast(srcRef, dstRef, hif::copy(altsInfo.operandType1));
}
template <typename T>
void HifStdVisitor::_dstPushCastToElements(
    BList<T> &srcList,
    BList<T> &dstList,
    Value *srcDefault,
    Value *dstDefault,
    Type *srcCastType)
{
    messageAssert(
        srcList.size() == dstList.size(), "Source and destination list must have same size.", srcList.getParent(),
        _srcSem);

    typename BList<T>::iterator j = dstList.begin();
    for (typename BList<T>::iterator i = srcList.begin(); i != srcList.end(); ++i, ++j) {
        _dstReplaceWithCast((*i)->getValue(), (*j)->getValue(), hif::copy(srcCastType));
    }

    if (srcDefault != nullptr) {
        messageAssert(dstDefault != nullptr, "Not found mapping of default.", srcDefault, _srcSem);

        _dstReplaceWithCast(srcDefault, dstDefault, hif::copy(srcCastType));
    }
}
void HifStdVisitor::_dstReplaceWithCast(Value *srcVal, Value *dstVal, Type *t)
{
    // creating cast.
    Cast *c = new Cast();
    c->setType(t);
    dstVal->replace(c);
    c->setValue(dstVal);

    Type *srcType = hif::semantics::getBaseType(hif::semantics::getSemanticType(srcVal, _srcSem), false, _srcSem);
    messageAssert(srcType != nullptr, "Cannot type value", srcVal, _srcSem);

    _fillCastMap(c, hif::copy(srcType));

    // changing the corresponding object in map.
    _mapSet(srcVal, c);
}
void HifStdVisitor::_maintainSpan(Type *rangeType, Type *castType)
{
    messageAssert(rangeType != nullptr, "Passed nullptr range type", nullptr, _dstSem);
    messageAssert(castType != nullptr, "Passed nullptr cast type", nullptr, _dstSem);

    Range *ro = nullptr;
    if (dynamic_cast<Array *>(castType) != nullptr) {
        // Special case: array of type T with concat value of type T.
        // The calculated cast type must be an array of size 1.
        // Ref design: vhdl/openCores/avs_aes (row2_out assignement)
        Array *arr    = static_cast<Array *>(castType);
        Type *arrBt   = getBaseType(arr->getType(), false, _dstSem, true);
        Type *rangeBt = getBaseType(rangeType, false, _dstSem, true);

        hif::EqualsOptions eqOpt;
        eqOpt.checkConstexprFlag = false;
        if (hif::equals(arrBt, rangeBt, eqOpt)) {
            ro = new Range(0, 0);
        }
    }

    // 1- Get original operator's span:
    if (ro == nullptr)
        ro = hif::copy(hif::typeGetSpan(rangeType, _dstSem));
    if (ro == nullptr)
        return;

    // 2- Create the type to use for cast:
    hif::typeSetSpan(castType, ro, _dstSem, true);
}
void HifStdVisitor::_removeListCasts(BList<Value> &srcList, BList<Value> &dstList)
{
    messageAssert(srcList.size() == dstList.size(), "List must have same size.", srcList.getParent(), _srcSem);

    BList<Value>::iterator d = dstList.begin();
    for (BList<Value>::iterator s = srcList.begin(); s != srcList.end(); ++s, ++d) {
        if (dynamic_cast<Cast *>(*d) == nullptr)
            continue;

        Value *dstChild = hif::copy(hif::getChildSkippingCasts(*d));
        (*d)->replace(dstChild);
        _mapDeleteTree(*s, dstChild);
        _mapSet(*s, dstChild);
    }
}
void HifStdVisitor::_makeCompatibleBounds(Value *left, Value *right)
{
    Value *dstLeft  = _mapTypedGet(left);
    Value *dstRight = _mapTypedGet(right);

    Type *dstLeftType = getSemanticType(dstLeft, _dstSem);
    messageAssert(dstLeftType != nullptr, "Cannot type left bound in destination semantics.", dstLeft, _dstSem);

    Type *dstRightType = getSemanticType(dstRight, _dstSem);
    messageAssert(dstRightType != nullptr, "Cannot type right bound in destination semantics.", dstRight, _dstSem);

    messageAssert(dstLeft != nullptr, "Cannot find destination tree value", left, _dstSem);
    messageAssert(dstRight != nullptr, "Cannot find destination tree value", right, _dstSem);
    ILanguageSemantics::ExpressionTypeInfo res =
        _dstSem->getExprType(dstLeftType, dstRightType, op_plus, left->getParent());

    if (res.returnedType != nullptr)
        return;

    // Assuming that bounds are already converted to mapped int
    // by previous fixes.
    bool resIsConstexpr    = typeIsConstexpr(dstLeftType, _dstSem) && typeIsConstexpr(dstRightType, _dstSem);
    bool resIsSigned       = typeIsSigned(dstLeftType, _dstSem) && typeIsSigned(dstRightType, _dstSem);
    Range *dstLeftTypeSpan = typeGetSpan(dstLeftType, _dstSem);
    messageAssert(dstLeftTypeSpan != nullptr, "Cannot find dstLeftType span", dstLeftType, _dstSem);
    Range *dstRightTypeSpan = typeGetSpan(dstRightType, _dstSem);
    messageAssert(dstRightTypeSpan != nullptr, "Cannot find dstRightType span", dstRightType, _dstSem);
    Range *resSpan = rangeGetMax(dstLeftTypeSpan, dstRightTypeSpan, _dstSem);

    hif::CopyOptions opt;
    opt.copyChildObjects    = false;
    Type *maxPrecisionBound = hif::copy(dstLeftType, opt);
    typeSetConstexpr(maxPrecisionBound, resIsConstexpr);
    typeSetSigned(maxPrecisionBound, resIsSigned, _dstSem);
    typeSetSpan(maxPrecisionBound, resSpan, _dstSem, true);

    // Fixing casts on left bound:
    if (!hif::equals(dstLeftType, maxPrecisionBound)) {
        // need a cast to mapped type
        _dstReplaceWithCast(left, dstLeft, hif::copy(maxPrecisionBound));
    }

    // Fixing casts on right bound:
    if (!hif::equals(dstRightType, maxPrecisionBound)) {
        // need a cast to mapped type
        _dstReplaceWithCast(right, dstRight, hif::copy(maxPrecisionBound));
    }

    delete maxPrecisionBound;
}
void HifStdVisitor::_mapAssign(Assign *o)
{
    Assign *dstObj = _mapTypedGet(o);
    messageAssert(dstObj != nullptr, "Object not found in map", o, _dstSem);

    CopyOptions opt;
    //opt.copySemanticsTypes = true;

    // Generally an assignment has the form expression = expression;
    // during children visit expression on LHS can be transformed in a
    // CAST node. In this case CAST have to be removed
    Value *dstTarget = hif::getChildSkippingCasts(dstObj->getLeftHandSide());
    if (dstTarget != dstObj->getLeftHandSide()) {
        dstTarget = hif::copy(dstTarget, opt);
        dstObj->setLeftHandSide(dstTarget);
        _mapDeleteTree(o->getLeftHandSide(), dstTarget);
        _mapSet(o->getLeftHandSide(), dstObj->getLeftHandSide());
    }

    // Now managing assignability:
    Type *targetType = getSemanticType(dstObj->getLeftHandSide(), _dstSem);
    messageAssert(
        targetType != nullptr, "Cannot type the destination left hand side of assign.", dstObj->getLeftHandSide(),
        _dstSem);

    Type *sourceType = getSemanticType(dstObj->getRightHandSide(), _dstSem);
    messageAssert(
        sourceType != nullptr, "Cannot type the destination right hand side of assign.", dstObj->getRightHandSide(),
        _dstSem);

    ILanguageSemantics::ExpressionTypeInfo res = _dstSem->getExprType(targetType, sourceType, op_assign, dstObj);

    if (res.returnedType != nullptr)
        return;

    if (targetType->getTypeVariant() != Type::NATIVE_TYPE) {
        Type *suggT = _dstSem->getSuggestedTypeForOp(targetType, op_assign, sourceType, dstObj, false);
        _dstReplaceWithCast(o->getRightHandSide(), dstObj->getRightHandSide(), suggT);
    } else {
        // need a cast to allow assignment
        _dstReplaceWithCast(o->getRightHandSide(), dstObj->getRightHandSide(), hif::copy(targetType, opt));
    }
}
void HifStdVisitor::_mapCondition(Value *o)
{
    // Some conditions can be ranges over which iterate.
    // E.g.: ForGenerate.
    if (dynamic_cast<Range *>(o) != nullptr)
        return;

    Value *dstObj = _mapTypedGet(o);
    messageAssert(dstObj != nullptr, "Object not found in map", o, _dstSem);

    Type *dstType = getSemanticType(dstObj, _dstSem);
    messageAssert(dstType != nullptr, "Cannot type destination object", dstObj, _dstSem);

    if (_dstSem->checkCondition(dstType, dstObj->getParent()))
        return;

    // need cast to mapped bool
    Bool bb;
    typeSetConstexpr(&bb, typeIsConstexpr(dstType, _dstSem));

    Type *dummyBool = _dstSem->getMapForType(&bb);

    // need a cast to mapped type
    _dstReplaceWithCast(o, dstObj, dummyBool);
}
void HifStdVisitor::_mapConstValue(ConstValue *o)
{
    ConstValue *dstObj = _mapTypedGet(o);
    messageAssert(dstObj != nullptr, "Object not found in map", o, _dstSem);
    messageAssert(
        (o->getType() != nullptr && dstObj->getType() != nullptr) ||
            (o->getType() == nullptr && dstObj->getType() == nullptr),
        "Unexpected mismatch between source and destination syntactic type", o, nullptr);

    Type *dstType = _dstSem->getTypeForConstant(dstObj);
    messageAssert(dstType != nullptr, "Cannot type destination object", dstObj, _dstSem);

    if (o->getType() == nullptr) {
        Type *srcType = _srcSem->getTypeForConstant(o);
        messageAssert(srcType != nullptr, "Cannot find source object type", o, _srcSem);
        Type *mappedType = _dstSem->getMapForType(srcType);
        messageAssert(mappedType != nullptr, "Cannot find mapped type", srcType, _dstSem);
        delete srcType;

        if (hif::equals(mappedType, dstType)) {
            delete mappedType;
            delete dstType;
            return;
        }

        // Transform destination constant to mapped type
        ConstValue *cv = hif::manipulation::transformConstant(dstObj, mappedType, _dstSem);
        if (cv == nullptr) {
            dstObj->setType(dstType);
            _dstReplaceWithCast(o, dstObj, mappedType);
        } else {
            delete dstType;
            Type *dstSemType = _dstSem->getTypeForConstant(cv);
            messageAssert(dstSemType != nullptr, "Cannot map bound type into semantics.", cv, _dstSem);

            if (hif::equals(mappedType, dstSemType)) {
                // Removing syntactic type.
                delete cv->setType(nullptr);
                delete dstSemType;
                dstObj->replace(cv);
                _mapDeleteTree(o, cv);
                _mapSet(o, cv);
                delete mappedType;
                return;
            } else {
                // Setting cv syntactic type to dstSemType
                delete cv->setType(dstSemType);
                // Adding a cast of cv to mappedType.
                dstObj->replace(cv);
                _mapDeleteTree(o, cv);
                _mapSet(o, cv);
                _dstReplaceWithCast(o, cv, mappedType);
            }
        }
    } else {
        Type *mappedType = dstObj->getType();
        ConstValue *cv   = hif::manipulation::transformConstant(dstObj, mappedType, _dstSem);
        if (cv == nullptr) {
            dstObj->setType(dstType);
            _dstReplaceWithCast(o, dstObj, mappedType);
        } else {
            delete dstType;
            Type *dstSemType = _dstSem->getTypeForConstant(cv);
            messageAssert(dstSemType != nullptr, "Cannot map bound type into semantics.", cv, _dstSem);

            if (hif::equals(mappedType, dstSemType)) {
                // ok
                delete dstSemType;
                dstObj->replace(cv);
                _mapDeleteTree(o, cv);
                _mapSet(o, cv);
            } else {
                // Setting cv syntactic type to dstSemType
                delete cv->setType(dstSemType);
                // Adding a cast of cv to mappedType.
                Type *castType = hif::copy(mappedType);
                dstObj->replace(cv);
                _mapDeleteTree(o, cv);
                _mapSet(o, cv);
                _dstReplaceWithCast(o, cv, castType);
            }
        }
    }
}
void HifStdVisitor::_mapExpression(Expression *o)
{
    Expression *dstObj = _mapTypedGet(o);
    messageAssert(dstObj != nullptr, "Object not found in map", o, _dstSem);

    Type *srcType = getSemanticType(o, _srcSem);
    messageAssert(srcType != nullptr, "Cannot type source expression.", o, _srcSem);

    Type *dstObjOp1Type = getSemanticType(dstObj->getValue1(), _dstSem);
    messageAssert(dstObjOp1Type != nullptr, "Cannot type destination op1.", dstObj->getValue1(), _dstSem);

    Type *dstObjOp2Type = nullptr;
    if (dstObj->getValue2() != nullptr) {
        dstObjOp2Type = getSemanticType(dstObj->getValue2(), _dstSem);
        messageAssert(dstObjOp2Type != nullptr, "Cannot type destination op2.", dstObj->getValue2(), _dstSem);
    }

    std::list<Type *> exprTypes;
    exprTypes.push_back(getSemanticType(o->getValue1(), _srcSem));
    if (o->getValue2() != nullptr) {
        exprTypes.push_back(getSemanticType(o->getValue2(), _srcSem));
    }

    AnalyzeParams params;
    params.operation    = o->getOperator();
    params.operandsType = exprTypes;
    params.startingObj  = o;

    AnalyzeInfo exprInfo = _analyzeExprType(params);

    if (exprInfo.error)
        messageError("Error during analysis of expression.", o, _srcSem);

    // Updating operator according with new types mapped by dst semantics.
    dstObj->setOperator(exprInfo.resultOperator);

    Type *castOp1 = nullptr;
    Type *castOp2 = nullptr;
    CopyOptions opt;
    //opt.copySemanticsTypes = true;
    if (exprInfo.operandType1 != nullptr) {
        castOp1 = hif::copy(exprInfo.operandType1, opt);
    }

    if (dstObj->getValue2() != nullptr && exprInfo.operandType2 != nullptr) {
        castOp2 = hif::copy(exprInfo.operandType2, opt);
    }

    // Special case: we must preserve spans with op concat!!
    if (dstObj->getOperator() == op_concat) {
        if (castOp1 != nullptr) {
            _maintainSpan(dstObjOp1Type, castOp1);
        }

        if (dstObj->getValue2() != nullptr && castOp2 != nullptr) {
            _maintainSpan(dstObjOp2Type, castOp2);
        }
    }

    // Fixing casts on operand 1:
    if (exprInfo.operandType1 != nullptr) {
        if (!hif::equals(dstObjOp1Type, castOp1)) {
            // need a cast to mapped type
            _dstReplaceWithCast(o->getValue1(), dstObj->getValue1(), castOp1);
        } else {
            delete castOp1;
        }
    }

    // Fixing casts on operand 2:
    if (dstObj->getValue2() != nullptr && exprInfo.operandType2 != nullptr) {
        if (!hif::equals(dstObjOp2Type, castOp2)) {
            // need a cast to mapped type
            _dstReplaceWithCast(o->getValue2(), dstObj->getValue2(), castOp2);
        } else {
            delete castOp2;
        }
    }

    // Fixing cast on result:
    if (exprInfo.resultType != nullptr) {
        Type *dstType = getSemanticType(dstObj, _dstSem);
        messageAssert(dstType != nullptr, "Cannot type destination expression (1).", dstObj, _dstSem);
        if (!hif::equals(dstType, exprInfo.resultType)) {
            // need a cast to mapped type
            _dstReplaceWithCast(o, dstObj, hif::copy(exprInfo.resultType));
        }
    }

    Type *dstType = getSemanticType(dstObj, _dstSem);
    messageAssert(dstType != nullptr, "Cannot type destination expression (2).", dstObj, _dstSem);
}
void HifStdVisitor::_mapIndex(Value *o)
{
    if (dynamic_cast<Range *>(o) != nullptr) {
        // already standardized (for_generate loops?)
        return;
    }

    // assure that mapped index type is integer
    Value *dstObj = _mapTypedGet(o);
    messageAssert(dstObj != nullptr, "Object not found in map", o, _dstSem);

    Type *dstType = getSemanticType(dstObj, _dstSem);
    messageAssert(dstType != nullptr, "Cannot type the mapped value in destination semantics (1).", dstObj, _dstSem);

    CopyOptions opt;
    //opt.copySemanticsTypes = true;

    // get the mapped type for a fake integer according to dest semantics.
    // and assure that in destination semantics, the mapped type is a
    // int object.
    Int ii;
    Range *r = hif::copy(typeGetSpan(dstType, _dstSem), opt);
    messageAssert(r != nullptr, "Type span not found", dstType, _dstSem);
    typeSetSpan(&ii, r, _dstSem);
    typeSetSigned(&ii, typeIsSigned(dstType, _dstSem), _dstSem);
    typeSetConstexpr(&ii, typeIsConstexpr(dstType, _dstSem));

    Type *dummy_int   = _dstSem->getMapForType(&ii);
    Type *allowedType = _dstSem->isTypeAllowedAsBound(dummy_int);
    if (allowedType != nullptr) {
        delete dummy_int;
        dummy_int = allowedType;
    }

    if (hif::equals(dstType, dummy_int)) {
        delete dummy_int;

        ConstValue *c = dynamic_cast<ConstValue *>(dstObj);
        // Constant indices must not have syntactic type.
        if (c != nullptr && !hif::manipulation::needSyntacticType(c)) {
            Value *ret = _dstHandleBound(c);
            if (ret != c) {
                dstObj->replace(ret);
                _mapSet(o, ret);
            }
        }

        return;
    }

    // need a cast to mapped type
    _dstReplaceWithCast(o, dstObj, dummy_int);
    hif::manipulation::assureSyntacticType(dstObj, _dstSem);
}
void HifStdVisitor::_mapInitialValue(DataDeclaration *o)
{
    if (o->getValue() == nullptr)
        return;

    DataDeclaration *dstObj = _mapTypedGet(o);
    messageAssert(dstObj != nullptr, "Object not found in map", o, _dstSem);

    Type *declType = dstObj->getType();
    messageAssert(declType != nullptr, "Unexpected declaration without type", dstObj, _dstSem);

    Type *initValType = getSemanticType(dstObj->getValue(), _dstSem);
    messageAssert(initValType != nullptr, "Cannot type destination initial value", dstObj->getValue(), _dstSem);

    ILanguageSemantics::ExpressionTypeInfo res = _dstSem->getExprType(declType, initValType, op_assign, dstObj);

    if (res.returnedType != nullptr)
        return;

    // need a cast to allow initialization
    CopyOptions opt;
    //opt.copySemanticsTypes = true;
    _dstReplaceWithCast(o->getValue(), dstObj->getValue(), hif::copy(declType, opt));
}
template <typename T> void HifStdVisitor::_mapReferencedAssign(T *o, const Operator op)
{
    T *dstObj = _mapTypedGet(o);
    messageAssert(dstObj != nullptr, "Object not found in map", o, _dstSem);

#if 1
    Type *srcFormalType = hif::semantics::getSemanticType(o, _srcSem);
    messageAssert(srcFormalType != nullptr, "Cannot type source referenced assign.", o, _srcSem);

    Type *dstFormalType = _dstGetType(srcFormalType, true);
    messageAssert(dstFormalType != nullptr, "Cannot type destination referenced assign.", dstObj, _dstSem);
    if (dstFormalType == nullptr)
        return;

    Type *dstActualType = getSemanticType(dstObj->getValue(), _dstSem);
    messageAssert(dstActualType != nullptr, "Cannot type referenced assign value.", dstObj->getValue(), _dstSem);

    ILanguageSemantics::ExpressionTypeInfo res = _dstSem->getExprType(dstFormalType, dstActualType, op, dstObj);

    if (res.returnedType != nullptr)
        return;

    // need a cast to allow assignment
    _dstReplaceWithCast(o->getValue(), dstObj->getValue(), hif::copy(dstFormalType));

#else
    Type *dstFormalType = hif::semantics::getSemanticType(dstObj, _dstSem);
    messageAssert(dstFormalType != nullptr, "Cannot type destination referenced assign.", dstObj, _dstSem);
    if (dstFormalType == nullptr)
        return;

    Type *dstActualType = getSemanticType(dstObj->getValue(), _dstSem);
    messageAssert(dstActualType != nullptr, "Cannot type referenced assign value.", dstObj->getValue(), _dstSem);

    ILanguageSemantics::ExpressionTypeInfo res = _dstSem->getExprType(dstFormalType, dstActualType, op, dstObj);

    if (res.returnedType != nullptr)
        return;

    // need a cast to allow assignment
    _dstReplaceWithCast(o->getValue(), dstObj->getValue(), hif::copy(dstFormalType));
#endif
}
void HifStdVisitor::_mapReferencedAssignList(BList<TPAssign> &list)
{
    for (BList<TPAssign>::iterator i = list.begin(); i != list.end(); ++i) {
        if (dynamic_cast<TypeTPAssign *>(*i) != nullptr) {
            continue;
        } else if (dynamic_cast<ValueTPAssign *>(*i) != nullptr) {
            ValueTPAssign *t = static_cast<ValueTPAssign *>(*i);
            _mapReferencedAssign(t, op_conv);
        } else
            messageError("Unexpected object.", *i, _srcSem);
    }
}
void HifStdVisitor::_mapReferencedAssignList(BList<ParameterAssign> &list)
{
    for (BList<ParameterAssign>::iterator i = list.begin(); i != list.end(); ++i) {
        _mapReferencedAssign(*i, op_conv);
    }
}
void HifStdVisitor::_mapReferencedAssignList(BList<PortAssign> &list)
{
    for (BList<PortAssign>::iterator i = list.begin(); i != list.end(); ++i) {
        _mapReferencedAssign(*i, op_bind);
    }
}
void HifStdVisitor::_mapReturn(Return *o)
{
    Return *dstObj = _mapTypedGet(o);
    messageAssert(dstObj != nullptr, "Object not found in map", o, _dstSem);

    CopyOptions opt;
    //opt.copySemanticsTypes = true;

    Procedure *proc = hif::getNearestParent<Procedure>(o);
    Function *func  = hif::getNearestParent<Function>(o);
    StateTable *st  = hif::getNearestParent<StateTable>(o);

    if (proc != nullptr || (st != nullptr && st->getFlavour() == hif::pf_thread)) {
        messageAssert(o->getValue() == nullptr, "Return of procedure cannot have a value.", o, _srcSem);
    } else if (func != nullptr) {
        messageAssert(o->getValue() != nullptr, "Return of function must have a value.", o, _srcSem);

        Function *dstFunc = _mapTypedGet(func);
        messageAssert(dstFunc != nullptr, "Function not found in destination tree", func, _dstSem);

        Type *dstFuncType = dstFunc->getType();

        Type *dstReturnType = getSemanticType(dstObj->getValue(), _dstSem);
        messageAssert(dstReturnType != nullptr, "Cannot type value of return statement.", dstObj->getValue(), _dstSem);

        ILanguageSemantics::ExpressionTypeInfo res = _dstSem->getExprType(dstFuncType, dstReturnType, op_conv, dstObj);

        if (res.returnedType != nullptr)
            return;

        // need a cast to allow assignment
        _dstReplaceWithCast(o->getValue(), dstObj->getValue(), hif::copy(dstFuncType, opt));
    } else {
        messageError("Parent method not found.", o, _srcSem);
    }
}
void HifStdVisitor::_mapType(Type *o)
{
    Type *dstObj = _mapTypedGet(o);
    messageAssert(dstObj != nullptr, "Object not found in map", o, _dstSem);

    Type *dstType = _dstSem->getMapForType(dstObj);
    messageAssert(dstType != nullptr, "Cannot map type in destination semantics.", dstObj, _dstSem);

    if (dstObj->getParent() != nullptr) {
        dstObj->replace(dstType);
        _mapDeleteTree(o, dstType);
    } else {
        messageAssert(
            o->getParent() == nullptr || isSemanticsType(o) || hif::manipulation::isInCache(o), // TODO check!
            "Unexpected related object or not semantics type", o, _srcSem);
        _mapDeleteTree(o);
    }
    _mapSet(o, dstType);

    if (_canRebaseTypes && !_srcSem->isSyntacticTypeRebased() && _dstSem->isSyntacticTypeRebased()) {
        // Rebasing to zero.
        Range *r = hif::typeGetSpan(dstType, _dstSem);
        if (r == nullptr || _isTypedRange(r, _dstSem))
            return;
        Value *min = hif::rangeGetMinBound(r);

        IntValue *i = dynamic_cast<IntValue *>(min);
        if (i != nullptr && i->getValue() == 0LL)
            return;

        min          = hif::manipulation::assureSyntacticType(hif::copy(min), _dstSem);
        Value *left  = hif::manipulation::assureSyntacticType(r->setLeftBound(nullptr), _dstSem);
        Value *right = hif::manipulation::assureSyntacticType(r->setRightBound(nullptr), _dstSem);

        r->setLeftBound(_dstFactory.expression(left, op_minus, min));

        r->setRightBound(_dstFactory.expression(right, op_minus, hif::copy(min)));
    }
}
void HifStdVisitor::_mapValue(Value *o)
{
    Type *srcType = getSemanticType(o, _srcSem);
    messageAssert(srcType != nullptr, "Cannot type the value in source semantics.", o, _srcSem);

    Value *dstObj = _mapTypedGet(o);
    messageAssert(dstObj != nullptr, "Object not found in map", o, _dstSem);

    Type *dstType = getSemanticType(dstObj, _dstSem);
    messageAssert(dstType != nullptr, "Cannot type the mapped value in destination semantics (2).", dstObj, _dstSem);

    Type *mappedType = _dstGetType(srcType, true);
    messageAssert(mappedType != nullptr, "Cannot map the source type.", srcType, _srcSem);

    if (hif::equals(dstType, mappedType)) {
        delete mappedType;
        return;
    }

    // need a cast to mapped type
    _dstReplaceWithCast(o, dstObj, mappedType);
}
void HifStdVisitor::_mapSlice(Slice *o) { _mapSliceSpan(o); }

void HifStdVisitor::_mapSliceSpan(Slice *o)
{
    bool srcTr = _srcSem->isSyntacticTypeRebased();
    bool srcSr = _srcSem->isSliceTypeRebased();

    bool dstTr = _dstSem->isSyntacticTypeRebased();
    bool dstSr = _dstSem->isSliceTypeRebased();

    // skip cases where fix is not necessary
    if (srcTr == dstTr && srcSr == dstSr)
        return;
    if (srcTr && srcSr && !dstTr && dstSr)
        return;

    Type *srcType = hif::semantics::getSemanticType(o, _srcSem);
    messageAssert(srcType != nullptr, "Cannot type slice in source semantics", o, _srcSem);

    Type *srcPrefixType = hif::semantics::getSemanticType(o->getPrefix(), _srcSem);
    messageAssert(srcPrefixType != nullptr, "Cannot type slice prefix in source semantics", o->getPrefix(), _srcSem);

    Slice *dstObj = _mapTypedGet(o);
    messageAssert(dstObj != nullptr, "Object not found in map", o, _dstSem);

    Type *dstType = getSemanticType(dstObj, _dstSem);
    messageAssert(dstType != nullptr, "Cannot type the mapped value in destination semantics (2).", dstObj, _dstSem);

    Type *dstPrefixType = getSemanticType(dstObj->getPrefix(), _dstSem);
    messageAssert(
        dstPrefixType != nullptr, "Cannot type the mapped slice prefix in destination semantics.", dstObj->getPrefix(),
        _dstSem);

    Type *mappedType = _dstGetType(srcType, true);
    messageAssert(mappedType != nullptr, "Cannot map the source slice type.", srcType, _srcSem);

    Type *mappedPrefixType = _dstGetType(srcPrefixType, true);
    messageAssert(mappedPrefixType != nullptr, "Cannot map the source prefix type.", srcPrefixType, _srcSem);

    if (!srcTr && dstTr) {
        Range *mappedPrefixRange = hif::typeGetSpan(mappedPrefixType, _dstSem);
        if (mappedPrefixRange == nullptr) {
            return;
        }
        Value *mappedPrefixMinBound = hif::copy(hif::rangeGetMinBound(mappedPrefixRange));
        mappedPrefixMinBound        = hif::manipulation::assureSyntacticType(mappedPrefixMinBound, _dstSem);

        Range *dstPrefixRange    = hif::typeGetSpan(dstPrefixType, _dstSem);
        Value *dstPrefixMinBound = hif::copy(hif::rangeGetMinBound(dstPrefixRange));
        if (dstPrefixMinBound == nullptr) {
            return;
        }
        dstPrefixMinBound = hif::manipulation::assureSyntacticType(dstPrefixMinBound, _dstSem);

        Range *dstSliceSpan   = dstObj->getSpan();
        Value *dstSliceLBound = hif::manipulation::assureSyntacticType(dstSliceSpan->setLeftBound(nullptr), _dstSem);
        Value *dstSliceRBound = hif::manipulation::assureSyntacticType(dstSliceSpan->setRightBound(nullptr), _dstSem);

        dstSliceSpan->setLeftBound(_dstFactory.expression(
            dstSliceLBound, op_minus, _dstFactory.expression(mappedPrefixMinBound, op_minus, dstPrefixMinBound)));

        dstSliceSpan->setRightBound(_dstFactory.expression(
            dstSliceRBound, op_minus,
            _dstFactory.expression(hif::copy(mappedPrefixMinBound), op_minus, hif::copy(dstPrefixMinBound))));
    }

    delete mappedPrefixType;

    Slice *pSlice   = dynamic_cast<Slice *>(dstType->getParent());
    Member *pMember = dynamic_cast<Member *>(dstType->getParent());

    // Cast is needed only if destination semantics
    // re-base slices but not re-base types.
    if ((pSlice != nullptr || pMember != nullptr) || (dstTr || !dstSr) || hif::equals(dstType, mappedType)) {
        delete mappedType;
        return;
    }

    // need a cast to mapped type
    _dstReplaceWithCast(o, dstObj, mappedType);
}
void HifStdVisitor::_mapMember(Member *o) { _mapMemberIndex(o); }

void HifStdVisitor::_mapMemberIndex(Member *o)
{
    bool srcTr = _srcSem->isSyntacticTypeRebased();
    bool srcSr = _srcSem->isSliceTypeRebased();

    bool dstTr = _dstSem->isSyntacticTypeRebased();
    bool dstSr = _dstSem->isSliceTypeRebased();

    // skip cases where fix is not necessary
    if (srcTr == dstTr && srcSr == dstSr)
        return;
    if (srcTr && srcSr && !dstTr && dstSr)
        return;

    Type *srcPrefixType = hif::semantics::getSemanticType(o->getPrefix(), _srcSem);
    messageAssert(srcPrefixType != nullptr, "Cannot type member prefix in source semantics", o->getPrefix(), _srcSem);

    Member *dstObj = _mapTypedGet(o);
    messageAssert(dstObj != nullptr, "Object not found in map", o, _dstSem);

    Type *dstPrefixType = getSemanticType(dstObj->getPrefix(), _dstSem);
    messageAssert(
        dstPrefixType != nullptr, "Cannot type the mapped member prefix in destination semantics.", dstObj->getPrefix(),
        _dstSem);

    Type *mappedPrefixType = _dstGetType(srcPrefixType, true);
    messageAssert(mappedPrefixType != nullptr, "Cannot map the source prefix type.", srcPrefixType, _srcSem);

    if (!srcTr && dstTr) {
        Range *mappedPrefixRange = hif::typeGetSpan(mappedPrefixType, _dstSem);
        if (mappedPrefixRange == nullptr) {
            return;
        }
        Value *mappedPrefixMinBound = hif::copy(hif::rangeGetMinBound(mappedPrefixRange));
        mappedPrefixMinBound        = hif::manipulation::assureSyntacticType(mappedPrefixMinBound, _dstSem);

        Range *dstPrefixRange = hif::typeGetSpan(dstPrefixType, _dstSem);
        if (dstPrefixRange == nullptr) {
            return;
        }
        Value *dstPrefixMinBound = hif::copy(hif::rangeGetMinBound(dstPrefixRange));
        dstPrefixMinBound        = hif::manipulation::assureSyntacticType(dstPrefixMinBound, _dstSem);

        Value *index  = dstObj->getIndex();
        Expression *e = _dstFactory.expression(hif::copy(mappedPrefixMinBound), op_minus, hif::copy(dstPrefixMinBound));
        Expression *e2 = new Expression();
        e2->setOperator(op_minus);
        e2->setValue2(e);
        index->replace(e2);
        _mapSet(o->getIndex(), e2);
        index = hif::manipulation::assureSyntacticType(index, _dstSem);
        e2->setValue1(index);

        delete mappedPrefixMinBound;
        delete dstPrefixMinBound;
    }

    delete mappedPrefixType;
}
// ///////////////////////////////////////////////////////////////////
// AnalizeInfo
// ///////////////////////////////////////////////////////////////////
HifStdVisitor::AnalyzeInfo::AnalyzeInfo()
    : operandType1(nullptr)
    , operandType2(nullptr)
    , resultType(nullptr)
    , resultOperator(op_none)
    , error(false)
{
    // ntd
}

HifStdVisitor::AnalyzeInfo::AnalyzeInfo(const AnalyzeInfo &t)
    : operandType1(hif::copy(t.operandType1))
    , operandType2(hif::copy(t.operandType2))
    , resultType(hif::copy(t.resultType))
    , resultOperator(t.resultOperator)
    , error(t.error)
{
    // ntd
}

HifStdVisitor::AnalyzeInfo::~AnalyzeInfo()
{
    delete operandType1;
    delete operandType2;
    delete resultType;
}
HifStdVisitor::AnalyzeInfo &HifStdVisitor::AnalyzeInfo::operator=(const AnalyzeInfo &t)
{
    if (&t == this)
        return *this;

    delete this->operandType1;
    delete this->operandType2;
    delete this->resultType;

    this->operandType1   = hif::copy(t.operandType1);
    this->operandType2   = hif::copy(t.operandType2);
    this->resultType     = hif::copy(t.resultType);
    this->resultOperator = t.resultOperator;
    this->error          = t.error;
    return *this;
}

// //////////////////////////////////////////////////
// Mapping Utilities.
// //////////////////////////////////////////////////

HifStdVisitor::AnalyzeInfo HifStdVisitor::_analyzeExprType(const AnalyzeParams &params)
{
    AnalyzeInfo result;

    if (params.operandsType.empty())
        return result;
    messageAssert(
        params.operandsType.size() <= 2, "Unexpected expression operands number", params.startingObj, _srcSem);

    Type *op1 = params.operandsType.front();
    Type *op2 = params.operandsType.back();
    if (op1 == op2)
        op2 = nullptr;

    // perform semantics algorithm.
    _performSemanticsAlgorithm(op1, op2, params.operation, result, params.startingObj);

    // If error cleanup memory and return.
    if (result.error) {
        delete result.resultType;
        delete result.operandType1;
        delete result.operandType2;
        result.resultType   = nullptr;
        result.operandType1 = nullptr;
        result.operandType2 = nullptr;
        return result;
    }

    return result;
}

HifStdVisitor::AnalyzeInfo HifStdVisitor::_analyzePrecisionType(const AnalyzeParams &params)
{
    AnalyzeInfo result;
    Type *t = analyzePrecisionType(params, _srcSem);
    messageAssert(t != nullptr, "Cannot type alts in source semantics", params.startingObj, _srcSem);
    result.operandType1 = _dstGetType(t, true);
    delete t;
    return result;
}

void HifStdVisitor::_performSemanticsAlgorithm(
    Type *t1,
    Type *t2,
    Operator operation,
    AnalyzeInfo &result,
    Object *startingObj)
{
    Object *dstStarting = _mapGet(startingObj);

    // ///////////////////////////////////////////////////////////////////
    // STEP 1: Analyze operation according with source language semantics.
    // ///////////////////////////////////////////////////////////////////
    ILanguageSemantics::ExpressionTypeInfo srcAnalysis = _srcSem->getExprType(t1, t2, operation, startingObj);

    messageAssert(
        srcAnalysis.returnedType != nullptr, "[SemanticsAlgorithm] Operation not allowed in source language.",
        startingObj, _srcSem);
#ifndef NDEBUG
    if (srcAnalysis.operationPrecision == nullptr) {
        std::clog << *_srcSem << " Operation: " << std::endl;
        hif::printOperator(operation, std::clog);
        std::clog << "\nOp1 type: " << std::endl;
        hif::writeFile(std::clog, t1, false);
        std::clog << "\nOp2 type: " << std::endl;
        hif::writeFile(std::clog, t2, false);
        std::clog << "\nstarting object: " << std::endl;
        hif::writeFile(std::clog, startingObj, false);
        std::clog << "\nreturned type: " << std::endl;
        hif::writeFile(std::clog, srcAnalysis.returnedType, false);
        std::clog << std::endl;
        assert(false);
    }
#endif
    messageDebugAssert(
        srcAnalysis.returnedType != srcAnalysis.operationPrecision,
        "Unexpected alias returned type - operation precision", startingObj, _srcSem);

    // ///////////////////////////////////////////////////////////////////
    // STEP 2: Retrieve type mapping for destination semantics.
    // ///////////////////////////////////////////////////////////////////
    Type *dstT1 = _dstGetType(t1, true);
    Type *dstT2 = nullptr;
    if (t2 != nullptr)
        dstT2 = _dstGetType(t2, true);
    if (dstT2 == dstT1)
        dstT2 = hif::copy(dstT1);

    // sanity checks
    messageAssert(dstT1 != nullptr, "[SemanticsAlgorithm] Cannot map operand 1 type.", t1, _dstSem);
    messageAssert(t2 == nullptr || dstT2 != nullptr, "[SemanticsAlgorithm] Cannot map operand 2 type.", t2, _dstSem);

    Type *dstResult    = _dstGetType(srcAnalysis.returnedType, true);
    Type *dstPrecision = _dstGetType(srcAnalysis.operationPrecision, true);

    messageAssert(
        dstResult != nullptr, "[SemanticsAlgorithm] Cannot map result type.", srcAnalysis.returnedType, _dstSem);
    messageAssert(
        dstPrecision != nullptr, "[SemanticsAlgorithm] Cannot map precision type.", srcAnalysis.operationPrecision,
        _dstSem);

    // ///////////////////////////////////////////////////////////////////
    // STEP 2.1: Retrieve operator mapping for destination semantics.
    // ///////////////////////////////////////////////////////////////////

    result.resultOperator = _dstSem->getMapForOperator(operation, t1, t2, dstT1, dstT2);
    operation             = result.resultOperator;

    // ///////////////////////////////////////////////////////////////////
    // STEP 3: Analyze operation according with destination language.
    // ///////////////////////////////////////////////////////////////////
    ILanguageSemantics::ExpressionTypeInfo dstAnalysis = _dstSem->getExprType(dstT1, dstT2, operation, dstStarting);

    // ///////////////////////////////////////////////////////////////////
    // STEP 4: Check if operations is now allowed in destination
    // semantics. If not try to introduce a cast on operands basing
    // on operators.
    // ///////////////////////////////////////////////////////////////////
    if (dstAnalysis.returnedType != nullptr) {
        messageAssert(
            dstAnalysis.operationPrecision != nullptr, "Unexpected precision not set in destination semantics",
            startingObj, _dstSem);

        // Operation allowed without any cast.
        // Check whether opetion precision differs.
        hif::EqualsOptions opt;
        opt.checkConstexprFlag = false;
        if (!hif::equals(dstPrecision, dstAnalysis.operationPrecision, opt)) {
            if (!hif::equals(dstT1, dstPrecision, opt)) {
                result.operandType1 = hif::copy(dstPrecision);
            }

            if (!hif::equals(dstT2, dstPrecision, opt)) {
                result.operandType2 = hif::copy(dstPrecision);
            }
        }
    } else // if( dstAnalysis.returnedType == nullptr )
    {
        messageAssert(
            dstAnalysis.operationPrecision == nullptr, "Unexpected precision set in destination semantics", startingObj,
            _dstSem);

        Type *dstOperandCast = nullptr;
        Type *dstT1Final     = nullptr;
        Type *dstT2Final     = nullptr;
        if (hif::operatorIsRelational(operation) || hif::operatorIsArithmetic(operation) ||
            hif::operatorIsBitwise(operation) || operation == op_concat) {
            // With arithmetic, relational and bitwise operators
            // cast operands to mapped type precision.
            dstOperandCast = hif::copy(dstPrecision);
        } else if (hif::operatorIsLogical(operation)) {
            // With logical operators
            // cast operands to mapped boolean type.
            Bool b;
            dstOperandCast = _dstGetType(&b, true);
        } else if (hif::operatorIsShift(operation)) {
            // With shift or rotate operators
            // probably the second operand needs a cast to int.
            Int ii;
            Range *ds = hif::copy(typeGetSpan(dstT2, _dstSem));
            if (ds == nullptr) {
                return;
            }
            typeSetSpan(&ii, ds, _dstSem);
            typeSetSigned(&ii, typeIsSigned(dstT2, _dstSem), _dstSem);
            typeSetConstexpr(&ii, typeIsConstexpr(dstT2, _dstSem));
            hif::manipulation::transformSpanToRange(ii.getSpan(), _dstSem, ii.isSigned());

            Type *dummyInt = _dstSem->getMapForType(&ii);
            dstOperandCast = dummyInt;
        } else if (hif::operatorIsReduce(operation)) {
            // TODO: this should never happen
            messageError("[SemanticsAlgorithm] STEP 4: Unexpected operator.", startingObj, _dstSem);
        } else {
            // Unhandled operator
            messageError("[SemanticsAlgorithm] STEP 4: Unhandled operator.", startingObj, _dstSem);
        }

        // Check if destination semantics allows the operations
        // casting operands to the established type.
        if (hif::operatorIsShift(operation)) {
            // cast only the second operand
            dstT1Final = hif::copy(dstT1);
            dstT2Final = dstOperandCast;
        } else // with all others operands
        {
            // cast both operands
            dstT1Final = dstOperandCast;
            dstT2Final = hif::copy(dstOperandCast);
        }

        dstAnalysis = _dstSem->getExprType(dstT1Final, dstT2Final, operation, dstStarting);

        // Check if operations is allowed with new types.
        if (dstAnalysis.returnedType == nullptr) {
            // If operation is still not allowed:
            // try to cast operands to the suggested type according with
            // destination semantics starting from type precision.
            Type *dstSuggestedType1 =
                _dstSem->getSuggestedTypeForOp(dstOperandCast, operation, dstT1, dstStarting, true);
            messageAssert(
                dstSuggestedType1 != nullptr, "[SemanticsAlgorithm] Suggested type 1 not found.", startingObj, _dstSem);
            Type *dstSuggestedType2 =
                _dstSem->getSuggestedTypeForOp(dstOperandCast, operation, dstT2, dstStarting, false);
            messageAssert(
                dstSuggestedType2 != nullptr, "[SemanticsAlgorithm] Suggested type 2 not found.", startingObj, _dstSem);

            dstAnalysis = _dstSem->getExprType(dstSuggestedType1, dstSuggestedType2, operation, dstStarting);

#ifndef NDEBUG
            if (dstAnalysis.returnedType == nullptr) {
                messageDebug("Suggested type 1", dstSuggestedType1, _dstSem);
                messageDebug("Suggested type 2", dstSuggestedType2, _dstSem);

#    if 0
                _dstSem->getSuggestedTypeForOp(
                            dstOperandCast, operation, dstT1, dstStarting, true );
                _dstSem->getSuggestedTypeForOp(
                            dstOperandCast, operation, dstT2, dstStarting, true );
#    endif
            }
#endif
            messageAssert(
                dstAnalysis.returnedType != nullptr, "[SemanticsAlgorithm] Suggested type is not valid for expression.",
                startingObj, _dstSem);

            delete dstT1Final;
            delete dstT2Final;

            // With suggested type the operation is allowed in
            // destination language. Set operands type to suggested type.
            dstT1Final = dstSuggestedType1;
            dstT2Final = dstSuggestedType2;
        }

        // checking required casts on operands
        if (!hif::equals(dstT1, dstT1Final)) {
            result.operandType1 = hif::copy(dstT1Final);
        }

        if (!hif::equals(dstT2, dstT2Final)) {
            result.operandType2 = hif::copy(dstT2Final);
        }

        delete dstT1Final;
        delete dstT2Final;
    }

    delete dstT1;
    delete dstT2;

    // ///////////////////////////////////////////////////////////////////
    // STEP 5: Control if result needs a cast
    // (when types are of different classes)
    // ///////////////////////////////////////////////////////////////////

    // Check if they are different types (but don't check the range)
    hif::EqualsOptions opt;
    opt.checkSpans = false;
    if (!hif::equals(dstResult, dstAnalysis.returnedType, opt)) {
        result.resultType = hif::copy(dstResult);
    }

    // ///////////////////////////////////////////////////////////////////
    // STEP 6: Control if a cast is needed basing on result length
    // This is performed only if the type has a span!
    // ///////////////////////////////////////////////////////////////////
    if (hif::typeGetSpan(dstResult, _dstSem) != nullptr) {
        _handleLength(dstPrecision, dstAnalysis.operationPrecision, dstResult, result, operation);
    }

    // ///////////////////////////////////////////////////////////////////
    // STEP 7: Control that expression result will have the same precision.
    // ///////////////////////////////////////////////////////////////////
    if (result.resultType == nullptr && !hif::equals(dstResult, dstAnalysis.returnedType)) {
        result.resultType = hif::copy(dstResult);
    }
    // Types must be equal at the end of the algorithm
    delete dstResult;
    delete dstPrecision;
}
void HifStdVisitor::_handleLength(Type *source, Type *dest, Type *resultType, AnalyzeInfo &result, Operator operation)
{
    // Skipping cases with strings.
    String *sstring = dynamic_cast<String *>(source);
    String *dstring = dynamic_cast<String *>(dest);
    if (sstring != nullptr || dstring != nullptr) {
        messageAssert(
            sstring != nullptr && sstring != nullptr, "Unexpected operation between string and other different type",
            source, _dstSem);
        return;
    }

    // When we calculate type span we use the same semantics to assure
    // that eventual flags for constant values are set in the same way.
    // Otherwise following compare can fail on range containing constants
    // even if constant value are the same.

    Range *sourceSpan = hif::typeGetSpan(source, _dstSem);
    Range *destSpan   = hif::typeGetSpan(dest, _dstSem);

    if (sourceSpan == nullptr) {
        return;
    }
    if (destSpan == nullptr) {
        return;
    }

    // Some types (e.g. Record) do not have spans.
    // In this case no precision span handling is required.
    if (sourceSpan == nullptr && destSpan == nullptr)
        return;

    messageAssert(sourceSpan != nullptr, "[HandleLength] Source span not found.", source, _dstSem);
    messageAssert(destSpan != nullptr, "[HandleLength] Destination span not found.", dest, _dstSem);

    Value *sourceSpanSize = spanGetSize(sourceSpan, _dstSem);
    Value *destSpanSize   = spanGetSize(destSpan, _dstSem);

    bool equalSpans = hif::equals(sourceSpanSize, destSpanSize);
    delete sourceSpanSize;
    delete destSpanSize;

    if (equalSpans)
        return;

    Range *max = rangeGetMax(sourceSpan, destSpan, _dstSem);

    if (hif::equals(max, sourceSpan)) {
        // Case source > dest
        // Loss of precision => cast operands
        if (hif::operatorIsShift(operation)) {
            // cast only operand 1
            _typeSetPrecision(result.operandType1, source);
        } else {
            // cast both
            _typeSetPrecision(result.operandType1, source);
            _typeSetPrecision(result.operandType2, source);
        }
    } else if (hif::equals(max, destSpan)) {
        // Case source < dest
        _typeSetPrecision(result.resultType, resultType);
    } else {
        // Unknown since spans are complex expressions.
        // Set precision both on operands and result.
        if (hif::operatorIsShift(operation)) {
            // cast only operand 1
            _typeSetPrecision(result.operandType1, source);
        } else {
            // cast both
            _typeSetPrecision(result.operandType1, source);
            _typeSetPrecision(result.operandType2, source);
        }
        _typeSetPrecision(result.resultType, resultType);
    }

    delete max;
}
void HifStdVisitor::_typeSetPrecision(Type *&result, Type *precision)
{
    if (result == nullptr) {
        result = hif::copy(precision);
        return;
    }

    Range *precisionSpan = typeGetSpan(precision, _dstSem);
    messageAssert(precisionSpan != nullptr, "destination precision span not found", precision, _dstSem);
    typeSetSpan(result, hif::copy(precisionSpan), _dstSem, true);
}
bool HifStdVisitor::_isTypedRange(Range *tSpan, hif::semantics::ILanguageSemantics *sem)
{
    // Heuristic to check if a range is typed.
    Identifier *lb = dynamic_cast<Identifier *>(tSpan->getLeftBound());
    Identifier *rb = dynamic_cast<Identifier *>(tSpan->getRightBound());
    ValueTP *lbDec = dynamic_cast<ValueTP *>(hif::semantics::getDeclaration(lb, sem));
    ValueTP *rbDec = dynamic_cast<ValueTP *>(hif::semantics::getDeclaration(rb, sem));
    bool isTyped =
        (lb != nullptr && rb != nullptr && lbDec != nullptr && rbDec != nullptr && lbDec->isInBList() &&
         rbDec->isInBList() && lbDec->getBList() == rbDec->getBList());

    return isTyped;
}
bool HifStdVisitor::_isTop(Value *o)
{
    Slice *s  = dynamic_cast<Slice *>(o->getParent());
    Member *m = dynamic_cast<Member *>(o->getParent());
    Cast *c   = dynamic_cast<Cast *>(o->getParent());

    return s == nullptr && m == nullptr && c == nullptr;
}

void HifStdVisitor::_fillCastMap(Cast *key, Type *t)
{
    if (_castMap.find(key) != _castMap.end()) {
        std::stringstream ss;
        ss << "key already present " << key;
        messageDebugAssert(false, ss.str(), key, nullptr);
    }

    _castMap[key] = t;
}

Value *HifStdVisitor::_dstHandleBound(Value *dstBound)
{
    ConstValue *dstCv = dynamic_cast<ConstValue *>(dstBound);
    if (dstCv == nullptr) {
        // Not a cost value, nothing to do.
        return dstBound;
    }

    Type *boundType = dstCv->getType();
    if (boundType == nullptr) {
        // Assuming it was already good.
        return dstBound;
    }

    Type *semType = _dstSem->getTypeForConstant(dstCv);
    messageAssert(semType != nullptr, "Cannot map bound type into semantics.", dstCv, _dstSem);

    if (hif::equals(boundType, semType)) {
        // Removing syntactic type.
        delete semType;
        delete dstCv->setType(nullptr);
        return dstBound;
    } else {
        // Setting cv syntactic type to semType
        dstCv->setType(semType);
        // Adding a cast of cv to boundType.
        Cast *c = new Cast();
        c->setType(boundType);
        c->setValue(dstCv);

        // remeber to assign base type
        //_castMap[c] = nullptr;

        return c;
    }
}

void HifStdVisitor::_sortDeclarations(BList<Declaration> &srcList, BList<Declaration> &dstList)
{
    dstList.removeAll();
    for (BList<Declaration>::iterator i = srcList.begin(); i != srcList.end(); ++i) {
        Declaration *dstDecl = _mapTypedGet(*i);
        messageAssert(dstDecl != nullptr, "Cannot find mapped declaration", *i, _srcSem);
        dstList.push_back(dstDecl);
    }
}

// //////////////////////////////////////////////////
// Object visitors
// //////////////////////////////////////////////////
int HifStdVisitor::visitAggregate(Aggregate &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitAggregate(o);

    // We need to preserve the Bitvector type of Aggregate.
    // Original bit can be mapped to bool (e.g. HIF -> SystemC).
    //
    // E.g. an Aggregate of not-logic bits may become Aggregate of Bool,
    // which makes its full type Array(Bool) instead of Bitvector.
    //
    // Therefore, we push a mapped-Bitvector cast to elements.

    Type *srcType = getSemanticType(&o, _srcSem);
    messageDebugAssert(srcType != nullptr, "Cannot find source type of Aggregate", &o, _srcSem);

    Type *mappedType = _dstGetType(srcType, true);
    messageDebugAssert(mappedType != nullptr, "Cannot find mappedType of Aggregate", srcType, _srcSem);

    Type *srcElementType = getVectorElementType(mappedType, _dstSem);
    messageDebugAssert(srcElementType != nullptr, "Cannot find getVectorElementType of Aggregate", mappedType, _dstSem);

    Aggregate *dstObj = _mapTypedGet(&o);
    messageDebugAssert(dstObj != nullptr, "Cannot find mapped Aggregate", &o, _srcSem);

    _dstPushCastToElements(o.alts, dstObj->alts, o.getOthers(), dstObj->getOthers(), srcElementType);

    delete srcElementType;
    delete mappedType;

    return 0;
}
int HifStdVisitor::visitAggregateAlt(AggregateAlt &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitAggregateAlt(o);

    for (BList<Value>::iterator i = o.indices.begin(); i != o.indices.end(); ++i) {
        _mapIndex(*i);
    }

    return 0;
}
int HifStdVisitor::visitAlias(Alias &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitAlias(o);

    messageAssert(o.getType() != nullptr, "Aliases without type are not supported yet.", &o, _srcSem);

    _mapInitialValue(&o);

    return 0;
}
int HifStdVisitor::visitArray(Array &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitArray(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitAssign(Assign &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitAssign(o);

    _mapAssign(&o);

    return 0;
}
int HifStdVisitor::visitBit(Bit &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitBit(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitBitValue(BitValue &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    hif::manipulation::assureSyntacticType(&o, _srcSem);

    GuideVisitor::visitBitValue(o);

    _mapConstValue(&o);

    return 0;
}
int HifStdVisitor::visitBitvector(Bitvector &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitBitvector(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitBitvectorValue(BitvectorValue &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    hif::manipulation::assureSyntacticType(&o, _srcSem);

    GuideVisitor::visitBitvectorValue(o);

    _mapConstValue(&o);

    return 0;
}
int HifStdVisitor::visitBool(Bool &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitBool(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitBoolValue(BoolValue &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    hif::manipulation::assureSyntacticType(&o, _srcSem);

    GuideVisitor::visitBoolValue(o);

    _mapConstValue(&o);

    return 0;
}
int HifStdVisitor::visitBreak(Break &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitBreak(o);

    return 0;
}
int HifStdVisitor::visitCast(Cast &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitCast(o);

    Type *srcType = hif::semantics::getBaseType(hif::semantics::getSemanticType(o.getValue(), _srcSem), false, _srcSem);
    messageAssert(srcType != nullptr, "Cannot type op", &o, _srcSem);

    _fillCastMap(_mapTypedGet(&o), hif::copy(srcType));

    return 0;
}
int HifStdVisitor::visitChar(Char &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitChar(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitCharValue(CharValue &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    hif::manipulation::assureSyntacticType(&o, _srcSem);

    GuideVisitor::visitCharValue(o);

    _mapConstValue(&o);

    return 0;
}
int HifStdVisitor::visitConst(Const &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitConst(o);

    messageAssert(o.getValue() != nullptr, "Const must have an initial value.", &o, _srcSem);

    _mapInitialValue(&o);

    return 0;
}
int HifStdVisitor::visitContents(Contents &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitContents(o);

    _sortDeclarations(o.declarations, _mapTypedGet(&o)->declarations);

    return 0;
}
int HifStdVisitor::visitContinue(Continue &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitContinue(o);
    return 0;
}
int HifStdVisitor::visitDesignUnit(DesignUnit &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitDesignUnit(o);
    return 0;
}
int HifStdVisitor::visitEnum(Enum &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitEnum(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitEnumValue(EnumValue &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitEnumValue(o);

    if (o.getValue() != nullptr)
        _mapInitialValue(&o);

    return 0;
}

int HifStdVisitor::visitEvent(Event &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitEvent(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitExpression(Expression &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitExpression(o);

    _mapExpression(&o);

    return 0;
}
int HifStdVisitor::visitFunctionCall(FunctionCall &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    // fix parameters and template parameters names.
    FunctionCall::DeclarationType *decl = hif::manipulation::instantiate(&o, _srcSem);

    FunctionCall::DeclarationType *treeDecl = hif::semantics::getDeclaration(&o, _srcSem);

    hif::manipulation::SortMissingKind kind = hif::declarationIsPartOfStandard(treeDecl)
                                                  ? hif::manipulation::SortMissingKind::NOTHING
                                                  : _dstSem->getSemanticsOptions().lang_sortKind;

    hif::manipulation::sortParameters(o.parameterAssigns, decl->parameters, true, kind, _srcSem);
    hif::manipulation::sortParameters(o.templateParameterAssigns, decl->templateParameters, true, kind, _srcSem);

    _dstGetDeclaration(&o);

    GuideVisitor::visitFunctionCall(o);

    _mapReferencedAssignList(o.templateParameterAssigns);

    _mapReferencedAssignList(o.parameterAssigns);

    return 0;
}
int HifStdVisitor::visitField(Field &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    _assureInitialValue(&o);

    GuideVisitor::visitField(o);

    _mapInitialValue(&o);

    return 0;
}
int HifStdVisitor::visitFieldReference(FieldReference &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    _dstGetDeclaration(&o);

    GuideVisitor::visitFieldReference(o);

    _mapValue(&o);

    return 0;
}
int HifStdVisitor::visitFile(File &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitFile(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitFor(For &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitFor(o);

    _mapCondition(o.getCondition());

    _sortDeclarations(
        o.initDeclarations.toOtherBList<Declaration>(), _mapTypedGet(&o)->initDeclarations.toOtherBList<Declaration>());

    return 0;
}
int HifStdVisitor::visitForGenerate(ForGenerate &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitForGenerate(o);

    _mapCondition(o.getCondition());

    _sortDeclarations(o.declarations, _mapTypedGet(&o)->declarations);
    _sortDeclarations(
        o.initDeclarations.toOtherBList<Declaration>(), _mapTypedGet(&o)->initDeclarations.toOtherBList<Declaration>());

    return 0;
}
int HifStdVisitor::visitFunction(Function &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitFunction(o);

    return 0;
}
int HifStdVisitor::visitGlobalAction(GlobalAction &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitGlobalAction(o);

    return 0;
}
int HifStdVisitor::visitEntity(Entity &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitEntity(o);

    return 0;
}
int HifStdVisitor::visitIdentifier(Identifier &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    _dstGetDeclaration(&o);

    GuideVisitor::visitIdentifier(o);

    return 0;
}
int HifStdVisitor::visitIf(If &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitIf(o);
    return 0;
}
int HifStdVisitor::visitIfAlt(IfAlt &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitIfAlt(o);

    _mapCondition(o.getCondition());

    return 0;
}
int HifStdVisitor::visitIfGenerate(IfGenerate &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitIfGenerate(o);

    _mapCondition(o.getCondition());
    _sortDeclarations(o.declarations, _mapTypedGet(&o)->declarations);

    return 0;
}
int HifStdVisitor::visitInstance(Instance &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    // Fix assign names:
    Instance::DeclarationType *decl = hif::manipulation::instantiate(&o, _srcSem);
    if (decl != nullptr) {
        hif::manipulation::sortParameters(
            o.portAssigns, decl->ports, true, hif::manipulation::SortMissingKind::NOTHING, _srcSem);
    } else {
        messageAssert(o.portAssigns.empty(), "Source declaration not found", &o, _srcSem);
    }

    _dstGetDeclaration(&o);

    GuideVisitor::visitInstance(o);

    _mapReferencedAssignList(o.portAssigns);

    return 0;
}
int HifStdVisitor::visitInt(Int &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitInt(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitIntValue(IntValue &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    hif::manipulation::assureSyntacticType(&o, _srcSem);

    GuideVisitor::visitIntValue(o);

    _mapConstValue(&o);

    return 0;
}
int HifStdVisitor::visitLibraryDef(LibraryDef &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitLibraryDef(o);

    _sortDeclarations(o.declarations, _mapTypedGet(&o)->declarations);

    return 0;
}
int HifStdVisitor::visitLibrary(Library &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    _dstGetDeclaration(&o);

    GuideVisitor::visitLibrary(o);

    return 0;
}
int HifStdVisitor::visitMember(Member &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitMember(o);

    _mapIndex(o.getIndex());
    _mapMember(&o);

    return 0;
}
int HifStdVisitor::visitNull(Null &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitNull(o);

    return 0;
}
int HifStdVisitor::visitTransition(Transition &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitTransition(o);

    return 0;
}
int HifStdVisitor::visitParameterAssign(ParameterAssign &o)
{
    _fixPPAssignSrcDirection(&o);

    if (!_dstCopyObject(&o))
        return 0;

    _dstGetDeclaration(&o);

    GuideVisitor::visitParameterAssign(o);

    return 0;
}
int HifStdVisitor::visitParameter(Parameter &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitParameter(o);

    if (o.getValue() != nullptr)
        _mapInitialValue(&o);

    return 0;
}
int HifStdVisitor::visitPointer(Pointer &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitPointer(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitPortAssign(PortAssign &o)
{
    _fixPPAssignSrcDirection(&o);

    if (!_dstCopyObject(&o))
        return 0;

    _dstGetDeclaration(&o);

    GuideVisitor::visitPortAssign(o);

    return 0;
}
int HifStdVisitor::visitPort(Port &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    if (o.getDirection() == dir_in) {
        delete o.setValue(nullptr);
    } else //  o.getDirection() == dir_out || dir_inout
    {
        _assureInitialValue(&o);
    }

    GuideVisitor::visitPort(o);

    messageAssert(o.getDirection() != dir_none, "Port direction not set.", &o, _srcSem);
    Type *destType = _mapTypedGet(o.getType());
    messageAssert(_dstSem->isTypeAllowedAsPort(destType), "Port type is not allowed", destType, _dstSem);

    if (o.getDirection() != dir_in)
        _mapInitialValue(&o);

    return 0;
}
int HifStdVisitor::visitProcedure(Procedure &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitProcedure(o);

    return 0;
}
int HifStdVisitor::visitProcedureCall(ProcedureCall &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    // fix parameters and template parameters names.
    ProcedureCall::DeclarationType *decl = hif::manipulation::instantiate(&o, _srcSem);

    ProcedureCall::DeclarationType *treeDecl = hif::semantics::getDeclaration(&o, _srcSem);

    hif::manipulation::SortMissingKind kind = hif::declarationIsPartOfStandard(treeDecl)
                                                  ? hif::manipulation::SortMissingKind::NOTHING
                                                  : _dstSem->getSemanticsOptions().lang_sortKind;

    hif::manipulation::sortParameters(o.parameterAssigns, decl->parameters, true, kind, _srcSem);
    hif::manipulation::sortParameters(o.templateParameterAssigns, decl->templateParameters, true, kind, _srcSem);

    _dstGetDeclaration(&o);

    GuideVisitor::visitProcedureCall(o);

    _mapReferencedAssignList(o.templateParameterAssigns);

    _mapReferencedAssignList(o.parameterAssigns);

    return 0;
}
int HifStdVisitor::visitRange(Range &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitRange(o);

    if (o.getLeftBound() != nullptr)
        _mapIndex(o.getLeftBound());
    if (o.getRightBound() != nullptr)
        _mapIndex(o.getRightBound());

    if (o.getLeftBound() != nullptr && o.getRightBound() != nullptr) {
        _makeCompatibleBounds(o.getLeftBound(), o.getRightBound());
    }

    return 0;
}
int HifStdVisitor::visitReal(Real &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitReal(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitRealValue(RealValue &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    hif::manipulation::assureSyntacticType(&o, _srcSem);

    GuideVisitor::visitRealValue(o);

    _mapConstValue(&o);

    return 0;
}
int HifStdVisitor::visitRecord(Record &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitRecord(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitRecordValue(RecordValue &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitRecordValue(o);

    // useless
    //_mapValue( &o );

    return 0;
}
int HifStdVisitor::visitRecordValueAlt(RecordValueAlt &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitRecordValueAlt(o);

    return 0;
}
int HifStdVisitor::visitReference(Reference &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitReference(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitReturn(Return &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitReturn(o);

    _mapReturn(&o);

    return 0;
}
int HifStdVisitor::visitSignal(Signal &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    _assureInitialValue(&o);

    GuideVisitor::visitSignal(o);

    _mapInitialValue(&o);

    return 0;
}
int HifStdVisitor::visitSigned(Signed &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitSigned(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitSlice(Slice &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitSlice(o);

    _mapSlice(&o);

    // Put a cast on the slice if slice type is different wrt source and dest
    // semantics, and slice is a top. Ref.design: LPF3x8
    if (_isTop(&o))
        _mapValue(&o);

    return 0;
}
int HifStdVisitor::visitState(State &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitState(o);

    return 0;
}
int HifStdVisitor::visitString(String &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitString(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitStateTable(StateTable &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitStateTable(o);

    StateTable *dstObj = _mapTypedGet(&o);
    messageAssert(dstObj != nullptr, "Object not found in map", &o, _dstSem);

    _removeListCasts(o.sensitivity, dstObj->sensitivity);
    _removeListCasts(o.sensitivityPos, dstObj->sensitivityPos);
    _removeListCasts(o.sensitivityNeg, dstObj->sensitivityNeg);

    _sortDeclarations(o.declarations, dstObj->declarations);

    return 0;
}
int HifStdVisitor::visitSwitchAlt(SwitchAlt &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitSwitchAlt(o);

    return 0;
}
int HifStdVisitor::visitSwitch(Switch &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitSwitch(o);

    Switch *dstObj = _mapTypedGet(&o);
    messageAssert(dstObj != nullptr, "Object not found in map", &o, _dstSem);

    // Ensuring matching can be performed:
    _dstPushCastToConditions(o.alts, dstObj->alts, o.getCondition(), dstObj->getCondition());

    return 0;
}
int HifStdVisitor::visitSystem(System &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitSystem(o);

    _sortDeclarations(o.declarations, _mapTypedGet(&o)->declarations);

    return 0;
}
int HifStdVisitor::visitStringValue(StringValue &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    hif::manipulation::assureSyntacticType(&o, _srcSem);

    GuideVisitor::visitStringValue(o);

    _mapConstValue(&o);

    return 0;
}
int HifStdVisitor::visitTime(Time &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitTime(o);

    _mapType(&o);

    return 0;
}
int HifStdVisitor::visitTimeValue(TimeValue &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    hif::manipulation::assureSyntacticType(&o, _srcSem);

    GuideVisitor::visitTimeValue(o);

    _mapConstValue(&o);

    return 0;
}
int HifStdVisitor::visitTypeDef(TypeDef &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitTypeDef(o);

    return 0;
}
int HifStdVisitor::visitTypeReference(TypeReference &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    // fix template parameters names.
    TypeReference::DeclarationType *decl = hif::manipulation::instantiate(&o, _srcSem);

    if (dynamic_cast<TypeDef *>(decl) != nullptr) {
        TypeDef *td = static_cast<TypeDef *>(decl);

        TypeReference::DeclarationType *treeDecl = hif::semantics::getDeclaration(&o, _srcSem);

        hif::manipulation::SortMissingKind kind = hif::declarationIsPartOfStandard(treeDecl)
                                                      ? hif::manipulation::SortMissingKind::NOTHING
                                                      : _dstSem->getSemanticsOptions().lang_sortKind;

        hif::manipulation::sortParameters(o.templateParameterAssigns, td->templateParameters, true, kind, _srcSem);
    }

    _dstGetDeclaration(&o);

    GuideVisitor::visitTypeReference(o);

    _mapReferencedAssignList(o.templateParameterAssigns);

    return 0;
}
int HifStdVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    _dstGetDeclaration(&o);

    GuideVisitor::visitTypeTPAssign(o);

    return 0;
}
int HifStdVisitor::visitTypeTP(TypeTP &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitTypeTP(o);

    return 0;
}
int HifStdVisitor::visitUnsigned(Unsigned &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitUnsigned(o);

    _mapType(&o);

    return 0;
}

int HifStdVisitor::visitValueStatement(ValueStatement &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitValueStatement(o);

    return 0;
}

int HifStdVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    _dstGetDeclaration(&o);

    GuideVisitor::visitValueTPAssign(o);

    return 0;
}
int HifStdVisitor::visitValueTP(ValueTP &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitValueTP(o);

    if (o.getValue() != nullptr)
        _mapInitialValue(&o);

    return 0;
}
int HifStdVisitor::visitVariable(Variable &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    // Initial value is required unless type is ViewReference.
    if (dynamic_cast<ViewReference *>(getBaseType(o.getType(), false, _srcSem)) == nullptr) {
        _assureInitialValue(&o);
    }

    GuideVisitor::visitVariable(o);

    if (o.getValue() != nullptr)
        _mapInitialValue(&o);

    return 0;
}
int HifStdVisitor::visitView(View &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitView(o);

    _sortDeclarations(o.declarations, _mapTypedGet(&o)->declarations);

    return 0;
}
int HifStdVisitor::visitViewReference(ViewReference &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    // fix template parameters names.
    ViewReference::DeclarationType *decl = hif::manipulation::instantiate(&o, _srcSem);
    messageAssert(decl != nullptr, "Cannot instantiate view reference.", &o, _srcSem);

    if (decl != nullptr) {
        ViewReference::DeclarationType *treeDecl = hif::semantics::getDeclaration(&o, _srcSem);

        hif::manipulation::SortMissingKind kind = hif::declarationIsPartOfStandard(treeDecl)
                                                      ? hif::manipulation::SortMissingKind::NOTHING
                                                      : _dstSem->getSemanticsOptions().lang_sortKind;

        hif::manipulation::sortParameters(o.templateParameterAssigns, decl->templateParameters, true, kind, _srcSem);
    }

    _dstGetDeclaration(&o);

    GuideVisitor::visitViewReference(o);

    _mapReferencedAssignList(o.templateParameterAssigns);

    return 0;
}
int HifStdVisitor::visitWait(Wait &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitWait(o);

    Wait *dstObj = _mapTypedGet(&o);
    messageAssert(dstObj != nullptr, "Object not found in map", &o, _dstSem);

    if (o.getCondition() != nullptr)
        _mapCondition(o.getCondition());
    if (o.getRepetitions() != nullptr)
        _mapIndex(o.getRepetitions());
    _removeListCasts(o.sensitivity, dstObj->sensitivity);

    return 0;
}
int HifStdVisitor::visitWhen(When &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitWhen(o);

    Type *srcType = getSemanticType(&o, _srcSem);
    messageAssert(srcType != nullptr, "Cannot find source object type", &o, _srcSem);

    When *dstObj = _mapTypedGet(&o);
    messageAssert(dstObj != nullptr, "Object not found in map", &o, _dstSem);

    Type *mappedType = _dstGetType(srcType, false);
    messageAssert(mappedType != nullptr, "Cannot find mapped type", srcType, _dstSem);

    _dstPushCastToElements(o.alts, dstObj->alts, o.getDefault(), dstObj->getDefault(), mappedType);

    delete mappedType;

    return 0;
}
int HifStdVisitor::visitWhenAlt(WhenAlt &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitWhenAlt(o);

    _mapCondition(o.getCondition());

    return 0;
}
int HifStdVisitor::visitWhile(While &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitWhile(o);

    _mapCondition(o.getCondition());

    return 0;
}
int HifStdVisitor::visitWith(With &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitWith(o);

    Type *srcType = getSemanticType(&o, _srcSem);
    messageAssert(srcType != nullptr, "Cannot find source object type", &o, _srcSem);

    With *dstObj = _mapTypedGet(&o);
    messageAssert(dstObj != nullptr, "Object not found in map", &o, _dstSem);

    Type *mappedType = _dstGetType(srcType, false);
    messageAssert(mappedType != nullptr, "Cannot find mapped type", srcType, _dstSem);

    // Ensuring all alts return the same type:
    _dstPushCastToElements(o.alts, dstObj->alts, o.getDefault(), dstObj->getDefault(), mappedType);

    // Ensuring matching can be performed:
    _dstPushCastToConditions(o.alts, dstObj->alts, o.getCondition(), dstObj->getCondition());

    delete mappedType;

    return 0;
}

int HifStdVisitor::visitWithAlt(WithAlt &o)
{
    if (!_dstCopyObject(&o))
        return 0;

    GuideVisitor::visitWithAlt(o);

    return 0;
}

} // namespace
System *standardizeHif(
    System *o,
    hif::semantics::ILanguageSemantics *src,
    hif::semantics::ILanguageSemantics *dst,
    CastMap &castMap)
{
    HifStdVisitor v(src, dst, castMap);
    o->acceptVisitor(v);
    return v.getResult(o);
}

} // namespace semantics
} // namespace hif
