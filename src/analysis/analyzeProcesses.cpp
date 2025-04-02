/// @file analyzeProcesses.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <utility>

#include "hif/hif.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/semantics/declarationUtils.hpp"

namespace hif
{

namespace analysis
{

namespace /* anon */
{
// /////////////////////////////////////
// Process analysis
// /////////////////////////////////////

auto _getOtherSignal(ProcessInfos &infos, DataDeclaration *other) -> DataDeclaration *
{
    messageDebugAssert(other != nullptr, "Expected other signal.", nullptr, nullptr);

    for (auto *i : infos.sensitivity) {
        if (i == other) {
            continue;
        }
        return i;
    }
    for (auto *i : infos.risingSensitivity) {
        if (i == other) {
            continue;
        }
        return i;
    }
    for (auto *i : infos.fallingSensitivity) {
        if (i == other) {
            continue;
        }
        return i;
    }
    return nullptr;
}
class ProcessVisitor : public GuideVisitor
{
public:
    using InfoMap = std::map<Object *, ProcessInfos>;
    using ObjList = std::list<Object *>;

    ProcessVisitor(InfoMap &map, semantics::ILanguageSemantics *sem, const AnalyzeProcessOptions &opt);
    ~ProcessVisitor() override;

    ProcessVisitor(const ProcessVisitor &)                     = delete;
    auto operator=(const ProcessVisitor &) -> ProcessVisitor & = delete;

    auto visitStateTable(StateTable &o) -> int override;
    auto visitGlobalAction(GlobalAction &o) -> int override;

private:
    /// @name Entry methods for process classification, called by visiting methods.
    /// @{

    void _analyze(Assign *o);
    void _analyze(StateTable *o);

    /// @}
    /// @name Main classification methods.
    /// @{

    /// @brief Analysis of process signals (read, written and sensitivity).
    void _classifySignals(ProcessInfos &infos, Object *proc);
    /// @brief First analysis of process, according with sensitivity.
    void _classifyWrtSensitivity(ProcessInfos &infos, StateTable *proc) const;
    /// @brief Refinement of analysis, inspecting the process body.
    void _classifyProcessBody(ProcessInfos &infos, StateTable *proc);

    /// @}
public:
    /// @name Merging infos methods.
    /// @{

