/// @file equals.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cmath>
#include <cstring>
#include <iostream>

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/equals.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

int comparePorts(const Object *p1, const Object *p2)
{
    std::string s1(hif::objectGetName(const_cast<Object *>(p1)));
    std::string s2(hif::objectGetName(const_cast<Object *>(p2)));
    if (s1 < s2)
        return -1;
    if (s1 > s2)
        return 1;
    return 0;
}
class HifEqualsVisitor : public HifVisitor
{
public:
    HifEqualsVisitor(const EqualsOptions &option);
    virtual ~HifEqualsVisitor();

    HifEqualsVisitor(const HifEqualsVisitor &)                     = delete;
    auto operator=(const HifEqualsVisitor &) -> HifEqualsVisitor & = delete;

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
    virtual int visitMember(Member &o);
    virtual int visitNull(Null &o);
    virtual int visitTransition(Transition &o);
    virtual int visitParameterAssign(ParameterAssign &o);
    virtual int visitParameter(Parameter &o);
    virtual int visitPointer(Pointer &o);
    virtual int visitPortAssign(PortAssign &o);
    virtual int visitPort(Port &o);
    virtual int visitProcedure(Procedure &o);
    virtual int visitProcedureCall(ProcedureCall &o);
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
    bool _equals(Object *obj1, Object *obj2);

    template <typename T> bool _equals(BList<T> &list1, BList<T> &list2);

private:
    bool _equalsSpanOfConstexpr(Range *r1, Range *r2);
    bool _equalsDeclarationRangeConstraint(Range *r1, Range *r2);
    bool _equalsSymbolDeclarations(Object *s1, Object *s2);
    bool _equalsCheckInnerType(Type *s1, Type *s2);
    bool _equalsChildren(Object *o1, Object *o2);
    template <typename T> bool _equalsChildren(BList<T> &o1, BList<T> &o2);
    bool _equalsSpans(Range *r1, Range *r2);
    bool _equalsInstance(Object *o1, Object *o2);
    bool _equalsProperties(Object *o1, Object *o2);
    bool _equalsAdditionalKeywords(Declaration *o1, Declaration *o2);

    Value *_spanGetSize(Range *r);

    bool _manageCheckOnlyName(Object *obj1, Object *obj2);
    bool _manageCheckOnlySymbolsDeclaration(Object *obj1, Object *obj2);

    Object *_objToCompare;
    const EqualsOptions _options;
    bool _isInSignature;
    bool _isViewSignature;
    bool _isViewInterfaceCheck;
};

HifEqualsVisitor::HifEqualsVisitor(const EqualsOptions &option)
    : _objToCompare(nullptr)
    , _options(option)
    , _isInSignature(false)
    , _isViewSignature(false)
    , _isViewInterfaceCheck(false)
{
    //ntd
}

HifEqualsVisitor::~HifEqualsVisitor()
{
    // ntd
}

Value *HifEqualsVisitor::_spanGetSize(Range *r)
{
    Value *rangeSpan = nullptr;
    messageAssert(_options.sem != nullptr, "Expected semantics", nullptr, nullptr);
    std::uint64_t rangeSize = hif::semantics::spanGetBitwidth(r, _options.sem);
    if (rangeSize == 0) {
        Value *lBound = hif::copy(r->getLeftBound());
        Value *rBound = hif::copy(r->getRightBound());

        IntValue *rightAddendum = new IntValue(1);

        if (r->getDirection() == dir_upto) {
            // Generate Expression (SUP-INF)
            Expression *leftAddendum = new Expression();
            leftAddendum->setOperator(op_minus);
            leftAddendum->setValue1(lBound);
            leftAddendum->setValue2(rBound);
            // Generate Expression (SUP-INF+1)
            rangeSpan = new Expression();
            static_cast<Expression *>(rangeSpan)->setOperator(op_plus);
            static_cast<Expression *>(rangeSpan)->setValue1(leftAddendum);
            static_cast<Expression *>(rangeSpan)->setValue2(rightAddendum);
        } else if (r->getDirection() == dir_downto) {
            Expression *leftAddendum = new Expression();
            leftAddendum->setOperator(op_minus);
            leftAddendum->setValue1(rBound);
            leftAddendum->setValue2(lBound);
            // Generate Expression (SUP-INF+1)
            rangeSpan = new Expression();
            static_cast<Expression *>(rangeSpan)->setOperator(op_plus);
            static_cast<Expression *>(rangeSpan)->setValue1(leftAddendum);
            static_cast<Expression *>(rangeSpan)->setValue2(rightAddendum);
        } else {
            delete lBound;
            delete rBound;
            delete rightAddendum;
        }
    } else {
        rangeSpan = new IntValue(static_cast<std::int64_t>(rangeSize));
    }

    return rangeSpan;
}

bool HifEqualsVisitor::_manageCheckOnlyName(Object *obj1, Object *obj2)
{
    std::string obj1n = hif::objectGetName(obj1);
    std::string obj2n = hif::objectGetName(obj2);
    if (obj1n.empty() && obj2n.empty())
        return true;
    else if (obj1n.empty() || obj2n.empty())
        return false;
    return (obj1n == obj2n);
}

bool HifEqualsVisitor::_manageCheckOnlySymbolsDeclaration(Object *obj1, Object *obj2)
{
    hif::features::ISymbol *symb1 = dynamic_cast<hif::features::ISymbol *>(obj1);
    hif::features::ISymbol *symb2 = dynamic_cast<hif::features::ISymbol *>(obj2);
    if (symb1 == nullptr || symb2 == nullptr)
        return false;
    // Matching only somthing like Fieldref vs. Identifier with same declaration.
    if (obj1->getClassId() == obj2->getClassId())
        return false;

    hif::semantics::ILanguageSemantics *hifSem = hif::semantics::HIFSemantics::getInstance();
    hif::semantics::DeclarationOptions opt;
    opt.dontSearch  = true;
    Declaration *d1 = hif::semantics::getDeclaration(obj1, hifSem, opt);
    Declaration *d2 = hif::semantics::getDeclaration(obj2, hifSem, opt);

    if (d1 == nullptr && d2 == nullptr)
        return false;
    return (d1 == d2);
}

