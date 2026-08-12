/// @file getNearestScope.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/getNearestScope.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

Scope *getNearestScope(Object *o, bool needDeclarationList, bool needLibraryList, bool needTemplates)
{
    if (o == nullptr)
        return nullptr;
    bool found = false;

    if (dynamic_cast<StateTable *>(o) != nullptr) {
        if (!needLibraryList && !needTemplates)
            found = true;
    } else if (dynamic_cast<SubProgram *>(o) != nullptr) {
        if (!needLibraryList && !needDeclarationList)
            found = true;
    } else if (dynamic_cast<BaseContents *>(o) != nullptr) {
        if (!needLibraryList && !needTemplates)
            found = true;
    } else if (dynamic_cast<Contents *>(o) != nullptr) {
        if (!needTemplates)
            found = true;
    } else if (dynamic_cast<View *>(o) != nullptr) {
        found = true;
    } else if (dynamic_cast<LibraryDef *>(o) != nullptr) {
        if (!needTemplates)
            found = true;
    } else if (dynamic_cast<System *>(o) != nullptr) {
        if (!needTemplates)
            found = true;
    }

    if (found)
        return dynamic_cast<Scope *>(o);

    return getNearestScope(getNearestParent<Scope>(o), needDeclarationList, needLibraryList, needTemplates);
}

} // namespace hif
