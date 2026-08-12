/// @file renameForbiddenNames.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/renameForbiddenNames.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/application_utils.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

/****************************************************************************/
/*  Forbidden names visitor.                                                */
/****************************************************************************/

/// @brief Checks the name of all declarations wrt the reference semantics.
class ForbiddenNamesVisitor : public hif::GuideVisitor
{
public:
    ForbiddenNamesVisitor(hif::Object *root, hif::semantics::ILanguageSemantics *sem);
    virtual ~ForbiddenNamesVisitor();

    virtual int visitAlias(hif::Alias &o);
    virtual int visitConst(hif::Const &o);
    virtual int visitContents(hif::Contents &o);
    virtual int visitDesignUnit(hif::DesignUnit &o);
    virtual int visitEntity(hif::Entity &o);
    virtual int visitEnumValue(hif::EnumValue &o);
    virtual int visitField(hif::Field &o);
    virtual int visitForGenerate(hif::ForGenerate &o);
    virtual int visitFunction(hif::Function &o);
    virtual int visitIfGenerate(hif::IfGenerate &o);
    virtual int visitLibraryDef(hif::LibraryDef &o);
    virtual int visitParameter(hif::Parameter &o);
    virtual int visitPort(hif::Port &o);
    virtual int visitProcedure(hif::Procedure &o);
    virtual int visitSignal(hif::Signal &o);
    virtual int visitState(hif::State &o);
    virtual int visitStateTable(hif::StateTable &o);
    virtual int visitSystem(hif::System &o);
    virtual int visitTypeDef(hif::TypeDef &o);
    virtual int visitTypeTP(hif::TypeTP &o);
    virtual int visitValueTP(hif::ValueTP &o);
    virtual int visitVariable(hif::Variable &o);
    virtual int visitView(hif::View &o);

private:
    bool _fixForbiddenDeclaration(hif::Declaration *decl);
    bool _fixForbiddenViewReferences(hif::DesignUnit *du, const std::string &fresh);

    // Disabled.
    ForbiddenNamesVisitor(const ForbiddenNamesVisitor &);
    ForbiddenNamesVisitor &operator=(const ForbiddenNamesVisitor &);

    /// @brief The reference semantics.
    hif::semantics::ILanguageSemantics *_sem;

    // @brief Map of all references.
    typedef std::set<Object *> ReferencesSet;
    typedef std::map<hif::Declaration *, ReferencesSet> ReferencesMap;
    ReferencesMap _references;
};
ForbiddenNamesVisitor::ForbiddenNamesVisitor(hif::Object *root, hif::semantics::ILanguageSemantics *sem)
    : GuideVisitor()
    , _sem(sem)
    , _references()
{
    hif::semantics::getAllReferences(_references, _sem, root);
}

ForbiddenNamesVisitor::~ForbiddenNamesVisitor()
{
    // ntd
}

