/// @file mergeTrees.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{
class BindVisitor : public GuideVisitor
{
public:
    BindVisitor(hif::semantics::ILanguageSemantics *sem);
    ~BindVisitor();

    int visitInstance(Instance &o);

private:
    BindVisitor(const BindVisitor &);
    BindVisitor &operator=(const BindVisitor &);

    hif::semantics::ILanguageSemantics *_sem;
};

BindVisitor::BindVisitor(hif::semantics::ILanguageSemantics *sem)
    : _sem(sem)
{
    // ntd
}

BindVisitor::~BindVisitor()
{
    // ntd
}

int BindVisitor::visitInstance(Instance &o)
{
    GuideVisitor::visitInstance(o);

    BaseContents *bp = dynamic_cast<BaseContents *>(o.getParent());
    if (bp != nullptr && o.isInBList() && o.getBList() == reinterpret_cast<hif::BList<hif::Object> *>(&bp->instances)) {
        for (BList<PortAssign>::iterator i = o.portAssigns.begin(); i != o.portAssigns.end(); ++i) {
            PortAssign *pa = *i;

            Type *paType = hif::semantics::getSemanticType(pa, _sem);
            messageAssert(paType != nullptr, "Cannot type portAssign", pa, _sem);

            Type *vType = hif::semantics::getSemanticType(pa->getValue(), _sem);
            messageAssert(vType != nullptr, "Cannot type portAssign value", pa->getValue(), _sem);

            if (_sem->getExprType(paType, vType, op_bind, pa).returnedType == nullptr) {
                Cast *cc = new Cast();
                cc->setType(hif::copy(paType));
                cc->setValue(pa->setValue(nullptr));
                pa->setValue(cc);
            }
        }
    }

    return 0;
}

typedef std::set<Object *> ReferencesSet;
typedef std::map<Declaration *, ReferencesSet> RefMap;

/// This visitor takes a reference tree and visit another one, choosing when
/// putting/merging something inside the reference one
/// Mainly based on top-down strategy exploiting getDeclaration()
class Bach : public HifVisitor
{
public:
    Bach(hif::semantics::ILanguageSemantics *sem, const MergeTreesOptions &opt);
    ~Bach();

    /// @name Methods used to dynamically dispact special visits.
    /// @{

    int visitFunction(Function &o);
    int visitProcedure(Procedure &o);
    int visitView(View &o);
    int visitTypeDef(TypeDef &o);

    int visitFunctionCall(FunctionCall &o);
    int visitProcedureCall(ProcedureCall &o);
    int visitViewReference(ViewReference &o);
    int visitTypeReference(TypeReference &o);

    /// @}

    /// @brief Merge given trees.
    ///
    /// @param o1 The first tree.
    /// @param o2 The second tree.
    ///
    void merge(Object *o1, Object *o2);

    /// @brief Set the references map.
    ///
    /// @param refMap The filled map.
    ///
    void setRefrenceMap(RefMap &refMap);

private:
    void _mergeFields(Object *o1, Object **f1, Object **f2);
    void _mergeBLists(BList<Object> *l1, BList<Object> *l2);

    /// @brief The reference semantics.
    hif::semantics::ILanguageSemantics *_sem;

    /// @brief The options.
    const MergeTreesOptions &_opt;

    /// @brief The reference map of tree to be merged.
    RefMap *_refMap;

    /// @brief Used to pass second parameter to the visitor.
    Object *_currentSource;

    /// @brief Equals options for skipping children.
    hif::EqualsOptions _skipChildrenOpt;

    /// @brief Equals options for managing special cases.
    hif::EqualsOptions _manageSpecialCasesOpt;

    /// @brief Equals options for typedef special case management.
    hif::EqualsOptions _recordSpecialCases;

    /// @brief Assures matching names for same declarations.
    void _fixNames(Declaration *decl);

    /// @brief Dispatches calls on declarations of set symbols.
    void _fixSymbolDeclaration(Object *symbol);

    /// @brief Matches special classes of objects.
    bool _mergeBranches(Object *&found, Object *o1, Object *o2);

