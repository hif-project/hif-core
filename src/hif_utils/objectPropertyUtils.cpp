/// @file objectPropertyUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/objectPropertyUtils.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

void objectSetName(Object *obj, const std::string &n)
{
    hif::features::INamedObject *no = dynamic_cast<hif::features::INamedObject *>(obj);
    if (no == nullptr)
        return;
    no->setName(n);
}

std::string objectGetName(Object *obj)
{
    hif::features::INamedObject *no = dynamic_cast<hif::features::INamedObject *>(obj);
    if (no == nullptr)
        return NameTable::getInstance()->none();
    return no->getName();
}

bool objectMatchName(Object *obj, const std::string &nameToMatch)
{
    hif::features::INamedObject *no = dynamic_cast<hif::features::INamedObject *>(obj);
    if (no == nullptr)
        return false;
    return no->matchName(nameToMatch);
}
Type *objectGetType(Object *obj)
{
    if (dynamic_cast<DataDeclaration *>(obj) != nullptr) {
        DataDeclaration *d = static_cast<DataDeclaration *>(obj);
        return d->getType();
    } else if (dynamic_cast<Function *>(obj) != nullptr) {
        Function *d = static_cast<Function *>(obj);
        return d->getType();
    } else if (dynamic_cast<TypeDeclaration *>(obj) != nullptr) {
        TypeDeclaration *d = static_cast<TypeDeclaration *>(obj);
        return d->getType();
    } else if (dynamic_cast<Type *>(obj) != nullptr) {
        Type *d = static_cast<Type *>(obj);
        return d;
    } else if (dynamic_cast<Cast *>(obj) != nullptr) {
        Cast *d = static_cast<Cast *>(obj);
        return d->getType();
    } else if (dynamic_cast<ConstValue *>(obj) != nullptr) {
        ConstValue *d = static_cast<ConstValue *>(obj);
        return d->getType();
    } else if (dynamic_cast<Instance *>(obj) != nullptr) {
        Instance *d = static_cast<Instance *>(obj);
        return d->getReferencedType();
    }

    return nullptr;
}
Type *objectSetType(Object *obj, Type *t)
{
    if (dynamic_cast<DataDeclaration *>(obj) != nullptr) {
        DataDeclaration *d = static_cast<DataDeclaration *>(obj);
        return d->setType(t);
    } else if (dynamic_cast<Function *>(obj) != nullptr) {
        Function *d = static_cast<Function *>(obj);
        return d->setType(t);
    } else if (dynamic_cast<TypeDeclaration *>(obj) != nullptr) {
        TypeDeclaration *d = static_cast<TypeDeclaration *>(obj);
        return d->setType(t);
    } else if (dynamic_cast<Cast *>(obj) != nullptr) {
        Cast *d = static_cast<Cast *>(obj);
        return d->setType(t);
    } else if (dynamic_cast<ConstValue *>(obj) != nullptr) {
        ConstValue *d = static_cast<ConstValue *>(obj);
        return d->setType(t);
    } else if (dynamic_cast<Instance *>(obj) != nullptr) {
        Instance *d        = static_cast<Instance *>(obj);
        ReferencedType *rt = dynamic_cast<ReferencedType *>(t);
        if (rt == nullptr) {
            messageAssert(false, "Unexpected type", t, nullptr);
        }
        return d->setReferencedType(rt);
    }

    return nullptr;
}
BList<TPAssign> *objectGetTemplateAssignList(Object *obj)
{
    if (dynamic_cast<ViewReference *>(obj) != nullptr) {
        return &static_cast<ViewReference *>(obj)->templateParameterAssigns;
    } else if (dynamic_cast<TypeReference *>(obj) != nullptr) {
        return &static_cast<TypeReference *>(obj)->templateParameterAssigns;
    } else if (dynamic_cast<FunctionCall *>(obj) != nullptr) {
        return &static_cast<FunctionCall *>(obj)->templateParameterAssigns;
    } else if (dynamic_cast<ProcedureCall *>(obj) != nullptr) {
        return &static_cast<ProcedureCall *>(obj)->templateParameterAssigns;
    }

    return nullptr;
}
BList<Declaration> *objectGetTemplateParameterList(Object *obj)
{
    if (dynamic_cast<SubProgram *>(obj) != nullptr) {
        return &static_cast<SubProgram *>(obj)->templateParameters;
    } else if (dynamic_cast<View *>(obj) != nullptr) {
        return &static_cast<View *>(obj)->templateParameters;
    } else if (dynamic_cast<TypeDef *>(obj) != nullptr) {
        return &static_cast<TypeDef *>(obj)->templateParameters;
    }
    return nullptr;
}
BList<Parameter> *objectGetParameterList(Object *obj)
{
    if (dynamic_cast<SubProgram *>(obj) != nullptr) {
        return &static_cast<SubProgram *>(obj)->parameters;
    }
    return nullptr;
}
BList<Declaration> *objectGetDeclarationList(Object *obj)
{
    if (dynamic_cast<BaseContents *>(obj) != nullptr) {
        return &static_cast<BaseContents *>(obj)->declarations;
    } else if (dynamic_cast<StateTable *>(obj) != nullptr) {
        return &static_cast<StateTable *>(obj)->declarations;
    } else if (dynamic_cast<LibraryDef *>(obj) != nullptr) {
        return &static_cast<LibraryDef *>(obj)->declarations;
    } else if (dynamic_cast<View *>(obj) != nullptr) {
        return &static_cast<View *>(obj)->declarations;
    } else if (dynamic_cast<System *>(obj) != nullptr) {
        return &static_cast<System *>(obj)->declarations;
    }
    return nullptr;
}
BList<Library> *objectGetLibraryList(Object *obj)
{
    if (dynamic_cast<DesignUnit *>(obj) != nullptr) {
        View *v = static_cast<DesignUnit *>(obj)->views.front();
        return (v == nullptr) ? nullptr : &v->libraries;
    } else if (dynamic_cast<LibraryDef *>(obj) != nullptr) {
        return &static_cast<LibraryDef *>(obj)->libraries;
    } else if (dynamic_cast<View *>(obj) != nullptr) {
        return &static_cast<View *>(obj)->libraries;
    } else if (dynamic_cast<Contents *>(obj) != nullptr) {
        return &static_cast<Contents *>(obj)->libraries;
    } else if (dynamic_cast<System *>(obj) != nullptr) {
        return &static_cast<System *>(obj)->libraries;
    }
    return nullptr;
}
hif::LanguageID objectGetLanguage(Object *obj)
{
    if (obj == nullptr)
        return hif::rtl; // Default...

    if (dynamic_cast<View *>(obj) != nullptr) {
        return static_cast<View *>(obj)->getLanguageID();
    } else if (dynamic_cast<DesignUnit *>(obj) != nullptr) {
        DesignUnit *du = static_cast<DesignUnit *>(obj);
        messageAssert(!du->views.empty() && du->views.size() == 1, "Unexpected numeber of views", du, nullptr);
        return du->views.front()->getLanguageID();
    } else if (dynamic_cast<LibraryDef *>(obj) != nullptr) {
        return static_cast<LibraryDef *>(obj)->getLanguageID();
    } else if (dynamic_cast<System *>(obj) != nullptr) {
        return static_cast<System *>(obj)->getLanguageID();
    }

    View *parentView = hif::getNearestParent<View>(obj);
    if (parentView != nullptr)
        return parentView->getLanguageID();

    LibraryDef *parentLD = hif::getNearestParent<LibraryDef>(obj);
    if (parentLD != nullptr)
        return parentLD->getLanguageID();

    System *parentSys = hif::getNearestParent<System>(obj);
    if (parentSys != nullptr)
        return parentSys->getLanguageID();

    messageError("Unexpected case", obj, nullptr);
}
void objectSetLanguage(Object *obj, const hif::LanguageID id, bool recursive)
{
    if (obj == nullptr)
        return;

    if (dynamic_cast<View *>(obj) != nullptr) {
        static_cast<View *>(obj)->setLanguageID(id);
    }
    if (dynamic_cast<DesignUnit *>(obj) != nullptr) {
        DesignUnit *du = static_cast<DesignUnit *>(obj);
        messageAssert(!du->views.empty() && du->views.size() == 1, "Unexpected numeber of views", du, nullptr);
        du->views.front()->setLanguageID(id);
    } else if (dynamic_cast<LibraryDef *>(obj) != nullptr) {
        static_cast<LibraryDef *>(obj)->setLanguageID(id);
    } else if (dynamic_cast<System *>(obj) != nullptr) {
        static_cast<System *>(obj)->setLanguageID(id);
    }

    if (!recursive)
        return;

    View *parentView = hif::getNearestParent<View>(obj);
    if (parentView != nullptr) {
        parentView->setLanguageID(id);
        return;
    }

    LibraryDef *parentLD = hif::getNearestParent<LibraryDef>(obj);
    if (parentLD != nullptr) {
        parentLD->setLanguageID(id);
        return;
    }

    System *parentSys = hif::getNearestParent<System>(obj);
    if (parentSys != nullptr) {
        parentSys->setLanguageID(id);
        return;
    }

    messageDebugAssert(false, "Unexpected case", nullptr, nullptr);
}

