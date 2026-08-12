/// @file expandAliases.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/expandAliases.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"
#include "hif/trash.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{
class ReplaceAliasesVisitor : public GuideVisitor
{
public:
    ReplaceAliasesVisitor(hif::semantics::ILanguageSemantics *sem);
    virtual ~ReplaceAliasesVisitor();

    int visitAlias(Alias &o);

private:
    // disable
    ReplaceAliasesVisitor(const ReplaceAliasesVisitor &);
    ReplaceAliasesVisitor &operator=(const ReplaceAliasesVisitor &);

    Trash _trash;
    hif::semantics::ILanguageSemantics *_sem;
};
ReplaceAliasesVisitor::ReplaceAliasesVisitor(hif::semantics::ILanguageSemantics *sem)
    : GuideVisitor()
    , _trash()
    , _sem(sem)
{
}
ReplaceAliasesVisitor::~ReplaceAliasesVisitor() { _trash.clear(); }
int ReplaceAliasesVisitor::visitAlias(Alias &o)
{
    GuideVisitor::visitAlias(o);
    _trash.insert(&o);

    Scope *s = hif::getNearestParent<Scope>(&o);
    messageDebugAssert(s != nullptr, "Cannot find nearest scope", &o, nullptr);

    hif::semantics::ReferencesSet refList;
    hif::semantics::getReferences(&o, refList, _sem, s);

    for (hif::semantics::ReferencesSet::iterator i = refList.begin(); i != refList.end(); ++i) {
        (*i)->replace(hif::copy(o.getValue()));
        delete *i;
    }

    return 0;
}

} // namespace

void expandAliases(Object *root, hif::semantics::ILanguageSemantics *refLang)
{
    ReplaceAliasesVisitor v(refLang);
    root->acceptVisitor(v);
}

} // namespace manipulation
} // namespace hif
