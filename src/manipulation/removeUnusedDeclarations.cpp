/// @file removeUnusedDeclarations.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/removeUnusedDeclarations.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /* anon */
{
class RemoveVisitor : public hif::HifVisitor
{
public:
    // Global traits.
    typedef std::set<Declaration *> DeclarationSet;

    RemoveVisitor(const RemoveUnusedDeclarationOptions &options, hif::semantics::ILanguageSemantics *sem);
    virtual ~RemoveVisitor();

    bool collect(Declaration *decl);

protected:
    int visitAlias(Alias &o);
    int visitConst(Const &o);
    int visitEnumValue(EnumValue &o);
    int visitField(Field &o);
    int visitFunction(Function &o);
    int visitLibraryDef(LibraryDef &o);
    int visitParameter(Parameter &o);
    int visitPort(Port &o);
    int visitProcedure(Procedure &o);
    int visitSignal(Signal &o);
    int visitStateTable(StateTable &o);
    int visitTypeDef(TypeDef &o);
    int visitTypeTP(TypeTP &o);
    int visitValueTP(ValueTP &o);
    int visitVariable(Variable &o);
    int visitView(View &o);

private:
    void _remove(Declaration *o, const bool removeFlag);

    const RemoveUnusedDeclarationOptions &_options;
    hif::semantics::ILanguageSemantics *_sem;
    DeclarationSet _trash;

    // Disabled.
    RemoveVisitor(const RemoveVisitor &);
    RemoveVisitor operator=(const RemoveVisitor &);
};

RemoveVisitor::RemoveVisitor(const RemoveUnusedDeclarationOptions &options, hif::semantics::ILanguageSemantics *sem)
    : _options(options)
    , _sem(sem)
    , _trash()
{
    // ntd
}

RemoveVisitor::~RemoveVisitor()
{
    // Cannot delete directly to avoid double-deletes in case of nested declaration
    // (both collected)

    typedef std::list<Declaration *> DeclarationList;
    DeclarationList trashList;

    for (DeclarationSet::iterator i = _trash.begin(); i != _trash.end(); ++i) {
        bool insert = true;
        for (DeclarationList::iterator j(trashList.begin()); j != trashList.end();) {
            const bool iSubNode = hif::isSubNode(*i, *j);
            const bool jSubNode = hif::isSubNode(*j, *i);

            if (!iSubNode && !jSubNode) {
                ++j;
            } else if (iSubNode) {
                insert = false;
                break;
            } else // if (jSubNode)
            {
                j = trashList.erase(j);
            }
        }

        if (insert) {
            trashList.push_back(*i);
        }
    }

    // actual delete
    for (DeclarationList::iterator j(trashList.begin()); j != trashList.end(); ++j) {
        Declaration *d     = (*j);
        DesignUnit *parent = nullptr;
        if (dynamic_cast<View *>(d) != nullptr)
            parent = dynamic_cast<DesignUnit *>(static_cast<View *>(d)->getParent());
        d->replace(nullptr);
        delete d;
        if (parent != nullptr && parent->views.empty()) {
            parent->replace(nullptr);
            delete parent;
        }
    }
}

bool RemoveVisitor::collect(Declaration *decl)
{
    const bool yet = _trash.find(decl) != _trash.end();
    decl->acceptVisitor(*this);
    return (!yet && _trash.find(decl) != _trash.end());
}

int RemoveVisitor::visitConst(Const &o)
{
    _remove(&o, _options.removeConstants);
    return 0;
}

int RemoveVisitor::visitFunction(Function &o)
{
    _remove(&o, _options.removeSubPrograms);
    return 0;
}

int RemoveVisitor::visitParameter(Parameter &o)
{
    _remove(&o, _options.removeParams);
    return 0;
}

int RemoveVisitor::visitProcedure(Procedure &o)
{
    _remove(&o, _options.removeSubPrograms);
    return 0;
}

int RemoveVisitor::visitTypeDef(TypeDef &o)
{
    _remove(&o, _options.removeTypeDefs);
    return 0;
}

int RemoveVisitor::visitTypeTP(TypeTP &o)
{
    _remove(&o, _options.removeTypeTPs);
    return 0;
}

int RemoveVisitor::visitValueTP(ValueTP &o)
{
    _remove(&o, _options.removeValueTPs);
    return 0;
}

int RemoveVisitor::visitVariable(Variable &o)
{
    _remove(&o, _options.removeVariables);
    return 0;
}

int RemoveVisitor::visitView(View &o)
{
    _remove(&o, _options.removeView);
    return 0;
}

int RemoveVisitor::visitAlias(Alias &o)
{
    _remove(&o, _options.removeAlias);
    return 0;
}

int RemoveVisitor::visitEnumValue(EnumValue &o)
{
    _remove(&o, _options.removeEnumValue);
    return 0;
}

int RemoveVisitor::visitField(Field &o)
{
    _remove(&o, _options.removeField);
    return 0;
}

int RemoveVisitor::visitLibraryDef(LibraryDef &o)
{
    _remove(&o, _options.removeLibraryDef);
    return 0;
}

int RemoveVisitor::visitPort(Port &o)
{
    _remove(&o, _options.removePort);
    return 0;
}

int RemoveVisitor::visitSignal(Signal &o)
{
    _remove(&o, _options.removeSignal);
    return 0;
}

int RemoveVisitor::visitStateTable(StateTable &o)
{
    _remove(&o, _options.removeStateTable);
    return 0;
}

void RemoveVisitor::_remove(Declaration *o, const bool removeFlag)
{
    if (!_options.removeAll && !removeFlag)
        return;
    _trash.insert(o);
}
bool _isCppCtorOrDtor(Declaration *d)
{
    Function *f = dynamic_cast<Function *>(d);
    if (f != nullptr)
        return (f->getName() == hif::NameTable::getInstance()->hifConstructor());

    Procedure *p = dynamic_cast<Procedure *>(d);
    if (p != nullptr)
        return (p->getName() == hif::NameTable::getInstance()->hifDestructor());

    return false;
}

void _removeDeclarationSubtree(
    Declaration *d,
    hif::semantics::ILanguageSemantics *sem,
    const RemoveUnusedDeclarationOptions &options,
    RemoveVisitor &rv)
{
    if (d == options.top)
        return;

    if (!(*options.allReferencesMap)[d].empty())
        return;

    DeclarationIsPartOfStandardOptions opts;
    opts.dontCheckStandardViews = !options.skipStandardViews;
    if ((options.skipStandardLibs || options.skipStandardViews) && hif::declarationIsPartOfStandard(d, opts))
        return;

    if (_isCppCtorOrDtor(d))
        return;

    if (isSubNode(options.top, d))
        return;

    const bool collected = rv.collect(d);
    if (!collected)
        return;

    typedef std::list<Object *> ObjectList;
    ObjectList symbolList;
    hif::semantics::collectSymbols(symbolList, d, sem);

    for (ObjectList::iterator it(symbolList.begin()); it != symbolList.end(); ++it) {
        Declaration *symbDecl = hif::semantics::getDeclaration(*it, sem);
        if (symbDecl == nullptr)
            continue;

        (*options.allReferencesMap)[symbDecl].erase(*it);
        _removeDeclarationSubtree(symbDecl, sem, options, rv);
    }
}

} // namespace
RemoveUnusedDeclarationOptions::RemoveUnusedDeclarationOptions()
    : skipStandardLibs(false)
    , skipStandardViews(false)
    , removeAll(true)
    , removeConstants(false)
    , removeParams(false)
    , removeSubPrograms(false)
    , removeTypeDefs(false)
    , removeTypeTPs(false)
    , removeValueTPs(false)
    , removeVariables(false)
    , removeView(false)
    , removeAlias(false)
    , removeEnumValue(false)
    , removeField(false)
    , removeLibraryDef(false)
    , removePort(false)
    , removeSignal(false)
    , removeStateTable(false)
    , top(nullptr)
    , declarationsTarget(nullptr)
    , allReferencesMap(nullptr)
{
    // ntd
}

