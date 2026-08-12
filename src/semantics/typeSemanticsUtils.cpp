/// @file typeSemanticsUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/typeSemanticUtils.hpp"

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

bool _checkInheritance(View *toCheckDecl, View *parentVrDecl, hif::semantics::ILanguageSemantics *sem)
{
    if (toCheckDecl == parentVrDecl)
        return true;

    for (BList<ViewReference>::iterator i = toCheckDecl->inheritances.begin(); i != toCheckDecl->inheritances.end();
         ++i) {
        hif::ViewReference::DeclarationType *parentClassDecl = hif::semantics::getDeclaration(*i, sem);
        if (parentClassDecl == nullptr) {
            // error?
            continue;
        }

        if (_checkInheritance(parentClassDecl, parentVrDecl, sem))
            return true;
    }

    return false;
}

} // namespace

bool isSignedType(Type *type, ILanguageSemantics *refLang)
{
    Type *t = getBaseType(type, false, refLang);

    if (t && dynamic_cast<Signed *>(t))
        return true;

    return false;
}
bool isUnsignedType(Type *type, ILanguageSemantics *refLang)
{
    Type *t = getBaseType(type, false, refLang);

    if (t && dynamic_cast<Unsigned *>(t))
        return true;

    return false;
}
bool isVectorType(Type *type, ILanguageSemantics *refLang)
{
    Type *t = getBaseType(type, false, refLang);

    if (isLogicVectorType(t, refLang))
        return true;
    else if (isBitVectorType(t, refLang))
        return true;
    else if (isUnsignedType(t, refLang))
        return true;
    else if (isSignedType(t, refLang))
        return true;

    return false;
}
bool isLogicBitType(Type *type, ILanguageSemantics *refLang)
{
    Type *t = getBaseType(type, false, refLang);

    if (dynamic_cast<Bit *>(t) != nullptr) {
        Bit *b = static_cast<Bit *>(t);

        if (b->isLogic())
            return true;
    }

    return false;
}
bool isLogicVectorType(Type *type, ILanguageSemantics *refLang)
{
    Type *t = getBaseType(type, false, refLang);

    if (dynamic_cast<Bitvector *>(t) != nullptr) {
        Bitvector *a = static_cast<Bitvector *>(t);
        return a->isLogic();
    }

    return false;
}
bool isSubType(Type *t1, Type *t2, ILanguageSemantics *refLang, bool compareSpan, bool considerOpacity)
{
    if (t1 == nullptr && t2 == nullptr)
        return true;
    if (t1 == nullptr || t2 == nullptr)
        return false;

    Type *base1 = hif::semantics::getBaseType(t1, considerOpacity, refLang);
    Type *base2 = hif::semantics::getBaseType(t2, considerOpacity, refLang);
    if (base1 == nullptr || base2 == nullptr)
        return false;

    hif::EqualsOptions optType;
    optType.checkOnlyTypes = true;
    if (!hif::equals(base1, base2, optType))
        return false;

    ViewReference *vr1            = dynamic_cast<ViewReference *>(base1);
    CompositeType *ct1            = dynamic_cast<CompositeType *>(base1);
    hif::features::ITypeSpan *ts1 = dynamic_cast<hif::features::ITypeSpan *>(base1);

    if (vr1 != nullptr) {
        ViewReference *vr2 = static_cast<ViewReference *>(base2);

        hif::ViewReference::DeclarationType *toCheckDecl = hif::semantics::getDeclaration(vr1, refLang);
        messageAssert(toCheckDecl != nullptr, "Cannot find declaration (1)", vr1, refLang);

        hif::ViewReference::DeclarationType *parentVrDecl = hif::semantics::getDeclaration(vr2, refLang);
        messageAssert(parentVrDecl != nullptr, "Cannot find declaration (2)", vr2, refLang);

        return _checkInheritance(toCheckDecl, parentVrDecl, refLang);
    }

    if (ct1 != nullptr) {
        CompositeType *ct2 = static_cast<CompositeType *>(base2);
        if (!isSubType(ct1->getType(), ct2->getType(), refLang, compareSpan, considerOpacity))
            return false;
    }

    hif::EqualsOptions optSpan;
    if (ts1 != nullptr && !compareSpan) {
        optSpan.checkSpans = false;
    }

    return hif::equals(base1, base2, optSpan);
}
bool isBitType(Type *type, ILanguageSemantics *refLang)
{
    Type *t = getBaseType(type, false, refLang);

    if (dynamic_cast<Bit *>(t) != nullptr)
        return true;

    return false;
}
bool isBitVectorType(Type *type, ILanguageSemantics *refLang)
{
    Type *t = getBaseType(type, false, refLang);

    if (dynamic_cast<Bitvector *>(t) != nullptr)
        return true;

    return false;
}
bool isSingleBitType(Type *type, ILanguageSemantics *sem)
{
    Type *t = getBaseType(type, false, sem);

    // TODO maybe valid also for logic bits...
    if (dynamic_cast<Bit *>(t) != nullptr && !static_cast<Bit *>(t)->isLogic())
        return true;
    if (dynamic_cast<Bool *>(t) != nullptr)
        return true;

    return false;
}
bool isSemanticsType(const Type *t)
{
    if (t == nullptr)
        return false;
    if (t->getParent() == nullptr)
        return false;
    auto to = dynamic_cast<TypedObject *>(t->getParent());
    if (to == nullptr)
        return false;
    return (to->getSemanticType() == t);
}

} // namespace semantics
} // namespace hif
