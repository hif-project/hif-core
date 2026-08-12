/// @file fixInstanceBindings.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/fixInstanceBindings.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/search.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

/// Typdef for the map of processes type.
typedef std::set<Object *> ProcessesSet;
typedef std::map<PortAssign *, ProcessesSet> ProcessesMap;
typedef std::map<Object *, StateTable *> GlobactMap;

/// This function builds this name: prefix_suffix[_xyzu]
std::string buildFreshName(const std::string &prefix, const std::string &suffix)
{
    return NameTable::getInstance()->getFreshName(prefix + "_" + suffix);
}

Signal *_buildSignal(PortAssign *portAssigns, Type *signalType, hif::semantics::ILanguageSemantics *sem)
{
    // 1- Build a new signal
    Signal *signal = new Signal();
    signal->setType(signalType);
    signal->setValue(sem->getTypeDefaultValue(signalType, signal));
    // build signal name
    std::string signalName = buildFreshName(portAssigns->getName(), "portassign");
    signal->setName(signalName);

    return signal;
}
bool _rebindPortAssign(PortAssign *portAssigns, Signal *sig)
{
    Contents *cc = hif::getNearestParent<Contents>(portAssigns);
    if (cc == nullptr)
        return false;
    delete portAssigns->setValue(new Identifier(sig->getName()));
    cc->declarations.push_back(sig);

    return true;
}
bool _addAssignInsideProcess(
    PortAssign *portAssigns,
    ProcessesMap &map,
    GlobactMap &globactMap,
    hif::semantics::ILanguageSemantics *sem)
{
    ProcessesMap::iterator checkIt = map.find(portAssigns);
    if (checkIt == map.end())
        return true; // does not need fix

    ProcessesSet &set = checkIt->second;

    messageAssert(!set.empty() && set.size() < 2, "Unexpected number of drivers.", portAssigns, sem);

    Object *proc = *set.begin();
    if (proc == nullptr) {
        // no drivers: just an open assign!
        Signal *sig = _buildSignal(portAssigns, hif::copy(hif::semantics::getSemanticType(portAssigns, sem)), sem);
        return _rebindPortAssign(portAssigns, sig);
    }

    GlobactMap::iterator gaIt = globactMap.find(proc);
    if (gaIt != globactMap.end()) {
        // already mapped globact to st
        proc = gaIt->second;
    } else if (dynamic_cast<Assign *>(proc) != nullptr) {
        // global action
        Assign *aa = static_cast<Assign *>(proc);
        std::set<StateTable *> list;
        transformGlobalActions(proc, list, sem);
        messageAssert(list.size() == 1, "Uneexpected number of processes.", nullptr, sem);
        // Fall into next fix (for StateTable)
        proc           = *list.begin();
        globactMap[aa] = *list.begin();
    }

    if (dynamic_cast<StateTable *>(proc) != nullptr) {
        // actual process
        StateTable *st = static_cast<StateTable *>(proc);
        Identifier *boundId =
            dynamic_cast<Identifier *>(hif::getTerminalPrefix(hif::getChildSkippingCasts(portAssigns->getValue())));
        messageAssert(boundId != nullptr, "Unexpected bind", portAssigns, sem);
        Signal *origSig = dynamic_cast<Signal *>(hif::semantics::getDeclaration(boundId, sem));
        messageAssert(origSig != nullptr, "Declaration not found", boundId, sem);

        // Creating new signal for binding:
        Signal *sig = _buildSignal(portAssigns, hif::copy(hif::semantics::getSemanticType(portAssigns, sem)), sem);

        // Creating new var:
        Variable *v = new Variable();
        v->setName(buildFreshName(portAssigns->getName(), "var"));
        v->setType(hif::copy(origSig->getType()));
        v->setValue(hif::copy(origSig->getValue()));
        st->declarations.push_back(v);
        // Adding two new assigns to update the new signal and also the original one.
        Assign aOrig;
        aOrig.setLeftHandSide(new Identifier(origSig->getName()));
        aOrig.setRightHandSide(new Identifier(v->getName()));

        Assign aNew;
        aNew.setLeftHandSide(new Identifier(sig->getName()));
        Value *bind = hif::copy(portAssigns->getValue());
        delete hif::setTerminalPrefix(hif::getChildSkippingCasts(bind), new Identifier(v->getName()));
        aNew.setRightHandSide(bind);

        // Replacing occurrences of original signal in lhs of assignments
        // with the new var
        hif::semantics::ReferencesSet refList;
        hif::semantics::getReferences(origSig, refList, sem, st);
        for (hif::semantics::ReferencesSet::iterator i = refList.begin(); i != refList.end(); ++i) {
            if (!hif::manipulation::isInLeftHandSide(*i))
                continue;
            Identifier *id = dynamic_cast<Identifier *>(*i);
            messageAssert(id != nullptr, "Unexpected reference", *i, sem);

            // Replacing with new var
            Identifier *newId = new Identifier(v->getName());
            id->replace(newId);
            delete id;

            // Updating signals.
            Assign *ass = getNearestParent<Assign>(newId);
            messageAssert(ass != nullptr, "Assign is nullptr.", newId, sem);
            messageAssert(ass->isInBList(), "Assign not in BList.", ass, sem);
            BList<Action>::iterator it(ass);
            it.insert_after(hif::copy(&aNew));
            it.insert_after(hif::copy(&aOrig));
        }

        // Rebinding
        bool check = _rebindPortAssign(portAssigns, sig);
        messageAssert(check, "Rebind failed", portAssigns, sem);

    } else {
        messageDebug("Failing portassign:", portAssigns, sem);
        messageError("Unexpected driver kind.", proc, sem);
    }

    return true;
}
void _setOptimizableFlags(PortDirection direction, bool &input_is_optimizable, bool &output_is_optimizable)
{
    if (direction == dir_in)
        input_is_optimizable = false;
    else if (direction == dir_out)
        output_is_optimizable = false;
    else if (direction == dir_inout) {
        input_is_optimizable  = false;
        output_is_optimizable = false;
    }
}
bool _hasSingleDriver(Declaration *decl, hif::semantics::ILanguageSemantics *sem, Object *&driver)
{
    Contents *contents = hif::getNearestParent<Contents>(decl);
    messageAssert(contents != nullptr, "Contents not found.", decl, sem);
    hif::semantics::ReferencesSet refList;
    std::set<Object *> drivers;
    hif::semantics::getReferences(decl, refList, sem, contents);
    driver = nullptr;
    for (hif::semantics::ReferencesSet::iterator i = refList.begin(); i != refList.end(); ++i) {
        PortAssign *pa = getNearestParent<PortAssign>(*i);
        if (pa != nullptr) {
            PortAssign::DeclarationType *port = hif::semantics::getDeclaration(pa, sem);
            messageAssert(port != nullptr, "Declaration not found", pa, sem);
            if (port->getDirection() == dir_in)
                continue;
            // Not optimizable: we must introduce a globact :(
            return false;
        }

        if (!hif::manipulation::isInLeftHandSide(*i))
            continue;
        StateTable *st = hif::getNearestParent<StateTable>(*i);
        if (st != nullptr) {
            // TODO FIXME: skip pf_initial process kind (from Verilog)
            driver = st;
        } else {
            Assign *a = hif::getNearestParent<Assign>(*i);
            if (a != nullptr)
                driver = a;
        }
        if (driver == nullptr)
            continue;
        drivers.insert(driver);
        if (drivers.size() > 1)
            return false;
    }
    return true;
}
bool _collectReaders(
    Declaration *decl,
    PortAssign *bind,
    ProcessesMap &processesMap,
    hif::semantics::ILanguageSemantics *sem)
{
    Contents *contents = hif::getNearestParent<Contents>(decl);
    messageAssert(contents != nullptr, "Contents not found.", decl, sem);
    hif::semantics::ReferencesSet refList;
    hif::semantics::getReferences(decl, refList, sem, contents);
    for (hif::semantics::ReferencesSet::iterator i = refList.begin(); i != refList.end(); ++i) {
        PortAssign *pa = getNearestParent<PortAssign>(*i);
        if (pa != nullptr) {
            Port *port = hif::semantics::getDeclaration(pa, sem);
            messageAssert(port != nullptr, "Declaration not found", pa, sem);
            if (port->getDirection() == dir_out)
                continue;
            // Not optimizable: we must introduce a globact :(
            return false;
        }

        if (hif::manipulation::isInLeftHandSide(*i))
            continue;
        Object *reader = nullptr;
        StateTable *st = hif::getNearestParent<StateTable>(*i);
        if (st != nullptr) {
            reader = st;
        } else {
            Assign *a = hif::getNearestParent<Assign>(*i);
            if (a != nullptr)
                reader = a;
        }
        if (reader == nullptr)
            continue;
        processesMap[bind].insert(reader);
    }

    return true;
}

