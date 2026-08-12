/// @file setDeclaration.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/setDeclaration.hpp"

#include "hif/application_utils/Log.hpp"
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

void setDeclaration(Object *o, Object *decl)
{
    hif::application_utils::initializeLogHeader("HIF", "setDeclaration");

    // checks
    messageAssert(o != nullptr, "Passed nullptr object", nullptr, nullptr);

    hif::features::ISymbol *symb = dynamic_cast<hif::features::ISymbol *>(o);
    messageAssert(symb != nullptr, "Passed non-symbol object", o, nullptr);

    symb->setDeclaration(decl);

    hif::application_utils::restoreLogHeader();
}

} // namespace semantics
} // namespace hif
