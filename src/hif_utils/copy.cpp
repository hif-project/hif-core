/// @file copy.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstring>

#include "hif/HifVisitor.hpp"
#include "hif/hif_utils/copy.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace
{
class CopyVisitor : public HifVisitor
{
public:
    CopyVisitor(const CopyOptions &opt);

    virtual ~CopyVisitor();

    virtual int visitAggregate(Aggregate &o);
    virtual int visitAggregateAlt(AggregateAlt &o);
    virtual int visitAlias(Alias &o);
    virtual int visitArray(Array &o);
    virtual int visitAssign(Assign &o);
    virtual int visitSystem(System &o);
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
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitField(Field &o);
    virtual int visitFieldReference(FieldReference &o);
    virtual int visitFile(File &o);
    virtual int visitFor(For &o);
    virtual int visitForGenerate(ForGenerate &o);
    virtual int visitFunction(Function &o);
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
    virtual int visitProcedureCall(ProcedureCall &o);
    virtual int visitPointer(Pointer &o);
    virtual int visitPortAssign(PortAssign &o);
    virtual int visitPort(Port &o);
    virtual int visitProcedure(Procedure &o);
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

    /// @name Parents visits
    /// @{

    int visitAction(Action &o);
    int visitAlt(Alt &o);
    int visitBaseContents(BaseContents &o);
    int visitCompositeType(CompositeType &o);
    int visitConstValue(ConstValue &o);
    int visitDataDeclaration(DataDeclaration &o);
    int visitDeclaration(Declaration &o);
    int visitGenerate(Generate &o);
    int visitObject(Object &o);
    int visitPPAssign(PPAssign &o);
    int visitPrefixedReference(PrefixedReference &o);
    int visitReferencedAssign(ReferencedAssign &o);
    int visitReferencedType(ReferencedType &o);
    int visitScope(Scope &o);
    int visitScopedType(ScopedType &o);
    int visitSimpleType(SimpleType &o);
    int visitSubProgram(SubProgram &o);
    int visitTPAssign(TPAssign &o);
    int visitType(Type &o);
    int visitTypeDeclaration(TypeDeclaration &o);
    int visitTypedObject(TypedObject &o);
    int visitValue(Value &o);

    /// @}

    /// @name Features visits
    /// @{

    int visitNamedObject(hif::features::INamedObject *destobj, hif::features::INamedObject &o);
    int visitSymbol(hif::features::ISymbol *destobj, hif::features::ISymbol &o);
    int visitTypeSpan(features::ITypeSpan *destobj, hif::features::ITypeSpan &o);

    /// @}

    void _callUserFunction(Object *s, Object *d);

    template <typename T> T *_copyChild(T *s);

    template <typename T> void _copyChild(BList<T> &s, BList<T> &d);

    /// @brief Copies the properties.
    /// This function copies src properties field in dst. It uses <tt>copy</tt> to create
    /// a copy of the Comment associated.
    ///
    /// @param src the object containing the properties to be copied
    /// @param dst the destination object where insert copied properties
    ///
    void _copyProperties(Object *src, Object *dst);

    /// @brief Copies the additional keywords.
    /// This function copies src additional keywords field in dst.
    /// @param src the object containing the properties to be copied
    /// @param dst the destination object where insert copied list
    void _copyAdditionalKeywords(Declaration *src, Declaration *dst);

    /// @brief Copies the CodeInfo.
    ///
    void _copyCodeInfo(Object *o, Object *n);

    /// @brief Copies a comment.
    /// This function copies src comment field in dst. It uses <tt>copy</tt> to create
    /// a copy of the Comment associated.
    ///
    void _copyComment(Object *src, Object *dst);

    Object *_result;
    const CopyOptions _opt;

private:
    // disabled
    CopyVisitor(const CopyVisitor &o);
    CopyVisitor &operator=(const CopyVisitor &o);
};
CopyVisitor::CopyVisitor(const CopyOptions &opt)
    : _result(nullptr)
    , _opt(opt)
{
    // ntd
}
CopyVisitor::~CopyVisitor()
{
    // ntd
}
void CopyVisitor::_callUserFunction(Object *s, Object *d)
{
    if (_opt.userFunction == nullptr)
        return;
    _result = (*_opt.userFunction)(s, d, _opt.userData);
}
template <typename T> T *CopyVisitor::_copyChild(T *s)
{
    if (!_opt.copyChildObjects)
        return nullptr;
    return copy(s, _opt);
}
template <typename T> void CopyVisitor::_copyChild(BList<T> &s, BList<T> &d)
{
    if (!_opt.copyChildObjects)
        return;
    copy(s, d, _opt);
}
void CopyVisitor::_copyProperties(Object *src, Object *dst)
{
    if (src == nullptr || dst == nullptr)
        return;
    if (!_opt.copyProperties)
        return;
    if (!src->hasProperties())
        return;
    Object::PropertyMapIterator endIt = src->getPropertyEndIterator();
    for (Object::PropertyMapIterator it = src->getPropertyBeginIterator(); it != endIt; ++it) {
        dst->addProperty(it->first, copy(it->second));
    }
}

void CopyVisitor::_copyAdditionalKeywords(Declaration *src, Declaration *dst)
{
    if (src == nullptr || dst == nullptr)
        return;
    if (!src->hasAdditionalKeywords())
        return;
    for (Declaration::KeywordList::iterator it = src->getAdditionalKeywordsBeginIterator();
         it != src->getAdditionalKeywordsEndIterator(); ++it) {
        dst->addAdditionalKeyword(*it);
    }
}

void CopyVisitor::_copyComment(Object *src, Object *dst)
{
    if (src == nullptr || dst == nullptr)
        return;
    if (!_opt.copyComments)
        return;
    if (!src->hasComments())
        return;
    dst->addComments(src->getComments());
}
void CopyVisitor::_copyCodeInfo(Object *o, Object *n)
{
    if ((o == nullptr) || (n == nullptr))
        return;
    if (!_opt.copyCodeInfos)
        return;

    n->setSourceFileName(o->getSourceFileName());
    n->setSourceLineNumber(o->getSourceLineNumber());
    n->setSourceColumnNumber(o->getSourceColumnNumber());
}
int CopyVisitor::visitAggregate(Aggregate &o)
{
    Aggregate *destobj = new Aggregate();
    _result            = destobj;

    _copyChild(o.alts, destobj->alts);
    destobj->setOthers(_copyChild(o.getOthers()));

    visitValue(o);
    return 0;
}
int CopyVisitor::visitAggregateAlt(AggregateAlt &o)
{
    AggregateAlt *destobj = new AggregateAlt();
    _result               = destobj;

    _copyChild(o.indices, destobj->indices);
    destobj->setValue(_copyChild(o.getValue()));

    visitAlt(o);
    return 0;
}
int CopyVisitor::visitAlias(Alias &o)
{
    Alias *destobj = new Alias();
    _result        = destobj;

    destobj->setStandard(o.isStandard());
    visitDataDeclaration(o);
    return 0;
}
int CopyVisitor::visitArray(Array &o)
{
    Array *destobj = new Array();
    _result        = destobj;

    destobj->setSigned(o.isSigned());

    visitTypeSpan(destobj, o);
    visitCompositeType(o);
    return 0;
}
int CopyVisitor::visitAssign(Assign &o)
{
    Assign *destobj = new Assign();
    _result         = destobj;

    destobj->setRightHandSide(_copyChild(o.getRightHandSide()));
    destobj->setLeftHandSide(_copyChild(o.getLeftHandSide()));
    destobj->setDelay(_copyChild(o.getDelay()));

    visitAction(o);
    return 0;
}
int CopyVisitor::visitSystem(System &o)
{
    System *destobj = new System();
    _result         = destobj;

    destobj->setLanguageID(o.getLanguageID());
    destobj->setVersionInfo(o.getVersionInfo());
    _copyChild(o.libraryDefs, destobj->libraryDefs);
    _copyChild(o.designUnits, destobj->designUnits);
    _copyChild(o.declarations, destobj->declarations);
    _copyChild(o.libraries, destobj->libraries);
    _copyChild(o.actions, destobj->actions);

    visitScope(o);
    return 0;
}
int CopyVisitor::visitBit(Bit &o)
{
    Bit *destobj = new Bit();
    _result      = destobj;

    destobj->setLogic(o.isLogic());
    destobj->setResolved(o.isResolved());

    visitSimpleType(o);
    return 0;
}
int CopyVisitor::visitBitValue(BitValue &o)
{
    BitValue *destobj = new BitValue();
    _result           = destobj;

    destobj->setValue(o.getValue());

    visitConstValue(o);
    return 0;
}
int CopyVisitor::visitBitvector(Bitvector &o)
{
    Bitvector *destobj = new Bitvector();
    _result            = destobj;

    destobj->setSigned(o.isSigned());
    destobj->setLogic(o.isLogic());
    destobj->setResolved(o.isResolved());

    visitTypeSpan(destobj, o);
    visitSimpleType(o);
    return 0;
}
int CopyVisitor::visitBitvectorValue(BitvectorValue &o)
{
    BitvectorValue *destobj = new BitvectorValue();
    _result                 = destobj;

    destobj->setValue(o.getValue());

    visitConstValue(o);
    return 0;
}
int CopyVisitor::visitBool(Bool &o)
{
    Bool *destobj = new Bool();
    _result       = destobj;

    visitSimpleType(o);
    return 0;
}
int CopyVisitor::visitBoolValue(BoolValue &o)
{
    BoolValue *destobj = new BoolValue();
    _result            = destobj;

    destobj->setValue(o.getValue());

    visitConstValue(o);
    return 0;
}
int CopyVisitor::visitIf(If &o)
{
    If *destobj = new If();
    _result     = destobj;

    _copyChild(o.alts, destobj->alts);
    _copyChild(o.defaults, destobj->defaults);

    visitAction(o);
    return 0;
}
int CopyVisitor::visitIfAlt(IfAlt &o)
{
    IfAlt *destobj = new IfAlt();
    _result        = destobj;

    destobj->setCondition(_copyChild(o.getCondition()));
    _copyChild(o.actions, destobj->actions);

    visitAlt(o);
    return 0;
}
int CopyVisitor::visitCast(Cast &o)
{
    Cast *destobj = new Cast();
    _result       = destobj;

    destobj->setType(_copyChild(o.getType()));
    destobj->setValue(_copyChild(o.getValue()));

    visitValue(o);
    return 0;
}
int CopyVisitor::visitChar(Char &o)
{
    Char *destobj = new Char();
    _result       = destobj;

    visitSimpleType(o);
    return 0;
}
int CopyVisitor::visitCharValue(CharValue &o)
{
    CharValue *destobj = new CharValue();
    _result            = destobj;

    destobj->setValue(o.getValue());

    visitConstValue(o);
    return 0;
}
int CopyVisitor::visitConst(Const &o)
{
    Const *destobj = new Const();
    _result        = destobj;

    destobj->setInstance(o.isInstance());
    destobj->setDefine(o.isDefine());
    destobj->setStandard(o.isStandard());

    visitDataDeclaration(o);
    return 0;
}
int CopyVisitor::visitContents(Contents &o)
{
    Contents *destobj = new Contents();
    _result           = destobj;

    _copyChild(o.libraries, destobj->libraries);

    visitBaseContents(o);
    return 0;
}
int CopyVisitor::visitDesignUnit(DesignUnit &o)
{
    DesignUnit *destobj = new DesignUnit();
    _result             = destobj;

    _copyChild(o.views, destobj->views);

    visitScope(o);
    return 0;
}
int CopyVisitor::visitEnum(Enum &o)
{
    Enum *destobj = new Enum();
    _result       = destobj;

    _copyChild(o.values, destobj->values);

    visitScopedType(o);
    return 0;
}
int CopyVisitor::visitEnumValue(EnumValue &o)
{
    EnumValue *destobj = new EnumValue();
    _result            = destobj;

    visitDataDeclaration(o);
    return 0;
}

int CopyVisitor::visitEvent(Event &o)
{
    Event *destobj = new Event();
    _result        = destobj;

    visitSimpleType(o);
    return 0;
}

int CopyVisitor::visitExpression(Expression &o)
{
    Expression *destobj = new Expression();
    _result             = destobj;

    destobj->setOperator(o.getOperator());
    destobj->setValue1(_copyChild(o.getValue1()));
    destobj->setValue2(_copyChild(o.getValue2()));

    visitValue(o);
    return 0;
}
int CopyVisitor::visitBreak(Break &o)
{
    Break *destobj = new Break();
    _result        = destobj;

    visitNamedObject(destobj, o);
    visitAction(o);
    return 0;
}
int CopyVisitor::visitField(Field &o)
{
    Field *destobj = new Field();
    _result        = destobj;

    destobj->setDirection(o.getDirection());
    visitDataDeclaration(o);
    return 0;
}
int CopyVisitor::visitFieldReference(FieldReference &o)
{
    FieldReference *destobj = new FieldReference();
    _result                 = destobj;

    visitNamedObject(destobj, o);
    visitSymbol(destobj, o);
    visitPrefixedReference(o);
    return 0;
}
int CopyVisitor::visitFile(File &o)
{
    File *destobj = new File();
    _result       = destobj;

    visitCompositeType(o);
    return 0;
}
int CopyVisitor::visitFunctionCall(FunctionCall &o)
{
    FunctionCall *destobj = new FunctionCall();
    _result               = destobj;

    _copyChild(o.templateParameterAssigns, destobj->templateParameterAssigns);
    _copyChild(o.parameterAssigns, destobj->parameterAssigns);
    destobj->setInstance(_copyChild(o.getInstance()));

    visitNamedObject(destobj, o);
    visitSymbol(destobj, o);
    visitValue(o);
    return 0;
}
int CopyVisitor::visitFor(For &o)
{
    For *destobj = new For();
    _result      = destobj;

    _copyChild(o.initDeclarations, destobj->initDeclarations);
    _copyChild(o.initValues, destobj->initValues);
    _copyChild(o.stepActions, destobj->stepActions);
    _copyChild(o.forActions, destobj->forActions);
    destobj->setCondition(_copyChild(o.getCondition()));

    visitNamedObject(destobj, o);
    visitAction(o);
    return 0;
}
int CopyVisitor::visitForGenerate(ForGenerate &o)
{
    ForGenerate *destobj = new ForGenerate();
    _result              = destobj;

    _copyChild(o.initDeclarations, destobj->initDeclarations);
    _copyChild(o.initValues, destobj->initValues);
    _copyChild(o.stepActions, destobj->stepActions);
    destobj->setCondition(_copyChild(o.getCondition()));

    visitGenerate(o);
    return 0;
}
int CopyVisitor::visitFunction(Function &o)
{
    Function *destobj = new Function();
    _result           = destobj;

    destobj->setType(_copyChild(o.getType()));

    visitSubProgram(o);
    return 0;
}
int CopyVisitor::visitGlobalAction(GlobalAction &o)
{
    GlobalAction *destobj = new GlobalAction();
    _result               = destobj;

    _copyChild(o.actions, destobj->actions);

    visitObject(o);
    return 0;
}
int CopyVisitor::visitIfGenerate(IfGenerate &o)
{
    IfGenerate *destobj = new IfGenerate();
    _result             = destobj;

    destobj->setCondition(_copyChild(o.getCondition()));
    visitGenerate(o);
    return 0;
}
int CopyVisitor::visitEntity(Entity &o)
{
    Entity *destobj = new Entity();
    _result         = destobj;

    _copyChild(o.ports, destobj->ports);
    _copyChild(o.parameters, destobj->parameters);

    visitScope(o);
    return 0;
}
int CopyVisitor::visitInstance(Instance &o)
{
    Instance *destobj = new Instance();
    _result           = destobj;

    _copyChild(o.portAssigns, destobj->portAssigns);
    destobj->setReferencedType(_copyChild(o.getReferencedType()));
    destobj->setValue(_copyChild(o.getValue()));

    visitSymbol(destobj, o);
    visitNamedObject(destobj, o);
    visitValue(o);
    return 0;
}
int CopyVisitor::visitInt(Int &o)
{
    Int *destobj = new Int();
    _result      = destobj;

    destobj->setSigned(o.isSigned());

    visitTypeSpan(destobj, o);
    visitSimpleType(o);
    return 0;
}
int CopyVisitor::visitIntValue(IntValue &o)
{
    IntValue *destobj = new IntValue();
    _result           = destobj;

    destobj->setValue(o.getValue());

    visitConstValue(o);
    return 0;
}
int CopyVisitor::visitLibrary(Library &o)
{
    Library *destobj = new Library();
    _result          = destobj;

    destobj->setFilename(o.getFilename());
    destobj->setStandard(o.isStandard());
    destobj->setSystem(o.isSystem());

    visitSymbol(destobj, o);
    visitReferencedType(o);
    return 0;
}
int CopyVisitor::visitLibraryDef(LibraryDef &o)
{
    LibraryDef *destobj = new LibraryDef();
    _result             = destobj;

    _copyChild(o.libraries, destobj->libraries);
    _copyChild(o.declarations, destobj->declarations);
    destobj->setLanguageID(o.getLanguageID());
    destobj->setStandard(o.isStandard());
    destobj->setCLinkage(o.hasCLinkage());

    visitScope(o);
    return 0;
}
int CopyVisitor::visitMember(Member &o)
{
    Member *destobj = new Member();
    _result         = destobj;

    destobj->setIndex(_copyChild(o.getIndex()));

    visitPrefixedReference(o);
    return 0;
}
int CopyVisitor::visitIdentifier(Identifier &o)
{
    Identifier *destobj = new Identifier();
    _result             = destobj;

    visitNamedObject(destobj, o);
    visitSymbol(destobj, o);
    visitValue(o);
    return 0;
}
int CopyVisitor::visitContinue(Continue &o)
{
    Continue *destobj = new Continue();
    _result           = destobj;

    visitNamedObject(destobj, o);
    visitAction(o);
    return 0;
}
int CopyVisitor::visitNull(Null &o)
{
    Null *destobj = new Null();
    _result       = destobj;

    visitAction(o);
    return 0;
}
int CopyVisitor::visitTransition(Transition &o)
{
    Transition *destobj = new Transition();
    _result             = destobj;

    destobj->setName(o.getName());
    destobj->setPrevName(o.getPrevName());
    destobj->setPriority(o.getPriority());
    _copyChild(o.enablingLabelList, destobj->enablingLabelList);
    _copyChild(o.enablingList, destobj->enablingList);
    _copyChild(o.updateLabelList, destobj->updateLabelList);
    _copyChild(o.updateList, destobj->updateList);
    destobj->setEnablingOrCondition(o.getEnablingOrCondition());

    visitAction(o);
    return 0;
}
int CopyVisitor::visitParameter(Parameter &o)
{
    Parameter *destobj = new Parameter();
    _result            = destobj;

    destobj->setDirection(o.getDirection());

    visitDataDeclaration(o);
    return 0;
}
int CopyVisitor::visitParameterAssign(ParameterAssign &o)
{
    ParameterAssign *destobj = new ParameterAssign();
    _result                  = destobj;

    visitSymbol(destobj, o);
    visitPPAssign(o);
    return 0;
}
int CopyVisitor::visitProcedureCall(ProcedureCall &o)
{
    ProcedureCall *destobj = new ProcedureCall();
    _result                = destobj;

    _copyChild(o.templateParameterAssigns, destobj->templateParameterAssigns);
    _copyChild(o.parameterAssigns, destobj->parameterAssigns);
    destobj->setInstance(_copyChild(o.getInstance()));

    visitSymbol(destobj, o);
    visitNamedObject(destobj, o);
    visitAction(o);
    return 0;
}
int CopyVisitor::visitPointer(Pointer &o)
{
    Pointer *destobj = new Pointer();
    _result          = destobj;

    visitCompositeType(o);
    return 0;
}
int CopyVisitor::visitPort(Port &o)
{
    Port *destobj = new Port();
    _result       = destobj;

    destobj->setDirection(o.getDirection());
    destobj->setWrapper(o.isWrapper());

    visitDataDeclaration(o);
    return 0;
}
int CopyVisitor::visitPortAssign(PortAssign &o)
{
    PortAssign *destobj = new PortAssign();
    _result             = destobj;

    destobj->setType(_copyChild(o.getType()));
    destobj->setPartialBind(_copyChild(o.getPartialBind()));

    visitSymbol(destobj, o);
    visitPPAssign(o);
    return 0;
}
int CopyVisitor::visitProcedure(Procedure &o)
{
    Procedure *destobj = new Procedure();
    _result            = destobj;

    visitSubProgram(o);
    return 0;
}
int CopyVisitor::visitRange(Range &o)
{
    Range *destobj = new Range();
    _result        = destobj;

    destobj->setDirection(o.getDirection());
    destobj->setLeftBound(_copyChild(o.getLeftBound()));
    destobj->setRightBound(_copyChild(o.getRightBound()));
    destobj->setType(_copyChild(o.getType()));

    visitValue(o);
    return 0;
}
int CopyVisitor::visitReal(Real &o)
{
    Real *destobj = new Real();
    _result       = destobj;

    visitTypeSpan(destobj, o);
    visitSimpleType(o);
    return 0;
}
int CopyVisitor::visitRealValue(RealValue &o)
{
    RealValue *destobj = new RealValue();
    _result            = destobj;

    destobj->setValue(o.getValue());

    visitConstValue(o);
    return 0;
}
int CopyVisitor::visitRecord(Record &o)
{
    Record *destobj = new Record();
    _result         = destobj;

    if (_opt.copySemanticsTypes) {
        destobj->setBaseType(_copyChild(o.getBaseType(false)), false);
        destobj->setBaseType(_copyChild(o.getBaseType(true)), true);
    }

    _copyChild(o.fields, destobj->fields);
    destobj->setPacked(o.isPacked());
    destobj->setUnion(o.isUnion());

    visitScopedType(o);
    return 0;
}
int CopyVisitor::visitRecordValue(RecordValue &o)
{
    RecordValue *destobj = new RecordValue();
    _result              = destobj;

    _copyChild(o.alts, destobj->alts);

    visitValue(o);
    return 0;
}
int CopyVisitor::visitRecordValueAlt(RecordValueAlt &o)
{
    RecordValueAlt *destobj = new RecordValueAlt();
    _result                 = destobj;

    destobj->setValue(_copyChild(o.getValue()));

    visitNamedObject(destobj, o);
    visitAlt(o);
    return 0;
}
int CopyVisitor::visitReference(Reference &o)
{
    Reference *destobj = new Reference();
    _result            = destobj;

    visitCompositeType(o);
    return 0;
}
int CopyVisitor::visitReturn(Return &o)
{
    Return *destobj = new Return();
    _result         = destobj;

    destobj->setValue(_copyChild(o.getValue()));

    visitAction(o);
    return 0;
}
int CopyVisitor::visitSignal(Signal &o)
{
    Signal *destobj = new Signal();
    _result         = destobj;

    destobj->setStandard(o.isStandard());
    destobj->setWrapper(o.isWrapper());

    visitDataDeclaration(o);
    return 0;
}
int CopyVisitor::visitSigned(Signed &o)
{
    Signed *destobj = new Signed();
    _result         = destobj;

    visitTypeSpan(destobj, o);
    visitSimpleType(o);
    return 0;
}
int CopyVisitor::visitSlice(Slice &o)
{
    Slice *destobj = new Slice();
    _result        = destobj;

    destobj->setSpan(_copyChild(o.getSpan()));

    visitPrefixedReference(o);
    return 0;
}
int CopyVisitor::visitStateTable(StateTable &o)
{
    StateTable *destobj = new StateTable();
    _result             = destobj;

    destobj->setFlavour(o.getFlavour());
    destobj->setDontInitialize(o.getDontInitialize());
    destobj->setStandard(o.isStandard());
    _copyChild(o.declarations, destobj->declarations);
    _copyChild(o.sensitivity, destobj->sensitivity);
    _copyChild(o.sensitivityPos, destobj->sensitivityPos);
    _copyChild(o.sensitivityNeg, destobj->sensitivityNeg);
    _copyChild(o.states, destobj->states);
    _copyChild(o.edges, destobj->edges);
    destobj->setEntryStateName(o.getEntryStateName()); // after copy of states

    visitScope(o);
    return 0;
}
int CopyVisitor::visitState(State &o)
{
    State *destobj = new State();
    _result        = destobj;

    destobj->setPriority(o.getPriority());
    _copyChild(o.actions, destobj->actions);
    _copyChild(o.invariants, destobj->invariants);
    destobj->setAtomic(o.isAtomic());

    visitDeclaration(o);
    return 0;
}
int CopyVisitor::visitString(String &o)
{
    String *destobj = new String();
    _result         = destobj;

    destobj->setSpanInformation(_copyChild(o.getSpanInformation()));
    visitSimpleType(o);
    return 0;
}
int CopyVisitor::visitSwitch(Switch &o)
{
    Switch *destobj = new Switch();
    _result         = destobj;

    destobj->setCaseSemantics(o.getCaseSemantics());
    _copyChild(o.alts, destobj->alts);
    _copyChild(o.defaults, destobj->defaults);
    destobj->setCondition(_copyChild(o.getCondition()));

    visitAction(o);
    return 0;
}
int CopyVisitor::visitSwitchAlt(SwitchAlt &o)
{
    SwitchAlt *destobj = new SwitchAlt();
    _result            = destobj;

    _copyChild(o.conditions, destobj->conditions);
    _copyChild(o.actions, destobj->actions);

    visitAlt(o);
    return 0;
}
int CopyVisitor::visitStringValue(StringValue &o)
{
    StringValue *destobj = new StringValue();
    _result              = destobj;

    destobj->setValue(o.getValue());

    visitConstValue(o);
    return 0;
}
int CopyVisitor::visitTime(Time &o)
{
    Time *destobj = new Time();
    _result       = destobj;

    visitSimpleType(o);
    return 0;
}
int CopyVisitor::visitTimeValue(TimeValue &o)
{
    TimeValue *destobj = new TimeValue();
    _result            = destobj;

    destobj->setValue(o.getValue());
    destobj->setUnit(o.getUnit());

    visitConstValue(o);
    return 0;
}
int CopyVisitor::visitTypeDef(TypeDef &o)
{
    TypeDef *destobj = new TypeDef();
    _result          = destobj;

    _copyChild(o.templateParameters, destobj->templateParameters);
    destobj->setRange(_copyChild(o.getRange()));
    destobj->setOpaque(o.isOpaque());
    destobj->setStandard(o.isStandard());
    destobj->setExternal(o.isExternal());

    visitTypeDeclaration(o);
    return 0;
}
int CopyVisitor::visitTypeReference(TypeReference &o)
{
    TypeReference *destobj = new TypeReference();
    _result                = destobj;

    _copyChild(o.templateParameterAssigns, destobj->templateParameterAssigns);
    _copyChild(o.ranges, destobj->ranges);

    visitSymbol(destobj, o);
    visitReferencedType(o);
    return 0;
}
int CopyVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    TypeTPAssign *destobj = new TypeTPAssign();
    _result               = destobj;

    destobj->setType(_copyChild(o.getType()));

    visitSymbol(destobj, o);
    visitTPAssign(o);
    return 0;
}
int CopyVisitor::visitTypeTP(TypeTP &o)
{
    TypeTP *destobj = new TypeTP();
    _result         = destobj;

    visitTypeDeclaration(o);
    return 0;
}
int CopyVisitor::visitUnsigned(Unsigned &o)
{
    Unsigned *destobj = new Unsigned();
    _result           = destobj;

    visitTypeSpan(destobj, o);
    visitSimpleType(o);
    return 0;
}