int ForbiddenNamesVisitor::visitAlias(hif::Alias &o)
{
    GuideVisitor::visitAlias(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitConst(hif::Const &o)
{
    GuideVisitor::visitConst(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitContents(hif::Contents &o)
{
    GuideVisitor::visitContents(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitDesignUnit(hif::DesignUnit &o)
{
    GuideVisitor::visitDesignUnit(o);
    std::string previousName = o.getName();
    if (_fixForbiddenDeclaration(&o)) {
        messageInfo(
            std::string("Component ") + previousName + " has been renamed to " + o.getName() +
            " to avoid wrong behaviors due to reserved keywords.");
    }
    return 0;
}

int ForbiddenNamesVisitor::visitEntity(hif::Entity &o)
{
    GuideVisitor::visitEntity(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitEnumValue(hif::EnumValue &o)
{
    GuideVisitor::visitEnumValue(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitField(hif::Field &o)
{
    GuideVisitor::visitField(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitForGenerate(hif::ForGenerate &o)
{
    GuideVisitor::visitForGenerate(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitFunction(hif::Function &o)
{
    GuideVisitor::visitFunction(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitIfGenerate(hif::IfGenerate &o)
{
    GuideVisitor::visitIfGenerate(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitLibraryDef(hif::LibraryDef &o)
{
    GuideVisitor::visitLibraryDef(o);
    std::string previousName = o.getName();
    if (_fixForbiddenDeclaration(&o)) {
        messageInfo(
            std::string("Package ") + previousName + " has been renamed to " + o.getName() +
            " to avoid wrong behaviors due to reserved keywords.");
    }
    return 0;
}

int ForbiddenNamesVisitor::visitParameter(hif::Parameter &o)
{
    GuideVisitor::visitParameter(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitPort(hif::Port &o)
{
    GuideVisitor::visitPort(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitProcedure(hif::Procedure &o)
{
    GuideVisitor::visitProcedure(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitSignal(hif::Signal &o)
{
    GuideVisitor::visitSignal(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitState(hif::State &o)
{
    GuideVisitor::visitState(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitStateTable(hif::StateTable &o)
{
    GuideVisitor::visitStateTable(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitSystem(hif::System &o)
{
    GuideVisitor::visitSystem(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitTypeDef(hif::TypeDef &o)
{
    GuideVisitor::visitTypeDef(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitTypeTP(hif::TypeTP &o)
{
    GuideVisitor::visitTypeTP(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitValueTP(hif::ValueTP &o)
{
    GuideVisitor::visitValueTP(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitVariable(hif::Variable &o)
{
    GuideVisitor::visitVariable(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

int ForbiddenNamesVisitor::visitView(hif::View &o)
{
    GuideVisitor::visitView(o);
    _fixForbiddenDeclaration(&o);
    return 0;
}

bool ForbiddenNamesVisitor::_fixForbiddenDeclaration(hif::Declaration *decl)
{
    if (!_sem->isForbiddenName(decl))
        return false;

    std::string fresh = hif::NameTable::getInstance()->getFreshName((std::string(decl->getName()) + "_hif").c_str());
    decl->setName(fresh);

    bool ret = false;

    DesignUnit *du = dynamic_cast<DesignUnit *>(decl);
    if (du != nullptr)
        ret |= _fixForbiddenViewReferences(du, fresh);

    ReferencesMap::iterator it(_references.find(decl));
    if (it == _references.end())
        return ret;

    ReferencesSet &set = it->second;
    for (ReferencesSet::iterator sIt(set.begin()); sIt != set.end(); ++sIt) {
        hif::objectSetName(*sIt, fresh);
        ret |= true;
    }

    return ret;
}

bool ForbiddenNamesVisitor::_fixForbiddenViewReferences(hif::DesignUnit *du, const std::string &fresh)
{
    messageAssert(!du->views.empty() && du->views.size() == 1, "Unexpected views size", du, _sem);
    View *v = du->views.front();

    ReferencesMap::iterator jt(_references.find(v));
    if (jt == _references.end())
        return false;

    bool ret = false;

    ReferencesSet &vRefSet = jt->second;
    for (ReferencesSet::iterator sJt(vRefSet.begin()); sJt != vRefSet.end(); ++sJt) {
        ViewReference *vRef = dynamic_cast<ViewReference *>(*sJt);
        messageAssert(vRef != nullptr, "Expected ViewReference", *sJt, _sem);

        vRef->setDesignUnit(fresh);
        ret |= true;
    }

    return ret;
}
} // namespace

void renameForbiddenNames(Object *root, hif::semantics::ILanguageSemantics *sem)
{
    hif::application_utils::initializeLogHeader("HIF", "renameForbiddenNames");

    if (root == nullptr || sem == nullptr) {
        messageAssert(root != nullptr, "Passed nullptr arguments", nullptr, nullptr);
        return;
    }

    ForbiddenNamesVisitor fnv(root, sem);
    root->acceptVisitor(fnv);

    hif::application_utils::restoreLogHeader();
}

} // namespace manipulation
} // namespace hif
