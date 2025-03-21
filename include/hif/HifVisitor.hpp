/// @file HifVisitor.hpp
/// @brief Defines the HifVisitor class for traversing and processing HIF object trees.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @class HifVisitor
/// @brief Provides a base implementation for visiting HIF objects.
/// @details The HifVisitor class defines a visiting interface for traversing and
/// processing objects in a HIF tree. Custom visitors can inherit from this class
/// and override specific visit methods to implement custom behavior.
class HifVisitor
{
public:
    /// @brief Constructor
    /// @details Initializes a new HifVisitor instance.
    HifVisitor();

    /// @brief Virtual Destructor
    /// @details Ensures proper cleanup for derived classes.
    virtual ~HifVisitor();

    /// @name Visit list of objects
    /// @{

    /// @brief Template method for visiting a list of objects.
    /// @tparam T The type of the objects in the list.
    /// @param l Reference to the list of objects to visit.
    /// @return The result of the visit operation.
    template <class T> int visitList(BList<T> &l);

    /// @brief Method for visiting a list of generic objects.
    /// @param l Reference to the list of objects to visit.
    /// @return The result of the visit operation.
    virtual int visitList(BList<Object> &l);

    /// @}

    /// @name Object Visits
    /// @{

    /// @brief Visit an Aggregate object.
    /// @param o Reference to the Aggregate object.
    /// @return Result of the visit operation.
    virtual int visitAggregate(Aggregate &o);

    /// @brief Visit an AggregateAlt object.
    /// @param o Reference to the AggregateAlt object.
    /// @return Result of the visit operation.
    virtual int visitAggregateAlt(AggregateAlt &o);

    /// @brief Visit an Alias object.
    /// @param o Reference to the Alias object.
    /// @return Result of the visit operation.
    virtual int visitAlias(Alias &o);

    /// @brief Visit an Array object.
    /// @param o Reference to the Array object.
    /// @return Result of the visit operation.
    virtual int visitArray(Array &o);

    /// @brief Visit an Assign object.
    /// @param o Reference to the Assign object.
    /// @return Result of the visit operation.
    virtual int visitAssign(Assign &o);

    /// @brief Visit a System object.
    /// @param o Reference to the System object.
    /// @return Result of the visit operation.
    virtual int visitSystem(System &o);

    /// @brief Visit a Bit object.
    /// @param o Reference to the Bit object.
    /// @return Result of the visit operation.
    virtual int visitBit(Bit &o);

    /// @brief Visit a BitValue object.
    /// @param o Reference to the BitValue object.
    /// @return Result of the visit operation.
    virtual int visitBitValue(BitValue &o);

    /// @brief Visit a Bitvector object.
    /// @param o Reference to the Bitvector object.
    /// @return Result of the visit operation.
    virtual int visitBitvector(Bitvector &o);

    /// @brief Visit a BitvectorValue object.
    /// @param o Reference to the BitvectorValue object.
    /// @return Result of the visit operation.
    virtual int visitBitvectorValue(BitvectorValue &o);

    /// @brief Visit a Bool object.
    /// @param o Reference to the Bool object.
    /// @return Result of the visit operation.
    virtual int visitBool(Bool &o);

    /// @brief Visit a BoolValue object.
    /// @param o Reference to the BoolValue object.
    /// @return Result of the visit operation.
    virtual int visitBoolValue(BoolValue &o);

    /// @brief Visit an IfAlt object.
    /// @param o Reference to the IfAlt object.
    /// @return Result of the visit operation.
    virtual int visitIfAlt(IfAlt &o);

    /// @brief Visit an If object.
    /// @param o Reference to the If object.
    /// @return Result of the visit operation.
    virtual int visitIf(If &o);

    /// @brief Visit a Cast object.
    /// @param o Reference to the Cast object.
    /// @return Result of the visit operation.
    virtual int visitCast(Cast &o);

    /// @brief Visit a Char object.
    /// @param o Reference to the Char object.
    /// @return Result of the visit operation.
    virtual int visitChar(Char &o);

    /// @brief Visit a CharValue object.
    /// @param o Reference to the CharValue object.
    /// @return Result of the visit operation.
    virtual int visitCharValue(CharValue &o);

