/// @file splitAssignTargets.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/splitAssignTargets.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"
#include "hif/trash.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#ifdef _MSC_VER
#    pragma warning(disable : 4996)
#endif

// Upper bound to choose whether simplify or not assignments.
#define ASSIGN_UNROLL_THRESHOLD 1000
namespace hif
{
namespace manipulation
{

namespace /* anon */
{
class SplitConcats : public hif::GuideVisitor
{
public:
    SplitConcats(hif::semantics::ILanguageSemantics *sem, const SplitAssignTargetOptions &opt);
    virtual ~SplitConcats();

    virtual int visitAssign(Assign &o);
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitProcedureCall(ProcedureCall &o);
    virtual int visitLibraryDef(LibraryDef &o);
    virtual int visitPortAssign(PortAssign &o);
    virtual int visitView(View &o);

    bool isFixed();

private:
    typedef std::set<Object *> VisitedSet;

    hif::semantics::ILanguageSemantics *_sem;
    hif::HifFactory _factory;
    hif::Trash _trash;
    const SplitAssignTargetOptions &_opt;
    bool _isFixed;
    VisitedSet _visitedSet;

    bool _splitConcat(Assign *o);
    bool _splitNonArrayType(Assign *o);
    bool _splitArrayType(Assign *o);
    bool _splitPortAssign(PortAssign *o);

    bool _fixArrayParameters(Object *o);

    Assign *
    _createRange(Value *targetId, Type *originalTargetType, Value *assignSource, RangeDirection dir, Value *&minBound);
    bool _splitRecordValue(Assign *o);
    Slice *_fixSlice(Slice *inner, Slice *sl);
    Range *_makeRange(const RangeDirection dir, unsigned long long minIndex, unsigned long long maxIndex, Value *v);
    Value *_makeBound(unsigned long long index, Value *v);
    Value *_makeElement(Value *val, Type *sourceType, Type *targetType, Value *min, unsigned long long i);

