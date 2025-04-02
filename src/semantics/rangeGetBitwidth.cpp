/// @file rangeGetBitwidth.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/rangeGetBitwidth.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

std::int64_t intvalsize(std::int64_t x)
{
    std::int64_t r;

    if (x < 0LL)
        x = ~x;
    if (x == 0LL)
        return 1LL;
    for (r = 0LL; x != 0LL; ++r, x >>= 1) {
    };
    return r;
}

} // namespace

unsigned int rangeGetBitwidth(Range *r, ILanguageSemantics *sem)
{
    if (r == nullptr)
        return 0;
    hif::manipulation::simplify(r, sem);

    if ((r->getDirection() != dir_upto) && (r->getDirection() != dir_downto))
        return 0;

    ConstValue *cv1 = dynamic_cast<ConstValue *>(r->getLeftBound());
    ConstValue *cv2 = dynamic_cast<ConstValue *>(r->getRightBound());
    if (cv1 == nullptr || cv2 == nullptr)
        return 0;

    Type *t1 = hif::semantics::getSemanticType(cv1, sem);
    Type *t2 = hif::semantics::getSemanticType(cv2, sem);
    messageAssert(t1 != nullptr, "Cannot type left bound", cv1, sem);
    messageAssert(t2 != nullptr, "Cannot type right bound", cv2, sem);

    Int it1;
    it1.setSigned(typeIsSigned(t1, sem));
    it1.setSpan(hif::copy(typeGetSpan(t1, sem)));
    it1.setConstexpr(true);

    Int it2;
    it2.setSigned(typeIsSigned(t2, sem));
    it2.setSpan(hif::copy(typeGetSpan(t2, sem)));
    it2.setConstexpr(true);

    IntValue *ivo1 = dynamic_cast<IntValue *>(hif::manipulation::transformConstant(cv1, &it1, sem));
    IntValue *ivo2 = dynamic_cast<IntValue *>(hif::manipulation::transformConstant(cv2, &it2, sem));
    messageAssert(ivo1 != nullptr && ivo2 != nullptr, "Unexpected case", cv1, sem);

    std::int64_t maxsize, lbound, rbound;

    lbound = ivo1->getValue();
    rbound = ivo2->getValue();

    maxsize = std::max(intvalsize(lbound), intvalsize(rbound));

    if (lbound < 0 || rbound < 0)
        maxsize++;

    delete ivo1;
    delete ivo2;
    return static_cast<unsigned>(maxsize);
}

} // namespace semantics
} // namespace hif
