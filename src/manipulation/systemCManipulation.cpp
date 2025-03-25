/// @file systemCManipulation.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/systemCManipulation.hpp"

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

typedef std::map<Identifier::DeclarationType *, std::vector<std::string>> DeclarationsMap;

// ///////////////////////////////////////////////////////////////////
// Last value fix part
// ///////////////////////////////////////////////////////////////////
FunctionCall *_mapRisingFalling(Object *o, Object *root, hif::semantics::ILanguageSemantics *sem, const bool isRising)
{
    FunctionCall *call = dynamic_cast<FunctionCall *>(o);
    messageAssert(call != nullptr && call->parameterAssigns.size() == 1, "Unexpected rising/falling reference", o, sem);

    if (!hif::isSubNode(call, root))
        return nullptr;

    Value *oper = hif::getChildSkippingCasts(call->parameterAssigns.front()->getValue());
    oper->replace(nullptr);

    HifFactory f(sem);

    FunctionCall *lastValue =
        f.functionCall("hif_vhdl_last_value", oper, f.noTemplateArguments(), f.noParameterArguments());

    // build for rising oper == '1' && oper'event && oper'last_value == '0'
    // build for falling oper == '0' && oper'event && oper'last_value == '1'
    Expression *e = f.expression(
        f.expression(
            f.functionCall("hif_vhdl_event", hif::copy(oper), f.noTemplateArguments(), f.noParameterArguments()),
            op_and,
            f.cast(
                f.boolean(),
                f.expression(
                    hif::copy(oper), op_case_eq, f.bitval((isRising ? bit_one : bit_zero), f.bit(true, false, true))))),
        op_and,
        f.cast(
            f.boolean(),
            f.expression(lastValue, op_case_eq, f.bitval((isRising ? bit_zero : bit_one), f.bit(true, false, true)))));

    o->replace(e);
    delete o;

    // add include to nearest scope
    Library *ref = f.library("hif_vhdl_standard", nullptr, "", false, true);
    Scope *s     = hif::getNearestScope(e, false, true, false);
    if (dynamic_cast<Contents *>(s) != nullptr) {
        Contents *c = static_cast<Contents *>(s);
        s           = static_cast<Scope *>(c->getParent());
    }
    hif::manipulation::AddUniqueObjectOptions addOpt;
    addOpt.deleteIfNotAdded = true;
    hif::manipulation::addUniqueObject(ref, s, addOpt);

    return lastValue;
}

