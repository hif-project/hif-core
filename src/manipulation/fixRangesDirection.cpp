/// @file fixRangesDirection.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/fixRangesDirection.hpp"

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

namespace
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

std::set<Range *> typeSpansToRevert;

// Note: must be called after the manipulation function: rebaseTypeSpan.

/// @brief Fixes the problem caused by span upto. In SystemC they don't exists.
/// The original span is transformed into the common [N downto 0].
/// A vhdl example:
/// @code{.vhd}
/// signal s : std_logic_vector(0 to 12);
/// @endcode
///
/// Is changed to:
/// @code{.vhd}
/// signal s : std_logic_vector(12 downto 0);
/// @endcode
///
/// Also Members and Slices are fixed wrt to their types (as they were before
/// the reverting operation).
///
class PreRefine_ranges : public hif::GuideVisitor
{
public:
    /// @brief Default constructor and destructor.
    PreRefine_ranges(hif::semantics::ILanguageSemantics *sem);
    virtual ~PreRefine_ranges();

    virtual int visitAggregate(hif::Aggregate &o);
    virtual int visitArray(hif::Array &o);
    virtual int visitBitvector(hif::Bitvector &o);
    virtual int visitCast(hif::Cast &o);
    virtual int visitInt(hif::Int &o);
    virtual int visitMember(hif::Member &o);
    virtual int visitSigned(hif::Signed &o);
    virtual int visitSlice(hif::Slice &o);
    virtual int visitString(hif::String &o);
    virtual int visitUnsigned(hif::Unsigned &o);

    virtual int visitParameterAssign(ParameterAssign &o);

private:
    PreRefine_ranges(const PreRefine_ranges &);
    PreRefine_ranges &operator=(const PreRefine_ranges &);

    hif::semantics::ILanguageSemantics *_sem;
    hif::HifFactory _factory;
    /// @brief Check type span. The span of Array (not packed), Array (packed),
    /// Signed and Unsigned must be downto. If not, collect it for further
    /// fixes.
    /// @param typeSpan the span of the type to check.
    ///
    void _checkType(Range *typeSpan);
    /// @brief Revert a Value depending on Value's type span.
    /// All reverted span must have direction downto.
    /// @param indexToFix the index of the Value.
    /// @param refSpan is the Value's type span.
    ///
    void _fixIndex(Value *indexToFix, Range *refSpan, Value *prefix = nullptr);
    /// @brief Revert a Span of a Value depending on Value's type span.
    /// All reverted span must have direction downto.
    /// @param spanToFix is the Span of a Value.
    /// @param refSpan is the Value's type span.
    ///
    /// e.g.
    /// slice( VAR, range( 14, 0 ) )
    /// spanToFix is range( 14, 0 )
    /// refSpan is span of VAR type
    ///
    void _fixRange(Range *spanToFix, Range *refSpan, Value *prefix = nullptr);

    /// @brief Checks when a span must be reverted.
    bool _mustBeReverted(Range *r);
};
PreRefine_ranges::PreRefine_ranges(hif::semantics::ILanguageSemantics *sem)
    : _sem(sem)
    , _factory()
{
    // ntd
}
PreRefine_ranges::~PreRefine_ranges()
{
    // ntd
}

int PreRefine_ranges::visitAggregate(Aggregate &o)
{
    GuideVisitor::visitAggregate(o);

    Type *t = hif::semantics::getSemanticType(&o, _sem);
    if (t == nullptr) {
        messageError("type not found", &o, _sem);
    }

    Range *typeSpan = hif::typeGetSpan(t, _sem);

    for (BList<AggregateAlt>::iterator i = o.alts.begin(); i != o.alts.end(); ++i) {
        AggregateAlt *alt = *i;
        for (BList<Value>::iterator j = alt->indices.begin(); j != alt->indices.end(); ++j) {
            Value *ind = *j;
            if (dynamic_cast<Range *>(ind) != nullptr) {
                Range *r = static_cast<Range *>(ind);
                _fixRange(r, typeSpan);
            } else {
                _fixIndex(ind, typeSpan);
            }
        }
    }

    return 0;
}