bool objectIsInSensitivityList(Object *obj, const ObjectSensitivityOptions &opts)
{
    return objectGetSensitivityList(obj, opts) != nullptr;
}

BList<Value> *objectGetSensitivityList(Object *obj, const ObjectSensitivityOptions &opts)
{
    if (obj == nullptr)
        return nullptr;
    Value *v = dynamic_cast<Value *>(obj);
    if (v == nullptr)
        return nullptr;
    StateTable *st = nullptr;
    if (opts.directParent)
        st = dynamic_cast<StateTable *>(v->getParent());
    else
        st = hif::getNearestParent<StateTable>(v);
    if (st == nullptr)
        return nullptr;
    if (!opts.checkAll) {
        // Optimization for processes sensitivities
        State *state = hif::getNearestParent<State>(v);
        if (state != nullptr)
            return nullptr;
    }

    // Supporting also fcalls inside sensitivity.
    // Ref design: verilog/vams_dg_vams/connect_modules/bidir2
    ParameterAssign *parentParam = dynamic_cast<ParameterAssign *>(v->getParent());
    Value *parentValue           = dynamic_cast<Value *>(v->getParent());
    while (parentParam != nullptr || parentValue != nullptr) {
        if ((!opts.considerFunctionCalls) && parentParam != nullptr)
            return nullptr;
        if (parentValue != nullptr) {
            if (dynamic_cast<PrefixedReference *>(parentValue) != nullptr) {
                PrefixedReference *tmp = static_cast<PrefixedReference *>(parentValue);
                if (tmp->getPrefix() != v)
                    return nullptr;
            }
            v           = parentValue;
            parentParam = dynamic_cast<ParameterAssign *>(parentValue->getParent());
            parentValue = dynamic_cast<Value *>(parentValue->getParent());
        } else {
            v           = nullptr;
            parentValue = dynamic_cast<Value *>(parentParam->getParent());
            parentParam = nullptr;
        }
    }

    if (v == nullptr)
        return nullptr;

    if (dynamic_cast<StateTable *>(v->getParent()) != nullptr) {
        StateTable *sst = static_cast<StateTable *>(v->getParent());
        if (sst->sensitivity.contains(v))
            return &sst->sensitivity;
        else if (sst->sensitivityPos.contains(v))
            return &sst->sensitivityPos;
        else if (sst->sensitivityNeg.contains(v))
            return &sst->sensitivityNeg;
        return nullptr;
    }

    if (opts.checkAll) {
        Wait *w = dynamic_cast<Wait *>(v->getParent());
        if (w == nullptr)
            return nullptr;
        if (w->sensitivity.contains(v))
            return &w->sensitivity;
        else if (w->sensitivityPos.contains(v))
            return &w->sensitivityPos;
        else if (w->sensitivityNeg.contains(v))
            return &w->sensitivityNeg;
        return nullptr;
    }

    return nullptr;
}