RemoveUnusedDeclarationOptions::~RemoveUnusedDeclarationOptions()
{
    // ntd
}

RemoveUnusedDeclarationOptions::RemoveUnusedDeclarationOptions(const RemoveUnusedDeclarationOptions &o)
    : skipStandardLibs(o.skipStandardLibs)
    , skipStandardViews(o.skipStandardViews)
    , removeAll(o.removeAll)
    , removeConstants(o.removeConstants)
    , removeParams(o.removeParams)
    , removeSubPrograms(o.removeSubPrograms)
    , removeTypeDefs(o.removeTypeDefs)
    , removeTypeTPs(o.removeTypeTPs)
    , removeValueTPs(o.removeValueTPs)
    , removeVariables(o.removeVariables)
    , removeView(o.removeView)
    , removeAlias(o.removeAlias)
    , removeEnumValue(o.removeEnumValue)
    , removeField(o.removeField)
    , removeLibraryDef(o.removeLibraryDef)
    , removePort(o.removePort)
    , removeSignal(o.removeSignal)
    , removeStateTable(o.removeStateTable)
    , top(o.top)
    , declarationsTarget(o.declarationsTarget)
    , allReferencesMap(o.allReferencesMap)
{
    // ntd
}

RemoveUnusedDeclarationOptions &RemoveUnusedDeclarationOptions::operator=(const RemoveUnusedDeclarationOptions &o)
{
    if (&o == this)
        return *this;

    removeAll          = o.removeAll;
    skipStandardLibs   = o.skipStandardLibs;
    skipStandardViews  = o.skipStandardViews;
    removeConstants    = o.removeConstants;
    removeParams       = o.removeParams;
    removeSubPrograms  = o.removeSubPrograms;
    removeTypeDefs     = o.removeTypeDefs;
    removeTypeTPs      = o.removeTypeTPs;
    removeValueTPs     = o.removeValueTPs;
    removeVariables    = o.removeVariables;
    declarationsTarget = o.declarationsTarget;
    allReferencesMap   = o.allReferencesMap;
    removeView         = o.removeView;
    removeAlias        = o.removeAlias;
    removeEnumValue    = o.removeEnumValue;
    removeField        = o.removeField;
    removeLibraryDef   = o.removeLibraryDef;
    removePort         = o.removePort;
    top                = o.top;
    removeSignal       = o.removeSignal;
    removeStateTable   = o.removeStateTable;

    return *this;
}

