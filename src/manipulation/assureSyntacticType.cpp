/// @file assureSyntacticType.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/assureSyntacticType.hpp"

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

bool needSyntacticType(Value *v)
{
    if (v->getParent() == nullptr)
        return true;

    Range *parentRange = dynamic_cast<Range *>(v->getParent());
    if (parentRange != nullptr)
        return false;

    AggregateAlt *parentAggrAlt = dynamic_cast<AggregateAlt *>(v->getParent());
    if (parentAggrAlt != nullptr && parentAggrAlt->indices.contains(v))
        return false;

    Member *parentMember = dynamic_cast<Member *>(v->getParent());
    if (parentMember != nullptr && parentMember->getIndex() == v)
        return false;

    return true;
}
Value *assureSyntacticType(Value *v, hif::semantics::ILanguageSemantics *sem, Type *suggestedType)
{
    ConstValue *cv = dynamic_cast<ConstValue *>(v);
    if (cv == nullptr)
        return v;

    if (!needSyntacticType(cv)) {
        if (cv->getType() != nullptr) {
            delete cv->setType(nullptr);
        }
        return cv;
    }

    if (cv->getType() != nullptr)
        return cv;

    if (suggestedType != nullptr) {
        Type *suggested = hif::copy(suggestedType);
        hif::typeSetConstexpr(suggested, true);
        cv->setType(suggested);
    } else {
        cv->setType(sem->getTypeForConstant(cv));
    }

    return cv;
}

} // namespace manipulation
} // namespace hif