int PreRefine_ranges::visitArray(Array &o)
{
    GuideVisitor::visitArray(o);
    _checkType(o.getSpan());
    return 0;
}

int PreRefine_ranges::visitBitvector(Bitvector &o)
{
    GuideVisitor::visitBitvector(o);
    _checkType(o.getSpan());
    return 0;
}

int PreRefine_ranges::visitCast(Cast &o)
{
    // Since casts in Verilog and VHDL always takes the rightmost bits,
    // we do not have to do any special management! :)
    // For example (no bug) check source and trace of verilog/custom/test_precision
    return GuideVisitor::visitCast(o);
}

int PreRefine_ranges::visitInt(Int &o)
{
    GuideVisitor::visitInt(o);
    _checkType(o.getSpan());
    return 0;
}

int PreRefine_ranges::visitMember(hif::Member &o)
{
    GuideVisitor::visitMember(o);

    Type *t         = hif::semantics::getSemanticType(o.getPrefix(), _sem);
    Range *typeSpan = hif::typeGetSpan(t, _sem, true);

    if (typeSpan == nullptr)
        return 0;

    _fixIndex(o.getIndex(), typeSpan, o.getPrefix());

    return 0;
}
int PreRefine_ranges::visitSlice(Slice &o)
{
    GuideVisitor::visitSlice(o);

    Type *t         = hif::semantics::getSemanticType(o.getPrefix(), _sem);
    Range *typeSpan = hif::typeGetSpan(t, _sem, true);

    if (typeSpan == nullptr)
        return 0;

    _fixRange(o.getSpan(), typeSpan, o.getPrefix());

    return 0;
}

int PreRefine_ranges::visitString(hif::String &o)
{
    GuideVisitor::visitString(o);
    _checkType(o.getSpanInformation());
    return 0;
}

int PreRefine_ranges::visitSigned(hif::Signed &o)
{
    GuideVisitor::visitSigned(o);
    _checkType(o.getSpan());
    return 0;
}
int PreRefine_ranges::visitUnsigned(hif::Unsigned &o)
{
    GuideVisitor::visitUnsigned(o);
    _checkType(o.getSpan());
    return 0;
}

