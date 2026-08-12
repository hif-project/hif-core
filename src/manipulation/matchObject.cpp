/// @file matchObject.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <iostream>

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/manipulation/matchObject.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace
{

Value *_getBound(Range *r, bool left, const RangeDirection dir)
{
    if (dir != r->getDirection()) {
        if (left) {
            return r->getRightBound();
        } else {
            return r->getLeftBound();
        }
    } else {
        if (left) {
            return r->getLeftBound();
        } else {
            return r->getRightBound();
        }
    }
}
/// @brief Add to five <tt>path</tt> list the series of parent object
/// from <tt>pattern</tt> object to <tt>referenceTree</tt> object.
/// @param pattern The object from which start to get parent.
/// @param referenceTree The reference tree root object.
void getPath(Object *pattern, Object *referenceTree, std::list<Object *> &path)
{
    while (pattern != nullptr && pattern != referenceTree->getParent()) {
        path.push_back(pattern);
        pattern = pattern->getParent();
    }

    // In case of error, returning an empty list:
    if (!path.empty() && path.back() != referenceTree)
        path.clear();
}
/// Replacing an object with another
class MatchingVisitor : public HifVisitor
{
public:
    MatchingVisitor(
        Object *matched,
        std::list<Object *> *path,
        hif::semantics::ILanguageSemantics *sem,
        const MatchObjectOptions &opt);
    virtual ~MatchingVisitor();

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
    virtual int visitIfAlt(IfAlt &o);
    virtual int visitIf(If &o);
    virtual int visitCast(Cast &o);
    virtual int visitChar(Char &o);
    virtual int visitCharValue(CharValue &o);
    virtual int visitConst(Const &o);
    virtual int visitContents(Contents &o);
    virtual int visitDataDeclaration(DataDeclaration &o);
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
    virtual int visitPPAssign(PPAssign &o);
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
    virtual int visitSubProgram(SubProgram &o);
    virtual int visitSwitchAlt(SwitchAlt &o);
    virtual int visitSwitch(Switch &o);
    virtual int visitSystem(System &o);
    virtual int visitStringValue(StringValue &o);
    virtual int visitTime(Time &o);
    virtual int visitTimeValue(TimeValue &o);
    virtual int visitTypeDef(TypeDef &o);
    virtual int visitTypeReference(TypeReference &o);
    virtual int visitTypeTP(TypeTP &o);
    virtual int visitTypeTPAssign(TypeTPAssign &o);
    virtual int visitUnsigned(Unsigned &o);
    virtual int visitVariable(Variable &o);
    virtual int visitValueStatement(ValueStatement &o);
    virtual int visitValueTP(ValueTP &o);
    virtual int visitValueTPAssign(ValueTPAssign &o);
    virtual int visitView(View &o);
    virtual int visitViewReference(ViewReference &o);
    virtual int visitWait(Wait &o);
    virtual int visitWhen(When &o);
    virtual int visitWhenAlt(WhenAlt &o);
    virtual int visitWhile(While &o);
    virtual int visitWith(With &o);
    virtual int visitWithAlt(WithAlt &o);

    Object *getMatched();

protected:
    Object *pattern, *matched;
    std::list<Object *> *path;

    void _update();
    bool _matchBList(BList<Object> *list);

private:
    MatchingVisitor(const MatchingVisitor &);
    MatchingVisitor &operator=(const MatchingVisitor &);

    hif::semantics::ILanguageSemantics *_sem;

    const MatchObjectOptions &_opt;
};

MatchingVisitor::MatchingVisitor(
    Object *_matched,
    std::list<Object *> *_path,
    hif::semantics::ILanguageSemantics *sem,
    const MatchObjectOptions &opt)
    : pattern(_path->back())
    , matched(_matched)
    , path(_path)
    , _sem(sem)
    , _opt(opt)
{
    _update();
}
MatchingVisitor::~MatchingVisitor() {}
Object *MatchingVisitor::getMatched()
{
    if (!path->empty())
        return nullptr;
    return matched;
}

void MatchingVisitor::_update()
{
    Object *recall = pattern;
    path->pop_back();
    if (path->empty())
        return;
    pattern = path->back();
    recall->acceptVisitor(*this);
}
bool MatchingVisitor::_matchBList(BList<Object> *list1)
{
    messageAssert(list1 != nullptr, "nullptr blist", nullptr, _sem);

    unsigned int count  = 0;
    BList<Object> *list = pattern->getBList();
    for (BList<Object>::iterator i = list->begin(); i != list->end(); ++i) {
        ++count;
        if (pattern == *i)
            break;
    }

    BList<Object>::iterator it = list1->begin();
    for (unsigned int i = 1; i < count && it != list1->end(); ++i, ++it)
        ;
    if (it == list1->end())
        return false;
    matched = *it;
    return true;
}
int MatchingVisitor::visitAggregate(Aggregate &o)
{
    Aggregate *c = dynamic_cast<Aggregate *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getOthers() == pattern) {
        matched = c->getOthers();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.alts)) {
            l = reinterpret_cast<BList<Object> *>(&c->alts);
        }
        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitAggregateAlt(AggregateAlt &o)
{
    AggregateAlt *c = dynamic_cast<AggregateAlt *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getValue() == pattern) {
        matched = c->getValue();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.indices)) {
            l = reinterpret_cast<BList<Object> *>(&c->indices);
        }
        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitAlias(Alias &o)
{
    Alias *c = dynamic_cast<Alias *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    return visitDataDeclaration(o);
}
int MatchingVisitor::visitArray(Array &o)
{
    Type *t = dynamic_cast<Type *>(matched);
    if (t == nullptr) {
        matched = nullptr;
        return 1;
    }
    Range *span = hif::typeGetSpan(t, _sem);

    Array *c = dynamic_cast<Array *>(matched);
    if (c == nullptr) {
        if (!_opt.matchStructure) {
            matched = nullptr;
            return 1;
        }
    }

    if (o.getType() == pattern && c != nullptr) {
        matched = c->getType();
    } else if (o.getSpan() == pattern) {
        matched = span;
    } else if (o.getBaseType(false) == pattern && c != nullptr) {
        matched = c->getBaseType(false);
    } else if (o.getBaseType(true) == pattern && c != nullptr) {
        matched = c->getBaseType(true);
    } else if (c == nullptr) {
        matched = nullptr;
        return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitAssign(Assign &o)
{
    Assign *c = dynamic_cast<Assign *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getRightHandSide() == pattern) {
        matched = c->getRightHandSide();
    } else if (o.getLeftHandSide() == pattern) {
        matched = c->getLeftHandSide();
    } else if (o.getDelay() == pattern) {
        matched = c->getDelay();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitSystem(System &o)
{
    System *c = dynamic_cast<System *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.designUnits)) {
            l = reinterpret_cast<BList<Object> *>(&c->designUnits);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.libraryDefs)) {
            l = reinterpret_cast<BList<Object> *>(&c->libraryDefs);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.declarations)) {
            l = reinterpret_cast<BList<Object> *>(&c->declarations);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.libraries)) {
            l = reinterpret_cast<BList<Object> *>(&c->libraries);
        }
        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitBit(Bit & /*o*/)
{
    Bit *c = dynamic_cast<Bit *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    _update();
    return 0;
}