    Bach(const Bach &);
    Bach &operator=(const Bach &);
};
Bach::Bach(semantics::ILanguageSemantics *sem, const MergeTreesOptions &opt)
    : _sem(sem)
    , _opt(opt)
    , _refMap(nullptr)
    , _currentSource(nullptr)
    , _skipChildrenOpt()
    , _manageSpecialCasesOpt()
    , _recordSpecialCases()
{
    _skipChildrenOpt.skipChilden = true;

    _manageSpecialCasesOpt.skipChilden             = true;
    _manageSpecialCasesOpt.skipDeclarationBodies   = true;
    _manageSpecialCasesOpt.handleExternalsTypedefs = true;
    _manageSpecialCasesOpt.skipViewContents        = opt.isIpxact;

    _recordSpecialCases.skipChilden             = true;
    _recordSpecialCases.handleExternalsTypedefs = true;
}
Bach::~Bach()
{
    // ntd
}

int Bach::visitFunction(Function &o)
{
    _fixNames(&o);
    return 0;
}

int Bach::visitProcedure(Procedure &o)
{
    _fixNames(&o);
    return 0;
}

int Bach::visitView(View &o)
{
    _fixNames(&o);
    return 0;
}

int Bach::visitTypeDef(TypeDef &o)
{
    _fixNames(&o);
    return 0;
}

int Bach::visitFunctionCall(FunctionCall &o)
{
    _fixSymbolDeclaration(&o);
    return 0;
}

int Bach::visitProcedureCall(ProcedureCall &o)
{
    _fixSymbolDeclaration(&o);
    return 0;
}

int Bach::visitViewReference(ViewReference &o)
{
    _fixSymbolDeclaration(&o);
    return 0;
}

int Bach::visitTypeReference(TypeReference &o)
{
    _fixSymbolDeclaration(&o);
    return 0;
}
void Bach::merge(Object *o1, Object *o2)
{
    // Merging fields:
    {
        const Object::Fields &fields1 = o1->getFields();
        const Object::Fields &fields2 = o2->getFields();

        messageAssert(fields1.size() == fields2.size(), "Unexpected sizes.", o1, _sem);

        Object::Fields::const_iterator i1 = fields1.begin();
        Object::Fields::const_iterator i2 = fields2.begin();
        for (; i1 != fields1.end(); ++i1, ++i2) {
            _mergeFields(o1, *i1, *i2);
        }
    }

    // Merging BLists:
    {
        const Object::BLists &blists1 = o1->getBLists();
        const Object::BLists &blists2 = o2->getBLists();

        messageAssert(blists1.size() == blists2.size(), "Unexpected sizes.", o1, _sem);

        Object::BLists::const_iterator i1 = blists1.begin();
        Object::BLists::const_iterator i2 = blists2.begin();
        for (; i1 != blists1.end(); ++i1, ++i2) {
            _mergeBLists(*i1, *i2);
        }
    }
}

