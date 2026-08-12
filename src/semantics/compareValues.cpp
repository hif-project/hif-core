/// @file compareValues.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/compareValues.hpp"

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

CompareResult compareValues(Value *v1, Value *v2, hif::semantics::ILanguageSemantics *refSem, bool simplify)
{
    hif::manipulation::SimplifyOptions opt;
    opt.replace_result = false;
    if (simplify) {
        opt.simplify_template_parameters = true;
        opt.simplify_constants           = true;
        v1                               = hif::manipulation::simplify(v1, refSem, opt);
        v2                               = hif::manipulation::simplify(v2, refSem, opt);
    } else {
        opt.simplify_template_parameters = false;
        opt.simplify_constants           = false;
        v1                               = hif::manipulation::simplify(v1, refSem, opt);
        v2                               = hif::manipulation::simplify(v2, refSem, opt);
    }

    if (hif::equals(v1, v2)) {
        delete v1;
        delete v2;
        return CompareResult::EQUAL;
    }

    IntValue *ivo1 = dynamic_cast<IntValue *>(v1);
    IntValue *ivo2 = dynamic_cast<IntValue *>(v2);

    RealValue *rvo1 = dynamic_cast<RealValue *>(v1);
    RealValue *rvo2 = dynamic_cast<RealValue *>(v2);

    if (ivo1 == nullptr && rvo1 == nullptr) {
        delete v1;
        delete v2;
        return CompareResult::UNKNOWN;
    }
    if (ivo2 == nullptr && rvo2 == nullptr) {
        delete v1;
        delete v2;
        return CompareResult::UNKNOWN;
    }

    long double l1 = 0, l2 = 0;

    if (ivo1 != nullptr)
        l1 = static_cast<long double>(ivo1->getValue());
    else
        l1 = static_cast<long double>(rvo1->getValue());

    if (ivo2 != nullptr)
        l2 = static_cast<long double>(ivo2->getValue());
    else
        l2 = static_cast<long double>(rvo2->getValue());

    if (l1 < l2) {
        delete v1;
        delete v2;
        return CompareResult::LESS;
    } else if (l1 > l2) {
        delete v1;
        delete v2;
        return CompareResult::GREATER;
    }

    delete v1;
    delete v2;
    return CompareResult::EQUAL;
}

} // namespace semantics
} // namespace hif