/// Check whether the given port binding is name-to-name.
bool _isName2NameBinding(
    PortAssign *bind,
    PortDirection dir,
    hif::semantics::ILanguageSemantics *sem,
    bool &input_is_optimizable,
    bool &output_is_optimizable,
    bool &hasConstantInput,
    bool &hasConstantOutput,
    ProcessesMap &processesMap,
    const FixBindingOptions &opt)
{
    Value *boundValue = bind->getValue();

    hif::TerminalPrefixOptions terminalOpts;
    terminalOpts.recurseIntoDerefExpressions = true;
    Value *innerValue = hif::getTerminalPrefix(hif::getChildSkippingCasts(boundValue), terminalOpts);
    if (opt.allowOnlySignalPorts && dynamic_cast<hif::features::ISymbol *>(innerValue) != nullptr) {
        Declaration *dec = hif::semantics::getDeclaration(innerValue, sem);
        if (dec != nullptr && dynamic_cast<Signal *>(dec) == nullptr && dynamic_cast<Port *>(dec) == nullptr) {
            _setOptimizableFlags(dir, input_is_optimizable, output_is_optimizable);
            return false;
        }
    }

    if (dynamic_cast<Cast *>(boundValue) != nullptr && !opt.fixVectorCasts) {
        Cast *c         = static_cast<Cast *>(boundValue);
        Type *valueType = hif::semantics::getSemanticType(c->getValue(), sem);
        messageAssert(valueType != nullptr, "Cannot type bound value.", c, sem);
        Type *castType = c->getType();

        Bitvector *vBv = dynamic_cast<Bitvector *>(valueType);
        Signed *vSig   = dynamic_cast<Signed *>(valueType);
        Unsigned *vUns = dynamic_cast<Unsigned *>(valueType);

        Bitvector *cBv = dynamic_cast<Bitvector *>(castType);
        Signed *cSig   = dynamic_cast<Signed *>(castType);
        Unsigned *cUns = dynamic_cast<Unsigned *>(castType);

        bool vIsV = (vBv != nullptr && vBv->isLogic()) || vSig != nullptr || vUns != nullptr;
        bool cIsV = (cBv != nullptr && cBv->isLogic()) || cSig != nullptr || cUns != nullptr;
        bool isVectorCast =
            vIsV && cIsV && hif::equals(hif::typeGetSpan(valueType, sem), hif::typeGetSpan(castType, sem));
        if (isVectorCast)
            boundValue = c->getValue();
    }

    if (dynamic_cast<Identifier *>(boundValue) != nullptr)
        return true;

    bool isCast   = (dynamic_cast<Cast *>(boundValue) != nullptr);
    bool isMember = (dynamic_cast<Member *>(boundValue) != nullptr);
    bool isSlice  = (dynamic_cast<Slice *>(boundValue) != nullptr);
    bool isConst  = (dynamic_cast<ConstValue *>(boundValue) != nullptr);
    bool isOther  = !isCast && !isMember && !isSlice;

    if (!opt.fixCasts && isCast)
        return true;
    if (!opt.fixMembers && isMember)
        return true;
    if (!opt.fixSlices && isSlice)
        return true;
    if (isConst) {
        if (!opt.fixConstants)
            return true;
        if (dir == dir_in)
            hasConstantInput = true;
        else if (dir == dir_out)
            hasConstantOutput = true;
        else if (dir == dir_inout) {
            hasConstantInput  = true;
            hasConstantOutput = true;
        }
        return true;
    }
    if (!opt.fixOthers && isOther)
        return true;

    Identifier *boundId = dynamic_cast<Identifier *>(hif::getTerminalPrefix(boundValue));
    if (boundId == nullptr) {
        _setOptimizableFlags(dir, input_is_optimizable, output_is_optimizable);
        return false;
    }
    Identifier::DeclarationType *decl = hif::semantics::getDeclaration(boundId, sem);
    Signal *signal                    = dynamic_cast<Signal *>(decl);
    if (signal == nullptr) {
        _setOptimizableFlags(dir, input_is_optimizable, output_is_optimizable);
        return false;
    }

    if (dir == dir_out) {
        if (!_collectReaders(signal, bind, processesMap, sem)) {
            _setOptimizableFlags(dir, input_is_optimizable, output_is_optimizable);
            return false;
        }
        return false;
    } else if (dir == dir_inout) {
        // TODO not managed
        _setOptimizableFlags(dir, input_is_optimizable, output_is_optimizable);
        return false;
    } else if (dir == dir_in) {
        Object *driver = nullptr;
        if (!_hasSingleDriver(signal, sem, driver)) {
            _setOptimizableFlags(dir, input_is_optimizable, output_is_optimizable);
            return false;
        }
        processesMap[bind].insert(driver);
        return false;
    }
    return false;
}
void _createGlobalAction(
    Value *globalTarget,
    Value *globalSource,
    BaseContents *contents,
    hif::semantics::ILanguageSemantics *sem)
{
    Assign *globalAssign = new Assign();
    globalAssign->setRightHandSide(globalSource);
    globalAssign->setLeftHandSide(globalTarget);

    if (contents->getGlobalAction() == nullptr) {
        contents->setGlobalAction(new GlobalAction());
    }
    contents->getGlobalAction()->actions.push_back(globalAssign);

    // ensuring casts are not implicit slices (or members)
    // ref design: vhdl/openCores/uart
    if (dynamic_cast<Cast *>(globalTarget) != nullptr) {
        Value *target = hif::getChildSkippingCasts(globalTarget);
        while (dynamic_cast<Cast *>(target->getParent()) != nullptr) {
            Cast *cast = static_cast<Cast *>(target->getParent());

            Type *targetType  = getSemanticType(target, sem);
            Type *castType    = getSemanticType(cast, sem);
            Range *targetSpan = hif::typeGetSpan(targetType, sem);
            Range *castSpan   = hif::typeGetSpan(castType, sem);
            Value *targetSize = targetSpan != nullptr ? hif::semantics::spanGetSize(targetSpan, sem) : nullptr;
            Value *castSize   = castSpan != nullptr ? hif::semantics::spanGetSize(castSpan, sem) : nullptr;
            // The check must be done possibly against consts, since wrong slices
            // could compromise the result. This is a trick, but at the moment
            // no better heuristic seems available.
            // ref design: vhdl/openCoresFunbase/hibi_udp
            targetSize        = hif::manipulation::getAggressiveSimplified(targetSize, sem);
            castSize          = hif::manipulation::getAggressiveSimplified(castSize, sem);
            // Same span size means the cast is just for type changing,
            // es. lv<3> --> bv<3>
            // or type shifting:
            // es. lv<8:5> --> lv<3:0>
            // so no slice is required, since all bits are preserved.
            // ref design: verilog/openCores/can_top
            if (!hif::equals(targetSize, castSize) && castSpan != nullptr) {
                Slice *slice = new Slice();
                slice->setSpan(hif::copy(castSpan));
                target->replace(slice);
                slice->setPrefix(target);
                target = slice;
            }

            cast->replace(target);
            delete cast;
        }
    }

    globalTarget = globalAssign->getLeftHandSide();

    // Eventually push cast on source.
    hif::semantics::resetTypes(globalAssign);
    Type *tt                                                    = getSemanticType(globalTarget, sem);
    Type *ts                                                    = getSemanticType(globalSource, sem);
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo info = sem->getExprType(tt, ts, op_assign, globalAssign);
    if (info.returnedType == nullptr) {
        Cast *c = new Cast();
        c->setType(hif::copy(tt));
        c->setValue(globalAssign->getRightHandSide());
        globalAssign->setRightHandSide(c);
    }

    hif::manipulation::simplify(globalAssign, sem);

    // Split potential concat targets since could containt casts.
    // Ref. design: verilog/openCores/or1200
    SplitAssignTargetOptions splitOpts;
    splitOpts.splitConcats            = true;
    splitOpts.createSignals           = true;
    splitOpts.splitRecordValueConcats = true;
    hif::manipulation::splitAssignTargets(globalAssign, sem, splitOpts);
}
bool _addSignalForConstant(PortAssign *pa, Type *type, BaseContents *bc, hif::semantics::ILanguageSemantics *sem)
{
    Signal *sig = _buildSignal(pa, type, sem);
    delete sig->setValue(pa->setValue(nullptr));
    bc->declarations.push_back(sig);

    Identifier *id = new Identifier;
    id->setName(sig->getName());
    pa->setValue(id);

    return true;
}
/// This function replace the portassign value with a new signal, added to the
/// declarations of the given contents.
bool _addSignalForPortassign(
    PortAssign *portassign,
    BaseContents *contents,
    Type *signalType,
    hif::semantics::ILanguageSemantics *sem)
{
    // 1- Build a new signal
    Signal *signal = _buildSignal(portassign, signalType, sem);

    // 2- Add a global action, writing to the signal the value of the
    //    PortAssign. The source of the global assign is a cast, but it is
    //    important to use the explicit cast of the semantics
    Value *globalSource = nullptr;
    Value *globalTarget = nullptr;
    // get the type of the PortAssign value
    Type *val_type      = hif::semantics::getSemanticType(portassign->getValue(), sem);
    if (val_type == nullptr)
        return false;
    // set declaration because it is not present in the tree now
    Identifier *signalRef = new Identifier(signal->getName());
    hif::semantics::setDeclaration(signalRef, signal);
    // required by some semantics
    hif::semantics::getSemanticType(signalRef, sem);
    // the target and the source of the assign, depends on the direction of the
    // PortAssign
    Port *port = hif::semantics::getDeclaration(portassign, sem);
    messageAssert(port != nullptr, "Declaration not found", portassign, sem);
    if (port->getDirection() == dir_in) {
        // build the source and the target
        globalSource = sem->explicitCast(portassign->getValue(), signalType, nullptr);
        globalTarget = signalRef;
        _createGlobalAction(globalTarget, globalSource, contents, sem);
    } else if (port->getDirection() == dir_out) {
        // build the source and the target
        globalSource = sem->explicitCast(signalRef, val_type, nullptr);
        delete signalRef;
        globalTarget = hif::copy(portassign->getValue());
        _createGlobalAction(globalTarget, globalSource, contents, sem);
    } else if (port->getDirection() == dir_inout) {
        // build the source and the target
        globalSource = sem->explicitCast(signalRef, val_type, nullptr);
        globalTarget = hif::copy(portassign->getValue());
        _createGlobalAction(globalTarget, globalSource, contents, sem);

        // build the source and the target
        globalSource = sem->explicitCast(portassign->getValue(), signalType, nullptr);
        globalTarget = signalRef;
        _createGlobalAction(globalTarget, globalSource, contents, sem);
    }

    if (globalSource == nullptr || globalTarget == nullptr) {
        delete signal;
        return false;
    }
    // 3- Change the value of the PortAssign with the new signal name and add
    //    the signal to the contents (including Generates) declarations.
    bool ret = _rebindPortAssign(portassign, signal);
    return ret;
}

