/// @file canRemoveInternalCast.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/canRemoveInternalCast.hpp"
#include "hif/classes/Type.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/semantics/semantics.hpp"
#include "hif/semantics/standardization.hpp"

namespace hif
{
namespace semantics
{

bool canRemoveInternalCast(
    Type *t1,
    Type *t2,
    Type *t3,
    ILanguageSemantics *sem,
    Object *o,
    bool conservativeBehavior)
{
    // First, ask semantics if a direct cast is allowed
    if (!sem->isCastAllowed(t1, t3))
        return false;
    // Ref design vhdl/openCores/uart
    if (dynamic_cast<Assign *>(o) != nullptr && sem->getExprType(t1, t3, hif::op_assign, o).returnedType == nullptr)
        return false;

    hif::semantics::precision_type_t t1_t2, t2_t3;
    t1_t2 = hif::semantics::comparePrecision(t1, t2, sem);
    t2_t3 = hif::semantics::comparePrecision(t2, t3, sem);

    // remove internal cast only if ! (t1 > t2) or ! (t2 < t3) and t1,
    // t2, t3 comparable
    if (t1_t2 == hif::semantics::UNCOMPARABLE_PRECISION && t2_t3 == hif::semantics::UNCOMPARABLE_PRECISION) {
        // uncomparable, cannot fix.
        return false;
    }

    /* How to consider T1, T2, T3 wrt to span (and sign):
         * Tx < Ty means that we are losing precision (Tx span is less than Ty's)
         * Tx > Ty means that we are gaining precision (Tx span is greater than Ty's)
         * Tx = Ty means same precision
         *
         * T1 < T2 < T3 : always trunk, remove
         * T1 < T2 > T3 : always remove, if T1 <= T3 remove, no extension
         *                else, extending according to T3 and change sign to T1
         * T1 > T2 < T3 : keep
         * T1 > T2 > T3 : conditional remove
         * T1 < T2 = T3 : always trunk, remove
         * T1 = T2 < T3 : always trunk, remove
         * T1 > T2 = T3 : keep, check sign to remove
         * T1 = T2 > T3 : extension depends on T3: remove if sign(t2)=sign(t3)
         * T1 = T2 ? T3 : extension depends on T3: remove if sign(t2)=sign(t3)
         * T1 = T2 = T3 : always remove
        */
    //bool sign1 = hif::typeIsSigned( t1, _sem ); // unused
    bool sign2 = hif::typeIsSigned(t2, sem);
    bool sign3 = hif::typeIsSigned(t3, sem);

    if (t1_t2 == hif::semantics::GREATER_PRECISION && t2_t3 == hif::semantics::LESS_PRECISION) {
        // T1 > T2 < T3 : keep
        return false;
    } else if (t1_t2 == hif::semantics::GREATER_PRECISION && t2_t3 == hif::semantics::GREATER_PRECISION) {
        // T1 > T2 > T3 : conditional remove
        // if sign(T1)=sign(T2) only extension, remove
        // if sign(T3)=unsigned extending always with 0, remove

        if (!(sign2 == sign3 || !sign3))
            return false;
    } else if (t1_t2 == hif::semantics::GREATER_PRECISION && t2_t3 == hif::semantics::EQUAL_PRECISION) {
        // T1 > T2 = T3 : remove if sign(t2)=sign(t3)
        if (sign2 != sign3)
            return false;
    } else if (t1_t2 == hif::semantics::UNCOMPARABLE_PRECISION && t2_t3 == hif::semantics::EQUAL_PRECISION) {
        // T1 ? T2 = T3 : remove if sign(t2)=sign(t3)
        if (sign2 != sign3)
            return false;

        return true;
    } else if (t1_t2 == hif::semantics::EQUAL_PRECISION) {
        // T1 = T2 < T3 : always trunk, remove
        // T1 = T2 = T3 : always remove
        // T1 = T2 > T3 : extension depends on T3 --> remove if sign(t2)=sign(t3)
        // T1 = T2 ? T3 : remove if sign(t2)=sign(t3)
        if (t2_t3 == hif::semantics::GREATER_PRECISION || t2_t3 == hif::semantics::UNCOMPARABLE_PRECISION) {
            if (conservativeBehavior)
                return false;
            return sign2 == sign3;
        }

        return true;
    } else if (t1_t2 == hif::semantics::UNCOMPARABLE_PRECISION || t2_t3 == hif::semantics::UNCOMPARABLE_PRECISION) {
        // uncomparable, cannot fix.
        return false;
    }

    return true;
}

} // namespace semantics
} // namespace hif
