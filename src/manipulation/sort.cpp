/// @file sort.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/sort.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/hif_utils/operatorUtils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"
namespace hif
{
namespace manipulation
{

namespace
{

/// @brief Sort children "] = s of an object with respect to total ordering
/// defined by hif::compare.
/// TODO At the moment, only Values are supported.
class SortVisitor : public GuideVisitor
{
public:
    SortVisitor(hif::semantics::ILanguageSemantics *sem, const SortOptions &opt);
    ~SortVisitor();

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
    virtual int visitNull(Null &o);
    virtual int visitMember(Member &o);
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

    bool _result;

private:
    // Disabled.
    SortVisitor(const SortVisitor &);
    const SortVisitor &operator=(const SortVisitor &);

    hif::semantics::ILanguageSemantics *_sem;

    SortOptions _opt;
};
SortVisitor::SortVisitor(hif::semantics::ILanguageSemantics *sem, const SortOptions &opt)
    : GuideVisitor()
    , _result(false)
    , _sem(sem)
    , _opt(opt)
{
    // ntd
}

SortVisitor::~SortVisitor()
{
    // ntd
}

int SortVisitor::visitAggregate(Aggregate &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitAggregate(o);
    _result |= o.alts.sort(hif::compare);
    return 0;
}

int SortVisitor::visitAggregateAlt(AggregateAlt &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitAggregateAlt(o);
    _result |= o.indices.sort(hif::compare);
    return 0;
}

int SortVisitor::visitAlias(Alias &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitAlias(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitArray(Array &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitArray(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitAssign(Assign &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitAssign(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitSystem(System &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitSystem(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitBit(Bit &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitBit(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitBitValue(BitValue &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitBitValue(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitBitvector(Bitvector &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitBitvector(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitBitvectorValue(BitvectorValue &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitBitvectorValue(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitBool(Bool &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitBool(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitBoolValue(BoolValue &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitBoolValue(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitBreak(Break &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitBreak(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitCast(Cast &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitCast(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitChar(Char &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitChar(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitCharValue(CharValue &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitCharValue(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitConst(Const &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitConst(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitContents(Contents &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitContents(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitContinue(Continue &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitContinue(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitDesignUnit(DesignUnit &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitDesignUnit(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitEnum(Enum &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitEnum(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitEnumValue(EnumValue &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitEnumValue(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitExpression(Expression &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitExpression(o);

    if (o.getValue2() == nullptr)
        return 0;

    if (!hif::operatorIsCommutative(o.getOperator()))
        return 0;

    // If ascending, nothing to do. Otherwise, swap them.
    int ret = hif::compare(o.getValue1(), o.getValue2());
    if (ret <= 0)
        return 0;

    Value *tmp = o.getValue1();
    o.setValue1(o.getValue2());
    o.setValue2(tmp);

    _result = true;

    return 0;
}

int SortVisitor::visitFunctionCall(FunctionCall &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitFunctionCall(o);

    FunctionCall::DeclarationType *decl = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(decl != nullptr, "Cannot find declaration of fcall", &o, _sem);

    if (_opt.sortParameters) {
        _result |= hif::manipulation::sortParameters(
            o.parameterAssigns, decl->parameters, true, SortMissingKind::NOTHING, _sem, false);
        _result |= hif::manipulation::sortParameters(
            o.templateParameterAssigns, decl->templateParameters, true, SortMissingKind::NOTHING, _sem, false);
    }

    return 0;
}

int SortVisitor::visitField(Field &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitField(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitFieldReference(FieldReference &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitFieldReference(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitFile(File &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitFile(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitFor(For &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitFor(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitForGenerate(ForGenerate &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitForGenerate(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitFunction(Function &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitFunction(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitGlobalAction(GlobalAction &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitGlobalAction(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitEntity(Entity &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitEntity(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitIdentifier(Identifier &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitIdentifier(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitIf(If &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitIf(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitIfAlt(IfAlt &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitIfAlt(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitIfGenerate(IfGenerate &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitIfGenerate(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitInstance(Instance &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitInstance(o);

    Instance::DeclarationType *decl = hif::semantics::getDeclaration(&o, _sem);

    if (decl == nullptr) {
        messageAssert(dynamic_cast<Library *>(o.getReferencedType()) != nullptr, "Declaration not found", &o, _sem);
        return 0;
    }

    if (_opt.sortParameters) {
        _result |= hif::manipulation::sortParameters(o.portAssigns, decl->ports, true, SortMissingKind::NOTHING, _sem);
    }

    return 0;
}

int SortVisitor::visitInt(Int &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitInt(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitIntValue(IntValue &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitIntValue(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitLibraryDef(LibraryDef &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitLibraryDef(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitLibrary(Library &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitLibrary(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitNull(Null &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitNull(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitMember(Member &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitMember(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitTransition(Transition &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitTransition(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitParameterAssign(ParameterAssign &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitParameterAssign(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitParameter(Parameter &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitParameter(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitProcedureCall(ProcedureCall &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitProcedureCall(o);

    ProcedureCall::DeclarationType *decl = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(decl != nullptr, "Cannot find declaration of pcall", &o, _sem);

    if (_opt.sortParameters) {
        _result |= hif::manipulation::sortParameters(
            o.parameterAssigns, decl->parameters, true, SortMissingKind::NOTHING, _sem, false);
        _result |= hif::manipulation::sortParameters(
            o.templateParameterAssigns, decl->templateParameters, true, SortMissingKind::NOTHING, _sem, false);
    }

    return 0;
}

int SortVisitor::visitPointer(Pointer &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitPointer(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitPortAssign(PortAssign &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitPortAssign(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitPort(Port &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitPort(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitProcedure(Procedure &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitProcedure(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitRange(Range &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitRange(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitReal(Real &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitReal(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitRealValue(RealValue &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitRealValue(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitRecord(Record &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitRecord(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitRecordValue(RecordValue &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitRecordValue(o);
    return 0;
}

int SortVisitor::visitRecordValueAlt(RecordValueAlt &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitRecordValueAlt(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitReference(Reference &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitReference(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitReturn(Return &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitReturn(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitSignal(Signal &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitSignal(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitSigned(Signed &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitSigned(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitSlice(Slice &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitSlice(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitState(State &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitState(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitString(String &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitString(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitStateTable(StateTable &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitStateTable(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitSwitchAlt(SwitchAlt &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitSwitchAlt(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitSwitch(Switch &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitSwitch(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitStringValue(StringValue &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitStringValue(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitTime(Time &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitTime(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitTimeValue(TimeValue &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitTimeValue(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitTypeDef(TypeDef &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitTypeDef(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitTypeReference(TypeReference &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitTypeReference(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitTypeTPAssign(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitTypeTP(TypeTP &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitTypeTP(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitUnsigned(Unsigned &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitUnsigned(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitValueTPAssign(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitValueTP(ValueTP &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitValueTP(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitVariable(Variable &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitVariable(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitView(View &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitView(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitViewReference(ViewReference &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitViewReference(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitWait(Wait &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitWait(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitWhen(When &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitWhen(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitWhenAlt(WhenAlt &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitWhenAlt(o);
    // Nothing to do.
    return 0;
}

int SortVisitor::visitWhile(While &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitWhile(o);
    messageError("Unexpected case", &o, nullptr);
}

int SortVisitor::visitWith(With &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitWith(o);
    _result |= o.alts.sort(hif::compare);
    return 0;
}

int SortVisitor::visitWithAlt(WithAlt &o)
{
    if (_opt.sortChildren)
        GuideVisitor::visitWithAlt(o);
    _result |= o.conditions.sort(hif::compare);
    return 0;
}
} // namespace

SortOptions::SortOptions()
    : sortChildren(true)
    , sortParameters(false)
{
    // ntd
}

SortOptions::~SortOptions()
{
    // ntd
}
bool sort(Object *obj, hif::semantics::ILanguageSemantics *sem, const SortOptions &opt)
{
    messageAssert(obj != nullptr, "Unexpected null Object", obj, sem);
    messageAssert(dynamic_cast<Value *>(obj) != nullptr, "Unexpected non-value Object", obj, sem);

    SortVisitor vis(sem, opt);
    obj->acceptVisitor(vis);

    return vis._result;
}
} // namespace manipulation
} // namespace hif
