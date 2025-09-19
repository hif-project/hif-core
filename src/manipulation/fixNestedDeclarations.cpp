/// @file fixNestedDeclarations.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <queue>

#include "hif/BiVisitor.hpp"
#include "hif/hif.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#    pragma clang diagnostic ignored "-Wmissing-noreturn"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

namespace hif
{
namespace manipulation
{
namespace
{

/// @brief Warning: move semantics.
/// Does not have ownership.
struct MoveData {
    hif::semantics::ILanguageSemantics *sem;
    Scope *newScope;

    MoveData();
    MoveData(const MoveData &d);
    ~MoveData();
    MoveData &operator=(const MoveData &d);
};

struct Categories {
    typedef std::set<Declaration *> TemplateSet;
    typedef std::set<Parameter *> ParameterSet;
    typedef std::set<DataDeclaration *> DataDeclarationSet;
    typedef std::set<TypeDef *> TypeDefSet;

    Categories();
    ~Categories();

    TemplateSet templates;
    ParameterSet parameters;
    DataDeclarationSet dataDeclarations;
    TypeDefSet typeDefs;
};

// ///////////////////////////////////////////////////////////////////
// MoveDeclarationMap
// ///////////////////////////////////////////////////////////////////
class MoveDeclarationVisitor : public BiVisitor<MoveDeclarationVisitor>
{
public:
    MoveDeclarationVisitor(MoveData &data);
    virtual ~MoveDeclarationVisitor();

    void map(Object *nested, Object *parent);
    void map(SubProgram *nested, SubProgram *parent);
    void map(StateTable *nested, SubProgram *parent);
    void map(TypeDef *nested, Scope *parent);

private:
    void _categorizeSymbols(Declaration *nested, Declaration *parent, Categories &categories);
    MoveData &_data;