int CopyVisitor::visitValueStatement(ValueStatement &o)
{
    ValueStatement *destobj = new ValueStatement();
    _result                 = destobj;

    destobj->setValue(_copyChild(o.getValue()));

    visitAction(o);
    return 0;
}

int CopyVisitor::visitVariable(Variable &o)
{
    Variable *destobj = new Variable();
    _result           = destobj;

    destobj->setInstance(o.isInstance());
    destobj->setStandard(o.isStandard());

    visitDataDeclaration(o);
    return 0;
}
int CopyVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    ValueTPAssign *destobj = new ValueTPAssign();
    _result                = destobj;

    destobj->setValue(_copyChild(o.getValue()));

    visitSymbol(destobj, o);
    visitTPAssign(o);
    return 0;
}
int CopyVisitor::visitValueTP(ValueTP &o)
{
    ValueTP *destobj = new ValueTP();
    _result          = destobj;

    destobj->setCompileTimeConstant(o.isCompileTimeConstant());

    visitDataDeclaration(o);
    return 0;
}
int CopyVisitor::visitView(View &o)
{
    View *destobj = new View();
    _result       = destobj;

    _copyChild(o.libraries, destobj->libraries);
    _copyChild(o.templateParameters, destobj->templateParameters);
    _copyChild(o.declarations, destobj->declarations);
    _copyChild(o.inheritances, destobj->inheritances);
    destobj->setContents(_copyChild(o.getContents()));
    destobj->setEntity(_copyChild(o.getEntity()));
    destobj->setLanguageID(o.getLanguageID());
    destobj->setStandard(o.isStandard());
    destobj->setFilename(o.getFilename());

    visitScope(o);
    return 0;
}
int CopyVisitor::visitViewReference(ViewReference &o)
{
    ViewReference *destobj = new ViewReference();
    _result                = destobj;

    _copyChild(o.templateParameterAssigns, destobj->templateParameterAssigns);
    destobj->setDesignUnit(o.getDesignUnit());

    visitSymbol(destobj, o);
    visitReferencedType(o);
    return 0;
}
int CopyVisitor::visitWait(Wait &o)
{
    Wait *destobj = new Wait();
    _result       = destobj;

    _copyChild(o.sensitivity, destobj->sensitivity);
    _copyChild(o.sensitivityPos, destobj->sensitivityPos);
    _copyChild(o.sensitivityNeg, destobj->sensitivityNeg);
    _copyChild(o.actions, destobj->actions);
    destobj->setCondition(_copyChild(o.getCondition()));
    destobj->setRepetitions(_copyChild(o.getRepetitions()));
    destobj->setTime(_copyChild(o.getTime()));

    visitAction(o);
    return 0;
}
int CopyVisitor::visitWhen(When &o)
{
    When *destobj = new When();
    _result       = destobj;

    destobj->setLogicTernary(o.isLogicTernary());
    _copyChild(o.alts, destobj->alts);
    destobj->setDefault(_copyChild(o.getDefault()));

    visitValue(o);
    return 0;
}
int CopyVisitor::visitWhenAlt(WhenAlt &o)
{
    WhenAlt *destobj = new WhenAlt();
    _result          = destobj;

    destobj->setCondition(_copyChild(o.getCondition()));
    destobj->setValue(_copyChild(o.getValue()));

    visitAlt(o);
    return 0;
}
int CopyVisitor::visitWhile(While &o)
{
    While *destobj = new While();
    _result        = destobj;

    _copyChild(o.actions, destobj->actions);
    destobj->setDoWhile(o.isDoWhile());
    destobj->setCondition(_copyChild(o.getCondition()));

    visitNamedObject(destobj, o);
    visitAction(o);
    return 0;
}
int CopyVisitor::visitWith(With &o)
{
    With *destobj = new With();
    _result       = destobj;

    destobj->setCaseSemantics(o.getCaseSemantics());
    _copyChild(o.alts, destobj->alts);
    destobj->setCondition(_copyChild(o.getCondition()));
    destobj->setDefault(_copyChild(o.getDefault()));

    visitValue(o);
    return 0;
}
int CopyVisitor::visitWithAlt(WithAlt &o)
{
    WithAlt *destobj = new WithAlt();
    _result          = destobj;

    _copyChild(o.conditions, destobj->conditions);
    destobj->setValue(_copyChild(o.getValue()));

    visitAlt(o);
    return 0;
}

