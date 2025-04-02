/// @file ILanguageSemantics.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <set>

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"
#include "hif/semantics/standardization.hpp"

#if defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

namespace hif
{
namespace semantics
{
namespace /* anon */
{

struct LeafInfos {
    Range *range;
    bool operator<(const LeafInfos &other) const;

    LeafInfos();
    ~LeafInfos();
    LeafInfos(const LeafInfos &other);
    LeafInfos &operator=(const LeafInfos &other);
};
LeafInfos::LeafInfos()
    : range(nullptr)
{
    // ntd
}

LeafInfos::~LeafInfos()
{
    // ntd
}

LeafInfos::LeafInfos(const LeafInfos &other)
    : range(other.range)
{
    // ntd
}

LeafInfos &LeafInfos::operator=(const LeafInfos &other)
{
    if (this == &other)
        return *this;

    range = other.range;

    return *this;
}
bool LeafInfos::operator<(const LeafInfos &other) const { return hif::compare(range, other.range) < 0; }
typedef std::set<LeafInfos> Infos;
Object *_getNearestIndependentParent(Object *o, Object *child = nullptr)
{
    bool recall = false;

    if (dynamic_cast<Expression *>(o) != nullptr) {
        Expression *e      = static_cast<Expression *>(o);
        hif::Operator oper = e->getOperator();
        if (e->getValue2() == nullptr) {
            recall = (oper == op_bnot) || (oper == op_plus) || (oper == op_minus);
        } else {
            if (operatorIsShift(oper) || operatorIsRotate(oper) || (oper == op_pow)) {
                // Only if left
                recall = (e->getValue1() == child);
            } else if (operatorIsArithmetic(oper) || operatorIsBitwise(oper)) {
                assert(
                    !operatorIsArithmetic(oper) || oper == op_plus || oper == op_minus || oper == op_mult ||
                    oper == op_div || oper == op_mod);

                recall = true;
            }
        }
    } else if (dynamic_cast<When *>(o) != nullptr) {
        recall = true;

    } else if (dynamic_cast<WhenAlt *>(o) != nullptr) {
        WhenAlt *w = static_cast<WhenAlt *>(o);
        recall     = (w->getCondition() != child);

    } else if (dynamic_cast<With *>(o) != nullptr) {
        With *w = static_cast<With *>(o);
        recall  = (w->getCondition() != child);
    } else if (dynamic_cast<WithAlt *>(o) != nullptr) {
        WithAlt *w = static_cast<WithAlt *>(o);
        recall     = (w->getValue() == child);
    } else if (dynamic_cast<Assign *>(o) != nullptr) {
        Assign *a = static_cast<Assign *>(o);
        if (a->getRightHandSide() == child)
            return o;
        else
            return child;
    } else if (dynamic_cast<Aggregate *>(o) != nullptr) {
        Aggregate *a = static_cast<Aggregate *>(o);
        recall       = (a->getOthers() == child);
    } else if (dynamic_cast<AggregateAlt *>(o) != nullptr) {
        AggregateAlt *a = static_cast<AggregateAlt *>(o);
        recall          = (a->getValue() == child);
    }

    if (recall) {
        Object *p = o->getParent();
        if (p == nullptr) {
            //assert(false);
            return o;
        }

        return _getNearestIndependentParent(p, o);
    }

    if (child == nullptr)
        return o;
    return child;
}
// ///////////////////////////////////////////////////////////////////
// LeafInfosVisitor
// ///////////////////////////////////////////////////////////////////
class LeafInfosVisitor : public GuideVisitor
{
public:
    LeafInfosVisitor(Infos &leaves, ILanguageSemantics *sem);
    ~LeafInfosVisitor();

    int visitAggregate(Aggregate &o);
    int visitAggregateAlt(AggregateAlt &o);
    int visitCast(Cast &o);
    int visitBitValue(BitValue &o);
    int visitBitvectorValue(BitvectorValue &o);
    int visitBoolValue(BoolValue &o);
    int visitCharValue(CharValue &o);
    int visitFunctionCall(FunctionCall &o);
    int visitIntValue(IntValue &o);
    int visitRealValue(RealValue &o);
    int visitStringValue(StringValue &o);
    int visitTimeValue(TimeValue &o);
    int visitExpression(Expression &o);
    int visitIdentifier(Identifier &o);
    int visitInstance(Instance &o);
    int visitFieldReference(FieldReference &o);
    int visitMember(Member &o);
    int visitSlice(Slice &o);
    int visitRange(Range &o);
    int visitRecordValue(RecordValue &o);
    int visitWhen(When &o);
    int visitWhenAlt(WhenAlt &o);
    int visitWith(With &o);
    int visitWithAlt(WithAlt &o);

private:
    Infos *_leaves;
    ILanguageSemantics *_sem;

