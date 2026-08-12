/// @file splitMixedProcesses.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>
#include <iostream>

#include "hif/hif.hpp"

#ifndef NDEBUG
//#define DEBUG_PRINT
//#define PRINT_STEP_PROCESSES
//#define DEBUG_PROCESS_GRAPH
//#define DEBUG_CONES
#endif

/* ***************************************************************** */
/* ***************************************************************** */
/* TODO LIST                                                         */
/* - Add option to keep asynchs splitted.                            */
/* - Add list to return new processes.                               */
/* - At the moment not all cases are supported: the general solution */
/*   is to collect all var assigns impacting on sigs/ports, line by  */
/*   line, and keep all asynchs splitted.                            */
/* ***************************************************************** */
/* ***************************************************************** */

namespace hif
{

namespace analysis
{

namespace /* anon */
{

const std::string SKIP_FROM_SYNCH_CONE("SKIP_FROM_SYNCH_CONE");

// /////////////////////////////////////
// Process split
// /////////////////////////////////////

using ProcessList = std::list<StateTable *>;
using SignalList  = std::list<Signal *>;
using RefSet      = std::set<Object *>;
using DeclSet     = std::set<Declaration *>;
using RefMap      = std::map<Declaration *, RefSet>;
using VarList     = std::list<Variable *>;
using DeclList    = std::list<DataDeclaration *>;
class ConeRefiner : public GuideVisitor
{
public:
    ConeRefiner(semantics::ILanguageSemantics *sem);
    ~ConeRefiner() override;

    auto visitStateTable(StateTable &o) -> int override;
    auto visitAssign(Assign &o) -> int override;
    auto visitProcedureCall(ProcedureCall &o) -> int override;

private:
    using ValueList = std::list<Value *>;

    using Map = std::map<DataDeclaration *, State *>;
    Map _paths;
    DeclList _declList;

    auto _getPath(Object *orig, Object *copy) -> State *;
    void _mergePaths(ValueList &targets, State *state, Object *origin);
    semantics::ILanguageSemantics *_sem;
    hif::CopyOptions _opt;
    hif::manipulation::MergeTreesOptions _mergeOpt;

