/// @file createConcatFromSpans.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/createConcatFromSpans.hpp"
#include "hif/application_utils/application_utils.hpp"
#include "hif/hif_utils/hif_utils.hpp"

namespace hif
{
namespace manipulation
{
namespace /*anon*/
{

typedef hif::analysis::AnalyzeSpansResult AnalyzeSpansResult;
typedef AnalyzeSpansResult::ValueMap ValueMap;
typedef AnalyzeSpansResult::ValueIndex ValueIndex;

Value *_makeConcat(
    Value *resultValue,
    Value *value,
    const hif::RangeDirection spanDirection,
    const bool copy,
    hif::semantics::ILanguageSemantics *sem)
{
    HifFactory factory(sem);
    if (copy)
        value = hif::copy(value);

    if (resultValue == nullptr) {
        return value;
    }

    if (spanDirection == hif::dir_upto) {
        return factory.expression(resultValue, hif::op_concat, value);
    } else {
        return factory.expression(value, hif::op_concat, resultValue);
    }
}

} // namespace

Value *createConcatFromSpans(
    Type *spanType,
    const hif::analysis::IndexMap &indexMap,
    hif::semantics::ILanguageSemantics *sem,
    Value *others)
{
    AnalyzeSpansResult res;
    const bool ret = hif::analysis::analyzeSpans(spanType, indexMap, sem, others, res);
    if (!ret)
        return nullptr;
    if (!res.allSpecified && others == nullptr)
        return nullptr;

    Range *r = hif::typeGetSpan(spanType, sem);
    if (r == nullptr)
        return nullptr;
    const hif::RangeDirection spanDirection = r->getDirection();

    Value *resultValue = nullptr;
    for (ValueMap::iterator i = res.resultMap.begin(); i != res.resultMap.end(); ++i) {
        const ValueIndex &index = i->first;
        Value *value            = i->second;
        res.resultMap[index]    = nullptr;
        switch (index.getKind()) {
        case AnalyzeSpansResult::INDEX_EXPRESSION:
        case AnalyzeSpansResult::INDEX_SLICE: {
            resultValue = _makeConcat(resultValue, value, spanDirection, false, sem);
            break;
        }
        case AnalyzeSpansResult::INDEX_RANGE: {
            for (std::uint64_t j = index.getMin(); j <= index.getMax(); ++j) {
                resultValue = _makeConcat(resultValue, value, spanDirection, true, sem);
            }
            delete value;
            break;
        }
        default: {
            messageError("Unexpected case", nullptr, nullptr);
        }
        }
    }

    if (res.allSpecified)
        return resultValue;

    HifFactory factory(sem);
    Type *ct   = hif::copy(spanType);
    Range *rr  = hif::typeGetSpan(ct, sem);
    Value *min = hif::rangeSetMinBound(rr, new IntValue(0));
    Value *max = hif::rangeGetMaxBound(rr);
    max->replace(nullptr);
    Value *diff = factory.expression(max, hif::op_minus, min);
    diff        = factory.expression(diff, hif::op_minus, factory.intval(static_cast<std::int64_t>(res.maxBound + 1)));
    hif::rangeSetMaxBound(rr, diff);

    Aggregate *agg = new Aggregate();
    agg->setOthers(hif::copy(others));

    resultValue = _makeConcat(resultValue, factory.cast(ct, agg), spanDirection, false, sem);

    return resultValue;
}

} // namespace manipulation
} // namespace hif