    /// @brief Combines the two infos into @p infos1.
    /// @p infos1 is considered as derived from sensitivity analysis.
    /// @p infos2 is considered as derived from body analysis.
    auto _mergeInfos(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool;

    auto _mergeProcessInfos(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool;
    static auto _mergeResetInfos(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool;
    auto _mergeEdgeInfos(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool;
    static auto _mergePhaseInfos(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool;
    static auto _mergeProcessStyle(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool;
    static auto _mergeSignals(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool;

    /// @}
private:
    /// @name Support methods.
    /// @{

    /// @brief Check that process body is in the form <tt>if ... else if ... </tt>.
    static auto _isIfElseIfStatement(State *state, IfAlt *&resetIf, IfAlt *&clockIf) -> bool;
    /// @brief Check that it is a clocked if, with an optional inner reset if.
    static auto _isIfOptIfElseStatement(State *state, IfAlt *&resetIf, IfAlt *&clockIf) -> bool;
    /// @brief Checks if given expression is: == 1 or ==  0.
    auto _isEqualsToZeroOrOne(Value *cond, DataDeclaration *&n, bool &isZero) -> bool;
    /// @brief Checks if given expression is: == 1 or ==  0.
    auto _isRisingFallingEdge(Value *cond, DataDeclaration *&n, bool &isZero) -> bool;
    /// @brief Analyze given condition to match the reset pattern.
    auto _checkResetCondition(ProcessInfos &infos, Value *cond) -> bool;
    /// @brief Analyze given condition to match the clock pattern.
    auto _checkClockCondition(ProcessInfos &infos, Value *cond) -> bool;

    /// @}
    /// @name Methods to classify process body,
    /// @{

    /// Process kind:
    /// if (reset == )
    /// else if (clock.event && clock == )
    ///
    auto _isProcessStyle1(ProcessInfos &infos, State *state) -> bool;
    /// Process kind:
    /// if (clock == )
    /// {
    ///     // Optional inner if:
    ///     if (reset)
    ///     {}
    ///     else
    ///     {}
    /// }
    ///
    auto _isProcessStyle2(ProcessInfos &infos, State *state) -> bool;
    /// Process kind:
    /// @(pos clock)
    /// switch (state)
    /// {
    ///     case  ...
    /// }
    ///
    auto _isProcessStyle3(ProcessInfos &infos, State *state) -> bool;
    /// Process kind: MIXED or SYNCH
    /// made by many inner sections, with one matching
    /// process style 1 or 2
    ///
    auto _isProcessStyle4(ProcessInfos &infos, State *state) -> bool;
    /// Process kind:
    /// @(pos clock) a <= b;
    ///
    static auto _isProcessStyle5(ProcessInfos &infos, State *state) -> bool;
    /// Process kind:
    /// @(pos clock)
    ///   if (reset)
    ///   ...
    ///   else
    ///   ...
    ///
    auto _isProcessStyle6(ProcessInfos &infos, State *state) -> bool;
    /// @}
    InfoMap &_map;
    semantics::ILanguageSemantics *_sem;
    const AnalyzeProcessOptions &_opt;
};

ProcessVisitor::ProcessVisitor(InfoMap &map, semantics::ILanguageSemantics *sem, const AnalyzeProcessOptions &opt)
    : _map(map)
    , _sem(sem)
    , _opt(opt)
{
    // ntd
}
ProcessVisitor::~ProcessVisitor()
{
    // ntd
}

auto ProcessVisitor::visitStateTable(StateTable &o) -> int
{
    // GuideVisitor::visitStateTable(o); // not required
    if (!hif::objectIsProcess(&o)) {
        return 0;
    }
    if (o.getFlavour() == hif::pf_initial) {
        return 0;
    }
    // Sanity check for the moment:
    {
        hif::HifTypedQuery<Wait> q;
        std::list<Object *> result;
        hif::search(result, &o, q);
        messageAssert(result.empty(), "Processes with wait statements are not supported at the moment.", &o, _sem);
    }
    _analyze(&o);

    return 0;
}

auto ProcessVisitor::visitGlobalAction(GlobalAction &o) -> int
{
    // GuideVisitor::visitGlobalAction(o); // not required
    for (BList<Action>::iterator i = o.actions.begin(); i != o.actions.end(); ++i) {
        auto *a = dynamic_cast<Assign *>(*i);
        // unknown/unexpected case
        messageAssert(a != nullptr, "Unexpected object inside global action.", *i, _sem);
        _analyze(a);
    }
    return 0;
}

void ProcessVisitor::_analyze(Assign *o)
{
    ProcessInfos &infos = _map[o];
    infos.processKind   = ProcessInfos::ASYNCHRONOUS;
    infos.resetKind     = ProcessInfos::NO_RESET;
    infos.workingEdge   = ProcessInfos::NO_EDGE;
    infos.clock         = nullptr;
    infos.reset         = nullptr;

    _classifySignals(infos, o);

    // For globacts, all read signals/ports are sensitivity:
    infos.sensitivity.insert(infos.inputs.begin(), infos.inputs.end());
}

void ProcessVisitor::_analyze(StateTable *o)
{
    ProcessInfos &infos = _map[o];
    _classifySignals(infos, o);
    infos.processKind = ProcessInfos::ASYNCHRONOUS;
    infos.resetKind   = ProcessInfos::NO_RESET;
    infos.workingEdge = ProcessInfos::NO_EDGE;
    infos.clock       = nullptr;
    infos.reset       = nullptr;

    _classifyWrtSensitivity(infos, o);
    _classifyProcessBody(infos, o);
}

void ProcessVisitor::_classifySignals(ProcessInfos &infos, Object *proc)
{
    ObjList list;
    hif::semantics::collectSymbols(list, proc, _sem, _opt.skip_standard_declarations);

    for (auto *o : list) {
        if (dynamic_cast<hif::features::ISymbol *>(o) == nullptr) {
            // error?
            messageError("Unexpected symbol.", o, _sem);
        }
        auto *inst   = dynamic_cast<Instance *>(o);
        Library *lib = nullptr;
        if (inst != nullptr) {
            lib = dynamic_cast<Library *>(inst->getReferencedType());
        }
        Declaration *decl = hif::semantics::getDeclaration(o, _sem);
        messageAssert(decl != nullptr || (inst != nullptr && lib != nullptr), "Declaration not found.", o, _sem);
        auto *s   = dynamic_cast<Signal *>(decl);
        Port *p   = dynamic_cast<Port *>(decl);
        auto *var = dynamic_cast<Variable *>(decl);
        if (s == nullptr && p == nullptr && var == nullptr) {
            continue;
        }
        DataDeclaration *d = s;
        if (d == nullptr) {
            d = p;
        }
        if (d == nullptr) {
            d = var;
        }
        if (hif::objectIsInSensitivityList(o)) {
            messageAssert(var == nullptr, "Unexpected case (1)", o, _sem);
            auto *st = hif::getNearestParent<StateTable>(o);
            messageAssert(st != nullptr, "Unexpected case (2).", o, _sem);

            if (hif::isSubNode(o, st->sensitivity)) {
                infos.sensitivity.insert(d);
            } else if (hif::isSubNode(o, st->sensitivityPos)) {
                infos.risingSensitivity.insert(d);
            } else if (hif::isSubNode(o, st->sensitivityNeg)) {
                infos.fallingSensitivity.insert(d);
            } else {
                messageError("Unexpected case (3).", o, _sem);
            }
        } else if (hif::manipulation::isInLeftHandSide(o)) {
            if (var != nullptr) {
                infos.outputVariables.insert(d);
            } else {
                infos.outputs.insert(d);
            }
        } else {
            if (var != nullptr) {
                infos.inputVariables.insert(d);
            } else {
                infos.inputs.insert(d);
            }
        }
    }
}

void ProcessVisitor::_classifyWrtSensitivity(ProcessInfos &infos, StateTable * /*proc*/) const
{
    infos.processKind = ProcessInfos::ASYNCHRONOUS;
    infos.resetKind   = ProcessInfos::NO_RESET;
    infos.workingEdge = ProcessInfos::NO_EDGE;
    infos.resetPhase  = ProcessInfos::NO_PHASE;

    // Optimization:
    if (_opt.clock.empty() && _opt.reset.empty()) {
        return;
    }
    bool clockPos             = false;
    bool clockNeg             = false;
    bool resetPos             = false;
    bool resetNeg             = false;
    unsigned asynch           = 0;
    unsigned asynchPos        = 0;
    unsigned asynchNeg        = 0;
    std::string asynchName    = nullptr;
    std::string asynchPosName = nullptr;
    std::string asynchNegName = nullptr;

    for (auto i = infos.sensitivity.begin(); i != infos.sensitivity.end(); ++i) {
        std::string n = (*i)->getName();
        if (n == _opt.clock) {
            infos.clock = *i;
            clockPos    = true;
            clockNeg    = true;
        } else if (n == _opt.reset) {
            infos.reset = *i;
            resetPos    = true;
            resetNeg    = true;
        } else {
            ++asynch;
            asynchName = n;
        }
    }
    for (auto i = infos.risingSensitivity.begin(); i != infos.risingSensitivity.end(); ++i) {
        std::string n = (*i)->getName();
        if (n == _opt.clock) {
            infos.clock = *i;
            clockPos    = true;
        } else if (n == _opt.reset) {
            infos.reset = *i;
            resetPos    = true;
        } else {
            ++asynchPos;
            asynchPosName = n;
        }
    }
    for (auto i = infos.fallingSensitivity.begin(); i != infos.fallingSensitivity.end(); ++i) {
        std::string n = (*i)->getName();
        if (n == _opt.clock) {
            infos.clock = *i;
            clockNeg    = true;
        } else if (n == _opt.reset) {
            infos.reset = *i;
            resetNeg    = true;
        } else {
            ++asynchNeg;
            asynchNegName = n;
        }
    }
    if (clockPos && clockNeg) {
        infos.processKind = ProcessInfos::SYNCHRONOUS;
        infos.workingEdge = ProcessInfos::BOTH_EDGES;
    } else if (clockPos && !clockNeg) {
        infos.processKind = ProcessInfos::SYNCHRONOUS;
        infos.workingEdge = ProcessInfos::RISING_EDGE;
    } else if (!clockPos && clockNeg) {
        infos.processKind = ProcessInfos::SYNCHRONOUS;
        infos.workingEdge = ProcessInfos::FALLING_EDGE;
    }
    // else asynch: already fine

    if (resetPos && resetNeg) {
        infos.resetKind  = ProcessInfos::ASYNCHRONOUS_RESET;
        infos.resetPhase = ProcessInfos::NO_PHASE;
    } else if (resetPos && !resetNeg) {
        infos.resetKind  = ProcessInfos::ASYNCHRONOUS_RESET;
        infos.resetPhase = ProcessInfos::HIGH_PHASE;
    } else if (!resetPos && resetNeg) {
        infos.resetKind  = ProcessInfos::ASYNCHRONOUS_RESET;
        infos.resetPhase = ProcessInfos::LOW_PHASE;
    }
    // else maybe synch, but we do not know: already fine
    // Refining taking into account also other signals.
    bool noAsynch = (asynch == 0 && asynchPos == 0 && asynchNeg == 0);
    if (noAsynch) {
        return;
    }
    bool sameName = (asynchName.empty() || asynchPosName.empty() || asynchName == asynchPosName) &&
                          (asynchName.empty() || asynchNegName.empty() || asynchName == asynchNegName) &&
                          (asynchPosName.empty() || asynchNegName.empty() || asynchPosName == asynchNegName);

    bool oneAsynchForKind =
        (asynch == 0 || asynch == 1) && (asynchPos == 0 || asynchPos == 1) && (asynchNeg == 0 || asynchNeg == 1);

    bool justOneAsynch = oneAsynchForKind && sameName;

    if (infos.processKind == ProcessInfos::SYNCHRONOUS) {
        if (infos.resetKind == ProcessInfos::NO_RESET && justOneAsynch) {
            // Heuristic: in case of just one signal,
            // it is considered as a secondary reset.
            infos.resetKind = ProcessInfos::ASYNCHRONOUS_RESET;
            if (asynch == 0 && asynchPos != 0 && asynchNeg == 0) {
                infos.resetPhase = ProcessInfos::HIGH_PHASE;
            } else if (asynch == 0 && asynchPos == 0 && asynchNeg != 0) {
                infos.resetPhase = ProcessInfos::LOW_PHASE;
            } else {
                infos.resetPhase = ProcessInfos::NO_PHASE;
            }
            infos.reset = _getOtherSignal(infos, infos.clock);
        } else {
            infos.processKind = ProcessInfos::MIXED;
        }
    } else if (infos.resetKind != ProcessInfos::NO_RESET && justOneAsynch) {
        // Heuristic: derived clock
        infos.processKind = ProcessInfos::DERIVED_SYNCHRONOUS;
        if (asynch == 0 && asynchPos != 0 && asynchNeg == 0) {
            infos.workingEdge = ProcessInfos::RISING_EDGE;
        } else if (asynch == 0 && asynchPos == 0 && asynchNeg != 0) {
            infos.workingEdge = ProcessInfos::FALLING_EDGE;
        } else {
            infos.workingEdge = ProcessInfos::BOTH_EDGES;
        }
        infos.clock = _getOtherSignal(infos, infos.reset);
    } else {
        // no synch && no reset || many asynch --> asynch
        infos.processKind = ProcessInfos::ASYNCHRONOUS;
        infos.workingEdge = ProcessInfos::NO_EDGE;
    }
}

void ProcessVisitor::_classifyProcessBody(ProcessInfos &infos, StateTable *proc)
{
    messageAssert(proc->states.size() <= 1, "Unexpected process.", proc, _sem);
    if (proc->states.empty() || proc->states.front()->actions.empty()) {
        // Classify according with just sensitivity list:
        return;
    }
    State *state = proc->states.front();
    if (_isProcessStyle1(infos, state)) {
        return;
    }
    if (_isProcessStyle2(infos, state)) {
        return;
    }
    if (_isProcessStyle3(infos, state)) {
        return;
    }
    if (_isProcessStyle4(infos, state)) {
        return;
    }
    if (_isProcessStyle5(infos, state)) {
        return;
    }
    if (_isProcessStyle6(infos, state)) {
        return;
    }
#if 0
    if (infos.processKind != ProcessInfos::ASYNCHRONOUS
        && infos.processKind != ProcessInfos::MIXED)
    {
        _classifyProcessBody(infos, proc);
    }
#endif

    // Unable to classify?
    // Should be asynch: sanity check w.r.t. sensitivity.
#ifndef NDEBUG
    if (infos.processKind != ProcessInfos::ASYNCHRONOUS && infos.processKind != ProcessInfos::MIXED) {
        hif::writeFile(std::clog, proc);
    }
#endif
    messageAssert(
        infos.processKind == ProcessInfos::ASYNCHRONOUS || infos.processKind == ProcessInfos::MIXED
        //|| infos.processKind == ProcessInfos::DERIVED_MIXED
        //|| (infos.processKind == ProcessInfos::SYNCHRONOUS
        //    && infos.inputs.find(infos.clock) == infos.inputs.end())
        ,
        "Unexpected process kind.", proc, _sem);
}

auto ProcessVisitor::_mergeInfos(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool
{
    if (!_mergeProcessInfos(infosRet, infos1, infos2)) {
        return false;
    }
    if (!_mergeResetInfos(infosRet, infos1, infos2)) {
        return false;
    }
    if (!_mergeEdgeInfos(infosRet, infos1, infos2)) {
        return false;
    }
    if (!_mergePhaseInfos(infosRet, infos1, infos2)) {
        return false;
    }
    if (!_mergeProcessStyle(infosRet, infos1, infos2)) {
        return false;
    }
    if (!_mergeSignals(infosRet, infos1, infos2)) {
        return false;
    }
    return true;
}

auto ProcessVisitor::_mergeProcessInfos(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool
{
    switch (infos2.processKind) {
    case ProcessInfos::SYNCHRONOUS: {
        // if somethong strange:
        if (infos1.processKind != ProcessInfos::SYNCHRONOUS && infos1.processKind != ProcessInfos::MIXED) {
            return false;
        }
        infosRet.processKind = infos1.processKind;
        break;
    }
    case ProcessInfos::ASYNCHRONOUS: {
        // if somethong strange:
        if (infos1.processKind != ProcessInfos::ASYNCHRONOUS) {
            return false;
        }
        infosRet.processKind = ProcessInfos::ASYNCHRONOUS;
        break;
    }
    case ProcessInfos::DERIVED_SYNCHRONOUS: {
        if (infos1.processKind != ProcessInfos::ASYNCHRONOUS &&
            infos1.processKind != ProcessInfos::DERIVED_SYNCHRONOUS) {
            return false;
        }
        // Heuristic:
        // - If just 1/2 signals, it is the clock (plus reset).
        // - Otherwise, DERIVED_MIXED.
        const ProcessInfos::ReferredDeclarations::size_type size = infosRet.getSensitivitySize();
        if (size == 0) {
            return false;
        }
        if (size == 1 || size == 2) {
            infosRet.processKind = ProcessInfos::DERIVED_SYNCHRONOUS;
        } else {
            infosRet.processKind = ProcessInfos::DERIVED_MIXED;
        }
        break;
    }
    case ProcessInfos::MIXED: {
        messageError("Unexpected process case (1).", nullptr, _sem);
    }
    case ProcessInfos::DERIVED_MIXED: {
        messageError("Unexpected process case (2).", nullptr, _sem);
    }
    default: {
        messageError("Unexpected process case (3).", nullptr, _sem);
    }
    }
    return true;
}

auto ProcessVisitor::_mergeResetInfos(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool
{
    switch (infos2.resetKind) {
    case ProcessInfos::NO_RESET: {
        if (infos1.resetKind != ProcessInfos::NO_RESET) {
            return false;
        }
        infosRet.resetKind = ProcessInfos::NO_RESET;
        break;
    }
    case ProcessInfos::SYNCHRONOUS_RESET: {
        if (infos1.resetKind != ProcessInfos::NO_RESET) {
            return false;
        }
        infosRet.resetKind = ProcessInfos::SYNCHRONOUS_RESET;
        break;
    }
    case ProcessInfos::ASYNCHRONOUS_RESET: {
        if (infos1.resetKind == ProcessInfos::NO_RESET) {
            // Actualy, a derived synch/asynch reset?
            // Heuristic:
            // - If just one signal, it is the clock.
            // - Otherwise, one of the other is the reset.
            if (infosRet.processKind == ProcessInfos::ASYNCHRONOUS) {
                return false;
            }
            const ProcessInfos::ReferredDeclarations::size_type size = infosRet.getSensitivitySize();
            if (size == 0) {
                return false;
            }
            if (size == 1) {
                if (infosRet.processKind == ProcessInfos::SYNCHRONOUS || infosRet.processKind == ProcessInfos::MIXED) {
                    infosRet.resetKind = ProcessInfos::SYNCHRONOUS_RESET;
                } else {
                    infosRet.resetKind = ProcessInfos::DERIVED_SYNCHRONOUS_RESET;
                }
            } else {
                infosRet.resetKind = ProcessInfos::ASYNCHRONOUS_RESET;
            }
        } else {
            if (infos2.resetKind != ProcessInfos::ASYNCHRONOUS_RESET) {
                return false;
            }
            infosRet.resetKind = ProcessInfos::ASYNCHRONOUS_RESET;
        }
        break;
    }
    case ProcessInfos::DERIVED_SYNCHRONOUS_RESET: {
        if (infos1.resetKind != ProcessInfos::NO_RESET) {
            return false;
        }
        infosRet.resetKind = ProcessInfos::DERIVED_SYNCHRONOUS_RESET;
        break;
    }
    default: {
        messageError("Unexpected reset case.", nullptr, nullptr);
    }
    }
    return true;
}

auto ProcessVisitor::_mergeEdgeInfos(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool
{
    switch (infos2.workingEdge) {
    case ProcessInfos::NO_EDGE: {
        if (infos1.workingEdge != ProcessInfos::NO_EDGE) {
            return false;
        }
        infosRet.workingEdge = ProcessInfos::NO_EDGE;
        break;
    }
    case ProcessInfos::RISING_EDGE: {
        if ((infos1.workingEdge == ProcessInfos::NO_EDGE && infosRet.processKind != ProcessInfos::DERIVED_SYNCHRONOUS &&
             infosRet.processKind != ProcessInfos::DERIVED_MIXED) ||
            infos1.workingEdge == ProcessInfos::FALLING_EDGE) {
            return false;
        }
        infosRet.workingEdge = ProcessInfos::RISING_EDGE;
        break;
    }
    case ProcessInfos::FALLING_EDGE: {
        if ((infos1.workingEdge == ProcessInfos::NO_EDGE &&
             infosRet.processKind != ProcessInfos::DERIVED_SYNCHRONOUS) ||
            infos1.workingEdge == ProcessInfos::RISING_EDGE) {
            return false;
        }
        infosRet.workingEdge = ProcessInfos::FALLING_EDGE;
        break;
    }
    case ProcessInfos::BOTH_EDGES: {
        if (infos1.workingEdge == ProcessInfos::NO_EDGE && infosRet.processKind != ProcessInfos::DERIVED_SYNCHRONOUS) {
            return false;
        }
        infosRet.workingEdge = infos1.workingEdge;
        break;
    }
    default: {
        messageError("Unexpected working edge case.", nullptr, _sem);
    }
    }
    return true;
}

auto ProcessVisitor::_mergePhaseInfos(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool
{
    switch (infos2.resetPhase) {
    case ProcessInfos::NO_PHASE: {
        if (infos1.resetKind != ProcessInfos::NO_RESET) {
            return false;
        }
        infosRet.resetPhase = infos1.resetPhase;
        break;
    }
    case ProcessInfos::HIGH_PHASE: {
        if (infos1.resetPhase == ProcessInfos::LOW_PHASE) {
            return false;
        }
        infosRet.resetPhase = ProcessInfos::HIGH_PHASE;
        break;
    }
    case ProcessInfos::LOW_PHASE: {
        if (infos1.resetPhase == ProcessInfos::HIGH_PHASE) {
            return false;
        }
        infosRet.resetPhase = ProcessInfos::LOW_PHASE;
        break;
    }
    default: {
        messageError("Unexpected reset phase case.", nullptr, nullptr);
    }
    }
    return true;
}

auto ProcessVisitor::_mergeProcessStyle(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool
{
    if (infos1.processStyle == ProcessInfos::NO_STYLE) {
        infosRet.processStyle = infos2.processStyle;
        return true;
    }
    if (infos2.processStyle == ProcessInfos::NO_STYLE) {
        infosRet.processStyle = infos1.processStyle;
        return true;
    }
    if (infos1.processStyle == infos2.processStyle) {
        infosRet.processStyle = infos1.processStyle;
        return true;
    }
    return false;
}

auto ProcessVisitor::_mergeSignals(ProcessInfos &infosRet, ProcessInfos &infos1, ProcessInfos &infos2) -> bool
{
    bool clockOk = infos1.clock == nullptr || infos2.clock == nullptr || infos1.clock == infos2.clock;

    bool resetOk = infos1.reset == nullptr || infos2.reset == nullptr || infos1.reset == infos2.reset;

    if (!clockOk || !resetOk) {
        return false;
    }
    infosRet.clock = infos2.clock != nullptr ? infos2.clock : infos1.clock;
    infosRet.reset = infos2.reset != nullptr ? infos2.reset : infos1.reset;

    return true;
}

auto ProcessVisitor::_isIfElseIfStatement(State *state, IfAlt *&resetIf, IfAlt *&clockIf) -> bool
{
    resetIf = nullptr;
    clockIf = nullptr;

    if (state->actions.size() != 1) {
        return false;
    }
    Action *a = state->actions.front();
    If *ifStm = dynamic_cast<If *>(a);
    if (ifStm == nullptr) {
        return false;
    }
    if (ifStm->alts.size() != 1 && ifStm->alts.size() != 2) {
        return false;
    }
    if (ifStm->alts.size() == 2) {
        // if ... elseif ...
        resetIf = ifStm->alts.front();
        clockIf = ifStm->alts.back();
    } else {
        // if ... else { if ... }
        if (ifStm->defaults.size() != 1) {
            return false;
        }
        If *clkStm = dynamic_cast<If *>(ifStm->defaults.front());
        if (clkStm == nullptr) {
            return false;
        }
        if (clkStm->alts.size() != 1 || !clkStm->defaults.empty()) {
            return false;
        }
        resetIf = ifStm->alts.front();
        clockIf = clkStm->alts.front();
    }
    return true;
}

auto ProcessVisitor::_isIfOptIfElseStatement(State *state, IfAlt *&resetIf, IfAlt *&clockIf) -> bool
{
    resetIf = nullptr;
    clockIf = nullptr;

    if (state->actions.size() != 1) {
        return false;
    }
    Action *a = state->actions.front();
    If *ifStm = dynamic_cast<If *>(a);
    if (ifStm == nullptr) {
        return false;
    }
    if (ifStm->alts.size() != 1 || !ifStm->defaults.empty()) {
        return false;
    }
    // Matched.
    clockIf = ifStm->alts.front();

    If *innerIf = nullptr;
    if (ifStm->alts.size() == 1 && ifStm->alts.front()->actions.size() == 1) {
        innerIf = dynamic_cast<If *>(ifStm->alts.front()->actions.front());
    }
    if (innerIf != nullptr) {
        // Matching the inner if pattern?
        if (innerIf->alts.size() == 1 && !innerIf->defaults.empty()) {
            resetIf = innerIf->alts.front();
        }
    }
    return true;
}

auto ProcessVisitor::_isEqualsToZeroOrOne(Value *cond, DataDeclaration *&n, bool &isZero) -> bool
{
    Identifier *id = nullptr;
    id             = dynamic_cast<Identifier *>(hif::getChildSkippingCasts(cond));
    if (id != nullptr) {
        // Direct identifier
        n      = hif::semantics::getDeclaration(id, _sem);
        isZero = false;
        return true;
    }
    auto *e = dynamic_cast<Expression *>(hif::getChildSkippingCasts(cond));
    if (e == nullptr) {
        return false;
    }
    if (e->getOperator() != op_eq && e->getOperator() != op_case_eq && e->getOperator() != op_not) {
        return false;
    }
    id = dynamic_cast<Identifier *>(hif::getChildSkippingCasts(e->getValue1()));
    if (id == nullptr) {
        return false;
    }
    if (e->getOperator() == op_not) {
        // cond: if (!reset)
        n      = hif::semantics::getDeclaration(id, _sem);
        isZero = true;
        return true;
    }
    // Equality expression:
    Int i;
    i.setSpan(new Range(63, 0));
    // using def. sem
    Value *tmp = hif::manipulation::transformValue(hif::getChildSkippingCasts(e->getValue2()), &i);
    if (tmp == nullptr) {
        return false;
    }
    auto *iv = dynamic_cast<IntValue *>(tmp);
    if (iv == nullptr) {
        delete tmp;
        return false;
    }
    std::int64_t cv = iv->getValue();
    delete iv;
    if (cv != 0 && cv != 1) {
        return false;
    }
    isZero = (cv == 0);

    n = hif::semantics::getDeclaration(id, _sem);
    return true;
}

auto ProcessVisitor::_isRisingFallingEdge(Value *cond, DataDeclaration *&n, bool &isZero) -> bool
{
    auto *fc = dynamic_cast<FunctionCall *>(cond);
    if (fc == nullptr) {
        return false;
    }
    if (fc->parameterAssigns.size() != 1) {
        return false;
    }
    if (fc->getName() == "hif_vhdl_rising_edge") {
        isZero = false;
    } else if (fc->getName() == "hif_vhdl_falling_edge") {
        isZero = true;
    } else {
        return false;
    }
    ParameterAssign *pa = fc->parameterAssigns.front();
    auto *id            = dynamic_cast<Identifier *>(hif::getChildSkippingCasts(pa->getValue()));
    if (id == nullptr) {
        return false;
    }
    n = hif::semantics::getDeclaration(id, _sem);

    return true;
}

auto ProcessVisitor::_checkResetCondition(ProcessInfos &infos, Value *cond) -> bool
{
    DataDeclaration *n = nullptr;
    bool isZero        = false;

    if (!_isEqualsToZeroOrOne(cond, n, isZero)) {
        return false;
    }
    //if (n->getName() != _opt.reset && _opt.reset != nullptr) return false;

    if (isZero) {
        infos.resetPhase = ProcessInfos::LOW_PHASE;
    } else {
        infos.resetPhase = ProcessInfos::HIGH_PHASE;
    }
    infos.reset = n;

    if (infos.isInSensitivity(infos.reset)) {
        infos.resetKind = ProcessInfos::ASYNCHRONOUS_RESET;
    } else if (infos.processKind == ProcessInfos::ASYNCHRONOUS) {
        infos.resetKind = ProcessInfos::ASYNCHRONOUS_RESET;
    } else if (infos.processKind == ProcessInfos::SYNCHRONOUS || infos.processKind == ProcessInfos::MIXED) {
        infos.resetKind = ProcessInfos::SYNCHRONOUS_RESET;
    } else if (
        infos.processKind == ProcessInfos::DERIVED_SYNCHRONOUS || infos.processKind == ProcessInfos::DERIVED_MIXED) {
        infos.resetKind = ProcessInfos::SYNCHRONOUS_RESET;
    } else {
        messageError("Unexpected clock value.", nullptr, nullptr);
    }
    return true;
}

auto ProcessVisitor::_checkClockCondition(ProcessInfos &infos, Value *cond) -> bool
{
    DataDeclaration *clkName = nullptr;
    bool clkIsZero           = false;
    bool found               = false;

    if (dynamic_cast<Expression *>(hif::getChildSkippingCasts(cond)) != nullptr) {
        auto *e = dynamic_cast<Expression *>(hif::getChildSkippingCasts(cond));
        if (e->getOperator() != op_and) {
            return false;
        }
        // Checking event:
        DataDeclaration *clkName1 = nullptr;
        {
            auto *fc = dynamic_cast<FunctionCall *>(e->getValue1());
            if (fc == nullptr) {
                return false;
            }
            if (fc->getName() != "hif_vhdl_event") {
                return false;
            }
            auto *id = dynamic_cast<Identifier *>(fc->getInstance());
            if (id == nullptr) {
                return false;
            }
            clkName1 = hif::semantics::getDeclaration(id, _sem);
        }
        if (!_isEqualsToZeroOrOne(e->getValue2(), clkName, clkIsZero)) {
            return false;
        }
        if (clkName1 != clkName) {
            return false;
        }
        // clock'event && clock == 1/0
        found = true;
    } else if (dynamic_cast<FunctionCall *>(cond) != nullptr) {
        auto *fc = dynamic_cast<FunctionCall *>(cond);
        if (!_isRisingFallingEdge(fc, clkName, clkIsZero)) {
            return false;
        }
        // rising_edge(clk)/falling_edge(clk)
        found = true;
    }
    if (!found) {
        return false;
    }
    // Matches the pattern, but it is not a true clock.
    if (!infos.isInSensitivity(clkName)) {
        return false;
    }
    infos.processKind =
        (clkName->getName() == _opt.clock) ? ProcessInfos::SYNCHRONOUS : ProcessInfos::DERIVED_SYNCHRONOUS;
    infos.workingEdge = clkIsZero ? ProcessInfos::FALLING_EDGE : ProcessInfos::RISING_EDGE;

    infos.clock = clkName;

    return true;
}

auto ProcessVisitor::_isProcessStyle1(ProcessInfos &infos, State *state) -> bool
{
    IfAlt *reset = nullptr;
    IfAlt *clock = nullptr;
    if (!_isIfElseIfStatement(state, reset, clock)) {
        return false;
    }
    ProcessInfos tmp(infos);
    if (!_checkClockCondition(tmp, clock->getCondition())) {
        return false;
    }
    if (!_checkResetCondition(tmp, reset->getCondition())) {
        return false;
    }
    // Matched!
    // Merging infos with the ones collected from the sensitivity list:
    tmp.processStyle = ProcessInfos::STYLE_1;

    ProcessInfos res(infos);
    if (!_mergeInfos(res, infos, tmp)) {
        if (_opt.printWarnings) {
            messageWarning("Unable to classify correctly the process (1).", state->getParent(), _sem);
        }
#if 0
        _mergeInfos(res, infos, tmp);
#endif
        assert(false);
        return true;
    }
    infos = res;
    return true;
}

auto ProcessVisitor::_isProcessStyle2(ProcessInfos &infos, State *state) -> bool
{
    IfAlt *reset = nullptr;
    IfAlt *clock = nullptr;
    if (!_isIfOptIfElseStatement(state, reset, clock)) {
        return false;
    }
    ProcessInfos tmp(infos);

    if (!_checkClockCondition(tmp, clock->getCondition())) {
        return false;
    }
    if (reset != nullptr) {
        // No error if not matched:
        // could be a plain inner if, not on reset.
        if (_checkResetCondition(tmp, reset->getCondition())) {
            tmp.resetKind = ProcessInfos::SYNCHRONOUS_RESET;
        } else {
            tmp.resetKind = ProcessInfos::NO_RESET;
        }
    }
    // Matched!
    // Merging infos with the ones collected from the sensitivity list:
    tmp.processStyle = ProcessInfos::STYLE_2;

    ProcessInfos res(infos);
    if (!_mergeInfos(res, infos, tmp)) {
        if (_opt.printWarnings) {
            messageWarning("Unable to classify correctly the process (2).", state->getParent(), _sem);
        }
        return true;
    }
    infos = res;
    return true;
}

auto ProcessVisitor::_isProcessStyle3(ProcessInfos &infos, State *state) -> bool
{
    if (state->actions.size() != 1) {
        return false;
    }
    auto *sw = dynamic_cast<Switch *>(state->actions.front());
    if (sw == nullptr) {
        return false;
    }
    // It is a state machine: maybe synch...
    if (infos.getSensitivitySize() != 1 ||
        (((infos.risingSensitivity.size() == 1) ^ (infos.fallingSensitivity.size() == 1)) == 0)) {
        return false;
    }
    ProcessInfos tmp(infos);

    if (!infos.risingSensitivity.empty()) {
        tmp.clock       = *infos.risingSensitivity.begin();
        tmp.workingEdge = ProcessInfos::RISING_EDGE;
    } else {
        tmp.clock       = *infos.fallingSensitivity.begin();
        tmp.workingEdge = ProcessInfos::FALLING_EDGE;
    }
    if (tmp.clock->getName() == _opt.clock) {
        tmp.processKind = ProcessInfos::SYNCHRONOUS;
    } else {
        tmp.processKind = ProcessInfos::DERIVED_SYNCHRONOUS;
    }
    // Matched!
    // Merging infos with the ones collected from the sensitivity list:
    tmp.processStyle = ProcessInfos::STYLE_3;

    ProcessInfos res(infos);
    if (!_mergeInfos(res, infos, tmp)) {
        if (_opt.printWarnings) {
            messageWarning("Unable to classify correctly the process (3).", state->getParent(), _sem);
        }
        return true;
    }
    infos = res;

    return true;
}

auto ProcessVisitor::_isProcessStyle4(ProcessInfos &infos, State *state) -> bool
{
    //    if (infos.processKind != ProcessInfos::MIXED
    //        && infos.processKind != ProcessInfos::ASYNCHRONOUS
    //        && infos.processKind != ProcessInfos::SYNCHRONOUS)
    //        return false;

    bool isStyle1 = false;
    bool isStyle2 = false;
    ProcessInfos infos1(infos);
    ProcessInfos infos2(infos);
    for (BList<Action>::iterator i = state->actions.begin(); i != state->actions.end(); ++i) {
        State st;
        st.actions.push_back(hif::copy(*i));
        isStyle1 |= _isProcessStyle1(infos1, &st);
        isStyle2 |= _isProcessStyle2(infos2, &st);
    }
    if (!isStyle1 && !isStyle2) {
        return false;
    }
    messageAssert(!isStyle1 || !isStyle2, "Unexpected analysis result.", state, _sem);

    if (isStyle1) {
        infos = infos1;
    } else if (isStyle2) {
        infos = infos2;
    }
    infos.processStyle = ProcessInfos::STYLE_4;

    return true;
}

auto ProcessVisitor::_isProcessStyle5(ProcessInfos &infos, State *state) -> bool
{
    if (infos.clock == nullptr) {
        return false;
    }
    if (infos.processKind != ProcessInfos::SYNCHRONOUS) {
        return false;
    }
    if (infos.inputs.find(infos.clock) != infos.inputs.end()) {
        return false;
    }
    if (state->actions.size() == 1 && dynamic_cast<If *>(state->actions.front()) != nullptr) {
        return false;
    }
    infos.processStyle = ProcessInfos::STYLE_5;

    return true;
}

auto ProcessVisitor::_isProcessStyle6(ProcessInfos &infos, State *state) -> bool
{
    if (infos.processKind != ProcessInfos::SYNCHRONOUS && infos.processKind != ProcessInfos::ASYNCHRONOUS) {
        return false;
    }
    if (state->actions.size() != 1) {
        return false;
    }
    If *ifStm = dynamic_cast<If *>(state->actions.front());
    if (ifStm == nullptr) {
        return false;
    }
    if (ifStm->alts.size() != 1) {
        return false;
    }
    ProcessInfos tmp(infos);
    if (!_checkResetCondition(tmp, ifStm->alts.front()->getCondition())) {
        return false;
    }
    bool hasMainClock = true;
    if (tmp.clock == nullptr) {
        hasMainClock = false;
        tmp.clock    = _getOtherSignal(tmp, tmp.reset);
    }
    if (tmp.risingSensitivity.find(tmp.clock) != tmp.risingSensitivity.end()) {
        if (hasMainClock) {
            tmp.processKind = ProcessInfos::SYNCHRONOUS;
        } else {
            tmp.processKind = ProcessInfos::DERIVED_SYNCHRONOUS;
        }
        tmp.workingEdge = ProcessInfos::RISING_EDGE;
    } else if (tmp.fallingSensitivity.find(tmp.clock) != tmp.fallingSensitivity.end()) {
        if (hasMainClock) {
            tmp.processKind = ProcessInfos::SYNCHRONOUS;
        } else {
            tmp.processKind = ProcessInfos::DERIVED_SYNCHRONOUS;
        }
        tmp.workingEdge = ProcessInfos::FALLING_EDGE;
    } else {
        return false;
    }
    // Matched!
    // Merging infos with the ones collected from the sensitivity list:
    tmp.processStyle = ProcessInfos::STYLE_6;

    ProcessInfos res(infos);
    if (!_mergeInfos(res, infos, tmp)) {
        if (_opt.printWarnings) {
            messageWarning("Unable to classify correctly the process (1).", state->getParent(), _sem);
        }
#if 0
        _mergeInfos(res, infos, tmp);
#endif
        assert(false);
        return true;
    }
    infos = res;
    return true;
}
} // namespace
ProcessInfos::ProcessInfos()
    : processKind(ASYNCHRONOUS)
    , resetKind(NO_RESET)
    , workingEdge(NO_EDGE)
    , resetPhase(NO_PHASE)
    , processStyle(NO_STYLE)
    , clock(nullptr)
    , reset(nullptr)
{
    // ntd
}
ProcessInfos::~ProcessInfos()
{
    // ntd
}
ProcessInfos::ProcessInfos(const ProcessInfos &other)
    : processKind(other.processKind)
    , resetKind(other.resetKind)
    , workingEdge(other.workingEdge)
    , resetPhase(other.resetPhase)
    , processStyle(other.processStyle)
    , risingSensitivity(other.risingSensitivity)
    , fallingSensitivity(other.fallingSensitivity)
    , sensitivity(other.sensitivity)
    , inputs(other.inputs)
    , outputs(other.outputs)
    , inputVariables(other.inputVariables)
    , outputVariables(other.outputVariables)
    , clock(other.clock)
    , reset(other.reset)
{
    // ntd
}

auto ProcessInfos::operator=(const ProcessInfos &other) -> ProcessInfos &
{
    if (this == &other) {
        return *this;
    }
    processKind        = other.processKind;
    resetKind          = other.resetKind;
    workingEdge        = other.workingEdge;
    resetPhase         = other.resetPhase;
    processStyle       = other.processStyle;
    risingSensitivity  = other.risingSensitivity;
    fallingSensitivity = other.fallingSensitivity;
    sensitivity        = other.sensitivity;
    inputs             = other.inputs;
    outputs            = other.outputs;
    inputVariables     = other.inputVariables;
    outputVariables    = other.outputVariables;
    clock              = other.clock;
    reset              = other.reset;

    return *this;
}

auto ProcessInfos::getSensitivitySize() const -> ProcessInfos::ReferredDeclarations::size_type
{
    ReferredDeclarations tmp;
    tmp.insert(sensitivity.begin(), sensitivity.end());
    tmp.insert(risingSensitivity.begin(), risingSensitivity.end());
    tmp.insert(fallingSensitivity.begin(), fallingSensitivity.end());
    return tmp.size();
}

auto ProcessInfos::isInSensitivity(ReferredDeclarations::value_type v) const -> bool
{
    auto i = sensitivity.find(v);
    if (i != sensitivity.end()) {
        return true;
    }
    i = risingSensitivity.find(v);
    if (i != risingSensitivity.end()) {
        return true;
    }
    i = fallingSensitivity.find(v);
    return i != fallingSensitivity.end();
}
AnalyzeProcessOptions::AnalyzeProcessOptions()
    : clock(nullptr)
    , reset(nullptr)
    , skip_standard_declarations(true)
    , printWarnings(false)
{
    // ntd
}
AnalyzeProcessOptions::~AnalyzeProcessOptions()
{
    // ntd.
}
AnalyzeProcessOptions::AnalyzeProcessOptions(const AnalyzeProcessOptions &other)
    : clock(other.clock)
    , reset(other.reset)
    , skip_standard_declarations(other.skip_standard_declarations)
    , printWarnings(other.printWarnings)
{
    // ntd
}

auto AnalyzeProcessOptions::operator=(const AnalyzeProcessOptions &other) -> AnalyzeProcessOptions &
{
    if (this == &other) {
        return *this;
    }
    clock                    = other.clock;
    reset                    = other.reset;
    skip_standard_declarations = other.skip_standard_declarations;
    printWarnings            = other.printWarnings;

    return *this;
}

auto analyzeProcesses(
    Object *root,
    AnalyzeProcessOptions::ProcessMap &map,
    semantics::ILanguageSemantics *sem,
    const AnalyzeProcessOptions &opt) -> bool
{
    hif::application_utils::initializeLogHeader("HIF", "analyzeProcesses");
    ProcessVisitor pv(map, sem, opt);
    root->acceptVisitor(pv);
    hif::application_utils::restoreLogHeader();
    return true;
}
} // namespace analysis

} // namespace hif