    void _collectRange(Value *o);

    LeafInfosVisitor(const LeafInfosVisitor &);
    LeafInfosVisitor &operator=(const LeafInfosVisitor &);
};

LeafInfosVisitor::LeafInfosVisitor(Infos &leaves, ILanguageSemantics *sem)
    : GuideVisitor()
    , _leaves(&leaves)
    , _sem(sem)
{
    // ntd
}

LeafInfosVisitor::~LeafInfosVisitor()
{
    // ntd
}

void LeafInfosVisitor::_collectRange(Value *o)
{
    Type *t = getSemanticType(o, _sem);
    if (t == nullptr)
        return;

    Range *r = typeGetSpan(t, _sem);
    if (r == nullptr)
        return;

    LeafInfos i;
    i.range = r;
    _leaves->insert(i);
}

int LeafInfosVisitor::visitAggregate(Aggregate &o)
{
    GuideVisitor::visitAggregate(o);
    return 0;
}

int LeafInfosVisitor::visitAggregateAlt(AggregateAlt &o)
{
    //GuideVisitor::visitAggregateAlt(o);
    if (o.getValue() != nullptr)
        o.getValue()->acceptVisitor(*this);
    return 0;
}

int LeafInfosVisitor::visitCast(Cast &o)
{
    //GuideVisitor::visitCast(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitBitValue(BitValue &o)
{
    //GuideVisitor::visitBitValue(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitBitvectorValue(BitvectorValue &o)
{
    //GuideVisitor::visitBitvectorValue(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitBoolValue(BoolValue &o)
{
    //GuideVisitor::visitBoolValue(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitCharValue(CharValue &o)
{
    //GuideVisitor::visitCharValue(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitFunctionCall(FunctionCall &o)
{
    //GuideVisitor::visitFunctionCall(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitIntValue(IntValue &o)
{
    //GuideVisitor::visitIntValue(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitRealValue(RealValue &o)
{
    //GuideVisitor::visitRealValue(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitStringValue(StringValue &o)
{
    //GuideVisitor::visitStringValue(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitTimeValue(TimeValue &o)
{
    //GuideVisitor::visitTimeValue(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitExpression(Expression &o)
{
    //GuideVisitor::visitExpression(o);
    hif::Operator oper = o.getOperator();
    if (o.getValue2() == nullptr) {
        if ((oper == op_bnot) || (oper == op_plus) || (oper == op_minus)) {
            GuideVisitor::visitExpression(o);
            return 0;
        }
    } else {
        if (operatorIsShift(oper) || operatorIsRotate(oper) || (oper == op_pow)) {
            if (o.getValue1() != nullptr) {
                o.getValue1()->acceptVisitor(*this);
                return 0;
            }
        } else if (operatorIsArithmetic(oper) || operatorIsBitwise(oper)) {
            GuideVisitor::visitExpression(o);
            return 0;
        } else if (operatorIsLogical(oper) || operatorIsRelational(oper)) {
            // Trick to avoid leak:
            // typeGetSpan() returns a static range on bit type!
            Bit b;
            Range *r = typeGetSpan(&b, _sem);
            if (r == nullptr)
                return 0;

            LeafInfos i;
            i.range = r;
            _leaves->insert(i);
            return 0;
        }
    }
    _collectRange(&o);

    return 0;
}

int LeafInfosVisitor::visitIdentifier(Identifier &o)
{
    //GuideVisitor::visitIdentifier(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitInstance(Instance &o)
{
    //GuideVisitor::visitInstance(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitFieldReference(FieldReference &o)
{
    //GuideVisitor::visitFieldReference(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitMember(Member &o)
{
    //GuideVisitor::visitMember(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitSlice(Slice &o)
{
    //GuideVisitor::visitSlice(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitRange(Range &)
{
    //GuideVisitor::visitRange(o);
    return 0;
}

int LeafInfosVisitor::visitRecordValue(RecordValue &o)
{
    //GuideVisitor::visitRecordValue(o);
    _collectRange(&o);
    return 0;
}

int LeafInfosVisitor::visitWhen(When &o)
{
    GuideVisitor::visitWhen(o);
    return 0;
}

int LeafInfosVisitor::visitWhenAlt(WhenAlt &o)
{
    //GuideVisitor::visitWhenAlt(o);
    if (o.getValue() != nullptr)
        o.getValue()->acceptVisitor(*this);
    return 0;
}

int LeafInfosVisitor::visitWith(With &o)
{
    //GuideVisitor::visitWith(o);
    visitList(o.alts);
    if (o.getDefault() != nullptr)
        o.getDefault()->acceptVisitor(*this);

    return 0;
}

int LeafInfosVisitor::visitWithAlt(WithAlt &o)
{
    //GuideVisitor::visitWithAlt(o);
    if (o.getValue() != nullptr)
        o.getValue()->acceptVisitor(*this);
    return 0;
}
void _getLeafInfos(Object *root, Infos &leaves, ILanguageSemantics *sem)
{
    messageAssert(root != nullptr, "Root is nullptr", nullptr, nullptr);

    LeafInfosVisitor v(leaves, sem);
    root->acceptVisitor(v);
}

} // namespace
ILanguageSemantics::ILanguageSemantics()
    : _factory(nullptr)
    , _hifFactory(nullptr)
    , _nameT(hif::NameTable::getInstance())
    , _strictChecking(true)
    , _forbiddenNames()
    , _semanticOptions()
    , _standardSymbols()
    , _standardFilenames()
    , _useNativeSemantics(false)
{
    _factory.setSemantics(this);
}
ILanguageSemantics::~ILanguageSemantics() {}

ILanguageSemantics::ExpressionTypeInfo::~ExpressionTypeInfo()
{
    delete returnedType;
    delete operationPrecision;
#ifndef NDEBUG
    returnedType       = nullptr;
    operationPrecision = nullptr;
#endif
}

ILanguageSemantics::ExpressionTypeInfo::ExpressionTypeInfo()
    : returnedType(nullptr)
    , operationPrecision(nullptr)
{
    //ntd
}

ILanguageSemantics::ExpressionTypeInfo::ExpressionTypeInfo(const ExpressionTypeInfo &t)
    : returnedType(nullptr)
    , operationPrecision(nullptr)
{
    hif::CopyOptions opt;
    //opt.copySemanticsTypes = true;
    returnedType       = hif::copy(t.returnedType, opt);
    operationPrecision = hif::copy(t.operationPrecision, opt);
}

ILanguageSemantics::ExpressionTypeInfo &ILanguageSemantics::ExpressionTypeInfo::operator=(const ExpressionTypeInfo &t)
{
    if (this == &t)
        return *this;

    delete this->returnedType;
    delete this->operationPrecision;

    hif::CopyOptions opt;
    //opt.copySemanticsTypes = true;

    this->returnedType       = hif::copy(t.returnedType, opt);
    this->operationPrecision = hif::copy(t.operationPrecision, opt);
    return *this;
}

ILanguageSemantics::SemanticOptions::SemanticOptions()
    : port_inNoInitialValue(false)
    , port_outInitialValue(false)
    , dataDeclaration_initialValue(false)
    , scopedType_insideTypedef(false)
    , int_bitfields(false)
    , designUnit_uniqueView(false)
    , for_implictIndex(false)
    , for_conditionType(RANGE_AND_EXPRESSION)
    , generates_isNoAllowed(false)
    , after_isNoAllowed(false)
    , with_isNoAllowed(false)
    , globact_isNoAllowed(false)
    , valueStatement_isNoAllowed(false)
    , case_isOnlyLiteral(false)
    , lang_signPortNoBitAccess(false)
    , lang_hasDontCare(false)
    , lang_has9logic(false)
    , waitWithActions(false)
    , lang_sortKind(hif::manipulation::SortMissingKind::NOTHING)
{
    // ntd
}

ILanguageSemantics::SemanticOptions::~SemanticOptions()
{
    // ntd
}

ILanguageSemantics::SemanticOptions::SemanticOptions(const SemanticOptions &o)
    : port_inNoInitialValue(o.port_inNoInitialValue)
    , port_outInitialValue(o.port_outInitialValue)
    , dataDeclaration_initialValue(o.dataDeclaration_initialValue)
    , scopedType_insideTypedef(o.scopedType_insideTypedef)
    , int_bitfields(o.int_bitfields)
    , designUnit_uniqueView(o.designUnit_uniqueView)
    , for_implictIndex(o.for_implictIndex)
    , for_conditionType(o.for_conditionType)
    , generates_isNoAllowed(o.generates_isNoAllowed)
    , after_isNoAllowed(o.after_isNoAllowed)
    , with_isNoAllowed(o.with_isNoAllowed)
    , globact_isNoAllowed(o.globact_isNoAllowed)
    , valueStatement_isNoAllowed(o.valueStatement_isNoAllowed)
    , case_isOnlyLiteral(o.case_isOnlyLiteral)
    , lang_signPortNoBitAccess(o.lang_signPortNoBitAccess)
    , lang_hasDontCare(o.lang_hasDontCare)
    , lang_has9logic(o.lang_has9logic)
    , waitWithActions(o.waitWithActions)
    , lang_sortKind(o.lang_sortKind)
{
    // ntd
}

ILanguageSemantics::SemanticOptions &ILanguageSemantics::SemanticOptions::operator=(const SemanticOptions &o)
{
    if (this == &o)
        return *this;

    port_inNoInitialValue        = o.port_inNoInitialValue;
    port_outInitialValue         = o.port_outInitialValue;
    dataDeclaration_initialValue = o.dataDeclaration_initialValue;
    scopedType_insideTypedef     = o.scopedType_insideTypedef;
    int_bitfields                = o.int_bitfields;
    designUnit_uniqueView        = o.designUnit_uniqueView;
    for_implictIndex             = o.for_implictIndex;
    for_conditionType            = o.for_conditionType;
    generates_isNoAllowed        = o.generates_isNoAllowed;
    after_isNoAllowed            = o.after_isNoAllowed;
    with_isNoAllowed             = o.with_isNoAllowed;
    globact_isNoAllowed          = o.globact_isNoAllowed;
    valueStatement_isNoAllowed   = o.valueStatement_isNoAllowed;
    case_isOnlyLiteral           = o.case_isOnlyLiteral;
    lang_signPortNoBitAccess     = o.lang_signPortNoBitAccess;
    lang_hasDontCare             = o.lang_hasDontCare;
    lang_has9logic               = o.lang_has9logic;
    waitWithActions              = o.waitWithActions;
    lang_sortKind                = o.lang_sortKind;

    return *this;
}

const ILanguageSemantics::SemanticOptions &ILanguageSemantics::getSemanticsOptions() const { return _semanticOptions; }

void ILanguageSemantics::setStrictTypeChecks(bool v) { _strictChecking = v; }

bool ILanguageSemantics::getStrictTypeChecks() const { return _strictChecking; }

Type *ILanguageSemantics::getMemberSemanticType(Member *m)
{
    // Retrieve the type
    Type *prefixType = hif::semantics::getSemanticType(m->getPrefix(), this);

    if (dynamic_cast<Reference *>(prefixType)) {
        // If it is a reference, get its type
        Reference *ref = static_cast<Reference *>(prefixType);
        prefixType     = ref->getType();
    }
    if (dynamic_cast<TypeReference *>(prefixType)) {
        // If it is a typeref, get the base type
        TypeReference *tr = static_cast<TypeReference *>(prefixType);
        prefixType        = getBaseType(tr, false, this, false);
    }

    // Now we have throw away typerefs. Get the type of the array
    if (dynamic_cast<Array *>(prefixType)) {
        // do not extract inner type
        Array *array = static_cast<Array *>(prefixType);
        hif::semantics::updateDeclarations(array->getType(), this);
        return hif::copy(array->getType());
    } else if (dynamic_cast<Bitvector *>(prefixType)) {
        Bitvector *decl = static_cast<Bitvector *>(prefixType);
        Bit type;
        type.setLogic(decl->isLogic());
        type.setResolved(decl->isResolved());
        type.setConstexpr(decl->isConstexpr());
        return getMapForType(&type);
    } else if (dynamic_cast<Bit *>(prefixType)) {
        Bit *decl = static_cast<Bit *>(prefixType);
        return hif::copy(decl);
    } else if (dynamic_cast<Bool *>(prefixType)) {
        Bool *decl = static_cast<Bool *>(prefixType);
        return hif::copy(decl);
    } else if (dynamic_cast<Signed *>(prefixType)) {
        Bit *type = new Bit();
        type->setLogic(true);
        type->setResolved(true);
        return type;
    } else if (dynamic_cast<Unsigned *>(prefixType)) {
        Bit *type = new Bit();
        type->setLogic(true);
        type->setResolved(true);
        return type;
    } else if (dynamic_cast<String *>(prefixType)) {
        Char *type = new Char();
        return type;
    }

    return nullptr;
}

Type *ILanguageSemantics::getSliceSemanticType(Slice *s)
{
    // If it is a typeref retrieve its declaration
    Type *prefixType = hif::semantics::getSemanticType(s->getPrefix(), this);
    if (dynamic_cast<Reference *>(prefixType)) {
        // If it is a reference, get its type
        Reference *ref = static_cast<Reference *>(prefixType);
        prefixType     = ref->getType();
    }
    if (dynamic_cast<TypeReference *>(prefixType) != nullptr) {
        // if it is a typeref, get the base type
        // Do not extract inner type (ref. design: vhdl/openCores/avs_aes hif2sc)
        TypeReference *tr = static_cast<TypeReference *>(prefixType);
        prefixType        = getBaseType(tr, false, this, false);
        if (prefixType == nullptr) {
            return nullptr;
        }
    }

    // Get the prefix range
    Range *prefixRange = typeGetSpan(prefixType, this);
    String *stringType = dynamic_cast<String *>(prefixType);
    if (stringType != nullptr) {
        // Special management for strings:
        hif::semantics::updateDeclarations(prefixType, this);
        String *resultType = hif::copy(stringType);
        hif::semantics::updateDeclarations(s->getSpan(), this);
        Range *span = hif::copy(s->getSpan());
        delete resultType->setSpanInformation(span);

        return resultType;
    } else if (prefixRange == nullptr) {
        return nullptr;
    }

    // Setting the result type
    hif::semantics::updateDeclarations(prefixType, this);
    Type *resultType = hif::copy(prefixType);
    hif::semantics::updateDeclarations(s->getSpan(), this);
    Range *span = hif::copy(s->getSpan());
    hif::typeSetSpan(resultType, span, this, true);

    return resultType;
}

bool ILanguageSemantics::isTypeAllowedAsReturn(Type *type) { return isTypeAllowed(type); }

bool ILanguageSemantics::canRemoveCastOnOperands(
    Expression *e,
    ExpressionTypeInfo &origInfo,
    ExpressionTypeInfo &simplifiedInfo,
    Type *castT1,
    Type *castT2,
    Type *subT1,
    Type *subT2,
    bool &canRemoveOnShift)
{
    bool isSafe = false;
    if (e->getOperator() == op_concat) {
        isSafe = _checkConcatCasts(e, castT1, castT2, subT1, subT2, origInfo, simplifiedInfo);
    } else if (hif::operatorIsRelational(e->getOperator())) {
        isSafe = _checkRelationalCasts(e, castT1, castT2, subT1, subT2, origInfo, simplifiedInfo);
    } else if (hif::operatorIsShift(e->getOperator())) {
        isSafe = _checkShiftCasts(e, castT1, castT2, subT1, subT2, origInfo, simplifiedInfo, canRemoveOnShift);
    } else if (hif::operatorIsBitwise(e->getOperator())) {
        isSafe = _checkBitwiseCasts(e, castT1, castT2, subT1, subT2, origInfo, simplifiedInfo);
    } else {
        isSafe = _checkArithmeticCasts(e, castT1, castT2, subT1, subT2, origInfo, simplifiedInfo);
    }

    return isSafe;
}

bool ILanguageSemantics::hasBitwiseOperationsOnBits(bool /*isLogic*/) const { return true; }

ILanguageSemantics *ILanguageSemantics::getInstance(SupportedLanguages lang)
{
    switch (lang) {
    case VHDL:
        return VHDLSemantics::getInstance();
    case Verilog:
        return VerilogSemantics::getInstance();
    case HIF:
        return HIFSemantics::getInstance();
    case SystemC:
        return SystemCSemantics::getInstance();
    default:
        return nullptr;
    }
}
Range *ILanguageSemantics::getContextPrecision(Object *o)
{
    messageAssert(o != nullptr, "Passed nullptr object", o, this);

    Object *p = _getNearestIndependentParent(o);

    if (p == o) {
        // If nearest independent object is the staring object,
        // there is no need to collect context precision.
        return nullptr;
    }

    Infos leaves;
    _getLeafInfos(p, leaves, this);
    messageAssert(!leaves.empty(), "No leaves found.", p, this);

    Range *current = hif::copy(leaves.begin()->range);
    for (Infos::iterator i = leaves.begin(); i != leaves.end(); ++i) {
        if (i == leaves.begin())
            continue;
        Range *newMax = hif::semantics::rangeGetMax(current, i->range, this);
        delete current;
        current = newMax;
    }

    hif::manipulation::simplify(current, this);
    return current;
}
std::string ILanguageSemantics::getStandardFilename(const std::string &n)
{
    std::string ret = _standardFilenames[n];
    return ret;
}

bool ILanguageSemantics::useNativeSemantics() const { return _useNativeSemantics; }

void ILanguageSemantics::setUseNativeSemantics(bool b) { _useNativeSemantics = b; }

std::string ILanguageSemantics::makeHifName(const std::string &reqName) const { return _makeHifName(reqName, true); }
template <typename T> T *ILanguageSemantics::getSuffixedCopy(T *obj, const std::string &suffix)
{
    Declaration *decl = dynamic_cast<Declaration *>(obj);
    messageAssert(decl != nullptr, "Expected declaration", obj, this);

    T *suffixed = hif::copy(obj);
    static_cast<Declaration *>(suffixed)->setName(decl->getName() + suffix);
    return suffixed;
}

bool ILanguageSemantics::_checkConcatCasts(
    Expression *e,
    Type * /*castT1*/,
    Type * /*castT2*/,
    Type * /*subT1*/,
    Type * /*subT2*/,
    ExpressionTypeInfo &exprInfo,
    ExpressionTypeInfo &info)
{
    hif::EqualsOptions retOpt;
    retOpt.checkConstexprFlag = false;

    bool equalType = hif::equals(exprInfo.returnedType, info.returnedType, retOpt);
    if (!equalType)
        return false;

    if (dynamic_cast<Cast *>(e->getValue1()) != nullptr) {
        Cast *c = static_cast<Cast *>(e->getValue1());
        if (dynamic_cast<Aggregate *>(c->getValue()) != nullptr) {
            Aggregate *a = static_cast<Aggregate *>(c->getValue());
            if (a->getOthers() != nullptr)
                return false;
        }
    }

    if (dynamic_cast<Cast *>(e->getValue2()) != nullptr) {
        Cast *c = static_cast<Cast *>(e->getValue2());
        if (dynamic_cast<Aggregate *>(c->getValue()) != nullptr) {
            Aggregate *a = static_cast<Aggregate *>(c->getValue());
            if (a->getOthers() != nullptr)
                return false;
        }
    }

    return true;
}

bool ILanguageSemantics::_checkShiftCasts(
    Expression * /*e*/,
    Type *castT1,
    Type *castT2,
    Type *subT1,
    Type *subT2,
    ExpressionTypeInfo &exprInfo,
    ExpressionTypeInfo &info,
    bool &canRemoveOnShift)
{
    hif::EqualsOptions retOpt;
    retOpt.checkConstexprFlag = false;

    bool equalType = hif::equals(exprInfo.returnedType, info.returnedType, retOpt);

    bool equalPrecision = hif::equals(exprInfo.operationPrecision, info.operationPrecision, retOpt);

    const hif::semantics::precision_type_t rhsPrec = hif::semantics::comparePrecision(castT2, subT2, this);

    canRemoveOnShift = (rhsPrec != hif::semantics::UNCOMPARABLE_PRECISION && rhsPrec != hif::semantics::LESS_PRECISION);

    const hif::semantics::precision_type_t leftPrec = hif::semantics::comparePrecision(castT1, subT1, this);
    if (leftPrec == hif::semantics::UNCOMPARABLE_PRECISION || leftPrec == hif::semantics::LESS_PRECISION)
        return false;

    return equalType && equalPrecision;
}

bool ILanguageSemantics::_checkArithmeticCasts(
    Expression *e,
    Type *castT1,
    Type *castT2,
    Type *subT1,
    Type *subT2,
    ExpressionTypeInfo &origInfo,
    ExpressionTypeInfo &simplifiedInfo)
{
    const hif::semantics::precision_type_t leftPrec = hif::semantics::comparePrecision(castT1, subT1, this);
    if (leftPrec == hif::semantics::UNCOMPARABLE_PRECISION || leftPrec == hif::semantics::LESS_PRECISION)
        return false;
    const hif::semantics::precision_type_t rightPrec = hif::semantics::comparePrecision(castT2, subT2, this);
    if (rightPrec == hif::semantics::UNCOMPARABLE_PRECISION || rightPrec == hif::semantics::LESS_PRECISION)
        return false;

    hif::EqualsOptions retOpt;
    retOpt.checkLogicFlag     = false;
    retOpt.checkConstexprFlag = false;
    retOpt.checkResolvedFlag  = false;
    retOpt.checkSignedFlag    = false;
    retOpt.checkSpans         = false;

    hif::EqualsOptions precOpt;
    precOpt.checkLogicFlag        = false;
    precOpt.checkConstexprFlag    = false;
    precOpt.checkResolvedFlag     = false;
    precOpt.checkSignedFlag       = false;
    // NOTE: ref. design: vhdl/custom/crc.
    // remove useless cast on ~range(...)
    precOpt.checkTypeVariantField = false;

    return _checkGenericCasts(e, castT1, castT2, subT1, subT2, origInfo, simplifiedInfo, precOpt, retOpt);
}

bool ILanguageSemantics::_checkRelationalCasts(
    Expression *e,
    Type *castT1,
    Type *castT2,
    Type *subT1,
    Type *subT2,
    ExpressionTypeInfo &origInfo,
    ExpressionTypeInfo &simplifiedInfo)
{
    const hif::semantics::precision_type_t leftPrec = hif::semantics::comparePrecision(castT1, subT1, this);
    if (leftPrec == hif::semantics::UNCOMPARABLE_PRECISION || leftPrec == hif::semantics::LESS_PRECISION)
        return false;
    const hif::semantics::precision_type_t rightPrec = hif::semantics::comparePrecision(castT2, subT2, this);
    if (rightPrec == hif::semantics::UNCOMPARABLE_PRECISION || rightPrec == hif::semantics::LESS_PRECISION)
        return false;
    hif::EqualsOptions retOpt;
    retOpt.checkLogicFlag     = false;
    retOpt.checkConstexprFlag = false;
    retOpt.checkResolvedFlag  = false;
    retOpt.checkSignedFlag    = false;
    retOpt.checkSpans         = false;

    hif::EqualsOptions precOpt;
    precOpt.checkLogicFlag        = false;
    precOpt.checkConstexprFlag    = false;
    precOpt.checkResolvedFlag     = false;
    precOpt.checkSignedFlag       = true;
    // NOTE: ref. design: vhdl/openCores/uart
    // remove useless cast on rx_rdys assignment rhs
    precOpt.checkTypeVariantField = false;

    // ref. design:vhdl/custom/test_library_equality
    bool equalPrec = hif::equals(origInfo.operationPrecision, simplifiedInfo.operationPrecision, precOpt);
    bool equalType = hif::equals(origInfo.returnedType, simplifiedInfo.returnedType, retOpt);

    // In case of bit vs bool, we assume that anything is fine for all semantics.
    // Ref. design: b03 vhdl2hif2vhdl.
    // Ref design: mios_MainControlUnit_verilog2hif for unsimplified whenalt
    bool newSingleBit =
        (hif::semantics::isSingleBitType(simplifiedInfo.operationPrecision, this) &&
         hif::semantics::isSingleBitType(simplifiedInfo.returnedType, this));
    bool origSingleBit =
        (hif::semantics::isSingleBitType(origInfo.operationPrecision, this) &&
         hif::semantics::isSingleBitType(origInfo.returnedType, this));
    // Cannot remove cast, not equivalent expression.
    bool isLogicArithOrNumStd =
        (dynamic_cast<Signed *>(origInfo.operationPrecision) != nullptr ||
         dynamic_cast<Unsigned *>(origInfo.operationPrecision) != nullptr);
    if (isLogicArithOrNumStd && !(equalPrec && equalType) && !(newSingleBit && origSingleBit))
        return false;

    // If operator is not eqaulity, sign can change the result
    // 1 < -1 --> false
    // (uint)1 < (uint)-1 --> true
    // ref. design agility_complex
    if (!hif::operatorIsEquality(e->getOperator())) {
        bool origSigned  = hif::typeIsSigned(origInfo.operationPrecision, this);
        bool simplSigned = hif::typeIsSigned(simplifiedInfo.operationPrecision, this);
        if (origSigned != simplSigned)
            return false;
    }

    return _checkGenericCasts(e, castT1, castT2, subT1, subT2, origInfo, simplifiedInfo, precOpt, retOpt);
}

bool ILanguageSemantics::_checkBitwiseCasts(
    Expression *e,
    Type *castT1,
    Type *castT2,
    Type *subT1,
    Type *subT2,
    ILanguageSemantics::ExpressionTypeInfo &origInfo,
    ILanguageSemantics::ExpressionTypeInfo &simplifiedInfo)
{
    const hif::semantics::precision_type_t leftPrec = hif::semantics::comparePrecision(castT1, subT1, this);
    if (leftPrec == hif::semantics::UNCOMPARABLE_PRECISION || leftPrec == hif::semantics::LESS_PRECISION)
        return false;
    const hif::semantics::precision_type_t rightPrec = hif::semantics::comparePrecision(castT2, subT2, this);
    if (rightPrec == hif::semantics::UNCOMPARABLE_PRECISION || rightPrec == hif::semantics::LESS_PRECISION)
        return false;

    hif::EqualsOptions retOpt;
    retOpt.checkLogicFlag     = false;
    retOpt.checkConstexprFlag = false;
    retOpt.checkResolvedFlag  = false;
    retOpt.checkSignedFlag    = false;
    retOpt.checkSpans         = false;

    hif::EqualsOptions precOpt;
    precOpt.checkLogicFlag        = false;
    precOpt.checkConstexprFlag    = false;
    precOpt.checkResolvedFlag     = false;
    precOpt.checkSignedFlag       = false;
    precOpt.checkTypeVariantField = false;

    return _checkGenericCasts(e, castT1, castT2, subT1, subT2, origInfo, simplifiedInfo, precOpt, retOpt);
}

bool ILanguageSemantics::_checkGenericCasts(
    Expression *e,
    Type *castT1,
    Type *castT2,
    Type *subT1,
    Type *subT2,
    ExpressionTypeInfo &origInfo,
    ExpressionTypeInfo &simplifiedInfo,
    const hif::EqualsOptions &precOpt,
    const hif::EqualsOptions &retOpt)
{
    bool equalPrec = hif::equals(origInfo.operationPrecision, simplifiedInfo.operationPrecision, precOpt);
    bool equalType = hif::equals(origInfo.returnedType, simplifiedInfo.returnedType, retOpt);

    // In case of bit vs bool, we assume that anything is fine for all semantics.
    // Ref. design: b03 vhdl2hif2vhdl.
    // Ref design: mios_MainControlUnit_verilog2hif for unsimplified whenalt
    bool newSingleBit  = ( //hif::semantics::isSingleBitType(simplifiedInfo.operationPrecision, this)
        //&&
        hif::semantics::isSingleBitType(simplifiedInfo.returnedType, this));
    bool origSingleBit = ( //hif::semantics::isSingleBitType(origInfo.operationPrecision, this)
        //&&
        hif::semantics::isSingleBitType(origInfo.returnedType, this));

    // Cannot remove cast, not equivalent expression.
    if (!(equalPrec && equalType) && !(newSingleBit && origSingleBit))
        return false;

    // Removing cast may alter operation precision?
    bool canRemove1 = canRemoveInternalCast(origInfo.operationPrecision, castT1, subT1, this, e);
    bool canRemove2       = true;
    if (castT2 != nullptr && subT2 != nullptr)
        canRemove2 = canRemoveInternalCast(origInfo.operationPrecision, castT2, subT2, this, e);

    return (canRemove1 && canRemove2);
}

} // namespace semantics
} // namespace hif
std::ostream &operator<<(std::ostream &o, const hif::semantics::ILanguageSemantics &s)
{
    if (dynamic_cast<hif::semantics::SystemCSemantics const *>(&s))
        o << "[SystemC] ";
    else if (dynamic_cast<hif::semantics::VHDLSemantics const *>(&s))
        o << "[VHDL] ";
    else if (dynamic_cast<hif::semantics::VerilogSemantics const *>(&s))
        o << "[Verilog] ";
    else if (dynamic_cast<hif::semantics::HIFSemantics const *>(&s))
        o << "[HIF] ";
    return o;
}
// Explicit instantiation for getSuffixedCopy() method

template hif::SubProgram *
hif::semantics::ILanguageSemantics::getSuffixedCopy<hif::SubProgram>(SubProgram *obj, const std::string &suffix);
template hif::Function *
hif::semantics::ILanguageSemantics::getSuffixedCopy<hif::Function>(Function *obj, const std::string &suffix);
template hif::Procedure *
hif::semantics::ILanguageSemantics::getSuffixedCopy<hif::Procedure>(Procedure *obj, const std::string &suffix);
