/// @file MapVisitor.i.hpp
/// @brief Inline implementations for MapVisitor template utilities.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/HifVisitor.hpp"
#include "hif/MapVisitor.hpp"
#include "hif/application_utils/portability.hpp"
#include "hif/classes/classes.hpp"

namespace hif
{

// ////////////////////////////////////////////////////////////////////////////////////////
// Support.
// ////////////////////////////////////////////////////////////////////////////////////////

namespace __conversion
{
template <class CHILD, class PARENT>
Conversion_t<CHILD, PARENT>::Conversion_t()
{
}

template <class CHILD, class PARENT>
Conversion_t<CHILD, PARENT>::~Conversion_t()
{
}

} // namespace __conversion
template <class FinalRebind>
MapVisitor1<FinalRebind>::MapVisitor1(Object *o1)
    : HifVisitor()
{
    o1->acceptVisitor(*this);
}

template <class FinalRebind>
MapVisitor1<FinalRebind>::~MapVisitor1()
{
}
///@name Visitor interface methods.
/// They just recall the general InternalVisitObject() method.
//@{
template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitAggregateAlt(AggregateAlt &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitAggregate(Aggregate &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitAlias(Alias &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitArray(Array &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitAssign(Assign &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitBit(Bit &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitBitValue(BitValue &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitBitvector(Bitvector &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitBitvectorValue(BitvectorValue &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitBool(Bool &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitBoolValue(BoolValue &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitIfAlt(IfAlt &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitIf(If &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitCast(Cast &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitChar(Char &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitCharValue(CharValue &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitConst(Const &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitContents(Contents &o)
{
    FinalRebind::map(&o);
    return 0;
}
template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitDesignUnit(DesignUnit &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitEnum(Enum &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitEnumValue(EnumValue &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitBreak(Break &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitEvent(Event &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitExpression(Expression &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitFunctionCall(FunctionCall &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitField(Field &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitFieldReference(FieldReference &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitFile(File &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitForGenerate(ForGenerate &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitFor(For &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitFunction(Function &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitGlobalAction(GlobalAction &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitEntity(Entity &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitIfGenerate(IfGenerate &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitInstance(Instance &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitInt(Int &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitIntValue(IntValue &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitLibraryDef(LibraryDef &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitLibrary(Library &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitMember(Member &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitIdentifier(Identifier &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitContinue(Continue &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitNull(Null &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitTransition(Transition &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitParameterAssign(ParameterAssign &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitParameter(Parameter &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitProcedureCall(ProcedureCall &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitPointer(Pointer &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitPortAssign(PortAssign &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitPort(Port &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitProcedure(Procedure &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitRange(Range &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitReal(Real &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitRealValue(RealValue &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitRecord(Record &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitRecordValue(RecordValue &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitRecordValueAlt(RecordValueAlt &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitReference(Reference &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitReturn(Return &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitSignal(Signal &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitSigned(Signed &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitSlice(Slice &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitState(State &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitStateTable(StateTable &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitString(String &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitSwitchAlt(SwitchAlt &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitSwitch(Switch &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitSystem(System &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitStringValue(StringValue &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitTime(Time &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitTimeValue(TimeValue &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitTypeDef(TypeDef &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitTypeReference(TypeReference &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitTypeTPAssign(TypeTPAssign &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitTypeTP(TypeTP &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitUnsigned(Unsigned &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitValueStatement(ValueStatement &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitValueTPAssign(ValueTPAssign &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitValueTP(ValueTP &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitVariable(Variable &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitView(View &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitViewReference(ViewReference &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitWait(Wait &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitWhenAlt(WhenAlt &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitWhen(When &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitWhile(While &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitWithAlt(WithAlt &o)
{
    FinalRebind::map(&o);
    return 0;
}

template <class FinalRebind>
int MapVisitor1<FinalRebind>::visitWith(With &o)
{
    FinalRebind::map(&o);
    return 0;
}

// ////////////////////////////////////////////////////////////////////////////////////////
// Maps two objects.
// ////////////////////////////////////////////////////////////////////////////////////////

namespace __conversion
{

/// @brief Template structure for internal object rebinding.
/// @details Handles runtime-to-compile-time polymorphism based on subclass status.
template <const bool isSub, class rebind_t>
struct internal_rebind_t {
    /// @brief Constructor for the general case.
    /// @param o1 First object.
    /// @param o2 Second object.
    internal_rebind_t(Object *o1, Object *o2);
};

/// @brief General case constructor implementation.
/// @param o1 First object.
/// @param o2 Second object.
template <const bool isSub, class rebind_t>
internal_rebind_t<isSub, rebind_t>::internal_rebind_t(Object *o1, Object *o2)
{
    rebind_t::FR_t::map(o1, o2); // Call the static map() function for object mapping.
}

/// @brief Specialization for the case when `isSub` is true.
template <class rebind_t>
struct internal_rebind_t<true, rebind_t> {
    /// @brief Constructor for the subclass case.
    /// @param o1 First object.
    /// @param o2 Object of a derived type.
    template <class T>
    internal_rebind_t(Object *o1, T *);
};

/// @brief Implementation of the constructor for the subclass case.
/// @param o1 First object.
/// @param o2 Object of a derived type.
template <class rebind_t>
template <class T>
internal_rebind_t<true, rebind_t>::internal_rebind_t(Object *o1, T *)
{
    MapVisitor1<rebind_t> v1(o1); // Use MapVisitor1 to handle the mapping.
}

} // namespace __conversion

template <class FinalRebind, class Parent2>
template <class T2>
template <typename T1>
void MapVisitor2<FinalRebind, Parent2>::rebind_t<T2>::map(T1 *o1)
{
    FinalRebind::map(o1, static_cast<T2 *>(MapVisitor2<FinalRebind, Parent2>::o2));
}

template <class FinalRebind, class Parent2>
MapVisitor2<FinalRebind, Parent2>::MapVisitor2(Object *oo1, Object *oo2)
    : HifVisitor()
    , o1(oo1)
{
    o2 = oo2;
    o2->acceptVisitor(*this);
}

template <class FinalRebind, class Parent2>
MapVisitor2<FinalRebind, Parent2>::~MapVisitor2()
{
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitAggregateAlt(AggregateAlt &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, AggregateAlt>::result, rebind_t<AggregateAlt>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitAggregate(Aggregate &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Aggregate>::result, rebind_t<Aggregate>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitAlias(Alias &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Alias>::result, rebind_t<Alias>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitArray(Array &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Array>::result, rebind_t<Array>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitAssign(Assign &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Assign>::result, rebind_t<Assign>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitBit(Bit &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Bit>::result, rebind_t<Bit>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitBitValue(BitValue &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, BitValue>::result, rebind_t<BitValue>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitBitvector(Bitvector &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Bitvector>::result, rebind_t<Bitvector>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitBitvectorValue(BitvectorValue &o)
{
    __conversion::internal_rebind_t<
        __conversion::checkSubclass<Parent2, BitvectorValue>::result, rebind_t<BitvectorValue>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitBool(Bool &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Bool>::result, rebind_t<Bool>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitBoolValue(BoolValue &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, BoolValue>::result, rebind_t<BoolValue>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitIfAlt(IfAlt &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, IfAlt>::result, rebind_t<IfAlt>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitIf(If &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, If>::result, rebind_t<If>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitCast(Cast &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Cast>::result, rebind_t<Cast>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitChar(Char &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Char>::result, rebind_t<Char>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitCharValue(CharValue &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, CharValue>::result, rebind_t<CharValue>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitConst(Const &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Const>::result, rebind_t<Const>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitContents(Contents &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Contents>::result, rebind_t<Contents>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitDesignUnit(DesignUnit &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, DesignUnit>::result, rebind_t<DesignUnit>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitEnum(Enum &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Enum>::result, rebind_t<Enum>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitEnumValue(EnumValue &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, EnumValue>::result, rebind_t<EnumValue>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitBreak(Break &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Break>::result, rebind_t<Break>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitEvent(Event &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Event>::result, rebind_t<Event>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitExpression(Expression &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Expression>::result, rebind_t<Expression>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitFunctionCall(FunctionCall &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, FunctionCall>::result, rebind_t<FunctionCall>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitField(Field &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Field>::result, rebind_t<Field>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitFieldReference(FieldReference &o)
{
    __conversion::internal_rebind_t<
        __conversion::checkSubclass<Parent2, FieldReference>::result, rebind_t<FieldReference>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitFile(File &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, File>::result, rebind_t<File>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitForGenerate(ForGenerate &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, ForGenerate>::result, rebind_t<ForGenerate>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitFor(For &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, For>::result, rebind_t<For>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitFunction(Function &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Function>::result, rebind_t<Function>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitGlobalAction(GlobalAction &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, GlobalAction>::result, rebind_t<GlobalAction>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitEntity(Entity &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Entity>::result, rebind_t<Entity>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitIfGenerate(IfGenerate &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, IfGenerate>::result, rebind_t<IfGenerate>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitInstance(Instance &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Instance>::result, rebind_t<Instance>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitInt(Int &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Int>::result, rebind_t<Int>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitIntValue(IntValue &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, IntValue>::result, rebind_t<IntValue>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitLibraryDef(LibraryDef &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, LibraryDef>::result, rebind_t<LibraryDef>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitLibrary(Library &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Library>::result, rebind_t<Library>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitMember(Member &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Member>::result, rebind_t<Member>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitIdentifier(Identifier &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Identifier>::result, rebind_t<Identifier>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitContinue(Continue &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Continue>::result, rebind_t<Continue>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitNull(Null &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Null>::result, rebind_t<Null>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitTransition(Transition &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Transition>::result, rebind_t<Transition>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitParameterAssign(ParameterAssign &o)
{
    __conversion::internal_rebind_t<
        __conversion::checkSubclass<Parent2, ParameterAssign>::result, rebind_t<ParameterAssign>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitParameter(Parameter &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Parameter>::result, rebind_t<Parameter>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitProcedureCall(ProcedureCall &o)
{
    __conversion::internal_rebind_t<
        __conversion::checkSubclass<Parent2, ProcedureCall>::result, rebind_t<ProcedureCall>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitPointer(Pointer &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Pointer>::result, rebind_t<Pointer>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitPortAssign(PortAssign &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, PortAssign>::result, rebind_t<PortAssign>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitPort(Port &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Port>::result, rebind_t<Port>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitProcedure(Procedure &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Procedure>::result, rebind_t<Procedure>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitRange(Range &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Range>::result, rebind_t<Range>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitReal(Real &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Real>::result, rebind_t<Real>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitRealValue(RealValue &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, RealValue>::result, rebind_t<RealValue>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitRecord(Record &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Record>::result, rebind_t<Record>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitRecordValue(RecordValue &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, RecordValue>::result, rebind_t<RecordValue>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitRecordValueAlt(RecordValueAlt &o)
{
    __conversion::internal_rebind_t<
        __conversion::checkSubclass<Parent2, RecordValueAlt>::result, rebind_t<RecordValueAlt>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitReference(Reference &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Reference>::result, rebind_t<Reference>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitReturn(Return &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Return>::result, rebind_t<Return>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitSignal(Signal &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Signal>::result, rebind_t<Signal>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitSigned(Signed &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Signed>::result, rebind_t<Signed>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitSlice(Slice &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Slice>::result, rebind_t<Slice>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitState(State &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, State>::result, rebind_t<State>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitStateTable(StateTable &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, StateTable>::result, rebind_t<StateTable>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitString(String &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, String>::result, rebind_t<String>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitSwitchAlt(SwitchAlt &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, SwitchAlt>::result, rebind_t<SwitchAlt>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitSwitch(Switch &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Switch>::result, rebind_t<Switch>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitSystem(System &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, System>::result, rebind_t<System>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitStringValue(StringValue &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, StringValue>::result, rebind_t<StringValue>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitTime(Time &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Time>::result, rebind_t<Time>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitTimeValue(TimeValue &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, TimeValue>::result, rebind_t<TimeValue>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitTypeDef(TypeDef &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, TypeDef>::result, rebind_t<TypeDef>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitTypeReference(TypeReference &o)
{
    __conversion::internal_rebind_t<
        __conversion::checkSubclass<Parent2, TypeReference>::result, rebind_t<TypeReference>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitTypeTPAssign(TypeTPAssign &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, TypeTPAssign>::result, rebind_t<TypeTPAssign>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitTypeTP(TypeTP &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, TypeTP>::result, rebind_t<TypeTP>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitUnsigned(Unsigned &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Unsigned>::result, rebind_t<Unsigned>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitValueStatement(ValueStatement &o)
{
    __conversion::internal_rebind_t<
        __conversion::checkSubclass<Parent2, ValueTPAssign>::result, rebind_t<ValueStatement>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitValueTPAssign(ValueTPAssign &o)
{
    __conversion::internal_rebind_t<
        __conversion::checkSubclass<Parent2, ValueTPAssign>::result, rebind_t<ValueTPAssign>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitValueTP(ValueTP &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, ValueTP>::result, rebind_t<ValueTP>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitVariable(Variable &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Variable>::result, rebind_t<Variable>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitView(View &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, View>::result, rebind_t<View>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitViewReference(ViewReference &o)
{
    __conversion::internal_rebind_t<
        __conversion::checkSubclass<Parent2, ViewReference>::result, rebind_t<ViewReference>>
        ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitWait(Wait &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, Wait>::result, rebind_t<Wait>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitWhenAlt(WhenAlt &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, WhenAlt>::result, rebind_t<WhenAlt>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitWhen(When &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, When>::result, rebind_t<When>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitWhile(While &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, While>::result, rebind_t<While>> ir(o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitWithAlt(WithAlt &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, WithAlt>::result, rebind_t<WithAlt>> ir(
        o1, &o);
    return 0;
}

template <class FinalRebind, class Parent2>
int MapVisitor2<FinalRebind, Parent2>::visitWith(With &o)
{
    __conversion::internal_rebind_t<__conversion::checkSubclass<Parent2, With>::result, rebind_t<With>> ir(o1, &o);
    return 0;
}
/// @name Helpers.
/// @brief Simplifies binding to design patterns using partial template specializations.
/// @details These helpers provide utilities to work with templates requiring classes, methods, or functors:
/// @{

/// @brief Helper for mapping using a single template class.
template <template <class Ta> class MapTo_t>
struct class_map_helper1_t {
    /// @brief Maps a single object using the template class.
    /// @param o1 Pointer to the object to map.
    template <typename T1>
    static void map(T1 *o1)
    {
        MapTo_t<T1>::map(o1); // Calls the map method of the template class.
    }
};

/// @brief Helper for mapping using a template class with two types.
template <template <class Ta, class Tb> class MapTo_t>
struct class_map_helper2_t {
    /// @brief Maps two objects using the template class.
    /// @param o1 Pointer to the first object.
    /// @param o2 Pointer to the second object.
    template <typename T1, typename T2>
    static void map(T1 *o1, T2 *o2)
    {
        MapTo_t<T1, T2>::map(o1, o2); // Calls the map method of the template class.
    }
};

/// @brief Helper for mapping using a function pointer (single object).
template <class Ta, void (*map_to_t)(Ta *)>
struct method_map_helper1_t {
    /// @brief Maps a single object using the provided function.
    /// @param o1 Pointer to the object to map.
    template <typename T1>
    static void map(T1 *o1)
    {
        map_to_t(o1); // Calls the function pointer.
    }
};

/// @brief Helper for mapping using a function pointer (two objects).
template <class Ta, class Tb, void (*map_to_t)(Ta *, Tb *)>
struct method_map_helper2_t {
    /// @brief Maps two objects using the provided function.
    /// @param o1 Pointer to the first object.
    /// @param o2 Pointer to the second object.
    template <typename T1, typename T2>
    static void map(T1 *o1, T2 *o2)
    {
        map_to_t(o1, o2); // Calls the function pointer.
    }
};

/// @brief Helper for mapping using a functor (single object).
template <class map_to_t>
struct functor_map_helper1_t {
    /// @brief Maps a single object using the provided functor.
    /// @param o1 Pointer to the object to map.
    template <typename T1>
    static void map(T1 *o1)
    {
        (*mapper)(o1); // Invokes the functor.
    }

    static map_to_t *mapper; ///< Pointer to the functor.
};

/// @brief Initializes the static functor pointer for single-object mapping.
template <class map_to_t>
map_to_t *functor_map_helper1_t<map_to_t>::mapper = nullptr;

/// @brief Helper for mapping using a functor (two objects).
template <class map_to_t>
struct functor_map_helper2_t {
    /// @brief Maps two objects using the provided functor.
    /// @param o1 Pointer to the first object.
    /// @param o2 Pointer to the second object.
    template <typename T1, typename T2>
    static void map(T1 *o1, T2 *o2)
    {
        (*mapper)(o1, o2); // Invokes the functor.
    }

    static map_to_t *mapper; ///< Pointer to the functor.
};

/// @brief Initializes the static functor pointer for two-object mapping.
template <class map_to_t>
map_to_t *functor_map_helper2_t<map_to_t>::mapper = nullptr;

/// @}

} // namespace hif