    SplitConcats(const SplitConcats &);
    SplitConcats &operator=(const SplitConcats &);
};

SplitConcats::SplitConcats(semantics::ILanguageSemantics *sem, const SplitAssignTargetOptions &opt)
    : hif::GuideVisitor()
    , _sem(sem)
    , _factory(sem)
    , _trash()
    , _opt(opt)
    , _isFixed(false)
    , _visitedSet()
{
    // ntd
}

SplitConcats::~SplitConcats()
{
    // ntd
}

int SplitConcats::visitAssign(Assign &o)
{
    GuideVisitor::visitAssign(o);
    if (_visitedSet.find(&o) != _visitedSet.end())
        return 0;
    _visitedSet.insert(&o);

    if (_splitConcat(&o))
        return 0;
    if (_splitNonArrayType(&o))
        return 0;
    if (_splitArrayType(&o))
        return 0;

    return 0;
}

int SplitConcats::visitFunctionCall(FunctionCall &o)
{
    GuideVisitor::visitFunctionCall(o);
    if (_fixArrayParameters(&o))
        return 0;
    return 0;
}

int SplitConcats::visitProcedureCall(ProcedureCall &o)
{
    GuideVisitor::visitProcedureCall(o);
    if (_fixArrayParameters(&o))
        return 0;
    return 0;
}

int SplitConcats::visitLibraryDef(LibraryDef &o)
{
    if (_opt.skipStandardDeclarations && o.isStandard())
        return 0;
    return GuideVisitor::visitLibraryDef(o);
}

int SplitConcats::visitPortAssign(PortAssign &o)
{
    GuideVisitor::visitPortAssign(o);
    if (_visitedSet.find(&o) != _visitedSet.end())
        return 0;
    _visitedSet.insert(&o);

    if (_splitPortAssign(&o))
        return 0;

    return 0;
}

int SplitConcats::visitView(View &o)
{
    if (_opt.skipStandardDeclarations && o.isStandard())
        return 0;
    return GuideVisitor::visitView(o);
}

bool SplitConcats::isFixed() { return _isFixed; }

bool SplitConcats::_splitConcat(Assign *o)
{
    if (!_opt.splitConcats)
        return false;

    RecordValue *rv = dynamic_cast<RecordValue *>(o->getLeftHandSide());
    if (rv != nullptr) {
        return _splitRecordValue(o);
    }

    // Check actual target concat:
    if (dynamic_cast<Expression *>(o->getLeftHandSide()) == nullptr ||
        static_cast<Expression *>(o->getLeftHandSide())->getOperator() != hif::op_concat)
        return false;

    typedef std::list<Value *> Targets;
    Targets targets = hif::manipulation::collectLeftHandSideSymbols(o);
    if (targets.size() <= 1)
        return false;
    Type *tgtType = hif::semantics::getSemanticType(o->getLeftHandSide(), _sem);
    messageAssert(tgtType != nullptr, "Cannot type description", o->getLeftHandSide(), _sem);
    Type *srcType = hif::semantics::getSemanticType(o->getRightHandSide(), _sem);
    messageAssert(srcType != nullptr, "Cannot type description", o->getRightHandSide(), _sem);

    Range *span     = hif::typeGetSpan(srcType, _sem);
    Value *minBound = hif::rangeGetMinBound(span);

    // Add support variable
    std::string sigNameString = "hif";
    for (Targets::iterator i = targets.begin(); i != targets.end(); ++i) {
        sigNameString += std::string("_") + hif::objectGetName(*i);
    }

    std::string supportName      = hif::NameTable::getInstance()->getFreshName(sigNameString.c_str());
    DataDeclaration *supportDecl = nullptr;
    if (_opt.createSignals) {
        supportDecl = _factory.signal(hif::copy(srcType), supportName);
    } else {
        supportDecl = _factory.variable(hif::copy(srcType), supportName);
    }
    supportDecl->setCodeInfo(o->getCodeInfo());

    // Ref design: verilog/custom/test_initial_propagation
    Value *terminalRhs = hif::getTerminalPrefix(hif::getChildSkippingCasts(o->getRightHandSide()));
    if (dynamic_cast<hif::features::ISymbol *>(terminalRhs) != nullptr) {
        Declaration *decl      = hif::semantics::getDeclaration(terminalRhs, _sem);
        DataDeclaration *ddecl = dynamic_cast<DataDeclaration *>(decl);
        if (ddecl != nullptr) {
            if (ddecl->getValue() != nullptr) {
                Value *rhs = hif::copy(o->getRightHandSide());
                supportDecl->setValue(rhs);
                Value *terminalInit = hif::copy(ddecl->getValue());
                terminalRhs         = hif::getTerminalPrefix(hif::getChildSkippingCasts(rhs));
                terminalRhs->replace(terminalInit);
                delete terminalRhs;
            } else {
                supportDecl->setValue(_sem->getTypeDefaultValue(srcType, ddecl));
            }
        }
    }
    if (supportDecl->getValue() == nullptr)
        supportDecl->setValue(_sem->getTypeDefaultValue(srcType, supportDecl));

    Scope *scope = hif::getNearestScope(o, true, false, false);
    messageAssert(scope != nullptr, "Cannot find parent scope", o, _sem);
    BList<Declaration> *declarations = hif::objectGetDeclarationList(scope);
    messageAssert(declarations != nullptr, "Cannot find parent declarations list", o, _sem);
    declarations->push_back(supportDecl);

    // Create new assign: newSig = oldSource
    // NOTE: must be blocking / continuous
    Value *newSource  = new Identifier(supportName);
    Assign *sigAssign = new Assign();
    sigAssign->setLeftHandSide(newSource);
    sigAssign->setRightHandSide(o->setRightHandSide(nullptr));
    messageAssert(o->isInBList(), "Assign is not in BList", o, _sem);
    BList<Assign> newAssigns;

    // split assigns:
    // tgtX = newVar.range(L,R)
    if (span->getDirection() == dir_downto) {
        for (Targets::reverse_iterator i = targets.rbegin(); i != targets.rend(); ++i) {
            Assign *ass = _createRange(*i, tgtType, newSource, span->getDirection(), minBound);
            // Potentially copying NONBLOCKING property for Verilog front-end
            for (Object::PropertyMapIterator iter = o->getPropertyBeginIterator(); iter != o->getPropertyEndIterator();
                 ++iter) {
                ass->addProperty(iter->first, iter->second);
            }
            newAssigns.push_front(ass);
        }
    } else if (span->getDirection() == dir_upto) {
        for (Targets::iterator i = targets.begin(); i != targets.end(); ++i) {
            Assign *ass = _createRange(*i, tgtType, newSource, span->getDirection(), minBound);
            // Potentially copying NONBLOCKING property for Verilog front-end
            for (Object::PropertyMapIterator iter = o->getPropertyBeginIterator(); iter != o->getPropertyEndIterator();
                 ++iter) {
                ass->addProperty(iter->first, iter->second);
            }
            newAssigns.push_back(ass);
        }
    }

    newAssigns.push_front(sigAssign);
    BList<Assign>::iterator it(o);
    it.insert_after(newAssigns);
    _trash.insert(o);

    _isFixed = true;
    return true;
}

bool SplitConcats::_splitNonArrayType(Assign *o)
{
    if (!_opt.splitIntegers && !_opt.splitVectors)
        return false;

    Value *lhs       = o->getLeftHandSide();
    Type *targetType = hif::semantics::getSemanticType(lhs, _sem);
    Type *baseType   = hif::semantics::getBaseType(targetType, false, _sem);
    if (dynamic_cast<Int *>(baseType) != nullptr && !_opt.splitIntegers)
        return false;
    else if (hif::semantics::isVectorType(baseType, _sem) && !_opt.splitVectors)
        return false;

    Range *span           = hif::typeGetSpan(baseType, _sem);
    unsigned long long bw = hif::semantics::spanGetBitwidth(span, _sem);
    if (bw == 0 || bw <= _opt.maxBitwidth)
        return false;

    unsigned long long replicates = bw / _opt.maxBitwidth;
    if (bw % _opt.maxBitwidth != 0) {
        ++replicates;
        messageError("TODO: Unsupported case of non-multiple bitwidth", nullptr, nullptr);
        // @TODO Should not return and support...
    }

    Value *minBound               = hif::rangeGetMinBound(span);
    const hif::RangeDirection dir = span->getDirection();
    Type *sourceType              = hif::semantics::getSemanticType(o->getRightHandSide(), _sem);
    const bool needCast           = !hif::equals(targetType, sourceType);
    Value *target                 = o->getLeftHandSide();
    Value *source                 = o->getRightHandSide();
    Slice *leftSlice              = dynamic_cast<Slice *>(target);
    if (leftSlice != nullptr)
        target = leftSlice->getPrefix();

    // a_128 = expr ->
    // a_128[127:64] = expr[127:64]
    // a_128[63:0] = expr[63:0]
    // In case of slice :
    // b_128[120:0] = expr ->
    // b_128[127:64][56:0] = expr[127:64]
    // b_128[63:0][63:0] = expr[63:0]
    BList<Object>::iterator it(o);

    for (unsigned long long i = 0; i < replicates; ++i) {
        Assign *as = new Assign();

        Slice *left = _factory.slice(
            hif::copy(target), _makeRange(dir, _opt.maxBitwidth * i, _opt.maxBitwidth * (i + 1) - 1, minBound));

        if (leftSlice != nullptr) {
            left = _fixSlice(left, leftSlice);
        }

        Value *rightValue = hif::copy(source);
        if (needCast)
            rightValue = _factory.cast(hif::copy(targetType), rightValue);
        Slice *right =
            _factory.slice(rightValue, _makeRange(dir, _opt.maxBitwidth * i, _opt.maxBitwidth * (i + 1) - 1, minBound));

        as->setLeftHandSide(left);
        as->setRightHandSide(right);
        it.insert_after(as);
        hif::manipulation::simplify(as, _sem);
    }

    _trash.insert(o);

    _isFixed = true;
    return true;
}

bool SplitConcats::_splitArrayType(Assign *o)
{
    if (!_opt.splitArrays)
        return false;

    Value *target    = o->getLeftHandSide();
    Type *targetType = hif::semantics::getSemanticType(target, _sem);
    Type *baseType   = hif::semantics::getBaseType(targetType, false, _sem);
    if (dynamic_cast<Array *>(baseType) == nullptr)
        return false;

    Range *targetSpan     = hif::typeGetSpan(baseType, _sem);
    unsigned long long bw = hif::semantics::spanGetBitwidth(targetSpan, _sem);
    if (bw == 0)
        return false;
    if (bw > _opt.unrollingUpperBound && _opt.unrollingUpperBound > 0)
        return false;

    // Just un roll assign:
    Value *min    = hif::rangeGetMinBound(targetSpan);
    Value *source = o->getRightHandSide();
    if (dynamic_cast<Cast *>(source) != nullptr && hif::equals(static_cast<Cast *>(source)->getType(), targetType)) {
        source = static_cast<Cast *>(source)->getValue();
    }
    Type *sourceBaseType    = hif::semantics::getBaseType(source, false, _sem);
    Type *targetElementType = hif::semantics::getVectorElementType(baseType, _sem);
    Type *sourceElementType = hif::semantics::getVectorElementType(sourceBaseType, _sem);
    const bool needCast     = !hif::equals(targetElementType, sourceElementType);

    BList<Object>::iterator it(o);
    hif::CopyOptions opt;
    opt.copySemanticsTypes = true;
    for (unsigned long long i = 0; i < bw; ++i) {
        Assign *ass = new Assign();
        ass->setLeftHandSide(_factory.member(hif::copy(target), _makeBound(i, min)));

        Value *right = hif::copy(source, opt);
        ass->setRightHandSide(_makeElement(right, sourceBaseType, baseType, min, i));
        if (needCast)
            ass->setRightHandSide(_factory.cast(hif::copy(targetElementType), ass->setRightHandSide(nullptr)));

        it.insert_after(ass);

        hif::semantics::resetTypes(ass);
        hif::manipulation::simplify(ass, _sem);
    }

    _trash.insert(o);

    _isFixed = true;
    return true;
}

bool SplitConcats::_splitPortAssign(PortAssign *o)
{
    if (!_opt.splitPortassigns)
        return false;

    Expression *expr = dynamic_cast<Expression *>(hif::getChildSkippingCasts(o->getValue()));
    if (expr == nullptr || expr->getOperator() != hif::op_concat)
        return false;

    Port *port = hif::semantics::getDeclaration(o, _sem);
    messageAssert(port != nullptr, "Declaration not found", o, _sem);

    if (port->getDirection() == hif::dir_in)
        return 0;
    messageAssert(port->getDirection() != hif::dir_inout, "Unsupported inout port to be split on concat", o, _sem);

    Signal *sig = new Signal();
    sig->setCodeInfo(o->getCodeInfo());
    BaseContents *bs = hif::getNearestParent<BaseContents>(o);
    messageAssert(bs != nullptr, "Unexpected case", o, _sem);
    bs->declarations.push_back(sig);
    sig->setName(NameTable::getInstance()->getFreshName(port->getName(), "_portconcat"));
    Type *t = hif::semantics::getSemanticType(o, _sem);
    messageAssert(t != nullptr, "Cannot type description", o, _sem);
    sig->setType(hif::copy(t));
    // Ref design: verilog/custom/test_initial_propagation
    Port *instPort = hif::manipulation::instantiate(o, _sem);
    messageAssert(instPort != nullptr, "Unable to instantiate port", o, _sem);
    if (instPort->getValue() == nullptr)
        sig->setValue(_sem->getTypeDefaultValue(t, port));
    else
        sig->setValue(hif::copy(instPort->getValue()));
    Value *bind = o->setValue(new Identifier(sig->getName()));

    Assign *ass = new Assign();
    ass->setLeftHandSide(bind);
    ass->setRightHandSide(new Identifier(sig->getName()));
    if (bs->getGlobalAction() == nullptr)
        bs->setGlobalAction(new GlobalAction());
    bs->getGlobalAction()->actions.push_back(ass);

    ass->acceptVisitor(*this);

    _isFixed = true;
    return true;
}

bool SplitConcats::_fixArrayParameters(Object *o)
{
    if (!_opt.removeSignalPortArrayParameters)
        return false;
    if (hif::declarationIsPartOfStandard(o, _sem))
        return false;

    BList<ParameterAssign> *params = nullptr;
    if (dynamic_cast<FunctionCall *>(o) != nullptr) {
        params = &static_cast<FunctionCall *>(o)->parameterAssigns;
    } else if (dynamic_cast<ProcedureCall *>(o) != nullptr) {
        params = &static_cast<ProcedureCall *>(o)->parameterAssigns;
    } else {
        return false;
    }

    for (BList<ParameterAssign>::iterator it = params->begin(); it != params->end(); ++it) {
        ParameterAssign *pa = *it;
        Type *type          = hif::semantics::getSemanticType(pa->getValue(), _sem);
        Type *baseType      = hif::semantics::getBaseType(pa->getValue(), false, _sem);
        Array *arr          = dynamic_cast<Array *>(baseType);
        if (arr == nullptr)
            continue;
        Identifier *id = dynamic_cast<Identifier *>(hif::getTerminalPrefix(pa->getValue()));
        if (id == nullptr)
            continue;
        DataDeclaration *decl = hif::semantics::getDeclaration(id, _sem);
        if (dynamic_cast<Signal *>(decl) == nullptr && dynamic_cast<Port *>(decl) == nullptr)
            continue;

        const std::string &n = std::string(id->getName()) + "_supp";
        std::string varName  = hif::NameTable::getInstance()->getFreshName(n.c_str());
        Variable *var        = _factory.variable(hif::copy(type), varName);
        StateTable *st       = hif::getNearestParent<StateTable>(o);
        messageAssert(st != nullptr, "Cannot find enclosing scope State Table", o, _sem);

        Assign *ass         = hif::getNearestParent<Assign>(o);
        DataDeclaration *dd = getNearestParent<DataDeclaration>(o);

        if (ass != nullptr) {
            messageAssert(ass->isInBList(), "Unexpected assign outside blist", ass, _sem);
            Assign *newAss = _factory.assignment(new Identifier(varName), pa->getValue());
            pa->setValue(new Identifier(varName));
            BList<Assign>::iterator jt(ass);
            jt.insert_before(newAss);
            st->declarations.push_back(var);
            var->setValue(_sem->getTypeDefaultValue(type, decl));
            var->acceptVisitor(*this);
            newAss->acceptVisitor(*this);
        } else if (dd != nullptr) {
            var->setValue(pa->getValue());
            pa->setValue(new Identifier(varName));
            BList<DataDeclaration>::iterator jt(dd);
            jt.insert_before(var);
            var->acceptVisitor(*this);
        } else {
            messageError("Unexpected case", o, _sem);
        }
    }

    _isFixed = true;
    return true;
}

Assign *SplitConcats::_createRange(
    Value *targetId,
    Type *originalTargetType,
    Value *assignSource,
    hif::RangeDirection dir,
    Value *&minBound)
{
    Value *vt = hif::manipulation::getLeftHandSide(targetId);
    Type *tt  = hif::semantics::getSemanticType(vt, _sem);
    messageAssert(tt != nullptr, "Cannot type description", targetId, _sem);
    Value *tSpanSize = hif::semantics::typeGetSpanSize(tt, _sem);

    Assign *a = new Assign();
    a->setLeftHandSide(hif::copy(vt));

    Slice *s = new Slice();
    a->setRightHandSide(s);
    IntValue *iv = _factory.intval(0, nullptr);
    delete iv->setType(_sem->getTypeForConstant(iv));
    Value *value = hif::manipulation::transformValue(iv, originalTargetType, _sem);
    messageDebugIfFails(value != nullptr, "original target type", originalTargetType, _sem);
    messageAssert(value != nullptr, "Cannot transform value", iv, _sem);
    delete iv;
    s->setPrefix(_factory.expression(hif::copy(assignSource), hif::op_plus, value));

    Range *r = new Range();
    r->setDirection(dir);
    s->setSpan(r);

    hif::rangeSetMinBound(r, hif::copy(minBound));

    Value *newMinBound =
        _factory.expression(hif::manipulation::assureSyntacticType(hif::copy(minBound), _sem), hif::op_plus, tSpanSize);

    Expression *e = _factory.expression(newMinBound, hif::op_minus, _factory.intval(1));

    hif::rangeSetMaxBound(r, e);
    minBound = newMinBound;

    a->setRightHandSide(_factory.cast(hif::copy(tt), a->setRightHandSide(nullptr)));

    return a;
}

bool SplitConcats::_splitRecordValue(Assign *o)
{
    if (!_opt.splitRecordValueConcats)
        return false;

    // Managing special case when target is a record value
    // Ref. design: vhdl/gaisler/can_oc
    RecordValue *rv = static_cast<RecordValue *>(o->getLeftHandSide());
    messageAssert(!rv->alts.empty(), "Unexpected record value", rv, _sem);

    BList<Assign> newAssigns;
    for (BList<RecordValueAlt>::iterator i = rv->alts.begin(); i != rv->alts.end(); ++i) {
        RecordValueAlt *rva = *i;
        Value *v            = rva->setValue(nullptr);
        Assign *ass         = new Assign();
        ass->setLeftHandSide(v);
        FieldReference *fr = new FieldReference();
        fr->setName(rva->getName());
        fr->setPrefix(hif::copy(o->getRightHandSide()));
        ass->setRightHandSide(fr);

        newAssigns.push_back(ass);
    }

    BList<Assign>::iterator it(o);
    it.insert_after(newAssigns);
    _trash.insert(o);

    _isFixed = true;
    return true;
}

Slice *SplitConcats::_fixSlice(Slice *inner, Slice *sl)
{
    Value *min       = hif::rangeGetMinBound(inner->getSpan());
    Range *sliceSpan = sl->getSpan();
    Value *lb        = _factory.expression(
        hif::manipulation::assureSyntacticType(hif::copy(sliceSpan->getLeftBound()), _sem), hif::op_minus,
        hif::manipulation::assureSyntacticType(hif::copy(min), _sem));
    Value *rb = _factory.expression(
        hif::manipulation::assureSyntacticType(hif::copy(sliceSpan->getRightBound()), _sem), hif::op_minus,
        hif::manipulation::assureSyntacticType(hif::copy(min), _sem));
    Slice *ret = _factory.slice(inner, _factory.range(lb, sliceSpan->getDirection(), rb));

    return ret;
}

Range *
SplitConcats::_makeRange(const RangeDirection dir, unsigned long long minIndex, unsigned long long maxIndex, Value *v)
{
    Range *ret = new Range();
    ret->setDirection(dir);
    Value *min = _makeBound(minIndex, v);
    Value *max = _makeBound(maxIndex, v);
    hif::rangeSetMinBound(ret, min);
    hif::rangeSetMaxBound(ret, max);
    return ret;
}

Value *SplitConcats::_makeBound(unsigned long long index, Value *v)
{
    const bool isZero =
        (v == nullptr) || (dynamic_cast<IntValue *>(v) != nullptr && static_cast<IntValue *>(v)->getValue() == 0);

    Value *ret = nullptr;
    if (isZero) {
        ret = new IntValue(index);
    } else {
        ret = _factory.expression(
            _factory.intval(index), hif::op_plus, hif::manipulation::assureSyntacticType(hif::copy(v), _sem));
    }

    return ret;
}

Value *SplitConcats::_makeElement(Value *val, Type *sourceType, Type *targetType, Value *min, unsigned long long i)
{
    Value *ret                    = nullptr;
    long long ill                 = static_cast<long long>(i);
    const bool isSourceVectorType = hif::semantics::isVectorType(sourceType, _sem);
    const bool isTargetArray      = (dynamic_cast<Array *>(targetType) != nullptr);

    if (isSourceVectorType && isTargetArray) {
        Type *nested = hif::typeGetNestedType(targetType, _sem, 1);
        messageAssert(nested != nullptr, "Unable to calculate type", targetType, _sem);
        Value *bw         = hif::semantics::typeGetSpanSize(nested, _sem);
        Range *sourceSpan = hif::typeGetSpan(sourceType, _sem);
        messageAssert(sourceSpan != nullptr, "Unable to calculate span", sourceType, _sem);
        Value *maxBound = hif::manipulation::assureSyntacticType(hif::copy(hif::rangeGetMaxBound(sourceSpan)), _sem);

        Value *rBound = _factory.expression(
            _factory.expression(bw, hif::op_mult, _factory.intval(ill)), hif::op_plus, hif::copy(min));

        Value *lBound = _factory.expression(
            _factory.expression(
                _factory.expression(
                    _factory.expression(_factory.intval(ill), hif::op_plus, _factory.intval(1)), hif::op_mult,
                    hif::copy(bw)),
                hif::op_minus, _factory.intval(1)),
            hif::op_plus, hif::copy(min));

        When *leftBound = new When();
        WhenAlt *wa     = new WhenAlt();
        leftBound->alts.push_back(wa);
        wa->setCondition(_factory.expression(hif::copy(lBound), hif::op_le, hif::copy(maxBound)));
        wa->setValue(lBound);
        leftBound->setDefault(maxBound);

        Range *span = _factory.range(leftBound, hif::dir_downto, rBound);
        ret         = _factory.cast(hif::copy(nested), _factory.slice(val, span));
    } else if (isSourceVectorType || dynamic_cast<Array *>(sourceType) != nullptr) {
        ret = _factory.member(val, _makeBound(i, min));
    } else if (dynamic_cast<Int *>(sourceType) != nullptr) {
        Expression *expr =
            _factory.expression(val, hif::op_srl, hif::manipulation::assureSyntacticType(_makeBound(i, min), _sem));
        ret = _factory.expression(
            expr, hif::op_band, _factory.intval(1, _factory.integer(hif::typeGetSpan(sourceType, _sem))));
    } else {
        messageError("Unexpected type", val, _sem);
    }

    return ret;
}

} // namespace

// /////////////////////////////////////////////////////////////////////////////
// SplitAssignTargetOptions
// /////////////////////////////////////////////////////////////////////////////
SplitAssignTargetOptions::SplitAssignTargetOptions()
    : skipStandardDeclarations(true)
    , splitConcats(false)
    , splitVectors(false)
    , splitIntegers(false)
    , splitArrays(false)
    , removeSignalPortArrayParameters(false)
    , splitRecordValueConcats(false)
    , createSignals(false)
    , splitPortassigns(false)
    , maxBitwidth(64)
    , unrollingUpperBound(ASSIGN_UNROLL_THRESHOLD)
{
    // ntd
}

SplitAssignTargetOptions::~SplitAssignTargetOptions()
{
    // ntd
}

SplitAssignTargetOptions::SplitAssignTargetOptions(const SplitAssignTargetOptions &other)
    : skipStandardDeclarations(other.skipStandardDeclarations)
    , splitConcats(other.splitConcats)
    , splitVectors(other.splitVectors)
    , splitIntegers(other.splitIntegers)
    , splitArrays(other.splitArrays)
    , removeSignalPortArrayParameters(other.removeSignalPortArrayParameters)
    , splitRecordValueConcats(other.splitRecordValueConcats)
    , createSignals(other.createSignals)
    , splitPortassigns(other.splitPortassigns)
    , maxBitwidth(other.maxBitwidth)
    , unrollingUpperBound(other.unrollingUpperBound)
{
    // ntd
}

SplitAssignTargetOptions &SplitAssignTargetOptions::operator=(SplitAssignTargetOptions other)
{
    this->swap(other);
    return *this;
}

void SplitAssignTargetOptions::swap(SplitAssignTargetOptions &other)
{
    std::swap(skipStandardDeclarations, other.skipStandardDeclarations);
    std::swap(splitConcats, other.splitConcats);
    std::swap(splitVectors, other.splitVectors);
    std::swap(splitIntegers, other.splitIntegers);
    std::swap(splitArrays, other.splitArrays);
    std::swap(removeSignalPortArrayParameters, other.removeSignalPortArrayParameters);
    std::swap(splitRecordValueConcats, other.splitRecordValueConcats);
    std::swap(createSignals, other.createSignals);
    std::swap(splitPortassigns, other.splitPortassigns);
    std::swap(maxBitwidth, other.maxBitwidth);
    std::swap(unrollingUpperBound, other.unrollingUpperBound);
}

// /////////////////////////////////////////////////////////////////////////////
// splitAssignTargets
// /////////////////////////////////////////////////////////////////////////////

bool splitAssignTargets(Object *root, hif::semantics::ILanguageSemantics *sem, const SplitAssignTargetOptions &opt)
{
    SplitConcats v(sem, opt);
    root->acceptVisitor(v);
    return v.isFixed();
}
} // namespace manipulation
} // namespace hif