// parent visits

int CopyVisitor::visitAction(Action &o)
{
    visitObject(o);
    return 0;
}

int CopyVisitor::visitAlt(Alt &o)
{
    visitObject(o);
    return 0;
}

int CopyVisitor::visitBaseContents(BaseContents &o)
{
    BaseContents *destobj = static_cast<BaseContents *>(_result);
    _copyChild(o.declarations, destobj->declarations);
    _copyChild(o.stateTables, destobj->stateTables);
    _copyChild(o.generates, destobj->generates);
    _copyChild(o.instances, destobj->instances);
    destobj->setGlobalAction(_copyChild(o.getGlobalAction()));

    visitScope(o);
    return 0;
}

int CopyVisitor::visitCompositeType(CompositeType &o)
{
    CompositeType *destobj = static_cast<CompositeType *>(_result);
    destobj->setType(_copyChild(o.getType()));
    if (_opt.copySemanticsTypes) {
        destobj->setBaseType(_copyChild(o.getBaseType(false)), false);
        destobj->setBaseType(_copyChild(o.getBaseType(true)), true);
    }

    visitType(o);
    return 0;
}

int CopyVisitor::visitConstValue(ConstValue &o)
{
    ConstValue *destobj = static_cast<ConstValue *>(_result);
    destobj->setType(_copyChild(o.getType()));

    visitValue(o);
    return 0;
}