// ///////////////////////////////////////////////////////////////////
// fixInstanceBindings()
// ///////////////////////////////////////////////////////////////////

bool _fixInstanceBindings(Instance *instance, hif::semantics::ILanguageSemantics *sem, const FixBindingOptions &opt)
{
    Entity *en = instantiate(instance, sem);
    if (en == nullptr)
        return false;

    bool input_need_fix        = false;
    bool output_need_fix       = false;
    bool input_is_optimizable  = true;
    bool output_is_optimizable = true;
    bool hasConstantInput      = false;
    bool hasConstantOutput     = false;
    ProcessesMap processesMap;
    for (BList<Port>::iterator i = en->ports.begin(); i != en->ports.end(); ++i) {
        Port *port          = *i;
        PortAssign *portAss = nullptr;
        for (BList<PortAssign>::iterator j = instance->portAssigns.begin(); j != instance->portAssigns.end(); ++j) {
            if (port->getName() != (*j)->getName())
                continue;

            // found port
            portAss = *j;
            break;
        }

        if (portAss == nullptr) {
            // not found this port... error or only skip?
            continue;
        }

        // allows only in, out and inout ports.
        if (port->getDirection() != dir_in && port->getDirection() != dir_out && port->getDirection() != dir_inout) {
            messageError("Unexpected direction", *i, nullptr);
        }

        // check presence of types.
        messageAssert(port->getType() != nullptr, "Expected type", *i, nullptr);

        // Check whether current binding is name-to-name
        if (!_isName2NameBinding(
                portAss, port->getDirection(), sem, input_is_optimizable, output_is_optimizable, hasConstantInput,
                hasConstantOutput, processesMap, opt)) {
            switch (port->getDirection()) {
            case dir_in:
                input_need_fix = true;
                break;
            case dir_out:
                output_need_fix = true;
                break;
            case dir_inout:
                input_need_fix  = true;
                output_need_fix = true;
                break;
            case dir_none:
            default:
                messageError("Unexpected port direction.", *i, sem);
            }
        }
    }

    // if at least one port need fix, do it on all port assign. This is a
    // workaround for avoid eventual deltacycle errors.
    bool needConstFix = (hasConstantInput || hasConstantOutput) && opt.fixConstants;
    if (!input_need_fix && !output_need_fix && !needConstFix) {
        return false;
    }

    // Get the contents/ifgen/forgen. This is important because the fix
    // adds a new global action, which is allowed in these objects only.
    BaseContents *bc = hif::getNearestParent<BaseContents>(instance);
    if (bc == nullptr) {
        messageError("Cannot find parent base contents", instance, sem);
    }
    GlobactMap globactMap;

    for (BList<Port>::iterator i = en->ports.begin(); i != en->ports.end(); ++i) {
        for (BList<PortAssign>::iterator j = instance->portAssigns.begin(); j != instance->portAssigns.end(); ++j) {
            if ((*i)->getName() != (*j)->getName())
                continue;
            bool isInput  = (*i)->getDirection() == dir_in || (*i)->getDirection() == dir_inout;
            bool isOutput = (*i)->getDirection() == dir_out || (*i)->getDirection() == dir_inout;
            if (isInput && !input_need_fix && !hasConstantInput)
                continue;
            if (isOutput && !output_need_fix && !hasConstantOutput)
                continue;

            Type *pAssT = hif::semantics::getSemanticType(*j, sem);
            messageAssert(pAssT != nullptr, "Cannot type port assign", *j, sem);

            // found port, perform fix.
            switch ((*i)->getDirection()) {
            case dir_in:
                if (dynamic_cast<ConstValue *>((*j)->getValue()) != nullptr) {
                    _addSignalForConstant(*j, hif::copy(pAssT), bc, sem);
                } else if (input_is_optimizable) {
                    if (!_addAssignInsideProcess(*j, processesMap, globactMap, sem))
                        return false;
                } else {
                    if (!_addSignalForPortassign(*j, bc, hif::copy(pAssT), sem))
                        return false;
                }
                break;
            case dir_out:
                // @TODO FIXME: no idea on how optimize outputs...
                // let's do this in the future...
                // if (output_is_optimizable)
                // {
                //     if (!_addReadInsideProcesses(*j, processesMap, sem)) return false;
                // }
                // else
                // {
                if (dynamic_cast<ConstValue *>((*j)->getValue()) != nullptr) {
                    _addSignalForConstant(*j, hif::copy(pAssT), bc, sem);
                } else if (!_addSignalForPortassign(*j, bc, hif::copy(pAssT), sem))
                    return false;
                // }
                break;
            case dir_inout:
                if (dynamic_cast<ConstValue *>((*j)->getValue()) != nullptr) {
                    _addSignalForConstant(*j, hif::copy(pAssT), bc, sem);
                } else if (input_is_optimizable || output_is_optimizable) {
                    messageError("TODO: this case should never happen!", *j, sem);
                } else {
                    if (!_addSignalForPortassign(*j, bc, hif::copy(pAssT), sem))
                        return false;
                }
                break;
            case dir_none:
            default:
                messageError("Unexpected portAssigns direction.", *j, sem);
            }
        }
    }

    if (!input_need_fix && !output_need_fix) {
        return false;
    }

    return true;
}
bool check_object_method(Object *o, const HifQueryBase *)
{
    if (dynamic_cast<Instance *>(o) == nullptr) {
        return false;
    }
    Instance *i = static_cast<Instance *>(o);
    if (!i->isInBList() || dynamic_cast<BaseContents *>(o->getParent()) == nullptr ||
        reinterpret_cast<BList<Object> *>(&static_cast<BaseContents *>(o->getParent())->instances) != i->getBList()) {
        return false;
    }

    return true;
}

} // namespace