void Bach::setRefrenceMap(RefMap &refMap) { _refMap = &refMap; }
void Bach::_mergeFields(Object *o1, Object **f1, Object **f2)
{
    if (*f1 == nullptr && *f2 == nullptr) {
        // ntd
    } else if (*f1 != nullptr && *f2 == nullptr) {
        // Keep f1 --> ntd
        //delete *f2;
    } else if (*f1 == nullptr && *f2 != nullptr) {
        // Keep f2: set it inside o1
        Object *tmp = *f2;
        tmp->replace(nullptr);
        o1->setChild(*f1, tmp);
    } else //if (*f1 != nullptr && *f2 != nullptr)
    {
        const bool areSpecialRecords = hif::equals(*f1, *f2, _recordSpecialCases);
        const bool areEquals         = hif::equals(*f1, *f2, _skipChildrenOpt);
        if (areSpecialRecords && !areEquals) {
            // Special case: typedef.
            TypeDef *td1 = dynamic_cast<TypeDef *>(*f1);
            TypeDef *td2 = dynamic_cast<TypeDef *>(*f2);
            messageDebug("f1:", *f1, _sem);
            messageDebug("f2:", *f2, _sem);
            messageAssert(td1 != nullptr && td2 != nullptr, "Unexpected case.", nullptr, nullptr);
            if (td2->isExternal()) {
                // Keep td1: ntd.
                (*f2)->replace(nullptr);
                //delete *f2;
            } else if (td1->isExternal()) {
                // Keep td2: moving
                td2->replace(nullptr);
                td1->replace(td2);
                //delete td1;
            } else {
                // Recursive call:
                merge(*f1, *f2);
                (*f2)->replace(nullptr);
                //delete *f2;
            }
            return;
        }

        const bool areSpecialMethods = hif::equals(*f1, *f2, _manageSpecialCasesOpt);
        if (areSpecialMethods && !areEquals) {
            // Fixing eventual special names in methods:
            _currentSource = *f2;
            (*f1)->acceptVisitor(*this);
            merge(*f1, *f2);
            return;
        }

        if (!areEquals) {
            // Maybe special case:
            messageDebug("f1:", *f1, _sem);
            messageDebug("f2:", *f2, _sem);
            messageError("Unexpected case.", nullptr, nullptr);
        }

        // Recursive merge call:
        merge(*f1, *f2);
    }
}
void Bach::_mergeBLists(BList<Object> *l1, BList<Object> *l2)
{
    BList<Object> pendingMerge;

    for (BList<Object>::iterator jt = l2->begin(); jt != l2->end();) {
        Object *found                   = nullptr;
        hif::features::INamedObject *n2 = dynamic_cast<hif::features::INamedObject *>(*jt);

        const bool isView = (dynamic_cast<View *>(*jt) != nullptr);
        bool isStdLibDef = (dynamic_cast<LibraryDef *>(*jt) != nullptr && static_cast<LibraryDef *>(*jt)->isStandard());
        if (n2 != nullptr) {
            // Named object.
            // Matches other names.
            // Special cases:
            // - Methods: can be overloaded.
            // - TypeDef: can be external.
            for (BList<Object>::iterator it = l1->begin(); it != l1->end(); ++it) {
                hif::features::INamedObject *n1 = dynamic_cast<hif::features::INamedObject *>(*it);

                if ((n1 == nullptr) || (n1->getName() != n2->getName() && !isView))
                    continue;

                isStdLibDef |=
                    (dynamic_cast<LibraryDef *>(*it) != nullptr && static_cast<LibraryDef *>(*it)->isStandard());

                if (isStdLibDef) {
                    // Not needed to check equals and fix references
                    found = *it;
                    break;
                }

                if (!hif::equals(*it, *jt, _manageSpecialCasesOpt))
                    continue;
                found = *it;

                if (isView) {
                    auto v1          = dynamic_cast<View *>(*it);
                    auto v2          = dynamic_cast<View *>(*jt);
                    const bool isStd = v1->isStandard() && v2->isStandard();
                    v1->setStandard(isStd);
                    v2->setStandard(isStd);
                }

                // Fixing possible names which do no match, since they have
                // been automatically generated.
                _currentSource = *jt;
                found->acceptVisitor(*this);

                break;
            }
        } else {
            // Unnamed object (e.g. Action).
            // Must be equals (e.g. Assign) or a special case (e.g. Switch, IfAlt).
            for (BList<Object>::iterator it = l1->begin(); it != l1->end(); ++it) {
                const bool isManaged = _mergeBranches(found, *it, *jt);
                if (isManaged) {
                    if (found != nullptr)
                        break;
                    continue;
                }

                if (!hif::equals(*it, *jt))
                    continue;
                found = *it;
                break;
            }
        }

        if (found != nullptr) {
            // If object is a standard library def and both trees contains it
            // is not needed to merge internal objects.
            if (isStdLibDef) {
                LibraryDef *local1 = dynamic_cast<LibraryDef *>(found);
                LibraryDef *local2 = dynamic_cast<LibraryDef *>(*jt);

                if (local1->isStandard()) {
                    LibraryDef tmp;
                    local1->replace(&tmp);
                    local2->replace(local1);
                    tmp.replace(local2);
                }
                ++jt;
                continue;
            }

            // Recursive merge call.
            // Since recursive merge could invalidate second object
            /// iterator, just assure increment before continuing.
            Object *tmp2 = *jt;
            ++jt;

            // Merging pendingMerge list.
            BList<Object>::iterator pos(found);
            pos.insert_before(pendingMerge);

            merge(found, tmp2);
            continue;
        }

        // Not found: it is missing in l1: adding.
        Object *o2 = *jt;
        jt         = jt.remove();
        pendingMerge.push_back(o2);
    }

    l1->merge(pendingMerge);
}

