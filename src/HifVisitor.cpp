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

/// @brief Instantiation of visitList for a list of generic Object.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Object>(BList<Object> &l);

/**
 * @brief Instantiation of visitList for a list of TypedObject.
 * @tparam T The type of the objects in the list.
 * @return The result of the visit operation.
 * @param l The list to visit.
 */
template int HifVisitor::visitList<TypedObject>(BList<TypedObject> &l);

/// @brief Instantiation of visitList for a list of Action elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Action>(BList<Action> &l);

/// @brief Instantiation of visitList for a list of AggregateAlt elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<AggregateAlt>(BList<AggregateAlt> &l);

/// @brief Instantiation of visitList for a list of ReferencedAssign elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<ReferencedAssign>(BList<ReferencedAssign> &l);

/// @brief Instantiation of visitList for a list of IfAlt elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<IfAlt>(BList<IfAlt> &l);

/// @brief Instantiation of visitList for a list of DesignUnit elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<DesignUnit>(BList<DesignUnit> &l);

/// @brief Instantiation of visitList for a list of DataDeclaration elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<DataDeclaration>(BList<DataDeclaration> &l);

/// @brief Instantiation of visitList for a list of Declaration elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Declaration>(BList<Declaration> &l);

/// @brief Instantiation of visitList for a list of EnumValue elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<EnumValue>(BList<EnumValue> &l);

/// @brief Instantiation of visitList for a list of Field elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Field>(BList<Field> &l);

/// @brief Instantiation of visitList for a list of Generate elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Generate>(BList<Generate> &l);

/// @brief Instantiation of visitList for a list of Instance elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Instance>(BList<Instance> &l);

/// @brief Instantiation of visitList for a list of Library elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Library>(BList<Library> &l);

/// @brief Instantiation of visitList for a list of LibraryDef elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<LibraryDef>(BList<LibraryDef> &l);

/// @brief Instantiation of visitList for a list of Identifier elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Identifier>(BList<Identifier> &l);

/// @brief Instantiation of visitList for a list of Parameter elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Parameter>(BList<Parameter> &l);

/// @brief Instantiation of visitList for a list of ParameterAssign elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<ParameterAssign>(BList<ParameterAssign> &l);

/// @brief Instantiation of visitList for a list of PortAssign elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<PortAssign>(BList<PortAssign> &l);

/// @brief Instantiation of visitList for a list of Port elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Port>(BList<Port> &l);

/// @brief Instantiation of visitList for a list of PPAssign elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<PPAssign>(BList<PPAssign> &l);

/// @brief Instantiation of visitList for a list of Range elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Range>(BList<Range> &l);

/// @brief Instantiation of visitList for a list of RecordValueAlt elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<RecordValueAlt>(BList<RecordValueAlt> &l);

/// @brief Instantiation of visitList for a list of State elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<State>(BList<State> &l);

/// @brief Instantiation of visitList for a list of StateTable elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<StateTable>(BList<StateTable> &l);

/// @brief Instantiation of visitList for a list of SwitchAlt elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<SwitchAlt>(BList<SwitchAlt> &l);

/// @brief Instantiation of visitList for a list of TPAssign elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<TPAssign>(BList<TPAssign> &l);

/// @brief Instantiation of visitList for a list of Value elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Value>(BList<Value> &l);

/// @brief Instantiation of visitList for a list of View elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<View>(BList<View> &l);

/// @brief Instantiation of visitList for a list of ViewReference elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<ViewReference>(BList<ViewReference> &l);

/// @brief Instantiation of visitList for a list of WhenAlt elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<WhenAlt>(BList<WhenAlt> &l);

/// @brief Instantiation of visitList for a list of WithAlt elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<WithAlt>(BList<WithAlt> &l);

/// @brief Instantiation of visitList for a list of Transition elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Transition>(BList<Transition> &l);

/// @brief Instantiation of visitList for a list of Aggregate elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Aggregate>(BList<Aggregate> &l);

/// @brief Instantiation of visitList for a list of Expression elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Expression>(BList<Expression> &l);

/// @brief Instantiation of visitList for a list of Assign elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Assign>(BList<Assign> &l);

/// @brief Instantiation of visitList for a list of FunctionCall elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<FunctionCall>(BList<FunctionCall> &l);

/// @brief Instantiation of visitList for a list of ProcedureCall elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<ProcedureCall>(BList<ProcedureCall> &l);

/// @brief Instantiation of visitList for a list of Break elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Break>(BList<Break> &l);

/// @brief Instantiation of visitList for a list of Continue elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Continue>(BList<Continue> &l);

/// @brief Instantiation of visitList for a list of Wait elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<Wait>(BList<Wait> &l);

/// @brief Instantiation of visitList for a list of SubProgram elements.
/// @param l The list to visit.
/// @return The result of the visit operation.
template int HifVisitor::visitList<SubProgram>(BList<SubProgram> &l);

} // namespace hif