bool removeUnusedDeclarations(
    System *root,
    hif::semantics::ILanguageSemantics *sem,
    const RemoveUnusedDeclarationOptions &options)
{
    if (root == nullptr)
        return false;

    RemoveUnusedDeclarationOptions localOptions(options);

    if (!localOptions.removeAll && !localOptions.removeConstants && !localOptions.removeParams &&
        !localOptions.removeSubPrograms && !localOptions.removeTypeDefs && !localOptions.removeTypeTPs &&
        !localOptions.removeValueTPs && !localOptions.removeVariables && !localOptions.removeView &&
        !localOptions.removeAlias && !localOptions.removeEnumValue && !localOptions.removeField &&
        !localOptions.removeLibraryDef && !localOptions.removePort && !localOptions.removeSignal &&
        !localOptions.removeStateTable) {
        // nothing to removes.
        return true;
    }

    const bool calculateRefMap = (localOptions.allReferencesMap == nullptr);
    if (calculateRefMap) {
        localOptions.allReferencesMap = new RemoveUnusedDeclarationOptions::ReferencesMap();

        hif::semantics::GetReferencesOptions opt;
        opt.include_unreferenced = true;
        hif::semantics::getAllReferences(*localOptions.allReferencesMap, sem, root, opt);
    }

    if (localOptions.top == nullptr) {
        localOptions.top = hif::manipulation::findTopLevelModule(root, sem);
        if (localOptions.top == nullptr) {
            if (calculateRefMap)
                delete localOptions.allReferencesMap;
            return false;
        }
    }

    RemoveVisitor rv(localOptions, sem);

    if (localOptions.declarationsTarget != nullptr) {
        for (RemoveUnusedDeclarationOptions::DeclarationSet::iterator i = localOptions.declarationsTarget->begin();
             i != localOptions.declarationsTarget->end(); ++i) {
            _removeDeclarationSubtree(*i, sem, localOptions, rv);
        }
    } else //if (localOptions.allReferencesMap != nullptr)
    {
        for (RemoveUnusedDeclarationOptions::ReferencesMap::iterator i = localOptions.allReferencesMap->begin();
             i != localOptions.allReferencesMap->end(); ++i) {
            _removeDeclarationSubtree(i->first, sem, localOptions, rv);
        }
    }

    if (calculateRefMap)
        delete localOptions.allReferencesMap;
    return true;
}

} // namespace manipulation
} // namespace hif
