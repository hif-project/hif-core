/// @file AncestorVisitor.hpp
/// @brief Visitor for traversing ancestor relationships in HIF object trees.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/HifVisitor.hpp"
#include "hif/features/features.hpp"

namespace hif
{

/// @brief This visitor Automatically visit ancestors of visited objects.
/// In case of multiple inheritance when object inherit from some features,
/// Visitor call visit before on features and after on hif::Object ancestors.
class AncestorVisitor : public HifVisitor
{
public:
    /// @brief Constructor.
    /// @param visitFeatures If <tt>true</tt> call visit of the features.
    AncestorVisitor(const bool visitFeatures);

    /// @brief Virtual Destructor
    virtual ~AncestorVisitor();

    /// @name Objects visits
    /// @{

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
    virtual int visitIfAlt(IfAlt &o);
    virtual int visitIf(If &o);
    virtual int visitCast(Cast &o);
    virtual int visitChar(Char &o);
    virtual int visitCharValue(CharValue &o);
    virtual int visitConst(Const &o);
    virtual int visitContents(Contents &o);
    virtual int visitDesignUnit(DesignUnit &o);
    virtual int visitEnum(Enum &o);
    virtual int visitEnumValue(EnumValue &o);
    virtual int visitBreak(Break &o);
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
    virtual int visitIfGenerate(IfGenerate &o);
    virtual int visitInt(Int &o);
    virtual int visitIntValue(IntValue &o);
    virtual int visitInstance(Instance &o);
    virtual int visitLibraryDef(LibraryDef &o);
    virtual int visitLibrary(Library &o);
    virtual int visitMember(Member &o);
    virtual int visitIdentifier(Identifier &o);
    virtual int visitContinue(Continue &o);
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

    /// @}

    /// @name Ancestors visits
    /// @{

    /// @brief Visit an Action object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitAction(Action &o);

    /// @brief Visit an Alt object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitAlt(Alt &o);

    /// @brief Visit an Assign object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitBaseContents(BaseContents &o);

    /// @brief Visit an BaseContents object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitCompositeType(CompositeType &o);

    /// @brief Visit an CompositeType object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitConstValue(ConstValue &o);

    /// @brief Visit an DataDeclaration object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitDataDeclaration(DataDeclaration &o);

    /// @brief Visit an Declaration object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitDeclaration(Declaration &o);

    /// @brief Visit an Generate object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitGenerate(Generate &o);

    /// @brief Visit an Object object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitObject(Object &o);

    /// @brief Visit an PPAssign object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitPPAssign(PPAssign &o);

    /// @brief Visit an PrefixedReference object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitPrefixedReference(PrefixedReference &o);

    /// @brief Visit an ReferencedAssign object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitReferencedAssign(ReferencedAssign &o);

    /// @brief Visit an ReferencedType object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitReferencedType(ReferencedType &o);

    /// @brief Visit an Scope object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitScope(Scope &o);

    /// @brief Visit an ScopedType object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitScopedType(ScopedType &o);

    /// @brief Visit an Object object.
    /// @param o The SimpleType to visit.
    /// @return The result of the visit.
    virtual int visitSimpleType(SimpleType &o);

    /// @brief Visit an SubProgram object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitSubProgram(SubProgram &o);

    /// @brief Visit an TPAssign object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitTPAssign(TPAssign &o);

    /// @brief Visit an Type object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitType(Type &o);

    /// @brief Visit an TypeDeclaration object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitTypeDeclaration(TypeDeclaration &o);

    /// @brief Visit an TypedObject object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitTypedObject(TypedObject &o);

    /// @brief Visit an Value object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitValue(Value &o);

    /// @}

    /// @name Features visits
    /// @{

    /// @brief Visit an INamedObject object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitNamedObject(hif::features::INamedObject &o);

    /// @brief Visit an ISymbol object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitSymbol(hif::features::ISymbol &o);

    /// @brief Visit an ITypeSpan object.
    /// @param o The object to visit.
    /// @return The result of the visit.
    virtual int visitTypeSpan(hif::features::ITypeSpan &o);

    /// @}

private:
    /// @brief If <tt>true</tt> call visit of the features.
    bool _visitFeatures;
};

} // namespace hif
