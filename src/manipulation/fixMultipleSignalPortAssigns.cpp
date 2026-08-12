/// @file fixMultipleSignalPortAssigns.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>
#include <list>
#include <map>
#include <vector>

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/fixMultipleSignalPortAssigns.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/search.hpp"
#include "hif/semantics/semantics.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#    pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#ifdef _MSC_VER
#    pragma warning(disable : 4996)
#endif
namespace hif
{
namespace manipulation
{

namespace /* anonymous */
{

typedef std::map<std::string, Signal *> Signals;
typedef std::map<DataDeclaration *, Signals> SupportSignals;
typedef std::set<DataDeclaration *> UpdatingProcesses;

SupportSignals _supportSignals1;
SupportSignals _supportSignals2;
UpdatingProcesses _updatingProcesses1;
UpdatingProcesses _updatingProcesses2;

// ///////////////////////////////////////////////////////////////////
// SubrangesVisitor
// ///////////////////////////////////////////////////////////////////
class SubrangesVisitor : public hif::GuideVisitor
{
public:
    SubrangesVisitor(hif::semantics::ILanguageSemantics *sem);
    ~SubrangesVisitor();

    bool isFixed();
    int visitView(View &o);
    int visitLibraryDef(LibraryDef &o);
    int visitStateTable(StateTable &o);

private:
    hif::semantics::ILanguageSemantics *_sem;
    hif::application_utils::WarningSet _warnings;
    hif::Trash _trash;
    bool _fixed;

    void _fixPrefix(Value *val);

    SubrangesVisitor(const SubrangesVisitor &);
    SubrangesVisitor &operator=(const SubrangesVisitor &);
};

SubrangesVisitor::SubrangesVisitor(hif::semantics::ILanguageSemantics *sem)
    : _sem(sem)
    , _warnings()
    , _trash()
    , _fixed(false)
{
}

SubrangesVisitor::~SubrangesVisitor()
{
    if (_fixed) {
        messageWarningList(
            true,
            "Found at least one member or slice inside a sensitivity list."
            " Replacing with full signal/port."
            " This could lead to non-equivalent simulation.",
            _warnings);
    }
}

bool SubrangesVisitor::isFixed() { return _fixed; }

int SubrangesVisitor::visitView(View &o)
{
    if (o.isStandard())
        return 0;
    return GuideVisitor::visitView(o);
}

int SubrangesVisitor::visitLibraryDef(LibraryDef &o)
{
    if (o.isStandard())
        return 0;
    return GuideVisitor::visitLibraryDef(o);
}

int SubrangesVisitor::visitStateTable(StateTable &o)
{
    for (BList<Value>::iterator it = o.sensitivity.begin(); it != o.sensitivity.end(); ++it) {
        _fixPrefix(*it);
    }
    for (BList<Value>::iterator it = o.sensitivityPos.begin(); it != o.sensitivityPos.end(); ++it) {
        _fixPrefix(*it);
    }
    for (BList<Value>::iterator it = o.sensitivityNeg.begin(); it != o.sensitivityNeg.end(); ++it) {
        _fixPrefix(*it);
    }

    return 0;
}

void SubrangesVisitor::_fixPrefix(Value *val)
{
    Member *mem  = dynamic_cast<Member *>(val);
    Slice *slice = dynamic_cast<Slice *>(val);
    if (mem == nullptr && slice == nullptr)
        return;

    Value *prefix    = (mem != nullptr) ? mem->getPrefix() : slice->getPrefix();
    Type *prefixType = hif::semantics::getBaseType(prefix, false, _sem);
    if (dynamic_cast<Array *>(prefixType) != nullptr)
        return;

    _fixed = true;
    _warnings.insert(val);
    TerminalPrefixOptions opts;
    opts.recurseIntoFieldRefs = false;
    Value *terminalPrefix     = hif::getTerminalPrefix(val, opts);
    val->replace(hif::copy(terminalPrefix));
    _trash.insert(val);
}
// ///////////////////////////////////////////////////////////////////
// CollectAssignmentVisitor
// ///////////////////////////////////////////////////////////////////
class CollectAssignmentVisitor : public hif::GuideVisitor
{
public:
    /// @name Traits.
    //{

    typedef std::list<Identifier *> Identifiers;

    // Brief: we use DataDeclaration to identify the target of the searched
    // port/signal assignment.
    // Each DataDeclaration is the key to a structure containing:
    // - a list of StateTables writing on that target
    // -- the list of Identifier referred to the target
    // Thus we know how many processes are writing on a signal/port, and how
    // many times each one is writing.
    //
    // Improve: collect all assignment targets and then discard the safe ones.

    /// @brief Target identifiers written in each process.
    typedef std::list<Identifier *> TargetList;

    /// @brief Collect all assign targets.
    struct AssignTargets {
        AssignTargets();
        ~AssignTargets();

        AssignTargets(const AssignTargets &other);
        const AssignTargets &operator=(const AssignTargets &other);

        TargetList targets;
    };

    /// @brief Map of processes writing on a signal/port and the target
    /// of the assignments they contain.
    typedef std::map<StateTable *, AssignTargets> WritingProcesses;

    /// @brief All the infos related to a signal/port.
    // K authorized
    struct AssignInfos {
        AssignInfos()
            : processes()
            , declaration(nullptr)
        {
        }

        ~AssignInfos() {}

        AssignInfos(const AssignInfos &o)
            : processes(o.processes)
            , declaration(o.declaration)
        {
        }

        AssignInfos &operator=(const AssignInfos &o)
        {
            if (this == &o)
                return *this;
            processes   = o.processes;
            declaration = o.declaration;
            return *this;
        }

        WritingProcesses processes;

        /// @brief Declaration of the signal/port.
        DataDeclaration *declaration;
    };

    /// @brief Map of the signal/port targets and all the infos related to them.
    typedef std::map<DataDeclaration *, AssignInfos> Targets;

    typedef std::set<DataDeclaration *> SignalSet;

    typedef std::map<StateTable *, SignalSet> Sensitivities;

    typedef std::list<Value *> ValueList;

    //}

    /// @brief Default constructor.
    CollectAssignmentVisitor(hif::semantics::ILanguageSemantics *sem);

    /// @brief Destructor.
    virtual ~CollectAssignmentVisitor();

    /// @brief Return the collected targets.
    Targets &getTargets();
    Targets &getInitialTargets();

    Sensitivities &getSensitivities();

    virtual int visitAssign(Assign &o);

    virtual int visitStateTable(StateTable &o);