    /// @brief Visit a Const object.
    /// @param o Reference to the Const object.
    /// @return Result of the visit operation.
    virtual int visitConst(Const &o);

    /// @brief Visit a Contents object.
    /// @param o Reference to the Contents object.
    /// @return Result of the visit operation.
    virtual int visitContents(Contents &o);

    /// @brief Visit a DesignUnit object.
    /// @param o Reference to the DesignUnit object.
    /// @return Result of the visit operation.
    virtual int visitDesignUnit(DesignUnit &o);

    /// @brief Visit an Enum object.
    /// @param o Reference to the Enum object.
    /// @return Result of the visit operation.
    virtual int visitEnum(Enum &o);

    /// @brief Visit an EnumValue object.
    /// @param o Reference to the EnumValue object.
    /// @return Result of the visit operation.
    virtual int visitEnumValue(EnumValue &o);

    /// @brief Visit a Break object.
    /// @param o Reference to the Break object.
    /// @return Result of the visit operation.
    virtual int visitBreak(Break &o);

    /// @brief Visit an Event object.
    /// @param o Reference to the Event object.
    /// @return Result of the visit operation.
    virtual int visitEvent(Event &o);

    /// @brief Visit an Expression object.
    /// @param o Reference to the Expression object.
    /// @return Result of the visit operation.
    virtual int visitExpression(Expression &o);

    /// @brief Visit a FunctionCall object.
    /// @param o Reference to the FunctionCall object.
    /// @return Result of the visit operation.
    virtual int visitFunctionCall(FunctionCall &o);

    /// @brief Visit a Field object.
    /// @param o Reference to the Field object.
    /// @return Result of the visit operation.
    virtual int visitField(Field &o);

    /// @brief Visit a FieldReference object.
    /// @param o Reference to the FieldReference object.
    /// @return Result of the visit operation.
    virtual int visitFieldReference(FieldReference &o);

    /// @brief Visit a File object.
    /// @param o Reference to the File object.
    /// @return Result of the visit operation.
    virtual int visitFile(File &o);

    /// @brief Visit a For object.
    /// @param o Reference to the For object.
    /// @return Result of the visit operation.
    virtual int visitFor(For &o);

    /// @brief Visit a ForGenerate object.
    /// @param o Reference to the ForGenerate object.
    /// @return Result of the visit operation.
    virtual int visitForGenerate(ForGenerate &o);

    /// @brief Visit a Function object.
    /// @param o Reference to the Function object.
    /// @return Result of the visit operation.
    virtual int visitFunction(Function &o);

    /// @brief Visit a GlobalAction object.
    /// @param o Reference to the GlobalAction object.
    /// @return Result of the visit operation.
    virtual int visitGlobalAction(GlobalAction &o);

    /// @brief Visit an Entity object.
    /// @param o Reference to the Entity object.
    /// @return Result of the visit operation.
    virtual int visitEntity(Entity &o);

    /// @brief Visit an IfGenerate object.
    /// @param o Reference to the IfGenerate object.
    /// @return Result of the visit operation.
    virtual int visitIfGenerate(IfGenerate &o);

    /// @brief Visit an Int object.
    /// @param o Reference to the Int object.
    /// @return Result of the visit operation.
    virtual int visitInt(Int &o);

    /// @brief Visit an IntValue object.
    /// @param o Reference to the IntValue object.
    /// @return Result of the visit operation.
    virtual int visitIntValue(IntValue &o);

    /// @brief Visit an Instance object.
    /// @param o Reference to the Instance object.
    /// @return Result of the visit operation.
    virtual int visitInstance(Instance &o);

    /// @brief Visit a LibraryDef object.
    /// @param o Reference to the LibraryDef object.
    /// @return Result of the visit operation.
    virtual int visitLibraryDef(LibraryDef &o);

    /// @brief Visit a Library object.
    /// @param o Reference to the Library object.
    /// @return Result of the visit operation.
    virtual int visitLibrary(Library &o);

    /// @brief Visit a Member object.
    /// @param o Reference to the Member object.
    /// @return Result of the visit operation.
    virtual int visitMember(Member &o);

    /// @brief Visit an Identifier object.
    /// @param o Reference to the Identifier object.
    /// @return Result of the visit operation.
    virtual int visitIdentifier(Identifier &o);