bool fixInstanceBindings(Object *root, hif::semantics::ILanguageSemantics *sem, const FixBindingOptions &opt)
{
    hif::HifTypedQuery<Instance> q;
    q.check_object_method = &check_object_method;
    std::list<Instance *> list;
    hif::search(list, root, q);

    bool ret = false;
    std::string msg;
    for (std::list<Instance *>::iterator i = list.begin(); i != list.end(); ++i) {
        Instance *inst = *i;
        if (dynamic_cast<BaseContents *>(inst->getParent()) == nullptr)
            continue;

        bool done = _fixInstanceBindings(inst, sem, opt);
        if (done) {
            DesignUnit *du = hif::getNearestParent<DesignUnit>(inst);
            std::string duName("Unknown");
            if (du != nullptr)
                duName = du->getName();
            std::string unit = static_cast<ViewReference *>(inst->getReferencedType())->getDesignUnit();
            msg += "DesignUnit: " + duName + " - Instance: " + inst->getName() + " [" + unit + " " +
                   inst->getSourceInfoString() + "]" + "\n";
        }
        ret |= done;
    }

    if (ret) {
        raiseUniqueWarning(
            "Introduced some support signals to fix port bindings."
            " This could lead to a not equivalent design:\n" +
            msg);
    }

    return ret;
}

} // namespace manipulation
} // namespace hif
