/// @file collectLeftHandSideSymbols.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/collectLeftHandSideSymbols.hpp"

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

std::list<Value *> collectLeftHandSideSymbols(Assign *obj, const LeftHandSideOptions &opt)
{
    std::list<Value *> ret;
    std::list<Object *> list;
    hif::semantics::collectSymbols(list, obj->getLeftHandSide());

    for (std::list<Object *>::iterator it = list.begin(); it != list.end(); ++it) {
        Identifier *id     = dynamic_cast<Identifier *>(*it);
        FieldReference *fr = dynamic_cast<FieldReference *>(*it);
        if (id == nullptr && fr == nullptr)
            continue;
        if (!isInLeftHandSide(*it, opt))
            continue;

        Value *v = static_cast<Value *>(*it);
        ret.push_back(v);
    }
    return ret;
}
} // namespace manipulation
} // namespace hif
