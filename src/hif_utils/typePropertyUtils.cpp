/// @file typePropertyUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/typePropertyUtils.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

bool typeIsConstexpr(Type *t, hif::semantics::ILanguageSemantics *refLang)
{
    if (t == nullptr)
        return false;

    if (dynamic_cast<CompositeType *>(t) != nullptr) {
        CompositeType *c = static_cast<CompositeType *>(t);
        return typeIsConstexpr(c->getType(), refLang);
    } else if (dynamic_cast<ScopedType *>(t) != nullptr) {
        ScopedType *c = static_cast<ScopedType *>(t);
        return c->isConstexpr();
    } else if (dynamic_cast<SimpleType *>(t) != nullptr) {
        SimpleType *c = static_cast<SimpleType *>(t);
        return c->isConstexpr();
    } else if (dynamic_cast<TypeReference *>(t) != nullptr) {
        Type *base = hif::semantics::getBaseType(t, false, refLang);
        messageAssert(
            dynamic_cast<TypeReference *>(base) == nullptr, "Invalid call to typeIsConstexpr: template type parameter",
            t, refLang);
        return typeIsConstexpr(base, refLang);
    }

    return false;
}

bool typeIsSigned(Type *to, hif::semantics::ILanguageSemantics *refLang, const bool skip)
{
    if (to == nullptr)
        return false;

    if (dynamic_cast<Array *>(to) != nullptr) {
        return (static_cast<Array *>(to))->isSigned();
    } else if (dynamic_cast<Bitvector *>(to) != nullptr) {
        return (static_cast<Bitvector *>(to))->isSigned();
    } else if (dynamic_cast<Int *>(to) != nullptr) {
        return (static_cast<Int *>(to))->isSigned();
    } else if (dynamic_cast<Pointer *>(to) != nullptr) {
        return typeIsSigned(static_cast<Pointer *>(to)->getType(), refLang);
    } else if (dynamic_cast<Reference *>(to) != nullptr) {
        return typeIsSigned(static_cast<Reference *>(to)->getType(), refLang);
    } else if (dynamic_cast<Char *>(to) != nullptr) {
        return true;
    } else if (dynamic_cast<Enum *>(to) != nullptr) {
        return true;
    } else if (dynamic_cast<Real *>(to) != nullptr) {
        return true;
    } else if (dynamic_cast<Signed *>(to) != nullptr) {
        return true;
    } else if (dynamic_cast<Bit *>(to) != nullptr) {
        return false; // ??
    } else if (dynamic_cast<TypeReference *>(to) != nullptr) {
        if (skip)
            return false; // ??

        Type *base = hif::semantics::getBaseType(to, false, refLang);
        messageAssert(
            dynamic_cast<TypeReference *>(base) == nullptr, "Invalid call to typeIsSigned: template type parameter", to,
            refLang);
        return typeIsSigned(base, refLang);
    }

    return false;
}
bool typeIsLogic(Type *to, hif::semantics::ILanguageSemantics *refLang)
{
    if (to == nullptr)
        return false;

    if (dynamic_cast<Array *>(to) != nullptr) {
        return typeIsLogic(static_cast<Array *>(to)->getType(), refLang);
    } else if (dynamic_cast<Bit *>(to) != nullptr) {
        return (static_cast<Bit *>(to)->isLogic());
    } else if (dynamic_cast<Bitvector *>(to) != nullptr) {
        return (static_cast<Bitvector *>(to)->isLogic());
    } else if (dynamic_cast<Pointer *>(to) != nullptr) {
        return typeIsLogic(static_cast<Pointer *>(to)->getType(), refLang);
    } else if (dynamic_cast<Reference *>(to) != nullptr) {
        return typeIsLogic(static_cast<Reference *>(to)->getType(), refLang);
    } else if (dynamic_cast<Signed *>(to) != nullptr) {
        return true;
    } else if (dynamic_cast<Unsigned *>(to) != nullptr) {
        return true;
    } else if (dynamic_cast<TypeReference *>(to) != nullptr) {
        Type *base = hif::semantics::getBaseType(to, false, refLang);
        messageAssert(
            dynamic_cast<TypeReference *>(base) == nullptr, "Invalid call to typeIsLogic: template type parameter", to,
            refLang);
        return typeIsLogic(base, refLang);
    }

    return false;
}
bool typeIsResolved(Type *to, hif::semantics::ILanguageSemantics *refLang)
{
    if (to == nullptr)
        return false;

    if (dynamic_cast<Array *>(to) != nullptr) {
        return typeIsResolved(static_cast<Array *>(to)->getType(), refLang);
    } else if (dynamic_cast<Bit *>(to) != nullptr) {
        return (static_cast<Bit *>(to)->isResolved());
    } else if (dynamic_cast<Bitvector *>(to) != nullptr) {
        return (static_cast<Bitvector *>(to)->isResolved());
    } else if (dynamic_cast<Pointer *>(to) != nullptr) {
        return typeIsResolved(static_cast<Pointer *>(to)->getType(), refLang);
    } else if (dynamic_cast<Reference *>(to) != nullptr) {
        return typeIsResolved(static_cast<Reference *>(to)->getType(), refLang);
    } else if (dynamic_cast<Signed *>(to) != nullptr) {
        return true;
    } else if (dynamic_cast<Unsigned *>(to) != nullptr) {
        return true;
    } else if (dynamic_cast<TypeReference *>(to) != nullptr) {
        Type *base = hif::semantics::getBaseType(to, false, refLang);
        messageAssert(
            dynamic_cast<TypeReference *>(base) == nullptr, "Invalid call to typeIsResolved: template type parameter",
            to, refLang);
        return typeIsResolved(base, refLang);
    }

    return false;
}