    /// @brief Visit a Continue object.
    /// @param o Reference to the Continue object.
    /// @return Result of the visit operation.
    virtual int visitContinue(Continue &o);

    /// @brief Visit a Null object.
    /// @param o Reference to the Null object.
    /// @return Result of the visit operation.
    virtual int visitNull(Null &o);

    /// @brief Visit a Transition object.
    /// @param o Reference to the Transition object.
    /// @return Result of the visit operation.
    virtual int visitTransition(Transition &o);

    /// @brief Visit a ParameterAssign object.
    /// @param o Reference to the ParameterAssign object.
    /// @return Result of the visit operation.
    virtual int visitParameterAssign(ParameterAssign &o);

    /// @brief Visit a Parameter object.
    /// @param o Reference to the Parameter object.
    /// @return Result of the visit operation.
    virtual int visitParameter(Parameter &o);

    /// @brief Visit a ProcedureCall object.
    /// @param o Reference to the ProcedureCall object.
    /// @return Result of the visit operation.
    virtual int visitProcedureCall(ProcedureCall &o);

    /// @brief Visit a Pointer object.
    /// @param o Reference to the Pointer object.
    /// @return Result of the visit operation.
    virtual int visitPointer(Pointer &o);

    /// @brief Visit a PortAssign object.
    /// @param o Reference to the PortAssign object.
    /// @return Result of the visit operation.
    virtual int visitPortAssign(PortAssign &o);

    /// @brief Visit a Port object.
    /// @param o Reference to the Port object.
    /// @return Result of the visit operation.
    virtual int visitPort(Port &o);

    /// @brief Visit a Procedure object.
    /// @param o Reference to the Procedure object.
    /// @return Result of the visit operation.
    virtual int visitProcedure(Procedure &o);

    /// @brief Visit a Range object.
    /// @param o Reference to the Range object.
    /// @return Result of the visit operation.
    virtual int visitRange(Range &o);

    /// @brief Visit a Real object.
    /// @param o Reference to the Real object.
    /// @return Result of the visit operation.
    virtual int visitReal(Real &o);

    /// @brief Visit a RealValue object.
    /// @param o Reference to the RealValue object.
    /// @return Result of the visit operation.
    virtual int visitRealValue(RealValue &o);

    /// @brief Visit a Record object.
    /// @param o Reference to the Record object.
    /// @return Result of the visit operation.
    virtual int visitRecord(Record &o);

    /// @brief Visit a RecordValue object.
    /// @param o Reference to the RecordValue object.
    /// @return Result of the visit operation.
    virtual int visitRecordValue(RecordValue &o);

    /// @brief Visit a RecordValueAlt object.
    /// @param o Reference to the RecordValueAlt object.
    /// @return Result of the visit operation.
    virtual int visitRecordValueAlt(RecordValueAlt &o);

    /// @brief Visit a Reference object.
    /// @param o Reference to the Reference object.
    /// @return Result of the visit operation.
    virtual int visitReference(Reference &o);

    /// @brief Visit a Return object.
    /// @param o Reference to the Return object.
    /// @return Result of the visit operation.
    virtual int visitReturn(Return &o);

    /// @brief Visit a Signal object.
    /// @param o Reference to the Signal object.
    /// @return Result of the visit operation.
    virtual int visitSignal(Signal &o);

    /// @brief Visit a Signed object.
    /// @param o Reference to the Signed object.
    /// @return Result of the visit operation.
    virtual int visitSigned(Signed &o);

    /// @brief Visit a Slice object.
    /// @param o Reference to the Slice object.
    /// @return Result of the visit operation.
    virtual int visitSlice(Slice &o);

    /// @brief Visit a State object.
    /// @param o Reference to the State object.
    /// @return Result of the visit operation.
    virtual int visitState(State &o);

    /// @brief Visit a String object.
    /// @param o Reference to the String object.
    /// @return Result of the visit operation.
    virtual int visitString(String &o);

    /// @brief Visit a StateTable object.
    /// @param o Reference to the StateTable object.
    /// @return Result of the visit operation.
    virtual int visitStateTable(StateTable &o);

    /// @brief Visit a SwitchAlt object.
    /// @param o Reference to the SwitchAlt object.
    /// @return Result of the visit operation.
    virtual int visitSwitchAlt(SwitchAlt &o);

