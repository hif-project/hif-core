/// @file flattenDesign.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/flattenDesign.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/search.hpp"
#include "hif/semantics/semantics.hpp"
#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#endif

// Uncomment to obtain debug output prints
//#define FLATTENER_PRINT_DEBUG
#ifndef NDEBUG
// Uncomment to print debug steps
//#define DEBUG_STEPS
#endif

namespace hif
{
namespace manipulation
{

namespace
{ // anonymous
class Flattener
{
public:
    // Typedef for the set of Objects type.
    typedef std::set<hif::Object *> ObjectsSet;
    // Typedef for the map of references type.
    typedef std::map<hif::Declaration *, ObjectsSet> ReferenceMap;

    Flattener(
        hif::System *sys,
        hif::semantics::ILanguageSemantics *sem,
        const std::string &topLevelName,
        const std::set<std::string> &rdu,
        const std::set<std::string> &ri);

    ~Flattener();

    void flattenDesign();
    void printWarnings();

private:
    hif::System *_system;
    hif::semantics::ILanguageSemantics *_sem;
    std::list<hif::Instance *> _rootInstances;
    ViewSet _topViews;
    hif::NameTable *_nameTable;
    ViewDependenciesMap _submodulesMap;
    hif::HifFactory _factory;
    hif::application_utils::WarningInfoSet _initialValueWarnings1;
    hif::application_utils::WarningInfoSet _initialValueWarnings2;
    hif::application_utils::WarningInfoSet _initialValueWarnings3;
    hif::application_utils::WarningInfoSet _initialValueWarnings4;
    hif::application_utils::WarningInfoSet _initialValueWarnings5;

    class ObjectCompare
    {
    public:
        bool operator()(const hif::Object *o1, const hif::Object *o2) const;
    };

    void _findTopLevel(const std::string &topLevelName);
    void _collectRoots(
        const std::set<std::string> &rdu,
        const std::set<std::string> &ri,
        std::set<hif::Instance *> &instances);
    void _sortRootInstances(std::set<hif::Instance *> &instances);
    void _flattenSubtreeFromView(hif::View *view);
    void _flattenInstance(hif::Instance *instance);
    void _insertNewView(hif::View *originalDecl, hif::View *newView, hif::ViewReference *vr);
    hif::Value *_extractBoundValue(const std::string &n, hif::BList<hif::PortAssign> &bindings);
    void _renameDeclarations(hif::View *view, const std::string &prefix, hif::Instance *instance);
    void _propagateBoundInitialValue(Port *port, Value *v);
    void _propagateConcatInitialValue(Expression *expr, Value *source);
    void _propagateConcatInitialValueToPrefixedReference(
        PrefixedReference *pr,
        const std::string &str,
        std::uint64_t &index);
    bool _decomposeConcat(Value *v, std::list<Value *> &members);
    void _propagateMemberInitialValue(Member *m, DataDeclaration *destination, Value *source);
    void _propagateSliceInitialValue(Slice *s, DataDeclaration *destination, Value *source);
    void _renameReferences(const std::string &newName, ObjectsSet &refs);
    void _propagateLibraries(hif::Scope *target, hif::Scope *source);
    void _expandDeclarationsList(
        hif::BList<hif::Declaration> &target,
        hif::BList<hif::Declaration> &source,
        const std::string &prefix);
    void _expandContents(hif::Contents *target, hif::Contents *source, const std::string &prefix);
    void _cleanUpTree();
    bool _isTopLevelDescendant(hif::View *v, ViewDependenciesMap &parentModulesMap);
    void _extractSensitivityItemsFromBoundValue(std::set<hif::Value *, ObjectCompare> &items, hif::Value *bound);

    // Disabled copy constructor.
    Flattener(const Flattener &);

