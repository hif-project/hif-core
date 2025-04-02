/// @file findTopLevelModule.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/findTopLevelModule.hpp"

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

typedef std::map<View *, std::size_t> ScoreMap;

#ifdef NDEBUG
#    define _printCandidates(...)
#else
void _printCandidates(const ViewSet &candidates)
{
    std::stringstream ss;
    ss << "Candidates are:\n";
    for (ViewSet::const_iterator i = candidates.begin(); i != candidates.end(); ++i) {
        View *view     = *i;
        DesignUnit *du = static_cast<DesignUnit *>(view->getParent());
        ss << " - " << du->getName() << "(" << view->getName() << ")\n";
    }
    messageDebug(ss.str(), nullptr, nullptr);
}
#endif

std::size_t _getChildInstances(View *v, View *c, semantics::ILanguageSemantics *sem)
{
    std::size_t ret = 0;

    for (BList<Instance>::iterator i = v->getContents()->instances.begin(); i != v->getContents()->instances.end();
         ++i) {
        ViewReference *vr = dynamic_cast<ViewReference *>((*i)->getReferencedType());
        messageAssert(vr != nullptr, "Unexpected case", (*i), sem);

        View *decl = hif::semantics::getDeclaration(vr, sem);
        messageAssert(decl != nullptr, "Declaration not found", vr, sem);

        ret += (decl == c);
    }

    return ret;
}

void _calculateScore(
    View *v,
    ScoreMap &scoreMap,
    ViewDependenciesSet &checked,
    ViewDependenciesMap &subModulesMap,
    semantics::ILanguageSemantics *sem)
{
    if (checked.find(v) != checked.end())
        return;
    Entity *e   = v->getEntity();
    Contents *c = v->getContents();
    scoreMap[v] = 0;
    // Compute a score that aims at representing the "weight" of the view
    // (i.e., its complexity in terms of ports, declarations, processes, instances and
    // global actions)
    if (e != nullptr)
        scoreMap[v] += e->ports.size();
    if (c != nullptr) {
        scoreMap[v] += c->declarations.size();
        scoreMap[v] += c->instances.size() * 5;
        scoreMap[v] += c->stateTables.size();
        GlobalAction *g = c->getGlobalAction();
        if (g != nullptr)
            scoreMap[v] += g->actions.size();
    }
    for (ViewDependenciesSet::iterator i = subModulesMap[v].begin(); i != subModulesMap[v].end(); ++i) {
        _calculateScore(*i, scoreMap, checked, subModulesMap, sem);
        scoreMap[v] += scoreMap[*i] * _getChildInstances(v, (*i), sem);
    }
}

bool _isComponent(View *view)
{
    Contents *viewContent = view->getContents();
    if (viewContent == nullptr)
        return true;

    CopyOptions opt;
    opt.copyChildObjects    = false;
    Contents *emptyContents = hif::copy(viewContent, opt);
    bool ret          = hif::equals(viewContent, emptyContents);
    delete emptyContents;
    return ret;
}

void _getCandidatesList(ViewSet &candidatesList, hif::semantics::ILanguageSemantics *sem, FindTopOptions &opt)
{
    // Calculate view dependencies is not passed.
    messageAssert(opt.smm != nullptr && opt.pmm != nullptr, "Expected dependences maps", nullptr, sem);

    // Filling candidates list.
    for (ViewDependenciesMap::iterator i = opt.pmm->begin(); i != opt.pmm->end(); ++i) {
        View *view                   = i->first;
        ViewDependenciesSet &parents = i->second;
        // The name of the top-level design unit has not been provided by the user,
        // so try to automatically determine the top-level DesignUnit.

        // Skip collecting of view having parent modules.
        if (!parents.empty())
            continue;

        // Skip collecting of standard views or views inside standard packages.
        DeclarationIsPartOfStandardOptions posOpts;
        posOpts.dontCheckStandardViews = true;
        if (hif::declarationIsPartOfStandard(view, posOpts))
            continue;

        // Skip collecting of "components": views without contents or with empity contents.
        if (_isComponent(view))
            continue;

        candidatesList.insert(view);
    }
}

