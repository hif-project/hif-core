/// @file getVectorElementType.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/getVectorElementType.hpp"

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

Type *getVectorElementType(Type *t, ILanguageSemantics *refLang)
{
    if (dynamic_cast<Array *>(t) != nullptr) {
        Array *o = static_cast<Array *>(t);
        return hif::copy(o->getType());
    } else if (dynamic_cast<Signed *>(t) != nullptr) {
        Signed *o = static_cast<Signed *>(t);
        Bit *b    = new Bit();
        b->setLogic(true);
        b->setConstexpr(o->isConstexpr());
        b->setResolved(true);
        return b;
    } else if (dynamic_cast<Unsigned *>(t) != nullptr) {
        Unsigned *o = static_cast<Unsigned *>(t);
        Bit *b      = new Bit();
        b->setLogic(true);
        b->setConstexpr(o->isConstexpr());
        b->setResolved(true);
        return b;
    } else if (dynamic_cast<Bitvector *>(t) != nullptr) {
        Bitvector *o = static_cast<Bitvector *>(t);
        Bit *b       = new Bit();
        b->setLogic(o->isLogic());
        b->setConstexpr(o->isConstexpr());
        b->setResolved(o->isResolved());
        return b;
    } else if (dynamic_cast<TypeReference *>(t) != nullptr) {
        TypeReference *o = static_cast<TypeReference *>(t);
        Type *tb         = hif::semantics::getBaseType(o, false, refLang, false);
        return getVectorElementType(tb, refLang);
    } else if (dynamic_cast<String *>(t) != nullptr) {
        String *o = static_cast<String *>(t);
        Char *b   = new Char();
        b->setConstexpr(o->isConstexpr());
        return b;
    } else {
        return nullptr;
    }
}

} // namespace semantics
} // namespace hif
