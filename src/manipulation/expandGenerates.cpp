/// @file expandGenerates.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/expandGenerates.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif.hpp"
#include "hif/hifIOUtils.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

typedef std::set<View *> ViewsSet;
typedef std::map<hif::View *, ViewsSet> ViewMap;
typedef std::map<hif::View *, std::string> NameMap;

// Wrapper for simplify call.
void _expandGenerates(View *o, hif::semantics::ILanguageSemantics *refLang)
{
    hif::manipulation::SimplifyOptions opt;
    opt.simplify_generates           = true;
    opt.simplify_template_parameters = true;
    // Too agressive!
    // Simplify of constants must be performed only inside simplify(),
    // when needeed by simplify_generates.
    // Ref design: vhdl/gaisler/can_oc
    opt.simplify_constants           = false;
    hif::manipulation::simplify(o, refLang, opt);

    // Ref design: vhdl/openCores/corproc
    hif::manipulation::SimplifyOptions opt2;
    opt2.simplify_constants = true;
    opt2.context            = o->getContents();
    hif::manipulation::simplify(o, refLang, opt2);
}
bool _assureTemplateDefaultValues(View *v, hif::semantics::ILanguageSemantics *refLang)
{
    bool warnDefault = false;
    for (BList<Declaration>::iterator i = v->templateParameters.begin(); i != v->templateParameters.end(); ++i) {
        if (dynamic_cast<ValueTP *>(*i) != nullptr) {
            ValueTP *vtp = static_cast<ValueTP *>(*i);
            if (vtp->getValue() != nullptr)
                continue;

            vtp->setValue(refLang->getTypeDefaultValue(vtp->getType(), nullptr));
            messageDebug(
                std::string("Set default value of template parameter ") + v->getName() + "->" + vtp->getName(),
                vtp->getValue(), refLang);
            warnDefault = true;
        } else //if( dynamic_cast< TypeTP * >( *i ) != nullptr )
        {
            TypeTP *vtp = static_cast<TypeTP *>(*i);
            if (vtp->getType() != nullptr)
                continue;

            messageError("Cannot add a default template parameter type.", vtp, refLang);
        }
    }

    return warnDefault;
}

bool _instantiateSubModules(
    View *prev,
    View *key,
    ViewMap &children,
    hif::semantics::ILanguageSemantics *refLang,
    NameMap &nameMap)
{
    _expandGenerates(prev, refLang);

    if (children.empty())
        return false;

    Contents *c = prev->getContents();
    if (c == nullptr)
        return false;

    bool haveInstantiated = false;
    for (ViewMap::mapped_type::iterator current_it = children[key].begin(); current_it != children[key].end();
         ++current_it) {
        View *current  = *current_it;
        DesignUnit *du = dynamic_cast<DesignUnit *>(current->getParent());
        messageAssert(du != nullptr, "Unexpected parent", current->getParent(), refLang);
        std::string duName = du->getName();

        for (BList<Instance>::iterator i = c->instances.begin(); i != c->instances.end(); ++i) {
            Instance *inst    = *i;
            ViewReference *vr = dynamic_cast<ViewReference *>(inst->getReferencedType());
            if (vr == nullptr) {
                messageError("Unexpected referenced type", (*i)->getReferencedType(), refLang);
            }
            if (vr->getName() != current->getName() || vr->getDesignUnit() != duName) {
                continue;
            }

            View *vrDecl = hif::semantics::getDeclaration(vr, refLang);
            messageAssert(vrDecl != nullptr, "Declaration not found", vr, refLang);
            if (vrDecl->templateParameters.empty()) {
                _instantiateSubModules(vrDecl, current, children, refLang, nameMap);
                continue;
            }

            View *instantiated = hif::manipulation::instantiate(vr, refLang);
            messageAssert(instantiated != nullptr, "Cannot instantiate viewref", vr, refLang);
            //_simplifyIntantiatedView(instantiated, vr)

            View *instView = nullptr;
            std::string newName;
            NameMap::iterator nit = nameMap.find(instantiated);
            if (nit != nameMap.end()) {
                // already expanded
                vr->templateParameterAssigns.clear();
                vr->setDesignUnit(nit->second);
                continue;
            }

            // TODO: add refine to avoid renamig in case of unique instantiation.
            newName               = hif::NameTable::getInstance()->getFreshName(duName);
            nameMap[instantiated] = newName;

            instView = hif::copy(instantiated);
            hif::semantics::mapDeclarationsInTree(instView, instantiated, instView, refLang);
            DesignUnit *newDu = new DesignUnit();
            newDu->setName(newName);
            newDu->views.push_back(instView);
            du->getBList()->push_back(newDu);
            vr->setDesignUnit(newName);
            haveInstantiated = true;

            _instantiateSubModules(instView, current, children, refLang, nameMap);

            vr->templateParameterAssigns.clear();
            instView->templateParameters.clear();
        }
    }

    prev->templateParameters.clear();

    hif::HifTypedQuery<ViewReference> query;
    query.name = prev->getName();
    hif::HifTypedQuery<ViewReference>::Results results;
    hif::search(results, prev, query);
    DesignUnit *du = dynamic_cast<DesignUnit *>(prev->getParent());
    for (hif::HifTypedQuery<ViewReference>::Results::iterator i = results.begin(); i != results.end(); ++i) {
        ViewReference *vr                      = *i;
        ViewReference::DeclarationType *vrDecl = hif::semantics::getDeclaration(vr, refLang);
        messageAssert(vr != nullptr, "Declaration not found", vr, refLang);

        if (vrDecl == prev) {
            vr->templateParameterAssigns.clear();
            vr->setDesignUnit(du->getName());
        }
    }

    return haveInstantiated;
}