/// @brief: 0th heuristic: compute a score that determines the "weight" of each candidate
///                        design unit and choose the "heaviest" one as top.
ViewSet _mostComplexHeuristic(
    System * /*root*/,
    const ViewSet &candidatesList,
    hif::semantics::ILanguageSemantics *sem,
    FindTopOptions &opt)
{
    ScoreMap scoreMap;
    ViewDependenciesSet checked;

    ViewSet localCandidates;
    std::size_t maxScore = 0;
    for (ViewSet::const_iterator i = candidatesList.begin(); i != candidatesList.end(); ++i) {
        View *v = *i;
        _calculateScore(v, scoreMap, checked, *opt.smm, sem);
        std::size_t currentScore = scoreMap[v];
        if (currentScore > maxScore) {
            localCandidates.clear();
            maxScore = currentScore;
            localCandidates.insert(v);
        } else if (currentScore == maxScore) {
            localCandidates.insert(v);
        }
    }

    return localCandidates;
}

/// @brief: 2nd heuristic: remove all DesignUnit candidate-parents not directly
/// children of system object.
ViewSet _systemChildrenHeuristic(const ViewSet &candidatesList, System *root)
{
    ViewSet localCandidates;
    for (ViewSet::const_iterator i = candidatesList.begin(); i != candidatesList.end(); ++i) {
        View *v        = *i;
        DesignUnit *du = dynamic_cast<DesignUnit *>(v->getParent());
        if (du == nullptr)
            continue;
        if (du->getParent() != root)
            continue;

        localCandidates.insert(v);
    }

    return localCandidates;
}

View *
_findByTopLevelName(System *root, hif::semantics::ILanguageSemantics *sem, FindTopOptions &opt, ViewSet &candidatesList)
{
    Object *foundSymbol = resolveHierarchicalSymbol(opt.topLevelName, root, sem);
    if (foundSymbol != nullptr) {
        View *foundView = nullptr;
        if (dynamic_cast<DesignUnit *>(foundSymbol) != nullptr) {
            DesignUnit *du = static_cast<DesignUnit *>(foundSymbol);
            messageAssert(du->views.size() == 1, "Unexpected number of views", du, sem);
            foundView = du->views.front();
        } else {
            foundView = dynamic_cast<View *>(foundSymbol);
        }

        if (foundView != nullptr) {
            // ensuring is a top level
            messageAssert(
                candidatesList.find(foundView) != candidatesList.end(),
                "Passed top-level module is not a top-level or is only a component", foundView, sem);
            return foundView;
        }
    }

    // If symbol name is not resolved by resolveHierarchicalSymbol, it may be
    // specified w/o fully path (e.g. module inside a library def).
    ViewSet result;
    for (ViewSet::iterator i = candidatesList.begin(); i != candidatesList.end(); ++i) {
        View *view         = *i;
        DesignUnit *parent = dynamic_cast<DesignUnit *>(view->getParent());
        if (parent == nullptr) {
            messageError("Unexpected unrelated view", view, sem);
        }

        const std::string &duName(parent->getName());
        if (duName != opt.topLevelName)
            continue;

        // Found view matching required name.
        if (opt.verbose)
            messageInfo("Found top level design unit " + duName);
        result.insert(view);
    }

    if (result.empty()) {
        if (opt.checkAtLeastOne) {
            messageError("Unable to find indicated " + opt.topLevelName + " top-level design unit.", nullptr, nullptr);
        }

        return nullptr;
    } else if (result.size() > 1) {
        // always consider error if topLevelName is passed
        messageError(
            "Found more the one top-level design unit having indicated " + opt.topLevelName + " name.", nullptr,
            nullptr);
    }

    return *result.begin();
}

