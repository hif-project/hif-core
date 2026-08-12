/// @file AncestorVisitor.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/AncestorVisitor.hpp"

#include "hif/hif.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

AncestorVisitor::AncestorVisitor(bool visitFeatures)
    : HifVisitor()
    , _visitFeatures(visitFeatures)
{
    // ntd
}

AncestorVisitor::~AncestorVisitor()
{
    // ntd
}

int AncestorVisitor::visitAggregate(Aggregate &o) { return visitValue(o); }

int AncestorVisitor::visitAggregateAlt(AggregateAlt &o) { return visitAlt(o); }

int AncestorVisitor::visitAlias(Alias &o) { return visitDataDeclaration(o); }

int AncestorVisitor::visitArray(Array &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitTypeSpan(o);
    rv |= visitCompositeType(o);
    return rv;
}

int AncestorVisitor::visitAssign(Assign &o) { return visitAction(o); }

int AncestorVisitor::visitSystem(System &o) { return visitScope(o); }

int AncestorVisitor::visitBit(Bit &o) { return visitSimpleType(o); }

int AncestorVisitor::visitBitValue(BitValue &o) { return visitConstValue(o); }

int AncestorVisitor::visitBitvector(Bitvector &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitTypeSpan(o);
    rv |= visitSimpleType(o);
    return rv;
}

int AncestorVisitor::visitBitvectorValue(BitvectorValue &o) { return visitConstValue(o); }

int AncestorVisitor::visitBool(Bool &o) { return visitSimpleType(o); }

int AncestorVisitor::visitBoolValue(BoolValue &o) { return visitConstValue(o); }

int AncestorVisitor::visitIf(If &o) { return visitAction(o); }

int AncestorVisitor::visitIfAlt(IfAlt &o) { return visitAlt(o); }

int AncestorVisitor::visitCast(Cast &o) { return visitValue(o); }

int AncestorVisitor::visitChar(Char &o) { return visitSimpleType(o); }

int AncestorVisitor::visitCharValue(CharValue &o) { return visitConstValue(o); }

int AncestorVisitor::visitConst(Const &o) { return visitDataDeclaration(o); }

int AncestorVisitor::visitContents(Contents &o) { return visitBaseContents(o); }

int AncestorVisitor::visitDesignUnit(DesignUnit &o) { return visitScope(o); }

int AncestorVisitor::visitEnum(Enum &o) { return visitScopedType(o); }

int AncestorVisitor::visitEnumValue(EnumValue &o) { return visitDataDeclaration(o); }

int AncestorVisitor::visitEvent(Event &o) { return visitSimpleType(o); }

int AncestorVisitor::visitExpression(Expression &o) { return visitValue(o); }

int AncestorVisitor::visitBreak(Break &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitNamedObject(o);
    rv |= visitAction(o);
    return rv;
}

int AncestorVisitor::visitField(Field &o) { return visitDataDeclaration(o); }

int AncestorVisitor::visitFieldReference(FieldReference &o)
{
    int rv = 0;
    if (_visitFeatures) {
        rv |= visitNamedObject(o);
        rv |= visitSymbol(o);
    }
    rv |= visitPrefixedReference(o);
    return rv;
}

int AncestorVisitor::visitFile(File &o) { return visitCompositeType(o); }

int AncestorVisitor::visitFunctionCall(FunctionCall &o)
{
    int rv = 0;
    if (_visitFeatures) {
        rv |= visitNamedObject(o);
        rv |= visitSymbol(o);
    }
    rv |= visitValue(o);
    return rv;
}

int AncestorVisitor::visitFor(For &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitNamedObject(o);
    rv |= visitAction(o);
    return rv;
}

int AncestorVisitor::visitForGenerate(ForGenerate &o) { return visitGenerate(o); }

int AncestorVisitor::visitFunction(Function &o) { return visitSubProgram(o); }

int AncestorVisitor::visitGlobalAction(GlobalAction &o) { return visitObject(o); }

int AncestorVisitor::visitIfGenerate(IfGenerate &o) { return visitGenerate(o); }

int AncestorVisitor::visitEntity(Entity &o) { return visitScope(o); }

int AncestorVisitor::visitInstance(Instance &o)
{
    int rv = 0;
    if (_visitFeatures) {
        rv |= visitSymbol(o);
        rv |= visitNamedObject(o);
    }
    rv |= visitValue(o);
    return rv;
}