void Bach::_fixNames(Declaration *decl)
{
    const bool isView = (dynamic_cast<View *>(decl) != nullptr);

    const Object::BLists &blists1 = decl->getBLists();
    const Object::BLists &blists2 = _currentSource->getBLists();

    Object::BLists::const_iterator i1 = blists1.begin();
    Object::BLists::const_iterator i2 = blists2.begin();
    for (; i1 != blists1.end(); ++i1, ++i2) {
        BList<Object> *blist1 = *i1;
        BList<Object> *blist2 = *i2;

        if (isView) {
            View *check = static_cast<View *>(decl);
            if (reinterpret_cast<BList<Object> *>(&check->libraries) == blist1)
                continue;

            if (reinterpret_cast<BList<Object> *>(&check->declarations) == blist1) {
                if (blist1->size() != blist2->size()) {
                    messageAssert(blist1->empty() || blist2->empty(), "Unexpected case", decl, _sem);
                    continue;
                }
            }

            if (_opt.isIpxact && reinterpret_cast<BList<Object> *>(&check->templateParameters) == blist1) {
                if (blist1->size() != blist2->size())
                    continue;
            }
        }

        messageAssert(blist1->size() == blist2->size(), "Unexpected blists", decl, _sem);

        BList<Object>::iterator j1 = blist1->begin();
        BList<Object>::iterator j2 = blist2->begin();
        for (; j1 != blist1->end(); ++j1, ++j2) {
            std::string n1 = hif::objectGetName(*j1);
            std::string n2 = hif::objectGetName(*j2);
            if (n1 == n2)
                continue;

            hif::objectSetName(*j2, n1);
            ReferencesSet &refs = (*_refMap)[static_cast<Declaration *>(*j2)];

            for (ReferencesSet::iterator k = refs.begin(); k != refs.end(); ++k) {
                hif::objectSetName(*k, n1);
            }
        }
    }

    if (isView) {
        View *view1 = static_cast<View *>(decl);
        View *view2 = static_cast<View *>(_currentSource);

        messageAssert(view1->getLanguageID() == view2->getLanguageID(), "Unexpected language ID", nullptr, nullptr);
        view1->setStandard(view1->isStandard() && view2->isStandard());

        const bool view1IsComponent = (view1->getContents() == nullptr);
        const bool view2IsComponent = (view2->getContents() == nullptr);

        View *toMerge       = nullptr;
        std::string toBeSet = nullptr;
        if (view1IsComponent && !view2IsComponent) {
            toMerge = view1;
            toBeSet = view2->getName();

        } else if ((!view1IsComponent && view2IsComponent) || (view1IsComponent && view2IsComponent)) {
            toMerge = view2;
            toBeSet = view1->getName();
        }

        if (toMerge != nullptr) {
            delete toMerge->setContents(nullptr);
            delete toMerge->setEntity(nullptr);
            toMerge->templateParameters.clear();
            toMerge->declarations.clear();
            toMerge->libraries.clear();
            toMerge->inheritances.clear();

            toMerge->setName(toBeSet);
            ReferencesSet &refs = (*_refMap)[toMerge];

            for (ReferencesSet::iterator k = refs.begin(); k != refs.end(); ++k) {
                hif::objectSetName(*k, toBeSet);
            }
        }

        // Updating references for future renames.
        // This is the only required case, since only here we rename the
        // left tree! (the resulting tree)
        ReferencesSet &refs1 = (*_refMap)[view1];
        ReferencesSet &refs2 = (*_refMap)[view2];
        refs1.insert(refs2.begin(), refs2.end());
    }
}