bool objectIsProcess(Object *o)
{
    if (o == nullptr)
        return false;

    StateTable *st = dynamic_cast<StateTable *>(o);
    Assign *a      = dynamic_cast<Assign *>(o);
    if (st == nullptr && a == nullptr)
        return false;
    if (o->getParent() == nullptr)
        return true;

    if (a != nullptr) {
        GlobalAction *p = dynamic_cast<GlobalAction *>(o->getParent());
        if (p == nullptr || !a->isInBList())
            return false;
        return (reinterpret_cast<BList<Object> *>(&p->actions) == a->getBList());
    }

    BaseContents *p = dynamic_cast<BaseContents *>(o->getParent());
    if (p == nullptr || !st->isInBList())
        return false;
    return (reinterpret_cast<BList<Object> *>(&p->stateTables) == st->getBList());
}

bool objectIsInProcess(Object *o, bool matchAssigns, bool matchStarting)
{
    auto ret = objectGetProcess(o, matchAssigns, matchStarting);
    return ret != nullptr;
}

Object *objectGetProcess(Object *o, bool matchAssigns, bool matchStarting)
{
    if (o == nullptr)
        return nullptr;

    // standard state tables
    StateTable *st = getNearestParent<StateTable>(o, matchStarting);
    if (st != nullptr) {
        if (o->getParent() == nullptr)
            return st;
        auto parent = dynamic_cast<BaseContents *>(o->getParent());
        if (parent == nullptr || !st->isInBList())
            return nullptr;
        return st;
    }

    // assigns
    if (!matchAssigns)
        return nullptr;
    Assign *ass = getNearestParent<Assign>(o, matchStarting);
    if (ass == nullptr || o->getParent() == nullptr)
        return nullptr;
    auto parent = dynamic_cast<GlobalAction *>(o->getParent());
    if (parent == nullptr || !ass->isInBList())
        return nullptr;
    if (&parent->actions.toOtherBList<Object>() != ass->getBList())
        return nullptr;
    return ass;
}