int PreRefine_ranges::visitParameterAssign(ParameterAssign &o)
{
    GuideVisitor::visitParameterAssign(o);

    Parameter *param = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(param != nullptr, "Declaration not found", &o, _sem);

    if (param->getDirection() != hif::dir_out && param->getDirection() != hif::dir_inout)
        return 0;

    if (!hif::declarationIsPartOfStandard(param))
        return 0;

    Type *t = hif::semantics::getSemanticType(o.getValue(), _sem);
    messageAssert(t != nullptr, "Cannot type value", o.getValue(), _sem);

    Range *span = hif::typeGetSpan(t, _sem, true);
    if (!_mustBeReverted(span))
        return 0;

    Action *act = hif::getNearestParent<Action>(&o);
    messageAssert(act != nullptr && act->isInBList(), "Expected parent action", &o, _sem);
    GlobalAction *ga = dynamic_cast<GlobalAction *>(act->getParent());
    messageAssert(ga == nullptr, "Unexpected global action", act->getParent(), _sem);
    Assign *ass =
        _factory.assignment(hif::copy(o.getValue()), _factory.expression(op_reverse, hif::copy(o.getValue())));
    BList<Action>::iterator it(act);
    it.insert_after(ass);

    return 0;
}
void PreRefine_ranges::_checkType(Range *typeSpan)
{
    if (!_mustBeReverted(typeSpan))
        return;

    typeSpansToRevert.insert(typeSpan);
}
void PreRefine_ranges::_fixIndex(Value *indexToFix, Range *refSpan, Value *prefix)
{
    // Starting situation:
    // index(a)
    // type_span(x, y)
    //
    // We COULD have already reverted the type related to type_span, BUT
    // typeTree() has been called before starting the visit, so the
    // type_span read here is still the unreverted one! (Good, since we use
    // it to know when the index has to be fixed).
    //
    // The fix transforms index to:
    // index(max(x,y) + min(x,y) - a)

    if (!_mustBeReverted(refSpan))
        return;

    Value *maxBound = hif::manipulation::assureSyntacticType(hif::copy(hif::rangeGetMaxBound(refSpan)), _sem);
    Value *minBound = hif::manipulation::assureSyntacticType(hif::copy(hif::rangeGetMinBound(refSpan)), _sem);

    bool bothSet   = (maxBound != nullptr && minBound != nullptr);
    bool bothUnset = (maxBound == nullptr && minBound == nullptr);

    if (bothSet) {
        Expression *bound = new Expression(hif::op_plus, maxBound, minBound);

        Type *indexType = hif::semantics::getSemanticType(indexToFix, _sem);
        messageAssert(indexType != nullptr, "Cannot type index", indexToFix, _sem);

        Expression *newIndex = new Expression();
        indexToFix->replace(newIndex);
        // ensuring typing of expression adding cast of max bound to index type.
        // ref. design: verilog/vams_std_afem/multiplexer
        newIndex->setValue1(_factory.cast(hif::copy(indexType), bound));
        newIndex->setOperator(op_minus);
        newIndex->setValue2(indexToFix);
        hif::manipulation::assureSyntacticType(indexToFix, _sem);
    } else // both unset or mixed
    {
        messageAssert(prefix != nullptr, "Expected prefix", nullptr, _sem);
        // index(size(refSpan)-1 + min - a)
        // NOTE: size is mapped in op_abs
        Expression *sizeExpr = new Expression(hif::op_size, hif::copy(prefix));
        if (!bothUnset) {
            messageAssert(minBound != nullptr, "Expected min bound", refSpan, _sem);
            sizeExpr = _factory.expression(sizeExpr, op_plus, minBound);
        }
        sizeExpr = _factory.expression(sizeExpr, op_minus, _factory.intval(1));

        Expression *newIndex = new Expression();
        indexToFix->replace(newIndex);
        newIndex->setValue1(sizeExpr);
        newIndex->setOperator(op_minus);
        newIndex->setValue2(indexToFix);
        hif::manipulation::assureSyntacticType(indexToFix, _sem);
    }
}
void PreRefine_ranges::_fixRange(Range *spanToFix, Range *refSpan, Value *prefix)
{
    // Starting situation:
    // slice_span(a, b)
    // type_span(x, y)
    //
    // We COULD have already reverted the type related to type_span, BUT
    // typeTree() has been called before starting the visit, so the
    // type_span read here is still the unreverted one! (Good, since we use
    // it to know when the span has to be fixed).
    //
    // The fix transforms slice_span to:
    // slice_span(bound - a), (bound - b) )
    // with bound = max(x,y) + min(x,y)
    // The sum of the min is needed, to support spans not starting from zero.
    // It also reverts it when required.

    /// @warning May be required a cast on new expressions like _fixIndex.
    if (!_mustBeReverted(refSpan))
        return;

    Value *maxBound = hif::manipulation::assureSyntacticType(hif::copy(hif::rangeGetMaxBound(refSpan)), _sem);
    Value *minBound = hif::manipulation::assureSyntacticType(hif::copy(hif::rangeGetMinBound(refSpan)), _sem);

    bool bothSet   = (maxBound != nullptr && minBound != nullptr);
    bool bothUnset = (maxBound == nullptr && minBound == nullptr);
    messageAssert(bothSet || bothUnset, "Unsupporte case", refSpan, _sem);

    if (bothSet) {
        Expression *bound = new Expression(hif::op_plus, maxBound, minBound);

        Value *lb       = spanToFix->getLeftBound();
        lb              = hif::manipulation::assureSyntacticType(lb, _sem);
        Expression *elb = _factory.expression(hif::copy(bound), op_minus, lb);

        Value *rb       = spanToFix->getRightBound();
        rb              = hif::manipulation::assureSyntacticType(rb, _sem);
        Expression *erb = _factory.expression(bound, op_minus, rb);

        spanToFix->setDirection((spanToFix->getDirection() == dir_downto) ? dir_upto : dir_downto);
        spanToFix->setLeftBound(elb);
        spanToFix->setRightBound(erb);
    } else // both unset and mixed
    {
        messageAssert(prefix != nullptr, "Expected prefix", nullptr, _sem);
        Expression *sizeExpr = new Expression(hif::op_size, hif::copy(prefix));
        if (!bothUnset) {
            messageAssert(minBound != nullptr, "Expected min bound", refSpan, _sem);
            sizeExpr = _factory.expression(sizeExpr, op_plus, minBound);
        }

        Expression *sizeMinus1 = _factory.expression(sizeExpr, op_minus, _factory.intval(1));

        Value *lb       = spanToFix->getLeftBound();
        lb              = hif::manipulation::assureSyntacticType(lb, _sem);
        Expression *elb = new Expression();
        elb->setValue1(sizeMinus1);
        elb->setOperator(op_minus);
        elb->setValue2(lb);

        Value *rb       = spanToFix->getRightBound();
        rb              = hif::manipulation::assureSyntacticType(rb, _sem);
        Expression *erb = new Expression();
        erb->setValue1(hif::copy(sizeMinus1));
        erb->setOperator(op_minus);
        erb->setValue2(rb);

        spanToFix->setDirection((spanToFix->getDirection() == dir_downto) ? dir_upto : dir_downto);
        spanToFix->setLeftBound(elb);
        spanToFix->setRightBound(erb);
    }
}

