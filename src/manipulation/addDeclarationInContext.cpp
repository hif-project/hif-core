/// @file addDeclarationInContext.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/addDeclarationInContext.hpp"

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

bool addDeclarationInContext(Declaration *newDecl, Declaration *context, bool before)
{
    if (dynamic_cast<Port *>(newDecl) != nullptr) {
        Port *newPortDecl = static_cast<Port *>(newDecl);
        Port *p           = dynamic_cast<Port *>(context);
        if (p == nullptr) {
            View *v = hif::getNearestParent<View>(context);
            messageAssert(v != nullptr, "Cannot find parent view", context, nullptr);
            v->getEntity()->ports.push_back(newPortDecl);
            return true;
        }

        BList<Declaration>::iterator it(context);
        if (before)
            it.insert_before(newDecl);
        else
            it.insert_after(newDecl);

        return true;
    } else if (dynamic_cast<StateTable *>(newDecl) != nullptr) {
        StateTable *newProcessDecl = static_cast<StateTable *>(newDecl);
        View *v                    = hif::getNearestParent<View>(context, true);
        messageAssert(v != nullptr, "Cannot find parent view", context, nullptr);
        v->getContents()->stateTables.push_back(newProcessDecl);
        return true;
    }

    Port *p = dynamic_cast<Port *>(context);
    if (p != nullptr) {
        View *v = hif::getNearestParent<View>(p);
        messageAssert(v != nullptr, "Cannot find parent view", p, nullptr);
        v->getContents()->declarations.push_front(newDecl);
        return true;
    }

    BList<Declaration>::iterator it(context);
    if (before)
        it.insert_before(newDecl);
    else
        it.insert_after(newDecl);

    return true;
}

} // namespace manipulation
} // namespace hif