bool objectSetInstance(Object *obj, Object *instance)
{
    if (obj == nullptr)
        return false;

    if (dynamic_cast<Library *>(obj) != nullptr && dynamic_cast<ReferencedType *>(instance) != nullptr) {
        Library *v = static_cast<Library *>(obj);
        delete v->setInstance(static_cast<ReferencedType *>(instance));
    } else if (dynamic_cast<ViewReference *>(obj) != nullptr && dynamic_cast<ReferencedType *>(instance) != nullptr) {
        ViewReference *v = static_cast<ViewReference *>(obj);
        delete v->setInstance(static_cast<ReferencedType *>(instance));
    } else if (dynamic_cast<TypeReference *>(obj) != nullptr && dynamic_cast<ReferencedType *>(instance) != nullptr) {
        TypeReference *v = static_cast<TypeReference *>(obj);
        delete v->setInstance(static_cast<ReferencedType *>(instance));
    } else if (dynamic_cast<Instance *>(obj) != nullptr && dynamic_cast<ReferencedType *>(instance) != nullptr) {
        Instance *v = static_cast<Instance *>(obj);
        delete v->setReferencedType(dynamic_cast<ReferencedType *>(instance));
    } else if (dynamic_cast<FunctionCall *>(obj) != nullptr && dynamic_cast<Value *>(instance) != nullptr) {
        FunctionCall *v = static_cast<FunctionCall *>(obj);
        delete v->setInstance(static_cast<Value *>(instance));
    } else if (dynamic_cast<ProcedureCall *>(obj) != nullptr && dynamic_cast<Value *>(instance) != nullptr) {
        ProcedureCall *v = static_cast<ProcedureCall *>(obj);
        delete v->setInstance(static_cast<Value *>(instance));
    } else if (dynamic_cast<FieldReference *>(obj) != nullptr && dynamic_cast<Value *>(instance) != nullptr) {
        FieldReference *v = static_cast<FieldReference *>(obj);
        delete v->setPrefix(static_cast<Value *>(instance));
    } else {
        return false;
    }

    // matched instance
    return true;
}
Object *objectGetInstance(Object *obj)
{
    if (obj == nullptr)
        return nullptr;

    if (dynamic_cast<Library *>(obj) != nullptr) {
        Library *v = static_cast<Library *>(obj);
        return v->getInstance();
    } else if (dynamic_cast<ViewReference *>(obj) != nullptr) {
        ViewReference *v = static_cast<ViewReference *>(obj);
        return v->getInstance();
    } else if (dynamic_cast<TypeReference *>(obj) != nullptr) {
        TypeReference *v = static_cast<TypeReference *>(obj);
        return v->getInstance();
    } else if (dynamic_cast<Instance *>(obj) != nullptr) {
        Instance *v = static_cast<Instance *>(obj);
        return v->getReferencedType();
    } else if (dynamic_cast<FunctionCall *>(obj) != nullptr) {
        FunctionCall *v = static_cast<FunctionCall *>(obj);
        return v->getInstance();
    } else if (dynamic_cast<ProcedureCall *>(obj) != nullptr) {
        ProcedureCall *v = static_cast<ProcedureCall *>(obj);
        return v->getInstance();
    } else if (dynamic_cast<FieldReference *>(obj) != nullptr) {
        FieldReference *f = static_cast<FieldReference *>(obj);
        return f->getPrefix();
    }

    return nullptr;
}