int AncestorVisitor::visitInt(Int &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitTypeSpan(o);
    rv |= visitSimpleType(o);
    return rv;
}

int AncestorVisitor::visitIntValue(IntValue &o) { return visitConstValue(o); }

int AncestorVisitor::visitLibrary(Library &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitSymbol(o);
    rv |= visitReferencedType(o);
    return rv;
}

int AncestorVisitor::visitLibraryDef(LibraryDef &o) { return visitScope(o); }

int AncestorVisitor::visitMember(Member &o) { return visitPrefixedReference(o); }

int AncestorVisitor::visitIdentifier(Identifier &o)
{
    int rv = 0;
    if (_visitFeatures) {
        rv |= visitNamedObject(o);
        rv |= visitSymbol(o);
    }
    rv |= visitValue(o);
    return rv;
}

int AncestorVisitor::visitContinue(Continue &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitNamedObject(o);
    rv |= visitAction(o);
    return rv;
}

int AncestorVisitor::visitNull(Null &o) { return visitAction(o); }

int AncestorVisitor::visitTransition(Transition &o) { return visitAction(o); }

int AncestorVisitor::visitParameter(Parameter &o) { return visitDataDeclaration(o); }

int AncestorVisitor::visitParameterAssign(ParameterAssign &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitSymbol(o);
    rv |= visitPPAssign(o);
    return rv;
}

int AncestorVisitor::visitProcedureCall(ProcedureCall &o)
{
    int rv = 0;
    if (_visitFeatures) {
        rv |= visitSymbol(o);
        rv |= visitNamedObject(o);
    }
    rv |= visitAction(o);
    return rv;
}

int AncestorVisitor::visitPointer(Pointer &o) { return visitCompositeType(o); }

int AncestorVisitor::visitPort(Port &o) { return visitDataDeclaration(o); }

int AncestorVisitor::visitPortAssign(PortAssign &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitSymbol(o);
    rv |= visitPPAssign(o);
    return rv;
}

int AncestorVisitor::visitProcedure(Procedure &o) { return visitSubProgram(o); }

int AncestorVisitor::visitRange(Range &o) { return visitValue(o); }

int AncestorVisitor::visitReal(Real &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitTypeSpan(o);
    rv |= visitSimpleType(o);
    return rv;
}

int AncestorVisitor::visitRealValue(RealValue &o) { return visitConstValue(o); }

int AncestorVisitor::visitRecord(Record &o) { return visitScopedType(o); }

int AncestorVisitor::visitRecordValue(RecordValue &o) { return visitValue(o); }

int AncestorVisitor::visitRecordValueAlt(RecordValueAlt &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitNamedObject(o);
    rv |= visitAlt(o);
    return rv;
}

int AncestorVisitor::visitReference(Reference &o) { return visitCompositeType(o); }

int AncestorVisitor::visitReturn(Return &o) { return visitAction(o); }

int AncestorVisitor::visitSignal(Signal &o) { return visitDataDeclaration(o); }

int AncestorVisitor::visitSigned(Signed &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitTypeSpan(o);
    rv |= visitSimpleType(o);
    return rv;
}

int AncestorVisitor::visitSlice(Slice &o) { return visitPrefixedReference(o); }

int AncestorVisitor::visitStateTable(StateTable &o) { return visitScope(o); }

int AncestorVisitor::visitState(State &o) { return visitDeclaration(o); }

int AncestorVisitor::visitString(String &o) { return visitSimpleType(o); }

int AncestorVisitor::visitSwitch(Switch &o) { return visitAction(o); }

int AncestorVisitor::visitSwitchAlt(SwitchAlt &o) { return visitAlt(o); }

int AncestorVisitor::visitStringValue(StringValue &o) { return visitConstValue(o); }

int AncestorVisitor::visitTime(Time &o) { return visitSimpleType(o); }

int AncestorVisitor::visitTimeValue(TimeValue &o) { return visitConstValue(o); }

int AncestorVisitor::visitTypeDef(TypeDef &o) { return visitTypeDeclaration(o); }

int AncestorVisitor::visitTypeReference(TypeReference &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitSymbol(o);
    rv |= visitReferencedType(o);
    return rv;
}

int AncestorVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitSymbol(o);
    rv |= visitTPAssign(o);
    return rv;
}