bool _fixLastValueAsLibrary(
    FunctionCall *o,
    DeclarationsMap &declarations,
    hif::semantics::ILanguageSemantics *sem,
    const bool isInline)
{
    Identifier *id = dynamic_cast<Identifier *>(hif::getTerminalPrefix(hif::getChildSkippingCasts(o->getInstance())));
    messageAssert(id != nullptr, "Unexpeceted instance", o, sem);

    Identifier::DeclarationType *decl = hif::semantics::getDeclaration(id, sem);
    messageAssert(decl != nullptr, "Declaration not found", id, sem);

    HifFactory f(sem);

    if (declarations.find(decl) != declarations.end()) {
        DeclarationsMap::mapped_type names = declarations[decl];
        o->parameterAssigns.push_back(f.parameterArgument("param1", hif::copy(id)));
        if (!isInline) {
            o->parameterAssigns.push_back(f.parameterArgument("param2", new Identifier(names[0])));
            o->parameterAssigns.push_back(f.parameterArgument("param3", new Identifier(names[1])));
        }
        o->setName(names[2]);

        Value *newInst = isInline ? nullptr : f.libraryInstance("hif_systemc_hif_systemc_extensions", false, true);
        Value *inst    = o->setInstance(newInst);
        if (inst == id) {
            // simple case
            delete inst;
        } else {
            // case with possibly multiple-dimensional member
            o->replace(inst);
            id->replace(o);
            delete id;
        }

        hif::semantics::resetTypes(o);
        hif::semantics::resetDeclarations(o);
        return false;
    }

    // Not already fixed

    Contents *c = hif::getNearestParent<Contents>(o);
    messageAssert(c != nullptr, "Cannot find nearest contents", o, sem);

    // creating fresh names
    std::string last = hif::NameTable::getInstance()->getFreshName((std::string(id->getName()) + "_last").c_str());
    std::string prev = hif::NameTable::getInstance()->getFreshName((std::string(id->getName()) + "_prev").c_str());

    std::string fname =
        isInline ? hif::NameTable::getInstance()->getFreshName((std::string(id->getName()) + "_last_value").c_str())
                 : "hif_systemc_hif_lastValue";

    declarations[decl].push_back(last);
    declarations[decl].push_back(prev);
    declarations[decl].push_back(fname);

    // creating last and prev support variable.

    // using systemc sem for init values since this fix must
    // always work into the backend
    hif::semantics::ILanguageSemantics *sysc = hif::semantics::SystemCSemantics::getInstance();

    Type *idType = hif::semantics::getSemanticType(id, sem);
    messageAssert(idType != nullptr, "Unable to type id.", id, sem);

    Variable *var = f.variable(hif::copy(idType), last, hif::copy(decl->getValue()));
    if (var->getValue() == nullptr) {
        var->setValue(sysc->getTypeDefaultValue(var->getType(), nullptr));
    }

    Variable *varp = f.variable(hif::copy(idType), prev, hif::copy(decl->getValue()));
    if (varp->getValue() == nullptr) {
        varp->setValue(sysc->getTypeDefaultValue(varp->getType(), nullptr));
    }

    SubProgram *subprogram = hif::getNearestParent<SubProgram>(o);
    if (subprogram != nullptr) {
        BList<Declaration>::iterator pos(subprogram);
        pos.insert_before(var);
        pos.insert_before(varp);
    } else {
        c->declarations.push_back(var);
        c->declarations.push_back(varp);
    }

    // fixing function call
    {
        DeclarationsMap::mapped_type names = declarations[decl];
        o->parameterAssigns.push_back(f.parameterArgument("param1", hif::copy(id)));
        if (!isInline) {
            o->parameterAssigns.push_back(f.parameterArgument("param2", new Identifier(names[0])));
            o->parameterAssigns.push_back(f.parameterArgument("param3", new Identifier(names[1])));
        }
        o->setName(names[2]);

        Value *newInst = isInline ? nullptr : f.libraryInstance("hif_systemc_hif_systemc_extensions", false, true);
        Value *inst    = o->setInstance(newInst);
        if (inst == id) {
            // simple case
        } else {
            // case with possibly multiple-dimensional member
            o->replace(inst);
            id->replace(o);
        }
        hif::semantics::resetTypes(o);
        hif::semantics::resetDeclarations(o);
    }

    // creating the process that will manage the clock events.
    DeclarationsMap::mapped_type names = declarations[decl];
    StateTable *st                     = new StateTable();
    st->setName(hif::NameTable::getInstance()->getFreshName((std::string(id->getName()) + "_edge_process").c_str()));
    st->setFlavour(hif::pf_method);
    st->sensitivity.push_back(id);
    State *s = new State();
    s->setName("state");
    st->states.push_back(s);
    ProcedureCall *pcall = new ProcedureCall();
    pcall->setName(o->getName());
    hif::copy(o->parameterAssigns, pcall->parameterAssigns);
    pcall->setInstance(hif::copy(o->getInstance()));
    s->actions.push_back(pcall);
    c->stateTables.push_back(st);
    if (isInline) {
        HifFactory factory(sem);
        Function *foo = new Function();
        c->declarations.push_back(foo);
        foo->setName(names[2]);
        foo->setType(hif::copy(decl->getType()));
        foo->parameters.push_back(factory.parameter(hif::copy(decl->getType()), "param1"));
        StateTable *newSt = new StateTable();
        newSt->setName("lastValue");
        foo->setStateTable(newSt);
        State *newState = new State();
        newState->setName("lastValue");
        newSt->states.push_back(newState);
        If *ifSt = new If();
        newState->actions.push_back(ifSt);
        IfAlt *ifa = new IfAlt();
        ifSt->alts.push_back(ifa);
        Bool b;
        Type *condType = sem->getMapForType(&b);
        messageAssert(condType != nullptr, "Cannot map type", &b, sem);
        ifa->setCondition(
            factory.cast(condType, factory.expression(hif::copy(id), hif::op_case_neq, new Identifier(names[1]))));
        Assign *ass1 = factory.assignment(new Identifier(names[0]), new Identifier(names[1]));
        ifa->actions.push_back(ass1);
        Assign *ass2 = factory.assignment(new Identifier(names[1]), hif::copy(id));
        ifa->actions.push_back(ass2);
        Return *ret = new Return();
        ret->setValue(new Identifier(names[0]));
        newState->actions.push_back(ret);
    }

    return true;
}

} // namespace
bool mapLastValueToSystemC(Object *root, const LastValueOptions &opts)
{
    hif::semantics::ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance();

    System *s = dynamic_cast<System *>(root);
    if (s == nullptr)
        s = hif::getNearestParent<System>(root);

    messageAssert(s != nullptr, "Cannot reach System node", root, sem);

    // Search VHDL standard library
    LibraryDef *std     = nullptr;
    LibraryDef *lib1164 = nullptr;
    for (BList<LibraryDef>::iterator i = s->libraryDefs.begin(); i != s->libraryDefs.end(); ++i) {
        if ((*i)->getName() == "hif_vhdl_standard") {
            std = *i;
        } else if ((*i)->getName() == "hif_vhdl_ieee_std_logic_1164") {
            lib1164 = *i;
        }
    }

    // Assuming inclusion of std if using rising/falling edge
    if (std == nullptr)
        return false;

    // Search VHDL last_value declaration
    Function *lastValue = nullptr;
    for (BList<Declaration>::iterator i = std->declarations.begin(); i != std->declarations.end(); ++i) {
        Function *f = dynamic_cast<Function *>(*i);
        if (f == nullptr || f->getName() != "hif_vhdl_last_value")
            continue;
        lastValue = f;
        break;
    }

    // Search VHDL last_value declaration
    Function *rising  = nullptr;
    Function *falling = nullptr;
    if (lib1164 != nullptr && opts.replaceRisingFallingEdge) {
        for (BList<Declaration>::iterator i = lib1164->declarations.begin(); i != lib1164->declarations.end(); ++i) {
            Function *f = dynamic_cast<Function *>(*i);
            if (f == nullptr)
                continue;
            if (f->getName() == "hif_vhdl_rising_edge")
                rising = f;
            else if (f->getName() == "hif_vhdl_falling_edge")
                falling = f;
        }
    }

    typedef std::set<Object *> SymbolSet;
    typedef std::map<Declaration *, SymbolSet> RefMap;
    RefMap refMap;
    hif::semantics::GetReferencesOptions get_refs_opts;
    get_refs_opts.error = false;
    hif::semantics::getAllReferences(refMap, sem, s, get_refs_opts);

    SymbolSet &lastValueRefs = refMap[lastValue];

    // If required and found expand rising and falling edge
    if (rising != nullptr) {
        SymbolSet &set = refMap[rising];
        for (SymbolSet::iterator i = set.begin(); i != set.end(); ++i) {
            Object *r = _mapRisingFalling(*i, root, sem, true);
            if (r != nullptr)
                lastValueRefs.insert(r);
        }
    }

    if (falling != nullptr) {
        SymbolSet &set = refMap[falling];
        for (SymbolSet::iterator i = set.begin(); i != set.end(); ++i) {
            Object *r = _mapRisingFalling(*i, root, sem, false);
            if (r != nullptr)
                lastValueRefs.insert(r);
        }
    }

    // Fix last value occurrences
    bool ret = false;
    DeclarationsMap declarations;

    for (SymbolSet::iterator i = lastValueRefs.begin(); i != lastValueRefs.end(); ++i) {
        // Sanity checks
        FunctionCall *f = dynamic_cast<FunctionCall *>(*i);
        messageAssert(f != nullptr, "Unxepected reference", *i, sem);

        if (!hif::isSubNode(f, root))
            continue;

        StateTable *p    = hif::getNearestParent<StateTable>(f);
        GlobalAction *p2 = hif::getNearestParent<GlobalAction>(f);
        messageAssert(
            (p != nullptr && dynamic_cast<Contents *>(p->getParent()) != nullptr) || p2 != nullptr,
            "Found last value inside unexpected scope", f, sem);

        // Actual fix
        const bool firstFixInScope = _fixLastValueAsLibrary(f, declarations, sem, opts.inlineLastValue);
        ret |= firstFixInScope;

        if (!firstFixInScope | opts.inlineLastValue)
            continue;

        // Adding inclusion of support library
        Contents *scope = hif::getNearestParent<Contents>(f);
        Library *ref    = new Library();
        ref->setName("hif_systemc_hif_systemc_extensions");
        ref->setStandard(false);
        ref->setSystem(true);
        hif::manipulation::AddUniqueObjectOptions addOpt;
        addOpt.deleteIfNotAdded = true;
        hif::manipulation::addUniqueObject(ref, scope, addOpt);
    }

    if (!opts.inlineLastValue) {
        // Adding support library
        LibraryDef *ld = sem->getStandardLibrary("hif_systemc_hif_systemc_extensions");
        hif::manipulation::AddUniqueObjectOptions addOpt;
        addOpt.equalsOptions.checkOnlyNames = true;
        addOpt.position                     = 0u;
        hif::manipulation::addUniqueObject(ld, s->libraryDefs, addOpt);
    }
    return ret;
}