Range *typeGetSpan(Type *to, hif::semantics::ILanguageSemantics *sem, const bool manageStrings)
{
    if (to == nullptr)
        return nullptr;

    // this shit is added by Krumo!
    if (hif::features::ITypeSpan *tsi = dynamic_cast<hif::features::ITypeSpan *>(to)) {
        return tsi->getSpan();
    } else if (manageStrings && dynamic_cast<String *>(to) != nullptr) {
        String *str = static_cast<String *>(to);
        return str->getSpanInformation();
    } else if (dynamic_cast<Bit *>(to) || dynamic_cast<Bool *>(to)) {
        // For bit and bool create a dummy range of length 1
        static Range ret(0, 0);
        return &ret;
    } else if (dynamic_cast<Char *>(to)) {
        // For char create a dummy range of length 8
        static Range ret(7ll, 0);
        return &ret;
    } else if (dynamic_cast<Enum *>(to)) {
        // For enum create a dummy range of length 32
        static Range ret(31ll, 0);
        return &ret;
    } else if (dynamic_cast<Reference *>(to)) {
        return typeGetSpan(static_cast<Reference *>(to)->getType(), sem);
    } else if (dynamic_cast<Pointer *>(to)) {
        static Range ret(sizeof(void *) * 8ll - 1ll, 0);
        return &ret;
    } else if (dynamic_cast<TypeReference *>(to)) {
        TypeReference *tr = static_cast<TypeReference *>(to);

        TypeReference::DeclarationType *decl = hif::semantics::getDeclaration(tr, sem);
        messageAssert((decl != nullptr), "Cannot find declaration", tr, sem);

        TypeTP *ttp = dynamic_cast<TypeTP *>(decl);
        if (ttp != nullptr) {
            return typeGetSpan(ttp->getType(), sem);
        }

        TypeDef *td = dynamic_cast<TypeDef *>(hif::manipulation::instantiate(tr, sem));
        if (td == nullptr) {
            messageError("Cannot instantiate typeref.", tr, sem);
        }

        return typeGetSpan(td->getType(), sem);
    }

    return nullptr;
}

