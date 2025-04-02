/// @file spanGetSize.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/spanGetSize.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

} // namespace
Value *typeGetSpanSize(Type *t, ILanguageSemantics *refLang)
{
    Range *range = hif::typeGetSpan(t, refLang);
    if (range == nullptr)
        return nullptr;

    return hif::semantics::spanGetSize(range, refLang);
}
Value *
spanGetSize(Range *r, ILanguageSemantics *sem, const bool simplify, const hif::manipulation::SimplifyOptions &opts)
{
    messageAssert(r != nullptr, "Passed nullptr range", nullptr, sem);

    if (r->getLeftBound() == nullptr || r->getRightBound() == nullptr)
        return nullptr;

    hif::semantics::updateDeclarations(r, sem);
    if (simplify)
        r = hif::manipulation::simplify(r, sem, opts);

    ConstValue *cv1 = dynamic_cast<ConstValue *>(r->getLeftBound());
    ConstValue *cv2 = dynamic_cast<ConstValue *>(r->getRightBound());

    // Optimization:
    if (cv1 != nullptr && cv2 != nullptr) {
        IntValue *ivo1 = dynamic_cast<IntValue *>(cv1);
        IntValue *ivo2 = dynamic_cast<IntValue *>(cv2);

        if (ivo1 == nullptr) {
            Int it;
            it.setSigned(true);
            it.setSpan(new Range(63, 0));
            it.setConstexpr(true);

            ivo1 = dynamic_cast<IntValue *>(hif::manipulation::transformConstant(cv1, &it, sem));
        }

        if (ivo2 == nullptr) {
            Int it;
            it.setSigned(true);
            it.setSpan(new Range(63, 0));
            it.setConstexpr(true);

            ivo2 = dynamic_cast<IntValue *>(hif::manipulation::transformConstant(cv2, &it, sem));
        }

        messageAssert(ivo1 != nullptr, "Cannot convert const value to int", cv1, sem);
        messageAssert(ivo2 != nullptr, "Cannot convert const value to int", cv2, sem);

        std::int64_t res =
            (r->getDirection() == dir_upto) ? ivo2->getValue() - ivo1->getValue() : ivo1->getValue() - ivo2->getValue();

        if (ivo1 != cv1)
            delete ivo1;
        if (ivo2 != cv2)
            delete ivo2;

        if (res < 0)
            res = 0;
        else
            ++res;

        IntValue *ret = new IntValue(res);
        ret->setType(sem->getTypeForConstant(ret));
        return ret;
    }

    Value *lBound = hif::copy(r->getLeftBound());
    Value *rBound = hif::copy(r->getRightBound());
    if (cv1 != nullptr) {
        ConstValue *iLBound = static_cast<ConstValue *>(lBound);
        iLBound->setType(sem->getTypeForConstant(iLBound));
    }

    if (cv2 != nullptr) {
        ConstValue *iRBound = static_cast<ConstValue *>(rBound);
        iRBound->setType(sem->getTypeForConstant(iRBound));
    }

    IntValue *rightAddendum = new IntValue(1);
    rightAddendum->setType(sem->getTypeForConstant(rightAddendum));

    Expression *rangeSpan = nullptr;
    if (r->getDirection() == dir_upto) {
        // Generate Expression (SUP-INF)
        Value *leftAddendum;
        Expression *eLeftAddendum = new Expression();
        eLeftAddendum->setOperator(op_minus);
        eLeftAddendum->setValue1(rBound);
        eLeftAddendum->setValue2(lBound);
        leftAddendum = eLeftAddendum;

        // Generate Expression (SUP-INF+1)
        rangeSpan = new Expression();
        rangeSpan->setOperator(op_plus);
        rangeSpan->setValue1(leftAddendum);
        rangeSpan->setValue2(rightAddendum);
    } else if (r->getDirection() == dir_downto) {
        // Generate Expression (SUP-INF)
        Value *leftAddendum;
        Expression *eLeftAddendum = new Expression();
        eLeftAddendum->setOperator(op_minus);
        eLeftAddendum->setValue1(lBound);
        eLeftAddendum->setValue2(rBound);
        leftAddendum = eLeftAddendum;

        // Generate Expression (SUP-INF+1)
        rangeSpan = new Expression();
        rangeSpan->setOperator(op_plus);
        rangeSpan->setValue1(leftAddendum);
        rangeSpan->setValue2(rightAddendum);
    } else {
        delete lBound;
        delete rBound;
        delete rightAddendum;
    }

    Value *ret = hif::manipulation::simplify(rangeSpan, sem, opts);
    messageDebugAssert(ret != nullptr, "Cannot simplify rangeSpan", rangeSpan, sem);
    return ret;
}

Value *typeGetTotalSpanSize(Type *t, ILanguageSemantics *sem)
{
    if (dynamic_cast<Array *>(t) == nullptr) {
        return hif::semantics::typeGetSpanSize(t, sem);
    }

    Array *arr      = static_cast<Array *>(t);
    Value *typeSize = hif::semantics::typeGetTotalSpanSize(arr->getType(), sem);
    if (typeSize == nullptr)
        return nullptr;
    Value *spanSize = hif::semantics::typeGetSpanSize(arr, sem);

    Expression *totalSpan = new Expression();
    totalSpan->setOperator(op_mult);
    totalSpan->setValue1(spanSize);
    totalSpan->setValue2(typeSize);

    Value *ret = hif::manipulation::simplify(totalSpan, sem);
    messageDebugAssert(ret != nullptr, "Cannot simplify totalSpan", totalSpan, sem);
    return ret;
}

} // namespace semantics
} // namespace hif