bool HifEqualsVisitor::_equalsSpanOfConstexpr(Range *r1, Range *r2)
{
    if (!_options.checkSpans)
        return true;
    if (_isViewInterfaceCheck)
        return true;
    if (_equalsChildren(r1, r2))
        return true;

    //  size of ranges must be equal. Don't care about direction.
    Value *v1 = _spanGetSize(r1);
    Value *v2 = _spanGetSize(r2);

    bool ret = _equals(v1, v2);

    delete v1;
    delete v2;
    return ret;
}
bool HifEqualsVisitor::_equalsDeclarationRangeConstraint(Range *r1, Range *r2)
{
    if (!_options.checkDeclarationRangeConstraint)
        return true;
    if (_equalsChildren(r1, r2))
        return true;

    return false;
}
bool HifEqualsVisitor::_equalsSymbolDeclarations(Object *s1, Object *s2)
{
    messageAssert(
        (dynamic_cast<hif::features::ISymbol *>(s1) != nullptr &&
         dynamic_cast<hif::features::ISymbol *>(s2) != nullptr),
        "Expected two symbols", nullptr, nullptr);

    if (!_options.assureSameSymbolDeclarations) {
        return true;
    }

    hif::semantics::ILanguageSemantics *hifSem = hif::semantics::HIFSemantics::getInstance();
    hif::semantics::DeclarationOptions opt;
    opt.dontSearch  = true;
    Declaration *d1 = hif::semantics::getDeclaration(s1, hifSem, opt);
    Declaration *d2 = hif::semantics::getDeclaration(s2, hifSem, opt);

    if (d1 == d2)
        return true;
    if (d1 == nullptr || d2 == nullptr)
        return true;

    return false;
}

bool HifEqualsVisitor::_equalsCheckInnerType(Type *s1, Type *s2)
{
    if (!_options.checkInnerTypeOfComposite && !_isInSignature)
        return true;
    return _equalsChildren(s1, s2);
}

bool HifEqualsVisitor::_equalsChildren(Object *o1, Object *o2)
{
    if (_options.skipChilden && !_isInSignature)
        return true;
    return _equals(o1, o2);
}

bool HifEqualsVisitor::_equalsSpans(Range *r1, Range *r2)
{
    if (!_options.checkSpans)
        return true;
    if (_isViewInterfaceCheck)
        return true;
    return _equalsChildren(r1, r2);
}

bool HifEqualsVisitor::_equalsInstance(Object *o1, Object *o2)
{
    if (!_options.checkReferencedInstance)
        return true;
    return _equalsChildren(o1, o2);
}

bool HifEqualsVisitor::_equalsProperties(Object *o1, Object *o2)
{
    if (!_options.assureSameProperties)
        return true;

    Object::PropertyMapIterator i = o1->getPropertyBeginIterator();
    Object::PropertyMapIterator j = o2->getPropertyBeginIterator();
    for (; i != o1->getPropertyEndIterator() && j != o2->getPropertyEndIterator(); ++i, ++j) {
        if (i->first != j->first)
            return 1;

        TypedObject *li = o1->getProperty(i->first);
        TypedObject *lj = o2->getProperty(j->first);
        if (!hif::equals(li, lj, _options))
            return false;
    }

    if (i != o1->getPropertyEndIterator() || j != o2->getPropertyEndIterator())
        return false;

    return true;
}

bool HifEqualsVisitor::_equalsAdditionalKeywords(Declaration *o1, Declaration *o2)
{
    if (!_options.assureSameAdditionalKeywords)
        return true;
    Declaration::KeywordList::iterator it1 = o1->getAdditionalKeywordsBeginIterator();
    Declaration::KeywordList::iterator it2 = o2->getAdditionalKeywordsBeginIterator();
    for (; it1 != o1->getAdditionalKeywordsEndIterator(); ++it1) {
        if (!o2->checkAdditionalKeyword(*it1))
            return false;
    }
    for (; it2 != o2->getAdditionalKeywordsEndIterator(); ++it2) {
        if (!o1->checkAdditionalKeyword(*it2))
            return false;
    }
    return true;
}

template <typename T> bool HifEqualsVisitor::_equalsChildren(BList<T> &o1, BList<T> &o2)
{
    if (_options.skipChilden && !_isInSignature)
        return true;
    return _equals(o1, o2);
}
// ////////////////////////////////////////////////////////////////////////////
bool HifEqualsVisitor::_equals(Object *obj1, Object *obj2)
{
    if (obj1 == obj2)
        return true;
    if (obj1 == nullptr)
        return _options.skipNullBranches;
    if (obj2 == nullptr)
        return false;

    if (dynamic_cast<Reference *>(obj1) != nullptr && (!_options.skipReferences)) {
        Reference *r1 = static_cast<Reference *>(obj1);
        obj1          = r1->getType();
    }

    if (dynamic_cast<Reference *>(obj2) != nullptr && (!_options.skipReferences)) {
        Reference *r2 = static_cast<Reference *>(obj2);
        obj2          = r2->getType();
    }

    if (_options.checkOnlyNames) {
        return _manageCheckOnlyName(obj1, obj2);
    }

    if (_options.checkOnlySymbolsDeclarations) {
        if (_manageCheckOnlySymbolsDeclaration(obj1, obj2))
            return true;
    }

    Object *o1 = obj1;
    Object *o2 = obj2;
    Bitvector v1;
    Bitvector v2;
    if (_options.handleVectorTypes || _isViewSignature) {
        Signed *o1s   = dynamic_cast<Signed *>(obj1);
        Signed *o2s   = dynamic_cast<Signed *>(obj2);
        Unsigned *o1u = dynamic_cast<Unsigned *>(obj1);
        Unsigned *o2u = dynamic_cast<Unsigned *>(obj2);

        if (o1s != nullptr) {
            o1 = &v1;
            v1.setSigned(true);
            v1.setSpan(hif::copy(o1s->getSpan()));
            v1.setResolved(true);
            v1.setLogic(true);
            v1.setConstexpr(o1s->isConstexpr());
            v1.setTypeVariant(o1s->getTypeVariant());
        } else if (o1u != nullptr) {
            o1 = &v1;
            v1.setSigned(false);
            v1.setSpan(hif::copy(o1u->getSpan()));
            v1.setResolved(true);
            v1.setLogic(true);
            v1.setConstexpr(o1u->isConstexpr());
            v1.setTypeVariant(o1u->getTypeVariant());
        }

        if (o2s != nullptr) {
            o2 = &v2;
            v2.setSigned(true);
            v2.setSpan(hif::copy(o2s->getSpan()));
            v2.setResolved(true);
            v2.setLogic(true);
            v2.setConstexpr(o2s->isConstexpr());
            v2.setTypeVariant(o2s->getTypeVariant());
        } else if (o2u != nullptr) {
            o2 = &v2;
            v2.setSigned(false);
            v2.setSpan(hif::copy(o2u->getSpan()));
            v2.setResolved(true);
            v2.setLogic(true);
            v2.setConstexpr(o2u->isConstexpr());
            v2.setTypeVariant(o2u->getTypeVariant());
        }
    }

    if (o1->getClassId() != o2->getClassId())
        return false;
    else if (_options.checkOnlyTypes)
        return true;

    _objToCompare = o2;
    return (o1->acceptVisitor(*this) != 0);
}

