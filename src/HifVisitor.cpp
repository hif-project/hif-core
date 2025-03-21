/// @file HifVisitor.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif.hpp"

namespace hif
{

HifVisitor::HifVisitor() {}

HifVisitor::~HifVisitor() {}

// /////////////////////////////////////////////////////////////////////////////////////////////

int HifVisitor::visitAggregate(Aggregate & /* o */) { return 0; }
int HifVisitor::visitAggregateAlt(AggregateAlt & /* o */) { return 0; }
int HifVisitor::visitAlias(Alias & /* o */) { return 0; }
int HifVisitor::visitArray(Array & /* o */) { return 0; }
int HifVisitor::visitAssign(Assign & /* o */) { return 0; }
int HifVisitor::visitSystem(System & /* o */) { return 0; }
int HifVisitor::visitBit(Bit & /* o */) { return 0; }
int HifVisitor::visitBitValue(BitValue & /* o */) { return 0; }
int HifVisitor::visitBitvector(Bitvector & /* o */) { return 0; }
int HifVisitor::visitBitvectorValue(BitvectorValue & /* o */) { return 0; }
int HifVisitor::visitBool(Bool & /* o */) { return 0; }
int HifVisitor::visitBoolValue(BoolValue & /* o */) { return 0; }
int HifVisitor::visitIfAlt(IfAlt & /* o */) { return 0; }
int HifVisitor::visitIf(If & /* o */) { return 0; }
int HifVisitor::visitCast(Cast & /* o */) { return 0; }
int HifVisitor::visitChar(Char & /* o */) { return 0; }
int HifVisitor::visitCharValue(CharValue & /* o */) { return 0; }
int HifVisitor::visitConst(Const & /* o */) { return 0; }
int HifVisitor::visitContents(Contents & /* o */) { return 0; }
int HifVisitor::visitDesignUnit(DesignUnit & /* o */) { return 0; }
int HifVisitor::visitEnum(Enum & /* o */) { return 0; }
int HifVisitor::visitEnumValue(EnumValue & /* o */) { return 0; }
int HifVisitor::visitBreak(Break & /* o */) { return 0; }
int HifVisitor::visitEvent(Event & /* o */) { return 0; }
int HifVisitor::visitExpression(Expression & /* o */) { return 0; }
int HifVisitor::visitFunctionCall(FunctionCall & /* o */) { return 0; }
int HifVisitor::visitField(Field & /* o */) { return 0; }
int HifVisitor::visitFieldReference(FieldReference & /* o */) { return 0; }
int HifVisitor::visitFile(File & /* o */) { return 0; }
int HifVisitor::visitFor(For & /* o */) { return 0; }
int HifVisitor::visitForGenerate(ForGenerate & /* o */) { return 0; }
int HifVisitor::visitFunction(Function & /* o */) { return 0; }
int HifVisitor::visitGlobalAction(GlobalAction & /* o */) { return 0; }
int HifVisitor::visitEntity(Entity & /* o */) { return 0; }
int HifVisitor::visitIfGenerate(IfGenerate & /* o */) { return 0; }
int HifVisitor::visitInt(Int & /* o */) { return 0; }
int HifVisitor::visitIntValue(IntValue & /* o */) { return 0; }
int HifVisitor::visitInstance(Instance & /* o */) { return 0; }
int HifVisitor::visitLibraryDef(LibraryDef & /* o */) { return 0; }
int HifVisitor::visitLibrary(Library & /* o */) { return 0; }
int HifVisitor::visitMember(Member & /* o */) { return 0; }
int HifVisitor::visitIdentifier(Identifier & /* o */) { return 0; }
int HifVisitor::visitContinue(Continue & /* o */) { return 0; }
int HifVisitor::visitNull(Null & /* o */) { return 0; }
int HifVisitor::visitTransition(Transition & /* o */) { return 0; }
int HifVisitor::visitParameterAssign(ParameterAssign & /* o */) { return 0; }
int HifVisitor::visitParameter(Parameter & /* o */) { return 0; }
int HifVisitor::visitProcedureCall(ProcedureCall & /* o */) { return 0; }
int HifVisitor::visitPointer(Pointer & /* o */) { return 0; }
int HifVisitor::visitPortAssign(PortAssign & /* o */) { return 0; }
int HifVisitor::visitPort(Port & /* o */) { return 0; }
int HifVisitor::visitProcedure(Procedure & /* o */) { return 0; }
int HifVisitor::visitRange(Range & /* o */) { return 0; }
int HifVisitor::visitReal(Real & /* o */) { return 0; }
int HifVisitor::visitRealValue(RealValue & /* o */) { return 0; }
int HifVisitor::visitRecord(Record & /* o */) { return 0; }
int HifVisitor::visitRecordValue(RecordValue & /* o */) { return 0; }
int HifVisitor::visitRecordValueAlt(RecordValueAlt & /* o */) { return 0; }
int HifVisitor::visitReference(Reference & /* o */) { return 0; }
int HifVisitor::visitReturn(Return & /* o */) { return 0; }
int HifVisitor::visitSignal(Signal & /* o */) { return 0; }
int HifVisitor::visitSigned(Signed & /* o */) { return 0; }
int HifVisitor::visitSlice(Slice & /* o */) { return 0; }
int HifVisitor::visitState(State & /* o */) { return 0; }
int HifVisitor::visitString(String & /* o */) { return 0; }
int HifVisitor::visitStateTable(StateTable & /* o */) { return 0; }
int HifVisitor::visitSwitchAlt(SwitchAlt & /* o */) { return 0; }
int HifVisitor::visitSwitch(Switch & /* o */) { return 0; }
int HifVisitor::visitStringValue(StringValue & /* o */) { return 0; }
int HifVisitor::visitTime(Time & /* o */) { return 0; }
int HifVisitor::visitTimeValue(TimeValue & /* o */) { return 0; }
int HifVisitor::visitTypeDef(TypeDef & /* o */) { return 0; }
int HifVisitor::visitTypeReference(TypeReference & /* o */) { return 0; }
int HifVisitor::visitTypeTPAssign(TypeTPAssign & /* o */) { return 0; }
int HifVisitor::visitTypeTP(TypeTP & /* o */) { return 0; }
int HifVisitor::visitUnsigned(Unsigned & /* o */) { return 0; }
int HifVisitor::visitValueStatement(ValueStatement & /* o */) { return 0; }
int HifVisitor::visitValueTPAssign(ValueTPAssign & /* o */) { return 0; }
int HifVisitor::visitValueTP(ValueTP & /* o */) { return 0; }
int HifVisitor::visitVariable(Variable & /* o */) { return 0; }
int HifVisitor::visitView(View & /* o */) { return 0; }
int HifVisitor::visitViewReference(ViewReference & /* o */) { return 0; }
int HifVisitor::visitWait(Wait & /* o */) { return 0; }
int HifVisitor::visitWhen(When & /* o */) { return 0; }
int HifVisitor::visitWhenAlt(WhenAlt & /* o */) { return 0; }
int HifVisitor::visitWhile(While & /* o */) { return 0; }
int HifVisitor::visitWith(With & /* o */) { return 0; }
int HifVisitor::visitWithAlt(WithAlt & /* o */) { return 0; }
// /////////////////////////////////////////////////////////////////////////////////////////////

template <class T> int HifVisitor::visitList(BList<T> &l) { return visitList(l.template toOtherBList<Object>()); }

int HifVisitor::visitList(BList<Object> &l)
{
    int ret = 0;
    BList<Object>::iterator i;
    for (i = l.begin(); i != l.end(); ++i)
        ret |= (*i)->acceptVisitor(*this);

    return ret;
}

bool HifVisitor::BeforeVisit(Object &)
{
    //Insert here actions that should be performed in each visitor,
    //before the visit start

    return false;
}

int HifVisitor::AfterVisit(Object &)
{
    //Insert here actions that should be performed in each visitor,
    //after the visit end

    return 0;
}

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of generic Object.
template int HifVisitor::visitList<Object>(BList<Object> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of TypedObject.
template int HifVisitor::visitList<TypedObject>(BList<TypedObject> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Action elements.
template int HifVisitor::visitList<Action>(BList<Action> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of AggregateAlt elements.
template int HifVisitor::visitList<AggregateAlt>(BList<AggregateAlt> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of ReferencedAssign elements.
template int HifVisitor::visitList<ReferencedAssign>(BList<ReferencedAssign> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of IfAlt elements.
template int HifVisitor::visitList<IfAlt>(BList<IfAlt> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of DesignUnit elements.
template int HifVisitor::visitList<DesignUnit>(BList<DesignUnit> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of DataDeclaration elements.
template int HifVisitor::visitList<DataDeclaration>(BList<DataDeclaration> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Declaration elements.
template int HifVisitor::visitList<Declaration>(BList<Declaration> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of EnumValue elements.
template int HifVisitor::visitList<EnumValue>(BList<EnumValue> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Field elements.
template int HifVisitor::visitList<Field>(BList<Field> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Generate elements.
template int HifVisitor::visitList<Generate>(BList<Generate> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Instance elements.
template int HifVisitor::visitList<Instance>(BList<Instance> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Library elements.
template int HifVisitor::visitList<Library>(BList<Library> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of LibraryDef elements.
template int HifVisitor::visitList<LibraryDef>(BList<LibraryDef> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Identifier elements.
template int HifVisitor::visitList<Identifier>(BList<Identifier> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Parameter elements.
template int HifVisitor::visitList<Parameter>(BList<Parameter> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of ParameterAssign elements.
template int HifVisitor::visitList<ParameterAssign>(BList<ParameterAssign> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of PortAssign elements.
template int HifVisitor::visitList<PortAssign>(BList<PortAssign> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Port elements.
template int HifVisitor::visitList<Port>(BList<Port> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of PPAssign elements.
template int HifVisitor::visitList<PPAssign>(BList<PPAssign> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Range elements.
template int HifVisitor::visitList<Range>(BList<Range> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of RecordValueAlt elements.
template int HifVisitor::visitList<RecordValueAlt>(BList<RecordValueAlt> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of State elements.
template int HifVisitor::visitList<State>(BList<State> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of StateTable elements.
template int HifVisitor::visitList<StateTable>(BList<StateTable> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of SwitchAlt elements.
template int HifVisitor::visitList<SwitchAlt>(BList<SwitchAlt> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of TPAssign elements.
template int HifVisitor::visitList<TPAssign>(BList<TPAssign> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Value elements.
template int HifVisitor::visitList<Value>(BList<Value> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of View elements.
template int HifVisitor::visitList<View>(BList<View> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of ViewReference elements.
template int HifVisitor::visitList<ViewReference>(BList<ViewReference> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of WhenAlt elements.
template int HifVisitor::visitList<WhenAlt>(BList<WhenAlt> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of WithAlt elements.
template int HifVisitor::visitList<WithAlt>(BList<WithAlt> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Transition elements.
template int HifVisitor::visitList<Transition>(BList<Transition> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Aggregate elements.
template int HifVisitor::visitList<Aggregate>(BList<Aggregate> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Expression elements.
template int HifVisitor::visitList<Expression>(BList<Expression> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Assign elements.
template int HifVisitor::visitList<Assign>(BList<Assign> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of FunctionCall elements.
template int HifVisitor::visitList<FunctionCall>(BList<FunctionCall> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of ProcedureCall elements.
template int HifVisitor::visitList<ProcedureCall>(BList<ProcedureCall> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Break elements.
template int HifVisitor::visitList<Break>(BList<Break> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Continue elements.
template int HifVisitor::visitList<Continue>(BList<Continue> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of Wait elements.
template int HifVisitor::visitList<Wait>(BList<Wait> &l);

/// @relates HifVisitor
/// @brief Instantiation of visitList for a list of SubProgram elements.
template int HifVisitor::visitList<SubProgram>(BList<SubProgram> &l);

} // namespace hif
