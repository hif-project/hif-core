/// @file replaceOccurrencesInScope.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/replaceOccurrencesInScope.hpp"

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

void replaceOccurrencesInScope(Declaration *decl, Object *to, hif::semantics::ILanguageSemantics *refLang, Object *from)
{
    hif::semantics::ReferencesSet refs;
    hif::semantics::getReferences(decl, refs, refLang, from);

    for (hif::semantics::ReferencesSet::iterator i = refs.begin(); i != refs.end(); ++i) {
        (*i)->replace(hif::copy(to));
    }
}

} // namespace manipulation
} // namespace hif
