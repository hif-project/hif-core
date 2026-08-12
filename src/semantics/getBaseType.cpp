/// @file getBaseType.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/getBaseType.hpp"

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

Type *getBaseType(Type *type, bool consider_opacity, ILanguageSemantics *sem, bool compositeRecurse)
{
    Type *ret = nullptr;

    if (dynamic_cast<TypeReference *>(type) != nullptr) {
        TypeReference *tRef                  = static_cast<TypeReference *>(type);
        TypeReference::DeclarationType *decl = hif::semantics::getDeclaration(tRef, sem);
        messageAssert(decl != nullptr, "Not found declaration of TypeRef", tRef, sem);

        if (dynamic_cast<TypeTP *>(decl) != nullptr) {
            return tRef;
        }

        TypeDef *td = dynamic_cast<TypeDef *>(decl);
        // sanity check.
        messageAssert(td != nullptr, "Unexpected case", decl, sem);

        if (consider_opacity && td->isOpaque())
            return tRef;

        TypeDef *tdinst = dynamic_cast<TypeDef *>(hif::manipulation::instantiate(tRef, sem));
        messageAssert(tdinst != nullptr, "Cannot instantiate type ref", tRef, sem);

        // recursive call on typedef type!
        ret = getBaseType(tdinst->getType(), consider_opacity, sem, compositeRecurse);
    } else if (dynamic_cast<CompositeType *>(type) != nullptr) {
        if (!compositeRecurse)
            return type;

        CompositeType *ct = static_cast<CompositeType *>(type);
        if (ct->getBaseType(consider_opacity) != nullptr)
            return ct->getBaseType(consider_opacity);

        CompositeType *ctn = hif::copy(ct);
        Type *baseSubType  = getBaseType(ct->getType(), consider_opacity, sem, compositeRecurse);
        delete ctn->setType(hif::copy(baseSubType));
        ct->setBaseType(ctn, consider_opacity);
        ret = ctn;
    } else if (dynamic_cast<Record *>(type) != nullptr) {
        if (!compositeRecurse)
            return type;

        Record *rec = static_cast<Record *>(type);
        if (rec->getBaseType(consider_opacity) != nullptr)
            return rec->getBaseType(consider_opacity);

        Record *recn          = hif::copy(rec);
        bool canReplace = (rec->getParent() != nullptr);
        if (canReplace)
            rec->replace(recn);
        for (BList<Field>::iterator i = recn->fields.begin(); i != recn->fields.end(); ++i) {
            Field *f          = *i;
            Type *baseSubType = getBaseType(f->getType(), consider_opacity, sem, compositeRecurse);
            delete f->setType(hif::copy(baseSubType));
        }

        if (canReplace)
            recn->replace(rec);
        rec->setBaseType(recn, consider_opacity);
        ret = recn;
    } else {
        ret = type;
    }

    return ret;
}

Type *getBaseType(
    TypedObject *v,
    bool considerOpacity,
    ILanguageSemantics *refSem,
    bool compositeRecurse,
    bool error)
{
    if (v == nullptr)
        return nullptr;
    Type *t = hif::semantics::getSemanticType(v, refSem, error);
    return hif::semantics::getBaseType(t, considerOpacity, refSem, compositeRecurse);
}

} // namespace semantics
} // namespace hif
