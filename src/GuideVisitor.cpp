/// @file GuideVisitor.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif.hpp"

namespace hif
{

GuideVisitor::GuideVisitor(int rv)
    : _rv(rv)
{
    // ntd
}

GuideVisitor::~GuideVisitor()
{
    // ntd
}

int GuideVisitor::visitAggregate(Aggregate &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.alts);
    if (o.getOthers())
        _rv |= o.getOthers()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}
int GuideVisitor::visitAggregateAlt(AggregateAlt &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.indices);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}
int GuideVisitor::visitAlias(Alias &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getRange())
        _rv |= o.getRange()->acceptVisitor(*this);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitArray(Array &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getSpan())
        _rv |= o.getSpan()->acceptVisitor(*this);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitAssign(Assign &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getLeftHandSide())
        _rv |= o.getLeftHandSide()->acceptVisitor(*this);
    if (o.getRightHandSide())
        _rv |= o.getRightHandSide()->acceptVisitor(*this);
    if (o.getDelay())
        _rv |= o.getDelay()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitSystem(System &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.libraryDefs);
    _rv |= visitList(o.designUnits);
    _rv |= visitList(o.declarations);
    _rv |= visitList(o.libraries);
    _rv |= visitList(o.actions);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitBit(Bit &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitBitValue(BitValue &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}
int GuideVisitor::visitBitvector(Bitvector &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getSpan())
        _rv |= o.getSpan()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitBitvectorValue(BitvectorValue &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}
int GuideVisitor::visitBool(Bool &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitBoolValue(BoolValue &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitIfAlt(IfAlt &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getCondition())
        _rv |= o.getCondition()->acceptVisitor(*this);
    _rv |= visitList(o.actions);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitIf(If &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.alts);
    _rv |= visitList(o.defaults);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitCast(Cast &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitChar(Char &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitCharValue(CharValue &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}
int GuideVisitor::visitConst(Const &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getRange())
        _rv |= o.getRange()->acceptVisitor(*this);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}
int GuideVisitor::visitContents(Contents &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.libraries);
    _rv |= visitList(o.declarations);
    _rv |= visitList(o.stateTables);
    _rv |= visitList(o.generates);
    _rv |= visitList(o.instances);
    if (o.getGlobalAction())
        _rv |= o.getGlobalAction()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitDesignUnit(DesignUnit &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.views);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitEnum(Enum &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.values);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitEnumValue(EnumValue &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getRange())
        _rv |= o.getRange()->acceptVisitor(*this);

    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitBreak(Break &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitEvent(Event &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitExpression(Expression &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getValue1())
        _rv |= o.getValue1()->acceptVisitor(*this);
    if (o.getValue2())
        _rv |= o.getValue2()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitFunctionCall(FunctionCall &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getInstance() != nullptr)
        _rv |= o.getInstance()->acceptVisitor(*this);
    _rv |= visitList(o.templateParameterAssigns);
    _rv |= visitList(o.parameterAssigns);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitField(Field &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getRange())
        _rv |= o.getRange()->acceptVisitor(*this);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitFieldReference(FieldReference &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getPrefix())
        _rv |= o.getPrefix()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitFile(File &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitFor(For &o)
{
    if (BeforeVisit(o))
        return _rv;

    _rv |= visitList(o.initDeclarations);
    _rv |= visitList(o.initValues);
    _rv |= visitList(o.stepActions);
    if (o.getCondition() != nullptr)
        _rv |= o.getCondition()->acceptVisitor(*this);
    _rv |= visitList(o.forActions);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitForGenerate(ForGenerate &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.declarations);
    _rv |= visitList(o.stateTables);
    _rv |= visitList(o.generates);
    _rv |= visitList(o.instances);
    if (o.getGlobalAction())
        _rv |= o.getGlobalAction()->acceptVisitor(*this);
    _rv |= visitList(o.initDeclarations);
    _rv |= visitList(o.initValues);
    _rv |= visitList(o.stepActions);
    if (o.getCondition() != nullptr)
        _rv |= o.getCondition()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitFunction(Function &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.templateParameters);
    _rv |= visitList(o.parameters);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    if (o.getStateTable())
        _rv |= o.getStateTable()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitGlobalAction(GlobalAction &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.actions);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitEntity(Entity &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.parameters);
    _rv |= visitList(o.ports);
    _rv |= AfterVisit(o);
    return _rv;
}

int GuideVisitor::visitIfGenerate(IfGenerate &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getCondition())
        _rv |= o.getCondition()->acceptVisitor(*this);
    _rv |= visitList(o.declarations);
    _rv |= visitList(o.stateTables);
    _rv |= visitList(o.generates);
    _rv |= visitList(o.instances);
    if (o.getGlobalAction())
        _rv |= o.getGlobalAction()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}
int GuideVisitor::visitInt(Int &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getSpan())
        _rv |= o.getSpan()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}
int GuideVisitor::visitIntValue(IntValue &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitInstance(Instance &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getReferencedType())
        _rv |= o.getReferencedType()->acceptVisitor(*this);
    _rv |= visitList(o.portAssigns);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitLibraryDef(LibraryDef &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.libraries);
    _rv |= visitList(o.declarations);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitLibrary(Library &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getInstance())
        _rv |= o.getInstance()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitMember(Member &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getPrefix() != nullptr)
        _rv |= o.getPrefix()->acceptVisitor(*this);
    if (o.getIndex() != nullptr)
        _rv |= o.getIndex()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitIdentifier(Identifier &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitContinue(Continue &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitNull(Null &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitTransition(Transition &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.enablingLabelList);
    _rv |= visitList(o.enablingList);
    _rv |= visitList(o.updateLabelList);
    _rv |= visitList(o.updateList);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitParameterAssign(ParameterAssign &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitParameter(Parameter &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getRange())
        _rv |= o.getRange()->acceptVisitor(*this);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitProcedureCall(ProcedureCall &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getInstance() != nullptr)
        _rv |= o.getInstance()->acceptVisitor(*this);
    _rv |= visitList(o.templateParameterAssigns);
    _rv |= visitList(o.parameterAssigns);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitPointer(Pointer &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitReference(Reference &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitPortAssign(PortAssign &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    if (o.getPartialBind())
        _rv |= o.getPartialBind()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitPort(Port &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getRange())
        _rv |= o.getRange()->acceptVisitor(*this);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitProcedure(Procedure &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.templateParameters);
    _rv |= visitList(o.parameters);
    if (o.getStateTable())
        _rv |= o.getStateTable()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}
int GuideVisitor::visitRange(Range &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getLeftBound())
        _rv |= o.getLeftBound()->acceptVisitor(*this);
    if (o.getRightBound())
        _rv |= o.getRightBound()->acceptVisitor(*this);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitReal(Real &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getSpan())
        _rv |= o.getSpan()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitRealValue(RealValue &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitRecord(Record &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.fields);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitRecordValue(RecordValue &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.alts);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitRecordValueAlt(RecordValueAlt &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitReturn(Return &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitSignal(Signal &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getRange())
        _rv |= o.getRange()->acceptVisitor(*this);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitSigned(Signed &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getSpan())
        _rv |= o.getSpan()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitSlice(Slice &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getPrefix())
        _rv |= o.getPrefix()->acceptVisitor(*this);
    if (o.getSpan())
        _rv |= o.getSpan()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitState(State &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.actions);
    _rv |= visitList(o.invariants);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitString(String &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getSpanInformation())
        _rv |= o.getSpanInformation()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitStateTable(StateTable &o)
{
    if (BeforeVisit(o))
        return _rv;
    //	 if (o.GetEntryState())
    //	 	_rv |= o.GetEntryState()->acceptVisitor(* this);
    _rv |= visitList(o.declarations);
    _rv |= visitList(o.sensitivity);
    _rv |= visitList(o.sensitivityPos);
    _rv |= visitList(o.sensitivityNeg);
    _rv |= visitList(o.states);
    _rv |= visitList(o.edges);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitSwitchAlt(SwitchAlt &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.conditions);
    _rv |= visitList(o.actions);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitSwitch(Switch &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getCondition())
        _rv |= o.getCondition()->acceptVisitor(*this);
    _rv |= visitList(o.alts);
    _rv |= visitList(o.defaults);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitStringValue(StringValue &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitTime(Time &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitTimeValue(TimeValue &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitTypeDef(TypeDef &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.templateParameters);
    if (o.getRange())
        _rv |= o.getRange()->acceptVisitor(*this);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitTypeReference(TypeReference &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.templateParameterAssigns);
    _rv |= visitList(o.ranges);
    if (o.getInstance())
        _rv |= o.getInstance()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitTypeTP(TypeTP &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitUnsigned(Unsigned &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getSpan())
        _rv |= o.getSpan()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitValueStatement(ValueStatement &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitValueTP(ValueTP &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getRange())
        _rv |= o.getRange()->acceptVisitor(*this);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitVariable(Variable &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getRange())
        _rv |= o.getRange()->acceptVisitor(*this);
    if (o.getType())
        _rv |= o.getType()->acceptVisitor(*this);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitView(View &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.templateParameters);
    _rv |= visitList(o.libraries);
    if (o.getEntity())
        _rv |= o.getEntity()->acceptVisitor(*this);
    _rv |= visitList(o.declarations);
    _rv |= visitList(o.inheritances);
    if (o.getContents())
        _rv |= o.getContents()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitViewReference(ViewReference &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.templateParameterAssigns);
    if (o.getInstance())
        _rv |= o.getInstance()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitWait(Wait &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.sensitivity);
    _rv |= visitList(o.sensitivityPos);
    _rv |= visitList(o.sensitivityNeg);
    _rv |= visitList(o.actions);
    if (o.getTime())
        _rv |= o.getTime()->acceptVisitor(*this);
    if (o.getCondition())
        _rv |= o.getCondition()->acceptVisitor(*this);
    if (o.getRepetitions())
        _rv |= o.getRepetitions()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitWhen(When &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.alts);
    if (o.getDefault())
        _rv |= o.getDefault()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitWhenAlt(WhenAlt &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getCondition())
        _rv |= o.getCondition()->acceptVisitor(*this);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitWhile(While &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getCondition())
        _rv |= o.getCondition()->acceptVisitor(*this);
    _rv |= visitList(o.actions);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitWith(With &o)
{
    if (BeforeVisit(o))
        return _rv;
    if (o.getCondition())
        _rv |= o.getCondition()->acceptVisitor(*this);
    _rv |= visitList(o.alts);
    if (o.getDefault())
        _rv |= o.getDefault()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

int GuideVisitor::visitWithAlt(WithAlt &o)
{
    if (BeforeVisit(o))
        return _rv;
    _rv |= visitList(o.conditions);
    if (o.getValue())
        _rv |= o.getValue()->acceptVisitor(*this);
    _rv |= AfterVisit(o);

    return _rv;
}

} // namespace hif