int MatchingVisitor::visitBitValue(BitValue &o)
{
    BitValue *c = dynamic_cast<BitValue *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitBitvector(Bitvector &o)
{
    Type *t = dynamic_cast<Type *>(matched);
    if (t == nullptr) {
        matched = nullptr;
        return 1;
    }
    Range *span = hif::typeGetSpan(t, _sem);

    Bitvector *c = dynamic_cast<Bitvector *>(matched);
    if (c == nullptr) {
        if (!_opt.matchStructure) {
            matched = nullptr;
            return 1;
        }
    }

    if (o.getSpan() == pattern) {
        matched = span;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitBitvectorValue(BitvectorValue &o)
{
    BitvectorValue *c = dynamic_cast<BitvectorValue *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitBool(Bool & /*o*/)
{
    Bool *c = dynamic_cast<Bool *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    _update();
    return 0;
}

int MatchingVisitor::visitBoolValue(BoolValue &o)
{
    BoolValue *c = dynamic_cast<BoolValue *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitIfAlt(IfAlt &o)
{
    IfAlt *c = dynamic_cast<IfAlt *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getCondition() == pattern) {
        matched = c->getCondition();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.actions)) {
            l = reinterpret_cast<BList<Object> *>(&c->actions);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitIf(If &o)
{
    If *c = dynamic_cast<If *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.alts)) {
            l = reinterpret_cast<BList<Object> *>(&c->alts);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.defaults)) {
            l = reinterpret_cast<BList<Object> *>(&c->defaults);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitCast(Cast &o)
{
    Cast *c = dynamic_cast<Cast *>(matched);
    if (c == nullptr) {
        if (_opt.matchStructure && o.getValue() == pattern && !path->empty() &&
            hif::getChildSkippingCasts(o.getValue()) == path->front()) {
            // Re-calling, by skipping this cast.
            _update();
            return 0;
        }

        matched = nullptr;
        return 1;
    }

    if (o.getValue() == pattern) {
        matched = c->getValue();
    } else if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitChar(Char & /*o*/)
{
    Char *c = dynamic_cast<Char *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    _update();
    return 0;
}

int MatchingVisitor::visitCharValue(CharValue &o)
{
    CharValue *c = dynamic_cast<CharValue *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitConst(Const &o)
{
    Const *c = dynamic_cast<Const *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    return visitDataDeclaration(o);
}
int MatchingVisitor::visitContents(Contents &o)
{
    Contents *c = dynamic_cast<Contents *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getGlobalAction() == pattern) {
        matched = c->getGlobalAction();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.declarations)) {
            l = reinterpret_cast<BList<Object> *>(&c->declarations);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.libraries)) {
            l = reinterpret_cast<BList<Object> *>(&c->libraries);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.generates)) {
            l = reinterpret_cast<BList<Object> *>(&c->generates);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.instances)) {
            l = reinterpret_cast<BList<Object> *>(&c->instances);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.stateTables)) {
            l = reinterpret_cast<BList<Object> *>(&c->stateTables);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitDesignUnit(DesignUnit &o)
{
    DesignUnit *c = dynamic_cast<DesignUnit *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.views)) {
            l = reinterpret_cast<BList<Object> *>(&c->views);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitEnum(Enum &o)
{
    Enum *c = dynamic_cast<Enum *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.values)) {
            l = reinterpret_cast<BList<Object> *>(&c->values);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitEnumValue(EnumValue &o)
{
    EnumValue *c = dynamic_cast<EnumValue *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    return visitDataDeclaration(o);
}
int MatchingVisitor::visitExpression(Expression &o)
{
    Expression *c = dynamic_cast<Expression *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getValue1() == pattern) {
        matched = c->getValue1();
    } else if (o.getValue2() == pattern) {
        matched = c->getValue2();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitBreak(Break & /*o*/)
{
    Break *c = dynamic_cast<Break *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    _update();
    return 0;
}

int MatchingVisitor::visitEvent(Event & /*o*/)
{
    Event *c = dynamic_cast<Event *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    _update();
    return 0;
}

int MatchingVisitor::visitFunctionCall(FunctionCall &o)
{
    FunctionCall *c = dynamic_cast<FunctionCall *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else if (o.getInstance() == pattern) {
        matched = c->getInstance();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.templateParameterAssigns)) {
            l = reinterpret_cast<BList<Object> *>(&c->templateParameterAssigns);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.parameterAssigns)) {
            l = reinterpret_cast<BList<Object> *>(&c->parameterAssigns);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitField(Field &o)
{
    Field *c = dynamic_cast<Field *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    return visitDataDeclaration(o);
}
int MatchingVisitor::visitFieldReference(FieldReference &o)
{
    FieldReference *c = dynamic_cast<FieldReference *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getPrefix() == pattern) {
        matched = c->getPrefix();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitFile(File &o)
{
    File *c = dynamic_cast<File *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getBaseType(false) == pattern) {
        matched = c->getBaseType(false);
    } else if (o.getBaseType(true) == pattern) {
        matched = c->getBaseType(true);
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitFor(For &o)
{
    For *c = dynamic_cast<For *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getCondition() == pattern) {
        matched = c->getCondition();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.initDeclarations)) {
            l = reinterpret_cast<BList<Object> *>(&c->initDeclarations);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.initValues)) {
            l = reinterpret_cast<BList<Object> *>(&c->initValues);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.stepActions)) {
            l = reinterpret_cast<BList<Object> *>(&c->stepActions);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.forActions)) {
            l = reinterpret_cast<BList<Object> *>(&c->forActions);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitForGenerate(ForGenerate &o)
{
    ForGenerate *c = dynamic_cast<ForGenerate *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getGlobalAction() == pattern) {
        matched = c->getGlobalAction();
    } else if (o.getCondition() == pattern) {
        matched = c->getCondition();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.declarations)) {
            l = reinterpret_cast<BList<Object> *>(&c->declarations);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.generates)) {
            l = reinterpret_cast<BList<Object> *>(&c->generates);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.instances)) {
            l = reinterpret_cast<BList<Object> *>(&c->instances);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.stateTables)) {
            l = reinterpret_cast<BList<Object> *>(&c->stateTables);
        }
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.initDeclarations)) {
            l = reinterpret_cast<BList<Object> *>(&c->initDeclarations);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.initValues)) {
            l = reinterpret_cast<BList<Object> *>(&c->initValues);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.stepActions)) {
            l = reinterpret_cast<BList<Object> *>(&c->stepActions);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitFunction(Function &o)
{
    Function *c = dynamic_cast<Function *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else
        return visitSubProgram(o);

    _update();
    return 0;
}
int MatchingVisitor::visitGlobalAction(GlobalAction &o)
{
    GlobalAction *c = dynamic_cast<GlobalAction *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.actions)) {
            l = reinterpret_cast<BList<Object> *>(&c->actions);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitEntity(Entity &o)
{
    Entity *c = dynamic_cast<Entity *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.parameters)) {
            l = reinterpret_cast<BList<Object> *>(&c->parameters);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.ports)) {
            l = reinterpret_cast<BList<Object> *>(&c->ports);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitIfGenerate(IfGenerate &o)
{
    IfGenerate *c = dynamic_cast<IfGenerate *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getCondition() == pattern) {
        matched = c->getCondition();
    } else if (o.getGlobalAction() == pattern) {
        matched = c->getGlobalAction();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.declarations)) {
            l = reinterpret_cast<BList<Object> *>(&c->declarations);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.generates)) {
            l = reinterpret_cast<BList<Object> *>(&c->generates);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.instances)) {
            l = reinterpret_cast<BList<Object> *>(&c->instances);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.stateTables)) {
            l = reinterpret_cast<BList<Object> *>(&c->stateTables);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitInstance(Instance &o)
{
    Instance *c = dynamic_cast<Instance *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getReferencedType() == pattern) {
        matched = c->getReferencedType();
    } else if (o.getValue() == pattern) {
        matched = c->getValue();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.portAssigns)) {
            l = reinterpret_cast<BList<Object> *>(&c->portAssigns);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitInt(Int &o)
{
    Int *c = dynamic_cast<Int *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getSpan() == pattern) {
        matched = c->getSpan();
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitIntValue(IntValue &o)
{
    IntValue *c = dynamic_cast<IntValue *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitLibraryDef(LibraryDef &o)
{
    LibraryDef *c = dynamic_cast<LibraryDef *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.declarations)) {
            l = reinterpret_cast<BList<Object> *>(&c->declarations);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.libraries)) {
            l = reinterpret_cast<BList<Object> *>(&c->libraries);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitLibrary(Library &o)
{
    Library *c = dynamic_cast<Library *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getInstance() == pattern) {
        matched = c->getInstance();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitMember(Member &o)
{
    Member *c = dynamic_cast<Member *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getPrefix() == pattern) {
        matched = c->getPrefix();
    } else if (o.getIndex() == pattern) {
        matched = c->getIndex();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitIdentifier(Identifier &o)
{
    Identifier *c = dynamic_cast<Identifier *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitContinue(Continue & /*o*/)
{
    Continue *c = dynamic_cast<Continue *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    _update();
    return 0;
}
int MatchingVisitor::visitNull(Null & /*o*/)
{
    Null *c = dynamic_cast<Null *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    _update();
    return 0;
}
int MatchingVisitor::visitTransition(Transition &o)
{
    Transition *c = dynamic_cast<Transition *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.enablingLabelList)) {
            l = reinterpret_cast<BList<Object> *>(&c->enablingLabelList);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.enablingList)) {
            l = reinterpret_cast<BList<Object> *>(&c->enablingList);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.updateLabelList)) {
            l = reinterpret_cast<BList<Object> *>(&c->updateLabelList);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.updateList)) {
            l = reinterpret_cast<BList<Object> *>(&c->updateList);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitParameterAssign(ParameterAssign &o)
{
    ParameterAssign *c = dynamic_cast<ParameterAssign *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return visitPPAssign(o);

    return 0;
}
int MatchingVisitor::visitParameter(Parameter &o)
{
    Parameter *c = dynamic_cast<Parameter *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    return visitDataDeclaration(o);
}
int MatchingVisitor::visitProcedureCall(ProcedureCall &o)
{
    ProcedureCall *c = dynamic_cast<ProcedureCall *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getInstance() == pattern) {
        matched = c->getInstance();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.templateParameterAssigns)) {
            l = reinterpret_cast<BList<Object> *>(&c->templateParameterAssigns);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.parameterAssigns)) {
            l = reinterpret_cast<BList<Object> *>(&c->parameterAssigns);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitPortAssign(PortAssign &o)
{
    PortAssign *c = dynamic_cast<PortAssign *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getPartialBind() == pattern) {
        matched = c->getPartialBind();
    } else
        return visitPPAssign(o);

    return 0;
}
int MatchingVisitor::visitPort(Port &o)
{
    Port *c = dynamic_cast<Port *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    return visitDataDeclaration(o);
}
int MatchingVisitor::visitPPAssign(PPAssign &o)
{
    PPAssign *c = dynamic_cast<PPAssign *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getValue() == pattern) {
        matched = c->getValue();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitProcedure(Procedure &o)
{
    Procedure *c = dynamic_cast<Procedure *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    return visitSubProgram(o);
}
int MatchingVisitor::visitRange(Range &o)
{
    Range *c = dynamic_cast<Range *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else if (o.getLeftBound() == pattern) {
        matched = _getBound(c, true, o.getDirection());
    } else if (o.getRightBound() == pattern) {
        matched = _getBound(c, false, o.getDirection());
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitReal(Real &o)
{
    Real *c = dynamic_cast<Real *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getSpan() == pattern) {
        matched = c->getSpan();
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitRealValue(RealValue &o)
{
    RealValue *c = dynamic_cast<RealValue *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitRecord(Record &o)
{
    Record *c = dynamic_cast<Record *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getBaseType(false) == pattern && c != nullptr) {
        matched = c->getBaseType(false);
    } else if (o.getBaseType(true) == pattern && c != nullptr) {
        matched = c->getBaseType(true);
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.fields)) {
            l = reinterpret_cast<BList<Object> *>(&c->fields);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitRecordValue(RecordValue &o)
{
    RecordValue *c = dynamic_cast<RecordValue *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.alts)) {
            l = reinterpret_cast<BList<Object> *>(&c->alts);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitRecordValueAlt(RecordValueAlt &o)
{
    RecordValueAlt *c = dynamic_cast<RecordValueAlt *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getValue() == pattern) {
        matched = c->getValue();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitReturn(Return &o)
{
    Return *c = dynamic_cast<Return *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getValue() == pattern) {
        matched = c->getValue();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitSignal(Signal &o)
{
    Signal *c = dynamic_cast<Signal *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    return visitDataDeclaration(o);
}

int MatchingVisitor::visitSigned(Signed &o)
{
    Type *t = dynamic_cast<Type *>(matched);
    if (t == nullptr) {
        matched = nullptr;
        return 1;
    }
    Range *span = hif::typeGetSpan(t, _sem);

    Signed *c = dynamic_cast<Signed *>(matched);
    if (c == nullptr) {
        if (!_opt.matchStructure) {
            matched = nullptr;
            return 1;
        }
    }

    if (o.getSpan() == pattern) {
        matched = span;
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitSlice(Slice &o)
{
    Slice *c = dynamic_cast<Slice *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getPrefix() == pattern) {
        matched = c->getPrefix();
    } else if (o.getSpan() == pattern) {
        matched = c->getSpan();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitState(State &o)
{
    State *c = dynamic_cast<State *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.actions)) {
            l = reinterpret_cast<BList<Object> *>(&c->actions);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.invariants)) {
            l = reinterpret_cast<BList<Object> *>(&c->invariants);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitString(String &o)
{
    String *c = dynamic_cast<String *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getSpanInformation() == pattern) {
        matched = c->getSpanInformation();
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitStateTable(StateTable &o)
{
    StateTable *c = dynamic_cast<StateTable *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.edges)) {
            l = reinterpret_cast<BList<Object> *>(&c->edges);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.states)) {
            l = reinterpret_cast<BList<Object> *>(&c->states);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.declarations)) {
            l = reinterpret_cast<BList<Object> *>(&c->declarations);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.sensitivity)) {
            l = reinterpret_cast<BList<Object> *>(&c->sensitivity);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.sensitivityPos)) {
            l = reinterpret_cast<BList<Object> *>(&c->sensitivityPos);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.sensitivityNeg)) {
            l = reinterpret_cast<BList<Object> *>(&c->sensitivityNeg);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitSubProgram(SubProgram &o)
{
    SubProgram *c = dynamic_cast<SubProgram *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getStateTable() == pattern) {
        matched = c->getStateTable();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.templateParameters)) {
            l = reinterpret_cast<BList<Object> *>(&c->templateParameters);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.parameters)) {
            l = reinterpret_cast<BList<Object> *>(&c->parameters);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitSwitchAlt(SwitchAlt &o)
{
    SwitchAlt *c = dynamic_cast<SwitchAlt *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.actions)) {
            l = reinterpret_cast<BList<Object> *>(&c->actions);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.conditions)) {
            l = reinterpret_cast<BList<Object> *>(&c->conditions);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitSwitch(Switch &o)
{
    Switch *c = dynamic_cast<Switch *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getCondition() == pattern) {
        matched = c->getCondition();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.alts)) {
            l = reinterpret_cast<BList<Object> *>(&c->alts);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.defaults)) {
            l = reinterpret_cast<BList<Object> *>(&c->defaults);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitStringValue(StringValue &o)
{
    StringValue *c = dynamic_cast<StringValue *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitTime(Time &)
{
    Time *c = dynamic_cast<Time *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    _update();
    return 0;
}
int MatchingVisitor::visitTimeValue(TimeValue &o)
{
    TimeValue *c = dynamic_cast<TimeValue *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitTypeDef(TypeDef &o)
{
    TypeDef *c = dynamic_cast<TypeDef *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getRange() == pattern) {
        matched = c->getRange();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.templateParameters)) {
            l = reinterpret_cast<BList<Object> *>(&c->templateParameters);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitUnsigned(Unsigned &o)
{
    Type *t = dynamic_cast<Type *>(matched);
    if (t == nullptr) {
        matched = nullptr;
        return 1;
    }
    Range *span = hif::typeGetSpan(t, _sem);

    Unsigned *c = dynamic_cast<Unsigned *>(matched);
    if (c == nullptr) {
        if (!_opt.matchStructure) {
            matched = nullptr;
            return 1;
        }
    }

    if (o.getSpan() == pattern) {
        matched = span;
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitDataDeclaration(DataDeclaration &o)
{
    DataDeclaration *c = dynamic_cast<DataDeclaration *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getValue() == pattern) {
        matched = c->getValue();
    } else if (o.getRange() == pattern) {
        matched = c->getRange();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitTypeReference(TypeReference &o)
{
    TypeReference *c = dynamic_cast<TypeReference *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getInstance() == pattern) {
        matched = c->getInstance();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.templateParameterAssigns)) {
            l = reinterpret_cast<BList<Object> *>(&c->templateParameterAssigns);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.ranges)) {
            l = reinterpret_cast<BList<Object> *>(&c->ranges);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitTypeTP(TypeTP &o)
{
    TypeTP *c = dynamic_cast<TypeTP *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    TypeTPAssign *c = dynamic_cast<TypeTPAssign *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitVariable(Variable &o)
{
    Variable *c = dynamic_cast<Variable *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    return visitDataDeclaration(o);
}

int MatchingVisitor::visitValueStatement(ValueStatement &o)
{
    ValueStatement *c = dynamic_cast<ValueStatement *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getValue() == pattern) {
        matched = c->getValue();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitValueTP(ValueTP &o)
{
    ValueTP *c = dynamic_cast<ValueTP *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    return visitDataDeclaration(o);
}
int MatchingVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    ValueTPAssign *c = dynamic_cast<ValueTPAssign *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getValue() == pattern) {
        matched = c->getValue();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitView(View &o)
{
    View *c = dynamic_cast<View *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getContents() == pattern) {
        matched = c->getContents();
    } else if (o.getEntity() == pattern) {
        matched = c->getEntity();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.templateParameters)) {
            l = reinterpret_cast<BList<Object> *>(&c->templateParameters);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.declarations)) {
            l = reinterpret_cast<BList<Object> *>(&c->declarations);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.libraries)) {
            l = reinterpret_cast<BList<Object> *>(&c->libraries);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.inheritances)) {
            l = reinterpret_cast<BList<Object> *>(&c->inheritances);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitViewReference(ViewReference &o)
{
    ViewReference *c = dynamic_cast<ViewReference *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getInstance() == pattern) {
        matched = c->getInstance();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.templateParameterAssigns)) {
            l = reinterpret_cast<BList<Object> *>(&c->templateParameterAssigns);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitWait(Wait &o)
{
    Wait *c = dynamic_cast<Wait *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getCondition() == pattern) {
        matched = c->getCondition();
    } else if (o.getRepetitions() == pattern) {
        matched = c->getRepetitions();
    } else if (o.getTime() == pattern) {
        matched = c->getTime();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.sensitivity)) {
            l = reinterpret_cast<BList<Object> *>(&c->sensitivity);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.sensitivityPos)) {
            l = reinterpret_cast<BList<Object> *>(&c->sensitivityPos);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.sensitivityNeg)) {
            l = reinterpret_cast<BList<Object> *>(&c->sensitivityNeg);
        } else if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.actions)) {
            l = reinterpret_cast<BList<Object> *>(&c->actions);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitWhen(When &o)
{
    When *c = dynamic_cast<When *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getDefault() == pattern) {
        matched = c->getDefault();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.alts)) {
            l = reinterpret_cast<BList<Object> *>(&c->alts);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitWhenAlt(WhenAlt &o)
{
    WhenAlt *c = dynamic_cast<WhenAlt *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getCondition() == pattern) {
        matched = c->getCondition();
    } else if (o.getValue() == pattern) {
        matched = c->getValue();
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitWhile(While &o)
{
    While *c = dynamic_cast<While *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getCondition() == pattern) {
        matched = c->getCondition();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.actions)) {
            l = reinterpret_cast<BList<Object> *>(&c->actions);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitWith(With &o)
{
    With *c = dynamic_cast<With *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getCondition() == pattern) {
        matched = c->getCondition();
    } else if (o.getDefault() == pattern) {
        matched = c->getDefault();
    } else if (o.getSemanticType() == pattern) {
        matched = c->getSemanticType();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.alts)) {
            l = reinterpret_cast<BList<Object> *>(&c->alts);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}
int MatchingVisitor::visitWithAlt(WithAlt &o)
{
    WithAlt *c = dynamic_cast<WithAlt *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getValue() == pattern) {
        matched = c->getValue();
    } else if (pattern->isInBList()) {
        BList<Object> *l = nullptr;
        if (pattern->getBList() == reinterpret_cast<BList<Object> *>(&o.conditions)) {
            l = reinterpret_cast<BList<Object> *>(&c->conditions);
        }

        if (!_matchBList(l))
            return 1;
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitReference(Reference &o)
{
    Reference *c = dynamic_cast<Reference *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getBaseType(false) == pattern) {
        matched = c->getBaseType(false);
    } else if (o.getBaseType(true) == pattern) {
        matched = c->getBaseType(true);
    } else
        return 1;

    _update();
    return 0;
}

int MatchingVisitor::visitPointer(Pointer &o)
{
    Pointer *c = dynamic_cast<Pointer *>(matched);
    if (c == nullptr) {
        matched = nullptr;
        return 1;
    }

    if (o.getType() == pattern) {
        matched = c->getType();
    } else if (o.getBaseType(false) == pattern) {
        matched = c->getBaseType(false);
    } else if (o.getBaseType(true) == pattern) {
        matched = c->getBaseType(true);
    } else
        return 1;

    _update();
    return 0;
}
} // end namespace

MatchObjectOptions::MatchObjectOptions()
    : matchStructure(false)
    , skipReferences(false)
{
    // ntd
}

MatchObjectOptions::~MatchObjectOptions()
{
    // ntd
}
Object *matchObject(
    Object *pattern,
    Object *referenceTree,
    Object *matchedTree,
    hif::semantics::ILanguageSemantics *sem,
    const MatchObjectOptions &opt)
{
    if (opt.skipReferences) {
        Reference *r1 = dynamic_cast<Reference *>(referenceTree);
        Reference *r2 = dynamic_cast<Reference *>(matchedTree);
        if (r1 != nullptr && r2 == nullptr) {
            referenceTree = r1->getType();
        } else if (r1 == nullptr && r2 != nullptr) {
            matchedTree = r2->getType();
        }
    }
    std::list<Object *> path;
    getPath(pattern, referenceTree, path);
    if (path.empty())
        return nullptr;
    MatchingVisitor v(matchedTree, &path, sem, opt);
    return v.getMatched();
}

} // namespace manipulation
} // namespace hif