bool addModuleConstructor(View *view, semantics::ILanguageSemantics *sem)
{
    hif::HifFactory f(sem);
    DesignUnit *du = dynamic_cast<DesignUnit *>(view->getParent());
    messageAssert(du != nullptr, "Unexpected parent", view, sem);

    Function *ctor = f.classConstructor(du, (f.parameter(f.pointer(f.character()), "name")), f.noTemplates());
    ctor->setStandard(true);

    hif::manipulation::AddUniqueObjectOptions addOpt;
    addOpt.deleteIfNotAdded = true;
    addOpt.position         = 0;
    return hif::manipulation::addUniqueObject(ctor, view->declarations, addOpt);
}

LastValueOptions::LastValueOptions()
    : replaceRisingFallingEdge(true)
    , inlineLastValue(false)
{
    // ntd
}

LastValueOptions::~LastValueOptions()
{
    // ntd
}

LastValueOptions::LastValueOptions(const LastValueOptions &other)
    : replaceRisingFallingEdge(other.replaceRisingFallingEdge)
    , inlineLastValue(other.inlineLastValue)
{
    // ntd
}

LastValueOptions &LastValueOptions::operator=(LastValueOptions other)
{
    swap(other);
    return *this;
}

void LastValueOptions::swap(LastValueOptions &other)
{
    std::swap(replaceRisingFallingEdge, other.replaceRisingFallingEdge);
    std::swap(inlineLastValue, other.inlineLastValue);
}

} // namespace manipulation
} // namespace hif
