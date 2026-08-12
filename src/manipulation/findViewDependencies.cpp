/// @file findViewDependencies.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/findViewDependencies.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/search.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace
{ // anonymous

// /////////////////////////////////////////////////////////////////////////////
// DUDependencyVisitor
// /////////////////////////////////////////////////////////////////////////////

/// \class DUDependencyVisitor
/// This class implements a design unit dependencies visitor. It is responsible for
/// scanning the design units in a system description and collecting information
/// about which modules are sub-modules of the other.
class DUDependencyVisitor : public hif::GuideVisitor
{
public:
    /// @brief Typedef for the design unit dependency map.
    typedef std::map<hif::View *, std::set<hif::View *>> DUDependencyMap;

    /// @brief Constructor.
    DUDependencyVisitor(
        DUDependencyMap &smm,
        DUDependencyMap &pmm,
        semantics::ILanguageSemantics *sem,
        const ViewDependencyOptions &opt);

    /// @brief Destructor.
    ~DUDependencyVisitor();

    int visitLibraryDef(hif::LibraryDef &o);
    int visitView(hif::View &o);

protected:
    DUDependencyMap &_subModulesMap;

    DUDependencyMap &_parentModulesMap;

    semantics::ILanguageSemantics *_sem;

    const ViewDependencyOptions &_opt;

    /// @brief Disabled copy constructor.
    DUDependencyVisitor(const DUDependencyVisitor &d);

    /// @brief Disabled assignment operator.
    DUDependencyVisitor &operator=(const DUDependencyVisitor &d);
};

DUDependencyVisitor::DUDependencyVisitor(
    DUDependencyMap &smm,
    DUDependencyMap &pmm,
    semantics::ILanguageSemantics *sem,
    const ViewDependencyOptions &opt)
    : GuideVisitor()
    , _subModulesMap(smm)
    , _parentModulesMap(pmm)
    , _sem(sem)
    , _opt(opt)
{
    // ntd
}

DUDependencyVisitor::~DUDependencyVisitor()
{
    // ntd
}

int DUDependencyVisitor::visitLibraryDef(hif::LibraryDef &o)
{
    if (_opt.skipStandardLibraries && o.isStandard())
        return 0;
    if (_opt.skipRtlDependencies && o.getLanguageID() == hif::rtl)
        return 0;
    if (_opt.skipTlmDependencies && o.getLanguageID() == hif::tlm)
        return 0;
    if (_opt.skipCppDependencies && o.getLanguageID() == hif::cpp)
        return 0;
    if (_opt.skipCDependencies && o.getLanguageID() == hif::c)
        return 0;
    if (_opt.skipPslDependencies && o.getLanguageID() == hif::psl)
        return 0;

    return GuideVisitor::visitLibraryDef(o);
}
int DUDependencyVisitor::visitView(hif::View &o)
{
    if (_opt.skipStandardViews && o.isStandard())
        return 0;
    if (_opt.skipRtlDependencies && o.getLanguageID() == hif::rtl)
        return 0;
    if (_opt.skipTlmDependencies && o.getLanguageID() == hif::tlm)
        return 0;
    if (_opt.skipCppDependencies && o.getLanguageID() == hif::cpp)
        return 0;
    if (_opt.skipCDependencies && o.getLanguageID() == hif::c)
        return 0;
    if (_opt.skipPslDependencies && o.getLanguageID() == hif::psl)
        return 0;

    if (_subModulesMap.find(&o) == _subModulesMap.end()) {
        _subModulesMap[&o];
    }
    if (_parentModulesMap.find(&o) == _parentModulesMap.end()) {
        _parentModulesMap[&o];
    }

    hif::HifTypedQuery<ViewReference> query;
    std::list<ViewReference *> resultList;
    hif::search(resultList, &o, query);
    for (std::list<ViewReference *>::iterator iter = resultList.begin(); iter != resultList.end(); ++iter) {
        View *subView = hif::semantics::getDeclaration(*iter, _sem);
        if (subView == nullptr || subView == &o) {
            continue;
        }
        _subModulesMap[&o].insert(subView);
        _parentModulesMap[subView].insert(&o);
    }
    return 0;
}

} // anonymous namespace

// ///////////////////////////////////////////////////////////////////
// DUDependencyOptions
// ///////////////////////////////////////////////////////////////////

ViewDependencyOptions::ViewDependencyOptions()
    : skipStandardViews(false)
    , skipStandardLibraries(true)
    , skipRtlDependencies(false)
    , skipTlmDependencies(false)
    , skipCppDependencies(false)
    , skipCDependencies(false)
    , skipPslDependencies(false)
{
    // ntd
}

ViewDependencyOptions::~ViewDependencyOptions()
{
    // ntd
}

ViewDependencyOptions::ViewDependencyOptions(const ViewDependencyOptions &other)
    : skipStandardViews(other.skipStandardViews)
    , skipStandardLibraries(other.skipStandardLibraries)
    , skipRtlDependencies(other.skipRtlDependencies)
    , skipTlmDependencies(other.skipTlmDependencies)
    , skipCppDependencies(other.skipCppDependencies)
    , skipCDependencies(other.skipCDependencies)
    , skipPslDependencies(other.skipPslDependencies)
{
    // ntd
}

ViewDependencyOptions &ViewDependencyOptions::operator=(ViewDependencyOptions other)
{
    swap(other);
    return *this;
}

void ViewDependencyOptions::swap(ViewDependencyOptions &other)
{
    std::swap(skipStandardViews, other.skipStandardViews);
    std::swap(skipStandardLibraries, other.skipStandardLibraries);
    std::swap(skipRtlDependencies, other.skipRtlDependencies);
    std::swap(skipTlmDependencies, other.skipTlmDependencies);
    std::swap(skipCppDependencies, other.skipCppDependencies);
    std::swap(skipCDependencies, other.skipCDependencies);
    std::swap(skipPslDependencies, other.skipPslDependencies);
}
// ///////////////////////////////////////////////////////////////////
// findViewDependencies
// ///////////////////////////////////////////////////////////////////
//#define DBG_VIEW_DEPENDENCIES

void findViewDependencies(
    hif::Object *obj,
    ViewDependenciesMap &smm,
    ViewDependenciesMap &pmm,
    semantics::ILanguageSemantics *sem,
    const ViewDependencyOptions &opt)
{
    DUDependencyVisitor ddv(smm, pmm, sem, opt);
    obj->acceptVisitor(ddv);

#ifdef DBG_VIEW_DEPENDENCIES
    std::clog << "###########################################################\n";
    std::clog << "Design unit dependencies\n";
    std::clog << "###########################################################\n";
    for (ViewDependenciesMap::iterator i = smm.begin(); i != smm.end(); ++i) {
        View *view     = i->first;
        DesignUnit *du = dynamic_cast<DesignUnit *>(view->getParent());
        assert(du != nullptr);
        std::clog << du->getName() << ": ";
        ViewDependenciesSet &deps = i->second;
        for (ViewDependenciesSet::iterator j = deps.begin(); j != deps.end(); ++j) {
            View *sub         = *j;
            DesignUnit *subDu = dynamic_cast<DesignUnit *>(sub->getParent());
            assert(subDu != nullptr);
            std::clog << subDu->getName() << " ";
        }
        std::clog << std::endl;
    }
    std::clog << "###########################################################\n";
#endif
}

} // namespace manipulation
} // namespace hif
