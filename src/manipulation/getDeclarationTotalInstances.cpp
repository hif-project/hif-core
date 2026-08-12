/// @file getDeclarationTotalInstances.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/getDeclarationTotalInstances.hpp"
#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/isAbstractionSubProgram.hpp"
#include "hif/semantics/declarationUtils.hpp"

namespace hif
{
namespace manipulation
{

namespace /* anon */
{
class CounterVisitor : public GuideVisitor
{
public:
    CounterVisitor(hif::semantics::ILanguageSemantics *sem);
    ~CounterVisitor();

    int visitLibraryDef(hif::LibraryDef &o);
    int visitProcedure(hif::Procedure &o);
    int visitView(hif::View &o);
    int visitViewReference(hif::ViewReference &o);
    TotalInstanceMap getResult();

private:
    hif::semantics::ILanguageSemantics *_sem;
    TotalInstanceMap _map;

    CounterVisitor(const CounterVisitor &)            = delete;
    CounterVisitor &operator=(const CounterVisitor &) = delete;
};

CounterVisitor::CounterVisitor(semantics::ILanguageSemantics *sem)
    : GuideVisitor()
    , _sem(sem)
    , _map()
{
    // ntd
}

CounterVisitor::~CounterVisitor()
{
    // ntd
}

int CounterVisitor::visitLibraryDef(LibraryDef &o)
{
    if (o.isStandard())
        return 0;
    return hif::GuideVisitor::visitLibraryDef(o);
}

int CounterVisitor::visitProcedure(Procedure &o)
{
    if (hif::manipulation::isAbstractionSubProgram(o.getName()))
        return 0;
    return hif::GuideVisitor::visitProcedure(o);
}

int CounterVisitor::visitView(View &o)
{
    if (o.isStandard())
        return 0;
    return hif::GuideVisitor::visitView(o);
}

int CounterVisitor::visitViewReference(ViewReference &o)
{
    Scope *decl = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(decl != nullptr, "Declaration not found", &o, _sem);

    // If the nearest view of the viewref is its declaration
    // we have to skip it to avoid loop
    // an exemple of this is the constructor when its type
    // is a viewreference
    View *nearestView = hif::getNearestParent<View>(&o);
    if (decl == nearestView) {
        return 0;
    }

    TotalInstanceMap::iterator it = _map.find(decl);
    if (it == _map.end()) {
        _map[decl] = 0;
    } else {
        _map[decl] += 1;
    }

    decl->acceptVisitor(*this);
    return GuideVisitor::visitViewReference(o);
}

TotalInstanceMap CounterVisitor::getResult() { return _map; }

} // namespace

TotalInstanceMap getDeclarationTotalInstances(Scope *scope, semantics::ILanguageSemantics *sem)
{
    CounterVisitor cv(sem);
    scope->acceptVisitor(cv);
    TotalInstanceMap map          = cv.getResult();
    TotalInstanceMap::iterator it = map.find(scope);
    if (it == map.end()) {
        map[scope] = 0;
    } else {
        map[scope] += 1;
    }

    return map;
}

} // namespace manipulation
} // namespace hif
