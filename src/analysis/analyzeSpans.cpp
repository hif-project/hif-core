/// @file analyzeSpans.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>

#include "hif/analysis/analyzeSpans.hpp"
#include "hif/application_utils/application_utils.hpp"
#include "hif/manipulation/manipulation.hpp"

namespace hif
{

namespace analysis
{

namespace /* anon */
{

using ValueIndex = AnalyzeSpansResult::ValueIndex;
using IndexKind  = AnalyzeSpansResult::IndexKind;
using ValueMap   = AnalyzeSpansResult::ValueMap;

// ///////////////////////////////////////////////////////////////////
// SpanAnalyzer
// ///////////////////////////////////////////////////////////////////
class SpanAnalyzer
{
public:
    SpanAnalyzer(hif::semantics::ILanguageSemantics *sem);
    ~SpanAnalyzer();

    auto analyzeSpans(Type *spanType, const IndexMap &indexMap, Value *others) -> bool;

    auto getResult() -> AnalyzeSpansResult &;

private:
    auto _categorizeSpans(Type *spanType, const IndexMap &indexMap, Value *others) -> bool;
    auto _fillHoles(Value *others) -> bool;
    void _mergeIndexes();
    void _refinePrefixedReferences();
    void _refineAllOthers(Value *others);

    void _mergeExpressionExpression(const ValueIndex &prec, const ValueIndex &curr);
    void _mergeExpressionRange(const ValueIndex &prec, const ValueIndex &curr);
    void _mergeExpressionSlice(const ValueIndex &prec, const ValueIndex &curr);
    void _mergeRangeExpression(const ValueIndex &prec, const ValueIndex &curr);
    void _mergeRangeRange(const ValueIndex &prec, const ValueIndex &curr);
    void _mergeRangeSlice(const ValueIndex &prec, const ValueIndex &curr);
    void _mergeSliceExpression(const ValueIndex &prec, const ValueIndex &curr);
    void _mergeSliceRange(const ValueIndex &prec, const ValueIndex &curr);
    void _mergeSliceSlice(const ValueIndex &prec, const ValueIndex &curr);

    auto _compareBoundIndexes(Value *ind1, Value *ind2, std::int64_t &res) -> bool;
    auto _getIndexConstvalue(Value *ind, Value *min, std::uint64_t &vind) -> bool;
    auto _getSpanMinIndex(Range *r) -> Value *;
    auto _getSpanMaxIndex(Range *r) -> Value *;
    auto _setSpanMinIndex(Range *r, Value *v) -> Value *;
    auto _setSpanMaxIndex(Range *r, Value *v) -> Value *;

    hif::semantics::ILanguageSemantics *_sem;
    hif::HifFactory _factory;
    AnalyzeSpansResult _result;
    Int *_intType;
    hif::RangeDirection _refDirection{hif::dir_downto};

