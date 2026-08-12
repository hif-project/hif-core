/// @file getInstantiatedType.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/resetDeclarations.hpp"

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
Type *getInstantiatedType(Type *t, ILanguageSemantics *ref_sem)
{
    if (dynamic_cast<ReferencedType *>(t) == nullptr) {
        return hif::copy(t);
    }

    Type *ret           = nullptr;
    TypeReference *tRef = dynamic_cast<TypeReference *>(t);
    ViewReference *vRef = dynamic_cast<ViewReference *>(t);

    if (tRef != nullptr) {
        TypeReference::DeclarationType *decl = hif::manipulation::instantiate(tRef, ref_sem);
        TypeDef *tDef                        = dynamic_cast<TypeDef *>(decl);
        TypeTP *tTP                          = dynamic_cast<TypeTP *>(decl);
        if (tDef != nullptr) {
            ret = getInstantiatedType(tDef->getType(), ref_sem);
        } else if (tTP != nullptr) {
            // TODO study it
            messageError("Unhandled case", decl, ref_sem);
        } else {
            messageError("Unexpected case", decl, ref_sem);
        }
    } else if (vRef != nullptr) {
        // TODO study it
        messageError("Unhandled case", vRef, ref_sem);
        //      TypeReference::DeclarationType* decl = hif::manipulation::instantiate( vRef, ref_sem, instOpt );
    } else {
        messageError("Unexpected case", t, ref_sem);
    }

    return ret;
}
} // namespace semantics
} // namespace hif