int AncestorVisitor::visitTypeTP(TypeTP &o) { return visitTypeDeclaration(o); }

int AncestorVisitor::visitUnsigned(Unsigned &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitTypeSpan(o);
    rv |= visitSimpleType(o);
    return rv;
}

int AncestorVisitor::visitValueStatement(ValueStatement &o) { return visitAction(o); }

int AncestorVisitor::visitVariable(Variable &o) { return visitDataDeclaration(o); }

int AncestorVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitSymbol(o);
    rv |= visitTPAssign(o);
    return rv;
}

int AncestorVisitor::visitValueTP(ValueTP &o) { return visitDataDeclaration(o); }

int AncestorVisitor::visitView(View &o) { return visitScope(o); }

int AncestorVisitor::visitViewReference(ViewReference &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitSymbol(o);
    rv |= visitReferencedType(o);
    return rv;
}

int AncestorVisitor::visitWait(Wait &o) { return visitAction(o); }

int AncestorVisitor::visitWhen(When &o) { return visitValue(o); }

int AncestorVisitor::visitWhenAlt(WhenAlt &o) { return visitAlt(o); }

int AncestorVisitor::visitWhile(While &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitNamedObject(o);
    rv |= visitAction(o);
    return rv;
}

int AncestorVisitor::visitWith(With &o) { return visitValue(o); }

int AncestorVisitor::visitWithAlt(WithAlt &o) { return visitAlt(o); }

// ///////////////////////////////////////////////////////////////////
// Ancestors visits
// ///////////////////////////////////////////////////////////////////

int AncestorVisitor::visitAction(Action &o) { return visitObject(o); }

int AncestorVisitor::visitAlt(Alt &o) { return visitObject(o); }

int AncestorVisitor::visitBaseContents(BaseContents &o) { return visitScope(o); }

int AncestorVisitor::visitCompositeType(CompositeType &o) { return visitType(o); }

int AncestorVisitor::visitConstValue(ConstValue &o) { return visitValue(o); }

int AncestorVisitor::visitDataDeclaration(DataDeclaration &o) { return visitDeclaration(o); }

int AncestorVisitor::visitDeclaration(Declaration &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitNamedObject(o);
    rv |= visitObject(o);
    return rv;
}

int AncestorVisitor::visitGenerate(Generate &o) { return visitBaseContents(o); }

int AncestorVisitor::visitObject(Object & /*o*/) { return 0; }

int AncestorVisitor::visitPPAssign(PPAssign &o) { return visitReferencedAssign(o); }

int AncestorVisitor::visitPrefixedReference(PrefixedReference &o) { return visitValue(o); }

int AncestorVisitor::visitReferencedAssign(ReferencedAssign &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitNamedObject(o);
    rv |= visitTypedObject(o);
    return rv;
}

int AncestorVisitor::visitReferencedType(ReferencedType &o)
{
    int rv = 0;
    if (_visitFeatures)
        rv |= visitNamedObject(o);
    rv |= visitType(o);
    return rv;
}

int AncestorVisitor::visitScope(Scope &o) { return visitDeclaration(o); }

int AncestorVisitor::visitScopedType(ScopedType &o) { return visitType(o); }

int AncestorVisitor::visitSimpleType(SimpleType &o) { return visitType(o); }

int AncestorVisitor::visitSubProgram(SubProgram &o) { return visitScope(o); }

int AncestorVisitor::visitTPAssign(TPAssign &o) { return visitReferencedAssign(o); }

int AncestorVisitor::visitType(Type &o) { return visitObject(o); }

int AncestorVisitor::visitTypeDeclaration(TypeDeclaration &o) { return visitScope(o); }

int AncestorVisitor::visitTypedObject(TypedObject &o) { return visitObject(o); }

int AncestorVisitor::visitValue(Value &o) { return visitTypedObject(o); }

// ///////////////////////////////////////////////////////////////////
// Features visits
// ///////////////////////////////////////////////////////////////////

int AncestorVisitor::visitNamedObject(hif::features::INamedObject & /*o*/) { return 0; }

int AncestorVisitor::visitSymbol(hif::features::ISymbol & /*o*/) { return 0; }

int AncestorVisitor::visitTypeSpan(hif::features::ITypeSpan & /*o*/) { return 0; }

} // namespace hif
