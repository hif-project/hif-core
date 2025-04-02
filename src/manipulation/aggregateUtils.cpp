/// @file aggregateUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/aggregateUtils.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

bool _getIndexOfAggregate(
    Value *ind,
    Value *min,
    Int *itype,
    std::uint64_t &vind,
    hif::semantics::ILanguageSemantics *sem)
{
    HifFactory _factory(sem);
    ind            = hif::manipulation::assureSyntacticType(ind, sem);
    Value *v       = _factory.expression(ind, op_minus, hif::copy(min));
    v              = simplify(v, sem);
    ConstValue *cv = dynamic_cast<ConstValue *>(v);
    if (cv == nullptr) {
        // cannot simplify
        delete v;
        return false;
    }

    IntValue *ival = dynamic_cast<IntValue *>(hif::manipulation::transformConstant(cv, itype, sem));
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

} // namespace
bool transformAggregateRollingAlts(Aggregate *obj, bool atLeastOne, hif::semantics::ILanguageSemantics * /*sem*/)
{
    if (obj->alts.empty())
        return false;
    bool hasOthers = (obj->getOthers() != nullptr);

    Value *refAltValue = obj->alts.front()->getValue();
    bool allAltsEquals = !hasOthers; // Note: works only without others.
    std::set<AggregateAlt *> altsAsOthers;
    for (BList<AggregateAlt>::iterator i = obj->alts.begin(); i != obj->alts.end(); ++i) {
        AggregateAlt *alt = *i;

        // check if the alt value is equal to the others value.
        // in this case alt is candidate to be removed.
        if (hif::equals(obj->getOthers(), alt->getValue())) {
            altsAsOthers.insert(alt);
        }

        if (!allAltsEquals || alt == obj->alts.front()) {
            // Skip following checks.
            // since:
            // - already unmatched values.
            // - aggregate has others.
            // - first iteration is useless
            continue;
        } else {
            // update alts equals value
            allAltsEquals = hif::equals(refAltValue, alt->getValue());
        }
    }

    // TODO: What is the policy to don't fall into unrolling case?
    // at the moment simplify only if all alts can be removed.
    bool removesAltsAsOthers = hasOthers;
    if (atLeastOne)
        removesAltsAsOthers &= (altsAsOthers.size() > 0);
    else
        removesAltsAsOthers &= (altsAsOthers.size() == obj->alts.size());

    if (!removesAltsAsOthers && !allAltsEquals)
        return false;

    if (removesAltsAsOthers) {
        for (std::set<AggregateAlt *>::iterator i = altsAsOthers.begin(); i != altsAsOthers.end(); ++i) {
            AggregateAlt *alt = *i;
            alt->replace(nullptr);
            delete alt;
        }
    } else if (allAltsEquals) {
        obj->setOthers(hif::copy(refAltValue));
        obj->alts.clear();
    }

    return true;
}

bool transformAggregateUnrollingAlts(
    Aggregate *obj,
    std::uint64_t threshold,
    hif::semantics::ILanguageSemantics *sem,
    bool force)
{
    if (obj->getOthers() == nullptr)
        return false;
    Type *t    = hif::semantics::getSemanticType(obj, sem);
    Type *bt   = hif::semantics::getBaseType(t, false, sem, false);
    Array *arr = dynamic_cast<Array *>(bt);
    if (arr == nullptr)
        return false;

    std::uint64_t ss = 0;
    if (force) {
        Range *arrSpan = arr->getSpan();
        Range *r       = hif::copy(arrSpan);
        arr->setSpan(r);
        hif::manipulation::SimplifyOptions sopt;
        sopt.simplify_constants = true;
        hif::manipulation::simplify(r, sem, sopt);
        ss = hif::semantics::spanGetBitwidth(r, sem);
        arr->setSpan(arrSpan);
        delete r;
    } else {
        ss = hif::semantics::spanGetBitwidth(arr->getSpan(), sem);
    }

    if (ss == 0)
        return false;

    HifFactory _factory(sem);
    Value *min = hif::rangeGetMinBound(arr->getSpan());
    min        = hif::manipulation::assureSyntacticType(hif::copy(min), sem);

    typedef std::map<std::uint64_t, Value *> IndexMap;
    IndexMap indexMap;
    Int *itype = _factory.integer();

    for (BList<AggregateAlt>::iterator i = obj->alts.begin(); i != obj->alts.end(); ++i) {
        AggregateAlt *a = *i;
        for (BList<Value>::iterator j = a->indices.begin(); j != a->indices.end(); ++j) {
            Value *ind = *j;
            if (dynamic_cast<Range *>(ind) != nullptr) {
                Range *r              = static_cast<Range *>(ind);
                std::uint64_t rr = hif::semantics::spanGetBitwidth(r, sem);
                if (rr == 0) {
                    delete min;
                    delete itype;
                    return false;
                }
                Value *minRangeBound    = hif::rangeGetMinBound(r);
                Value *maxRangeBound    = hif::rangeGetMaxBound(r);
                std::uint64_t vmin = 0;
                std::uint64_t vmax = 0;
                if (!_getIndexOfAggregate(hif::copy(minRangeBound), min, itype, vmin, sem) ||
                    !_getIndexOfAggregate(hif::copy(maxRangeBound), min, itype, vmax, sem)) {
                    delete min;
                    delete itype;
                    return false;
                }

                for (std::uint64_t k = vmin; k <= vmax; ++k) {
                    indexMap[k] = a->getValue();
                }
            } else {
                std::uint64_t vind = 0;
                if (!_getIndexOfAggregate(hif::copy(ind), min, itype, vind, sem)) {
                    delete min;
                    delete itype;
                    return false;
                }
                indexMap[vind] = a->getValue();
            }
        }
    }

    delete itype;

    if (ss > threshold) {
        delete min;
        return false;
    }

    BList<AggregateAlt> unrolled;
    for (std::uint64_t i = 0; i < ss; ++i) {
        Expression *ind = _factory.expression(hif::copy(min), op_plus, _factory.intval(static_cast<std::int64_t>(i)));

        Value *v = nullptr;
        if (indexMap.find(i) == indexMap.end()) {
            // add others
            v = hif::copy(obj->getOthers());
        } else {
            v = hif::copy(indexMap[i]);
        }

        AggregateAlt *alt = new AggregateAlt();
        alt->indices.push_back(ind);
        alt->setValue(v);
        unrolled.push_back(alt);
    }

    obj->alts.clear();
    delete obj->setOthers(nullptr);
    obj->alts.merge(unrolled);

    delete min;

    return true;
}
} // namespace manipulation
} // namespace hif