bool PreRefine_ranges::_mustBeReverted(Range *r)
{
    if (r == nullptr)
        return false;
    bool isString   = (dynamic_cast<String *>(r->getParent()) != nullptr);
    // If enabling this:
    // - check systemc_semantics_methods and switch array bounds.
    // - check ref designs: vhdl/gaisler/can_oc and vhdl/custom/polar2rect.
    //bool isArray = (dynamic_cast <Array *>(r->getParent()) != nullptr);
    bool mustBeUpto = isString /*|| isArray*/;
    if (r->getDirection() == dir_downto && !mustBeUpto)
        return false;
    if (r->getDirection() == dir_upto && mustBeUpto)
        return false;

    return true;
}

void revertTypeSpans(hif::semantics::ILanguageSemantics * /*sem*/)
{
    for (std::set<Range *>::iterator it(typeSpansToRevert.begin()); it != typeSpansToRevert.end(); ++it) {
        Range *typeSpan = *it;

        typeSpan->setDirection((typeSpan->getDirection() == dir_downto) ? dir_upto : dir_downto);

        // Revert the span
        Value *tmp = typeSpan->getLeftBound();
        typeSpan->setLeftBound(typeSpan->getRightBound());
        typeSpan->setRightBound(tmp);
    }
}
} // namespace
void fixRangesDirection(hif::System *o, hif::semantics::ILanguageSemantics *sem)
{
    hif::application_utils::initializeLogHeader("HIF", "fixRangesDirection");

    _explicitCallsParameters(o, sem);

    // Calculate types for all objects, so further calls to getSemanticType will return
    // the "old" types. This is because we are going to change the type before
    // changing their use in Member and Slice.
    hif::semantics::typeTree(o, sem);

    PreRefine_ranges ranges(sem);
    o->acceptVisitor(ranges);

    revertTypeSpans(sem);
    typeSpansToRevert.clear();

    hif::semantics::UpdateDeclarationOptions opt;
    opt.dontSearch = true;

    // The already set types are now invalid.
    hif::semantics::resetTypes(o);
    hif::manipulation::flushInstanceCache();
    hif::semantics::flushTypeCacheEntries();
    hif::semantics::updateDeclarations(o, sem, opt);
    hif::manipulation::simplify(o, sem);

    hif::application_utils::restoreLogHeader();
}

} // namespace manipulation
} // namespace hif
