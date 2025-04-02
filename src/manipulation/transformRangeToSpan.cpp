/// @file transformRangeToSpan.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>
#include <cstdlib>
#include <iostream>

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/manipulation/transformSpanToRange.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

//int MAX_INT_VAL = 2147483647;

/// this function is used to avoid the approximations due to floating point
/// calculation of c99 log2 (double)
std::uint64_t log2_int(std::uint64_t x)
{
    std::uint64_t ans = 0;
    while (x >>= 1)
        ++ans;
    return ans;
}

/// Returns the number of bits needed to represent the given number minus
/// one. This number can be used as is in the Range that represents
/// the span.
std::int64_t get_num_bits(std::int64_t n, bool is_signed)
{
    if (n < 0)
        n = -n;
    return static_cast<std::int64_t>(log2_int(static_cast<std::uint64_t>(n)) + is_signed);
}
/// Returns the value object representing the number of bits needed to
/// represent the given range. That is log(n, 2) + is_signed
Value *get_num_bits(Value *n, bool is_signed, hif::semantics::ILanguageSemantics *refLang)
{
    hif::HifFactory factory;
    factory.setSemantics(refLang);
    Value *result = factory.expression(hif::copy(n), op_log, factory.intval(2));

    if (is_signed)
        return factory.expression(result, op_plus, factory.intval(1));
    else
        return result;
}

} // namespace

Range *transformRangeToSpan(Range *range, hif::semantics::ILanguageSemantics *refLang, bool isSigned)
{
    messageDebugAssert(range != nullptr, "Unexpected nullptr range", nullptr, refLang);
    if (range->getDirection() != dir_upto && range->getDirection() != dir_downto) {
        return nullptr;
    }

    // get a copy trying to simplify the range bounds (this is not
    // performed on the original range because I don't want to change the
    // original range)
    Range *r = hif::copy(range);
    hif::manipulation::simplify(r, refLang);

    hif::HifFactory factory;
    factory.setSemantics(refLang);
    Value *result_bound   = nullptr;
    Value *nr             = nullptr;
    Value *nl             = nullptr;
    bool nr_is_expression = false;
    bool nl_is_expression = false;

    // compute the bits needed for the rbound
    if (dynamic_cast<IntValue *>(r->getRightBound())) {
        IntValue *rval = static_cast<IntValue *>(r->getRightBound());
        nr             = factory.intval(get_num_bits(rval->getValue(), isSigned));
    } else {
        nr               = get_num_bits(r->getRightBound(), isSigned, refLang);
        nr_is_expression = true;
    }

    // compute the bits needed for the lbound
    if (dynamic_cast<IntValue *>(r->getLeftBound())) {
        IntValue *lval = static_cast<IntValue *>(r->getLeftBound());
        nl             = factory.intval(get_num_bits(lval->getValue(), isSigned));
    } else {
        nl               = get_num_bits(r->getLeftBound(), isSigned, refLang);
        nl_is_expression = true;
    }

    // build the resulting span
    if (!nl_is_expression && !nr_is_expression) {
        // return the max of the two computed values
        Value *max_val;
        Value *min_val;
        if (static_cast<IntValue *>(nl)->getValue() > static_cast<IntValue *>(nr)->getValue()) {
            max_val = nl;
            min_val = nr;
        } else {
            max_val = nr;
            min_val = nl;
        }
        result_bound = max_val;
        delete min_val;
    } else {
        // there is an expression, so build a value like this:
        // nr > nl ? nr : nl
        // optimization: if there is a 0, the other expression is the max
        // (transform [0 > nr ? 0 : nr] in [nr])
        if (!nr_is_expression && static_cast<IntValue *>(nr)->getValue() < 2) {
            result_bound = nl;
        } else if (!nl_is_expression && static_cast<IntValue *>(nl)->getValue() < 2) {
            result_bound = nr;
        } else {
            When *cond       = new When();
            WhenAlt *alt     = new WhenAlt();
            Expression *expr = factory.expression(nr, op_gt, nl);
            alt->setCondition(expr);
            alt->setValue(hif::copy(nr));
            cond->setDefault(hif::copy(nl));
            cond->alts.push_back(alt);
            // so the resulting range is [cond downto 0]
            result_bound = cond;
        }
    }

    Range *result = new Range();
    result->setLeftBound(result_bound);
    result->setRightBound(factory.intval(0));
    result->setDirection(dir_downto);
    delete r;

    result = hif::manipulation::simplify(result, refLang);

    return result;
}

} // namespace manipulation
} // namespace hif