void Bach::_fixSymbolDeclaration(Object *symbol)
{
    Declaration *d = hif::semantics::getDeclaration(symbol, _sem);
    messageAssert(d != nullptr, "Declaration not found", symbol, _sem);

    Declaration *cd = hif::semantics::getDeclaration(_currentSource, _sem);
    messageAssert(cd != nullptr, "Declaration not found", _currentSource, _sem);

    _currentSource = cd;
    _fixNames(d);
}

bool Bach::_mergeBranches(Object *&found, Object *o1, Object *o2)
{
    if (!_opt.mergeBranches)
        return false;

    if (dynamic_cast<Switch *>(o1) != nullptr && dynamic_cast<Switch *>(o2) != nullptr) {
        Switch *m1 = static_cast<Switch *>(o1);
        Switch *m2 = static_cast<Switch *>(o2);
        if (!hif::equals(m1->getCondition(), m2->getCondition()))
            return false;
        found = o1;
        return true;
    } else if (dynamic_cast<SwitchAlt *>(o1) != nullptr && dynamic_cast<SwitchAlt *>(o2) != nullptr) {
        SwitchAlt *m1 = static_cast<SwitchAlt *>(o1);
        SwitchAlt *m2 = static_cast<SwitchAlt *>(o2);
        if (!hif::equals(m1->conditions, m2->conditions))
            return false;
        found = o1;
        return true;
    } else if (dynamic_cast<With *>(o1) != nullptr && dynamic_cast<With *>(o2) != nullptr) {
        With *m1 = static_cast<With *>(o1);
        With *m2 = static_cast<With *>(o2);
        if (!hif::equals(m1->getCondition(), m2->getCondition()))
            return false;
        found = o1;
        return true;
    } else if (dynamic_cast<WithAlt *>(o1) != nullptr && dynamic_cast<WithAlt *>(o2) != nullptr) {
        WithAlt *m1 = static_cast<WithAlt *>(o1);
        WithAlt *m2 = static_cast<WithAlt *>(o2);
        if (!hif::equals(m1->conditions, m2->conditions))
            return false;
        found = o1;
        return true;
    } else if (dynamic_cast<If *>(o1) != nullptr && dynamic_cast<If *>(o2) != nullptr) {
        If *m1 = static_cast<If *>(o1);
        If *m2 = static_cast<If *>(o2);
        if (m1->alts.size() != m2->alts.size())
            return false;
        BList<IfAlt>::iterator i = m1->alts.begin();
        BList<IfAlt>::iterator j = m2->alts.begin();
        for (; i != m1->alts.end(); ++i, ++j) {
            Object *fake = nullptr;
            if (!_mergeBranches(fake, *i, *j))
                return false;
        }
        found = o1;
        return true;
    } else if (dynamic_cast<IfAlt *>(o1) != nullptr && dynamic_cast<IfAlt *>(o2) != nullptr) {
        IfAlt *m1 = static_cast<IfAlt *>(o1);
        IfAlt *m2 = static_cast<IfAlt *>(o2);
        if (!hif::equals(m1->getCondition(), m2->getCondition()))
            return false;
        found = o1;
        return true;
    } else if (dynamic_cast<When *>(o1) != nullptr && dynamic_cast<When *>(o2) != nullptr) {
        When *m1 = static_cast<When *>(o1);
        When *m2 = static_cast<When *>(o2);
        if (m1->alts.size() != m2->alts.size())
            return false;
        BList<WhenAlt>::iterator i = m1->alts.begin();
        BList<WhenAlt>::iterator j = m2->alts.begin();
        for (; i != m1->alts.end(); ++i, ++j) {
            Object *fake = nullptr;
            if (!_mergeBranches(fake, *i, *j))
                return false;
        }
        found = o1;
        return true;
    } else if (dynamic_cast<WhenAlt *>(o1) != nullptr && dynamic_cast<WhenAlt *>(o2) != nullptr) {
        WhenAlt *m1 = static_cast<WhenAlt *>(o1);
        WhenAlt *m2 = static_cast<WhenAlt *>(o2);
        if (!hif::equals(m1->getCondition(), m2->getCondition()))
            return false;
        found = o1;
        return true;
    }

    return false;
}