    virtual int visitParameterAssign(ParameterAssign &o);

    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitProcedureCall(ProcedureCall &o);

    DataDeclaration *_getIdentifierDeclaration(Identifier *id, Value *v = nullptr);
    void _collectSensitivity(StateTable *o);
    void _fillInfos(Identifiers &list);

private:
    CollectAssignmentVisitor(const CollectAssignmentVisitor &);
    CollectAssignmentVisitor &operator=(const CollectAssignmentVisitor &);

    /// @brief Some symbols may have been collected erroneously. This method
    /// refines the given result to the ones that actually need the fix.
    void _refineCollectedSymbols(ValueList &symbols);

    hif::semantics::ILanguageSemantics *_sem;

    Targets _initialTargets;
    Targets _targets;
    Sensitivities _sensitivities;

    StateTable *_currentProcess;
};

CollectAssignmentVisitor::AssignTargets::AssignTargets()
    : targets()
{
    // ntd
}

CollectAssignmentVisitor::AssignTargets::~AssignTargets()
{
    // ntd
}

CollectAssignmentVisitor::AssignTargets::AssignTargets(const AssignTargets &other)
    : targets(other.targets)
{
    // ntd
}

const CollectAssignmentVisitor::AssignTargets &
CollectAssignmentVisitor::AssignTargets::operator=(const AssignTargets &other)
{
    if (this == &other)
        return *this;
    targets = other.targets;
    return *this;
}

CollectAssignmentVisitor::CollectAssignmentVisitor(hif::semantics::ILanguageSemantics *sem)
    : _sem(sem)
    , _initialTargets()
    , _targets()
    , _sensitivities()
    , _currentProcess(nullptr)
{
    // ntd
}

CollectAssignmentVisitor::~CollectAssignmentVisitor()
{
    // ntd
}

CollectAssignmentVisitor::Targets &CollectAssignmentVisitor::getTargets() { return _targets; }

CollectAssignmentVisitor::Targets &CollectAssignmentVisitor::getInitialTargets() { return _initialTargets; }

CollectAssignmentVisitor::Sensitivities &CollectAssignmentVisitor::getSensitivities() { return _sensitivities; }

DataDeclaration *CollectAssignmentVisitor::_getIdentifierDeclaration(Identifier *id, Value *v)
{
    DataDeclaration *deco = hif::semantics::getDeclaration(id, _sem);
    if (deco == nullptr) {
        messageError("Cannot find declaration.", id, _sem);
    }

    if (dynamic_cast<Port *>(deco) == nullptr && dynamic_cast<Signal *>(deco) == nullptr)
        return nullptr;

    Type *semT  = hif::semantics::getSemanticType(id, _sem);
    Type *baseT = hif::semantics::getBaseType(semT, false, _sem);
    Array *t1   = dynamic_cast<Array *>(baseT);
    Bit *t2     = dynamic_cast<Bit *>(baseT);
    Bool *t3    = dynamic_cast<Bool *>(baseT);
    Enum *t4    = dynamic_cast<Enum *>(baseT);
    Real *t5    = dynamic_cast<Real *>(baseT);
    if (t2 != nullptr || t3 != nullptr || t4 != nullptr || t5 != nullptr)
        return nullptr;

    if (v != nullptr && v != id && t1 != nullptr) {
        // Check whether the sensitivity is a simple member
        // of array. in this case no sensitivity fix is required.
        // Ref design: vhdl/unott/dig_proc + ddt
        Type *t = hif::semantics::getSemanticType(v, _sem);
        Type *b = hif::semantics::getBaseType(t, false, _sem);
        if (hif::equals(hif::typeGetNestedType(t1, _sem), b))
            return nullptr;
    }

    return deco;
}
void CollectAssignmentVisitor::_collectSensitivity(StateTable *o)
{
    for (BList<Value>::iterator i = o->sensitivity.begin(); i != o->sensitivity.end(); ++i) {
        if (dynamic_cast<Identifier *>(*i) != nullptr)
            continue;
        if (dynamic_cast<FunctionCall *>(*i) != nullptr)
            continue;
        Identifier *v = dynamic_cast<Identifier *>(hif::getTerminalPrefix(*i));
        messageAssert(v != nullptr, "Unexpected sensitivity.", *i, _sem);

        DataDeclaration *deco = _getIdentifierDeclaration(v, *i);
        if (deco == nullptr)
            continue;

        _sensitivities[o].insert(deco);
    }

    for (BList<Value>::iterator i = o->sensitivityNeg.begin(); i != o->sensitivityNeg.end(); ++i) {
        if (dynamic_cast<Identifier *>(*i) != nullptr)
            continue;
        Identifier *v = dynamic_cast<Identifier *>(hif::getTerminalPrefix(*i));
        messageAssert(v != nullptr, "Unexpected sensitivity.", v, _sem);

        DataDeclaration *deco = _getIdentifierDeclaration(v, *i);
        if (deco == nullptr)
            continue;

        _sensitivities[o].insert(deco);
    }

    for (BList<Value>::iterator i = o->sensitivityPos.begin(); i != o->sensitivityPos.end(); ++i) {
        if (dynamic_cast<Identifier *>(*i) != nullptr)
            continue;
        Identifier *v = dynamic_cast<Identifier *>(hif::getTerminalPrefix(*i));
        messageAssert(v != nullptr, "Unexpected sensitivity.", v, _sem);

        DataDeclaration *deco = _getIdentifierDeclaration(v, *i);
        if (deco == nullptr)
            continue;

        _sensitivities[o].insert(deco);
    }
}

void CollectAssignmentVisitor::_fillInfos(CollectAssignmentVisitor::Identifiers &list)
{
    messageAssert(_currentProcess != nullptr, "Current process not set", nullptr, _sem);

    // Search for interesting cases.
    for (std::list<Identifier *>::iterator it = list.begin(); it != list.end(); ++it) {
        DataDeclaration *deco = hif::semantics::getDeclaration(*it, _sem);

        if (_currentProcess->getFlavour() == hif::pf_initial) {
            _initialTargets[deco].declaration = deco;
            _initialTargets[deco].processes[_currentProcess].targets.push_back(*it);
        } else {
            _targets[deco].declaration = deco;
            _targets[deco].processes[_currentProcess].targets.push_back(*it);
        }
    }
}

int CollectAssignmentVisitor::visitAssign(Assign &o)
{
    // Store the identifier until we don't know where to put it.
    hif::manipulation::LeftHandSideOptions opt;
    opt.sem            = _sem;
    opt.considerRecord = true;
    ValueList symbols  = hif::manipulation::collectLeftHandSideSymbols(&o, opt);
    // Refine the collected symbols.
    _refineCollectedSymbols(symbols);

    bool toCheck = false;
    Identifiers list;
    for (std::list<Value *>::iterator it = symbols.begin(); it != symbols.end(); ++it) {
        Identifier *sName = dynamic_cast<Identifier *>(*it);
        messageAssert(sName != nullptr, "Unexpected object", *it, _sem);

        DataDeclaration *deco = _getIdentifierDeclaration(sName);
        if (deco == nullptr)
            continue;

        list.push_back(sName);
        toCheck = true;
    }

    if (!toCheck)
        return 0;

    _fillInfos(list);

    return 0;
}

int CollectAssignmentVisitor::visitStateTable(StateTable &o)
{
    // Skip Function/Procedure StateTables.
    bool hasCurrentProcess = (_currentProcess != nullptr);
    if (dynamic_cast<hif::BaseContents *>(o.getParent()) == nullptr && !hasCurrentProcess)
        return 0;

    StateTable *restore = _currentProcess;
    if (!hasCurrentProcess)
        _currentProcess = &o;

    GuideVisitor::visitStateTable(o);

    _collectSensitivity(&o);

    if (!hasCurrentProcess)
        _currentProcess = restore;

    return 0;
}

int CollectAssignmentVisitor::visitParameterAssign(ParameterAssign &o)
{
    GuideVisitor::visitParameterAssign(o);

    ParameterAssign::DeclarationType *param = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(param != nullptr, "Declaration not found", &o, _sem);

    // Cannot pass as reference to methods signals and ports.
    if (param->getDirection() != dir_out && param->getDirection() != dir_inout)
        return 0;

    hif::TerminalPrefixOptions opt;
    opt.recurseIntoFieldRefs = true;
    opt.recurseIntoMembers   = true;
    opt.recurseIntoSlices    = true;
    Value *v                 = hif::getChildSkippingCasts(o.getValue()); // ref design: vhdl/openCores/plasma
    v                        = hif::getTerminalPrefix(v, opt);
    Identifier *id           = dynamic_cast<Identifier *>(v);
    messageAssert(id != nullptr, "Unsupported case", o.getValue(), _sem);

    DataDeclaration *decl = _getIdentifierDeclaration(id);
    if (decl == nullptr)
        return 0;

    // Assuming out and inout ports/signals are write by method.

    Identifiers list;
    list.push_back(id);
    _fillInfos(list);

    return 0;
}

int CollectAssignmentVisitor::visitFunctionCall(FunctionCall &o)
{
    GuideVisitor::visitFunctionCall(o);

    FunctionCall::DeclarationType *d = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(d != nullptr, "Declaration not found", &o, _sem);

    d->acceptVisitor(*this);

    return 0;
}

int CollectAssignmentVisitor::visitProcedureCall(ProcedureCall &o)
{
    GuideVisitor::visitProcedureCall(o);

    ProcedureCall::DeclarationType *d = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(d != nullptr, "Declaration not found", &o, _sem);

    d->acceptVisitor(*this);

    return 0;
}

void CollectAssignmentVisitor::_refineCollectedSymbols(ValueList &symbols)
{
    // RTL fieldreference can never be referred to signals.
    for (ValueList::iterator it(symbols.begin()); it != symbols.end();) {
        FieldReference *fr = dynamic_cast<FieldReference *>(*it);
        if (fr != nullptr) {
            it = symbols.erase(it);
            continue;
        }

        ++it;
    }

    // step1: heuristic.
    // Remove the symbols ending with "_mspw" suffix. Reference design is
    // every design involving two or more calls to the fix (e.g., ddt + hif2sc).
    for (ValueList::iterator it(symbols.begin()); it != symbols.end();) {
        Identifier *id = dynamic_cast<Identifier *>(*it);
        if (id == nullptr) {
            ++it;
            continue;
        }

        std::string idName(id->getName());
        if (idName.find("_mspw") != std::string::npos) {
            it = symbols.erase(it);
            continue;
        }

        ++it;
    }

    // step2: recognize disjoint indexes of members
    // TODO

    // step3: recognize disjoint ranges of slices
    // TODO
}
// /////////////////////////////////////////////////////////////////////
// Actual fixes.
// /////////////////////////////////////////////////////////////////////

Type *_getUnpackedType(Type *source, hif::semantics::ILanguageSemantics *sem)
{
    Type *baseT = hif::semantics::getBaseType(source, false, sem);

    Array *t1     = dynamic_cast<Array *>(baseT);
    Signed *t2    = dynamic_cast<Signed *>(baseT);
    Unsigned *t3  = dynamic_cast<Unsigned *>(baseT);
    Bitvector *t4 = dynamic_cast<Bitvector *>(baseT);
    Int *t5       = dynamic_cast<Int *>(baseT);

    if (t1 != nullptr) {
        hif::CopyOptions opt;
        opt.copyChildObjects = false;

        Array *ret = hif::copy(t1, opt);        // without children!
        ret->setSpan(hif::copy(t1->getSpan())); // with children!
        ret->setType(_getUnpackedType(t1->getType(), sem));
        return ret;
    } else if (t2 != nullptr) {
        Bit *b = new Bit();
        b->setLogic(true);
        b->setResolved(true);

        Array *ret = new Array();
        ret->setSpan(hif::copy(t2->getSpan()));
        ret->setSigned(true);
        ret->setType(b);
        return ret;
    } else if (t3 != nullptr) {
        Bit *b = new Bit();
        b->setLogic(true);
        b->setResolved(true);

        Array *ret = new Array();
        ret->setSpan(hif::copy(t3->getSpan()));
        ret->setSigned(false);
        ret->setType(b);
        return ret;
    } else if (t4 != nullptr) {
        Bit *b = new Bit();
        b->setLogic(t4->isLogic());
        b->setResolved(t4->isResolved());

        Array *ret = new Array();
        ret->setSpan(hif::copy(t4->getSpan()));
        ret->setSigned(t4->isSigned());
        ret->setType(b);
        return ret;
    } else if (t5 != nullptr) {
        Bool *b    = new Bool();
        Array *ret = new Array();
        ret->setSpan(hif::copy(t5->getSpan()));
        ret->setSigned(t5->isSigned());
        ret->setType(b);
        return ret;
    }

    return hif::copy(source);
}
void _addEventualCast(Assign *supportAssign, hif::semantics::ILanguageSemantics *sem)
{
    Type *tgtType = hif::semantics::getSemanticType(supportAssign->getLeftHandSide(), sem);
    Type *srcType = hif::semantics::getSemanticType(supportAssign->getRightHandSide(), sem);
    messageAssert(tgtType != nullptr, "Target type not found", supportAssign->getLeftHandSide(), sem);
    messageAssert(srcType != nullptr, "Source type not found", supportAssign->getRightHandSide(), sem);
    if (hif::equals(tgtType, srcType))
        return;

    hif::Cast *co = new Cast();
    co->setType(hif::copy(tgtType));
    co->setValue(supportAssign->getRightHandSide());
    supportAssign->setRightHandSide(co);

    hif::manipulation::simplify(supportAssign, sem);
}
bool _mspwCreateSupportSignals(
    DataDeclaration *decl,
    Signals &supportSignals,
    hif::semantics::ILanguageSemantics *sem,
    bool isSrc)
{
    Type *baseType = hif::semantics::getBaseType(decl->getType(), false, sem);
    Record *rec    = dynamic_cast<Record *>(baseType);

    // A mspw can be created two times for same signal, if both read and written
    // in a unfortunate way!
    // Ref design: verilog/openCores/can_top
    SupportSignals &_supportSignals = isSrc ? _supportSignals1 : _supportSignals2;
    SupportSignals::iterator it     = _supportSignals.find(decl);
    if (it != _supportSignals.end()) {
        // Signal already created!
        // Ref design: vhdl/unott/dig_proc + ddt
        supportSignals = it->second;
        return rec != nullptr;
    }

    if (rec != nullptr) {
        for (BList<Field>::iterator i = rec->fields.begin(); i != rec->fields.end(); ++i) {
            Field *f                     = *i;
            Signal *supportSig           = new Signal();
            supportSignals[f->getName()] = supportSig;
            supportSig->setType(hif::copy(f->getType()));
            supportSig->setName(hif::NameTable::getInstance()->getFreshName(
                (decl->getName() + std::string("_") + f->getName() + std::string("_mspw")).c_str()));
            if (decl->getValue() == nullptr) {
                Value *v = sem->getTypeDefaultValue(supportSig->getType(), decl);
                supportSig->setValue(v);
            } else {
                // Cast is needed since record values may contain aggregate with others.
                // Ref. design: vhdl/gaisler/can_oc
                FieldReference *fr = new FieldReference();
                Cast *c            = new Cast();
                c->setValue(hif::copy(decl->getValue()));
                c->setType(hif::copy(decl->getType()));
                fr->setPrefix(c);
                fr->setName(f->getName());

                supportSig->setValue(fr);
            }

            hif::manipulation::addDeclarationInContext(supportSig, decl);
        }

        _supportSignals[decl] = supportSignals;
        return true;
    }

    Signal *supportSig              = new Signal();
    supportSignals[decl->getName()] = supportSig;
    supportSig->setType(_getUnpackedType(decl->getType(), sem));
    supportSig->setName(hif::NameTable::getInstance()->getFreshName((decl->getName() + std::string("_mspw")).c_str()));
    if (decl->getValue() == nullptr) {
        Value *v = sem->getTypeDefaultValue(supportSig->getType(), decl);
        supportSig->setValue(v);
    } else {
        Value *unpackedInitVal = hif::manipulation::transformValue(decl->getValue(), supportSig->getType(), sem);
        if (unpackedInitVal != nullptr) {
            supportSig->setValue(unpackedInitVal);
        } else {
            supportSig->setValue(sem->getTypeDefaultValue(supportSig->getType(), decl));
        }
    }
    hif::manipulation::addDeclarationInContext(supportSig, decl);
    _supportSignals[decl] = supportSignals;
    return false;
}

void _mspwCreateUpdatingProcess(
    DataDeclaration *decl,
    Signals &supportSignals,
    hif::BaseContents *scope,
    bool isRecord,
    hif::semantics::ILanguageSemantics *sem)
{
    UpdatingProcesses::iterator it = _updatingProcesses1.find(decl);
    if (it != _updatingProcesses1.end()) {
        // Already created!
        // Ref design: vhdl/unott/dig_proc + ddt
        return;
    }
    _updatingProcesses1.insert(decl);

    StateTable *supportST = new StateTable();
    supportST->setName(
        hif::NameTable::getInstance()->getFreshName((decl->getName() + std::string("_mspw_proc_tgt")).c_str()));

    for (Signals::iterator i = supportSignals.begin(); i != supportSignals.end(); ++i) {
        Signal *supportSig = i->second;
        supportST->sensitivity.push_back(new Identifier(supportSig->getName()));
    }

    State *state = new State();
    state->setName(supportST->getName());
    supportST->states.push_back(state);
    scope->stateTables.push_back(supportST);
    Assign *supportAssign = new Assign();
    supportAssign->setLeftHandSide(new Identifier(decl->getName()));
    if (isRecord) {
        Type *baseType = hif::semantics::getBaseType(decl->getType(), false, sem);
        Record *rec    = dynamic_cast<Record *>(baseType);
        messageAssert(rec != nullptr, "Expected record", baseType, sem);
        RecordValue *rv = new RecordValue();
        for (BList<Field>::iterator i = rec->fields.begin(); i != rec->fields.end(); ++i) {
            Field *f           = *i;
            std::string n      = f->getName();
            Signal *supportSig = supportSignals[n];
            messageAssert(supportSig != nullptr, "Expected related signal", f, sem);
            RecordValueAlt *rva = new RecordValueAlt();
            rva->setName(n);
            rva->setValue(new Identifier(supportSig->getName()));
            rv->alts.push_back(rva);
        }

        supportAssign->setRightHandSide(rv);
    } else {
        Signal *supportSig = supportSignals.begin()->second;
        supportAssign->setRightHandSide(new Identifier(supportSig->getName()));
    }
    state->actions.push_front(supportAssign);
    _addEventualCast(supportAssign, sem);
}

void _mspwCreateUpdatingProcess(
    Signals &supportSignals,
    DataDeclaration *decl,
    hif::BaseContents *scope,
    bool isRecord,
    hif::semantics::ILanguageSemantics *sem)
{
    UpdatingProcesses::iterator it = _updatingProcesses2.find(decl);
    if (it != _updatingProcesses2.end()) {
        // Already created!
        // Ref design: vhdl/unott/dig_proc + ddt
        return;
    }
    _updatingProcesses2.insert(decl);

    StateTable *supportST = new StateTable();
    supportST->setName(
        hif::NameTable::getInstance()->getFreshName((decl->getName() + std::string("_mspw_proc_src")).c_str()));

    supportST->sensitivity.push_back(new Identifier(decl->getName()));
    State *state = new State();
    state->setName(supportST->getName());
    supportST->states.push_back(state);
    scope->stateTables.push_back(supportST);
    if (isRecord) {
        Type *baseType = hif::semantics::getBaseType(decl->getType(), false, sem);
        Record *rec    = dynamic_cast<Record *>(baseType);
        messageAssert(rec != nullptr, "Expected record", baseType, sem);
        for (BList<Field>::iterator i = rec->fields.begin(); i != rec->fields.end(); ++i) {
            Field *f           = *i;
            std::string n      = f->getName();
            Signal *supportSig = supportSignals[n];
            messageAssert(supportSig != nullptr, "Expected related signal", f, sem);
            Assign *supportAssign = new Assign();
            supportAssign->setLeftHandSide(new Identifier(supportSig->getName()));
            FieldReference *fr = new FieldReference();
            fr->setPrefix(new Identifier(decl->getName()));
            fr->setName(n);
            supportAssign->setRightHandSide(fr);
            state->actions.push_front(supportAssign);
            _addEventualCast(supportAssign, sem);
        }
    } else {
        Signal *supportSig    = supportSignals.begin()->second;
        Assign *supportAssign = new Assign();
        supportAssign->setLeftHandSide(new Identifier(supportSig->getName()));
        supportAssign->setRightHandSide(new Identifier(decl->getName()));
        state->actions.push_front(supportAssign);
        _addEventualCast(supportAssign, sem);
    }
}
bool _fixSensitivityProcesses(
    CollectAssignmentVisitor::Sensitivities &sensitivities,
    CollectAssignmentVisitor::Targets &targets,
    hif::semantics::ILanguageSemantics *sem)
{
    // 0- check whether interesting declarations have refs used inside concat
    // expressions. In this case a refine must be performed adding a support variable
    // to avoid possible problems due to concatenation of bit-arrays and bitvectors.
    // 1- creating mspw support signal
    // 2- renaming references inside current process
    // 3- creating support process to read original signal
    // 4- update data structures

    // 0
    typedef std::set<Object *> References;
    for (CollectAssignmentVisitor::Sensitivities::iterator i = sensitivities.begin(); i != sensitivities.end(); ++i) {
        StateTable *process = i->first;
        for (CollectAssignmentVisitor::SignalSet::iterator j = i->second.begin(); j != i->second.end(); ++j) {
            DataDeclaration *deco = *j;

            hif::semantics::ReferencesSet list;
            hif::semantics::getReferences(deco, list, sem, process);

            bool findBadLocation = false;
            References references;

            for (hif::semantics::ReferencesSet::iterator k = list.begin(); k != list.end(); ++k) {
                Object *ref = *k;
                Expression *e =
                    dynamic_cast<Expression *>(hif::getParentSkippingClass<PrefixedReference>(ref->getParent()));
                if (e == nullptr)
                    continue;

                messageAssert(!hif::manipulation::isInLeftHandSide(ref), "Unsupported case", ref, sem);

                findBadLocation = true;
                references.insert(ref);
            }

            if (!findBadLocation)
                continue;

            std::string varName  = NameTable::getInstance()->getFreshName(deco->getName(), "_expr_var");
            Variable *supportVar = new Variable();
            supportVar->setName(varName);
            supportVar->setType(hif::copy(deco->getType()));
            process->declarations.push_front(supportVar);

            if (deco->getValue() != nullptr) {
                supportVar->setValue(hif::copy(deco->getValue()));
            } else {
                Value *v = sem->getTypeDefaultValue(supportVar->getType(), supportVar);
                supportVar->setValue(v);
            }

            for (References::iterator k = references.begin(); k != references.end(); ++k) {
                Object *ref = *k;

                // renaming the refenrece to support var name
                hif::objectSetName(ref, varName);
                // hif::semantics::resetTypes(ref, false); // same type
                hif::semantics::setDeclaration(ref, supportVar);

                // getting parent action
                Action *act = hif::getNearestParent<Action>(ref);
                messageAssert(act != nullptr, "Cannot find parent action", ref, sem);

                // creating update assign
                Identifier *varId = new Identifier(varName);
                hif::semantics::setDeclaration(varId, supportVar);
                Identifier *declId = new Identifier(deco->getName());
                hif::semantics::setDeclaration(declId, deco);
                Assign *ass = new Assign();
                ass->setLeftHandSide(varId);
                ass->setRightHandSide(declId);

                // add assign before reading
                BList<Action>::iterator it(act);
                it.insert_before(ass);

                // infos are already up to date since declarations are set.
            }
        }
    }

    for (CollectAssignmentVisitor::Sensitivities::iterator i = sensitivities.begin(); i != sensitivities.end(); ++i) {
        // A slice or member of a signal is inside a sensitivity.
        // Create a mspw.
        StateTable *process = i->first;

        for (CollectAssignmentVisitor::SignalSet::iterator j = i->second.begin(); j != i->second.end(); ++j) {
            DataDeclaration *deco = *j;

            // 1
            Signals supportSignals;
            bool isRecord = _mspwCreateSupportSignals(deco, supportSignals, sem, true);
            messageAssert(!isRecord, "Record signals in sensitiyity is not supported yet!", deco, sem);
            Signal *supportSignal              = supportSignals.begin()->second;
            targets[supportSignal].declaration = supportSignal;

            // 2
            hif::semantics::ReferencesSet list;
            hif::semantics::getReferences(deco, list, sem, process);

            // Check whether the signal appears within at least one LHS of an assignment
            // (i.e., the signal is actually written)
            bool isTarget = false;
            hif::manipulation::LeftHandSideOptions lhsOpts;
            lhsOpts.sem = sem;
            for (hif::semantics::ReferencesSet::iterator k = list.begin(); k != list.end(); ++k) {
                // renaming
                Identifier *iidd = dynamic_cast<Identifier *>(*k);
                messageAssert(iidd != nullptr, "Unexpected reference.", *k, sem);

                if (!hif::manipulation::isInLeftHandSide(iidd, lhsOpts))
                    continue;
                isTarget = true;
                break;
            }

            for (hif::semantics::ReferencesSet::iterator k = list.begin(); k != list.end(); ++k) {
                // renaming
                Identifier *iidd = dynamic_cast<Identifier *>(*k);
                messageAssert(iidd != nullptr, "Unexpected reference.", *k, sem);

                // If the occurence is the prefix of an event call it must be fixed.
                // Reference design: vhdl/unott/dig_proc.
                FunctionCall *parentFCall = hif::getNearestParent<FunctionCall>(iidd);
                bool isPrefixOfEventCall =
                    (parentFCall != nullptr && sem->isEventCall(parentFCall) &&
                     hif::isSubNode(iidd, parentFCall->getInstance()));

                // If the occurrence is within the process code, but the signal is not
                // written by the process, do not rename it with the support variable.
                // Reference design: can_top (verilog/openCores) + ddt.
                if (!isTarget && !isPrefixOfEventCall && hif::isSubNode(iidd, process->states.front())) {
                    continue;
                }
                iidd->setName(supportSignal->getName());
                hif::semantics::setDeclaration(iidd, supportSignal);
                hif::semantics::resetTypes(iidd);

                Assign *ass = hif::getNearestParent<Assign>(iidd);
                if (ass == nullptr)
                    continue;
                hif::semantics::resetTypes(ass);
                _addEventualCast(ass, sem);
            }

            // 3
            Contents *scope = hif::getNearestParent<Contents>(supportSignal);
            _mspwCreateUpdatingProcess(supportSignals, deco, scope, isRecord, sem);

            // 4
            targets[supportSignal].processes[process].targets.splice(
                targets[supportSignal].processes[process].targets.end(), targets[deco].processes[process].targets);

            targets[deco].processes.erase(process);
        }
    }

    return true;
}
bool _fixBetweenProcesses(CollectAssignmentVisitor::Targets &targets, hif::semantics::ILanguageSemantics *sem)
{
    bool hasFix = false;

    CollectAssignmentVisitor::Targets supportMap;
    hif::application_utils::WarningSet warnings;

    for (CollectAssignmentVisitor::Targets::iterator tgt(targets.begin()); tgt != targets.end(); ++tgt) {
        CollectAssignmentVisitor::WritingProcesses &processes = (*tgt).second.processes;
        if (processes.size() <= 1)
            continue;

        hasFix = true;

        // Do fix:
        // 1- create a support signal <signal>_mspw
        // -- in the same scope of <signal>
        // -- with a type which is the unpacked version of <signal> type
        // 2- replace each assignment to <signal> with assign to <signal>_mspw
        // -- add also a cast to the source of the assign
        // -- previous assignment targets have been changed, thus add them
        //      in a support map with key = declaration of <signal>_mspw
        // 3- create a process sensitive to <signal>_mspw that assigns
        //      <signal>_mspw to <signal>
        // 4- updating data structure: merge the support map with the
        //      previous one

        DataDeclaration *decl = (*tgt).second.declaration;
        messageAssert(decl != nullptr, "Declaration not set", nullptr, sem);

        // Can be a mspw signal.
        // In this case should already be bit-splitted, so just skip:
        // TODO: do a better check!
        // Ref design: vhdl/unott/dig_proc
        {
            std::string idName(decl->getName());
            if (idName.find("_mspw") != std::string::npos)
                continue;
        }

        warnings.insert(decl);

        // 1
        Signals supportSignals;
        bool isRecord = _mspwCreateSupportSignals(decl, supportSignals, sem, false);

        for (Signals::iterator supportIter = supportSignals.begin(); supportIter != supportSignals.end();
             ++supportIter) {
            Signal *supportSig              = supportIter->second;
            targets[supportSig].declaration = supportSig;
        }

        // 2
        for (CollectAssignmentVisitor::WritingProcesses::iterator tgtProc(processes.begin());
             tgtProc != processes.end(); ++tgtProc) {
            CollectAssignmentVisitor::AssignTargets &assigns = (*tgtProc).second;

            // Replace targets ones.
            for (CollectAssignmentVisitor::TargetList::iterator prevTgts(assigns.targets.begin());
                 prevTgts != assigns.targets.end(); prevTgts = assigns.targets.erase(prevTgts)) {
                Identifier *target = *prevTgts;
                Signal *supportSig = nullptr;
                FieldReference *fr = dynamic_cast<FieldReference *>(target->getParent());
                if (fr != nullptr && supportSignals.find(fr->getName()) != supportSignals.end()) {
                    supportSig = supportSignals[fr->getName()];
                    target->replace(nullptr);
                    target->setName(supportSig->getName());
                    fr->replace(target);
                    delete fr;

                } else {
                    messageAssert(
                        supportSignals.find(target->getName()) != supportSignals.end(),
                        "Expected support signal in map", target, sem);
                    supportSig = supportSignals[target->getName()];
                    target->setName(supportSig->getName());
                }
                hif::semantics::setDeclaration(target, supportSig);
                hif::semantics::resetTypes(target);

                Assign *currAssign = hif::getNearestParent<Assign>(target);
                messageAssert(currAssign != nullptr, "Current Assign not set (1)", nullptr, sem);
                hif::semantics::resetTypes(currAssign->getLeftHandSide());
                _addEventualCast(currAssign, sem);

                // Updating collected infos by moving old identifiers (which have
                // just been renamed) to the exact position which matches the new name.
                CollectAssignmentVisitor::TargetList &newList =
                    supportMap[supportSig].processes[tgtProc->first].targets;
                newList.push_back(target);
            }
        }

        // 3
        Contents *scope = hif::getNearestParent<Contents>(supportSignals.begin()->second);
        _mspwCreateUpdatingProcess(decl, supportSignals, scope, isRecord, sem);

        // 4
        targets.insert(supportMap.begin(), supportMap.end());
    }

    messageWarningList(
        true,
        "Found at least one signal (partially) written by multiple processes."
        " Adding intermediate support signals to allow partial writes."
        " This introduces a delta-cycle, that could lead to non-equivalent simulation.",
        warnings);

    return hasFix;
}

// ///////////////////////////////////////////////////////////////////
// _fixPartialWritings
// ///////////////////////////////////////////////////////////////////
bool refCollectMethod(
    Object *o,
    hif::semantics::ILanguageSemantics *sem,
    const hif::semantics::GetReferencesOptions & /*opt*/)
{
    if (dynamic_cast<Declaration *>(o) != nullptr) {
        // declaration: collect only sig and port.
        Signal *sig = dynamic_cast<Signal *>(o);
        Port *port  = dynamic_cast<Port *>(o);
        return (sig != nullptr || port != nullptr);
    }

    // symbol: collect only partial sig/port targets
    Declaration *decl = hif::semantics::getDeclaration(o, sem);
    messageAssert(decl != nullptr, "Declaration not found", o, sem);

    Signal *sig = dynamic_cast<Signal *>(decl);
    Port *port  = dynamic_cast<Port *>(decl);
    if (sig == nullptr && port == nullptr)
        return false;

    hif::manipulation::LeftHandSideOptions opt;
    opt.sem            = sem;
    opt.considerRecord = true;
    if (!hif::manipulation::isInLeftHandSide(o, opt))
        return false;

    Value *to = dynamic_cast<Value *>(o);
    if (to == nullptr)
        return false;

    Value *tgt       = dynamic_cast<Value *>(to->getParent());
    Type *type2check = nullptr;
    while (tgt != nullptr) {
        // It is partial when its type is not an array!
        Member *member = dynamic_cast<Member *>(tgt);
        if (member != nullptr) {
            Type *prefixType =
                hif::semantics::getBaseType(hif::semantics::getSemanticType(member->getPrefix(), sem), false, sem);
            if (dynamic_cast<Array *>(prefixType) == nullptr) {
                type2check = prefixType;
                break;
            }
        }

        Slice *slice = dynamic_cast<Slice *>(tgt);
        if (slice != nullptr) {
            Type *prefixType =
                hif::semantics::getBaseType(hif::semantics::getSemanticType(slice->getPrefix(), sem), false, sem);
            if (dynamic_cast<Array *>(prefixType) == nullptr) {
                type2check = prefixType;
                break;
            }
        }

        // It is partial when its type is a record!
        hif::FieldReference *fr = dynamic_cast<hif::FieldReference *>(tgt);
        if (fr != nullptr) {
            Type *prefixType =
                hif::semantics::getBaseType(hif::semantics::getSemanticType(fr->getPrefix(), sem), false, sem);
            if (dynamic_cast<Record *>(prefixType) != nullptr) {
                type2check = prefixType;
                break;
            }
        }

        tgt = dynamic_cast<Value *>(tgt->getParent());
    }

    // it is for sure a total write
    if (type2check == nullptr)
        return false;

    Type *tgtType = hif::semantics::getBaseType(
        hif::semantics::getSemanticType(hif::manipulation::getLeftHandSide(to), sem), false, sem);

    // I'm writing on its whole length?
    if (hif::equals(type2check, tgtType))
        return false;

    return true;
}

// ///////////////////////////////////////////////////////////////////
// fix methods
// ///////////////////////////////////////////////////////////////////
typedef std::set<StateTable *> StateTableSet;
typedef std::map<DataDeclaration *, StateTableSet> PartialWritesMap;

bool _fixSubrangesInSensitivity(System *o, hif::semantics::ILanguageSemantics *sem)
{
    SubrangesVisitor vis(sem);
    o->acceptVisitor(vis);
    return vis.isFixed();
}

bool _fixMultipleWritings(System *o, hif::semantics::ILanguageSemantics *sem)
{
    CollectAssignmentVisitor collectVis(sem);
    o->acceptVisitor(collectVis);

    CollectAssignmentVisitor::Targets targets             = collectVis.getTargets();
    //CollectAssignmentVisitor::Targets initialTargets = collectVis.getInitialTargets();
    CollectAssignmentVisitor::Sensitivities sensitivities = collectVis.getSensitivities();
    bool ret                                              = false;

    // Note: do not change order of the following fixes. In the case of more process
    // that write on a portion of a vector, this would lead to inserting a new
    // variable in each process, but the final multiple writings <signal> <= <signal>_var
    // will cause simulation error (both are writing the entire signal).

    // Fix on processes with sensitivity with member or slice
    ret |= _fixSensitivityProcesses(sensitivities, targets, sem);

    // Fix between processes that perform multiple writes on a signal/port.
    ret |= _fixBetweenProcesses(targets, sem);

    return ret;
}

bool collectMethod(Object *o, const HifQueryBase *q)
{
    if (dynamic_cast<Wait *>(o) != nullptr) {
        return true;
    } else if (dynamic_cast<Return *>(o) != nullptr) {
        return true;
    }

    Declaration *d = nullptr;
    if (dynamic_cast<FunctionCall *>(o) != nullptr) {
        FunctionCall *fc = static_cast<FunctionCall *>(o);
        d                = hif::semantics::getDeclaration(fc, q->sem);
    } else if (dynamic_cast<ProcedureCall *>(o) != nullptr) {
        ProcedureCall *pc = static_cast<ProcedureCall *>(o);
        d                 = hif::semantics::getDeclaration(pc, q->sem);
    } else {
        return false;
    }
    messageAssert(d != nullptr, "Declaration not found", o, q->sem);

    hif::HifTypedQuery<Wait> qRec;
    qRec.checkInsideCallsDeclarations = true;
    qRec.onlyFirstMatch               = true;
    qRec.sem                          = q->sem;
    std::list<Wait *> list;
    hif::search(list, d, qRec);

    return !list.empty();
}

void _addInitialAssign(StateTable *currentSt, Assign *firstAssign, hif::semantics::ILanguageSemantics *sem)
{
    hif::HifTypedQuery<hif::Wait> q1;
    hif::HifTypedQuery<hif::ProcedureCall> q2;
    hif::HifTypedQuery<FunctionCall> q3;

    q1.setNextQueryType(&q2);
    q2.setNextQueryType(&q3);

    q1.sem                 = sem;
    q1.check_object_method = &collectMethod;

    std::list<hif::Object *> list;
    hif::search(list, currentSt, q1);

    bool isProcess = (dynamic_cast<BaseContents *>(currentSt->getParent()) != nullptr);
    if (list.empty() && isProcess) {
        currentSt->states.front()->actions.push_front(firstAssign);
    }

    for (std::list<hif::Object *>::iterator i = list.begin(); i != list.end(); ++i) {
        hif::Object *obj = *i;
        Action *pAct     = hif::getNearestParent<Action>(obj, true);
        messageAssert(pAct != nullptr, "Unexpected ref", obj, nullptr);
        messageAssert(pAct->isInBList(), "Unexpected ref position", pAct, nullptr);

        BList<Action>::iterator it(pAct);
        Assign *a = hif::copy(firstAssign);
        it.insert_after(a);
    }
}

void _addSignalAssign(StateTable *currentSt, Assign *lastAssign, hif::semantics::ILanguageSemantics *sem)
{
    hif::HifTypedQuery<hif::Wait> q1;
    hif::HifTypedQuery<hif::ProcedureCall> q2;
    hif::HifTypedQuery<FunctionCall> q3;
    hif::HifTypedQuery<hif::Return> q4;
    // TODO: consider disable?

    q1.setNextQueryType(&q2);
    q2.setNextQueryType(&q3);
    q3.setNextQueryType(&q4);

    q1.sem                 = sem;
    q1.check_object_method = &collectMethod;

    std::list<hif::Object *> list;
    hif::search(list, currentSt, q1);

    bool isProcess = (dynamic_cast<BaseContents *>(currentSt->getParent()) != nullptr);
    if (list.empty() && isProcess) {
        currentSt->states.front()->actions.push_back(lastAssign);
    }

    for (std::list<hif::Object *>::iterator i = list.begin(); i != list.end(); ++i) {
        Object *o = *i;

        Action *act = hif::getNearestParent<Action>(o, true);
        messageAssert(act->isInBList(), "Unexpected bad object position", act, nullptr);

        BList<Action>::iterator it(act);
        Assign *a = hif::copy(lastAssign);
        it.insert_before(a);
    }
}

void _replaceTargets(DataDeclaration *decl, StateTable *st, Variable *var, hif::semantics::ILanguageSemantics *sem)
{
    hif::semantics::ReferencesSet refSet;
    hif::semantics::getReferences(decl, refSet, sem, st);
    for (hif::semantics::ReferencesSet::iterator i = refSet.begin(); i != refSet.end(); ++i) {
        Object *ref = *i;
        hif::manipulation::LeftHandSideOptions opt;
        opt.sem            = sem;
        opt.considerRecord = true;
        if (!hif::manipulation::isInLeftHandSide(ref, opt))
            continue;

        Identifier *id = dynamic_cast<Identifier *>(ref);
        messageAssert(id != nullptr, "Unexpected target type", ref, sem);

        id->setName(var->getName());
        hif::semantics::setDeclaration(id, var);
    }
}

bool _fixPartialWritings(System *o, hif::semantics::ILanguageSemantics *sem)
{
    bool ret = false;

    hif::semantics::ReferencesMap refMap;

    hif::semantics::GetReferencesOptions opt;
    opt.skip_standard_declarations = true;
    opt.check_object_method      = refCollectMethod;
    hif::semantics::getAllReferences(refMap, sem, o, opt);

    PartialWritesMap partialWritesMap;

    for (hif::semantics::ReferencesMap::iterator i = refMap.begin(); i != refMap.end(); ++i) {
        DataDeclaration *decl = dynamic_cast<DataDeclaration *>(i->first);
        messageAssert(decl != nullptr, "unexpected declaration", decl, sem);
        hif::semantics::ReferencesSet &refSet = i->second;

        for (hif::semantics::ReferencesSet::iterator j = refSet.begin(); j != refSet.end(); ++j) {
            Object *ref = *j;

            StateTable *st = hif::getNearestParent<StateTable>(ref);
            if (st == nullptr)
                continue;

            partialWritesMap[decl].insert(st);
        }
    }

    for (PartialWritesMap::iterator i = partialWritesMap.begin(); i != partialWritesMap.end(); ++i) {
        DataDeclaration *decl      = i->first;
        StateTableSet &stateTables = i->second;

        // create a support variable <signal/port>_var
        Variable *supportVar = new Variable();
        std::string varName =
            hif::NameTable::getInstance()->getFreshName((decl->getName() + std::string("_var")).c_str());
        supportVar->setType(hif::copy(decl->getType()));
        supportVar->setName(varName);
        supportVar->setValue(hif::copy(decl->getValue()));

        hif::manipulation::addDeclarationInContext(supportVar, decl);

        for (StateTableSet::iterator j = stateTables.begin(); j != stateTables.end(); ++j) {
            StateTable *st = *j;
            ret            = true;

            // as first instruction, add the assignment of target to variable
            Assign *firstAssign = new Assign();
            firstAssign->setLeftHandSide(new Identifier(supportVar->getName()));
            firstAssign->setRightHandSide(new Identifier(decl->getName()));
            _addInitialAssign(st, firstAssign, sem);

            // replace all target refs inside st
            _replaceTargets(decl, st, supportVar, sem);

            // as last instruction, add the assignment of variable to target
            Assign *lastAssign = new Assign();
            lastAssign->setLeftHandSide(new Identifier(decl->getName()));
            lastAssign->setRightHandSide(new Identifier(supportVar->getName()));
            _addSignalAssign(st, lastAssign, sem);
        }
    }

    return ret;
}

} // namespace

FixMultipleSignalPortAssignsOptions::FixMultipleSignalPortAssignsOptions()
    : fixMultipleWritings(true)
    , fixPartialWritings(true)
    , fixSubrangesInSensitivity(false)
{
    // n.t.d.
}

FixMultipleSignalPortAssignsOptions::~FixMultipleSignalPortAssignsOptions()
{
    // n.t.d.
}

FixMultipleSignalPortAssignsOptions::FixMultipleSignalPortAssignsOptions(
    const FixMultipleSignalPortAssignsOptions &other)
    : fixMultipleWritings(other.fixMultipleWritings)
    , fixPartialWritings(other.fixPartialWritings)
    , fixSubrangesInSensitivity(other.fixSubrangesInSensitivity)
{
    // n.t.d.
}

FixMultipleSignalPortAssignsOptions &
FixMultipleSignalPortAssignsOptions::operator=(FixMultipleSignalPortAssignsOptions other)
{
    swap(other);
    return *this;
}

void FixMultipleSignalPortAssignsOptions::swap(FixMultipleSignalPortAssignsOptions &other)
{
    std::swap(fixMultipleWritings, other.fixMultipleWritings);
    std::swap(fixPartialWritings, other.fixPartialWritings);
    std::swap(fixSubrangesInSensitivity, other.fixSubrangesInSensitivity);
}

bool fixMultipleSignalPortAssigns(
    System *o,
    hif::semantics::ILanguageSemantics *sem,
    const FixMultipleSignalPortAssignsOptions &opts)
{
    hif::application_utils::initializeLogHeader("HIF", "fixMultipleSignalPortAssigns");

    bool ret = false;

    std::set<StateTable *> list;
    hif::manipulation::transformGlobalActions(o, list, sem);

    // TODO: make class vars!
    _supportSignals1.clear();
    _supportSignals2.clear();
    _updatingProcesses1.clear();
    _updatingProcesses2.clear();

    if (opts.fixSubrangesInSensitivity) {
        ret |= _fixSubrangesInSensitivity(o, sem);
    }

    if (opts.fixMultipleWritings) {
        ret |= _fixMultipleWritings(o, sem);
    }

    if (opts.fixPartialWritings) {
        ret |= _fixPartialWritings(o, sem);
    }

    hif::application_utils::restoreLogHeader();
    return ret;
}
} // namespace manipulation
} // namespace hif