int CopyVisitor::visitDataDeclaration(DataDeclaration &o)
{
    DataDeclaration *destobj = static_cast<DataDeclaration *>(_result);
    destobj->setType(_copyChild(o.getType()));
    destobj->setValue(_copyChild(o.getValue()));
    destobj->setRange(_copyChild(o.getRange()));

    visitDeclaration(o);
    return 0;
}

int CopyVisitor::visitDeclaration(Declaration &o)
{
    Declaration *destobj = static_cast<Declaration *>(_result);

    _copyAdditionalKeywords(&o, destobj);
    visitNamedObject(destobj, o);
    visitObject(o);
    return 0;
}

int CopyVisitor::visitGenerate(Generate &o)
{
    visitBaseContents(o);
    return 0;
}

int CopyVisitor::visitObject(Object &o)
{
    _copyProperties(&o, _result);
    _copyComment(&o, _result);
    _copyCodeInfo(&o, _result);
    _callUserFunction(&o, _result);
    return 0;
}

int CopyVisitor::visitPPAssign(PPAssign &o)
{
    PPAssign *destobj = static_cast<PPAssign *>(_result);
    destobj->setDirection(o.getDirection());
    destobj->setValue(_copyChild(o.getValue()));

    visitReferencedAssign(o);
    return 0;
}