bool typeSetConstexpr(Type *t, const bool v)
{
    if (t == nullptr)
        return false;

    if (dynamic_cast<CompositeType *>(t) != nullptr) {
        CompositeType *c = static_cast<CompositeType *>(t);
        return typeSetConstexpr(c->getType(), v);
    } else if (dynamic_cast<ScopedType *>(t) != nullptr) {
        ScopedType *c = static_cast<ScopedType *>(t);
        c->setConstexpr(v);
        return true;
    } else if (dynamic_cast<SimpleType *>(t) != nullptr) {
        SimpleType *c = static_cast<SimpleType *>(t);
        c->setConstexpr(v);
        return true;
    }

    return false;
}
bool typeSetSigned(Type *to, const bool sign, hif::semantics::ILanguageSemantics *refLang)
{
    if (to == nullptr)
        return false;

    if (dynamic_cast<Array *>(to)) {
        static_cast<Array *>(to)->setSigned(sign);
        return true;
    } else if (dynamic_cast<Bitvector *>(to)) {
        static_cast<Bitvector *>(to)->setSigned(sign);
        return true;
    } else if (dynamic_cast<Int *>(to)) {
        static_cast<Int *>(to)->setSigned(sign);
        return true;
    } else if (dynamic_cast<Pointer *>(to)) {
        return typeSetSigned(static_cast<Pointer *>(to)->getType(), sign, refLang);
    } else if (dynamic_cast<Reference *>(to)) {
        return typeSetSigned(static_cast<Reference *>(to)->getType(), sign, refLang);
    }
    // K: NEVER CHANGE DEFINITION PROPERTIES!
    // else if ( dynamic_cast<TypeReference*>(to) )
    // {
    //  Type *base = hif::semantics::getBaseType(to, false, refLang);
    //  return typeSetSigned (base, sign, refLang);
    // }

    return false;
}
bool typeSetLogic(Type *to, const bool logic, hif::semantics::ILanguageSemantics *refLang)
{
    if (to == nullptr)
        return false;

    if (dynamic_cast<Array *>(to)) {
        return typeSetLogic(static_cast<Array *>(to)->getType(), logic, refLang);
    } else if (dynamic_cast<Bit *>(to)) {
        static_cast<Bit *>(to)->setLogic(logic);
        return true;
    } else if (dynamic_cast<Bitvector *>(to)) {
        static_cast<Bitvector *>(to)->setLogic(logic);
        return true;
    } else if (dynamic_cast<Pointer *>(to)) {
        return typeSetLogic(static_cast<Pointer *>(to)->getType(), logic, refLang);
    } else if (dynamic_cast<Reference *>(to)) {
        return typeSetLogic(static_cast<Reference *>(to)->getType(), logic, refLang);
    }
    // K: NEVER CHANGE DEFINITION PROPERTIES!
    // else if ( dynamic_cast<TypeReference*>(to) )
    // {
    //  Type *base = hif::semantics::getBaseType(to, false, refLang);
    //  return typeSetLogic (base, logic, refLang);
    // }

    return false;
}
bool typeSetResolved(Type *to, const bool resolved, hif::semantics::ILanguageSemantics *refLang)
{
    if (to == nullptr)
        return false;

    if (dynamic_cast<Array *>(to) != nullptr) {
        return typeSetResolved(static_cast<Array *>(to)->getType(), resolved, refLang);
    } else if (dynamic_cast<Bit *>(to) != nullptr) {
        (static_cast<Bit *>(to))->setResolved(resolved);
        return true;
    } else if (dynamic_cast<Bitvector *>(to) != nullptr) {
        (static_cast<Bitvector *>(to))->setResolved(resolved);
        return true;
    } else if (dynamic_cast<Pointer *>(to) != nullptr) {
        return typeSetResolved(static_cast<Pointer *>(to)->getType(), resolved, refLang);
    } else if (dynamic_cast<Reference *>(to) != nullptr) {
        return typeSetResolved(static_cast<Reference *>(to)->getType(), resolved, refLang);
    }

    return false;
}

