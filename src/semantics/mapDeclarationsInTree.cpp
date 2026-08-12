/// @file mapDeclarationsInTree.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/mapDeclarationsInTree.hpp"

#include "hif/application_utils/application_utils.hpp"
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

void mapDeclarationsInTree(Object *root, Object *oldTree, Object *newTree, ILanguageSemantics *sem)
{
    hif::application_utils::initializeLogHeader("HIF", "mapDeclarationsInTree");

    if (root == nullptr || oldTree == nullptr || newTree == nullptr)
        return;
    std::list<Object *> list;
    collectSymbols(list, root);

    for (std::list<Object *>::iterator i = list.begin(); i != list.end(); ++i) {
        hif::semantics::DeclarationOptions dopt;
        dopt.dontSearch   = true;
        Declaration *decl = getDeclaration(*i, sem, dopt);
        if (decl == nullptr)
            continue;
        if (!hif::isSubNode(decl, oldTree))
            continue;

        Declaration *newDecl = dynamic_cast<Declaration *>(hif::manipulation::matchObject(decl, oldTree, newTree, sem));

        if (newDecl == nullptr) {
            messageDebug("old tree is", oldTree, sem);
            messageDebug("new tree is", newTree, sem);
            messageError("newDecl not matched in new Tree", decl, sem);
        }

        hif::semantics::setDeclaration(*i, newDecl);
    }

    hif::application_utils::restoreLogHeader();
}

} // namespace semantics
} // namespace hif