int CopyVisitor::visitPrefixedReference(PrefixedReference &o)
{
    PrefixedReference *destobj = static_cast<PrefixedReference *>(_result);
    destobj->setPrefix(_copyChild(o.getPrefix()));

    visitValue(o);
    return 0;
}

int CopyVisitor::visitReferencedAssign(ReferencedAssign &o)
{
    ReferencedAssign *destobj = static_cast<ReferencedAssign *>(_result);

    visitNamedObject(destobj, o);
    visitTypedObject(o);
    return 0;
}

int CopyVisitor::visitReferencedType(ReferencedType &o)
{
    ReferencedType *destobj = static_cast<ReferencedType *>(_result);
    destobj->setInstance(_copyChild(o.getInstance()));

    visitNamedObject(destobj, o);
    visitType(o);
    return 0;
}

int CopyVisitor::visitScope(Scope &o)
{
    visitDeclaration(o);
    return 0;
}

int CopyVisitor::visitScopedType(ScopedType &o)
{
    ScopedType *destobj = static_cast<ScopedType *>(_result);
    destobj->setConstexpr(o.isConstexpr());

    visitType(o);
    return 0;
}

int CopyVisitor::visitSimpleType(SimpleType &o)
{
    SimpleType *destobj = static_cast<SimpleType *>(_result);
    destobj->setConstexpr(o.isConstexpr());

    visitType(o);
    return 0;
}