template <typename T> bool HifEqualsVisitor::_equals(BList<T> &list1, BList<T> &list2)
{
    typename BList<T>::iterator i, j;

    if (&list1 == &list2)
        return true;

    if (list1.size() != list2.size())
        return false;

    for (i = list1.begin(), j = list2.begin(); i != list1.end(); ++i, ++j) {
        if (!_equals(*i, *j))
            return false;
    }

    return true;
}
int HifEqualsVisitor::visitAggregate(Aggregate &o)
{
    Aggregate *obj2 = static_cast<Aggregate *>(_objToCompare);

    if (!_equalsChildren(o.getOthers(), obj2->getOthers()))
        return false;
    if (!_equalsChildren(o.alts, obj2->alts))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitAggregateAlt(AggregateAlt &o)
{
    AggregateAlt *obj2 = static_cast<AggregateAlt *>(_objToCompare);

    if (!_equalsChildren(o.indices, obj2->indices))
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitAlias(Alias &o)
{
    Alias *obj2 = static_cast<Alias *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.isStandard() != obj2->isStandard())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsDeclarationRangeConstraint(o.getRange(), obj2->getRange()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitArray(Array &o)
{
    Array *obj2 = static_cast<Array *>(_objToCompare);

    if (!_equalsSpans(o.getSpan(), obj2->getSpan()))
        return false;
    if (_options.checkSignedFlag)
        if (o.isSigned() != obj2->isSigned())
            return false;
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsCheckInnerType(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitAssign(Assign &o)
{
    Assign *obj2 = static_cast<Assign *>(_objToCompare);

    if (!_equalsChildren(o.getLeftHandSide(), obj2->getLeftHandSide()))
        return false;
    if (!_equalsChildren(o.getRightHandSide(), obj2->getRightHandSide()))
        return false;
    if (!_equalsChildren(o.getDelay(), obj2->getDelay()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitSystem(System &o)
{
    System *obj2 = static_cast<System *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.getLanguageID() != obj2->getLanguageID())
        return false;

    if (!_equalsChildren(o.libraryDefs, obj2->libraryDefs))
        return false;
    if (!_equalsChildren(o.designUnits, obj2->designUnits))
        return false;
    if (!_equalsChildren(o.declarations, obj2->declarations))
        return false;
    if (!_equalsChildren(o.libraries, obj2->libraries))
        return false;
    if (!_equalsChildren(o.actions, obj2->actions))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitBit(Bit &o)
{
    Bit *obj2 = static_cast<Bit *>(_objToCompare);

    if (_options.handleConstexprTypes && (o.isConstexpr() || obj2->isConstexpr())) {
        // nothing to check
    } else {
        if (_options.checkResolvedFlag)
            if (o.isResolved() != obj2->isResolved())
                return false;
        if (_options.checkConstexprFlag)
            if (o.isConstexpr() != obj2->isConstexpr())
                return false;
        if (_options.checkLogicFlag)
            if (o.isLogic() != obj2->isLogic())
                return false;
        if (_options.checkTypeVariantField)
            if (o.getTypeVariant() != obj2->getTypeVariant())
                return false;
    }
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitBitValue(BitValue &o)
{
    BitValue *obj2 = static_cast<BitValue *>(_objToCompare);

    if (o.getValue() != obj2->getValue())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitBitvector(Bitvector &o)
{
    Bitvector *obj2 = static_cast<Bitvector *>(_objToCompare);

    if (_options.handleConstexprTypes && (o.isConstexpr() || obj2->isConstexpr())) {
        if (!_equalsSpanOfConstexpr(o.getSpan(), obj2->getSpan()))
            return false;
    } else {
        if (!_equalsSpans(o.getSpan(), obj2->getSpan()))
            return false;
        if (_options.checkConstexprFlag)
            if (o.isConstexpr() != obj2->isConstexpr())
                return false;
        if (_options.checkSignedFlag)
            if (o.isSigned() != obj2->isSigned())
                return false;
        if (_options.checkTypeVariantField)
            if (o.getTypeVariant() != obj2->getTypeVariant())
                return false;
    }

    if (_options.checkLogicFlag)
        if (o.isLogic() != obj2->isLogic())
            return false;
    if (_options.checkResolvedFlag)
        if (o.isResolved() != obj2->isResolved())
            return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitBitvectorValue(BitvectorValue &o)
{
    BitvectorValue *obj2 = static_cast<BitvectorValue *>(_objToCompare);

    if (o.getValue() != obj2->getValue())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitBool(Bool &o)
{
    Bool *obj2 = static_cast<Bool *>(_objToCompare);

    if (_options.handleConstexprTypes && (o.isConstexpr() || obj2->isConstexpr())) {
        // ntd
    } else {
        if (_options.checkConstexprFlag)
            if (o.isConstexpr() != obj2->isConstexpr())
                return false;
        if (_options.checkTypeVariantField)
            if (o.getTypeVariant() != obj2->getTypeVariant())
                return false;
    }
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitBoolValue(BoolValue &o)
{
    BoolValue *obj2 = static_cast<BoolValue *>(_objToCompare);

    if (o.getValue() != obj2->getValue())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitIf(If &o)
{
    If *obj2 = static_cast<If *>(_objToCompare);

    if (!_equalsChildren(o.alts, obj2->alts))
        return false;
    if (!_equalsChildren(o.defaults, obj2->defaults))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitIfAlt(IfAlt &o)
{
    IfAlt *obj2 = static_cast<IfAlt *>(_objToCompare);

    if (!_equalsChildren(o.getCondition(), obj2->getCondition()))
        return false;
    if (!_equalsChildren(o.actions, obj2->actions))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitCast(Cast &o)
{
    Cast *obj2 = static_cast<Cast *>(_objToCompare);

    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitChar(Char &o)
{
    Char *obj2 = static_cast<Char *>(_objToCompare);

    if (_options.checkConstexprFlag)
        if (o.isConstexpr() != obj2->isConstexpr())
            return false;
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitCharValue(CharValue &o)
{
    CharValue *obj2 = static_cast<CharValue *>(_objToCompare);

    if (o.getValue() != obj2->getValue())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitConst(Const &o)
{
    Const *obj2 = static_cast<Const *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.isInstance() != obj2->isInstance())
        return false;
    if (o.isDefine() != obj2->isDefine())
        return false;
    if (o.isStandard() != obj2->isStandard())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsDeclarationRangeConstraint(o.getRange(), obj2->getRange()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitContents(Contents &o)
{
    Contents *obj2 = static_cast<Contents *>(_objToCompare);

    if (!_equalsChildren(o.declarations, obj2->declarations))
        return false;
    if (!_equalsChildren(o.stateTables, obj2->stateTables))
        return false;
    if (!_equalsChildren(o.getGlobalAction(), obj2->getGlobalAction()))
        return false;
    if (!_equalsChildren(o.generates, obj2->generates))
        return false;
    if (!_equalsChildren(o.instances, obj2->instances))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitDesignUnit(DesignUnit &o)
{
    DesignUnit *obj2 = static_cast<DesignUnit *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.views, obj2->views))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitEnum(Enum &o)
{
    Enum *obj2 = static_cast<Enum *>(_objToCompare);

    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsChildren(o.values, obj2->values))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitEnumValue(EnumValue &o)
{
    EnumValue *obj2 = static_cast<EnumValue *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsDeclarationRangeConstraint(o.getRange(), obj2->getRange()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitEvent(Event &o)
{
    Time *obj2 = static_cast<Time *>(_objToCompare);

    if (_options.checkConstexprFlag)
        if (o.isConstexpr() != obj2->isConstexpr())
            return false;
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitBreak(Break &o)
{
    Break *obj2 = static_cast<Break *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitExpression(Expression &o)
{
    Expression *obj2 = static_cast<Expression *>(_objToCompare);

    if (o.getOperator() != obj2->getOperator())
        return false;
    if (!_equalsChildren(o.getValue1(), obj2->getValue1()))
        return false;
    if (!_equalsChildren(o.getValue2(), obj2->getValue2()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitFunctionCall(FunctionCall &o)
{
    FunctionCall *obj2 = static_cast<FunctionCall *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.templateParameterAssigns, obj2->templateParameterAssigns))
        return false;
    if (!_equalsChildren(o.parameterAssigns, obj2->parameterAssigns))
        return false;
    if (!_equalsChildren(o.getInstance(), obj2->getInstance()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return _equalsSymbolDeclarations(&o, obj2);
}

int HifEqualsVisitor::visitField(Field &o)
{
    Field *obj2 = static_cast<Field *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.getDirection() != obj2->getDirection())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (_options.checkFieldsInitialvalue)
        if (!_equalsChildren(o.getValue(), obj2->getValue()))
            return false;
    if (!_equalsDeclarationRangeConstraint(o.getRange(), obj2->getRange()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitFieldReference(FieldReference &o)
{
    FieldReference *obj2 = static_cast<FieldReference *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.getPrefix(), obj2->getPrefix()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return _equalsSymbolDeclarations(&o, obj2);
}

int HifEqualsVisitor::visitFile(File &o)
{
    File *obj2 = static_cast<File *>(_objToCompare);

    if (!_equalsCheckInnerType(o.getType(), obj2->getType()))
        return false;
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitFor(For &o)
{
    For *obj2 = static_cast<For *>(_objToCompare);

    if (!o.getName().empty() || !obj2->getName().empty())
        if (o.getName() != obj2->getName())
            return false;
    if (!_equalsChildren(o.initDeclarations, obj2->initDeclarations))
        return false;
    if (!_equalsChildren(o.initValues, obj2->initValues))
        return false;
    if (!_equalsChildren(o.getCondition(), obj2->getCondition()))
        return false;
    if (!_equalsChildren(o.stepActions, obj2->stepActions))
        return false;
    if (!_equalsChildren(o.forActions, obj2->forActions))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitForGenerate(ForGenerate &o)
{
    ForGenerate *obj2 = static_cast<ForGenerate *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.declarations, obj2->declarations))
        return false;
    if (!_equalsChildren(o.stateTables, obj2->stateTables))
        return false;
    if (!_equalsChildren(o.getGlobalAction(), obj2->getGlobalAction()))
        return false;
    if (!_equalsChildren(o.generates, obj2->generates))
        return false;
    if (!_equalsChildren(o.instances, obj2->instances))
        return false;
    if (!_equalsChildren(o.initDeclarations, obj2->initDeclarations))
        return false;
    if (!_equalsChildren(o.initValues, obj2->initValues))
        return false;
    if (!_equalsChildren(o.getCondition(), obj2->getCondition()))
        return false;
    if (!_equalsChildren(o.stepActions, obj2->stepActions))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitFunction(Function &o)
{
    Function *obj2 = static_cast<Function *>(_objToCompare);
    bool restore   = _isInSignature;
    _isInSignature = _options.skipDeclarationBodies;

    bool ret = true;
    if (o.getName() != obj2->getName())
        ret = false;
    else if (o.getKind() != obj2->getKind())
        ret = false;
    else if (o.isStandard() != obj2->isStandard())
        ret = false;
    else if (!_equalsChildren(o.templateParameters, obj2->templateParameters))
        ret = false;
    else if (!_equalsChildren(o.getType(), obj2->getType()))
        ret = false;
    else if (!_equalsChildren(o.parameters, obj2->parameters))
        ret = false;
    else if (!_options.skipDeclarationBodies) {
        if (!_equalsChildren(o.getStateTable(), obj2->getStateTable()))
            ret = false;
    }
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    _isInSignature = restore;
    return ret;
}

int HifEqualsVisitor::visitGlobalAction(GlobalAction &o)
{
    GlobalAction *obj2 = static_cast<GlobalAction *>(_objToCompare);

    if (!_equalsChildren(o.actions, obj2->actions))
        return false;
    return true;
}

int HifEqualsVisitor::visitIfGenerate(IfGenerate &o)
{
    IfGenerate *obj2 = static_cast<IfGenerate *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.getCondition(), obj2->getCondition()))
        return false;
    if (!_equalsChildren(o.declarations, obj2->declarations))
        return false;
    if (!_equalsChildren(o.stateTables, obj2->stateTables))
        return false;
    if (!_equalsChildren(o.getGlobalAction(), obj2->getGlobalAction()))
        return false;
    if (!_equalsChildren(o.generates, obj2->generates))
        return false;
    if (!_equalsChildren(o.instances, obj2->instances))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitEntity(Entity &o)
{
    Entity *obj2 = static_cast<Entity *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.parameters, obj2->parameters))
        return false;

    if (_isViewInterfaceCheck) {
        BList<Port> sorted1;
        hif::copy(o.ports, sorted1);
        BList<Port> sorted2;
        hif::copy(obj2->ports, sorted2);

        sorted1.sort(comparePorts);
        sorted2.sort(comparePorts);
        return _equalsChildren(sorted1, sorted2);
    } else if (!_equalsChildren(o.ports, obj2->ports))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitInstance(Instance &o)
{
    Instance *obj2 = static_cast<Instance *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.getReferencedType(), obj2->getReferencedType()))
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsChildren(o.portAssigns, obj2->portAssigns))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return _equalsSymbolDeclarations(&o, obj2);
}

int HifEqualsVisitor::visitInt(Int &o)
{
    Int *obj2 = static_cast<Int *>(_objToCompare);

    if (_options.handleConstexprTypes && (o.isConstexpr() || obj2->isConstexpr())) {
        // Nothing to check with const_expr int node
    } else {
        if (!_equalsSpans(o.getSpan(), obj2->getSpan()))
            return false;
        if (_options.checkConstexprFlag)
            if (o.isConstexpr() != obj2->isConstexpr())
                return false;
        if (_options.checkSignedFlag)
            if (o.isSigned() != obj2->isSigned())
                return false;
        if (_options.checkTypeVariantField)
            if (o.getTypeVariant() != obj2->getTypeVariant())
                return false;
    }
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitIntValue(IntValue &o)
{
    IntValue *obj2 = static_cast<IntValue *>(_objToCompare);

    if (o.getValue() != obj2->getValue())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitLibrary(Library &o)
{
    Library *obj2 = static_cast<Library *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.getFilename() != obj2->getFilename())
        return false;
    if (o.isStandard() != obj2->isStandard())
        return false;
    if (o.isSystem() != obj2->isSystem())
        return false;
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsInstance(o.getInstance(), obj2->getInstance()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return _equalsSymbolDeclarations(&o, obj2);
}

int HifEqualsVisitor::visitLibraryDef(LibraryDef &o)
{
    LibraryDef *obj2 = static_cast<LibraryDef *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.getLanguageID() != obj2->getLanguageID())
        return false;
    if (o.isStandard() != obj2->isStandard())
        return false;
    if (o.hasCLinkage() != obj2->hasCLinkage())
        return false;
    if (!_equalsChildren(o.libraries, obj2->libraries))
        return false;
    if (!_equalsChildren(o.declarations, obj2->declarations))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitMember(Member &o)
{
    Member *obj2 = static_cast<Member *>(_objToCompare);

    if (!_equalsChildren(o.getPrefix(), obj2->getPrefix()))
        return false;
    if (!_equalsChildren(o.getIndex(), obj2->getIndex()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}
int HifEqualsVisitor::visitNull(Null &o)
{
    Null *obj2 = static_cast<Null *>(_objToCompare);
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitIdentifier(Identifier &o)
{
    Identifier *obj2 = static_cast<Identifier *>(_objToCompare);

    if (!_options.skipDeclarationBodies || !_isInSignature)
        if (o.getName() != obj2->getName())
            return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return _equalsSymbolDeclarations(&o, obj2);
}

int HifEqualsVisitor::visitContinue(Continue &o)
{
    Continue *obj2 = static_cast<Continue *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitTransition(Transition &o)
{
    Transition *obj2 = static_cast<Transition *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.getPriority() != obj2->getPriority())
        return false;
    if (o.getEnablingOrCondition() != obj2->getEnablingOrCondition())
        return false;
    if (!_equalsChildren(o.enablingLabelList, obj2->enablingLabelList))
        return false;
    if (!_equalsChildren(o.enablingList, obj2->enablingList))
        return false;
    if (!_equalsChildren(o.updateLabelList, obj2->updateLabelList))
        return false;
    if (!_equalsChildren(o.updateList, obj2->updateList))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitParameter(Parameter &o)
{
    Parameter *obj2 = static_cast<Parameter *>(_objToCompare);

    if (!_options.skipDeclarationBodies)
        if (o.getName() != obj2->getName())
            return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (o.getDirection() != obj2->getDirection())
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsDeclarationRangeConstraint(o.getRange(), obj2->getRange()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitParameterAssign(ParameterAssign &o)
{
    ParameterAssign *obj2 = static_cast<ParameterAssign *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.getDirection() != obj2->getDirection())
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return _equalsSymbolDeclarations(&o, obj2);
}

int HifEqualsVisitor::visitProcedureCall(ProcedureCall &o)
{
    ProcedureCall *obj2 = static_cast<ProcedureCall *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.templateParameterAssigns, obj2->templateParameterAssigns))
        return false;
    if (!_equalsChildren(o.parameterAssigns, obj2->parameterAssigns))
        return false;
    if (!_equalsChildren(o.getInstance(), obj2->getInstance()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return _equalsSymbolDeclarations(&o, obj2);
}

int HifEqualsVisitor::visitPointer(Pointer &o)
{
    Pointer *obj2 = static_cast<Pointer *>(_objToCompare);

    if (!_equalsCheckInnerType(o.getType(), obj2->getType()))
        return false;

    // TODO: check
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitPort(Port &o)
{
    Port *obj2 = static_cast<Port *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (o.getDirection() != obj2->getDirection())
        return false;
    if (o.isWrapper() != obj2->isWrapper())
        return false;
    if (!_isViewInterfaceCheck) {
        if (!_equalsChildren(o.getValue(), obj2->getValue()))
            return false;
    }
    if (!_equalsDeclarationRangeConstraint(o.getRange(), obj2->getRange()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitPortAssign(PortAssign &o)
{
    PortAssign *obj2 = static_cast<PortAssign *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.getDirection() != obj2->getDirection())
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsChildren(o.getPartialBind(), obj2->getPartialBind()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return _equalsSymbolDeclarations(&o, obj2);
}

int HifEqualsVisitor::visitProcedure(Procedure &o)
{
    Procedure *obj2 = static_cast<Procedure *>(_objToCompare);
    bool restore    = _isInSignature;
    _isInSignature  = _options.skipDeclarationBodies;

    bool ret = true;
    if (o.getName() != obj2->getName())
        ret = false;
    else if (o.getKind() != obj2->getKind())
        ret = false;
    else if (o.isStandard() != obj2->isStandard())
        ret = false;
    else if (!_equalsChildren(o.templateParameters, obj2->templateParameters))
        ret = false;
    else if (!_equalsChildren(o.parameters, obj2->parameters))
        ret = false;
    else if (!_options.skipDeclarationBodies) {
        if (!_equalsChildren(o.getStateTable(), obj2->getStateTable()))
            ret = false;
    }
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    _isInSignature = restore;
    return ret;
}

int HifEqualsVisitor::visitRange(Range &o)
{
    Range *obj2 = static_cast<Range *>(_objToCompare);

    if (_options.checkSpanDirection && o.getDirection() != obj2->getDirection())
        return false;
    if (!_equalsChildren(hif::rangeGetMinBound(&o), hif::rangeGetMinBound(obj2)))
        return false;
    if (!_equalsChildren(hif::rangeGetMaxBound(&o), hif::rangeGetMaxBound(obj2)))
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitReal(Real &o)
{
    Real *obj2 = static_cast<Real *>(_objToCompare);

    if (_options.handleConstexprTypes && (o.isConstexpr() || obj2->isConstexpr())) {
        // nothing to check
    } else {
        if (!_equalsSpans(o.getSpan(), obj2->getSpan()))
            return false;
        if (_options.checkConstexprFlag)
            if (o.isConstexpr() != obj2->isConstexpr())
                return false;
        if (_options.checkTypeVariantField)
            if (o.getTypeVariant() != obj2->getTypeVariant())
                return false;
    }
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitRealValue(RealValue &o)
{
    RealValue *obj2 = static_cast<RealValue *>(_objToCompare);

    if (o.getValue() < obj2->getValue() || o.getValue() > obj2->getValue())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitRecord(Record &o)
{
    Record *obj2 = static_cast<Record *>(_objToCompare);

    if (_options.checkConstexprFlag)
        if (o.isConstexpr() != obj2->isConstexpr())
            return false;
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (o.isPacked() != obj2->isPacked())
        return false;
    if (o.isUnion() != obj2->isUnion())
        return false;
    if (!_equalsChildren(o.fields, obj2->fields))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitRecordValue(RecordValue &o)
{
    RecordValue *obj2 = static_cast<RecordValue *>(_objToCompare);

    if (!_equalsChildren(o.alts, obj2->alts))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitRecordValueAlt(RecordValueAlt &o)
{
    RecordValueAlt *obj2 = static_cast<RecordValueAlt *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitReference(Reference &o)
{
    Reference *obj2 = static_cast<Reference *>(_objToCompare);

    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;

    // TODO: check
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitReturn(Return &o)
{
    Return *obj2 = static_cast<Return *>(_objToCompare);

    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitSignal(Signal &o)
{
    Signal *obj2 = static_cast<Signal *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.isStandard() != obj2->isStandard())
        return false;
    if (o.isWrapper() != obj2->isWrapper())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsDeclarationRangeConstraint(o.getRange(), obj2->getRange()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitSigned(Signed &o)
{
    Signed *obj2 = static_cast<Signed *>(_objToCompare);

    if (!_equalsSpans(o.getSpan(), obj2->getSpan()))
        return false;
    if (_options.checkConstexprFlag)
        if (o.isConstexpr() != obj2->isConstexpr())
            return false;
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitSlice(Slice &o)
{
    Slice *obj2 = static_cast<Slice *>(_objToCompare);

    if (!_equalsChildren(o.getPrefix(), obj2->getPrefix()))
        return false;
    if (_options.checkSpans)
        if (!_equalsChildren(o.getSpan(), obj2->getSpan()))
            return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitState(State &o)
{
    State *obj2 = static_cast<State *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.isAtomic() != obj2->isAtomic())
        return false;
    if (o.getPriority() != obj2->getPriority())
        return false;
    if (!_equalsChildren(o.actions, obj2->actions))
        return false;
    if (!_equalsChildren(o.invariants, obj2->invariants))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitStateTable(StateTable &o)
{
    StateTable *obj2 = static_cast<StateTable *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.getFlavour() != obj2->getFlavour())
        return false;
    if (o.getDontInitialize() != obj2->getDontInitialize())
        return false;
    if (o.getEntryStateName() != obj2->getEntryStateName())
        return false;
    if (o.isStandard() != obj2->isStandard())
        return false;
    if (!_equalsChildren(o.declarations, obj2->declarations))
        return false;
    if (!_equalsChildren(o.sensitivity, obj2->sensitivity))
        return false;
    if (!_equalsChildren(o.sensitivityPos, obj2->sensitivityPos))
        return false;
    if (!_equalsChildren(o.sensitivityNeg, obj2->sensitivityNeg))
        return false;
    if (!_equalsChildren(o.states, obj2->states))
        return false;
    if (!_equalsChildren(o.edges, obj2->edges))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitSwitch(Switch &o)
{
    Switch *obj2 = static_cast<Switch *>(_objToCompare);

    if (o.getCaseSemantics() != obj2->getCaseSemantics())
        return false;
    if (!_equalsChildren(o.getCondition(), obj2->getCondition()))
        return false;
    if (!_equalsChildren(o.alts, obj2->alts))
        return false;
    if (!_equalsChildren(o.defaults, obj2->defaults))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitString(String &o)
{
    String *obj2 = static_cast<String *>(_objToCompare);

    if (_options.handleConstexprTypes && (o.isConstexpr() || obj2->isConstexpr())) {
        // ntc
    } else {
        if (_options.checkConstexprFlag)
            if (o.isConstexpr() != obj2->isConstexpr())
                return false;
        if (_options.checkTypeVariantField)
            if (o.getTypeVariant() != obj2->getTypeVariant())
                return false;
        if (_options.checkStringSpan) {
            if (!_equalsChildren(o.getSpanInformation(), obj2->getSpanInformation()))
                return false;
        }
    }
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitSwitchAlt(SwitchAlt &o)
{
    SwitchAlt *obj2 = static_cast<SwitchAlt *>(_objToCompare);

    if (!_equalsChildren(o.conditions, obj2->conditions))
        return false;
    if (!_equalsChildren(o.actions, obj2->actions))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitStringValue(StringValue &o)
{
    StringValue *obj2 = static_cast<StringValue *>(_objToCompare);

    if (o.getValue() != obj2->getValue())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitTime(Time &o)
{
    Time *obj2 = static_cast<Time *>(_objToCompare);

    // TODO: check
    if (_options.checkConstexprFlag)
        if (o.isConstexpr() != obj2->isConstexpr())
            return false;
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitTimeValue(TimeValue &o)
{
    TimeValue *obj2 = static_cast<TimeValue *>(_objToCompare);

    if (o.getUnit() != obj2->getUnit())
        return false;
    if (fabs(o.getValue() - obj2->getValue()) < 1e-7)
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitTypeDef(TypeDef &o)
{
    TypeDef *obj2 = static_cast<TypeDef *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (_options.handleExternalsTypedefs && (o.isExternal() || obj2->isExternal())) {
        return true;
    }

    bool restore   = _isInSignature;
    _isInSignature = _options.skipDeclarationBodies;
    bool ret       = true;

    if (!_equalsChildren(o.templateParameters, obj2->templateParameters))
        ret = false;
    else if (o.isOpaque() != obj2->isOpaque())
        ret = false;
    else if (o.isStandard() != obj2->isStandard())
        ret = false;
    else if (o.isExternal() != obj2->isExternal())
        ret = false;
    else if (!_options.skipDeclarationBodies && !_equalsChildren(o.getType(), obj2->getType()))
        ret = false;
    else if (_options.checkSpans) {
        if (!_equalsChildren(o.getRange(), obj2->getRange()))
            ret = false;
    }

    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    _isInSignature = restore;
    return ret;
}

int HifEqualsVisitor::visitTypeReference(TypeReference &o)
{
    TypeReference *obj2 = static_cast<TypeReference *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsChildren(o.templateParameterAssigns, obj2->templateParameterAssigns))
        return false;
    if (_options.checkSpans)
        if (!_equalsChildren(o.ranges, obj2->ranges))
            return false;
    if (!_equalsInstance(o.getInstance(), obj2->getInstance()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return _equalsSymbolDeclarations(&o, obj2);
}

int HifEqualsVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    TypeTPAssign *obj2 = static_cast<TypeTPAssign *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return _equalsSymbolDeclarations(&o, obj2);
}

int HifEqualsVisitor::visitTypeTP(TypeTP &o)
{
    TypeTP *obj2 = static_cast<TypeTP *>(_objToCompare);

    if (!_options.skipDeclarationBodies)
        if (o.getName() != obj2->getName())
            return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitUnsigned(Unsigned &o)
{
    Unsigned *obj2 = static_cast<Unsigned *>(_objToCompare);

    if (!_equalsSpans(o.getSpan(), obj2->getSpan()))
        return false;
    if (_options.checkConstexprFlag)
        if (o.isConstexpr() != obj2->isConstexpr())
            return false;
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitValueStatement(ValueStatement &o)
{
    ValueStatement *obj2 = static_cast<ValueStatement *>(_objToCompare);

    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    ValueTPAssign *obj2 = static_cast<ValueTPAssign *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return _equalsSymbolDeclarations(&o, obj2);
}

int HifEqualsVisitor::visitValueTP(ValueTP &o)
{
    ValueTP *obj2 = static_cast<ValueTP *>(_objToCompare);

    if (!_options.skipDeclarationBodies)
        if (o.getName() != obj2->getName())
            return false;
    if (o.isCompileTimeConstant() != obj2->isCompileTimeConstant())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsDeclarationRangeConstraint(o.getRange(), obj2->getRange()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitVariable(Variable &o)
{
    Variable *obj2 = static_cast<Variable *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.isInstance() != obj2->isInstance())
        return false;
    if (o.isStandard() != obj2->isStandard())
        return false;
    if (!_equalsChildren(o.getType(), obj2->getType()))
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsDeclarationRangeConstraint(o.getRange(), obj2->getRange()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitView(View &o)
{
    View *obj2 = static_cast<View *>(_objToCompare);

    bool areComponents         = (o.getContents() == nullptr) || (obj2->getContents() == nullptr);
    bool isComponentComparison = _options.skipDeclarationBodies && areComponents;
    if (!isComponentComparison) {
        if (o.getName() != obj2->getName())
            return false;
        if (o.isStandard() != obj2->isStandard())
            return false;
        if (o.getLanguageID() != obj2->getLanguageID())
            return false;
        if (o.getFilename() != obj2->getFilename())
            return false;
    }

    bool restoreIsSignature        = _isInSignature;
    bool restoreView               = _isViewSignature;
    bool restoreCheckOnlyInterface = _isViewInterfaceCheck;
    _isInSignature                 = _options.skipDeclarationBodies;
    _isViewSignature               = _options.skipDeclarationBodies;
    _isViewInterfaceCheck          = _options.skipViewContents || isComponentComparison;
    bool ret                       = true;
    if (!_options.skipViewContents)
        if (!_equalsChildren(o.templateParameters, obj2->templateParameters))
            ret = false;
    if (!_equalsChildren(o.getEntity(), obj2->getEntity()))
        ret = false;
    _isInSignature        = restoreIsSignature;
    _isViewSignature      = restoreView;
    _isViewInterfaceCheck = restoreCheckOnlyInterface;
    if (!ret)
        return false;
    if (!_options.skipDeclarationBodies && !_options.skipViewContents) {
        if (!_equalsChildren(o.libraries, obj2->libraries))
            return false;
        if (!_equalsChildren(o.inheritances, obj2->inheritances))
            return false;
        if (!_equalsChildren(o.declarations, obj2->declarations))
            return false;
        if (!_equalsChildren(o.getContents(), obj2->getContents()))
            return false;
    }
    if (!_equalsProperties(&o, obj2))
        return false;
    if (!_equalsAdditionalKeywords(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitViewReference(ViewReference &o)
{
    ViewReference *obj2 = static_cast<ViewReference *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.getDesignUnit() != obj2->getDesignUnit())
        return false;
    if (_options.checkTypeVariantField)
        if (o.getTypeVariant() != obj2->getTypeVariant())
            return false;
    if (!_equalsInstance(o.getInstance(), obj2->getInstance()))
        return false;
    if (!_equalsChildren(o.templateParameterAssigns, obj2->templateParameterAssigns))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return _equalsSymbolDeclarations(&o, obj2);
}

int HifEqualsVisitor::visitWait(Wait &o)
{
    Wait *obj2 = static_cast<Wait *>(_objToCompare);

    if (!_equalsChildren(o.getRepetitions(), obj2->getRepetitions()))
        return false;
    if (!_equalsChildren(o.getTime(), obj2->getTime()))
        return false;
    if (!_equalsChildren(o.getCondition(), obj2->getCondition()))
        return false;
    if (!_equalsChildren(o.sensitivity, obj2->sensitivity))
        return false;
    if (!_equalsChildren(o.sensitivityPos, obj2->sensitivityPos))
        return false;
    if (!_equalsChildren(o.sensitivityNeg, obj2->sensitivityNeg))
        return false;
    if (!_equalsChildren(o.actions, obj2->actions))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitWhen(When &o)
{
    When *obj2 = static_cast<When *>(_objToCompare);

    if (o.isLogicTernary() != obj2->isLogicTernary())
        return false;
    if (!_equalsChildren(o.alts, obj2->alts))
        return false;
    if (!_equalsChildren(o.getDefault(), obj2->getDefault()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitWhenAlt(WhenAlt &o)
{
    WhenAlt *obj2 = static_cast<WhenAlt *>(_objToCompare);

    if (!_equalsChildren(o.getCondition(), obj2->getCondition()))
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitWhile(While &o)
{
    While *obj2 = static_cast<While *>(_objToCompare);

    if (o.getName() != obj2->getName())
        return false;
    if (o.isDoWhile() != obj2->isDoWhile())
        return false;
    if (!_equalsChildren(o.getCondition(), obj2->getCondition()))
        return false;
    if (!_equalsChildren(o.actions, obj2->actions))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitWith(With &o)
{
    With *obj2 = static_cast<With *>(_objToCompare);

    if (o.getCaseSemantics() != obj2->getCaseSemantics())
        return false;
    if (!_equalsChildren(o.getCondition(), obj2->getCondition()))
        return false;
    if (!_equalsChildren(o.alts, obj2->alts))
        return false;
    if (!_equalsChildren(o.getDefault(), obj2->getDefault()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

int HifEqualsVisitor::visitWithAlt(WithAlt &o)
{
    WithAlt *obj2 = static_cast<WithAlt *>(_objToCompare);

    if (!_equalsChildren(o.conditions, obj2->conditions))
        return false;
    if (!_equalsChildren(o.getValue(), obj2->getValue()))
        return false;
    if (!_equalsProperties(&o, obj2))
        return false;
    return true;
}

} // namespace

EqualsOptions::EqualsOptions()
    : checkOnlyTypes(false)
    , checkOnlyNames(false)
    , checkOnlySymbolsDeclarations(false)
    , checkSpans(true)
    , checkInnerTypeOfComposite(true)
    , checkDeclarationRangeConstraint(true)
    , checkFieldsInitialvalue(true)
    , checkReferencedInstance(true)
    , checkConstexprFlag(true)
    , checkLogicFlag(true)
    , checkSignedFlag(true)
    , checkResolvedFlag(true)
    , checkTypeVariantField(true)
    , checkStringSpan(true)
    , checkSpanDirection(true)
    , handleConstexprTypes(false)
    , handleVectorTypes(false)
    , handleExternalsTypedefs(false)
    , skipReferences(true)
    , skipChilden(false)
    , skipNullBranches(false)
    , skipDeclarationBodies(false)
    , skipViewContents(false)
    , assureSameSymbolDeclarations(false)
    , assureSameProperties(false)
    , assureSameAdditionalKeywords(false)
    , sem(nullptr)
{
    // ntd
}
EqualsOptions::~EqualsOptions()
{
    // ntd
}

EqualsOptions::EqualsOptions(const EqualsOptions &o)
    : checkOnlyTypes(o.checkOnlyTypes)
    , checkOnlyNames(o.checkOnlyNames)
    , checkOnlySymbolsDeclarations(o.checkOnlySymbolsDeclarations)
    , checkSpans(o.checkSpans)
    , checkInnerTypeOfComposite(o.checkInnerTypeOfComposite)
    , checkDeclarationRangeConstraint(o.checkDeclarationRangeConstraint)
    , checkFieldsInitialvalue(o.checkFieldsInitialvalue)
    , checkReferencedInstance(o.checkReferencedInstance)
    , checkConstexprFlag(o.checkConstexprFlag)
    , checkLogicFlag(o.checkLogicFlag)
    , checkSignedFlag(o.checkSignedFlag)
    , checkResolvedFlag(o.checkResolvedFlag)
    , checkTypeVariantField(o.checkTypeVariantField)
    , checkStringSpan(o.checkStringSpan)
    , checkSpanDirection(o.checkSpanDirection)
    , handleConstexprTypes(o.handleConstexprTypes)
    , handleVectorTypes(o.handleVectorTypes)
    , handleExternalsTypedefs(o.handleExternalsTypedefs)
    , skipReferences(o.skipReferences)
    , skipChilden(o.skipChilden)
    , skipNullBranches(o.skipNullBranches)
    , skipDeclarationBodies(o.skipDeclarationBodies)
    , skipViewContents(o.skipViewContents)
    , assureSameSymbolDeclarations(o.assureSameSymbolDeclarations)
    , assureSameProperties(o.assureSameProperties)
    , assureSameAdditionalKeywords(o.assureSameAdditionalKeywords)
    , sem(o.sem)
{
    // ntd
}

EqualsOptions &EqualsOptions::operator=(EqualsOptions o)
{
    this->swap(o);
    return *this;
}

void EqualsOptions::swap(EqualsOptions &o)
{
    std::swap(checkOnlyTypes, o.checkOnlyTypes);
    std::swap(checkOnlyNames, o.checkOnlyNames);
    std::swap(checkOnlySymbolsDeclarations, o.checkOnlySymbolsDeclarations);
    std::swap(checkSpans, o.checkSpans);
    std::swap(checkInnerTypeOfComposite, o.checkInnerTypeOfComposite);
    std::swap(checkDeclarationRangeConstraint, o.checkDeclarationRangeConstraint);
    std::swap(checkFieldsInitialvalue, o.checkFieldsInitialvalue);
    std::swap(checkReferencedInstance, o.checkReferencedInstance);
    std::swap(checkConstexprFlag, o.checkConstexprFlag);
    std::swap(checkLogicFlag, o.checkLogicFlag);
    std::swap(checkSignedFlag, o.checkSignedFlag);
    std::swap(checkResolvedFlag, o.checkResolvedFlag);
    std::swap(checkTypeVariantField, o.checkTypeVariantField);
    std::swap(checkStringSpan, o.checkStringSpan);
    std::swap(checkSpanDirection, o.checkSpanDirection);
    std::swap(handleConstexprTypes, o.handleConstexprTypes);
    std::swap(handleVectorTypes, o.handleVectorTypes);
    std::swap(handleExternalsTypedefs, o.handleExternalsTypedefs);
    std::swap(skipReferences, o.skipReferences);
    std::swap(skipChilden, o.skipChilden);
    std::swap(skipNullBranches, o.skipNullBranches);
    std::swap(skipDeclarationBodies, o.skipDeclarationBodies);
    std::swap(skipViewContents, o.skipViewContents);
    std::swap(assureSameSymbolDeclarations, o.assureSameSymbolDeclarations);
    std::swap(assureSameProperties, o.assureSameProperties);
    std::swap(assureSameAdditionalKeywords, o.assureSameAdditionalKeywords);
    std::swap(sem, o.sem);
}

bool equals(Object *obj1, Object *obj2, const EqualsOptions &options)
{
    HifEqualsVisitor v(options);
    return v._equals(obj1, obj2);
}

} // namespace hif
