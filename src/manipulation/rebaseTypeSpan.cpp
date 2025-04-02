/// @file rebaseTypeSpan.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/rebaseTypeSpan.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/search.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

void _explicitCallsParameters(Object *root, hif::semantics::ILanguageSemantics *sem)
{
    hif::HifTypedQuery<SubProgram> q;
    std::list<SubProgram *> list;
    hif::search(list, root, q);

    std::set<SubProgram *> toFix;

    for (std::list<SubProgram *>::iterator i = list.begin(); i != list.end(); ++i) {
        SubProgram *sub = *i;
        if (!sub->checkProperty(PROPERTY_METHOD_EXPLICIT_PARAMETERS))
            continue;
        sub->removeProperty(PROPERTY_METHOD_EXPLICIT_PARAMETERS);
        toFix.insert(sub);
    }

    if (toFix.empty())
        return;

    hif::semantics::ReferencesMap refMap;
    hif::semantics::getAllReferences(refMap, sem, root);

    hif::manipulation::explicitCallsParameters(toFix, refMap, sem);
}

// ///////////////////////////////////////////////////////////////////
// RebaseTypeVisitor
// ///////////////////////////////////////////////////////////////////
class RebaseTypeVisitor : public hif::GuideVisitor
{
public:
    RebaseTypeVisitor(hif::semantics::ILanguageSemantics *sem, bool currentOnly);
    ~RebaseTypeVisitor();

    virtual int visitAggregate(Aggregate &o);
    virtual int visitArray(Array &o);
    virtual int visitBitvector(Bitvector &o);
    virtual int visitCast(Cast &o);
    virtual int visitInt(Int &o);
    virtual int visitMember(Member &o);
    virtual int visitSigned(Signed &o);
    virtual int visitSlice(Slice &o);
    virtual int visitString(String &o);
    virtual int visitTypeReference(TypeReference &o);
    virtual int visitUnsigned(Unsigned &o);

private:
    RebaseTypeVisitor(const RebaseTypeVisitor &);
    RebaseTypeVisitor &operator=(const RebaseTypeVisitor &);

    void _rebaseSpan(Range *r);

    bool _fixTypedRange(Range *tSpan);

    void _collectSpan(Range *r);

    bool _isTop(Value *o);

    hif::semantics::ILanguageSemantics *_sem;
    HifFactory _factory;

    typedef std::set<Range *> Ranges;
    Ranges _ranges;

    Value *_innerShift;

    bool _currentOnly;
};
RebaseTypeVisitor::RebaseTypeVisitor(hif::semantics::ILanguageSemantics *sem, bool currentOnly)
    : _sem(sem)
    , _factory(sem)
    , _ranges()
    , _innerShift(nullptr)
    , _currentOnly(currentOnly)
{
    // ntd
}
RebaseTypeVisitor::~RebaseTypeVisitor()
{
    for (Ranges::iterator i = _ranges.begin(); i != _ranges.end(); ++i) {
        _rebaseSpan(*i);
    }
}