int CopyVisitor::visitSubProgram(SubProgram &o)
{
    SubProgram *destobj = static_cast<SubProgram *>(_result);
    _copyChild(o.parameters, destobj->parameters);
    _copyChild(o.templateParameters, destobj->templateParameters);
    destobj->setStandard(o.isStandard());
    destobj->setKind(o.getKind());
    destobj->setStateTable(_copyChild(o.getStateTable()));

    visitScope(o);
    return 0;
}

int CopyVisitor::visitTPAssign(TPAssign &o)
{
    visitReferencedAssign(o);
    return 0;
}

int CopyVisitor::visitType(Type &o)
{
    SimpleType *destobj = static_cast<SimpleType *>(_result);
    destobj->setTypeVariant(o.getTypeVariant());

    visitObject(o);
    return 0;
}

int CopyVisitor::visitTypeDeclaration(TypeDeclaration &o)
{
    TypeDeclaration *destobj = static_cast<TypeDeclaration *>(_result);
    destobj->setType(_copyChild(o.getType()));

    visitScope(o);
    return 0;
}

int CopyVisitor::visitTypedObject(TypedObject &o)
{
    TypedObject *destobj = static_cast<TypedObject *>(_result);
    if (_opt.copySemanticsTypes) {
        destobj->setSemanticType(_copyChild(o.getSemanticType()));
    }

    visitObject(o);
    return 0;
}