ViewSet _searchTopCandidates(System *root, hif::semantics::ILanguageSemantics *sem, FindTopOptions &opt)
{
    ViewSet candidatesList;
    _getCandidatesList(candidatesList, sem, opt);

    if (candidatesList.empty()) {
        if (opt.checkAtLeastOne) {
            messageError("Unable to find top-level design unit.", nullptr, nullptr);
        }

        return candidatesList;
    }

    // Recognize user-defined top-level module
    if (!opt.topLevelName.empty()) {
        ViewSet ret;
        View *found = _findByTopLevelName(root, sem, opt, candidatesList);
        if (found != nullptr)
            ret.insert(found);
        return ret;
    }

    // No top-level module name given by user

    // Check whether the top level is already determinated.
    if (candidatesList.size() == 1) {
        return candidatesList;
    }

    // More than one candidate!
    if (!opt.useHeuristics) {
        if (opt.checkAtMostOne) {
            _printCandidates(candidatesList);
            messageError("Found more the one top-level design unit.", nullptr, nullptr);
        }

        return candidatesList;
    }

    // Using of heuristics requested.
    ViewSet h1Result = _mostComplexHeuristic(root, candidatesList, sem, opt);
    if (h1Result.size() == 1) {
        // heuristic works return unique candiate
        View *found = *h1Result.begin();
        if (opt.verbose) {
            messageWarning("Assuming top level design unit is " + std::string(found->getName()), nullptr, nullptr);
        }
        return h1Result;
    }

    ViewSet h2Result = _systemChildrenHeuristic(candidatesList, root);
    if (h2Result.size() == 1) {
        // heuristic works return unique candiate
        View *found = *h1Result.begin();
        if (opt.verbose) {
            messageWarning("Assuming top level design unit is " + std::string(found->getName()), nullptr, nullptr);
        }
        return h2Result;
    }

    // Both heuristics does not resolve candidates.
    bool allMore  = h1Result.size() > 1 && h2Result.size() > 1;
    bool allEmpty = h1Result.empty() && h2Result.empty();
    if (allMore && opt.checkAtMostOne) {
        _printCandidates(candidatesList);
        messageError("Found more the one top-level design unit.", nullptr, nullptr);
    }

    if (allEmpty && opt.checkAtLeastOne) {
        messageError("Unable to find top-level design unit.", nullptr, nullptr);
    }

    // return first heuristics with any result
    if (!h1Result.empty())
        return h1Result;
    if (!h2Result.empty())
        return h2Result;

    // unreachable code
    return candidatesList;
}

} // namespace

// ///////////////////////////////////////////////////////////////////
// FindTopOptions
// ///////////////////////////////////////////////////////////////////

FindTopOptions::FindTopOptions()
    : verbose(false)
    , topLevelName()
    , smm(nullptr)
    , pmm(nullptr)
    , useHeuristics(false)
    , checkAtMostOne(false)
    , checkAtLeastOne(false)
{
    // ntd
}

FindTopOptions::~FindTopOptions()
{
    // ntd
}

FindTopOptions::FindTopOptions(const FindTopOptions &o)
    : verbose(o.verbose)
    , topLevelName(o.topLevelName)
    , smm(o.smm)
    , pmm(o.pmm)
    , useHeuristics(o.useHeuristics)
    , checkAtMostOne(o.checkAtMostOne)
    , checkAtLeastOne(o.checkAtLeastOne)
{
    // ntd
}

FindTopOptions &FindTopOptions::operator=(const FindTopOptions &o)
{
    if (this == &o)
        return *this;

    verbose         = o.verbose;
    topLevelName    = o.topLevelName;
    smm             = o.smm;
    pmm             = o.pmm;
    useHeuristics   = o.useHeuristics;
    checkAtMostOne  = o.checkAtMostOne;
    checkAtLeastOne = o.checkAtLeastOne;

    return *this;
}

// ///////////////////////////////////////////////////////////////////
// findTop methods
// ///////////////////////////////////////////////////////////////////

ViewSet findTopLevelModules(hif::System *root, semantics::ILanguageSemantics *sem, const FindTopOptions &opt)
{
    messageAssert(root != nullptr, "Passed nullptr root", nullptr, sem);
    hif::application_utils::initializeLogHeader("HIF", "findTopLevelModules");

    FindTopOptions fopt(opt);
    ViewDependenciesMap subModulesMap;
    ViewDependenciesMap parentModulesMap;
    if (fopt.pmm == nullptr || fopt.smm == nullptr) {
        findViewDependencies(root, subModulesMap, parentModulesMap, sem);
        fopt.pmm = &parentModulesMap;
        fopt.smm = &subModulesMap;
    }

    ViewSet ret = _searchTopCandidates(root, sem, fopt);
    hif::application_utils::restoreLogHeader();
    return ret;
}

View *findTopLevelModule(hif::System *root, semantics::ILanguageSemantics *sem, const FindTopOptions &opt)
{
    messageAssert(root != nullptr, "Passed nullptr root", nullptr, sem);
    hif::application_utils::initializeLogHeader("HIF", "findTopLevelModule");

    FindTopOptions fopt(opt);
    fopt.checkAtMostOne = true;
    ViewSet ret         = findTopLevelModules(root, sem, fopt);
    messageAssert(ret.size() <= 1, "Unexpected number of candidates", nullptr, nullptr);
    hif::application_utils::restoreLogHeader();
    return (ret.empty()) ? nullptr : *ret.begin();
}

} // namespace manipulation
} // namespace hif
