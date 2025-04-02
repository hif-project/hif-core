/// @file propagateSymbols.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/propagateSymbols.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/search.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /* anon */
{
class Propagator : public hif::HifVisitor
{
public:
    // Global traits.
    typedef PropagationOptions::DeclarationTargets DeclarationTargets;
    typedef PropagationOptions::ObjectSet ObjectSet;
    typedef PropagationOptions::ViewSet ViewSet;
    typedef PropagationOptions::ViewMap ViewMap;

    // Local traits.
    typedef std::list<Instance *> InstanceList;

    Propagator(PropagationOptions &options, hif::semantics::ILanguageSemantics *sem);
    virtual ~Propagator();

    bool propagateTargets();

private:
    int visitInstance(Instance &o);
    int visitPort(Port &o);
    int visitSignal(Signal &o);
    int visitVariable(Variable &o);
    int visitView(View &o);

    void _collectParentViewInstances(InstanceList &parentViewInstances, View *parentView, View *parentModule);

    void _pushNewTarget(Object *o);

    void _assureSafeScope(Type *o, Object *newScope);

    bool _isExcludedType(Type *o);

    PropagationOptions &_options;
    hif::semantics::ILanguageSemantics *_sem;
    ObjectSet _propagatedSet;
    ObjectSet _newObjectsSet;
    HifFactory _factory;

    // Disabled.
    Propagator(const Propagator &);
    Propagator operator=(const Propagator &);
};

Propagator::Propagator(PropagationOptions &options, hif::semantics::ILanguageSemantics *sem)
    : _options(options)
    , _sem(sem)
    , _propagatedSet()
    , _newObjectsSet()
    , _factory(sem)
{
    // ntd
}

Propagator::~Propagator()
{
    // ntd
}

bool Propagator::propagateTargets()
{
    bool error = false;
    for (DeclarationTargets::iterator i = _options.declarationTargets.begin(); i != _options.declarationTargets.end();
         ++i) {
        Object *obj = *i;
        error |= (obj->acceptVisitor(*this) != 0);
    }

    return error;
}

int Propagator::visitInstance(Instance &o)
{
    if (_propagatedSet.find(&o) != _propagatedSet.end())
        return 0;
    _propagatedSet.insert(&o);

    View *parentView = hif::getNearestParent<View>(&o);
    messageAssert(parentView != nullptr, "Cannot find parent view", &o, _sem);

    for (BList<PortAssign>::iterator i = o.portAssigns.begin(); i != o.portAssigns.end(); ++i) {
        PortAssign *pa = *i;

        Type *paType = hif::semantics::getSemanticType(pa, _sem);
        messageAssert(paType != nullptr, "Cannot type portassign", pa, _sem);

        // Replacing original port binding with a binding with support signal,
        // and adding a globact to propagate new supportSignal to old binding.

        // create support signal
        std::string sigName = hif::NameTable::getInstance()->getFreshName(
            (std::string("out_") + o.getName() + "_" + pa->getName()).c_str());

        Port *port = hif::semantics::getDeclaration(pa, _sem);
        messageAssert(port != nullptr, "Declaration not found", pa, _sem);

        Signal *supportSig = _factory.signal(hif::copy(paType), sigName, _sem->getTypeDefaultValue(paType, port));

        parentView->getContents()->declarations.push_back(supportSig);

        // create the globact
        Assign *ass = nullptr;
        if (port->getDirection() == dir_out) {
            ass = _factory.assignment(
                pa->setValue(nullptr), _factory.cast(hif::copy(paType), new Identifier(supportSig->getName())));
        } else if (port->getDirection() == dir_in) {
            ass = _factory.assignment(
                new Identifier(supportSig->getName()),
                _factory.cast(hif::copy(supportSig->getType()), pa->setValue(nullptr)));
        } else {
            messageError("Unsupported case", pa, _sem);
        }

        GlobalAction *ga = parentView->getContents()->getGlobalAction();
        if (ga == nullptr) {
            ga = new GlobalAction();
            parentView->getContents()->setGlobalAction(ga);
        }
        ga->actions.push_back(ass);

        pa->setValue(new Identifier(supportSig->getName()));

        _pushNewTarget(supportSig);
    }

    return 0;
}

int Propagator::visitPort(Port &o)
{
    if (_isExcludedType(o.getType()))
        return 0;

    messageAssert(o.getDirection() != dir_none, "Unexpected port direction", &o, _sem);

    if (_propagatedSet.find(&o) != _propagatedSet.end())
        return 0;
    _propagatedSet.insert(&o);

    View *parentView = hif::getNearestParent<View>(&o);
    messageAssert(parentView != nullptr, "Cannot find parent view", &o, _sem);

    if (o.getDirection() == dir_in || o.getDirection() == dir_inout) {
        if (!_options.inputPorts)
            return 0;

        // create a support signal
        std::string sigName = hif::NameTable::getInstance()->getFreshName((std::string("in_") + o.getName()).c_str());

        Signal *supportSig =
            _factory.signal(hif::copy(o.getType()), sigName, _sem->getTypeDefaultValue(o.getType(), &o));

        parentView->getContents()->declarations.push_back(supportSig);

        // create the globact
        Assign *ass = _factory.assignment(
            new Identifier(o.getName()), _factory.cast(hif::copy(o.getType()), new Identifier(supportSig->getName())));
        GlobalAction *ga = parentView->getContents()->getGlobalAction();
        if (ga == nullptr) {
            ga = new GlobalAction();
            parentView->getContents()->setGlobalAction(ga);
        }
        ga->actions.push_back(ass);

        _pushNewTarget(supportSig);

        return 0;
    }

    // dir_out case

    for (ViewSet::iterator i = (*_options.pmm)[parentView].begin(); i != (*_options.pmm)[parentView].end(); ++i) {
        View *parentModule = *i;

        // name of all instances of parentView inside parent modules
        InstanceList parentViewInstances;
        _collectParentViewInstances(parentViewInstances, parentView, parentModule);

        for (InstanceList::iterator j = parentViewInstances.begin(); j != parentViewInstances.end(); ++j) {
            Instance *inst = *j;
            if (_newObjectsSet.find(&o) == _newObjectsSet.end()) {
                // outport of original design
                // recursion on new support signal
                for (BList<PortAssign>::iterator k = inst->portAssigns.begin(); k != inst->portAssigns.end(); ++k) {
                    PortAssign *pa = *k;
                    if (pa->getName() != o.getName())
                        continue;

                    Type *paType = hif::semantics::getSemanticType(pa, _sem);
                    messageAssert(paType != nullptr, "Cannot type portassign", pa, _sem);

                    // Replacing original port binding with a binding with support signal,
                    // and adding a globact to propagate new supportSignal to old binding.

                    // create support signal
                    std::string sigName = hif::NameTable::getInstance()->getFreshName(
                        (std::string("out_") + inst->getName() + "_" + o.getName()).c_str());

                    Signal *supportSig =
                        _factory.signal(hif::copy(paType), sigName, _sem->getTypeDefaultValue(paType, &o));

                    parentModule->getContents()->declarations.push_back(supportSig);

                    // create the globact
                    Assign *ass = _factory.assignment(
                        pa->setValue(nullptr), _factory.cast(hif::copy(paType), new Identifier(supportSig->getName())));
                    GlobalAction *ga = parentModule->getContents()->getGlobalAction();
                    if (ga == nullptr) {
                        ga = new GlobalAction();
                        parentModule->getContents()->setGlobalAction(ga);
                    }
                    ga->actions.push_back(ass);

                    pa->setValue(new Identifier(supportSig->getName()));

                    _pushNewTarget(supportSig);
                }
            } else {
                // outport created for propagation
                // direct port to port binding
                std::string name = std::string(inst->getName()) + "_" + o.getName();

                PortAssign *pa = _factory.portAssign(o.getName(), _factory.identifier(name.c_str()));
                inst->portAssigns.push_back(pa);

                Type *paType = hif::semantics::getSemanticType(pa, _sem);
                messageAssert(paType != nullptr, "Cannot type portassign", pa, _sem);

                Port *newPort =
                    _factory.port(hif::copy(paType), name.c_str(), dir_out, _sem->getTypeDefaultValue(paType, &o));

                parentModule->getEntity()->ports.push_back(newPort);

                _pushNewTarget(newPort);
            }
        }
    }

    return 0;
}

int Propagator::visitSignal(Signal &o)
{
    if (_isExcludedType(o.getType()))
        return 0;

    if (_propagatedSet.find(&o) != _propagatedSet.end())
        return 0;
    _propagatedSet.insert(&o);

    // create support port
    std::string portName =
        hif::NameTable::getInstance()->getFreshName((std::string("sig_") + o.getName() + "_dbg").c_str());

    hif::semantics::updateDeclarations(o.getType(), _sem);

    Port *supportPort =
        _factory.port(hif::copy(o.getType()), portName, dir_out, _sem->getTypeDefaultValue(o.getType(), &o));

    View *parentView = hif::getNearestParent<View>(&o);
    messageAssert(parentView != nullptr, "Cannot find parent view", &o, _sem);
    parentView->getEntity()->ports.push_back(supportPort);

    // Ensuring new port type does not propagates internal constants or types:
    _assureSafeScope(supportPort->getType(), parentView);

    // create the globact
    Assign *ass = _factory.assignment(
        new Identifier(supportPort->getName()),
        _factory.cast(hif::copy(supportPort->getType()), new Identifier(o.getName())));
    GlobalAction *ga = parentView->getContents()->getGlobalAction();
    if (ga == nullptr) {
        ga = new GlobalAction();
        parentView->getContents()->setGlobalAction(ga);
    }
    ga->actions.push_back(ass);

    _pushNewTarget(supportPort);

    return 0;
}

int Propagator::visitVariable(Variable &o)
{
    if (_isExcludedType(o.getType()))
        return 0;

    if (_propagatedSet.find(&o) != _propagatedSet.end())
        return 0;
    _propagatedSet.insert(&o);

    StateTable *st = hif::getNearestParent<StateTable>(&o);
    messageAssert(st != nullptr, "Variable propagation in allowed only into processes (1)", &o, _sem);

    Contents *c = dynamic_cast<Contents *>(st->getParent());
    messageAssert(
        c != nullptr && st->isInBList() && st->getBList() == reinterpret_cast<BList<Object> *>(&c->stateTables),
        "Variable propagation in allowed only into processes (2)", &o, _sem);

    // create support signal
    std::string sigName = hif::NameTable::getInstance()->getFreshName((std::string("sig_") + o.getName()).c_str());

    hif::semantics::updateDeclarations(o.getType(), _sem);

    Signal *supportSig = _factory.signal(hif::copy(o.getType()), sigName, _sem->getTypeDefaultValue(o.getType(), &o));

    c->declarations.push_back(supportSig);

    _assureSafeScope(supportSig->getType(), c);

    // create the globact
    Assign *ass = _factory.assignment(
        new Identifier(supportSig->getName()),
        _factory.cast(hif::copy(supportSig->getType()), new Identifier(o.getName())));

    hif::HifTypedQuery<Wait> q;
    std::list<Wait *> res;
    hif::search(res, st, q);

    if (res.empty()) {
        st->states.front()->actions.push_back(ass);
    } else {
        for (std::list<Wait *>::iterator i = res.begin(); i != res.end(); ++i) {
            BList<Action>::iterator it(*i);
            it.insert_before(hif::copy(ass));
        }

        delete ass;
    }

    _pushNewTarget(supportSig);
    return 0;
}

int Propagator::visitView(View &o)
{
    Contents *c = o.getContents();
    messageAssert(c != nullptr, "Unexpected view without contents", &o, _sem);

    if (_options.allSignals) {
        for (BList<Declaration>::iterator i = c->declarations.begin(); i != c->declarations.end(); ++i) {
            if (dynamic_cast<Signal *>(*i) == nullptr)
                continue;

            _options.declarationTargets.push_back(*i);
        }
    }

    if (_options.allPorts) {
        for (BList<Instance>::iterator i = c->instances.begin(); i != c->instances.end(); ++i) {
            _options.declarationTargets.push_back(*i);
        }
    }

    if (_options.allVariables) {
        for (BList<StateTable>::iterator i = c->stateTables.begin(); i != c->stateTables.end(); ++i) {
            for (BList<Declaration>::iterator j = (*i)->declarations.begin(); j != (*i)->declarations.end(); ++j) {
                Variable *var = dynamic_cast<Variable *>(*j);
                if (var == nullptr)
                    continue;

                _options.declarationTargets.push_back(var);
            }
        }
    }

    return 0;
}

void Propagator::_collectParentViewInstances(InstanceList &parentViewInstances, View *parentView, View *parentModule)
{
    for (BList<Instance>::iterator i = parentModule->getContents()->instances.begin();
         i != parentModule->getContents()->instances.end(); ++i) {
        Instance *inst = *i;

        ViewReference *vr = dynamic_cast<ViewReference *>(inst->getReferencedType());
        if (vr == nullptr)
            continue;

        ViewReference::DeclarationType *v = hif::semantics::getDeclaration(vr, _sem);
        messageAssert(v != nullptr, "Declaration not found", vr, _sem);

        if (v != parentView)
            continue;

        parentViewInstances.push_back(inst);
    }
}

void Propagator::_pushNewTarget(Object *o)
{
    _options.declarationTargets.push_back(o);
    _newObjectsSet.insert(o);
}

void Propagator::_assureSafeScope(Type *o, Object *newScope)
{
    SimplifyOptions opt;
    opt.simplify_constants      = true;
    opt.simplify_typereferences = true;
    opt.context                 = newScope;
    simplify(o, _sem, opt);
}

bool Propagator::_isExcludedType(Type *o)
{
    Type *t = hif::semantics::getBaseType(o, false, _sem);

    if (_options.skipArrays && dynamic_cast<Array *>(t) != nullptr)
        return true;

    if (_options.includeNotRtlTypes)
        return false;

    if (dynamic_cast<File *>(t) != nullptr)
        return true;
    if (dynamic_cast<Pointer *>(t) != nullptr)
        return true;
    if (dynamic_cast<String *>(t) != nullptr)
        return true;

    return false;
}
} // namespace
PropagationOptions::PropagationOptions()
    : allSignals(false)
    , allPorts(false)
    , allVariables(false)
    , inputPorts(false)
    , includeNotRtlTypes(false)
    , skipArrays(false)
    ,
    //doNotSplitArrays(false),
    declarationTargets()
    , system(nullptr)
    , smm(nullptr)
    , pmm(nullptr)
{
    // ntd
}