int CopyVisitor::visitValue(Value &o)
{
    visitTypedObject(o);
    return 0;
}

int CopyVisitor::visitNamedObject(hif::features::INamedObject *destobj, hif::features::INamedObject &o)
{
    destobj->setName(o.getName());
    return 0;
}

int CopyVisitor::visitSymbol(hif::features::ISymbol *destobj, hif::features::ISymbol &o)
{
    if (!_opt.copyDeclarations)
        return 0;
    hif::semantics::DeclarationOptions opt;
    opt.dontSearch = true;
    hif::semantics::setDeclaration(destobj->toObject(), hif::semantics::getDeclaration(o.toObject(), nullptr, opt));
    return 0;
}

int CopyVisitor::visitTypeSpan(hif::features::ITypeSpan *destobj, features::ITypeSpan &o)
{
    destobj->setSpan(_copyChild(o.getSpan()));
    return 0;
}

} // namespace
Object *copy(const Object *obj, const CopyOptions &opt)
{
    if (obj == nullptr)
        return nullptr;
    CopyVisitor v(opt);
    const_cast<Object *>(obj)->acceptVisitor(v);
    return v._result;
}

void copy(const BList<Object> &src, BList<Object> &dest, const CopyOptions &opt)
{
    dest.clear();
    for (BList<Object>::iterator i = src.begin(); i != src.end(); ++i) {
        dest.push_back(copy(*i, opt));
    }
}

