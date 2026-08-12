/// @file renameInScope.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/renameInScope.hpp"

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

bool renameInScope(Declaration *decl, hif::semantics::ILanguageSemantics *refSem)
{
    Scope *scope = hif::getNearestParent<Scope>(decl);
    messageAssert(scope != nullptr, "Unexpected case (1)", nullptr, nullptr);

    System *sys = hif::getNearestParent<System>(decl);
    messageAssert(sys != nullptr, "Unexpected case (2)", nullptr, nullptr);

    std::string newName = moveDeclaration(decl, scope, sys, refSem, "");
    return !newName.empty();
}

} // namespace manipulation
} // namespace hif