    SpanAnalyzer(const SpanAnalyzer &)                     = delete;
    auto operator=(const SpanAnalyzer &) -> SpanAnalyzer & = delete;
};

SpanAnalyzer::SpanAnalyzer(hif::semantics::ILanguageSemantics *sem)
    : _sem(sem)
    , _factory(sem)
    , _intType(_factory.integer())

{
    // ntd
}
SpanAnalyzer::~SpanAnalyzer() { delete _intType; }

auto SpanAnalyzer::analyzeSpans(Type *spanType, const IndexMap &indexMap, Value *others) -> bool
{
    Range *spanTypeRange = hif::typeGetSpan(spanType, _sem);
    if (spanTypeRange == nullptr) {
        return false;
    }
    _refDirection = spanTypeRange->getDirection();

    if (!_categorizeSpans(spanType, indexMap, others)) {
        return false;
    }
    if (!_fillHoles(others)) {
        return false;
    }
    _mergeIndexes();

    // Refine fixes
    _refinePrefixedReferences();
    _refineAllOthers(others);

    return true;
}

auto SpanAnalyzer::getResult() -> AnalyzeSpansResult & { return _result; }

auto SpanAnalyzer::_categorizeSpans(Type *spanType, const IndexMap &indexMap, Value *others) -> bool
{
    Range *span = hif::typeGetSpan(spanType, _sem);
    Value *min  = hif::rangeGetMinBound(span);

    for (const auto &i : indexMap) {
        const IndexInfo &info = i.first;
        Value *value          = i.second;
        if (info.expression != nullptr) {
            std::uint64_t vind = 0;
            if (!_getIndexConstvalue(hif::copy(info.expression), min, vind)) {
                return false;
            }
            _result.maxBound = std::max(vind, _result.maxBound);
            ValueIndex ind(AnalyzeSpansResult::INDEX_EXPRESSION, vind, 0);
            _result.resultMap[ind] = hif::copy(value);
        } else if (info.range != nullptr) {
            Value *minRangeBound = hif::rangeGetMinBound(info.range);
            Value *maxRangeBound = hif::rangeGetMaxBound(info.range);
            std::uint64_t vmin   = 0;
            std::uint64_t vmax   = 0;
            if (!_getIndexConstvalue(hif::copy(minRangeBound), min, vmin) ||
                !_getIndexConstvalue(hif::copy(maxRangeBound), min, vmax)) {
                return false;
            }
            _result.maxBound = std::max(vmax, _result.maxBound);
            ValueIndex ind(AnalyzeSpansResult::INDEX_RANGE, vmin, vmax);
            _result.resultMap[ind] = hif::copy(value);
        } else if (info.slice != nullptr) {
            Value *minRangeBound = hif::rangeGetMinBound(info.slice);
            Value *maxRangeBound = hif::rangeGetMaxBound(info.slice);
            std::uint64_t vmin   = 0;
            std::uint64_t vmax   = 0;
            if (!_getIndexConstvalue(hif::copy(minRangeBound), min, vmin) ||
                !_getIndexConstvalue(hif::copy(maxRangeBound), min, vmax)) {
                return false;
            }
            _result.maxBound = std::max(vmax, _result.maxBound);

            ValueIndex ind(AnalyzeSpansResult::INDEX_SLICE, vmin, vmax);
            _result.resultMap[ind] = hif::copy(value);
        } else {
            messageError("Expected type", nullptr, _sem);
        }
    }
    std::uint64_t spanBitwidth = hif::semantics::spanGetBitwidth(span, _sem);
    if (spanBitwidth != 0) {
        if (_result.maxBound >= spanBitwidth) {
            messageError("Wrong input indexes.", nullptr, nullptr);
        }
        if (spanBitwidth != _result.maxBound + 1) {
            ValueIndex ind(AnalyzeSpansResult::INDEX_RANGE, _result.maxBound + 1, spanBitwidth - 1);
            _result.resultMap[ind] = hif::copy(others);
        }
        _result.allSpecified = true;
    }
    return true;
}

auto SpanAnalyzer::_fillHoles(Value *others) -> bool
{
    for (std::uint64_t i = 0; i < _result.maxBound; ++i) {
        ValueIndex ind(AnalyzeSpansResult::INDEX_EXPRESSION, i, 0);
        if (_result.resultMap.find(ind) != _result.resultMap.end()) {
            continue;
        }
        if (others == nullptr) {
            return false;
        }
        _result.resultMap[ind] = hif::copy(others);
    }
    return true;
}

void SpanAnalyzer::_mergeIndexes()
{
    if (_result.resultMap.empty()) {
        return;
    }
    auto precElement = _result.resultMap.begin();
    auto currElement = _result.resultMap.begin();
    ++currElement;
    for (; currElement != _result.resultMap.end();) {
        ValueIndex prec = precElement->first;
        ValueIndex curr = currElement->first;
        // _mergeXXX() can change the resultMap!
        ++currElement;
        if (prec.getKind() == AnalyzeSpansResult::INDEX_EXPRESSION &&
            curr.getKind() == AnalyzeSpansResult::INDEX_EXPRESSION) {
            _mergeExpressionExpression(prec, curr);
        } else if (
            prec.getKind() == AnalyzeSpansResult::INDEX_EXPRESSION &&
            curr.getKind() == AnalyzeSpansResult::INDEX_RANGE) {
            _mergeExpressionRange(prec, curr);
        } else if (
            prec.getKind() == AnalyzeSpansResult::INDEX_EXPRESSION &&
            curr.getKind() == AnalyzeSpansResult::INDEX_SLICE) {
            _mergeExpressionSlice(prec, curr);
        } else if (
            prec.getKind() == AnalyzeSpansResult::INDEX_RANGE &&
            curr.getKind() == AnalyzeSpansResult::INDEX_EXPRESSION) {
            _mergeRangeExpression(prec, curr);
        } else if (
            prec.getKind() == AnalyzeSpansResult::INDEX_RANGE && curr.getKind() == AnalyzeSpansResult::INDEX_RANGE) {
            _mergeRangeRange(prec, curr);
        } else if (
            prec.getKind() == AnalyzeSpansResult::INDEX_RANGE && curr.getKind() == AnalyzeSpansResult::INDEX_SLICE) {
            _mergeRangeSlice(prec, curr);
        } else if (
            prec.getKind() == AnalyzeSpansResult::INDEX_SLICE &&
            curr.getKind() == AnalyzeSpansResult::INDEX_EXPRESSION) {
            _mergeSliceExpression(prec, curr);
        } else if (
            prec.getKind() == AnalyzeSpansResult::INDEX_SLICE && curr.getKind() == AnalyzeSpansResult::INDEX_RANGE) {
            _mergeSliceRange(prec, curr);
        } else if (
            prec.getKind() == AnalyzeSpansResult::INDEX_SLICE && curr.getKind() == AnalyzeSpansResult::INDEX_SLICE) {
            _mergeSliceSlice(prec, curr);
        }
        precElement = currElement;
        --precElement;
    }
}

void SpanAnalyzer::_refinePrefixedReferences()
{
    for (auto &i : _result.resultMap) {
        const ValueIndex &index = i.first;
        Value *value            = i.second;

        if (index.getKind() == AnalyzeSpansResult::INDEX_EXPRESSION) {
            // Remove possible member when prefix with size 1
            auto *memVal = dynamic_cast<Member *>(value);
            if (memVal == nullptr) {
                continue;
            }
            Value *memPrefix    = memVal->getPrefix();
            Type *memPrefixType = hif::semantics::getSemanticType(memPrefix, _sem);
            if (memPrefixType == nullptr) {
                continue;
            }
            std::uint64_t bw = hif::semantics::typeGetSpanBitwidth(memPrefixType, _sem);
            if (bw != 1ULL) {
                continue;
            }
            memVal->setPrefix(nullptr);
            delete memVal;
            _result.resultMap[index] = memPrefix;
        } else if (index.getKind() == AnalyzeSpansResult::INDEX_SLICE) {
            // Remove possible slice when
            auto *sliceVal = dynamic_cast<Slice *>(value);
            if (sliceVal == nullptr) {
                continue;
            }
            Value *slicePrefix    = sliceVal->getPrefix();
            Type *slicePrefixType = hif::semantics::getSemanticType(slicePrefix, _sem);
            if (slicePrefixType == nullptr) {
                continue;
            }
            std::uint64_t bw = hif::semantics::typeGetSpanBitwidth(slicePrefixType, _sem);
            if (bw == 0) {
                continue;
            }
            if (bw != index.getSize()) {
                continue;
            }
            sliceVal->setPrefix(nullptr);
            delete sliceVal;
            _result.resultMap[index] = slicePrefix;
        }
    }
}

void SpanAnalyzer::_refineAllOthers(Value *others)
{
    for (auto &i : _result.resultMap) {
        const ValueIndex &index = i.first;
        Value *value            = i.second;
        if (index.getKind() == AnalyzeSpansResult::INDEX_SLICE) {
            return;
        }
        if (!hif::equals(others, value)) {
            return;
        }
    }
    // All indexes equals to others
    for (auto &i : _result.resultMap) {
        Value *value = i.second;
        delete value;
    }
    _result.resultMap.clear();
    ValueIndex allRange(AnalyzeSpansResult::INDEX_RANGE, 0, _result.maxBound);
    _result.resultMap[allRange] = hif::copy(others);
    _result.allOthers           = true;
}

void SpanAnalyzer::_mergeExpressionExpression(const ValueIndex &prec, const ValueIndex &curr)
{
    Value *precVal = _result.resultMap[prec];
    Value *currVal = _result.resultMap[curr];

    // If both equals: merge in range.
    if (hif::equals(precVal, currVal)) {
        ValueIndex res(AnalyzeSpansResult::INDEX_RANGE, prec.getMin(), curr.getMax());
        _result.resultMap.erase(prec);
        _result.resultMap.erase(curr);
        _result.resultMap[res] = currVal;
        delete precVal;
        return;
    }
    // If both member with continguos indexes, merge in slice.
    auto *memPrec = dynamic_cast<Member *>(precVal);
    auto *memCurr = dynamic_cast<Member *>(currVal);
    if (memPrec == nullptr || memCurr == nullptr) {
        return;
    }
    if (!hif::equals(memPrec->getPrefix(), memCurr->getPrefix())) {
        return;
    }
    std::int64_t compare = 0;
    if (!_compareBoundIndexes(hif::copy(memPrec->getIndex()), hif::copy(memCurr->getIndex()), compare)) {
        return;
    }
    if (compare != -1 && compare != 1) {
        return;
    }
    if (compare == -1) {
        return;
    }
    ValueIndex res(AnalyzeSpansResult::INDEX_SLICE, prec.getMin(), curr.getMax());
    _result.resultMap.erase(prec);
    _result.resultMap.erase(curr);
    auto *slice = new Slice();
    slice->setPrefix(memCurr->setPrefix(nullptr));
    auto *r = new Range();
    r->setDirection(_refDirection);
    _setSpanMinIndex(r, memPrec->setIndex(nullptr));
    _setSpanMaxIndex(r, memCurr->setIndex(nullptr));
    _result.resultMap[res] = slice;
    delete memPrec;
    delete memCurr;
}

void SpanAnalyzer::_mergeExpressionRange(const ValueIndex &prec, const ValueIndex &curr)
{
    Value *precVal = _result.resultMap[prec];
    Value *currVal = _result.resultMap[curr];

    if (hif::equals(precVal, currVal)) {
        ValueIndex res(AnalyzeSpansResult::INDEX_RANGE, prec.getMin(), curr.getMax());
        _result.resultMap.erase(prec);
        _result.resultMap.erase(curr);
        _result.resultMap[res] = currVal;
        delete precVal;
        return;
    }
}

void SpanAnalyzer::_mergeExpressionSlice(const ValueIndex &prec, const ValueIndex &curr)
{
    Value *precVal  = _result.resultMap[prec];
    Value *currVal  = _result.resultMap[curr];
    auto *memPrec   = dynamic_cast<Member *>(precVal);
    auto *sliceCurr = dynamic_cast<Slice *>(currVal);

    if (memPrec == nullptr || sliceCurr == nullptr) {
        return;
    }
    if (!hif::equals(memPrec->getPrefix(), sliceCurr->getPrefix())) {
        return;
    }
    std::int64_t compare = 0;
    if (!_compareBoundIndexes(
            hif::copy(memPrec->getIndex()), hif::copy(_getSpanMinIndex(sliceCurr->getSpan())), compare)) {
        return;
    }
    if (compare != -1 && compare != 1) {
        return;
    }
    ValueIndex res(AnalyzeSpansResult::INDEX_SLICE, prec.getMin(), curr.getMax());
    _result.resultMap.erase(prec);
    _result.resultMap.erase(curr);
    delete _setSpanMinIndex(sliceCurr->getSpan(), memPrec->setIndex(nullptr));
    _result.resultMap[res] = sliceCurr;
    delete memPrec;
}

void SpanAnalyzer::_mergeRangeExpression(const ValueIndex &prec, const ValueIndex &curr)
{
    Value *precVal = _result.resultMap[prec];
    Value *currVal = _result.resultMap[curr];

    if (!hif::equals(precVal, currVal)) {
        return;
    }
    ValueIndex res(AnalyzeSpansResult::INDEX_RANGE, prec.getMin(), curr.getMax());
    _result.resultMap.erase(prec);
    _result.resultMap.erase(curr);
    _result.resultMap[res] = currVal;
    delete precVal;
}

void SpanAnalyzer::_mergeRangeRange(const ValueIndex &prec, const ValueIndex &curr)
{
    Value *precVal = _result.resultMap[prec];
    Value *currVal = _result.resultMap[curr];

    if (!hif::equals(precVal, currVal)) {
        return;
    }
    ValueIndex res(AnalyzeSpansResult::INDEX_RANGE, prec.getMin(), curr.getMax());
    _result.resultMap.erase(prec);
    _result.resultMap.erase(curr);
    _result.resultMap[res] = currVal;
    delete precVal;
}

void SpanAnalyzer::_mergeRangeSlice(const ValueIndex & /*prec*/, const ValueIndex & /*curr*/)
{
    // cannot be merged ...
}

void SpanAnalyzer::_mergeSliceExpression(const ValueIndex &prec, const ValueIndex &curr)
{
    Value *precVal  = _result.resultMap[prec];
    Value *currVal  = _result.resultMap[curr];
    auto *slicePrec = dynamic_cast<Slice *>(precVal);
    auto *memCurr   = dynamic_cast<Member *>(currVal);

    if (slicePrec == nullptr || memCurr == nullptr) {
        return;
    }
    if (!hif::equals(slicePrec->getPrefix(), memCurr->getPrefix())) {
        return;
    }
    std::int64_t compare = 0;
    if (!_compareBoundIndexes(
            hif::copy(_getSpanMaxIndex(slicePrec->getSpan())), hif::copy(memCurr->getIndex()), compare)) {
        return;
    }
    if (compare != -1 && compare != 1) {
        return;
    }
    ValueIndex res(AnalyzeSpansResult::INDEX_SLICE, prec.getMin(), curr.getMax());
    _result.resultMap.erase(prec);
    _result.resultMap.erase(curr);
    delete _setSpanMaxIndex(slicePrec->getSpan(), memCurr->setIndex(nullptr));
    _result.resultMap[res] = slicePrec;
    delete memCurr;
}

void SpanAnalyzer::_mergeSliceRange(const ValueIndex & /*prec*/, const ValueIndex & /*curr*/)
{
    // cannot be merged ...
}

void SpanAnalyzer::_mergeSliceSlice(const ValueIndex &prec, const ValueIndex &curr)
{
    Value *precVal  = _result.resultMap[prec];
    Value *currVal  = _result.resultMap[curr];
    auto *slicePrec = dynamic_cast<Slice *>(precVal);
    auto *sliceCurr = dynamic_cast<Slice *>(currVal);

    if (slicePrec == nullptr || sliceCurr == nullptr) {
        return;
    }
    if (!hif::equals(slicePrec->getPrefix(), sliceCurr->getPrefix())) {
        return;
    }
    std::int64_t compare = 0;
    if (!_compareBoundIndexes(
            hif::copy(_getSpanMaxIndex(slicePrec->getSpan())), hif::copy(_getSpanMinIndex(sliceCurr->getSpan())),
            compare)) {
        return;
    }
    if (compare != -1 && compare != 1) {
        return;
    }
    ValueIndex res(AnalyzeSpansResult::INDEX_SLICE, prec.getMin(), curr.getMax());
    _result.resultMap.erase(prec);
    _result.resultMap.erase(curr);
    delete _setSpanMinIndex(sliceCurr->getSpan(), _setSpanMinIndex(slicePrec->getSpan(), nullptr));
    _result.resultMap[res] = sliceCurr;
    delete slicePrec;
}

auto SpanAnalyzer::_compareBoundIndexes(Value *ind1, Value *ind2, std::int64_t &res) -> bool
{
    ind1     = hif::manipulation::assureSyntacticType(ind1, _sem);
    ind2     = hif::manipulation::assureSyntacticType(ind2, _sem);
    Value *v = _factory.expression(ind2, op_minus, ind1);
    v        = hif::manipulation::simplify(v, _sem);
    auto *cv = dynamic_cast<ConstValue *>(v);
    if (cv == nullptr) {
        // cannot simplify
        delete v;
        return false;
    }
    auto *ival = dynamic_cast<IntValue *>(hif::manipulation::transformConstant(cv, _intType, _sem));
    if (ival == nullptr) {
        // cannot simplify
        delete cv;
        return false;
    }
    res = ival->getValue();
    delete ival;
    return true;
}

auto SpanAnalyzer::_getIndexConstvalue(Value *ind, Value *min, std::uint64_t &vind) -> bool
{
    ind      = hif::manipulation::assureSyntacticType(ind, _sem);
    Value *v = _factory.expression(ind, op_minus, hif::copy(min));
    v        = hif::manipulation::simplify(v, _sem);
    auto *cv = dynamic_cast<ConstValue *>(v);
    if (cv == nullptr) {
        // cannot simplify
        delete v;
        return false;
    }
    auto *ival = dynamic_cast<IntValue *>(hif::manipulation::transformConstant(cv, _intType, _sem));
    if (ival == nullptr) {
        // cannot simplify
        delete cv;
        return false;
    }
    vind = static_cast<std::uint64_t>(ival->getValue());
    delete ival;
    delete cv;
    return true;
}

auto SpanAnalyzer::_getSpanMinIndex(Range *r) -> Value *
{
    if (_refDirection == hif::dir_upto) {
        return r->getLeftBound();
    } // dir_downto
    return r->getRightBound();
}

auto SpanAnalyzer::_getSpanMaxIndex(Range *r) -> Value *
{
    if (_refDirection == hif::dir_upto) {
        return r->getRightBound();
    } // dir_downto
    return r->getLeftBound();
}

auto SpanAnalyzer::_setSpanMinIndex(Range *r, Value *v) -> Value *
{
    if (_refDirection == hif::dir_upto) {
        return r->setLeftBound(v);
    } // dir_downto
    return r->setRightBound(v);
}

auto SpanAnalyzer::_setSpanMaxIndex(Range *r, Value *v) -> Value *
{
    if (_refDirection == hif::dir_upto) {
        return r->setRightBound(v);
    } // dir_downto
    return r->setLeftBound(v);
}
} // namespace

// ///////////////////////////////////////////////////////////////////
// IndexInfo
// ///////////////////////////////////////////////////////////////////

auto IndexInfo::operator<(const IndexInfo &o) const -> bool { return (this < &o); }
// ///////////////////////////////////////////////////////////////////
// ValueIndex
// ///////////////////////////////////////////////////////////////////

ValueIndex::ValueIndex()
    : _kind(AnalyzeSpansResult::INDEX_EXPRESSION)
    , _index(0)
    , _minRangeIndex(0)
    , _maxRangeIndex(0)
    , _minSliceIndex(0)
    , _maxSliceIndex(0)
{
}

ValueIndex::ValueIndex(const IndexKind kind, std::uint64_t min, std::uint64_t max)
    : _kind(kind)
    , _index(min)
    , _minRangeIndex(min)
    , _maxRangeIndex(max)
    , _minSliceIndex(min)
    , _maxSliceIndex(max)
{
    // This supports range and span with equal bounds.
    if (min == max) {
        _kind = AnalyzeSpansResult::INDEX_EXPRESSION;
    }
}

void ValueIndex::swap(ValueIndex &other) noexcept
{
    std::swap(_kind, other._kind);
    std::swap(_index, other._index);
    std::swap(_minRangeIndex, other._minRangeIndex);
    std::swap(_maxRangeIndex, other._maxRangeIndex);
    std::swap(_minSliceIndex, other._minSliceIndex);
    std::swap(_maxSliceIndex, other._maxSliceIndex);
}

auto ValueIndex::operator<(const ValueIndex &other) const -> bool
{
    std::uint64_t thisMin  = getMin();
    std::uint64_t thisMax  = getMax();
    std::uint64_t otherMin = other.getMin();
    std::uint64_t otherMax = other.getMax();

    if (thisMax < otherMin) {
        return true;
    }
    if (thisMin > otherMax) {
        return false;
    }

    // overlapping
    return false;
}

auto ValueIndex::getKind() const -> IndexKind { return _kind; }

auto ValueIndex::getMax() const -> std::uint64_t
{
    if (_kind == AnalyzeSpansResult::INDEX_EXPRESSION) {
        return _index;
    }
    if (_kind == AnalyzeSpansResult::INDEX_RANGE) {
        return _maxRangeIndex;
    } /*if (kind == AnalyzeSpansResult::INDEX_SLICE)*/
    return _maxSliceIndex;
}

auto ValueIndex::getMin() const -> std::uint64_t
{
    if (_kind == AnalyzeSpansResult::INDEX_EXPRESSION) {
        return _index;
    }
    if (_kind == AnalyzeSpansResult::INDEX_RANGE) {
        return _minRangeIndex;
    } /*if (kind == AnalyzeSpansResult::INDEX_SLICE)*/
    return _minSliceIndex;
}

auto ValueIndex::getSize() const -> std::uint64_t { return (getMax() - getMin() + 1); }
// ///////////////////////////////////////////////////////////////////
// AnalyzeSpansResult
// ///////////////////////////////////////////////////////////////////

AnalyzeSpansResult::~AnalyzeSpansResult()
{
    for (auto &i : resultMap) {
        Value *value = i.second;
        delete value;
    }
    resultMap.clear();
}

AnalyzeSpansResult::AnalyzeSpansResult(AnalyzeSpansResult &&other) noexcept
    : resultMap(std::move(other.resultMap))
    , maxBound(other.maxBound)
    , allSpecified(other.allSpecified)
    , allOthers(other.allOthers)
{
    other.resultMap.clear();
    other.maxBound     = 0;
    other.allSpecified = false;
    other.allOthers    = false;
}

auto AnalyzeSpansResult::operator=(AnalyzeSpansResult &&other) noexcept -> AnalyzeSpansResult &
{
    if (this != &other) {
        // Clean up current resources
        for (auto &i : resultMap) {
            delete i.second;
        }
        resultMap.clear();

        // Move from other
        resultMap    = std::move(other.resultMap);
        maxBound     = other.maxBound;
        allSpecified = other.allSpecified;
        allOthers    = other.allOthers;

        other.resultMap.clear();
        other.maxBound     = 0;
        other.allSpecified = false;
        other.allOthers    = false;
    }
    return *this;
}

// ///////////////////////////////////////////////////////////////////
// analyzeSpans()
// ///////////////////////////////////////////////////////////////////

auto analyzeSpans(
    Type *spanType,
    const IndexMap &indexMap,
    hif::semantics::ILanguageSemantics *sem,
    Value *others,
    AnalyzeSpansResult &result) -> bool
{
    hif::application_utils::initializeLogHeader("HIF", "SpanAnalyzer");
    SpanAnalyzer analyzer(sem);
    bool ret = analyzer.analyzeSpans(spanType, indexMap, others);
    hif::application_utils::restoreLogHeader();

    if (!ret) {
        return ret;
    }
    result = std::move(analyzer.getResult());
    return ret;
}

} // namespace analysis

} // namespace hif
