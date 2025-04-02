/// @file backendManipulations.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif.hpp"

#include "hif/backends/backendManipulations.hpp"

namespace hif
{
namespace backends
{

namespace /*anon*/
{

template <typename T>
void _makeParametersAssignable(
    T *call,
    hif::semantics::ILanguageSemantics *sem,
    bool shiftToZero,
    unsigned int skip,
    bool removeCastOnOutputs)
{
    if (shiftToZero) {
        unsigned int count = 0;
        for (BList<ParameterAssign>::iterator ii = call->parameterAssigns.begin(); ii != call->parameterAssigns.end();
             ++ii) {
            ++count;
            if (count <= skip)
                continue;

            ParameterAssign *pa = *ii;
            Type *t             = hif::semantics::getSemanticType(pa->getValue(), sem);
            messageAssert(t != nullptr, "Cannot type parameter value", pa->getValue(), sem);

            Range *rt = hif::typeGetSpan(t, sem);
            if (rt == nullptr)
                continue;

            IntValue *i = dynamic_cast<IntValue *>(hif::rangeGetMinBound(rt));
            if (i != nullptr && i->getValue() == 0LL)
                continue;
            t = hif::copy(t);
            hif::manipulation::rebaseTypeSpan(t, sem, true);

            Cast *c = new Cast();
            c->setType(t);
            c->setValue(pa->setValue(nullptr));
            pa->setValue(c);
        }
    }

    std::list<Declaration *> list;
    hif::semantics::GetCandidatesOptions opt;
    opt.atLeastOne = true;
    opt.location   = nullptr;
    hif::semantics::getCandidates(list, call, sem, opt);
    messageAssert(list.size() == 1, "Expected just one candidate.", call, sem);

    typedef typename T::DeclarationType SubprogramType;
    SubprogramType *inst               = nullptr;
    BList<ParameterAssign> *callParams = nullptr;
    hif::manipulation::InstantiateOptions instOpt;
    instOpt.candidate = dynamic_cast<SubprogramType *>(list.front());
    inst              = hif::manipulation::instantiate(call, sem, instOpt);
    messageAssert(inst != nullptr, "Unable to instantiate", call, sem);

    callParams         = &call->parameterAssigns;
    unsigned int count = 0;
    for (BList<ParameterAssign>::iterator i(callParams->begin()); i != callParams->end(); ++i) {
        ++count;
        if (count <= skip)
            continue;
        BList<Parameter>::iterator j = inst->parameters.begin();
        bool matched                 = false;
        for (; j != inst->parameters.end(); ++j) {
            if ((*j)->getName() != (*i)->getName())
                continue;
            matched = true;
            break;
        }
        messageAssert(matched, "Cannot find formal parameter", *i, sem);

        Type *formal = (*j)->getType();
        Type *actual = hif::semantics::getSemanticType((*i)->getValue(), sem);
        messageAssert(formal != nullptr && actual != nullptr, "Cannot type parameter", *i, sem);

        if (removeCastOnOutputs &&
            (dynamic_cast<Reference *>(hif::semantics::getBaseType(formal, false, sem)) != nullptr ||
             (*j)->getDirection() == hif::dir_out || (*j)->getDirection() == hif::dir_inout)) {
            // ref. design vhdl_openCores_plasma
            Value *v      = (*i)->getValue();
            Value *newVal = hif::getChildSkippingCasts(v);
            if (v == newVal)
                continue;
            (*i)->setValue(newVal);
            delete v;
            continue;
        }

        hif::semantics::ILanguageSemantics::ExpressionTypeInfo check = sem->getExprType(formal, actual, op_conv, *i);
        if (check.returnedType != nullptr)
            continue;

        Cast *c = new Cast();
        c->setType(hif::copy(formal));
        c->setValue((*i)->setValue(nullptr));
        (*i)->setValue(c);
    }
}

std::string _calculateInclude(Object *where, Scope *toInclude)
{
    LibraryDef *lib = hif::getNearestParent<LibraryDef>(toInclude, true);

    typedef std::vector<LibraryDef *> Libs;
    Libs includes;

    // Calculating include info:
    LibraryDef *currLib = lib;
    Libs::size_type i   = 0;
    while (currLib != nullptr) {
        i = 1;
        includes.push_back(currLib);
        currLib = dynamic_cast<LibraryDef *>(currLib->getParent());
    }

    // Calculating include relative path:
    LibraryDef *currPrefix = hif::getNearestParent<LibraryDef>(where, true);
    std::string dots;
    while (currPrefix != nullptr) {
        bool found = false;
        for (i = 0; i < includes.size(); ++i) {
            if (includes[i] != currPrefix)
                continue;
            found = true;
            break;
        }
        if (found)
            break;
        if (dots.empty())
            dots = "..";
        else
            dots += "/..";
        currPrefix = hif::getNearestParent<LibraryDef>(currPrefix, false);
    }

    // With standard toInclude generating "absolute" path
    Scope *tmpScope = toInclude;
    DesignUnit *du  = dynamic_cast<DesignUnit *>(toInclude);
    if (du != nullptr && !du->views.empty())
        tmpScope = du->views.front();
    if (hif::declarationIsPartOfStandard(tmpScope)) {
        dots = "";
        i    = includes.size();
    }

    // Calculating descending string:
    LibraryDef *l = nullptr;
    std::string dirs;
    for (; i > 0;) {
        --i;
        l = includes[i];
        if (dirs.empty())
            dirs = std::string(l->getName());
        //else dirs = std::string(l->getName()) + "/" + dirs;
        else
            dirs += "/" + std::string(l->getName());
    }

    if (dots.empty())
        return dirs;
    else if (dirs.empty())
        return dots;
    return dots + "/" + dirs;
}
Scope *_getNearestIncludeScope(Scope *scope)
{
    Scope *ret = getNearestParent<DesignUnit>(scope, true);
    if (ret != nullptr)
        return ret;

    ret = getNearestParent<LibraryDef>(scope, true);
    if (ret != nullptr)
        return ret;

    ret = getNearestParent<System>(scope, true);
    if (ret != nullptr)
        return ret;

    return nullptr;
}

} // namespace

void makeParametersAssignable(
    ProcedureCall *call,
    hif::semantics::ILanguageSemantics *sem,
    bool shiftToZero,
    unsigned int skip,
    bool removeCastOnOutputs)
{
    _makeParametersAssignable(call, sem, shiftToZero, skip, removeCastOnOutputs);
}

void makeParametersAssignable(
    FunctionCall *call,
    hif::semantics::ILanguageSemantics *sem,
    bool shiftToZero,
    unsigned int skip,
    bool removeCastOnOutputs)
{
    _makeParametersAssignable(call, sem, shiftToZero, skip, removeCastOnOutputs);
}

bool addEventualCast(Value *v, Type *valueType, Type *originalType)
{
    if (hif::equals(valueType, originalType))
        return false;
    Cast *c = new Cast();
    c->setType(hif::copy(originalType));
    v->replace(c);
    c->setValue(v);

    return true;
}

bool addHifLibrary(
    const std::string &libName,
    Object *scope,
    System *system,
    hif::semantics::ILanguageSemantics *sem,
    bool standard)
{
    hif::HifFactory factory(sem);

    LibraryDef *ld = sem->getStandardLibrary(libName);
    hif::manipulation::AddUniqueObjectOptions addOpt;
    addOpt.equalsOptions.checkOnlyNames = true;
    addOpt.position                     = 0;
    hif::manipulation::addUniqueObject(ld, system->libraryDefs, addOpt);

    bool added = false;
    if (scope != nullptr) {
        Object *s = hif::getNearestScope(scope, false, true, false);
        if (s == nullptr)
            return false;
        Library *ref = factory.library(libName.c_str(), nullptr, "", standard, true);
        hif::manipulation::AddUniqueObjectOptions addOpt2;
        addOpt2.deleteIfNotAdded = true;
        hif::manipulation::addUniqueObject(ref, s, addOpt2);
        added = true;
    }

    return added;
}

std::string calculateIncludePath(
    Scope *where,
    Scope *toInclude,
    const std::string &cppHeaderExtension,
    hif::semantics::ILanguageSemantics *sem)
{
    if (where == toInclude && dynamic_cast<System *>(where) == nullptr)
        return "";
    where     = _getNearestIncludeScope(where);
    toInclude = _getNearestIncludeScope(toInclude);
    messageAssert(where != nullptr && toInclude != nullptr, "Unable to get a valid include scope.", nullptr, sem);

    // For std decls, ask to sem!
    LibraryDef *includeLib = getNearestParent<LibraryDef>(toInclude, true);
    if (includeLib != nullptr && sem->isStandardInclusion(includeLib->getName(), true)) {
        // TODO: manage nested components of standard!
        return includeLib->getName();
    }

    std::string includePath = _calculateInclude(where, toInclude);

    Scope *parent         = toInclude;
    DesignUnit *newParent = getNearestParent<DesignUnit>(toInclude, true);
    while (newParent != nullptr) {
        newParent = getNearestParent<DesignUnit>(newParent, false);
        if (newParent != nullptr)
            parent = newParent;
    }

    std::string parentName      = parent->getName();
    std::string specifiedDuName = "";
    if (dynamic_cast<System *>(parent) != nullptr) {
        parentName = NameTable::getInstance()->hifGlobals();
    } else if (dynamic_cast<DesignUnit *>(parent) != nullptr) {
        DesignUnit *du = static_cast<DesignUnit *>(parent);
        messageAssert(du->views.size() == 1, "Unexpected number of views", du, sem);
        specifiedDuName = du->views.front()->getFilename();
    }

    if (!includePath.empty())
        includePath += "/";

    if (specifiedDuName.empty()) {
        includePath += parentName;
        std::string ext = "";
        if (objectGetLanguage(parent) == hif::c ||
            (includeLib != nullptr && includeLib->getLanguageID() == hif::cpp && includeLib->hasCLinkage())) {
            ext = ".h"; // C only.
        } else {
            ext = "." + cppHeaderExtension;
        }
        includePath += ext;
    } else {
        includePath += specifiedDuName;
    }

    return includePath;
}

} // namespace backends
} // namespace hif