    // disabled.
    MoveDeclarationVisitor(const MoveDeclarationVisitor &);
    MoveDeclarationVisitor &operator=(const MoveDeclarationVisitor &);
};

MoveData::MoveData()
    : sem(nullptr)
    , newScope(nullptr)
{
    // Nothing to do.
}

MoveData::MoveData(const MoveData &d)
    : sem(d.sem)
    , newScope(d.newScope)
{
    // Nothing to do.
}

MoveData::~MoveData()
{
    // Nothing to do.
}

MoveData &MoveData::operator=(const MoveData &d)
{
    if (this == &d)
        return *this;
    sem      = d.sem;
    newScope = d.newScope;
    return *this;
}

Categories::Categories()
    : templates()
    , parameters()
    , dataDeclarations()
    , typeDefs()
{
    // Nothing to do.
}

Categories::~Categories()
{
    // Nothing to do.
}

void MoveDeclarationVisitor::_categorizeSymbols(Declaration *nested, Declaration *parent, Categories &categories)
{
    std::list<Object *> symbols;
    hif::semantics::collectSymbols(symbols, nested);

    for (std::list<Object *>::iterator i = symbols.begin(); i != symbols.end(); ++i) {
        Declaration *decl = hif::semantics::getDeclaration(*i, _data.sem);
        if (decl == nullptr) {
            messageError("Declaratio not found", *i, _data.sem);
        }

        // Matching only symbols declared inside parent subprogram but not
        // inside nested subprogram.
        if (hif::isSubNode(decl, nested))
            continue;
        if (!hif::isSubNode(decl, parent))
            continue;

        if (dynamic_cast<TypeDef *>(decl) != nullptr) {
            TypeDef *o = static_cast<TypeDef *>(decl);
            categories.typeDefs.insert(o);
        } else if (dynamic_cast<Parameter *>(decl) != nullptr) {
            Parameter *o = static_cast<Parameter *>(decl);
            categories.parameters.insert(o);
        } else if (dynamic_cast<DataDeclaration *>(decl) != nullptr) {
            DataDeclaration *o = static_cast<DataDeclaration *>(decl);
            categories.dataDeclarations.insert(o);
        } else if (dynamic_cast<TypeTP *>(decl) != nullptr) {
            TypeTP *o = static_cast<TypeTP *>(decl);
            categories.templates.insert(o);
        } else if (dynamic_cast<ValueTP *>(decl) != nullptr) {
            ValueTP *o = static_cast<ValueTP *>(decl);
            categories.templates.insert(o);
        }
    }
}

MoveDeclarationVisitor::MoveDeclarationVisitor(MoveData &data)
    : _data(data)
{
    // ntd
}

MoveDeclarationVisitor::~MoveDeclarationVisitor()
{
    // ntd
}

void MoveDeclarationVisitor::map(Object * /*nested*/, Object * /*parent*/)
{
    messageError("Unexpected objects reached by MoveDeclarationMap", nullptr, nullptr);
}

void MoveDeclarationVisitor::map(StateTable * /*nested*/, SubProgram * /*parent*/)
{
    messageError("MoveDeclarationMap case StateTable to SubProgram not handled yet.", nullptr, nullptr);
}

void MoveDeclarationVisitor::map(SubProgram *nested, SubProgram *parent)
{
    // template< int foo_tp1, class foo_tp2, ... >
    // void foo( int foo_p1, foo_tp2 foo_p2, ... )
    // {
    //     int local1 = 0;
    //     int local2 = 0;
    //     typedef int pippo;
    //     pippo foo_p;
    //     [...]
    //     template< int poo_tp1, class poo_tp2, ... >
    //     void poo( int poo_p1, foo_tp2 poo_t2, ... )
    //     {
    //         pippo poo_p;
    //         int local3 = local1 + foo_tp1;
    //         int local4 = foo_p1 + poo_p1;
    //         foo_tp2 a = nullptr;
    //         [...]
    //     }
    //     [...]
    //     poo<3,C1>( 5, C2, ... )
    //     [...]
    // }
    //
    // CONVERTED TO:
    //
    // typedef int pippo_fresh;
    //
    // template< int foo_tp1, class foo_tp2, ... >
    // void foo( int foo_p1, foo_tp2 foo_p2, ... )
    // {
    //     int local1 = 0;
    //     int local2 = 0;
    //     pippo_fresh foo_p;
    //     [...]
    //     poo<3, C1, foo_tp1, foo_tp2>( 5, C2, ..., foo_p1, local1 )
    //     [...]
    // }
    //
    // template< int poo_tp1, class poo_tp2, ... , int foo_tp1, class foo_tp2 >
    // void poo( int poo_p1, foo_tp2 poo_t2, ..., foo_p1, local1 )
    // {
    //     pippo_fresh poo_p;
    //     int local3 = local1 + foo_tp1;
    //     int local4 = foo_p1 + poo_p1;
    //     foo_tp2 a = nullptr;
    //     [...]
    // }

    hif::semantics::ReferencesSet nestedReferences;
    hif::semantics::getReferences(nested, nestedReferences, _data.sem, parent);

    Categories categories;
    _categorizeSymbols(nested, parent, categories);

    // Move collected declarations into new scope.
    for (Categories::ParameterSet::iterator i = categories.parameters.begin(); i != categories.parameters.end(); ++i) {
        // Step 1: change references
        for (hif::semantics::ReferencesSet::iterator j = nestedReferences.begin(); j != nestedReferences.end(); ++j) {
            ParameterAssign *pa = new ParameterAssign();
            pa->setName((*i)->getName());
            pa->setValue(new Identifier((*i)->getName()));
            if (dynamic_cast<FunctionCall *>(*j) != nullptr) {
                FunctionCall *f = static_cast<FunctionCall *>(*j);
                f->parameterAssigns.push_back(pa);
            } else if (dynamic_cast<ProcedureCall *>(*j) != nullptr) {
                ProcedureCall *p = static_cast<ProcedureCall *>(*j);
                p->parameterAssigns.push_back(pa);
            } else {
                delete pa;
                messageDebugAssert(false, "Unexpected object", *j, _data.sem);
            }
        }

        // Step 2: change nested declaration
        Parameter *pa = hif::copy(*i);
        delete pa->setValue(nullptr);
        nested->parameters.push_back(pa);

        // Fixing nested declarations already set into refs:
        typedef hif::semantics::ReferencesSet RefsList;
        RefsList refsList;
        hif::semantics::getReferences(*i, refsList, _data.sem, nested);
        for (RefsList::iterator j = refsList.begin(); j != refsList.end(); ++j) {
            hif::semantics::setDeclaration(*j, pa);
        }
    }

    for (Categories::TemplateSet::iterator i = categories.templates.begin(); i != categories.templates.end(); ++i) {
        // Step 1: change references
        TPAssign *tpa   = nullptr;
        Declaration *tp = nullptr;
        if (dynamic_cast<TypeTP *>(*i) != nullptr) {
            TypeTP *ttp       = hif::copy(static_cast<TypeTP *>(*i));
            TypeReference *tr = new TypeReference();
            tr->setName(ttp->getName());
            TypeTPAssign *ttpa = new TypeTPAssign();
            ttpa->setName(ttp->getName());
            ttpa->setType(tr);
            tpa = ttpa;

            delete ttpa->setType(nullptr);
            tp = ttp;
        } else // if( dynamic_cast< ValueTP * >( *i ) != nullptr )
        {
            ValueTP *vtp        = hif::copy(static_cast<ValueTP *>(*i));
            ValueTPAssign *vtpa = new ValueTPAssign();
            vtpa->setName(vtp->getName());
            vtpa->setValue(new Identifier(vtp->getName()));
            tpa = vtpa;

            delete vtp->setValue(nullptr);
            tp = vtp;
        }

        for (hif::semantics::ReferencesSet::iterator j = nestedReferences.begin(); j != nestedReferences.end(); ++j) {
            if (dynamic_cast<FunctionCall *>(*j) != nullptr) {
                FunctionCall *f = static_cast<FunctionCall *>(*j);
                f->templateParameterAssigns.push_back(hif::copy(tpa));
            } else if (dynamic_cast<ProcedureCall *>(*j) != nullptr) {
                ProcedureCall *p = static_cast<ProcedureCall *>(*j);
                p->templateParameterAssigns.push_back(hif::copy(tpa));
            } else {
                assert(false);
            }
        }

        delete tpa;

        // Step 2: change nested declaration
        nested->templateParameters.push_back(tp);

        // Fixing nested declarations already set into refs:
        typedef hif::semantics::ReferencesSet RefsList;
        RefsList refsList;
        hif::semantics::getReferences(*i, refsList, _data.sem, nested);
        for (RefsList::iterator j = refsList.begin(); j != refsList.end(); ++j) {
            hif::semantics::setDeclaration(*j, tp);
        }
    }

    for (Categories::DataDeclarationSet::iterator i = categories.dataDeclarations.begin();
         i != categories.dataDeclarations.end(); ++i) {
        // Step 1: change references
        for (hif::semantics::ReferencesSet::iterator j = nestedReferences.begin(); j != nestedReferences.end(); ++j) {
            ParameterAssign *pa = new ParameterAssign();
            pa->setName((*i)->getName());
            pa->setValue(new Identifier((*i)->getName()));
            if (dynamic_cast<FunctionCall *>(*j) != nullptr) {
                FunctionCall *f = static_cast<FunctionCall *>(*j);
                f->parameterAssigns.push_back(pa);
            } else if (dynamic_cast<ProcedureCall *>(*j) != nullptr) {
                ProcedureCall *p = static_cast<ProcedureCall *>(*j);
                p->parameterAssigns.push_back(pa);
            } else {
                delete pa;
                messageDebugAssert(false, "Unexpected object", *j, _data.sem);
            }
        }

        // Step 2: change nested declaration
        Parameter *pa = new Parameter();
        pa->setName((*i)->getName());
        pa->setType(hif::copy((*i)->getType()));
        nested->parameters.push_back(pa);

        // Fixing nested declarations already set into refs:
        typedef hif::semantics::ReferencesSet RefsList;
        RefsList refsList;
        hif::semantics::getReferences(*i, refsList, _data.sem, nested);
        for (RefsList::iterator j = refsList.begin(); j != refsList.end(); ++j) {
            hif::semantics::setDeclaration(*j, pa);
        }
    }

    hif::semantics::resetTypes(nested);

    for (Categories::TypeDefSet::iterator i = categories.typeDefs.begin(); i != categories.typeDefs.end(); ++i) {
        map(*i, parent->getStateTable());
    }

    hif::manipulation::moveToScope(parent->getStateTable(), _data.newScope, _data.sem, "_moved", nested);
}

void MoveDeclarationVisitor::map(TypeDef *nested, Scope *parent)
{
    hif::semantics::ReferencesSet nestedReferences;
    hif::semantics::getReferences(nested, nestedReferences, _data.sem, parent);

    Categories categories;
    _categorizeSymbols(nested, parent, categories);

    // Move collected declarations into new scope.
    for (Categories::ParameterSet::iterator i = categories.parameters.begin(); i != categories.parameters.end(); ++i) {
        // Step 1: change references
        for (hif::semantics::ReferencesSet::iterator j = nestedReferences.begin(); j != nestedReferences.end(); ++j) {
            ValueTPAssign *vtpa = new ValueTPAssign();
            vtpa->setName((*i)->getName());
            vtpa->setValue(new Identifier((*i)->getName()));

            if (dynamic_cast<TypeReference *>(*j) != nullptr) {
                TypeReference *tr = static_cast<TypeReference *>(*j);
                tr->templateParameterAssigns.push_back(vtpa);
            } else {
                delete vtpa;
                messageDebugAssert(false, "Unexpected object", *j, _data.sem);
            }
        }

        // Step 2: change nested declaration
        ValueTP *vtp = new ValueTP();
        vtp->setName((*i)->getName());
        vtp->setType(hif::copy((*i)->getType()));
        nested->templateParameters.push_back(vtp);
    }

    for (Categories::TemplateSet::iterator i = categories.templates.begin(); i != categories.templates.end(); ++i) {
        // Step 1: change references
        TPAssign *tpa   = nullptr;
        Declaration *tp = nullptr;
        if (dynamic_cast<TypeTP *>(*i) != nullptr) {
            TypeTP *ttp       = hif::copy(static_cast<TypeTP *>(*i));
            TypeReference *tr = new TypeReference();
            tr->setName(ttp->getName());
            TypeTPAssign *ttpa = new TypeTPAssign();
            ttpa->setName(ttp->getName());
            ttpa->setType(tr);
            tpa = ttpa;

            delete ttpa->setType(nullptr);
            tp = ttp;
        } else // if( dynamic_cast< ValueTP * >( *i ) != nullptr )
        {
            ValueTP *vtp        = hif::copy(static_cast<ValueTP *>(*i));
            ValueTPAssign *vtpa = new ValueTPAssign();
            vtpa->setName(vtp->getName());
            vtpa->setValue(new Identifier(vtp->getName()));
            tpa = vtpa;

            delete vtp->setValue(nullptr);
            tp = vtp;
        }

        for (hif::semantics::ReferencesSet::iterator j = nestedReferences.begin(); j != nestedReferences.end(); ++j) {
            if (dynamic_cast<TypeReference *>(*j) != nullptr) {
                TypeReference *tr = static_cast<TypeReference *>(*j);
                tr->templateParameterAssigns.push_back(hif::copy(tpa));
            } else {
                messageDebugAssert(false, "Unexpected object", *j, _data.sem);
            }
        }

        delete tpa;

        // Step 2: change nested declaration
        nested->templateParameters.push_back(tp);
    }

    for (Categories::DataDeclarationSet::iterator i = categories.dataDeclarations.begin();
         i != categories.dataDeclarations.end(); ++i) {
        // Step 1: change references
        for (hif::semantics::ReferencesSet::iterator j = nestedReferences.begin(); j != nestedReferences.end(); ++j) {
            ValueTPAssign *vtpa = new ValueTPAssign();
            vtpa->setName((*i)->getName());
            vtpa->setValue(new Identifier((*i)->getName()));

            if (dynamic_cast<TypeReference *>(*j) != nullptr) {
                TypeReference *tr = static_cast<TypeReference *>(*j);
                tr->templateParameterAssigns.push_back(vtpa);
            } else {
                delete vtpa;
                messageDebugAssert(false, "Unexpected object", *j, _data.sem);
            }
        }

        // Step 2: change nested declaration
        ValueTP *vtp = new ValueTP();
        vtp->setName((*i)->getName());
        vtp->setType(hif::copy((*i)->getType()));
        nested->templateParameters.push_back(vtp);
    }

    for (Categories::TypeDefSet::iterator i = categories.typeDefs.begin(); i != categories.typeDefs.end(); ++i) {
        map(*i, parent);
    }

    hif::manipulation::moveToScope(parent, _data.newScope, _data.sem, "_moved", nested);
}
// ///////////////////////////////////////////////////////////////////
// _moveDeclaration method
// ///////////////////////////////////////////////////////////////////

void _moveDeclaration(Declaration *d, Declaration *root, Scope *newScope, hif::semantics::ILanguageSemantics *sem)
{
    MoveData data;
    data.sem      = sem;
    data.newScope = newScope;
    MoveDeclarationVisitor mdv(data);
    mdv.callMap(d, root);
}

// ///////////////////////////////////////////////////////////////////
// FixNestedDeclarationVisitor
// ///////////////////////////////////////////////////////////////////
class FixNestedDeclarationVisitor : public hif::GuideVisitor
{
public:
    /// @brief Default constructor.
    FixNestedDeclarationVisitor(hif::semantics::ILanguageSemantics *sem);