    auto operator=(const ConeRefiner &) -> ConeRefiner = delete;
    ConeRefiner(const ConeRefiner &)                   = delete;
};

ConeRefiner::ConeRefiner(semantics::ILanguageSemantics *sem)
    : _sem(sem)

{
    _opt.copyChildObjects = false;

    _mergeOpt.mergeBranches = true;
}

ConeRefiner::~ConeRefiner()
{
    // ntd
}

auto ConeRefiner::visitStateTable(StateTable &o) -> int
{
    hif::GuideVisitor::visitStateTable(o);

    State *state = o.states.front();
    state->actions.clear();

    for (auto &i : _declList) {
        State *other = _paths[i];
        state->actions.merge(other->actions);
        delete other;
    }

    return 0;
}

auto ConeRefiner::visitAssign(Assign &o) -> int
{
    ValueList targets = hif::manipulation::collectLeftHandSideSymbols(&o);
    State *state      = _getPath(&o, hif::copy(&o));
    _mergePaths(targets, state, &o);

    return 0;
}

auto ConeRefiner::visitProcedureCall(ProcedureCall &o) -> int { messageError("Unsupported case", &o, _sem); }
void ConeRefiner::_mergePaths(ConeRefiner::ValueList &targets, State *state, Object *origin)
{
    for (auto i = targets.begin(); i != targets.end(); ++i) {
        Identifier *id = dynamic_cast<Identifier *>(hif::getTerminalPrefix(*i));
        messageAssert(id != nullptr, "Unexpected target.", *i, _sem);
        DataDeclaration *decl = hif::semantics::getDeclaration(id, _sem);
        messageAssert(decl != nullptr, "Declaration not found.", id, _sem);

        if (_paths.find(decl) == _paths.end()) {
            _paths[decl] = state;
            _declList.push_back(decl);
            continue;
        }

        State *other = _paths[decl];
        std::list<Object *> trees;
        trees.push_back(other);
        trees.push_back(state);

        // Updating decls by temporary attaching the state to parent statetable.
        // Required since mergeTrees() resets declarations.
        {
            other        = dynamic_cast<State *>(hif::manipulation::mergeTrees(trees, _sem, _mergeOpt));
            _paths[decl] = other;
            BList<State>::iterator parentList(hif::getNearestParent<State>(origin));
            parentList.insert_after(other);
            hif::semantics::updateDeclarations(other, _sem);
            BList<State>::iterator otherIt(other);
            otherIt.remove();
        }
    }
}

auto ConeRefiner::_getPath(Object *orig, Object *copy) -> State *
{
    Object *parent     = orig->getParent();
    Object *parentCopy = hif::copy(parent, _opt);

    if (dynamic_cast<IfAlt *>(parent) != nullptr) {
        auto *p  = dynamic_cast<IfAlt *>(parent);
        auto *pc = dynamic_cast<IfAlt *>(parentCopy);
        pc->setCondition(hif::copy(p->getCondition()));
    } else if (dynamic_cast<If *>(parent) != nullptr) {
        // Copy the structure:
        If *p  = dynamic_cast<If *>(parent);
        If *pc = dynamic_cast<If *>(parentCopy);
        for (BList<IfAlt>::iterator i = p->alts.begin(); i != p->alts.end(); ++i) {
            if (orig == *i) {
                continue;
            }
            auto *a = new IfAlt();
            a->setCondition(hif::copy((*i)->getCondition()));
            pc->alts.push_back(a);
        }
    } else if (dynamic_cast<SwitchAlt *>(parent) != nullptr) {
        auto *p  = dynamic_cast<SwitchAlt *>(parent);
        auto *pc = dynamic_cast<SwitchAlt *>(parentCopy);
        hif::copy(p->conditions, pc->conditions);
    } else if (dynamic_cast<Switch *>(parent) != nullptr) {
        auto *p  = dynamic_cast<Switch *>(parent);
        auto *pc = dynamic_cast<Switch *>(parentCopy);
        pc->setCondition(hif::copy(p->getCondition()));
    } else if (dynamic_cast<State *>(parent) != nullptr) {
        // ntd
    } else {
        messageError("Unexpected parent", parent, _sem);
    }

    bool ok = hif::manipulation::matchedInsert(
        copy, parentCopy, orig, parent, hif::manipulation::MatchedInsertType::TYPE_EXPAND);
#ifndef NDEBUG
    if (!ok) {
        messageDebug("Copy src", copy, _sem);
        messageDebug("Copy tree", parentCopy, _sem);
        messageDebug("Orig src", orig, _sem);
        messageDebug("Orig tree", parent, _sem);
    }
#endif
    messageAssert(ok, "Matched insert failed.", orig, _sem);

    auto *state = dynamic_cast<State *>(parent);
    if (state != nullptr) {
        return dynamic_cast<State *>(parentCopy);
    }

    return _getPath(parent, parentCopy);
}
auto _checkIntersections(
    ProcessInfos::ReferredDeclarations &writerInfos,
    ProcessInfos::ReferredDeclarations &readerInfos) -> bool
{
    for (auto *readerInfo : readerInfos) {
        auto found = writerInfos.find(readerInfo);
        if (found != writerInfos.end()) {
            return true;
        }
    }

    return false;
}

auto _requiresMergeOfCone(
    ProcessInfos::ReferredDeclarations &vars,
    BList<Action> &where,
    semantics::ILanguageSemantics *sem) -> bool
{
    hif::semantics::ReferencesSet refs;
    for (auto *var : vars) {
        hif::semantics::getReferences(var, refs, sem, where);
        if (refs.empty()) {
            continue;
        }
        for (auto *ref : refs) {
            if (hif::manipulation::isInLeftHandSide(ref)) {
                continue;
            }
            return true;
        }
    }
    return false;
}

void _mergeConeBefore(BList<Action> &from, BList<Action> &where)
{
    BList<Action> tmp;
    hif::copy(from, tmp);
    tmp.merge(where);
    where.merge(tmp);
}

void _mergeSensitivity(BList<Value> &tgt, BList<Value> &src)
{
    for (BList<Value>::iterator i = src.begin(); i != src.end();) {
        bool found = false;
        for (BList<Value>::iterator j = tgt.begin(); j != tgt.end(); ++j) {
            if (hif::equals(*i, *j)) {
                found = true;
                break;
            }
        }
        if (found) {
            ++i;
            continue;
        }
        Value *v = *i;
        i        = i.remove();
        tgt.push_back(v);
    }
    src.clear();
}
#ifdef DEBUG_PRINT
void _printInfos(ProcessList &proc, AnalyzeProcessOptions::ProcessMap &map)
{
    static int phase = 0;
    static int roll  = 0;
    if (phase % 6 == 0) {
        phase = 0;
        ++roll;
    }
    std::clog << "--------------------------------------------------\n";
    std::clog << "Phase: " << roll << "." << phase << std::endl;
    std::clog << "--------------------------------------------------\n";
    ++phase;
    for (ProcessList::iterator i = proc.begin(); i != proc.end(); ++i) {
        ProcessInfos &infos = map[*i];
        std::clog << (*i)->getName() << " " << static_cast<int>(infos.processKind) << std::endl;
#    ifdef PRINT_STEP_PROCESSES
        hif::writeFile(std::clog, *i);
        std::clog << std::endl;
#    endif
    }
    std::clog << "--------------------------------------------------\n";
}

#endif

auto _isSupportedProcess(StateTable *originalProc, semantics::ILanguageSemantics *sem) -> bool
{
    // References to global variables are not supported.
    RefMap map;
    hif::semantics::getAllReferences(map, sem, originalProc);
    for (auto &i : map) {
        if (dynamic_cast<Variable *>(i.first) != nullptr) {
            auto *local = dynamic_cast<Variable *>(i.first);
            if (local == nullptr || hif::isSubNode(local, originalProc)) {
                continue;
            }
            return false;
        }
    }

    return true;
}

void _fixSensitivityRefs(
    Declaration *decl,
    BList<Value> &newSensitivity,
    BList<Value> &sensitivityList,
    hif::semantics::ILanguageSemantics *sem)
{
    using RefList = hif::semantics::ReferencesSet;
    RefList refList;
    hif::semantics::getReferences(decl, refList, sem, sensitivityList);

    for (auto *ref : refList) {
        auto *v = dynamic_cast<Value *>(ref);
        while (dynamic_cast<Value *>(v->getParent()) != nullptr) {
            v = dynamic_cast<Value *>(v->getParent());
        }
        newSensitivity.push_back(hif::copy(v));
    }
}
void _split_refineLogicCones(StateTable *st, semantics::ILanguageSemantics *sem)
{
    ConeRefiner cv(sem);
    st->acceptVisitor(cv);
}
auto _split_fixLocalDeclarations(
    StateTable *originalProcess,
    /*ProcessInfos & originalInfos,*/
    VarList &newVariables,
    semantics::ILanguageSemantics *sem) -> bool
{
    auto *bc = hif::getNearestParent<BaseContents>(originalProcess);
    messageAssert(bc != nullptr, "BaseContents not found.", originalProcess, sem);

    RefMap refMap;
    hif::semantics::getAllReferences(refMap, sem, originalProcess);

    for (BList<Declaration>::iterator i = originalProcess->declarations.begin();
         i != originalProcess->declarations.end();) {
        Declaration *d = *i;
        i              = i.remove();
        if (dynamic_cast<Variable *>(d) != nullptr) {
            auto *local = dynamic_cast<Variable *>(d);
            bc->declarations.push_back(local);
            newVariables.push_back(local);
        } else if (dynamic_cast<Const *>(d) != nullptr) {
            auto *local = dynamic_cast<Const *>(d);
            bc->declarations.push_back(local);
        } else if (dynamic_cast<TypeDef *>(d) != nullptr) {
            auto *local = dynamic_cast<Const *>(d);
            bc->declarations.push_back(local);
        } else {
            messageError("Unsupported case", d, sem);
        }
    }

    return true;
}
auto _split_createNewProcesses(
    StateTable *originalProc,
    ProcessInfos &originalInfos,
    ProcessList &newProcesses,
    hif::semantics::ILanguageSemantics *sem) -> bool
{
    messageAssert(originalProc->isInBList(), "BList required.", originalProc, sem);
    BList<Declaration>::iterator place(originalProc);

    RefMap originalRefs;
    hif::semantics::getAllReferences(originalRefs, sem, originalProc);

    hif::HifFactory factory(sem);
    int num = 0;

    for (BList<Action>::iterator i = originalProc->states.front()->actions.begin();
         i != originalProc->states.front()->actions.end();) {
        Action *a = *i;
        i         = i.remove();
        std::stringstream ss;
        ss << originalProc->getName() << "_" << num;
        ++num;
        std::string n = NameTable::getInstance()->getFreshName(ss.str());

        StateTable *st = factory.stateTable(
            n, factory.noDeclarations(), (a), originalProc->getDontInitialize(), originalProc->getFlavour());

        RefMap refMap;
        hif::semantics::getAllReferences(refMap, sem, st);
        // Collect decls of read symbols
        DeclSet declSet;
        for (auto &j : refMap) {
            bool found = false;
            for (auto k : j.second) {
                if (hif::manipulation::isInLeftHandSide(k)) {
                    continue;
                }
                found = true;
                break;
            }
            if (found) {
                declSet.insert(j.first);
            }
        }
        // Collect decls of symbols inside sensitivity
        DeclSet sensSet;
        for (auto *j : declSet) {
            if (!originalInfos.isInSensitivity(dynamic_cast<DataDeclaration *>(j))) {
                continue;
            }
            sensSet.insert(j);
        }
        // Get values (symbols) from original sensitivity:
        for (auto *j : sensSet) {
            RefSet &origRefs = originalRefs[j];
            for (auto *ref : origRefs) {
                if (hif::getNearestParent<State>(ref) != nullptr) {
                    continue;
                }
                auto *v = dynamic_cast<Value *>(ref);
                while (dynamic_cast<Value *>(v->getParent()) != nullptr) {
                    v = dynamic_cast<Value *>(v->getParent());
                }

                if (originalProc->sensitivity.contains(v)) {
                    st->sensitivity.push_back(hif::copy(v));
                } else if (originalProc->sensitivityPos.contains(v)) {
                    st->sensitivityPos.push_back(hif::copy(v));
                } else if (originalProc->sensitivityNeg.contains(v)) {
                    st->sensitivityNeg.push_back(hif::copy(v));
                } else {
                    messageError("Unexpected case.", ref, sem);
                }
            }
        }

        hif::semantics::resetDeclarations(st);
        newProcesses.push_back(st);
        place.insert_before(st);
    }

    return true;
}
auto _split_refineSynchSensitivities(
    AnalyzeProcessOptions::ProcessMap &map,
    ProcessList &newProcesses,
    hif::semantics::ILanguageSemantics *sem,
    const AnalyzeProcessOptions &opt) -> bool
{
    for (auto i = newProcesses.begin(); i != newProcesses.end(); ++i) {
        StateTable *st = *i;

        if (!hif::analysis::analyzeProcesses(st, map, sem, opt)) {
            return false;
        }
        ProcessInfos &infos = map[st];
        bool clock          = false;
        bool reset          = false;
        switch (infos.processStyle) {
        case ProcessInfos::NO_STYLE:
            // Asynch
            clock = false;
            reset = false;
            break;
        case ProcessInfos::STYLE_1:
            // Synch w/ or w/o reset
            clock = true;
            reset = true;
            break;
        case ProcessInfos::STYLE_2:
            // Synch w/o reset
            clock = true;
            reset = false;
            break;
        case ProcessInfos::STYLE_3:
            // Synch w/o reset
            clock = true;
            reset = false;
            break;
        case ProcessInfos::STYLE_4:
        case ProcessInfos::STYLE_5:
            messageError("Unexpected case.", st, sem);
        case ProcessInfos::STYLE_6:
            // Synch w/ or w/o reset
            clock = true;
            reset = true;
            break;
        default:
            messageError("Unexpected case (default).", st, sem);
        }

        if (!clock) {
            continue;
        }
        messageAssert(infos.clock != nullptr, "Clock not found", st, sem);
        BList<Value> origSens;
        origSens.merge(st->sensitivity);
        BList<Value> origSensPos;
        origSensPos.merge(st->sensitivityPos);
        BList<Value> origSensNeg;
        origSensNeg.merge(st->sensitivityNeg);

        _fixSensitivityRefs(infos.clock, st->sensitivity, origSens, sem);
        _fixSensitivityRefs(infos.clock, st->sensitivityPos, origSensPos, sem);
        _fixSensitivityRefs(infos.clock, st->sensitivityNeg, origSensNeg, sem);

        if (reset) {
            messageAssert(infos.reset != nullptr, "Reset not found", st, sem);

            _fixSensitivityRefs(infos.reset, st->sensitivity, origSens, sem);
            _fixSensitivityRefs(infos.reset, st->sensitivityPos, origSensPos, sem);
            _fixSensitivityRefs(infos.reset, st->sensitivityNeg, origSensNeg, sem);
        }

        map[st] = ProcessInfos();
        if (!hif::analysis::analyzeProcesses(st, map, sem, opt)) {
            return false;
        }

        bool hasAsynchResetVars = false;
        if (!map[st].outputVariables.empty()) {
            switch (map[st].resetKind) {
            case ProcessInfos::NO_RESET:
            case ProcessInfos::SYNCHRONOUS_RESET:
            case ProcessInfos::DERIVED_SYNCHRONOUS_RESET:
                break;
            case ProcessInfos::ASYNCHRONOUS_RESET:
                hasAsynchResetVars = true;
                break;
            default:
                messageError("Unexpected reset kind.", nullptr, nullptr);
            }
        }

        if (hasAsynchResetVars) {
            // In this case, the written output vars must be put also
            // into asynch cones!
            StateTable *stNew = hif::copy(st);
            If *ifStm         = dynamic_cast<If *>(stNew->states.front()->actions.front());
            messageAssert(ifStm != nullptr, "Unexpected object.", stNew->states.front()->actions.front(), sem);
            ifStm->defaults.clear();
            IfAlt *ifa = ifStm->alts.front();
            BList<IfAlt>::iterator rm(ifa);
            rm.remove();
            ifStm->alts.clear();
            ifStm->alts.push_back(ifa);
            stNew->setName(NameTable::getInstance()->getFreshName((st->getName() + "_asynch")));
            stNew->sensitivity.clear();
            stNew->sensitivityPos.clear();
            stNew->sensitivityNeg.clear();
            _fixSensitivityRefs(infos.reset, stNew->sensitivity, st->sensitivity, sem);
            _fixSensitivityRefs(infos.reset, stNew->sensitivityPos, st->sensitivityPos, sem);
            _fixSensitivityRefs(infos.reset, stNew->sensitivityNeg, st->sensitivityNeg, sem);
            BList<StateTable>::iterator pos(st);
            stNew->addProperty(SKIP_FROM_SYNCH_CONE);
            pos.insert_before(stNew);
            newProcesses.insert(i, stNew);
            if (!hif::analysis::analyzeProcesses(stNew, map, sem, opt)) {
                return false;
            }
        }
    }

    // Now sorting all processes according with their vars
    using StTypes = Types<StateTable, StateTable>;
    using StGraph = StTypes::Graph;
    StGraph graph;
    for (auto i = newProcesses.begin(); i != newProcesses.end(); ++i) {
        // Always inserting into the graph
        graph.first[*i];
        graph.second[*i];
        //ProcessList::iterator j = newProcesses.begin();
        auto j = i;
        ++j;
        for (; j != newProcesses.end(); ++j) {
            if (_checkIntersections(map[*i].outputVariables, map[*j].inputVariables)) {
#ifdef DEBUG_PROCESS_GRAPH
                std::clog << (*i)->getName() << " --> " << (*j)->getName() << std::endl;
#endif
                graph.second[*i].insert(*j);
                graph.first[*j].insert(*i);
            } else if (_checkIntersections(map[*j].outputVariables, map[*i].inputVariables)) {
#ifdef DEBUG_PROCESS_GRAPH
                std::clog << (*j)->getName() << " --> " << (*i)->getName() << std::endl;
#endif
                graph.second[*j].insert(*i);
                graph.first[*i].insert(*j);
            }
        }
    }

    for (auto i = newProcesses.begin(); i != newProcesses.end(); ++i) {
        auto j = i;
        ++j;
        for (; j != newProcesses.end(); ++j) {
            if (_checkIntersections(map[*i].outputVariables, map[*j].outputVariables)) {
#ifdef DEBUG_PROCESS_GRAPH
                std::clog << (*i)->getName() << " --> " << (*j)->getName() << std::endl;
#endif
                graph.second[*i].insert(*j);
                graph.first[*j].insert(*i);
            } else if (_checkIntersections(map[*i].inputVariables, map[*j].inputVariables)) {
#ifdef DEBUG_PROCESS_GRAPH
                std::clog << (*i)->getName() << " --> " << (*j)->getName() << std::endl;
#endif
                graph.second[*i].insert(*j);
                graph.first[*j].insert(*i);
            }
        }
    }

    ProcessList sortResult;
    sortGraph<StateTable, StateTable>(graph, sortResult, false, &newProcesses);
    messageAssert(sortResult.size() == newProcesses.size(), "Wrong sorting.", nullptr, nullptr);
    newProcesses.clear();
    newProcesses = sortResult;
    return true;
}
auto _split_mergeSynchs(
    AnalyzeProcessOptions::ProcessMap &map,
    ProcessList &newProcesses,
    hif::semantics::ILanguageSemantics *sem,
    const AnalyzeProcessOptions &opt) -> bool
{
    ProcessInfos *prevInfos = nullptr;
    StateTable *prev        = nullptr;
    ProcessList trash;
    for (auto i = newProcesses.rbegin(); i != newProcesses.rend(); ++i) {
        StateTable *st      = *i;
        ProcessInfos &info1 = map[st];

        if (info1.processKind == ProcessInfos::ASYNCHRONOUS || info1.processKind == ProcessInfos::MIXED ||
            info1.processKind == ProcessInfos::DERIVED_MIXED) {
            continue;
        }

        if (prev == nullptr || prevInfos->processKind != info1.processKind || prevInfos->clock != info1.clock ||
            prevInfos->reset != info1.reset) {
            prev      = st;
            prevInfos = &info1;
            continue;
        }

        // Same clock and kind: merging.
        messageAssert(
            prevInfos->sensitivity == info1.sensitivity && prevInfos->risingSensitivity == info1.risingSensitivity &&
                prevInfos->fallingSensitivity == info1.fallingSensitivity,
            "Unexpected sensitivity.", nullptr, nullptr);

        trash.push_back(*i);

        std::list<Object *> trees;
        trees.push_back(st->states.front());
        trees.push_back(prev->states.front());
        hif::manipulation::MergeTreesOptions mergeOpt;
        mergeOpt.mergeBranches = true;
        auto *res              = dynamic_cast<State *>(hif::manipulation::mergeTrees(trees, sem, mergeOpt));
        messageAssert(res != nullptr, "Error during merging.", nullptr, nullptr);
        prev->setName(st->getName());
        res->replace(nullptr);
        prev->states.clear();
        prev->states.push_back(res);

        // Updating infos:
        map[prev] = ProcessInfos();
        if (!hif::analysis::analyzeProcesses(prev, map, sem, opt)) {
            return false;
        }
    }

    for (auto &i : trash) {
        map.erase(i);
        newProcesses.erase(std::find(newProcesses.begin(), newProcesses.end(), i));
        i->replace(nullptr);
        delete i;
    }

    return true;
}

// Phase 5: merging synchs with impacting logic cones.
auto _split_mergeSynchLogicCones(
    AnalyzeProcessOptions::ProcessMap &map,
    ProcessList &newProcesses,
    VarList &newVariables,
    hif::semantics::ILanguageSemantics *sem,
    const AnalyzeProcessOptions &opt) -> bool
{
    DeclSet newSignalSet;
    for (auto &newVariable : newVariables) {
        newSignalSet.insert(newVariable);
    }

    for (auto i = newProcesses.begin(); i != newProcesses.end(); ++i) {
        ProcessInfos &info1 = map[*i];

        if (info1.processKind != ProcessInfos::SYNCHRONOUS && info1.processKind != ProcessInfos::DERIVED_SYNCHRONOUS) {
            continue;
        }

        if ((*i)->checkProperty(SKIP_FROM_SYNCH_CONE)) {
            continue;
        }

        if (info1.inputVariables.empty()) {
            continue;
        }

        ProcessList::reverse_iterator j(i);
        for (; j != newProcesses.rend(); ++j) {
            ProcessInfos &otherInfos = map[*j];
            if (otherInfos.outputVariables.empty()) {
                continue;
            }

            if ((*j)->checkProperty(SKIP_FROM_SYNCH_CONE)) {
                continue;
            }

            bool hasIntersections = _checkIntersections(otherInfos.outputVariables, info1.inputVariables);
            if (!hasIntersections) {
                continue;
            }

            // Have to merge:
#ifdef DEBUG_CONES
            messageDebug((std::string("Merging: ") + (*i)->getName() + " += " + (*j)->getName()), nullptr, nullptr);
            for (DeclVect::iterator k = set.begin(); k != set.end(); ++k) {
                std::clog << "var " << (*k)->getName() << std::endl;
            }
#endif

            StateTable *st = *j;
            // Keep original synch sensitivity. Hence, no sensitivity merge.
            switch (info1.processStyle) {
            case ProcessInfos::NO_STYLE:
                messageError("Unexpected case.", nullptr, nullptr);
            case ProcessInfos::STYLE_1: {
                // Synch w/ reset
                If *ifStm       = nullptr;
                ifStm           = dynamic_cast<If *>((*i)->states.front()->actions.front());
                IfAlt *resetAlt = ifStm->alts.front();
                IfAlt *clockAlt = ifStm->alts.back();
                if (_requiresMergeOfCone(otherInfos.outputVariables, resetAlt->actions, sem)) {
                    _mergeConeBefore(st->states.front()->actions, resetAlt->actions);
                }
                if (_requiresMergeOfCone(otherInfos.outputVariables, clockAlt->actions, sem)) {
                    _mergeConeBefore(st->states.front()->actions, clockAlt->actions);
                }
            } break;
            case ProcessInfos::STYLE_2: {
                // Synch w/ or w/o reset
                If *ifStm                = nullptr;
                ifStm                    = dynamic_cast<If *>((*i)->states.front()->actions.front());
                BList<Action> *clockActs = nullptr;
                BList<Action> *resetActs = nullptr;
                if (info1.reset != nullptr) {
                    ifStm     = dynamic_cast<If *>(ifStm->alts.front()->actions.front());
                    resetActs = &ifStm->alts.front()->actions;
                    clockActs = &ifStm->defaults;
                } else {
                    IfAlt *alt = ifStm->alts.back();
                    clockActs  = &alt->actions;
                }

                if (resetActs != nullptr && _requiresMergeOfCone(otherInfos.outputVariables, *resetActs, sem)) {
                    _mergeConeBefore(st->states.front()->actions, *resetActs);
                }

                if (_requiresMergeOfCone(otherInfos.outputVariables, *clockActs, sem)) {
                    _mergeConeBefore(st->states.front()->actions, *clockActs);
                }
            } break;
            case ProcessInfos::STYLE_3:
                // Synch w/o reset
                {
                    _mergeConeBefore(st->states.front()->actions, (*i)->states.front()->actions);
                }
                break;
            case ProcessInfos::STYLE_4:
            case ProcessInfos::STYLE_5:
                messageError("Unexpected case.", st, sem);
            case ProcessInfos::STYLE_6: {
                // Synch w/ or w/o reset
                If *ifStm = nullptr;
                if (!(*i)->states.front()->actions.empty()) {
                    ifStm = dynamic_cast<If *>((*i)->states.front()->actions.front());
                }
                BList<Action> *clockActs = nullptr;
                BList<Action> *resetActs = nullptr;
                if (ifStm != nullptr) {
                    resetActs = &ifStm->alts.front()->actions;
                    clockActs = &ifStm->defaults;
                } else {
                    clockActs = &(*i)->states.front()->actions;
                }
                if (resetActs != nullptr && _requiresMergeOfCone(otherInfos.outputVariables, *resetActs, sem)) {
                    _mergeConeBefore(st->states.front()->actions, *resetActs);
                }

                if (_requiresMergeOfCone(otherInfos.outputVariables, *clockActs, sem)) {
                    _mergeConeBefore(st->states.front()->actions, *clockActs);
                }
            } break;
            default:
                messageError("Unexpected case (default).", st, sem);
            }

            // Updating infos:
            map[*i] = ProcessInfos();
            if (!hif::analysis::analyzeProcesses(*i, map, sem, opt)) {
                return false;
            }
        }
    }

    return true;
}
auto _splitPhase_mergeAsynchs(
    AnalyzeProcessOptions::ProcessMap &map,
    ProcessList &newProcesses,
    hif::semantics::ILanguageSemantics *sem,
    const AnalyzeProcessOptions &opt) -> bool
{
    StateTable *first = nullptr;
    ProcessList trash;
    for (auto i = newProcesses.rbegin(); i != newProcesses.rend(); ++i) {
        // No more useful property.
        (*i)->removeProperty(SKIP_FROM_SYNCH_CONE);
        ProcessInfos &info1 = map[*i];

        if (info1.processKind != ProcessInfos::ASYNCHRONOUS && info1.processKind != ProcessInfos::MIXED &&
            info1.processKind != ProcessInfos::DERIVED_MIXED) {
            continue;
        }

        StateTable *st = *i;
        if (first == nullptr) {
            // Do not set as reference a useless terminal logic cone.
            if (info1.outputVariables.empty()) {
                first = st;
            } else {
                // Remove logic cone:
                trash.push_back(*i);
            }
            continue;
        }

        // Merging process:
        if (!info1.outputVariables.empty() && !_checkIntersections(info1.outputVariables, map[first].inputVariables)) {
            // Useless logic cone.
            trash.push_back(*i);
            continue;
        }

        _mergeSensitivity(st->sensitivity, first->sensitivity);
        _mergeSensitivity(st->sensitivityPos, first->sensitivityPos);
        _mergeSensitivity(st->sensitivityNeg, first->sensitivityNeg);
        st->states.front()->actions.merge(first->states.front()->actions);

        first->setName(st->getName());
        first->sensitivity.merge(st->sensitivity);
        first->sensitivityPos.merge(st->sensitivityPos);
        first->sensitivityNeg.merge(st->sensitivityNeg);
        first->states.front()->actions.merge(st->states.front()->actions);

        // Updating infos:
        map[first] = ProcessInfos();
        if (!hif::analysis::analyzeProcesses(first, map, sem, opt)) {
            return false;
        }
        trash.push_back(*i);
    }

    for (auto &i : trash) {
        map.erase(i);
        newProcesses.erase(std::find(newProcesses.begin(), newProcesses.end(), i));
        i->replace(nullptr);
        delete i;
    }

    return true;
}
auto _split_refineVariables(
    AnalyzeProcessOptions::ProcessMap &map,
    ProcessList &newProcesses,
    VarList &newVariables,
    hif::semantics::ILanguageSemantics *sem,
    const AnalyzeProcessOptions &opt) -> bool
{
    DeclSet readSet;
    DeclSet writeSet;
    DeclSet readWriteSet;

    for (auto *st : newProcesses) {
        ProcessInfos &infos = map[st];

        for (auto *v : newVariables) {
            bool isRead  = infos.inputVariables.find(v) != infos.inputVariables.end();
            bool isWrite = infos.outputVariables.find(v) != infos.outputVariables.end();
            if (isRead && !isWrite) {
                readSet.insert(v);
            } else if (!isRead && isWrite) {
                writeSet.insert(v);
            } else if (isRead && isWrite) {
                readWriteSet.insert(v);
                st->declarations.push_back(hif::copy(v));
            }
            // else if (!isRead && !isWrite) continue
        }
    }

    // Deleting vars.
    for (auto *v : newVariables) {
        bool isRead      = readSet.find(v) != readSet.end();
        bool isWrite     = writeSet.find(v) != writeSet.end();
        bool isReadWrite = readWriteSet.find(v) != readWriteSet.end();
        messageAssert(!isReadWrite || (!isRead && !isWrite), "Unexpected case.", v, sem);

        // Actually, since previous steps split and copy logic cones,
        // vars should be always of type isReadWrite.
        // Unfortunatly, there is the case that a low-quality design
        // has variables written by a synch process and read by asynchs,
        // therefore we cannot enable this assert().
        // messageAssert(isReadWrite, "Unexpected variable type.", v, sem);

        if (!isRead && !isWrite) {
            v->replace(nullptr);
            delete v;
        } else {
            auto *s = new Signal();
            s->setName(NameTable::getInstance()->getFreshName((v->getName() + std::string("_moved")).c_str()));

            // Refine to signal:
            hif::semantics::ReferencesSet refs;
            for (auto j = newProcesses.begin(); j != newProcesses.end(); ++j) {
                hif::semantics::getReferences(v, refs, sem);
            }
            for (auto *ref : refs) {
                hif::objectSetName(ref, s->getName());
            }

            s->setValue(v->setValue(nullptr));
            s->setType(v->setType(nullptr));
            v->replace(s);
            delete v;
        }
    }

    for (auto *st : newProcesses) {
        hif::semantics::resetDeclarations(st);
        map[st] = ProcessInfos();
        if (!hif::analysis::analyzeProcesses(st, map, sem, opt)) {
            return false;
        }
    }

    return true;
}

} // namespace

auto splitMixedProcesses(
    AnalyzeProcessOptions::ProcessMap &map,
    semantics::ILanguageSemantics *sem,
    const AnalyzeProcessOptions &opt) -> bool
{
    hif::application_utils::initializeLogHeader("HIF", "splitMixedProcesses");

    bool ret = true;
    for (auto i = map.begin(); i != map.end();) {
        if (i->second.processKind != ProcessInfos::MIXED && i->second.processKind != ProcessInfos::DERIVED_MIXED) {
            ++i;
            continue;
        }

        auto *originalProc = dynamic_cast<StateTable *>(i->first);
        messageAssert(originalProc != nullptr, "Unsupported process kind", i->first, sem);
        messageAssert(originalProc->isInBList(), "Parent required but not found.", originalProc, sem);
        hif::semantics::updateDeclarations(originalProc, sem);
        if (originalProc->states.empty()) {
            ++i;
            continue;
        }

        if (!_isSupportedProcess(originalProc, sem)) {
            if (opt.printWarnings) {
                messageWarning(
                    "Cannot split process " + hif::manipulation::buildHierarchicalSymbol(originalProc), originalProc,
                    sem);
            }
            ++i;
            continue;
        }

        _split_refineLogicCones(originalProc, sem);

        VarList newVariables;
        _split_fixLocalDeclarations(originalProc, newVariables, sem);

        ProcessList newProcesses;
        _split_createNewProcesses(originalProc, i->second, newProcesses, sem);

#ifdef DEBUG_PRINT
        // 0
        _printInfos(newProcesses, map);
#endif

        // Also classifies all processes
        _split_refineSynchSensitivities(map, newProcesses, sem, opt);

#ifdef DEBUG_PRINT
        // 1
        _printInfos(newProcesses, map);
#endif

        _split_mergeSynchs(map, newProcesses, sem, opt);

#ifdef DEBUG_PRINT
        // 2
        _printInfos(newProcesses, map);
#endif

        _split_mergeSynchLogicCones(map, newProcesses, newVariables, sem, opt);

#ifdef DEBUG_PRINT
        // 3
        _printInfos(newProcesses, map);
#endif

        _splitPhase_mergeAsynchs(map, newProcesses, sem, opt);

#ifdef DEBUG_PRINT
        // 4
        _printInfos(newProcesses, map);
#endif

        _split_refineVariables(map, newProcesses, newVariables, sem, opt);

#ifdef DEBUG_PRINT
        // 5
        _printInfos(newProcesses, map);
#endif
        originalProc->replace(nullptr);
        delete originalProc;

        map.erase(i++);
    }
    hif::application_utils::restoreLogHeader();
    return ret;
}

} // namespace analysis

} // namespace hif