void _getGeneratePaths(
    View *v,
    View *prev,
    ViewMap &pmm,
    ViewsSet &topModules,
    ViewsSet &childModules,
    ViewMap &dependencies)
{
    // Adding the new dependency
    if (prev != nullptr) {
        dependencies[v].insert(prev);
    }

    // If no templates are found, the path is completed.
    if (v->templateParameters.empty()) {
        topModules.insert(v);
        return;
    }

    // If this is a top module, the path is completed.
    if (pmm[v].empty()) {
        topModules.insert(v);
        return;
    }

    childModules.insert(v);

    for (ViewMap::mapped_type::iterator i = pmm[v].begin(); i != pmm[v].end(); ++i) {
        _getGeneratePaths(*i, v, pmm, topModules, childModules, dependencies);
    }
}

// ///////////////////////////////////////////////////////////////////
// GuardVisitor
// ///////////////////////////////////////////////////////////////////
// Visitor that identifies guard condition for further fixes.
class GuardVisitor : public GuideVisitor
{
public:
    GuardVisitor();
    virtual ~GuardVisitor();

    int visitForGenerate(ForGenerate &o);
    int visitIfGenerate(IfGenerate &o);
    int visitView(View &o);

    bool hasGenerate();
    bool hasTemplateModule();

private:
    GuardVisitor(const GuardVisitor &o);
    GuardVisitor &operator=(const GuardVisitor &o);

    bool _hasGenerate;
    bool _hasTemplateModule;
};

GuardVisitor::GuardVisitor()
    : _hasGenerate(false)
    , _hasTemplateModule(false)
{
    // ntd
}

GuardVisitor::~GuardVisitor()
{
    // ntd
}

bool GuardVisitor::hasGenerate() { return _hasGenerate; }

bool GuardVisitor::hasTemplateModule() { return _hasTemplateModule; }

int GuardVisitor::visitForGenerate(ForGenerate &o)
{
    GuideVisitor::visitForGenerate(o);
    _hasGenerate = true;
    return 0;
}

int GuardVisitor::visitIfGenerate(IfGenerate &o)
{
    GuideVisitor::visitIfGenerate(o);
    _hasGenerate = true;
    return 0;
}

int GuardVisitor::visitView(View &o)
{
    GuideVisitor::visitView(o);
    if (!o.templateParameters.empty())
        _hasTemplateModule = true;
    return 0;
}

// ///////////////////////////////////////////////////////////////////
// Generate Tree Visitor
// ///////////////////////////////////////////////////////////////////
class GenerateTreeVisitor : public GuideVisitor
{
public:
    GenerateTreeVisitor();
    virtual ~GenerateTreeVisitor();

    int visitView(View &o);

    std::set<View *> foundedViews;

private:
    GenerateTreeVisitor(const GenerateTreeVisitor &o);
    GenerateTreeVisitor &operator=(const GenerateTreeVisitor &o);
};

GenerateTreeVisitor::GenerateTreeVisitor()
    : foundedViews()
{
}

GenerateTreeVisitor::~GenerateTreeVisitor() {}

int GenerateTreeVisitor::visitView(View &o)
{
    GuideVisitor::visitView(o);

    if (o.getContents() == nullptr)
        return 0;
    if (o.getContents()->generates.empty())
        return 0;

    foundedViews.insert(&o);

    return 0;
}

} // namespace