    /// @brief Visit a Switch object.
    /// @param o Reference to the Switch object.
    /// @return Result of the visit operation.
    virtual int visitSwitch(Switch &o);

    /// @brief Visit a StringValue object.
    /// @param o Reference to the StringValue object.
    /// @return Result of the visit operation.
    virtual int visitStringValue(StringValue &o);

    /// @brief Visit a Time object.
    /// @param o Reference to the Time object.
    /// @return Result of the visit operation.
    virtual int visitTime(Time &o);

    /// @brief Visit a TimeValue object.
    /// @param o Reference to the TimeValue object.
    /// @return Result of the visit operation.
    virtual int visitTimeValue(TimeValue &o);

    /// @brief Visit a TypeDef object.
    /// @param o Reference to the TypeDef object.
    /// @return Result of the visit operation.
    virtual int visitTypeDef(TypeDef &o);

    /// @brief Visit a TypeReference object.
    /// @param o Reference to the TypeReference object.
    /// @return Result of the visit operation.
    virtual int visitTypeReference(TypeReference &o);

    /// @brief Visit a TypeTPAssign object.
    /// @param o Reference to the TypeTPAssign object.
    /// @return Result of the visit operation.
    virtual int visitTypeTPAssign(TypeTPAssign &o);

    /// @brief Visit a TypeTP object.
    /// @param o Reference to the TypeTP object.
    /// @return Result of the visit operation.
    virtual int visitTypeTP(TypeTP &o);

    /// @brief Visit an Unsigned object.
    /// @param o Reference to the Unsigned object.
    /// @return Result of the visit operation.
    virtual int visitUnsigned(Unsigned &o);

    /// @brief Visit a ValueStatement object.
    /// @param o Reference to the ValueStatement object.
    /// @return Result of the visit operation.
    virtual int visitValueStatement(ValueStatement &o);

    /// @brief Visit a ValueTPAssign object.
    /// @param o Reference to the ValueTPAssign object.
    /// @return Result of the visit operation.
    virtual int visitValueTPAssign(ValueTPAssign &o);

    /// @brief Visit a ValueTP object.
    /// @param o Reference to the ValueTP object.
    /// @return Result of the visit operation.
    virtual int visitValueTP(ValueTP &o);

    /// @brief Visit a Variable object.
    /// @param o Reference to the Variable object.
    /// @return Result of the visit operation.
    virtual int visitVariable(Variable &o);

    /// @brief Visit a View object.
    /// @param o Reference to the View object.
    /// @return Result of the visit operation.
    virtual int visitView(View &o);

    /// @brief Visit a ViewReference object.
    /// @param o Reference to the ViewReference object.
    /// @return Result of the visit operation.
    virtual int visitViewReference(ViewReference &o);

    /// @brief Visit a Wait object.
    /// @param o Reference to the Wait object.
    /// @return Result of the visit operation.
    virtual int visitWait(Wait &o);

    /// @brief Visit a When object.
    /// @param o Reference to the When object.
    /// @return Result of the visit operation.
    virtual int visitWhen(When &o);

    /// @brief Visit a WhenAlt object.
    /// @param o Reference to the WhenAlt object.
    /// @return Result of the visit operation.
    virtual int visitWhenAlt(WhenAlt &o);

    /// @brief Visit a While object.
    /// @param o Reference to the While object.
    /// @return Result of the visit operation.
    virtual int visitWhile(While &o);

    /// @brief Visit a With object.
    /// @param o Reference to the With object.
    /// @return Result of the visit operation.
    virtual int visitWith(With &o);

    /// @brief Visit a WithAlt object.
    /// @param o Reference to the WithAlt object.
    /// @return Result of the visit operation.
    virtual int visitWithAlt(WithAlt &o);

    /// @}

protected:
    /// @brief Actions performed before visiting an object.
    /// @details Override this method to define behavior before visiting an object.
    /// @param o Reference to the object to visit.
    /// @return `true` if subtree visits should stop, `false` otherwise.
    virtual bool BeforeVisit(Object &o);

    /// @brief Actions performed after visiting an object.
    /// @details Override this method to define behavior after visiting an object.
    /// @param o Reference to the visited object.
    /// @return Integer representing the result of the visit.
    virtual int AfterVisit(Object &o);
};

} // namespace hif
