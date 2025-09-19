/// @file objectGetKey.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

#include "hif/hif.hpp"
#include "hif/hifPrinter.hpp"

//extern char *strlower (char *str);

namespace hif
{

namespace
{
class ObjectKeyVisitor : public GuideVisitor
{
public:
    /// @brief Constructor.
    ObjectKeyVisitor();
    /// @brief Destructor.
    virtual ~ObjectKeyVisitor();

    /// @brief Returns the result string
    std::string getResult() const;

    /// @name Hif Objects visitors
    /// @{

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
    virtual int visitEntity(Entity &o);
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
    virtual int visitIdentifier(Identifier &o);
    virtual int visitIfAlt(IfAlt &o);
    virtual int visitIf(If &o);
    virtual int visitIfGenerate(IfGenerate &o);
    virtual int visitInt(Int &o);
    virtual int visitIntValue(IntValue &o);
    virtual int visitInstance(Instance &o);
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
    virtual int visitSystem(System &o);
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

    /// @}

    /// @brief Override of visit of BLists.
    virtual int visitList(BList<Object> &l);
    virtual int AfterVisit(Object &o);

private:
    std::stringstream _result;

    inline void _flag2String(bool v) { _result << '[' << (v ? '1' : '0') << ']'; }

    inline void _enum2String(int v) { _result << '[' << v << ']'; }

    inline void _int2String(std::int64_t v) { _result << v; }

    inline void _double2String(const double v) { _result << v; }

    inline void _visitField(Object *o)
    {
        if (o == nullptr) {
            _result << "()";
        } else {
            o->acceptVisitor(*this);
        }
    }