PropagationOptions::~PropagationOptions()
{
    // ntd
}

PropagationOptions::PropagationOptions(const PropagationOptions &o)
    : allSignals(o.allSignals)
    , allPorts(o.allPorts)
    , allVariables(o.allVariables)
    , inputPorts(o.inputPorts)
    , includeNotRtlTypes(o.includeNotRtlTypes)
    , skipArrays(o.skipArrays)
    ,
    //doNotSplitArrays(o.doNotSplitArrays),
    declarationTargets(o.declarationTargets)
    , system(o.system)
    , smm(o.smm)
    , pmm(o.pmm)
{
    // ntd
}

PropagationOptions &PropagationOptions::operator=(const PropagationOptions &o)
{
    if (&o == this)
        return *this;

    allSignals         = o.allSignals;
    allPorts           = o.allPorts;
    allVariables       = o.allVariables;
    inputPorts         = o.inputPorts;
    includeNotRtlTypes = o.includeNotRtlTypes;
    skipArrays         = o.skipArrays;
    //doNotSplitArrays = o.doNotSplitArrays;
    declarationTargets = o.declarationTargets;
    system             = o.system;
    smm                = o.smm;
    pmm                = o.pmm;

    return *this;
}

bool propagateSymbols(PropagationOptions &options, hif::semantics::ILanguageSemantics *sem)
{
    messageDebugAssert(
        !options.declarationTargets.empty(), "Required at least declarationTargets or instanceTargets not empty",
        nullptr, sem);
    if (options.declarationTargets.empty())
        return false;

    if (options.system == nullptr) {
        Object *obj    = *options.declarationTargets.begin();
        options.system = hif::getNearestParent<System>(obj);

        messageDebugAssert(options.system != nullptr, "Cannot reach system", obj, sem);
        if (options.system == nullptr)
            return false;
    }

    if (expandGenerates(options.system, sem)) {
        messageWarning("Generate statements have been expanded.", nullptr, nullptr);
    }

    if (options.smm == nullptr || options.pmm == nullptr) {
        messageAssert(options.smm == nullptr && options.pmm == nullptr, "Unexpected options.", nullptr, nullptr);
        options.smm = new PropagationOptions::ViewMap();
        options.pmm = new PropagationOptions::ViewMap();
        ViewDependencyOptions opt;
        opt.skipStandardLibraries = true;
        findViewDependencies(options.system, *options.smm, *options.pmm, sem, opt);
    }

    Propagator p(options, sem);
    bool ret = p.propagateTargets();

    return ret;
}

} // namespace manipulation
} // namespace hif