bool objectIsStandard(Object *o)
{
    if (o == nullptr)
        return false;

    if (dynamic_cast<Alias *>(o) != nullptr) {
        Alias *tmp = static_cast<Alias *>(o);
        return tmp->isStandard();
    } else if (dynamic_cast<Const *>(o) != nullptr) {
        Const *tmp = static_cast<Const *>(o);
        return tmp->isStandard();
    } else if (dynamic_cast<Library *>(o) != nullptr) {
        Library *tmp = static_cast<Library *>(o);
        return tmp->isStandard();
    } else if (dynamic_cast<LibraryDef *>(o) != nullptr) {
        LibraryDef *tmp = static_cast<LibraryDef *>(o);
        return tmp->isStandard();
    } else if (dynamic_cast<Signal *>(o) != nullptr) {
        Signal *tmp = static_cast<Signal *>(o);
        return tmp->isStandard();
    } else if (dynamic_cast<StateTable *>(o) != nullptr) {
        StateTable *tmp = static_cast<StateTable *>(o);
        return tmp->isStandard();
    } else if (dynamic_cast<SubProgram *>(o) != nullptr) {
        SubProgram *tmp = static_cast<SubProgram *>(o);
        return tmp->isStandard();
    } else if (dynamic_cast<TypeDef *>(o) != nullptr) {
        TypeDef *tmp = static_cast<TypeDef *>(o);
        return tmp->isStandard();
    } else if (dynamic_cast<Variable *>(o) != nullptr) {
        Variable *tmp = static_cast<Variable *>(o);
        return tmp->isStandard();
    } else if (dynamic_cast<View *>(o) != nullptr) {
        View *tmp = static_cast<View *>(o);
        return tmp->isStandard();
    }

    return false;
}