#ifdef COMPOSER_PRINT_DEBUG_FILES
void _printStep(Object *tree, unsigned int stepNumber, Object *merged)
{
    std::string stepName = "MERGED_";
    std::string name     = hif::objectGetName(merged);
    if (name == nullptr)
        stepName += "unamed_object";
    else
        stepName += name;

    std::stringstream ssName;
    ssName << hif::application_utils::getApplicationName() << "_";
    if (stepNumber < 10)
        ssName << '0';
    ssName << stepNumber << "_" << stepName;

    hif::PrintHifOptions opt;
    opt.printHifStandardLibraries = true;
    hif::writeFile(ssName.str(), tree, true, opt);
    hif::writeFile(ssName.str(), tree, false, opt);
}
#endif

} // namespace

MergeTreesOptions::MergeTreesOptions()
    : printInfos(false)
    , isIpxact(false)
    , mergeBranches(false)
{
    // ntd
}

MergeTreesOptions::~MergeTreesOptions()
{
    // ntd
}

MergeTreesOptions::MergeTreesOptions(const MergeTreesOptions &o)
    : printInfos(o.printInfos)
    , isIpxact(o.isIpxact)
    , mergeBranches(o.mergeBranches)
{
    // ntd
}

MergeTreesOptions &MergeTreesOptions::operator=(const MergeTreesOptions &o)
{
    if (this == &o)
        return *this;

    printInfos    = o.printInfos;
    isIpxact      = o.isIpxact;
    mergeBranches = o.mergeBranches;

    return *this;
}

// //////////////////////////////////////
// Defines
// //////////////////////////////////////
#ifndef NDEBUG
//#define COMPOSER_PRINT_DEBUG_FILES
#endif

Object *mergeTrees(std::list<Object *> &partialTrees, semantics::ILanguageSemantics *sem, const MergeTreesOptions &opt)
{
    messageAssert(!partialTrees.empty(), "Expected at least a tree", nullptr, nullptr);

    Object *tree = partialTrees.front();
    partialTrees.pop_front();
    if (opt.printInfos)
        messageInfo(std::string("Merging ") + hif::objectGetName(tree) + "...");

#ifdef COMPOSER_PRINT_DEBUG_FILES
    unsigned int stepNumber = 0;
    _printStep(tree, stepNumber, tree);
#endif
    if (partialTrees.empty())
        return tree;
    // Instantiate Bach.
    Bach cv(sem, opt);

    // search all references
    RefMap refMap;
    hif::semantics::GetReferencesOptions options;
    //options.skipStandardDeclarations = true;
    options.includeUnreferenced = true;
    hif::semantics::getAllReferences(refMap, sem, tree, options);
    cv.setRefrenceMap(refMap);

    for (std::list<Object *>::iterator it(partialTrees.begin()); it != partialTrees.end(); ++it) {
        if (opt.printInfos)
            messageInfo(std::string("Merging ") + hif::objectGetName(*it) + "...");
        // add partial tree references
        hif::semantics::getAllReferences(refMap, sem, *it, options);
        cv.merge(tree, *it);

#ifdef COMPOSER_PRINT_DEBUG_FILES
        _printStep(tree, ++stepNumber, *it);
#endif

        (*it)->replace(nullptr);
        delete *it;
    }
    partialTrees.clear();

    // resetting declaration
    hif::semantics::resetTypes(tree);
    hif::semantics::resetDeclarations(tree);
    hif::manipulation::flushInstanceCache();
    hif::semantics::flushTypeCacheEntries();

    if (opt.isIpxact) {
        // Fixing bindings
        BindVisitor bv(sem);
        tree->acceptVisitor(bv);
    }

    return tree;
}

} // namespace manipulation
} // namespace hif
