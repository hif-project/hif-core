/// @file spanGetBitwidth.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/spanGetBitwidth.hpp"

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

std::uint64_t
spanGetBitwidth(Range *r, ILanguageSemantics *sem, const bool simplify, const manipulation::SimplifyOptions &opts)
{
    if (r == nullptr)
        return 0;
    Value *res = spanGetSize(r, sem, simplify, opts);
    if (res == nullptr)
        return 0;
    ConstValue *cvRes = dynamic_cast<ConstValue *>(res);
    if (cvRes == nullptr) {
        delete res;
        return 0;
    }
    IntValue *ivoRes = dynamic_cast<IntValue *>(cvRes);
    if (ivoRes != nullptr) {
        std::int64_t resInt = ivoRes->getValue();
        if (resInt < 0)
            resInt = 0;
        delete ivoRes;
        return static_cast<std::uint64_t>(resInt);
    }

    Int it1;
    it1.setSigned(true);
    it1.setSpan(new Range(63, 0));
    it1.setConstexpr(true);

    ivoRes = dynamic_cast<IntValue *>(hif::manipulation::transformConstant(cvRes, &it1, sem));
    if (ivoRes != nullptr) {
        std::int64_t resInt = ivoRes->getValue();
        if (resInt < 0)
            resInt = 0;
        delete ivoRes;
        return static_cast<std::uint64_t>(resInt);
    }

    delete cvRes;
    return 0;
}

std::uint64_t typeGetSpanBitwidth(
    Type *type,
    ILanguageSemantics *sem,
    const bool simplify,
    const hif::manipulation::SimplifyOptions &opts)
{
    Range *span = hif::typeGetSpan(type, sem);
    return spanGetBitwidth(span, sem, simplify, opts);
}

} // namespace semantics
} // namespace hif