void objectSetStandard(Object *o, bool isStandard)
{
    if (o == nullptr)
        return;

    if (dynamic_cast<Alias *>(o) != nullptr) {
        Alias *tmp = static_cast<Alias *>(o);
        return tmp->setStandard(isStandard);
    } else if (dynamic_cast<Const *>(o) != nullptr) {
        Const *tmp = static_cast<Const *>(o);
        return tmp->setStandard(isStandard);
    } else if (dynamic_cast<Library *>(o) != nullptr) {
        Library *tmp = static_cast<Library *>(o);
        return tmp->setStandard(isStandard);
    } else if (dynamic_cast<LibraryDef *>(o) != nullptr) {
        LibraryDef *tmp = static_cast<LibraryDef *>(o);
        return tmp->setStandard(isStandard);
    } else if (dynamic_cast<Signal *>(o) != nullptr) {
        Signal *tmp = static_cast<Signal *>(o);
        return tmp->setStandard(isStandard);
    } else if (dynamic_cast<StateTable *>(o) != nullptr) {
        StateTable *tmp = static_cast<StateTable *>(o);
        return tmp->setStandard(isStandard);
    } else if (dynamic_cast<SubProgram *>(o) != nullptr) {
        SubProgram *tmp = static_cast<SubProgram *>(o);
        return tmp->setStandard(isStandard);
    } else if (dynamic_cast<TypeDef *>(o) != nullptr) {
        TypeDef *tmp = static_cast<TypeDef *>(o);
        return tmp->setStandard(isStandard);
    } else if (dynamic_cast<Variable *>(o) != nullptr) {
        Variable *tmp = static_cast<Variable *>(o);
        return tmp->setStandard(isStandard);
    } else if (dynamic_cast<View *>(o) != nullptr) {
        View *tmp = static_cast<View *>(o);
        return tmp->setStandard(isStandard);
    }
}
bool objectIsExternal(Object *o)
{
    if (dynamic_cast<TypeDef *>(o) != nullptr) {
        TypeDef *local = static_cast<TypeDef *>(o);
        return local->isExternal();
    }

    return false;
}

void objectSetExternal(Object *o, bool isExternal)
{
    if (dynamic_cast<TypeDef *>(o) != nullptr) {
        TypeDef *local = static_cast<TypeDef *>(o);
        local->setExternal(isExternal);
    }
}

bool objectIsNUllPointer(Object *o, hif::semantics::ILanguageSemantics *sem)
{
    Cast *c = dynamic_cast<Cast *>(o);
    if (c == nullptr)
        return false;
    Pointer *p = dynamic_cast<Pointer *>(hif::semantics::getBaseType(c->getType(), false, sem));
    if (p == nullptr)
        return false;
    IntValue *i = dynamic_cast<IntValue *>(c->getValue());
    if (i == nullptr)
        return false;
    return (i->getValue() == 0LL);
}

PortDirection objectGetPortDirection(Object *o)
{
    if (dynamic_cast<Port *>(o) != nullptr) {
        Port *obj = static_cast<Port *>(o);
        return obj->getDirection();
    } else if (dynamic_cast<Field *>(o) != nullptr) {
        Field *obj = static_cast<Field *>(o);
        return obj->getDirection();
    }

    return hif::dir_none;
}

void objectSetPortDirection(Object *o, PortDirection dir)
{
    if (dynamic_cast<Port *>(o) != nullptr) {
        Port *obj = static_cast<Port *>(o);
        obj->setDirection(dir);
    } else if (dynamic_cast<Field *>(o) != nullptr) {
        Field *obj = static_cast<Field *>(o);
        obj->setDirection(dir);
    }
}

bool objectGetProcessFlavour(Object *obj, ProcessFlavour &flavour)
{
    if (obj == nullptr)
        return false;
    auto st = dynamic_cast<StateTable *>(obj);
    if (st != nullptr) {
        flavour = st->getFlavour();
        return true;
    }
    return objectGetProcessFlavour(obj->getParent(), flavour);
}

ObjectSensitivityOptions::ObjectSensitivityOptions()
    : checkAll(false)
    , directParent(false)
    , considerFunctionCalls(true)
{
    // ntd
}

ObjectSensitivityOptions::~ObjectSensitivityOptions()
{
    // ntd
}

ObjectSensitivityOptions::ObjectSensitivityOptions(const ObjectSensitivityOptions &other)
    : checkAll(other.checkAll)
    , directParent(other.directParent)
    , considerFunctionCalls(other.considerFunctionCalls)
{
    // ntd
}

ObjectSensitivityOptions &ObjectSensitivityOptions::operator=(const ObjectSensitivityOptions &other)
{
    if (this == &other)
        return *this;
    checkAll              = other.checkAll;
    directParent          = other.directParent;
    considerFunctionCalls = other.considerFunctionCalls;
    return *this;
}

} // namespace hif