CopyOptions::CopyOptions()
    : copySemanticsTypes(false)
    , copyDeclarations(true)
    , copyChildObjects(true)
    , copyProperties(true)
    , copyCodeInfos(true)
    , copyComments(true)
    , userFunction(nullptr)
    , userData(nullptr)
{
    // ntd
}

CopyOptions::~CopyOptions()
{
    // ntd
}

CopyOptions::CopyOptions(const CopyOptions &o)
    : copySemanticsTypes(o.copySemanticsTypes)
    , copyDeclarations(o.copyDeclarations)
    , copyChildObjects(o.copyChildObjects)
    , copyProperties(o.copyProperties)
    , copyCodeInfos(o.copyCodeInfos)
    , copyComments(o.copyComments)
    , userFunction(o.userFunction)
    , userData(o.userData)
{
    // ntd
}

CopyOptions &CopyOptions::operator=(const CopyOptions &o)
{
    if (&o == this)
        return *this;
    copySemanticsTypes = o.copySemanticsTypes;
    copyDeclarations   = o.copyDeclarations;
    copyChildObjects   = o.copyChildObjects;
    copyProperties     = o.copyProperties;
    copyCodeInfos      = o.copyCodeInfos;
    copyComments       = o.copyComments;
    userFunction       = o.userFunction;
    userData           = o.userData;
    return *this;
}

} // namespace hif
