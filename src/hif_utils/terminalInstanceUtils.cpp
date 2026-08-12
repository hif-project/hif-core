/// @file terminalInstanceUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/terminalInstanceUtils.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

Object *getTerminalInstance(Object *obj)
{
    if (obj == nullptr)
        return nullptr;

    if (dynamic_cast<Library *>(obj)) {
        Library *v = static_cast<Library *>(obj);
        if (v->getInstance() == nullptr)
            return obj;
        return getTerminalInstance(v->getInstance());
    } else if (dynamic_cast<ViewReference *>(obj)) {
        ViewReference *v = static_cast<ViewReference *>(obj);
        if (v->getInstance() == nullptr)
            return obj;
        return getTerminalInstance(v->getInstance());
    } else if (dynamic_cast<TypeReference *>(obj)) {
        TypeReference *v = static_cast<TypeReference *>(obj);
        if (v->getInstance() == nullptr)
            return obj;
        return getTerminalInstance(v->getInstance());
    } else if (dynamic_cast<Instance *>(obj)) {
        Instance *v = static_cast<Instance *>(obj);
        if (v->getReferencedType() == nullptr)
            return obj;
        return getTerminalInstance(v->getReferencedType());
    } else if (dynamic_cast<FunctionCall *>(obj)) {
        FunctionCall *v = static_cast<FunctionCall *>(obj);
        if (v->getInstance() == nullptr)
            return obj;
        return getTerminalInstance(v->getInstance());
    } else if (dynamic_cast<ProcedureCall *>(obj)) {
        ProcedureCall *v = static_cast<ProcedureCall *>(obj);
        if (v->getInstance() == nullptr)
            return obj;
        return getTerminalInstance(v->getInstance());
    } else if (dynamic_cast<FieldReference *>(obj)) {
        FieldReference *v = static_cast<FieldReference *>(obj);
        if (v->getPrefix() == nullptr)
            return obj;
        hif::TerminalPrefixOptions opt;
        opt.recurseIntoSlices  = false;
        opt.recurseIntoMembers = false;
        Value *tp              = hif::getTerminalPrefix(v, opt);
        Instance *inst         = dynamic_cast<Instance *>(tp);
        if (inst == nullptr)
            return obj;
        return getTerminalInstance(inst);
    }

    return obj;
}

} // namespace hif