bool expandGenerates(Object *root, hif::semantics::ILanguageSemantics *refLang)
{
    GuardVisitor gv;
    root->acceptVisitor(gv);
    if (!gv.hasGenerate())
        return false;

    hif::application_utils::initializeLogHeader("HIF", "expandGenerates");

    ViewMap smm;
    ViewMap pmm;

    // Calculating dependencies:
    findViewDependencies(root, smm, pmm, refLang);

    // Getting list of views with generates:
    GenerateTreeVisitor gtv;
    root->acceptVisitor(gtv);

    // Calculating dependencies and top modules:
    ViewMap dependencies;
    ViewsSet topModules;
    ViewsSet childModules;
    for (ViewsSet::iterator i = gtv.foundedViews.begin(); i != gtv.foundedViews.end(); ++i) {
        _getGeneratePaths(*i, nullptr, pmm, topModules, childModules, dependencies);
    }

    bool warnDefault = false;
    NameMap nameMap;
    for (ViewsSet::iterator i = topModules.begin(); i != topModules.end(); ++i) {
        warnDefault |= _assureTemplateDefaultValues(*i, refLang);
        _instantiateSubModules(*i, *i, dependencies, refLang, nameMap);
    }

    BList<Declaration> tmp;
    for (ViewsSet::iterator i = childModules.begin(); i != childModules.end(); ++i) {
        // Removing view:
        DesignUnit *du = dynamic_cast<DesignUnit *>((*i)->getParent());
        (*i)->replace(nullptr);
        if (du != nullptr) {
            if (du->views.empty()) {
                du->replace(nullptr);
                //delete du;
                tmp.push_back(du);
            }
        }
        //delete *i;
        tmp.push_back(*i);
    }

    hif::semantics::ResetDeclarationsOptions ropt;
    ropt.onlyCurrent = true;

    for (ViewsSet::iterator i = childModules.begin(); i != childModules.end(); ++i) {
        View *view = *i;
        // Removing pending references:
        hif::semantics::ReferencesSet refs;
        hif::semantics::getReferences(view, refs, refLang, root);
        for (hif::semantics::ReferencesSet::iterator j = refs.begin(); j != refs.end(); ++j) {
            ViewReference *vr = dynamic_cast<ViewReference *>(*j);
            messageAssert(vr != nullptr, "Unexpected view reference", *j, refLang);

            hif::semantics::resetDeclarations(vr, ropt);
            hif::semantics::resetDeclarations(vr->templateParameterAssigns, ropt);
            // Since we are going to delete the design unit
            // we have to reset also the eventual pointers to
            // design unit entity.
            if (dynamic_cast<Instance *>((*j)->getParent()) != nullptr) {
                Instance *ii = static_cast<Instance *>((*j)->getParent());
                hif::semantics::resetDeclarations(ii, ropt);
                hif::semantics::resetDeclarations(ii->portAssigns, ropt);
                hif::semantics::resetTypes(ii, false);
                hif::semantics::resetTypes(ii->portAssigns, false);
                // Ensure portAssign typability
                for (BList<PortAssign>::iterator it = ii->portAssigns.begin(); it != ii->portAssigns.end(); ++it) {
                    PortAssign *pa = *it;
                    Type *portType = hif::semantics::getSemanticType(pa, refLang);
                    Type *valType  = hif::semantics::getSemanticType(pa->getValue(), refLang);
                    if (refLang->getExprType(portType, valType, hif::op_bind, pa).returnedType == nullptr) {
                        Cast *c = new Cast();
                        c->setType(hif::copy(portType));
                        c->setValue(pa->getValue());
                        pa->setValue(c);
                    }
                }
            }
        }
    }
#if 0
    for( ViewsSet::iterator i = topModules.begin();
         i != topModules.end(); ++i )
    {
        View * view = *i;
        if (view->getContents() == nullptr) continue;
        for (BList<Instance>::iterator j = view->getContents()->instances.begin();
             j != view->getContents()->instances.end(); ++j)
        {
            Instance * ii = *j;
            //ViewReference * vr = dynamic_cast<ViewReference *>(ii->getReferencedType());
            hif::semantics::resetDeclarations(ii, ropt);
            hif::semantics::resetDeclarations(ii->portAssigns, ropt);
            hif::semantics::resetTypes(ii, false);
            hif::semantics::resetTypes(ii->portAssigns, false);
            // Ensure portAssign typability
            for (BList<PortAssign>::iterator it = ii->portAssigns.begin();
                 it != ii->portAssigns.end(); ++it)
            {
                PortAssign * pa = *it;
                Type * portType = hif::semantics::getSemanticType(pa, refLang);
                Type * valType = hif::semantics::getSemanticType(pa->getValue(), refLang);
                if (refLang->getExprType(portType, valType, hif::op_bind, pa).returnedType == nullptr)
                {
                    Cast * c = new Cast();
                    c->setType(hif::copy(portType));
                    c->setValue(pa->getValue());
                    pa->setValue(c);
                }
            }
        }
    }
#endif

    if (warnDefault) {
        messageWarning(
            "Added default value for top module template "
            "parameter, needed to expand generate statements.",
            nullptr, nullptr);
    }

    hif::application_utils::restoreLogHeader();
    return true;
}

} // namespace manipulation
} // namespace hif