    /// @brief Destructor.
    virtual ~FixNestedDeclarationVisitor();

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

    bool hasFixed();

protected:
    std::queue<hif::Scope *> _queue;

    Scope *_getSuitableScope(Scope *src);

    /// @brief Moves symbol declarations appearing in old scope and declared
    /// in root, into new scope.
    ///
    void _moveNestedSymbols(Declaration *oldScope, Scope *root, Scope *newScope);

private:
    hif::semantics::ILanguageSemantics *_sem;
    bool _fixed;
    FixNestedDeclarationVisitor(const FixNestedDeclarationVisitor &);
    FixNestedDeclarationVisitor &operator=(const FixNestedDeclarationVisitor &);
};
FixNestedDeclarationVisitor::FixNestedDeclarationVisitor(hif::semantics::ILanguageSemantics *sem)
    : GuideVisitor()
    , _queue()
    , _sem(sem)
    , _fixed(false)
{
    // ntd
}

FixNestedDeclarationVisitor::~FixNestedDeclarationVisitor()
{
    // ntd
}

Scope *FixNestedDeclarationVisitor::_getSuitableScope(Scope *src)
{
    BaseContents *bc = hif::getNearestParent<BaseContents>(src);
    if (bc != nullptr)
        return bc;

    LibraryDef *ld = hif::getNearestParent<LibraryDef>(src);
    if (ld != nullptr)
        return ld;

    System *sy = hif::getNearestParent<System>(src);
    return sy;
}

void FixNestedDeclarationVisitor::_moveNestedSymbols(Declaration *oldScope, Scope *root, Scope *newScope)
{
    std::list<Object *> symbols;
    hif::semantics::collectSymbols(symbols, oldScope);

    for (std::list<Object *>::iterator i = symbols.begin(); i != symbols.end(); ++i) {
        _fixed            = true;
        Declaration *decl = hif::semantics::getDeclaration(*i, _sem);
        if (decl == nullptr) {
            messageError("Declaration not found", *i, _sem);
        }

        if (hif::isSubNode(decl, oldScope))
            continue;
        if (!hif::isSubNode(decl, root))
            continue;

        _moveNestedSymbols(decl, root, newScope);

        hif::manipulation::moveToScope(root, newScope, _sem, "_moved", decl);
    }
}

int FixNestedDeclarationVisitor::visitContents(hif::Contents &o)
{
    _queue.push(&o);
    GuideVisitor::visitContents(o);
    _queue.pop();
    return 0;
}

int FixNestedDeclarationVisitor::visitDesignUnit(hif::DesignUnit &o)
{
    _queue.push(&o);
    GuideVisitor::visitDesignUnit(o);
    _queue.pop();
    return 0;
}

int FixNestedDeclarationVisitor::visitEntity(hif::Entity &o)
{
    _queue.push(&o);
    GuideVisitor::visitEntity(o);
    _queue.pop();
    return 0;
}

int FixNestedDeclarationVisitor::visitForGenerate(hif::ForGenerate &o)
{
    _queue.push(&o);
    GuideVisitor::visitForGenerate(o);
    _queue.pop();
    return 0;
}

int FixNestedDeclarationVisitor::visitFunction(hif::Function &o)
{
    _queue.push(&o);
    GuideVisitor::visitFunction(o);
    _queue.pop();
    return 0;
}

int FixNestedDeclarationVisitor::visitIfGenerate(hif::IfGenerate &o)
{
    _queue.push(&o);
    GuideVisitor::visitIfGenerate(o);
    _queue.pop();
    return 0;
}

int FixNestedDeclarationVisitor::visitLibraryDef(hif::LibraryDef &o)
{
    _queue.push(&o);
    GuideVisitor::visitLibraryDef(o);
    _queue.pop();
    return 0;
}

int FixNestedDeclarationVisitor::visitProcedure(hif::Procedure &o)
{
    _queue.push(&o);
    GuideVisitor::visitProcedure(o);
    _queue.pop();
    return 0;
}

int FixNestedDeclarationVisitor::visitStateTable(hif::StateTable &o)
{
    _queue.push(&o);
    GuideVisitor::visitStateTable(o);
    _queue.pop();

    for (BList<Declaration>::iterator i = o.declarations.begin(); i != o.declarations.end();) {
        if (dynamic_cast<SubProgram *>(*i) == nullptr) {
            ++i;
            continue;
        }

        SubProgram *sp = static_cast<SubProgram *>(*i);

        // C++ does not support methods declared inside a state table
        // (process or method).

        ++i;
        Scope *parentScope = _getSuitableScope(&o);
        messageAssert(parentScope != nullptr, "Suitable scope not found", &o, _sem);

        SubProgram *parentSub = dynamic_cast<SubProgram *>(o.getParent());
        if (parentSub != nullptr) {
            _moveDeclaration(sp, parentSub, parentScope, _sem);
        } else {
            _moveDeclaration(sp, &o, parentScope, _sem);
        }
        _fixed = true;
    }
    return 0;
}

int FixNestedDeclarationVisitor::visitSystem(hif::System &o)
{
    _queue.push(&o);
    GuideVisitor::visitSystem(o);
    _queue.pop();
    return 0;
}

int FixNestedDeclarationVisitor::visitTypeDef(hif::TypeDef &o)
{
    _queue.push(&o);
    GuideVisitor::visitTypeDef(o);
    _queue.pop();
    return 0;
}

int FixNestedDeclarationVisitor::visitTypeTP(hif::TypeTP &o)
{
    _queue.push(&o);
    GuideVisitor::visitTypeTP(o);
    _queue.pop();
    return 0;
}

int FixNestedDeclarationVisitor::visitView(hif::View &o)
{
    _queue.push(&o);
    GuideVisitor::visitView(o);
    _queue.pop();
    return 0;
}

bool FixNestedDeclarationVisitor::hasFixed() { return _fixed; }

} // namespace
void fixNestedDeclarations(System *o, hif::semantics::ILanguageSemantics *sem)
{
    hif::application_utils::initializeLogHeader("Manipulation", "fixNestedDeclarations");

    FixNestedDeclarationVisitor v(sem);
    o->acceptVisitor(v);
    if (v.hasFixed()) {
        hif::semantics::resetTypes(o, true);
        hif::semantics::resetDeclarations(o);
        hif::semantics::flushTypeCacheEntries();
        hif::manipulation::flushInstanceCache();
    }
    hif::application_utils::restoreLogHeader();
}

} // namespace manipulation
} // namespace hif
