/// @file insertDelays.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <sstream>

#include "hif/hif.hpp"
#include "hif/semantics/declarationUtils.hpp"

namespace hif
{

namespace analysis
{

namespace /* anon */
{

using RefSet    = std::set<Object *>;
using RefMap    = std::map<Declaration *, RefSet>;
using Processes = std::list<StateTable *>;

auto _makeName(
    const std::string &prefix,
    const std::size_t clocks,
    const std::size_t deltas,
    const bool halfClock) -> std::string
{
    std::stringstream ss;
    ss << prefix << clocks;
    if (halfClock) {
        ss << "_5";
    }
    ss << "cc_" << deltas << "d";

    return ss.str();
}

void _fixOutputReset(Processes &newProcesses, Signal *prev, DataDeclaration *reset, const bool isInput)
{
    if (isInput || reset == nullptr) {
        return;
    }
    for (auto *st : newProcesses) {
        If *ifStm = dynamic_cast<If *>(st->states.front()->actions.front());
        messageAssert(ifStm != nullptr, "Unexpected case (1).", nullptr, nullptr);
        auto *ass = dynamic_cast<Assign *>(ifStm->alts.front()->actions.front());
        messageAssert(ass != nullptr, "Unexpected case (2).", nullptr, nullptr);
        auto *id = dynamic_cast<Identifier *>(ass->getRightHandSide());
        messageAssert(id != nullptr, "Unexpected case (3).", nullptr, nullptr);
        id->setName(prev->getName());
    }
}

auto _makeSignal(
    DataDeclaration *ref,
    View *view,
    const std::size_t deltas,
    const std::size_t clocks,
    const bool halfClock,
    hif::semantics::ILanguageSemantics *sem) -> Signal *
{
    std::string name(_makeName("_ds_", clocks, deltas, halfClock));

    auto *s = new Signal();
    s->setName(NameTable::getInstance()->getFreshName(ref->getName(), name.c_str()));
    s->setType(hif::copy(ref->getType()));
    view->getContents()->declarations.push_back(s);
    if (ref->getValue() != nullptr) {
        s->setValue(hif::copy(ref->getValue()));
    } else {
        s->setValue(sem->getTypeDefaultValue(s->getType(), s));
    }
    return s;
}

auto _createProcessBody(
    const std::string &procNameSuffix,
    DataDeclaration *ref,
    View *view,
    DataDeclaration *reset,
    Value *resetValue,
    BList<Action> *&actions,
    BList<Action> *&resetActions) -> StateTable *
{
    StateTable *st = nullptr;
    st             = new StateTable();
    view->getContents()->stateTables.push_back(st);
    st->setName(NameTable::getInstance()->getFreshName(ref->getName(), procNameSuffix.c_str()));
    State *state = nullptr;
    state        = new State();
    state->setName(st->getName());
    st->states.push_back(state);
    if (reset != nullptr) {
        If *ifStm  = nullptr;
        ifStm      = new If();
        IfAlt *ifa = nullptr;
        ifa        = new IfAlt();
        ifStm->alts.push_back(ifa);
        actions      = &ifStm->defaults;
        resetActions = &ifa->actions;

        Expression *cond = nullptr;
        cond             = new Expression();
        cond->setOperator(op_case_eq);
        cond->setValue1(new Identifier(reset->getName()));
        cond->setValue2(hif::copy(resetValue));
        ifa->setCondition(cond);

        st->sensitivity.push_back(new Identifier(reset->getName()));
        state->actions.push_back(ifStm);
    } else {
        actions      = &state->actions;
        resetActions = nullptr;
    }
    return st;
}

auto _makeDeltaProcess(
    Processes &newProcesses,
    DataDeclaration *ref,
    DataDeclaration *prev,
    View *view,
    DataDeclaration *reset,
    Value *resetValue,
    const std::size_t deltas,
    const std::size_t clockCycles,
    const bool halfClock,
    hif::semantics::ILanguageSemantics *sem,
    const bool isInput) -> Signal *
{
    if (prev == nullptr) {
        prev = ref;
    }
    std::string procSuffix(_makeName("_dp_", clockCycles, deltas, halfClock));

    BList<Action> *actions      = nullptr;
    BList<Action> *resetActions = nullptr;
    StateTable *st              = _createProcessBody(procSuffix, ref, view, reset, resetValue, actions, resetActions);
    newProcesses.push_back(st);

    Signal *s = _makeSignal(ref, view, deltas, clockCycles, halfClock, sem);

    Assign *ass = nullptr;
    ass         = new Assign();
    if (isInput) {
        ass->setLeftHandSide(new Identifier(s->getName()));
        ass->setRightHandSide(new Identifier(prev->getName()));
        st->sensitivity.push_front(new Identifier(prev->getName()));
        actions->push_back(ass);
    } else {
        ass->setLeftHandSide(new Identifier(prev->getName()));
        ass->setRightHandSide(new Identifier(s->getName()));
        st->sensitivity.push_front(new Identifier(s->getName()));
        actions->push_front(ass);
    }
    if (resetActions != nullptr) {
        Assign *rass = nullptr;
        rass         = new Assign();
        if (isInput) {
            rass->setLeftHandSide(new Identifier(s->getName()));
            rass->setRightHandSide(new Identifier(ref->getName()));
        } else {
            rass->setLeftHandSide(new Identifier(prev->getName()));
            rass->setRightHandSide(new Identifier(s->getName()));
        }
        resetActions->push_back(rass);
    }
    return s;
}

auto _makeClockProcess(
    Processes &newProcesses,
    DataDeclaration *ref,
    DataDeclaration *prev,
    DataDeclaration *clock,
    DelayInfos::WorkingEdge workingEdge,
    View *view,
    DataDeclaration *reset,
    Value *resetValue,
    const std::size_t deltas,
    const std::size_t clockCycles,
    const bool halfClock,
    hif::semantics::ILanguageSemantics *sem,
    const bool isInput) -> Signal *
{
    messageAssert(clock != nullptr, "Clock not found.", nullptr, nullptr);

    if (prev == nullptr) {
        prev = ref;
    }
    std::string procSuffix(_makeName("_dp_", clockCycles, deltas, halfClock));
    BList<Action> *actions      = nullptr;
    BList<Action> *resetActions = nullptr;
    StateTable *st              = _createProcessBody(procSuffix, ref, view, reset, resetValue, actions, resetActions);
    newProcesses.push_back(st);

    switch (workingEdge) {
    case ProcessInfos::NO_EDGE:
        messageError("Clock edge missing.", clock, sem);
    case ProcessInfos::RISING_EDGE:
        st->sensitivityPos.push_front(new Identifier(clock->getName()));
        break;
    case ProcessInfos::FALLING_EDGE:
        st->sensitivityNeg.push_front(new Identifier(clock->getName()));
        break;
    case ProcessInfos::BOTH_EDGES:
        messageError("Unexpected clock edge (1).", clock, sem);
    default:
        messageError("Unexpected clock edge (2).", clock, sem);
    }
    Signal *s = nullptr;
    for (std::size_t i = 0; i <= clockCycles; ++i) {
        s = _makeSignal(ref, view, deltas, i, false, sem);

        Assign *ass = nullptr;
        ass         = new Assign();
        if (isInput) {
            ass->setLeftHandSide(new Identifier(s->getName()));
            ass->setRightHandSide(new Identifier(prev->getName()));
            actions->push_back(ass);
        } else {
            ass->setLeftHandSide(new Identifier(prev->getName()));
            ass->setRightHandSide(new Identifier(s->getName()));
            actions->push_front(ass);
        }
        if (resetActions != nullptr) {
            Assign *rass = nullptr;
            rass         = new Assign();
            if (isInput) {
                rass->setLeftHandSide(new Identifier(s->getName()));
                rass->setRightHandSide(new Identifier(ref->getName()));
            } else {
                rass->setLeftHandSide(new Identifier(prev->getName()));
                rass->setRightHandSide(new Identifier(s->getName()));
            }
            resetActions->push_back(rass);
        }
        prev = s;
    }
    return s;
}

auto _makeHalfClockProcess(
    Processes &newProcesses,
    DataDeclaration *ref,
    DataDeclaration *prev,
    DataDeclaration *clock,
    DelayInfos::WorkingEdge workingEdge,
    View *view,
    DataDeclaration *reset,
    Value *resetValue,
    const std::size_t deltas,
    const std::size_t clockCycles,
    hif::semantics::ILanguageSemantics *sem,
    const bool isInput) -> Signal *
{
    messageAssert(clock != nullptr, "Clock not found.", nullptr, nullptr);

    if (prev == nullptr) {
        prev = ref;
    }
    std::string procSuffix(_makeName("_dp_", clockCycles, deltas, true));

    BList<Action> *actions      = nullptr;
    BList<Action> *resetActions = nullptr;
    StateTable *st              = _createProcessBody(procSuffix, ref, view, reset, resetValue, actions, resetActions);
    newProcesses.push_back(st);

    switch (workingEdge) {
    case ProcessInfos::NO_EDGE:
        messageError("Clock edge missing.", clock, sem);
    case ProcessInfos::RISING_EDGE:
        // Works on opposite clock side
        st->sensitivityNeg.push_front(new Identifier(clock->getName()));
        break;
    case ProcessInfos::FALLING_EDGE:
        // Works on opposite clock side
        st->sensitivityPos.push_front(new Identifier(clock->getName()));
        break;
    case ProcessInfos::BOTH_EDGES:
        messageError("Unexpected clock edge (1).", clock, sem);
    default:
        messageError("Unexpected clock edge (2).", clock, sem);
    }
    Signal *s = nullptr;
    s         = _makeSignal(ref, view, deltas, clockCycles, true, sem);

    Assign *ass = nullptr;
    ass         = new Assign();
    if (isInput) {
        ass->setLeftHandSide(new Identifier(s->getName()));
        ass->setRightHandSide(new Identifier(prev->getName()));
        actions->push_back(ass);
    } else {
        ass->setLeftHandSide(new Identifier(prev->getName()));
        ass->setRightHandSide(new Identifier(s->getName()));
        actions->push_front(ass);
    }
    if (resetActions != nullptr) {
        Assign *rass = nullptr;
        rass         = new Assign();
        if (isInput) {
            rass->setLeftHandSide(new Identifier(s->getName()));
            rass->setRightHandSide(new Identifier(ref->getName()));
        } else {
            rass->setLeftHandSide(new Identifier(prev->getName()));
            rass->setRightHandSide(new Identifier(s->getName()));
        }
        resetActions->push_back(rass);
    }
    return s;
}

void _insertTrueDelay(
    DelayInfos &delayInfos,
    RefSet &refs,
    DelayProperties &delay,
    hif::semantics::ILanguageSemantics *sem,
    const bool isInput)
{
    // Creating support reset value.
    Value *rv = nullptr;
    if (delayInfos.reset != nullptr) {
        BitValue bb;
        switch (delayInfos.resetPhase) {
        case ProcessInfos::NO_PHASE:
            messageError("Reset phase not found.", nullptr, nullptr);
        case ProcessInfos::HIGH_PHASE:
            bb.setValue(hif::bit_one);
            break;
        case ProcessInfos::LOW_PHASE:
            bb.setValue(hif::bit_zero);
            break;
        default:
            messageError("Unexpected reset case.", nullptr, nullptr);
        }
        bb.setType(sem->getTypeForConstant(&bb));
        rv = hif::manipulation::transformConstant(&bb, delayInfos.reset->getType(), sem);
        messageAssert(rv != nullptr, "Cannot create reset phase value.", delayInfos.reset, sem);
    }
    if (delay.deltas == 0 && delay.clockCycles == 0 && !delay.halfClock) {
        // Not delayed ports must get a delta delay to be aligned with delayed ports
        delay.deltas = 1;
    } else if (delay.deltas != 0 && delay.clockCycles == 0 && !delay.halfClock) {
        // delta ports must be delayed of one additional delta
        ++delay.deltas;
    }
    Signal *prev = nullptr;
    Processes newProcesses;

    if (delay.clockCycles != 0) {
        prev = _makeClockProcess(
            newProcesses, delay.port, prev, delayInfos.clock, delayInfos.workingEdge, delayInfos.view, delayInfos.reset,
            rv, 0, delay.clockCycles, false, sem, isInput);
    }
    if (delay.halfClock) {
        prev = _makeHalfClockProcess(
            newProcesses, delay.port, prev, delayInfos.clock, delayInfos.workingEdge, delayInfos.view, delayInfos.reset,
            rv, 0, delay.clockCycles, sem, isInput);
    }
    for (std::size_t i = 0; i < delay.deltas; ++i) {
        prev = _makeDeltaProcess(
            newProcesses, delay.port, prev, delayInfos.view, delayInfos.reset, rv, i, delay.clockCycles,
            delay.halfClock, sem, isInput);
    }
    if (delay.deltas != 0 && delay.clockCycles == 0 && !delay.halfClock) {
        --delay.deltas;
    }
    delete rv;
    messageAssert(prev != nullptr, "unexpected manipulation result.", nullptr, nullptr);

    _fixOutputReset(newProcesses, prev, delayInfos.reset, isInput);

    // Updating references:
    for (auto *ref : refs) {
        hif::objectSetName(ref, prev->getName());
    }
}

void _insertNoDelay(
    Port *p,
    DelayInfos &delayInfos,
    RefSet &refs,
    hif::semantics::ILanguageSemantics *sem,
    const bool isInput)
{
    DelayProperties delay;
    delay.port = p;
    _insertTrueDelay(delayInfos, refs, delay, sem, isInput);
}

auto _insertDelay(DelayInfos &delayInfos, hif::semantics::ILanguageSemantics *sem) -> bool
{
    bool hasInputs  = false;
    bool hasOutputs = false;
    // calculating the input and output ports
    for (auto &delayPropertie : delayInfos.delayProperties) {
        DelayProperties &info = delayPropertie.second;
        switch (info.port->getDirection()) {
        case hif::dir_none:
            messageError("Wrong port direction.", info.port, sem);
        case hif::dir_in:
            hasInputs = true;
            break;
        case hif::dir_out:
            hasOutputs = true;
            break;
        case hif::dir_inout:
            messageWarning("Unsupported port direction.", info.port, sem);
            return false;
        default:
            messageError("Unexpected port direction.", info.port, sem);
        };
    }
    RefMap refMap;
    hif::semantics::getAllReferences(refMap, sem, delayInfos.view);

    // inserting delay on inputs
    if (hasInputs) {
        for (BList<Port>::iterator i = delayInfos.view->getEntity()->ports.begin();
             i != delayInfos.view->getEntity()->ports.end(); ++i) {
            Port *p = *i;
            if (p->getDirection() != hif::dir_in) {
                continue;
            }
            auto it = delayInfos.delayProperties.find(p);
            if (it == delayInfos.delayProperties.end()) {
                // Insert just one delta to keep coherence
                _insertNoDelay(p, delayInfos, refMap[p], sem, true);
            } else {
                // Insert specified delay plus one delta cycle in case of at
                // least one delta specified.
                _insertTrueDelay(delayInfos, refMap[p], it->second, sem, true);
            }
        }
    }
    if (hasOutputs) {
        for (BList<Port>::iterator i = delayInfos.view->getEntity()->ports.begin();
             i != delayInfos.view->getEntity()->ports.end(); ++i) {
            Port *p = *i;
            if (p->getDirection() != hif::dir_out) {
                continue;
            }
            auto it = delayInfos.delayProperties.find(p);
            if (it == delayInfos.delayProperties.end()) {
                // Insert just one delta to keep coherence
                _insertNoDelay(p, delayInfos, refMap[p], sem, false);
            } else {
                // Insert specified delay plus one delta cycle in case of at
                // least one delta specified.
                _insertTrueDelay(delayInfos, refMap[p], it->second, sem, false);
            }
        }
    }
    return true;
}
} // namespace
// /////////////////////////////////////
// DelayProperties
// /////////////////////////////////////

DelayProperties::DelayProperties()
    : port(nullptr)
    , clockCycles(0)
    , deltas(0)
    , halfClock(false)
{
    // ntd
}
DelayProperties::~DelayProperties()
{
    // ntd
}
DelayProperties::DelayProperties(const DelayProperties &other)
    : port(other.port)
    , clockCycles(other.clockCycles)
    , deltas(other.deltas)
    , halfClock(other.halfClock)
{
    // ntd
}

auto DelayProperties::operator=(const DelayProperties &other) -> DelayProperties &
{
    if (this == &other) {
        return *this;
    }
    port        = other.port;
    clockCycles = other.clockCycles;
    deltas      = other.deltas;
    halfClock   = other.halfClock;

    return *this;
}
// /////////////////////////////////////
// DelayInfos
// /////////////////////////////////////

DelayInfos::DelayInfos()
    : view(nullptr)
    , clock(nullptr)
    , reset(nullptr)
    , workingEdge(ProcessInfos::NO_EDGE)
    , resetPhase(ProcessInfos::NO_PHASE)

{
    // ntd
}
DelayInfos::~DelayInfos()
{
    // ntd
}
DelayInfos::DelayInfos(const DelayInfos &other)
    : view(other.view)
    , clock(other.clock)
    , reset(other.reset)
    , workingEdge(other.workingEdge)
    , resetPhase(other.resetPhase)
    , delayProperties(other.delayProperties)
{
    // ntd
}

auto DelayInfos::operator=(const DelayInfos &other) -> DelayInfos &
{
    if (this == &other) {
        return *this;
    }
    view            = other.view;
    clock           = other.clock;
    reset           = other.reset;
    workingEdge     = other.workingEdge;
    resetPhase      = other.resetPhase;
    delayProperties = other.delayProperties;

    return *this;
}
// /////////////////////////////////////
// insertDelay()
// /////////////////////////////////////

auto insertDelays(DelayList &delays, hif::semantics::ILanguageSemantics *sem) -> bool
{
    // 1- Split inputs and outputs. No inouts.
    // 2- For each class, add required delay.

    bool ret = true;
    for (auto &delay : delays) {
        ret &= _insertDelay(delay, sem);
    }
    return ret;
}

} // namespace analysis

} // namespace hif
