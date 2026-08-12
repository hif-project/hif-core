/// @file callingProcessesUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/callingProcessesUtils.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/search.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

void _findCallingProcesses(
    SubProgram *sub,
    ReferencesMap &refMap,
    CallerSet &callerSet,
    ILanguageSemantics *sem,
    Object *root,
    CallerSet &checkSet)
{
    typedef std::list<Object *> CallerList;
    CallerList tmpList;
    ReferencesSet &refSet = refMap[sub];

    // Collecting all interesting scopes:
    for (ReferencesSet::iterator i = refSet.begin(); i != refSet.end(); ++i) {
        ReferencesSet::value_type ref = *i;

        // may be inside global action, process or method.
        Assign *ass    = hif::getNearestParent<Assign>(ref);
        StateTable *st = hif::getNearestParent<StateTable>(ref);
        SubProgram *sp = hif::getNearestParent<SubProgram>(ref);

        if (sp != nullptr) {
            // Used to collect also default parameter values which are
            // fcalls.
            if (root == nullptr || hif::isSubNode(sp, root))
                tmpList.push_back(sp);
        } else if (st != nullptr) {
            // inside a process
            // Collect only if is subnode of root
            if (root == nullptr || hif::isSubNode(st, root))
                tmpList.push_back(st);
        } else if (ass != nullptr) {
            // global action
            GlobalAction *g = dynamic_cast<GlobalAction *>(ass->getParent());
            messageAssert(g != nullptr, "Unexpected case", ass, sem);
            if (root == nullptr || hif::isSubNode(ass, root))
                tmpList.push_back(ass);
        }
    }

    // For all collected scopes, if are methods, recall to get processes/globacts.
    for (CallerList::iterator i = tmpList.begin(); i != tmpList.end(); ++i) {
        Object *caller         = *i;
        SubProgram *currentSub = dynamic_cast<SubProgram *>(caller);
        if (currentSub == nullptr) {
            // recoursion ended, push!
            callerSet.insert(caller);
        } else {
            if (checkSet.find(currentSub) != checkSet.end())
                continue;
            // recall funciton
            checkSet.insert(currentSub);
            _findCallingProcesses(currentSub, refMap, callerSet, sem, root, checkSet);
        }
    }
}

} // namespace

void findCallingProcesses(
    SubProgram *sub,
    ReferencesMap &refMap,
    CallerSet &callerSet,
    ILanguageSemantics *sem,
    Object *root)
{
    CallerSet checkSet;

    _findCallingProcesses(sub, refMap, callerSet, sem, root, checkSet);
}

void findAllCallingProcesses(Object *root, ReferencesMap &refMap, CallerMap &callerMap, ILanguageSemantics *sem)
{
    if (root == nullptr)
        return;

    System *sys        = dynamic_cast<System *>(root);
    Object *passedRoot = (sys != nullptr) ? nullptr : root;
    typedef std::list<Object *> QueryResult;

    hif::HifTypedQuery<FunctionCall> q1;
    hif::HifTypedQuery<ProcedureCall> q2;
    QueryResult res;
    hif::search(res, root, q1);
    hif::search(res, root, q2);

    for (QueryResult::iterator i = res.begin(); i != res.end(); ++i) {
        Object *symbol   = *i;
        SubProgram *decl = dynamic_cast<SubProgram *>(hif::semantics::getDeclaration(symbol, sem));
        messageAssert(decl != nullptr, "Declaration not found", symbol, sem);

        if (callerMap.find(decl) != callerMap.end())
            continue;

        findCallingProcesses(decl, refMap, callerMap[decl], sem, passedRoot);
    }
}

} // namespace semantics
} // namespace hif
