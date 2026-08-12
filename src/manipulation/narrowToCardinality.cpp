/// @file narrowToCardinality.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/narrowToCardinality.hpp"

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

} // namespace

Value *
narrowToCardinality(Value *v, unsigned int c, hif::semantics::ILanguageSemantics *sem, bool considerOnlyBits)
{
    Value *current = v;
    for (;;) // while(true)
    {
        Type *t = hif::semantics::getSemanticType(current, sem);
        messageAssert(t != nullptr, "Cannot type current value", current, sem);

        unsigned int vCard = hif::typeGetCardinality(t, sem, considerOnlyBits);

        if (dynamic_cast<Member *>(current) != nullptr) {
            if (vCard == c)
                break;
            current = static_cast<Member *>(current)->getPrefix();
        } else if (dynamic_cast<Slice *>(current) != nullptr) {
            Slice *s                     = static_cast<Slice *>(current);
            std::uint64_t sliceSize = hif::semantics::spanGetBitwidth(s->getSpan(), sem);
            if (sliceSize == 1ull) // acts as Member
            {
                // typeGetCardinality does not distinguish between N-span Slice and 1-span Slice
                if (vCard - 1 == c)
                    break;
            }
            current = s->getPrefix();
        } else {
            if (vCard == c)
                break;
            // No more objects can be skipped
            return nullptr;
        }
    }

    return current;
}

} // namespace manipulation
} // namespace hif