void RebaseTypeVisitor::_rebaseSpan(Range *r)
{
    Value *min = hif::rangeGetMinBound(r);
    if (min == nullptr)
        return;

    IntValue *i = dynamic_cast<IntValue *>(min);
    messageAssert(i == nullptr || i->getValue() != 0, "Unexpected collected range", r, _sem);

    Value *left  = hif::manipulation::assureSyntacticType(r->setLeftBound(nullptr), _sem);
    Value *right = hif::manipulation::assureSyntacticType(r->setRightBound(nullptr), _sem);

    if (left != nullptr)
        r->setLeftBound(_factory.expression(left, op_minus, hif::copy(min)));
    if (right != nullptr)
        r->setRightBound(_factory.expression(right, op_minus, hif::copy(min)));
}
bool RebaseTypeVisitor::_fixTypedRange(Range *tSpan)
{
    // Heuristic to check if a range is typed.
    Identifier *lb = dynamic_cast<Identifier *>(tSpan->getLeftBound());
    Identifier *rb = dynamic_cast<Identifier *>(tSpan->getRightBound());
    ValueTP *lbDec = dynamic_cast<ValueTP *>(hif::semantics::getDeclaration(lb, _sem));
    ValueTP *rbDec = dynamic_cast<ValueTP *>(hif::semantics::getDeclaration(rb, _sem));
    bool isTyped =
        (lb != nullptr && rb != nullptr && lbDec != nullptr && rbDec != nullptr && lbDec->isInBList() &&
         rbDec->isInBList() && lbDec->getBList() == rbDec->getBList());

    if (!isTyped)
        return false;

    // Get the owner of BList
    Declaration *bListOwner = dynamic_cast<Declaration *>(lbDec->getParent());
    System *sys             = hif::getNearestParent<System>(tSpan);
    if (bListOwner == nullptr || sys == nullptr) {
        messageDebugAssert(bListOwner != nullptr, "Unexpected blist owner", lbDec->getParent(), _sem);
        return false;
    }

    // Special management for standard methods (created by us)
    bool isStandard =
        (bListOwner->getParent() != nullptr && dynamic_cast<SubProgram *>(bListOwner) != nullptr &&
         dynamic_cast<LibraryDef *>(bListOwner->getParent()) != nullptr &&
         static_cast<LibraryDef *>(bListOwner->getParent())->isStandard());
    if (isStandard) {
        messageAssert(tSpan->getDirection() == dir_downto, "Unexpected direction", tSpan->getParent(), _sem);

        hif::semantics::ReferencesSet rightRefs;
        hif::semantics::getReferences(rbDec, rightRefs, _sem, sys);

        for (hif::semantics::ReferencesSet::iterator it(rightRefs.begin()); it != rightRefs.end(); ++it) {
            if (dynamic_cast<Identifier *>(*it) != nullptr) {
                IntValue *zero = new IntValue(0);
                Object *old    = *it;
                (*it)->replace(zero);
                hif::manipulation::assureSyntacticType(zero, _sem);
                delete old;
            } else
                messageError("Unexpected use of right bound in typed range", (*it)->getParent(), _sem);
        }

        rbDec->replace(nullptr);
        delete rbDec;

        return true;
    }

    // Get references to the owner
    hif::semantics::ReferencesSet references;
    hif::semantics::getReferences(bListOwner, references, _sem, sys);

    for (hif::semantics::ReferencesSet::iterator it(references.begin()); it != references.end(); ++it) {
        Object *o                     = *it;
        BList<TPAssign> *tpAssignList = objectGetTemplateAssignList(o);
        messageAssert(tpAssignList != nullptr, "Unexpected case", nullptr, nullptr);

        BList<Declaration> &tpDecls = lbDec->getBList()->toOtherBList<Declaration>();
        hif::manipulation::sortParameters(
            *tpAssignList, tpDecls, true, hif::manipulation::SortMissingKind::ALL, _sem, false);
    }

    return false;
}

void RebaseTypeVisitor::_collectSpan(Range *r)
{
    if (r == nullptr)
        return;
    Value *min = hif::rangeGetMinBound(r);

    IntValue *i = dynamic_cast<IntValue *>(min);
    if (i != nullptr && i->getValue() == 0) {
        return;
    }

    if (_fixTypedRange(r)) {
        return;
    }

    _ranges.insert(r);
}

bool RebaseTypeVisitor::_isTop(Value *o)
{
    Slice *s  = dynamic_cast<Slice *>(o->getParent());
    Member *m = dynamic_cast<Member *>(o->getParent());
    Cast *c   = dynamic_cast<Cast *>(o->getParent());

    return s == nullptr && m == nullptr && c == nullptr;
}

int RebaseTypeVisitor::visitAggregate(Aggregate &o)
{
    if (!_currentOnly)
        GuideVisitor::visitAggregate(o);

    Type *aggType = hif::semantics::getSemanticType(&o, _sem);
    messageAssert(aggType != nullptr, "Cannot type aggregate", &o, _sem);

    Array *arr = dynamic_cast<Array *>(aggType);
    if (arr == nullptr)
        return 0;
    Range *r    = arr->getSpan();
    Value *min  = hif::rangeGetMinBound(r);
    IntValue *i = dynamic_cast<IntValue *>(min);
    if (i != nullptr && i->getValue() == 0)
        return 0;
    min = hif::manipulation::assureSyntacticType(hif::copy(min), _sem);

    for (BList<AggregateAlt>::iterator it = o.alts.begin(); it != o.alts.end(); ++it) {
        for (BList<Value>::iterator j = (*it)->indices.begin(); j != (*it)->indices.end(); ++j) {
            Value *ind  = *j;
            Range *rInd = dynamic_cast<Range *>(ind);
            if (rInd != nullptr) {
                Value *left    = rInd->getLeftBound();
                Expression *e1 = _factory.expression(nullptr, op_minus, hif::copy(min));
                left->replace(e1);
                e1->setValue1(left);
                hif::manipulation::assureSyntacticType(left, _sem);

                Value *right   = rInd->getLeftBound();
                Expression *e2 = _factory.expression(nullptr, op_minus, hif::copy(min));
                right->replace(e2);
                e2->setValue1(right);
                hif::manipulation::assureSyntacticType(right, _sem);

                continue;
            }

            // intvalue / expression
            Expression *e = _factory.expression(nullptr, op_minus, hif::copy(min));
            ind->replace(e);
            e->setValue1(ind);
            hif::manipulation::assureSyntacticType(ind, _sem);
        }
    }

    delete min;
    return 0;
}