    // K: disabled.
    ObjectKeyVisitor(const ObjectKeyVisitor &);
    ObjectKeyVisitor &operator=(const ObjectKeyVisitor &);
};

ObjectKeyVisitor::ObjectKeyVisitor()
    : _result()
{
    // ntd
}

ObjectKeyVisitor::~ObjectKeyVisitor()
{
    // ntd
}

std::string ObjectKeyVisitor::getResult() const { return _result.str(); }

int ObjectKeyVisitor::visitList(BList<Object> &l)
{
    _result << "{";
    for (BList<Object>::iterator i = l.begin(); i != l.end(); ++i) {
        (*i)->acceptVisitor(*this);
        _result << ";";
    }
    _result << "}";
    return 0;
}

int ObjectKeyVisitor::AfterVisit(Object &o)
{
    if (!o.hasProperties())
        return 0;
    _result << "[PROPERTIES:";

    Object::PropertyMapIterator i   = o.getPropertyBeginIterator();
    Object::PropertyMapIterator end = o.getPropertyEndIterator();

    for (; i != end; ++i) {
        _result << "(" << i->first;
        if (i->second != nullptr) {
            i->second->acceptVisitor(*this);
        }
        _result << ")";
    }
    _result << "]";

    // If declaration check for additional keywords
    if (dynamic_cast<Declaration *>(&o) != nullptr) {
        Declaration *d = static_cast<Declaration *>(&o);
        if (d->hasAdditionalKeywords()) {
            _result << "[ADDITIONAL_KEYWORDS:";
            for (Declaration::KeywordList::iterator kwit = d->getAdditionalKeywordsBeginIterator();
                 kwit != d->getAdditionalKeywordsEndIterator(); ++kwit) {
                _result << "(" << *kwit << ")";
            }
            _result << "]";
        }
    }
    return 0;
}

int ObjectKeyVisitor::visitAggregate(Aggregate &o)
{
    _result << "(Aggregate";
    GuideVisitor::visitAggregate(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitAggregateAlt(AggregateAlt &o)
{
    _result << "(AggregateAlt";
    GuideVisitor::visitAggregateAlt(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitAlias(Alias &o)
{
    _result << "(Alias";
    _result << o.getName();
    _flag2String(o.isStandard());
    GuideVisitor::visitAlias(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitArray(Array &o)
{
    _result << "(Array";
    _flag2String(o.isSigned());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitArray(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitAssign(Assign &o)
{
    _result << "(Assign";
    _visitField(o.getLeftHandSide());
    _visitField(o.getRightHandSide());
    _visitField(o.getDelay());
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitSystem(System &o)
{
    _result << "(System";
    _enum2String(static_cast<int>(o.getLanguageID()));
    System::VersionInfo version = o.getVersionInfo();
    _result << version.release;
    _result << version.tool;
    //_result << version.generationDate;
    //_result << version.formatVersion;
    GuideVisitor::visitSystem(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitBit(Bit &o)
{
    _result << "(Bit";
    _flag2String(o.isConstexpr());
    _flag2String(o.isLogic());
    _flag2String(o.isResolved());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitBit(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitBitValue(BitValue &o)
{
    _result << "(BitValue";
    _enum2String(static_cast<int>(o.getValue()));
    GuideVisitor::visitBitValue(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitBitvector(Bitvector &o)
{
    _result << "(Bitvector";
    _flag2String(o.isConstexpr());
    _flag2String(o.isLogic());
    _flag2String(o.isResolved());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitBitvector(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitBitvectorValue(BitvectorValue &o)
{
    _result << "(BitvectorValue";
    _result << o.getValue();
    GuideVisitor::visitBitvectorValue(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitBool(Bool &o)
{
    _result << "(Bool";
    _flag2String(o.isConstexpr());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitBool(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitBoolValue(BoolValue &o)
{
    _result << "(BoolValue";
    _flag2String(o.getValue());
    GuideVisitor::visitBoolValue(o);
    _result << ")";
    return 0;
}
int ObjectKeyVisitor::visitIfAlt(IfAlt &o)
{
    _result << "(IfAlt";
    GuideVisitor::visitIfAlt(o);
    _result << ")";
    return 0;
}
int ObjectKeyVisitor::visitIf(If &o)
{
    _result << "(If";
    GuideVisitor::visitIf(o);
    _result << ")";
    return 0;
}
int ObjectKeyVisitor::visitCast(Cast &o)
{
    _result << "(Cast";
    GuideVisitor::visitCast(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitChar(Char &o)
{
    _result << "(Char";
    _flag2String(o.isConstexpr());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitChar(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitCharValue(CharValue &o)
{
    _result << "(CharValue";
    if (o.getValue() == '\0')
        _result << "'NUL'";
    else if (o.getValue() == '\n')
        _result << "'LF'";
    else
        _result << o.getValue();
    GuideVisitor::visitCharValue(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitConst(Const &o)
{
    _result << "(Const";
    _result << o.getName();
    _flag2String(o.isStandard());
    _flag2String(o.isInstance());
    _flag2String(o.isDefine());
    GuideVisitor::visitConst(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitContents(Contents &o)
{
    _result << "(Contents";
    _result << o.getName();
    GuideVisitor::visitContents(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitDesignUnit(DesignUnit &o)
{
    _result << "(DesignUnit";
    _result << o.getName();
    GuideVisitor::visitDesignUnit(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitEnum(Enum &o)
{
    _result << "(Enum";
    _flag2String(o.isConstexpr());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitEnum(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitEnumValue(EnumValue &o)
{
    _result << "(EnumValue";
    _result << o.getName();
    GuideVisitor::visitEnumValue(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitEvent(Event &o)
{
    _result << "(Event";
    _flag2String(o.isConstexpr());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitEvent(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitBreak(Break &o)
{
    _result << "(Break";
    _result << o.getName();
    GuideVisitor::visitBreak(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitExpression(Expression &o)
{
    _result << "(Expression";
    _enum2String(static_cast<int>(o.getOperator()));
    _visitField(o.getValue1());
    _visitField(o.getValue2());
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitFunctionCall(FunctionCall &o)
{
    _result << "(FunctionCall";
    _result << o.getName();
    GuideVisitor::visitFunctionCall(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitField(Field &o)
{
    _result << "(Field";
    _result << o.getName();
    _enum2String(static_cast<int>(o.getDirection()));
    GuideVisitor::visitField(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitFieldReference(FieldReference &o)
{
    _result << "(FieldReference";
    _result << o.getName();
    GuideVisitor::visitFieldReference(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitFile(File &o)
{
    _result << "(File";
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitFile(o);
    _result << ")";
    return 0;
}
int ObjectKeyVisitor::visitFor(For &o)
{
    _result << "(For";
    _result << o.getName();
    GuideVisitor::visitFor(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitForGenerate(ForGenerate &o)
{
    _result << "(ForGenerate";
    _result << o.getName();
    GuideVisitor::visitForGenerate(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitFunction(Function &o)
{
    _result << "(Function";
    _result << o.getName();
    _flag2String(o.isStandard());
    _enum2String(static_cast<int>(o.getKind()));
    GuideVisitor::visitFunction(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitGlobalAction(GlobalAction &o)
{
    _result << "(GlobalAction";
    GuideVisitor::visitGlobalAction(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitEntity(Entity &o)
{
    _result << "(Entity";
    _result << o.getName();
    GuideVisitor::visitEntity(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitIfGenerate(IfGenerate &o)
{
    _result << "(IfGenerate";
    _result << o.getName();
    GuideVisitor::visitIfGenerate(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitInt(Int &o)
{
    _result << "(Int";
    _flag2String(o.isConstexpr());
    _flag2String(o.isSigned());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitInt(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitIntValue(IntValue &o)
{
    _result << "(IntValue";
    _int2String(o.getValue());
    GuideVisitor::visitIntValue(o);
    _result << ")";
    return 0;
}
int ObjectKeyVisitor::visitInstance(Instance &o)
{
    _result << "(Instance";
    _result << o.getName();
    GuideVisitor::visitInstance(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitLibraryDef(LibraryDef &o)
{
    _result << "(LibraryDef";
    _result << o.getName();
    _flag2String(o.isStandard());
    _flag2String(o.hasCLinkage());
    _enum2String(static_cast<int>(o.getLanguageID()));
    GuideVisitor::visitLibraryDef(o);
    _result << ")";
    return 0;
}
int ObjectKeyVisitor::visitLibrary(Library &o)
{
    _result << "(Library";
    _result << o.getName();
    _result << o.getFilename();
    _flag2String(o.isStandard());
    _flag2String(o.isSystem());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitLibrary(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitMember(Member &o)
{
    _result << "(Member";
    GuideVisitor::visitMember(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitIdentifier(Identifier &o)
{
    _result << "(Identifier";
    _result << o.getName();
    GuideVisitor::visitIdentifier(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitContinue(Continue &o)
{
    _result << "(Continue";
    _result << o.getName();
    GuideVisitor::visitContinue(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitNull(Null &o)
{
    _result << "(Null";
    GuideVisitor::visitNull(o);
    _result << ")";
    return 0;
}
int ObjectKeyVisitor::visitTransition(Transition &o)
{
    _result << "(Transition";
    _result << o.getName();
    _result << o.getPrevName().c_str();
    _int2String(static_cast<std::int64_t>(o.getPriority()));
    GuideVisitor::visitTransition(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitParameterAssign(ParameterAssign &o)
{
    _result << "(ParameterAssign";
    _result << o.getName();
    _enum2String(static_cast<int>(o.getDirection()));
    GuideVisitor::visitParameterAssign(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitParameter(Parameter &o)
{
    _result << "(Parameter";
    _result << o.getName();
    _enum2String(static_cast<int>(o.getDirection()));
    GuideVisitor::visitParameter(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitProcedureCall(ProcedureCall &o)
{
    _result << "(ProcedureCall";
    _result << o.getName();
    GuideVisitor::visitProcedureCall(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitPointer(Pointer &o)
{
    _result << "(Pointer";
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitPointer(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitPortAssign(PortAssign &o)
{
    _result << "(PortAssign";
    _result << o.getName();
    _enum2String(static_cast<int>(o.getDirection()));
    GuideVisitor::visitPortAssign(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitPort(Port &o)
{
    _result << "(Port";
    _result << o.getName();
    _enum2String(static_cast<int>(o.getDirection()));
    _flag2String(o.isWrapper());
    GuideVisitor::visitPort(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitProcedure(Procedure &o)
{
    _result << "(Procedure";
    _result << o.getName();
    _flag2String(o.isStandard());
    _enum2String(static_cast<int>(o.getKind()));
    GuideVisitor::visitProcedure(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitRange(Range &o)
{
    _result << "(Range";
    _enum2String(static_cast<int>(o.getDirection()));
    _visitField(o.getLeftBound());
    _visitField(o.getRightBound());
    _visitField(o.getType());
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitReal(Real &o)
{
    _result << "(Real";
    _flag2String(o.isConstexpr());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitReal(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitRealValue(RealValue &o)
{
    _result << "(RealValue";
    _double2String(o.getValue());
    GuideVisitor::visitRealValue(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitRecord(Record &o)
{
    _result << "(Record";
    _flag2String(o.isConstexpr());
    _flag2String(o.isPacked());
    _flag2String(o.isUnion());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitRecord(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitRecordValue(RecordValue &o)
{
    _result << "(RecordValue";
    GuideVisitor::visitRecordValue(o);
    _result << ")";
    return 0;
}
int ObjectKeyVisitor::visitRecordValueAlt(RecordValueAlt &o)
{
    _result << "(RecordValueAlt";
    _result << o.getName();
    GuideVisitor::visitRecordValueAlt(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitReference(Reference &o)
{
    _result << "(Reference";
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitReference(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitReturn(Return &o)
{
    _result << "(Return";
    GuideVisitor::visitReturn(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitSignal(Signal &o)
{
    _result << "(Signal";
    _result << o.getName();
    _flag2String(o.isStandard());
    _flag2String(o.isWrapper());
    GuideVisitor::visitSignal(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitSigned(Signed &o)
{
    _result << "(Signed";
    _flag2String(o.isConstexpr());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitSigned(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitSlice(Slice &o)
{
    _result << "(Slice";
    GuideVisitor::visitSlice(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitState(State &o)
{
    _result << "(State";
    _result << o.getName();
    _flag2String(o.isAtomic());
    _int2String(static_cast<std::int64_t>(o.getPriority()));
    GuideVisitor::visitState(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitString(String &o)
{
    _result << "(String";
    _flag2String(o.isConstexpr());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitString(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitStateTable(StateTable &o)
{
    _result << "(StateTable";
    _result << o.getName();
    _flag2String(o.isStandard());
    _flag2String(o.getDontInitialize());
    _enum2String(static_cast<int>(o.getFlavour()));
    GuideVisitor::visitStateTable(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitSwitchAlt(SwitchAlt &o)
{
    _result << "(SwitchAlt";
    GuideVisitor::visitSwitchAlt(o);
    _result << ")";
    return 0;
}
int ObjectKeyVisitor::visitSwitch(Switch &o)
{
    _result << "(Switch";
    _enum2String(static_cast<int>(o.getCaseSemantics()));
    GuideVisitor::visitSwitch(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitStringValue(StringValue &o)
{
    _result << "(StringValue";
    _result << o.getValue();
    _flag2String(o.isPlain());
    GuideVisitor::visitStringValue(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitTime(Time &o)
{
    _result << "(Time";
    _flag2String(o.isConstexpr());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitTime(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitTimeValue(TimeValue &o)
{
    _result << "(TimeValue";
    _double2String(o.getValue());
    _enum2String(static_cast<int>(o.getUnit()));
    GuideVisitor::visitTimeValue(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitTypeDef(TypeDef &o)
{
    _result << "(TypeDef";
    _result << o.getName();
    _flag2String(o.isStandard());
    _flag2String(o.isOpaque());
    _flag2String(o.isExternal());
    GuideVisitor::visitTypeDef(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitTypeReference(TypeReference &o)
{
    _result << "(TypeReference";
    _result << o.getName();
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitTypeReference(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    _result << "(TypeTPAssign";
    _result << o.getName();
    GuideVisitor::visitTypeTPAssign(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitTypeTP(TypeTP &o)
{
    _result << "(TypeTP";
    _result << o.getName();
    GuideVisitor::visitTypeTP(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitUnsigned(Unsigned &o)
{
    _result << "(Unsigned";
    _flag2String(o.isConstexpr());
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitUnsigned(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitValueStatement(ValueStatement &o)
{
    _result << "(ValueStatement";
    GuideVisitor::visitValueStatement(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    _result << "(ValueTPAssign";
    _result << o.getName();
    GuideVisitor::visitValueTPAssign(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitValueTP(ValueTP &o)
{
    _result << "(ValueTP";
    _result << o.getName();
    _flag2String(o.isCompileTimeConstant());
    GuideVisitor::visitValueTP(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitVariable(Variable &o)
{
    _result << "(Variable";
    _result << o.getName();
    _flag2String(o.isStandard());
    _flag2String(o.isInstance());
    GuideVisitor::visitVariable(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitView(View &o)
{
    _result << "(View";
    _result << o.getName();
    _flag2String(o.isStandard());
    _enum2String(static_cast<int>(o.getLanguageID()));
    GuideVisitor::visitView(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitViewReference(ViewReference &o)
{
    _result << "(ViewReference";
    _result << o.getName();
    _result << o.getDesignUnit().c_str();
    _enum2String(static_cast<int>(o.getTypeVariant()));
    GuideVisitor::visitViewReference(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitWait(Wait &o)
{
    _result << "(Wait";
    GuideVisitor::visitWait(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitWhen(When &o)
{
    _result << "(When";
    _flag2String(o.isLogicTernary());
    GuideVisitor::visitWhen(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitWhenAlt(WhenAlt &o)
{
    _result << "(WhenAlt";
    _visitField(o.getCondition());
    _visitField(o.getValue());
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitWhile(While &o)
{
    _result << "(While";
    _result << o.getName();
    _flag2String(o.isDoWhile());
    GuideVisitor::visitWhile(o);
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitWith(With &o)
{
    _result << "(With";
    _enum2String(static_cast<int>(o.getCaseSemantics()));
    _visitField(o.getCondition());
    visitList(o.alts);
    _visitField(o.getDefault());
    _result << ")";
    return 0;
}

int ObjectKeyVisitor::visitWithAlt(WithAlt &o)
{
    _result << "(WithAlt";
    GuideVisitor::visitWithAlt(o);
    _result << ")";
    return 0;
}

} // namespace

std::string objectGetKey(Object *obj)
{
    if (obj == nullptr)
        return "";

    ObjectKeyVisitor okv;
    obj->acceptVisitor(okv);

    return okv.getResult();
}

} // namespace hif