    // Disabled assignment operator.
    Flattener &operator=(const Flattener &);
};

Flattener::Flattener(
    System *sys,
    hif::semantics::ILanguageSemantics *sem,
    const std::string &topLevelName,
    const std::set<std::string> &rdu,
    const std::set<std::string> &ri)
    : _system(sys)
    , _sem(sem)
    , _rootInstances()
    , _topViews()
    , _nameTable(hif::NameTable::getInstance())
    , _submodulesMap()
    , _factory(sem)
    , _initialValueWarnings1()
    , _initialValueWarnings2()
    , _initialValueWarnings3()
    , _initialValueWarnings4()
    , _initialValueWarnings5()
{
    ViewDependenciesMap parentModulesMap;
    findViewDependencies(_system, _submodulesMap, parentModulesMap, _sem);
    FindTopOptions opt;
    opt.smm             = &_submodulesMap;
    opt.pmm             = &parentModulesMap;
    opt.checkAtLeastOne = true;
    opt.topLevelName    = topLevelName;
    _topViews           = hif::manipulation::findTopLevelModules(sys, sem, opt);

    std::set<Instance *> instances;
    _collectRoots(rdu, ri, instances);
    if (!instances.empty())
        _sortRootInstances(instances);
}

Flattener::~Flattener()
{
    // ntd
}

void Flattener::flattenDesign()
{
    // Bind open port bindings (if any)
    hif::manipulation::bindOpenPortAssigns(*_system, _sem);

    if (_rootInstances.empty()) {
        // Flattening of the whole description (from the top-level views)
        for (ViewSet::iterator i = _topViews.begin(); i != _topViews.end(); ++i) {
            View *view = *i;
            _flattenSubtreeFromView(view);
        }
    } else {
        // Partial flattening according to the provided root instances
        for (std::list<hif::Instance *>::iterator iter = _rootInstances.begin(); iter != _rootInstances.end(); ++iter) {
            Instance *i = *iter;
#ifdef FLATTENER_PRINT_DEBUG
            std::string instanceName(hif::manipulation::buildHierarchicalSymbol(i->getReferencedType(), _sem));
            std::cout << "Flattening instance " << instanceName << "..." << std::endl;
#endif
            _flattenInstance(i);
#ifdef FLATTENER_PRINT_DEBUG
            hif::writeFile("after_flat_" + instanceName, _system, false);
#endif
        }
    }

    _cleanUpTree();
}

void Flattener::printWarnings()
{
    messageWarningList(
        true,
        "Unable to propagate initial value of child output port to parent bound value "
        "in non-name-to-name binding. This could lead to a non-equivalent design description. (1)",
        _initialValueWarnings1);

    messageWarningList(
        true,
        "Unable to propagate initial value of child output port to parent bound value "
        "in non-name-to-name binding. This could lead to a non-equivalent design description. (2)",
        _initialValueWarnings2);

    messageWarningList(
        true,
        "Unable to propagate initial value of child output port to parent bound value "
        "in non-name-to-name binding. This could lead to a non-equivalent design description. (3)",
        _initialValueWarnings3);

    messageWarningList(
        true,
        "Unable to propagate initial value of child output port to parent bound value "
        "in non-name-to-name binding. This could lead to a non-equivalent design description. (4)",
        _initialValueWarnings4);

    messageWarningList(
        true,
        "Unable to propagate initial value of child output port to parent bound value "
        "in non-name-to-name binding. This could lead to a non-equivalent design description. (5)",
        _initialValueWarnings5);
}

void Flattener::_collectRoots(
    const std::set<std::string> &rdu,
    const std::set<std::string> &ri,
    std::set<Instance *> &instances)
{
    if (rdu.empty() && ri.empty())
        return;

    for (std::set<std::string>::const_iterator iter = rdu.begin(); iter != rdu.end(); ++iter) {
        std::string duName(*iter);
        Object *ref = hif::manipulation::resolveHierarchicalSymbol(duName, _system, _sem);
        View *v     = dynamic_cast<View *>(ref);
        if (v == nullptr)
            continue;
        hif::semantics::ReferencesSet refs;
        hif::semantics::getReferences(v, refs, _sem, _system);
        for (hif::semantics::ReferencesSet::iterator jter = refs.begin(); jter != refs.end(); ++jter) {
            ViewReference *vr = dynamic_cast<ViewReference *>(*jter);
            if (vr == nullptr)
                continue;
            Instance *inst = dynamic_cast<Instance *>(vr->getParent());
            if (inst == nullptr)
                continue;
            instances.insert(inst);
        }
    }

    for (std::set<std::string>::const_iterator iter = ri.begin(); iter != ri.end(); ++iter) {
        std::string instanceName(*iter);
        Object *ref    = hif::manipulation::resolveHierarchicalSymbol(instanceName, _system, _sem);
        Instance *inst = dynamic_cast<Instance *>(ref);
        if (inst == nullptr)
            continue;
        instances.insert(inst);
    }
}

void Flattener::_sortRootInstances(std::set<Instance *> &instances)
{
    std::map<View *, std::set<Instance *>> instancesByView;
    std::set<View *> toBeProcessed;
    for (std::set<Instance *>::iterator i = instances.begin(); i != instances.end(); ++i) {
        Instance *inst = *i;
        View *v        = dynamic_cast<View *>(hif::semantics::getDeclaration(inst->getReferencedType(), _sem));
        if (v == nullptr) {
            messageError("Unable to reorder root instances according to design hierarchy.", nullptr, nullptr);
        }
        if (instancesByView.find(v) == instancesByView.end())
            instancesByView[v];
        instancesByView[v].insert(inst);
        toBeProcessed.insert(v);
    }
    // Root instances must be sorted from children to parent in order for
    // _flattenSubtreeFromInstance() to work properly
    unsigned int cycles     = 0;
    unsigned int viewsCount = static_cast<unsigned int>(toBeProcessed.size());
    // The loop to sort root views is expected to take at most as many iterations
    // as the number of views to be sorted
    while (!toBeProcessed.empty() && (cycles < viewsCount)) {
        for (std::set<View *>::iterator i = toBeProcessed.begin(); i != toBeProcessed.end();) {
            View *v                         = *i;
            std::set<View *> &submodulesSet = _submodulesMap[v];
            bool allSubmodulesProcessed     = true;
            for (std::set<View *>::iterator j = submodulesSet.begin(); j != submodulesSet.end(); ++j) {
                if (toBeProcessed.find(*j) == toBeProcessed.end())
                    continue;
                allSubmodulesProcessed = false;
                break;
            }
            if (!allSubmodulesProcessed) {
                ++i;
                continue;
            }
            for (std::set<Instance *>::iterator j = instancesByView[v].begin(); j != instancesByView[v].end(); ++j) {
                _rootInstances.push_back(*j);
            }
            toBeProcessed.erase(i++);
        }
        ++cycles;
    }
    if (_rootInstances.size() < instances.size()) {
        messageError("Unable to reorder root instances according to design hierarchy.", nullptr, nullptr);
    }
#ifdef FLATTENER_PRINT_DEBUG
    for (std::list<Instance *>::iterator i = _rootInstances.begin(); i != _rootInstances.end(); ++i) {
        ViewReference *vr = dynamic_cast<ViewReference *>((*i)->getReferencedType());
        if (vr == nullptr)
            continue;
        std::cout << "Root instance: " << hif::manipulation::buildHierarchicalSymbol(vr, _sem) << " ("
                  << vr->getDesignUnit().c_str() << ")" << std::endl;
    }
#endif
}

void Flattener::_flattenSubtreeFromView(View *view)
{
    messageDebugAssert(view != nullptr, "View is nullptr", nullptr, nullptr);
    if (view == nullptr)
        return;
    Contents *contents = view->getContents();
    messageDebugAssert(contents != nullptr, "Contents are nullptr", nullptr, nullptr);
    if (contents == nullptr)
        return;
    for (BList<Instance>::iterator iter = contents->instances.begin(); iter != contents->instances.end();) {
        Instance *instance = *iter;
        ViewReference *vr  = dynamic_cast<ViewReference *>(instance->getReferencedType());
        messageAssert(vr != nullptr, "Unexpected case (1)", instance->getReferencedType(), _sem);

        ViewReference::DeclarationType *tmpView          = hif::manipulation::instantiate(vr, _sem);
        ViewReference::DeclarationType *instantiatedView = hif::copy(tmpView);
        messageAssert(instantiatedView != nullptr, "Unexpected case (2)", vr, _sem);
        hif::semantics::mapDeclarationsInTree(instantiatedView, tmpView, instantiatedView, _sem);
        ViewReference::DeclarationType *originalDecl = hif::semantics::getDeclaration(vr, _sem);

        _insertNewView(originalDecl, instantiatedView, vr);

        std::string newInstanceName = _nameTable->getFreshName(instance->getName());
        std::string originalName    = instance->getName();
        instance->setName(newInstanceName);
        hif::semantics::setDeclaration(instance, instantiatedView->getEntity());
        instantiatedView->templateParameters.clear();
        vr->templateParameterAssigns.clear();

        hif::manipulation::expandGenerates(instantiatedView, _sem);

        _renameDeclarations(instantiatedView, originalName, instance);
        _propagateLibraries(view, instantiatedView);
        _expandDeclarationsList(view->declarations, instantiatedView->declarations, originalName);
        _expandContents(contents, instantiatedView->getContents(), originalName);

        iter = iter.erase();
    }
}

void Flattener::_flattenInstance(Instance *instance)
{
    messageAssert(instance != nullptr, "Instance is nullptr", nullptr, nullptr);
    ViewReference *vr = dynamic_cast<ViewReference *>(instance->getReferencedType());
    messageAssert(vr != nullptr, "Unexpected case (1)", instance->getReferencedType(), _sem);
    ViewReference::DeclarationType *tmpView          = hif::manipulation::instantiate(vr, _sem);
    ViewReference::DeclarationType *instantiatedView = hif::copy(tmpView);
    messageAssert(instantiatedView != nullptr, "Unexpected case (2)", vr, _sem);
    hif::semantics::mapDeclarationsInTree(instantiatedView, tmpView, instantiatedView, _sem);
    ViewReference::DeclarationType *originalDecl = hif::semantics::getDeclaration(vr, _sem);
    _insertNewView(originalDecl, instantiatedView, vr);

    std::string newInstanceName = _nameTable->getFreshName(instance->getName());
    std::string originalName    = instance->getName();
    instance->setName(newInstanceName);
    hif::semantics::setDeclaration(instance, instantiatedView->getEntity());
    instantiatedView->templateParameters.clear();
    vr->templateParameterAssigns.clear();

    hif::manipulation::expandGenerates(instantiatedView, _sem);

    _renameDeclarations(instantiatedView, originalName, instance);
    Contents *contents = dynamic_cast<Contents *>(instance->getParent());
    messageDebugAssert(contents != nullptr, "Unexpected non-Contents parent", instance->getParent(), _sem);
    View *parentView = dynamic_cast<View *>(contents->getParent());
    messageAssert(parentView != nullptr, "Unexpected non-View parent", contents->getParent(), _sem);

    _propagateLibraries(parentView, instantiatedView);
    _expandDeclarationsList(parentView->declarations, instantiatedView->declarations, originalName);
    _expandContents(contents, instantiatedView->getContents(), originalName);

#ifdef FLATTENER_PRINT_DEBUG
    Contents *c    = dynamic_cast<Contents *>(instance->getBList()->getParent());
    DesignUnit *du = dynamic_cast<DesignUnit *>(c->getParent()->getParent());
    if (du != nullptr) {
        std::cout << "\tRemoving instance " << instance->getName() << " from design unit " << du->getName()
                  << std::endl;
    }
#endif

    // Remove flattened instance
    BList<Instance>::iterator iter(instance);
    iter.erase();

#ifdef FLATTENER_PRINT_DEBUG
    if (du != nullptr) {
        std::cout << "\tInstances after removal:" << std::endl;
        for (BList<Instance>::iterator i = c->instances.begin(); i != c->instances.end(); ++i) {
            if ((*i) == instance)
                continue;
            std::cout << "\t- " << (*i)->getName() << std::endl;
        }
    }
#endif

    // Flushing instance cache is required, otherwise following calls to
    // hif::manipulation::instantiate() may return an instantiated view
    // that still contains flattened instances of submodules
    hif::manipulation::flushInstanceCache();
}

void Flattener::_insertNewView(View *originalDecl, View *newView, ViewReference *vr)
{
    Scope *context = hif::getNearestParent<LibraryDef>(originalDecl);
    if (context == nullptr) {
        context = hif::getNearestParent<System>(originalDecl);
    }
    messageAssert(context != nullptr, "Contents not found", nullptr, _sem);
    std::string newUnitName          = _nameTable->getFreshName(vr->getDesignUnit().c_str());
    BList<Declaration> *declarations = hif::objectGetDeclarationList(context);
    DesignUnit *newUnit              = new DesignUnit();
    newUnit->setName(newUnitName);
    newUnit->views.push_back(newView);
    vr->setDesignUnit(newUnitName);
    hif::semantics::setDeclaration(vr, newView);
    declarations->push_back(newUnit);
}

Value *Flattener::_extractBoundValue(const std::string &n, BList<PortAssign> &bindings)
{
    for (BList<PortAssign>::iterator iter = bindings.begin(); iter != bindings.end(); ++iter) {
        PortAssign *bind = *iter;
        if (bind->getName() != n)
            continue;
        return bind->getValue();
    }
    return nullptr;
}

void Flattener::_renameDeclarations(View *view, const std::string &prefix, Instance *instance)
{
    ReferenceMap refMap;
    hif::semantics::GetReferencesOptions opt;
    opt.include_unreferenced = true;
    hif::semantics::getAllReferences(refMap, _sem, view, opt);
    for (ReferenceMap::iterator iter = refMap.begin(); iter != refMap.end(); ++iter) {
        Declaration *decl = iter->first;
        ObjectsSet &set   = iter->second;

        if (dynamic_cast<Port *>(decl) != nullptr) {
            // Replace port references with bound value
            Port *port = static_cast<Port *>(decl);
            Value *v   = _extractBoundValue(port->getName(), instance->portAssigns);
            if (v == nullptr) {
                if (!hif::isSubNode(port, view->getEntity())) {
                    // Skip references to bound ports of submodule instances
                    continue;
                } else {
                    // Since open port bindings should have been previously bound,
                    // raise an error if no bound value is found
                    messageError("Unexpected open port binding", port, _sem);
                }
            }
            // If the child port is an output port and it is directly bound to a parent output port
            // or a parent signal, replace the initial value of the parent port (or signal) with
            // the initial value of the child port
            // At the moment only the cases when the bound value is a symbol or a member or a slice are handled.
            // If the bound value is an expression, this fix will not be performed.
            if (port->getDirection() == dir_out)
                _propagateBoundInitialValue(port, hif::getChildSkippingCasts(v));
            hif::semantics::getSemanticType(v, _sem);
            hif::CopyOptions copt;
            copt.copySemanticsTypes = true;
            v                       = hif::copy(v, copt);
            if (!_sem->isSliceTypeRebased() && dynamic_cast<Slice *>(v) != nullptr) {
                // In this case, it could happen:
                // port => v[7:4]
                // portRef[3] = expr
                // -->
                // v[7:4][3] = expr           -- which is wrong, since no slice
                //                               rebase!
                // (([3:0]) v[7:3])[3] = expr -- which is fine, hoping that simplify()
                //                               will remove that cast
                // Ref design: verilog/yogitech/m6502_original
                Cast *c = new Cast();
                c->setValue(v);
                c->setType(hif::copy(port->getType()));
                v = c;
            }
            bool isBoundToCast = dynamic_cast<Cast *>(v) != nullptr;
            for (ObjectsSet::iterator jter = set.begin(); jter != set.end(); ++jter) {
                Object *obj = *jter;
                // Skip replacement of PortAssign objects
                if (dynamic_cast<PortAssign *>(obj) != nullptr)
                    continue;
                // If the occurrence is within a sensitivity list, replace it with
                // all the identifiers appearing in the bound value
                hif::ObjectSensitivityOptions opts;
                opts.considerFunctionCalls = false;
                if (hif::objectIsInSensitivityList(obj, opts)) {
                    std::set<Value *, ObjectCompare> items;
                    _extractSensitivityItemsFromBoundValue(items, v);
                    Value *v2 = dynamic_cast<Value *>(obj);
                    if (v2 == nullptr)
                        continue;
                    if (!v2->isInBList()) {
                        auto tmp = dynamic_cast<Value *>(v2->getParent());
                        messageAssert(tmp != nullptr, "Unexpected parent (1)", v2->getParent(), _sem);
                        v2 = tmp;
                        while (!v2->isInBList()) {
                            tmp = dynamic_cast<Value *>(v2->getParent());
                            messageAssert(tmp != nullptr, "Unexpected parent (2)", v2->getParent(), _sem);
                            v2 = tmp;
                        }
                    }
                    BList<Value>::iterator sensIter(v2);
                    for (std::set<Value *>::iterator kter = items.begin(); kter != items.end(); ++kter) {
                        sensIter.insert_after(*kter);
                    }
                    sensIter.erase();
                    continue;
                }
                // If the bound value is a cast, and the occurrence appears in the LHS
                // of an assignment, remove the cast from the LHS, and put a corresponding
                // cast on the RHS of the assignment
                else if (isBoundToCast && hif::manipulation::isInLeftHandSide(obj)) {
                    Assign *a = hif::getNearestParent<Assign>(obj);
                    if (a == nullptr)
                        continue;
                    hif::semantics::resetTypes(a->getLeftHandSide());
                    Value *rhs = a->getRightHandSide();
                    // Check previous comment about this design and
                    // slice no-rabasing sems
                    // Ref design: verilog/yogitech/m6502_original
                    //v = hif::getChildSkippingCasts(v);
                    obj->replace(hif::copy(v, copt));
                    delete obj;
                    Type *t = hif::semantics::getSemanticType(a->getLeftHandSide(), _sem);
                    Cast *c = new Cast();
                    c->setType(hif::copy(t));
                    c->setValue(rhs);
                    a->setRightHandSide(c);
                    continue;
                } else if (hif::manipulation::isInLeftHandSide(obj)) {
                    Assign *a = hif::getNearestParent<Assign>(obj);
                    if (a == nullptr)
                        continue;
                    // Resetting semantic type to ensure that it is updated
                    // according with the new LHS. E.g. obj is the prefix of a member.
                    // But always preserve v original type, since it must be calculated
                    // w.r.t v original scope (into witch it will be expanded),
                    // and not the new one.
                    // Ref design: vhdl/openCores/color_converter_original + hr
                    hif::semantics::resetTypes(a->getLeftHandSide());
                    obj->replace(hif::copy(v, copt));
                    delete obj;
                    Type *t = hif::semantics::getSemanticType(a->getLeftHandSide(), _sem);
                    if (t == nullptr)
                        continue;
                    a->setRightHandSide(_factory.cast(hif::copy(t), a->setRightHandSide(nullptr)));
                    continue;
                }
                obj->replace(_factory.cast(hif::copy(port->getType()), hif::copy(v)));
                delete obj;
            }
            delete v;
        } else if (dynamic_cast<TypeDef *>(decl) != nullptr) {
            if (!hif::isSubNode(decl, view))
                continue;
            std::string newName = _nameTable->getFreshName((prefix + "_" + decl->getName()).c_str());
            decl->setName(newName);
            _renameReferences(newName, set);
        } else if (dynamic_cast<DataDeclaration *>(decl) != nullptr) {
            if (!hif::isSubNode(decl, view))
                continue;
            if ((dynamic_cast<ValueTP *>(decl) != nullptr) || (dynamic_cast<Parameter *>(decl) != nullptr) ||
                (dynamic_cast<Field *>(decl) != nullptr))
                continue;
            std::string newName = _nameTable->getFreshName((prefix + "_" + decl->getName()).c_str());
            decl->setName(newName);
            _renameReferences(newName, set);
        } else if (dynamic_cast<SubProgram *>(decl) != nullptr) {
            if (!hif::isSubNode(decl, view))
                continue;
            std::string newName = _nameTable->getFreshName((prefix + "_" + decl->getName()).c_str());
            decl->setName(newName);
            _renameReferences(newName, set);
        }
    }
    // Rename state tables (i.e., processes)
    Contents *contents = view->getContents();
    for (BList<StateTable>::iterator iter = contents->stateTables.begin(); iter != contents->stateTables.end();
         ++iter) {
        StateTable *st      = *iter;
        std::string newName = _nameTable->getFreshName((prefix + "_" + st->getName()).c_str());
        st->setName(newName);
    }
}

void Flattener::_propagateBoundInitialValue(Port *port, Value *v)
{
    hif::features::ISymbol *symbol = dynamic_cast<hif::features::ISymbol *>(v);
    if (symbol != nullptr) {
        DataDeclaration *parentDecl = dynamic_cast<DataDeclaration *>(hif::semantics::getDeclaration(v, _sem));
        Port *parentPort            = dynamic_cast<Port *>(parentDecl);
        Signal *parentSignal        = dynamic_cast<Signal *>(parentDecl);
        if ((port->getValue() != nullptr) &&
            ((parentSignal != nullptr) || ((parentPort != nullptr) && (parentPort->getDirection() == dir_out)))) {
            // Replace initial value of the parent port (or signal)
            Cast *initVal = new Cast();
            initVal->setType(hif::copy(parentDecl->getType()));
            initVal->setValue(hif::copy(port->getValue()));
            delete parentDecl->setValue(initVal);
        }
        return;
    }
    Expression *e = dynamic_cast<Expression *>(v);
    if (e != nullptr) {
        if (port->getValue() == nullptr) {
            // Consider default value for port type
            port->setValue(_sem->getTypeDefaultValue(port->getType(), port));
        }
        _propagateConcatInitialValue(e, hif::getChildSkippingCasts(port->getValue()));
        return;
    }
    PrefixedReference *pr = dynamic_cast<PrefixedReference *>(v);
    if (pr != nullptr) {
        symbol = dynamic_cast<hif::features::ISymbol *>(pr->getPrefix());
    }
    if (symbol == nullptr) {
        hif::application_utils::WarningInfo info(v);
        _initialValueWarnings1.insert(info);
        return;
    }
    DataDeclaration *parentDecl =
        dynamic_cast<DataDeclaration *>(hif::semantics::getDeclaration(pr->getPrefix(), _sem));
    Port *parentPort = dynamic_cast<Port *>(parentDecl);
    if ((port->getValue() == nullptr) || ((parentPort != nullptr) && (parentPort->getDirection() != dir_out)))
        return;
    Member *m = dynamic_cast<Member *>(pr);
    Slice *s  = dynamic_cast<Slice *>(pr);
    if (m != nullptr) {
        _propagateMemberInitialValue(m, parentDecl, port->getValue());
    } else if (s != nullptr) {
        _propagateSliceInitialValue(s, parentDecl, port->getValue());
    } else {
        hif::application_utils::WarningInfo info(v);
        _initialValueWarnings1.insert(info);
    }
}

void Flattener::_propagateConcatInitialValue(Expression *expr, Value *source)
{
    std::list<Value *> members;
    BitvectorValue *bvs = dynamic_cast<BitvectorValue *>(source);
    if (!_decomposeConcat(expr, members) || (bvs == nullptr)) {
        hif::application_utils::WarningInfo info(source);
        _initialValueWarnings2.insert(info);
        return;
    }
    std::string str(bvs->getValue());
    std::uint64_t index = 0;
    for (std::list<Value *>::iterator iter = members.begin(); iter != members.end(); ++iter) {
        Value *v              = *iter;
        PrefixedReference *pr = dynamic_cast<PrefixedReference *>(v);
        if (pr != nullptr) {
            _propagateConcatInitialValueToPrefixedReference(pr, str, index);
            continue;
        }
        hif::features::ISymbol *symbol = dynamic_cast<hif::features::ISymbol *>(v);
        if (symbol == nullptr) {
            hif::application_utils::WarningInfo info(v);
            _initialValueWarnings2.insert(info);
            continue;
        }
        DataDeclaration *d = dynamic_cast<DataDeclaration *>(hif::semantics::getDeclaration(v, _sem));
        if (d->getValue() == nullptr) {
            d->setValue(_sem->getTypeDefaultValue(d->getType(), d));
        }
        Bit *t1      = dynamic_cast<Bit *>(d->getType());
        BitValue *bi = dynamic_cast<BitValue *>(d->getValue());
        if ((t1 != nullptr) && (bi != nullptr)) {
            bi->setValue(hif::bitConstantFromString(str.substr(static_cast<std::string::size_type>(index), 1)));
            ++index;
            continue;
        }
        Bitvector *t2       = dynamic_cast<Bitvector *>(d->getType());
        BitvectorValue *bvd = dynamic_cast<BitvectorValue *>(d->getValue());
        if ((t2 == nullptr) || (bvd == nullptr)) {
            hif::application_utils::WarningInfo info(v);
            _initialValueWarnings2.insert(info);
            continue;
        }
        std::uint64_t w = hif::semantics::spanGetBitwidth(t2->getSpan(), _sem);
        bvd->setValue(str.substr(static_cast<std::string::size_type>(index), static_cast<std::string::size_type>(w)));
        index += w;
    }
}

void Flattener::_propagateConcatInitialValueToPrefixedReference(
    PrefixedReference *pr,
    const std::string &str,
    std::uint64_t &index)
{
    hif::HifFactory factory(_sem);
    Member *m                   = dynamic_cast<Member *>(pr);
    Slice *s                    = dynamic_cast<Slice *>(pr);
    DataDeclaration *parentDecl = nullptr;
    Type *t                     = nullptr;
    if ((m != nullptr) || (s != nullptr)) {
        parentDecl = dynamic_cast<DataDeclaration *>(hif::semantics::getDeclaration(pr->getPrefix(), _sem));
        if (parentDecl == nullptr) {
            hif::application_utils::WarningInfo info(pr);
            _initialValueWarnings3.insert(info);
            return;
        }
        t = parentDecl->getType();
    }
    if (m != nullptr) {
        BitValue *bi = factory.bitval(
            hif::bitConstantFromString(str.substr(static_cast<std::string::size_type>(index), 1)),
            factory.bit(hif::typeIsLogic(t, _sem), hif::typeIsResolved(t, _sem), hif::typeIsConstexpr(t, _sem)));
        _propagateMemberInitialValue(m, parentDecl, bi);
        ++index;
        return;
    } else if (s != nullptr) {
        std::uint64_t w    = hif::semantics::spanGetBitwidth(s->getSpan(), _sem);
        BitvectorValue *bv = factory.bitvectorval(
            str.substr(static_cast<std::string::size_type>(index), static_cast<std::string::size_type>(w)),
            factory.bitvector(
                hif::copy(s->getSpan()), hif::typeIsLogic(t, _sem), hif::typeIsResolved(t, _sem),
                hif::typeIsConstexpr(t, _sem)));
        _propagateSliceInitialValue(s, parentDecl, bv);
        index += w;
        return;
    }
}

bool Flattener::_decomposeConcat(Value *v, std::list<Value *> &members)
{
    Expression *e = dynamic_cast<Expression *>(v);
    if (e != nullptr) {
        if (e->getOperator() != op_concat)
            return false;
        if (!_decomposeConcat(e->getValue1(), members))
            return false;
        if (!_decomposeConcat(e->getValue2(), members))
            return false;
        return true;
    }
    if ((dynamic_cast<hif::features::ISymbol *>(v) == nullptr) && (dynamic_cast<Member *>(v) == nullptr) &&
        (dynamic_cast<Slice *>(v)))
        return false;
    members.push_back(v);
    return true;
}

void Flattener::_propagateMemberInitialValue(Member *m, DataDeclaration *destination, Value *source)
{
    IntValue *index = dynamic_cast<IntValue *>(m->getIndex());
    if (index == nullptr) {
        hif::application_utils::WarningInfo info(m);
        _initialValueWarnings4.insert(info);
        return;
    }
    if (destination->getValue() == nullptr) {
        destination->setValue(_sem->getTypeDefaultValue(destination->getType(), destination));
    }
    BitvectorValue *bv = dynamic_cast<BitvectorValue *>(destination->getValue());
    BitValue *bi       = dynamic_cast<BitValue *>(source);
    if ((bi != nullptr) && (bv != nullptr)) {
        std::string str(bv->getValue());
        std::uint64_t j = str.length() - static_cast<std::uint64_t>(index->getValue()) - 1ULL;
        str[static_cast<std::string::size_type>(j)] = hif::bitConstantToString(bi->getValue())[0];
        bv->setValue(str);
        return;
    }

    hif::application_utils::WarningInfo info(m);
    _initialValueWarnings4.insert(info);
}

void Flattener::_propagateSliceInitialValue(Slice *s, DataDeclaration *destination, Value *source)
{
    IntValue *upper = dynamic_cast<IntValue *>(hif::rangeGetMaxBound(s->getSpan()));
    IntValue *lower = dynamic_cast<IntValue *>(hif::rangeGetMinBound(s->getSpan()));
    if ((upper == nullptr) || (lower == nullptr)) {
        hif::application_utils::WarningInfo info(s);
        _initialValueWarnings5.insert(info);
        return;
    }
    std::int64_t hi = upper->getValue();
    std::int64_t lo = lower->getValue();
    if (destination->getValue() == nullptr) {
        destination->setValue(_sem->getTypeDefaultValue(destination->getType(), destination));
    }
    BitvectorValue *bvd = dynamic_cast<BitvectorValue *>(destination->getValue());
    BitvectorValue *bvs = dynamic_cast<BitvectorValue *>(source);
    if ((bvs != nullptr) && (bvd != nullptr)) {
        std::string strd(bvd->getValue());
        std::string strs(bvs->getValue());
        hi   = static_cast<std::int64_t>(strd.length()) - hi - 1LL;
        lo   = static_cast<std::int64_t>(strd.length()) - lo - 1LL;
        strd = strd.substr(0, static_cast<std::string::size_type>(hi)) + strs +
               strd.substr(static_cast<std::string::size_type>(lo));
        bvd->setValue(strd);
        return;
    }

    hif::application_utils::WarningInfo info(s);
    _initialValueWarnings5.insert(info);
}

void Flattener::_renameReferences(const std::string &newName, ObjectsSet &refs)
{
    for (std::set<Object *>::iterator iter = refs.begin(); iter != refs.end(); ++iter) {
        Object *obj = *iter;
        hif::objectSetName(obj, newName);
    }
}

void Flattener::_propagateLibraries(Scope *target, Scope *source)
{
    BList<Library> *list = hif::objectGetLibraryList(source);
    for (BList<Library>::iterator iter = list->begin(); iter != list->end();) {
        Library *lib = *iter;
        iter         = iter.remove();
        hif::manipulation::AddUniqueObjectOptions addOpt;
        addOpt.equalsOptions.checkOnlyNames = true;
        addOpt.deleteIfNotAdded             = true;
        hif::manipulation::addUniqueObject(lib, target, addOpt);
    }
}

void Flattener::_expandDeclarationsList(
    BList<Declaration> &target,
    BList<Declaration> &source,
    const std::string &prefix)
{
    for (BList<Declaration>::iterator iter = source.begin(); iter != source.end();) {
        Declaration *decl = *iter;
        iter              = iter.remove();
        hif::manipulation::AddUniqueObjectOptions addOpt;
        addOpt.equalsOptions.checkOnlyNames = true;
        if (!hif::manipulation::addUniqueObject(decl, target, addOpt)) {
            std::string newName = _nameTable->getFreshName((prefix + "_" + decl->getName()).c_str());
            decl->setName(newName);
            target.push_back(decl);
        }
    }
}

void Flattener::_expandContents(Contents *target, Contents *source, const std::string &prefix)
{
    _propagateLibraries(target, source);

    _expandDeclarationsList(target->declarations, source->declarations, prefix);

    // TODO: check generates list
    target->generates.merge(source->generates);
    target->stateTables.merge(source->stateTables);
    if (source->getGlobalAction() != nullptr) {
        if (target->getGlobalAction() == nullptr) {
            target->setGlobalAction(source->setGlobalAction(nullptr));
        } else {
            target->getGlobalAction()->actions.merge(source->getGlobalAction()->actions);
        }
    }

#ifdef FLATTENER_PRINT_DEBUG
    for (BList<Instance>::iterator i = source->instances.begin(); i != source->instances.end(); ++i) {
        std::cout << "\tAdding instance " << (*i)->getName() << " to target" << std::endl;
    }
#endif
    for (BList<Instance>::iterator i = source->instances.begin(); i != source->instances.end(); ++i) {
        Instance *inst = *i;
        std::string n  = _nameTable->getFreshName((prefix + "_" + inst->getName()).c_str());
        inst->setName(n);
    }
    target->instances.merge(source->instances);
}

void Flattener::_cleanUpTree()
{
    ViewSet toBeRemoved;
    ViewDependenciesMap subModulesMap;
    ViewDependenciesMap parentModulesMap;
    hif::manipulation::findViewDependencies(_system, subModulesMap, parentModulesMap, _sem);
    for (ViewDependenciesMap::iterator iter = parentModulesMap.begin(); iter != parentModulesMap.end(); ++iter) {
        if (_isTopLevelDescendant(iter->first, parentModulesMap))
            continue;
        toBeRemoved.insert(iter->first);
    }
    for (std::set<View *>::iterator iter = toBeRemoved.begin(); iter != toBeRemoved.end(); ++iter) {
        // FIXME: at the moment we assume a single view for each design unit
        DesignUnit *du = dynamic_cast<DesignUnit *>((*iter)->getParent());
        messageAssert(du != nullptr, "Unexpected non-DesignUnit parent", (*iter)->getParent(), _sem);
#ifdef FLATTENER_PRINT_DEBUG
        std::cout << "Removing design unit " << du->getName() << std::endl;
#endif
        BList<Object> *parentList = du->getBList();
        parentList->erase(du);
    }
}

bool Flattener::_isTopLevelDescendant(View *v, ViewDependenciesMap &parentModulesMap)
{
    if (_topViews.empty())
        return true;
    if (_topViews.find(v) != _topViews.end())
        return true;

    for (ViewSet::iterator i = _topViews.begin(); i != _topViews.end(); ++i) {
        View *top = *i;
        if (parentModulesMap[v].find(top) != parentModulesMap[v].end())
            return true;
    }

    for (ViewSet::iterator iter = parentModulesMap[v].begin(); iter != parentModulesMap[v].end(); ++iter) {
        if (_isTopLevelDescendant(*iter, parentModulesMap))
            return true;
    }

    return false;
}

void Flattener::_extractSensitivityItemsFromBoundValue(std::set<Value *, ObjectCompare> &items, Value *bound)
{
    CopyOptions opt;
    opt.copyDeclarations   = false;
    opt.copySemanticsTypes = false;
    HifTypedQuery<Identifier> query;
    std::list<Identifier *> results;
    hif::search(results, bound, query);
    for (std::list<Identifier *>::iterator iter = results.begin(); iter != results.end(); ++iter) {
        Identifier *id     = *iter;
        // Ensure identifier has declaration
        DataDeclaration *d = hif::semantics::getDeclaration(id, _sem);
        if (d == nullptr)
            continue;
        // Ensure declaration is a signal or a port
        if ((dynamic_cast<Signal *>(d) == nullptr) && (dynamic_cast<Port *>(d) == nullptr))
            continue;
        // If the identifier belongs to a member or a slice, trace back its last prefixed reference parent
        PrefixedReference *pr = dynamic_cast<PrefixedReference *>(id->getParent());
        while (pr != nullptr) {
            PrefixedReference *tmp = dynamic_cast<PrefixedReference *>(pr->getParent());
            if (tmp == nullptr)
                break;
            pr = tmp;
        }
        if (pr == nullptr) {
            items.insert(hif::copy(id, opt));
        } else {
            items.insert(hif::copy(pr, opt));
        }
    }
}

bool Flattener::ObjectCompare::operator()(const Object *o1, const Object *o2) const
{
    return (hif::compare(const_cast<Object *>(o1), const_cast<Object *>(o2)) < 0);
}

} // anonymous namespace

// ///////////////////////////////////////////////////////////////////
// flattenDesign
// ///////////////////////////////////////////////////////////////////

void flattenDesign(hif::System *sys, semantics::ILanguageSemantics *sem, const FlattenDesignOptions &opt)
{
    hif::application_utils::initializeLogHeader("HIF", "flattenDesign");

#ifdef DEBUG_STEPS
    hif::writeFile("FLATTEN_00_before_flattening", sys, true);
    hif::writeFile("FLATTEN_00_before_flattening", sys, false);
#endif

    // Expand generate statements (if any)
    if (hif::manipulation::expandGenerates(sys, sem)) {
        if (opt.verbose)
            messageWarning("Generate statements have been expanded.", nullptr, nullptr);
    }

#ifdef DEBUG_STEPS
    hif::writeFile("FLATTEN_01_after_expand_generates", sys, true);
    hif::writeFile("FLATTEN_01_after_expand_generates", sys, false);
#endif

    // Reset types and update declarations before flattening
    hif::manipulation::flushInstanceCache();
    hif::semantics::resetTypes(sys);
    hif::semantics::UpdateDeclarationOptions dopt;
    dopt.forceRefresh = true;
    hif::semantics::updateDeclarations(sys, sem, dopt);

    // Flatten the description
    if (opt.verbose)
        messageInfo("Flattening description");
    Flattener flattener(sys, sem, opt.topLevelName, opt.rootDUs, opt.rootInstances);
    flattener.flattenDesign();
    flattener.printWarnings();

#ifdef DEBUG_STEPS
    hif::writeFile("FLATTEN_02_after_flattening", sys, true);
    hif::writeFile("FLATTEN_02_after_flattening", sys, false);
#endif

    // Ensure correct declarations after flattening
    dopt.error = true;
    hif::semantics::updateDeclarations(sys, sem, dopt);

    // Print collected warnings
    if (opt.verbose)
        printUniqueWarnings("During flattening, one or more warnings have been raised:");

    // Simplify the flattened description
    if (opt.verbose)
        messageInfo("Simplifying flattened description");
    hif::manipulation::simplify(sys, sem);

#ifdef DEBUG_STEPS
    hif::writeFile("FLATTEN_03_after_simplify", sys, true);
    hif::writeFile("FLATTEN_03_after_simplify", sys, false);
#endif

    hif::application_utils::restoreLogHeader();
}

} // namespace manipulation
} // namespace hif