int RebaseTypeVisitor::visitArray(Array &o)
{
    if (!_currentOnly)
        GuideVisitor::visitArray(o);
    _collectSpan(o.getSpan());
    return 0;
}
int RebaseTypeVisitor::visitBitvector(Bitvector &o)
{
    if (!_currentOnly)
        GuideVisitor::visitBitvector(o);
    _collectSpan(o.getSpan());
    return 0;
}
int RebaseTypeVisitor::visitCast(Cast &o)
{
    if (!_currentOnly)
        GuideVisitor::visitCast(o);

    delete _innerShift;
    _innerShift = nullptr;

    return 0;
}
int RebaseTypeVisitor::visitInt(Int &o)
{
    if (!_currentOnly)
        GuideVisitor::visitInt(o);
    _collectSpan(o.getSpan());
    return 0;
}
int RebaseTypeVisitor::visitMember(Member &o)
{
    if (!_currentOnly)
        GuideVisitor::visitMember(o);

    if (_innerShift == nullptr) {
        Type *t = hif::semantics::getSemanticType(o.getPrefix(), _sem);
        messageAssert(t != nullptr, "Cannot find type", &o, _sem);
        Range *r = hif::typeGetSpan(t, _sem, true);
        if (r == nullptr)
            return 0;
        Value *min = hif::rangeGetMinBound(r);
        if (min == nullptr)
            return 0;
        min         = hif::manipulation::assureSyntacticType(hif::copy(min), _sem);
        _innerShift = min;
    }

    IntValue *i = dynamic_cast<IntValue *>(_innerShift);
    if (i != nullptr && i->getValue() == 0) {
        delete _innerShift;
        _innerShift = nullptr;
        return 0;
    }

    messageAssert(o.getIndex() != nullptr, "Expected index", &o, _sem);
    Value *index = o.getIndex();
    index->replace(nullptr);
    index = hif::manipulation::assureSyntacticType(index, _sem);
    o.setIndex(_factory.expression(index, op_minus, hif::copy(_innerShift)));

    delete _innerShift;
    _innerShift = nullptr;
    return 0;
}
int RebaseTypeVisitor::visitSigned(Signed &o)
{
    if (!_currentOnly)
        GuideVisitor::visitSigned(o);
    _collectSpan(o.getSpan());
    return 0;
}
int RebaseTypeVisitor::visitSlice(Slice &o)
{
    if (!_currentOnly)
        GuideVisitor::visitSlice(o);

    if (_innerShift == nullptr) {
        Type *t = hif::semantics::getSemanticType(o.getPrefix(), _sem);
        messageAssert(t != nullptr, "Cannot find type", &o, _sem);
        Range *r = hif::typeGetSpan(t, _sem, true);
        if (r == nullptr)
            return 0;
        Value *min = hif::rangeGetMinBound(r);
        if (min == nullptr)
            return 0;
        min         = hif::manipulation::assureSyntacticType(hif::copy(min), _sem);
        _innerShift = min;
    }

    bool isTop = _isTop(&o);

    IntValue *i = dynamic_cast<IntValue *>(_innerShift);
    if (i == nullptr || i->getValue() != 0) {
        Range *rs    = o.getSpan();
        Value *left  = hif::manipulation::assureSyntacticType(rs->setLeftBound(nullptr), _sem);
        Value *right = hif::manipulation::assureSyntacticType(rs->setRightBound(nullptr), _sem);

        rs->setLeftBound(_factory.expression(left, op_minus, hif::copy(_innerShift)));
        rs->setRightBound(_factory.expression(right, op_minus, hif::copy(_innerShift)));
    }

    if (isTop) {
        Type *st = hif::semantics::getSemanticType(&o, _sem);
        messageAssert(st != nullptr, "Cannot type slice", &o, _sem);

        Cast *c = new Cast();
        c->setType(hif::copy(st));
        o.replace(c);
        c->setValue(&o);
        c->getType()->acceptVisitor(*this);

        delete _innerShift;
        _innerShift = nullptr;
    }

    return 0;
}
int RebaseTypeVisitor::visitString(String &o)
{
    if (!_currentOnly)
        GuideVisitor::visitString(o);
    Range *r = o.getSpanInformation();
    if (r == nullptr)
        return 0;
    if (r->getLeftBound() != nullptr || r->getRightBound() != nullptr) {
        messageAssert(rangeGetMinBound(r) != nullptr, "Unexpected mixed case", &o, _sem);
        _collectSpan(o.getSpanInformation());
    }

    return 0;
}
int RebaseTypeVisitor::visitTypeReference(TypeReference &o)
{
    if (!_currentOnly)
        GuideVisitor::visitTypeReference(o);
    return 0;
}
int RebaseTypeVisitor::visitUnsigned(Unsigned &o)
{
    if (!_currentOnly)
        GuideVisitor::visitUnsigned(o);
    _collectSpan(o.getSpan());
    return 0;
}

} // namespace

bool rebaseTypeSpan(Object *root, hif::semantics::ILanguageSemantics *refLang, bool currentOnly)
{
    _explicitCallsParameters(root, refLang);

    {
        RebaseTypeVisitor vis(refLang, currentOnly);
        root->acceptVisitor(vis);
    }

    return true;
}
} // namespace manipulation
} // namespace hif
