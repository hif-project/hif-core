/// @file findScopeDependencies.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/findScopeDependencies.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

// /////////////////////////////////////////////////////////////////////////////
// ScopeDependencyVisitor
// /////////////////////////////////////////////////////////////////////////////
class ScopeDependencyVisitor : public hif::GuideVisitor
{
public:
    // Traits.
    typedef std::list<hif::Scope *> ScopedParents;
    typedef std::map<hif::Scope *, ScopedParents> ScopeRelations;

    // Ctor, dtor.
    ScopeDependencyVisitor(ScopeRelations &scopeRelations);
    virtual ~ScopeDependencyVisitor();

    // Visits.
    int visitDesignUnit(DesignUnit &o);
    int visitLibraryDef(LibraryDef &o);

private:
    // Disabled.
    ScopeDependencyVisitor(const ScopeDependencyVisitor &);
    ScopeDependencyVisitor &operator=(const ScopeDependencyVisitor &);

    Scope *_getNearestScope(Scope *start);
    void _calculateScope(Scope *start);

    ScopeRelations &_scopeRelations;
};
ScopeDependencyVisitor::ScopeDependencyVisitor(ScopeRelations &scopeRelations)
    : _scopeRelations(scopeRelations)
{
    // ntd
}

ScopeDependencyVisitor::~ScopeDependencyVisitor()
{
    // ntd
}

int ScopeDependencyVisitor::visitDesignUnit(DesignUnit &o)
{
    _calculateScope(&o);
    return GuideVisitor::visitDesignUnit(o);
}

int ScopeDependencyVisitor::visitLibraryDef(LibraryDef &o)
{
    if (o.isStandard())
        return 0;
    _calculateScope(&o);
    return GuideVisitor::visitLibraryDef(o);
}

Scope *ScopeDependencyVisitor::_getNearestScope(Scope *start)
{
    if (dynamic_cast<System *>(start) != nullptr)
        return nullptr;

    Scope *ret = nullptr;

    ret = hif::getNearestParent<DesignUnit>(start);
    if (ret != nullptr)
        return ret;

    ret = hif::getNearestParent<LibraryDef>(start);
    if (ret != nullptr)
        return ret;

    ret = hif::getNearestParent<System>(start);
    return ret;
}

void ScopeDependencyVisitor::_calculateScope(Scope *start)
{
    Scope *next = _getNearestScope(start);
    while (next != nullptr) {
        _scopeRelations[start].push_back(next);

        //        // If scope parent already in map, exploit its scope dependencies.
        //        // Just an optimization.
        //        if (_scopeRelations.find(next) != _scopeRelations.end())
        //        {
        //            for (ScopedParents::iterator it( _scopeRelations[next].begin() );
        //                it != _scopeRelations[next].end(); ++it)
        //            {
        //                _scopeRelations[start].push_back( *it );
        //            }
        //            return;
        //        }

        next = _getNearestScope(next);
    }
}

} // namespace

void findScopeDependencies(hif::System *root, std::map<hif::Scope *, std::list<hif::Scope *>> &scopeRelations)
{
    ScopeDependencyVisitor sdv(scopeRelations);
    root->acceptVisitor(sdv);
}

} // namespace manipulation
} // namespace hif