bool typeSetSpan(
    Type *to,
    Range *ro,
    hif::semantics::ILanguageSemantics *refLang,
    const bool deleteIfNotSet,
    const bool manageStrings)
{
    if (to == nullptr) {
        messageDebugAssert(to != nullptr, "Passed nullptr type", to, refLang);
        if (deleteIfNotSet)
            delete ro;
        return false;
    }

    if (dynamic_cast<Array *>(to)) {
        delete (static_cast<Array *>(to))->setSpan(ro);
        return true;
    } else if (manageStrings && dynamic_cast<String *>(to) != nullptr) {
        delete (static_cast<String *>(to))->setSpanInformation(ro);
        return true;
    } else if (dynamic_cast<Int *>(to)) {
        delete (static_cast<Int *>(to))->setSpan(ro);
        return true;
    } else if (dynamic_cast<Unsigned *>(to)) {
        delete (static_cast<Unsigned *>(to))->setSpan(ro);
        return true;
    } else if (dynamic_cast<Signed *>(to)) {
        delete (static_cast<Signed *>(to))->setSpan(ro);
        return true;
    } else if (dynamic_cast<Real *>(to)) {
        delete (static_cast<Real *>(to))->setSpan(ro);
        return true;
    } else if (dynamic_cast<Reference *>(to)) {
        return typeSetSpan(static_cast<Reference *>(to)->getType(), ro, refLang, deleteIfNotSet);
    } else if (dynamic_cast<Bitvector *>(to)) {
        delete (static_cast<Bitvector *>(to))->setSpan(ro);
        return true;
    } else if (dynamic_cast<Pointer *>(to)) {
        messageError("Unexpected case.", to, refLang);
    }

    if (deleteIfNotSet)
        delete ro;
    return false;
}

unsigned int typeGetCardinality(Type *type, hif::semantics::ILanguageSemantics *refLang, const bool considerOnlyBits)
{
    Type *t = hif::semantics::getBaseType(type, false, refLang);
    if (t == nullptr) {
        messageError("Base type not found.", type, refLang);
    }

    if (dynamic_cast<Array *>(t) != nullptr) {
        Array *a = static_cast<Array *>(t);
        return 1u + typeGetCardinality(a->getType(), refLang, considerOnlyBits);
    } else if (dynamic_cast<Unsigned *>(t) != nullptr) {
        return 1u;
    } else if (dynamic_cast<Bitvector *>(t) != nullptr) {
        return 1u;
    } else if (dynamic_cast<Signed *>(t) != nullptr) {
        return 1u;
    } else if (!considerOnlyBits) {
        if (dynamic_cast<Bit *>(t) == nullptr && dynamic_cast<Bool *>(t) == nullptr)
            return 1u;
        else
            return 0u;
    }

    return 0u;
}

Type *typeGetNestedType(Type *t, hif::semantics::ILanguageSemantics *refLang, std::int64_t depth)
{
    if (depth == 0)
        return t;

    CompositeType *com = dynamic_cast<CompositeType *>(t);
    if (com == nullptr) {
        if (depth < 0)
            return t;
        return nullptr;
    }

    t = com->getType();
    if (depth < 0)
        return typeGetNestedType(t, refLang, depth);

    --depth;
    return typeGetNestedType(t, refLang, depth);
}

bool typeDependsOnTemplates(Type *t, hif::semantics::ILanguageSemantics *sem)
{
    hif::semantics::SymbolList symbols;
    hif::semantics::collectSymbols(symbols, t, sem, true);
    for (hif::semantics::SymbolList::iterator i = symbols.begin(); i != symbols.end(); ++i) {
        Object *symb      = *i;
        Declaration *decl = hif::semantics::getDeclaration(symb, sem);
        if (decl == nullptr)
            continue;
        ValueTP *vtp = dynamic_cast<ValueTP *>(decl);
        TypeTP *ttp  = dynamic_cast<TypeTP *>(decl);
        if (vtp == nullptr && ttp == nullptr)
            continue;
        return true;
    }
    return false;
}

} // namespace hif
