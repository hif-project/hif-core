/// @file terminalPrefixUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/terminalPrefixUtils.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

Value *getTerminalPrefix(Value *obj, const TerminalPrefixOptions &opt)
{
    if (obj == nullptr)
        return nullptr;

    if (dynamic_cast<FieldReference *>(obj)) {
        FieldReference *v = static_cast<FieldReference *>(obj);
        if (opt.recurseIntoFieldRefs)
            return getTerminalPrefix(v->getPrefix(), opt);
        return v;
    } else if (dynamic_cast<Member *>(obj)) {
        Member *v = static_cast<Member *>(obj);
        if (opt.recurseIntoMembers)
            return getTerminalPrefix(v->getPrefix(), opt);
        return v;
    } else if (dynamic_cast<Slice *>(obj)) {
        Slice *v = static_cast<Slice *>(obj);
        if (opt.recurseIntoSlices)
            return getTerminalPrefix(v->getPrefix(), opt);
        return v;
    } else if (dynamic_cast<Expression *>(obj)) {
        Expression *v = static_cast<Expression *>(obj);
        if (opt.recurseIntoDerefExpressions && v->getOperator() == op_deref)
            return getTerminalPrefix(v->getValue1(), opt);
        return v;
    }

    return obj;
}
Value *setTerminalPrefix(Value *obj, Value *newPrefix, const TerminalPrefixOptions &opt)
{
    if (obj == nullptr)
        return nullptr;
    Value *ret = getTerminalPrefix(obj, opt);

    ret->replace(newPrefix);

    return ret;
}

} // namespace hif
