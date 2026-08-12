/// @file checkConflictingName.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/checkConflictingName.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{
class ConflictChecker : public HifVisitor
{
public:
    ConflictChecker(const std::string &n);
    ~ConflictChecker();

    virtual int visitContents(hif::Contents &o);
    virtual int visitDesignUnit(hif::DesignUnit &o);
    virtual int visitEntity(hif::Entity &o);
    virtual int visitForGenerate(hif::ForGenerate &o);
    virtual int visitFunction(hif::Function &o);
    virtual int visitIfGenerate(hif::IfGenerate &o);
    virtual int visitLibraryDef(hif::LibraryDef &o);
    virtual int visitProcedure(hif::Procedure &o);
    virtual int visitStateTable(hif::StateTable &o);
    virtual int visitSystem(hif::System &o);
    virtual int visitTypeDef(hif::TypeDef &o);
    virtual int visitTypeTP(hif::TypeTP &o);
    virtual int visitView(hif::View &o);

    int visitDeclaration(hif::Declaration &o);
    int visitTypeDeclaration(hif::TypeDeclaration &o);
    int visitBaseContents(hif::BaseContents &o);
    int visitSubProgram(hif::SubProgram &o);

    template <typename T> bool _isConflicting(const std::string &n, BList<T> &list);

private:
    std::string _name;

    // Disabled.
    ConflictChecker(const ConflictChecker &);
    ConflictChecker &operator=(const ConflictChecker &);
};
template <typename T> bool ConflictChecker::_isConflicting(const std::string &n, BList<T> &list)
{
    for (typename BList<T>::iterator i = list.begin(); i != list.end(); ++i) {
        if ((*i)->getName() == n)
            return true;
    }

    return false;
}

ConflictChecker::ConflictChecker(const std::string &n)
    : _name(n)
{
    // Nothing to do.
}

ConflictChecker::~ConflictChecker()
{
    // Nothing to do.
}

#if (defined _MSC_VER)
#    pragma warning(push)
// disabling unreferenced params under Windows
#    pragma warning(disable : 4805)
#endif
int ConflictChecker::visitContents(hif::Contents &o)
{
    int ret(0);
    ret |= _isConflicting(_name, o.libraries);
    ret |= visitBaseContents(o);
    return ret;
}

int ConflictChecker::visitDesignUnit(hif::DesignUnit &o)
{
    int ret(0);
    ret |= visitDeclaration(o);
    ret |= _isConflicting(_name, o.views);
    return ret;
}

int ConflictChecker::visitEntity(hif::Entity &o)
{
    int ret(0);
    ret |= visitDeclaration(o);
    ret |= _isConflicting(_name, o.ports);
    ret |= _isConflicting(_name, o.parameters);

    View *parent = dynamic_cast<View *>(o.getParent());
    if (parent != nullptr) {
        ret |= _name == parent->getName();

        DesignUnit *gparent = dynamic_cast<DesignUnit *>(parent->getParent());
        if (gparent != nullptr)
            ret |= _name == gparent->getName();
    }

    return ret;
}

int ConflictChecker::visitForGenerate(hif::ForGenerate &o)
{
    int ret(0);
    ret |= _isConflicting(_name, o.initDeclarations);
    ret |= visitBaseContents(o);
    return ret;
}

int ConflictChecker::visitFunction(hif::Function &o)
{
    int ret(0);
    ret |= visitSubProgram(o);
    return ret;
}

int ConflictChecker::visitIfGenerate(hif::IfGenerate &o)
{
    int ret(0);
    ret |= visitBaseContents(o);
    return ret;
}

int ConflictChecker::visitLibraryDef(hif::LibraryDef &o)
{
    int ret(0);
    ret |= visitDeclaration(o);
    ret |= _isConflicting(_name, o.libraries);
    ret |= _isConflicting(_name, o.declarations);
    return ret;
}

int ConflictChecker::visitProcedure(hif::Procedure &o)
{
    int ret(0);
    ret |= visitSubProgram(o);
    return ret;
}

int ConflictChecker::visitStateTable(hif::StateTable &o)
{
    int ret(0);
    ret |= visitDeclaration(o);
    ret |= _isConflicting(_name, o.declarations);
    //            ret |= _isConflicting( _name, o.edges );
    //            ret |= _isConflicting( _name, o.states );
    return ret;
}

int ConflictChecker::visitSystem(hif::System &o)
{
    int ret(0);
    ret |= visitDeclaration(o);
    ret |= _isConflicting(_name, o.declarations);
    ret |= _isConflicting(_name, o.designUnits);
    ret |= _isConflicting(_name, o.libraryDefs);
    ret |= _isConflicting(_name, o.libraries);
    return ret;
}

int ConflictChecker::visitTypeDef(hif::TypeDef &o)
{
    int ret(0);
    ret |= visitTypeDeclaration(o);
    ret |= _isConflicting(_name, o.templateParameters);
    return ret;
}

int ConflictChecker::visitTypeTP(hif::TypeTP &o) { return visitTypeDeclaration(o); }

int ConflictChecker::visitView(hif::View &o)
{
    int ret(0);
    ret |= visitDeclaration(o);
    ret |= _isConflicting(_name, o.declarations);
    ret |= _isConflicting(_name, o.inheritances);
    ret |= _isConflicting(_name, o.libraries);
    ret |= _isConflicting(_name, o.templateParameters);

    DesignUnit *parent = dynamic_cast<DesignUnit *>(o.getParent());
    if (parent != nullptr)
        ret |= _name == parent->getName();

    return ret;
}
int ConflictChecker::visitDeclaration(hif::Declaration &o) { return o.getName() == _name; }

int ConflictChecker::visitTypeDeclaration(hif::TypeDeclaration &o) { return visitDeclaration(o); }

int ConflictChecker::visitBaseContents(hif::BaseContents &o)
{
    int ret(0);
    ret |= visitDeclaration(o);
    ret |= _isConflicting(_name, o.declarations);
    ret |= _isConflicting(_name, o.instances);
    ret |= _isConflicting(_name, o.generates);
    ret |= _isConflicting(_name, o.stateTables);
    return ret;
}

int ConflictChecker::visitSubProgram(hif::SubProgram &o)
{
    int ret(0);
    ret |= visitDeclaration(o);
    ret |= _isConflicting(_name, o.parameters);
    ret |= _isConflicting(_name, o.templateParameters);
    return ret;
}
#if defined(_MSC_VER) && !(defined __GNUC__)
#    pragma warning(pop)
#endif

} // namespace

bool checkConflictingName(const std::string &n, Scope *scope)
{
    ConflictChecker cc(n);
    return scope->acceptVisitor(cc) != 0;
}

} // namespace manipulation
} // namespace hif
