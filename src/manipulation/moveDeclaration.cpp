/// @file moveDeclaration.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/moveDeclaration.hpp"

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

std::string moveDeclaration(
    Declaration *decl,
    Scope *newScope,
    Object *subtree,
    hif::semantics::ILanguageSemantics *refSem,
    const std::string &suffix,
    int pos)
{
    if (newScope == nullptr || decl == nullptr || subtree == nullptr) {
        messageError("Unexpected arguments give to the function", nullptr, nullptr);
    }

    // check if declaration name conflicts into the new scope
    bool found = false;

    BList<Declaration> *new_decl_list = nullptr;
    if (dynamic_cast<Parameter *>(decl) != nullptr) {
        new_decl_list = reinterpret_cast<BList<Declaration> *>(hif::objectGetParameterList(newScope));
    } else if (dynamic_cast<ValueTP *>(decl) != nullptr || dynamic_cast<TypeTP *>(decl) != nullptr) {
        new_decl_list = hif::objectGetTemplateParameterList(newScope);
    } else {
        new_decl_list = hif::objectGetDeclarationList(newScope);
    }
    messageAssert(new_decl_list != nullptr, "Unexpcted declaration list", nullptr, refSem);
    for (BList<Declaration>::iterator i = new_decl_list->begin(); i != new_decl_list->end(); ++i) {
        if ((*i)->getName() != decl->getName())
            continue;
        found = true;
        break;
    }

    // if conflicts, renaming.
    if (found) {
        hif::semantics::ReferencesSet list;
        hif::semantics::getReferences(decl, list, refSem, subtree);

        std::string newName = decl->getName();
        if (suffix != "") {
            std::string n(decl->getName());
            newName = n + suffix;
        }
        newName = NameTable::getInstance()->getFreshName(newName);

        decl->setName(newName);
        for (hif::semantics::ReferencesSet::iterator i = list.begin(); i != list.end(); ++i) {
            objectSetName(*i, newName);
        }
    }

    // If declaration is not moved (i.e. just renamed), all done.
    BList<Declaration> *origList = reinterpret_cast<BList<Declaration> *>(decl->getBList());
    if (origList == new_decl_list)
        return decl->getName();

    // removing from old declaration list (without deleting)
    BList<Declaration>::iterator it(decl);
    it.remove();

    // push back in new blist
    if (pos < 0)
        new_decl_list->push_back(decl);
    else
        new_decl_list->insert(decl, static_cast<unsigned int>(pos), true);

    return decl->getName();
}

} // namespace manipulation
} // namespace hif
