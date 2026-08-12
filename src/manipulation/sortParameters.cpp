/// @file sortParameters.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/sortParameters.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wmissing-noreturn"
#elif defined __GNUC__
#    if __GNUC__ >= 5
#        pragma GCC diagnostic ignored "-Wduplicated-cond"
#    endif
#endif
namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

#ifndef NDEBUG
void _debug_getImplicitValueParameter(
    Object *front,
    Object *poType,
    Object *tit,
    hif::semantics::ILanguageSemantics *sem)
{
    // useless, just avoiding compiler warnings
    if (front != nullptr && poType != nullptr && tit != nullptr && sem != nullptr) {
        messageDebug("No candidate found. Pattern is", front, sem);
        messageDebug("Reference tree is", poType, sem);
        messageDebug("Matched tree is", tit, sem);
    }
    assert(false);
}
#endif

void _updateActualDeclarations(ReferencedAssign *actual, hif::semantics::ILanguageSemantics *refSem)
{
    if (dynamic_cast<PPAssign *>(actual) != nullptr) {
        hif::semantics::updateDeclarations(static_cast<PPAssign *>(actual)->getValue(), refSem);
    } else if (dynamic_cast<ValueTPAssign *>(actual) != nullptr) {
        hif::semantics::updateDeclarations(static_cast<ValueTPAssign *>(actual)->getValue(), refSem);
    } else if (dynamic_cast<TypeTPAssign *>(actual) != nullptr) {
        hif::semantics::updateDeclarations(static_cast<TypeTPAssign *>(actual)->getType(), refSem);
    } else {
        messageDebugAssert(false, "Unexpected case", nullptr, refSem);
    }
}
template <typename T>
TPAssign *_getExplicitValueParameter(
    Declaration *formal,
    T *fcall,
    hif::semantics::ILanguageSemantics *refSem,
    bool /*hasCandidate*/)
{
    for (BList<TPAssign>::iterator it = fcall->templateParameterAssigns.begin();
         it != fcall->templateParameterAssigns.end(); ++it) {
        if ((*it)->getName() == formal->getName()) {
            _updateActualDeclarations(*it, refSem);
            return *it;
        }
    }
    return nullptr;
}
template <typename T>
TPAssign *_getImplicitValueParameter(
    Declaration *formal,
    T *fcall,
    hif::semantics::ILanguageSemantics *refSem,
    bool hasCandidate)
{
    SubProgram *sub = dynamic_cast<SubProgram *>(formal->getParent());
    messageAssert(sub != nullptr, "Unexpected parent", formal->getParent(), refSem);
    BList<ParameterAssign>::iterator it = fcall->parameterAssigns.begin();
    BList<Parameter>::iterator jt       = sub->parameters.begin();
    messageAssert(
        fcall->parameterAssigns.size() == sub->parameters.size(), "Unexpecte different list size", nullptr, refSem);

    for (; it != fcall->parameterAssigns.end(); ++it, ++jt) {
        Parameter *po     = *jt;
        Value *paramValue = (*it)->getValue();

        Type *tit = hif::semantics::getSemanticType(paramValue, refSem);
        messageAssert(tit != nullptr, "Unable to get type of value", paramValue, refSem);
        Type *titb = hif::semantics::getBaseType(tit, false, refSem, true);
        messageAssert(titb != nullptr, "Unable to get base type of value", tit, refSem);
        Type *pob = hif::semantics::getBaseType(po->getType(), false, refSem, true);
        messageAssert(pob != nullptr, "Unable to get base type of parameter", po->getType(), refSem);

        // TODO: maybe the two base types should be simplified...
        // but this could be unsafe.
        // Ref. design: verilog/vams_dg_va_rf_models/lossy_ind

        Object *obj = getImplicitTemplate(formal, pob, titb, refSem, hasCandidate);
        if (obj == nullptr)
            continue;

        // found!
        // distinguish case type template parameter assign vs
        // value template parameter assign.
        if (dynamic_cast<TypeTP *>(formal) != nullptr) {
            Type *tt = dynamic_cast<Type *>(obj);
            messageAssert(tt != nullptr, "Unexpected object", obj, refSem);

            TypeTPAssign *ret = new TypeTPAssign();
            ret->setName(formal->getName());
            hif::semantics::updateDeclarations(tt, refSem);
            ret->setType(hif::copy(tt));
            return ret;
        } else if (dynamic_cast<ValueTP *>(formal) != nullptr) {
            Value *val = dynamic_cast<Value *>(obj);
            messageAssert(val != nullptr, "Unexpected object", obj, refSem);

            ValueTPAssign *ret = new ValueTPAssign();
            ret->setName(formal->getName());
            hif::semantics::updateDeclarations(val, refSem);
            ret->setValue(hif::copy(val));

            ConstValue *cv = dynamic_cast<ConstValue *>(ret->getValue());
            if (cv != nullptr) {
                // set synt type
                cv->setType(refSem->getTypeForConstant(cv));
            }
            return ret;
        }
    }

    // TODO change this implementation with a more correct and standard check
    if (dynamic_cast<TypeTP *>(formal) != nullptr) {
        // K:
        // Maybe the TypeTP is the type of "this".
        // Example: we overload a standard instance method, as hdtlib::set_range().
        // So we check the call: if it is instance, we match the type of this.
        // this is done just because in HIF we do not have the declarations
        // of the classes.
        // Supported only for standard methods.
        if (fcall->getInstance() == nullptr)
            return nullptr;

        SubProgram *decl = hif::semantics::getDeclaration(fcall, refSem);
        if (decl == nullptr)
            return nullptr;
        if (decl->getKind() != SubProgram::IMPLICIT_INSTANCE)
            return nullptr;

        Type *tit = hif::semantics::getSemanticType(fcall->getInstance(), refSem);
        messageAssert(tit != nullptr || hasCandidate, "Cannot type instance", fcall, refSem);
        if (tit != nullptr) {
            TypeTPAssign *ret = new TypeTPAssign();
            ret->setName(formal->getName());
            hif::semantics::updateDeclarations(tit, refSem);
            ret->setType(hif::copy(tit));
            return ret;
        }
    }

    return nullptr;
}
void _fixReferencesToPreviousParameters(
    Declaration *formal,
    BList<TPAssign> &actualParams,
    Object *value,
    hif::semantics::ILanguageSemantics *refSem)
{
    // Example:
    // declaration: template < int i, int j = i + 10 > void foo();
    // call:        foo<5>();
    // when expliciting without this fix:
    // call:        foo<5,i+10>();
    // and then i will not be found correctly.
    // when expliciting with this fix:
    // call:        foo<5,5+10>();
    //
    // Reference design: Built_in_FIR.

    SubProgram *sub = dynamic_cast<SubProgram *>(formal->getParent());
    if (sub == nullptr) {
        messageAssert(sub != nullptr, "Unexpected parent", formal->getParent(), refSem);
        return;
    }

    BList<Declaration>::iterator currentDecl = sub->templateParameters.begin();
    Object *currentRoot                      = value;
    for (BList<TPAssign>::iterator i = actualParams.begin(); i != actualParams.end(); ++i, ++currentDecl) {
        if ((*i)->getName() == formal->getName())
            break;
        messageAssert(
            (*i)->getName() == (*currentDecl)->getName(), "Unexpected mismatch between formal and anctual param name",
            nullptr, refSem);

        hif::semantics::ReferencesSet list;
        hif::semantics::getReferences(*currentDecl, list, refSem, currentRoot);

        for (hif::semantics::ReferencesSet::iterator j = list.begin(); j != list.end(); ++j) {
            TypeTPAssign *typeTP   = dynamic_cast<TypeTPAssign *>(*i);
            ValueTPAssign *valueTP = dynamic_cast<ValueTPAssign *>(*i);
            Object *tmp            = *j;
            if (valueTP != nullptr) {
                Value *copy = hif::copy(valueTP->getValue());
                if (tmp == currentRoot)
                    currentRoot = copy;
                tmp->replace(copy);
                delete tmp;
            } else {
                Type *copy = hif::copy(typeTP->getType());
                if (tmp == currentRoot)
                    currentRoot = copy;
                tmp->replace(copy);
                delete tmp;
            }
        }
    }
}

template <typename T>
TPAssign *_getDefaultValueParameter(
    Declaration *formal,
    T * /*fcall*/,
    BList<TPAssign> &actualParams,
    hif::semantics::ILanguageSemantics *refSem,
    bool /*hasCandidate*/)
{
    TypeTP *typeTP   = dynamic_cast<TypeTP *>(formal);
    ValueTP *valueTP = dynamic_cast<ValueTP *>(formal);

    if (typeTP != nullptr && typeTP->getType() != nullptr) {
        TypeTPAssign *ret = new TypeTPAssign();
        ret->setName(typeTP->getName());
        hif::semantics::updateDeclarations(typeTP->getType(), refSem);
        ret->setType(hif::copy(typeTP->getType()));
        hif::semantics::setDeclaration(ret, typeTP);

        _fixReferencesToPreviousParameters(typeTP, actualParams, ret->getType(), refSem);

        return ret;
    } else if (valueTP != nullptr && valueTP->getValue() != nullptr) {
        ValueTPAssign *ret = new ValueTPAssign();
        ret->setName(valueTP->getName());
        hif::semantics::updateDeclarations(valueTP->getValue(), refSem);
        ret->setValue(hif::copy(valueTP->getValue()));
        hif::semantics::setDeclaration(ret, valueTP);

        _fixReferencesToPreviousParameters(valueTP, actualParams, ret->getValue(), refSem);

        return ret;
    }
    return nullptr;
}
/// Search the name in the template parameters assigns.
/// @return the corresponding TPAssign if it is present, nullptr
/// otherwise
///
template <typename T>
TPAssign *_getCallTPAssign(
    Declaration *formal,
    T *fcall,
    BList<TPAssign> &actualParams,
    hif::semantics::ILanguageSemantics *refSem,
    bool hasCandidate)
{
    // search the name in the template parameter assigns of the fcall
    TPAssign *ret = nullptr;

    // Case 1: explicitly assigned parameter TODO: check new?
    ret = _getExplicitValueParameter(formal, fcall, refSem, hasCandidate);
    if (ret != nullptr)
        return ret;

    // Case 2: implicitly assigned parameter
    ret = _getImplicitValueParameter(formal, fcall, refSem, hasCandidate);
    if (ret != nullptr)
        return ret;

    // Case 3: parameter with default value
    ret = _getDefaultValueParameter(formal, fcall, actualParams, refSem, hasCandidate);
    if (ret != nullptr)
        return ret;

    return ret;
}

#ifndef NDEBUG
void _debug_make_parameter(Declaration *formal, Object *call, hif::semantics::ILanguageSemantics *sem)
{
    if (formal != nullptr && call != nullptr && sem != nullptr) {
        messageDebug("No candidate found for call. Formal parameter is", formal, sem);
        messageDebug("Call is", call, sem);
    }
    assert(false);
}
#endif

void _addEventualCast(ValueTP *formal, ValueTPAssign *actual, hif::semantics::ILanguageSemantics *sem)
{
    if (actual == nullptr)
        return;
    messageAssert(actual->getValue() != nullptr, "Actual value tp without value", actual, sem);
    messageAssert(formal != nullptr, "Unexpected null formal param", nullptr, sem);
    messageAssert(formal->getType() != nullptr, "Unexpected formal param without type", formal, sem);

    Type *actualType = hif::semantics::getSemanticType(actual->getValue(), sem);

    if (hif::equals(formal->getType(), actualType))
        return;

    // Formal type may have symbols w/o declarations set.
    // Therefore updated declaration is needed since they could be not visible
    // from actual "context".
    // Ref. design: verilog/openCores/openMSP430.
    hif::semantics::updateDeclarations(formal->getType(), sem);
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo info;
    info = sem->getExprType(formal->getType(), actualType, op_conv, actual);
    if (info.returnedType != nullptr)
        return;

    Cast *c = new Cast();
    c->setValue(actual->getValue());
    actual->setValue(c);
    c->setType(hif::copy(formal->getType()));
}
TPAssign *_make_parameter(
    Declaration *formal,
    Object *call,
    BList<TPAssign> &actualParams,
    hif::semantics::ILanguageSemantics *refSem,
    bool hasCandidate)
{
    TPAssign *r          = nullptr;
    FunctionCall *fcall  = dynamic_cast<FunctionCall *>(call);
    ProcedureCall *pcall = dynamic_cast<ProcedureCall *>(call);

    if (fcall != nullptr) {
        if (dynamic_cast<ValueTP *>(formal) != nullptr) {
            ValueTP *f = static_cast<ValueTP *>(formal);
            r          = _getCallTPAssign(f, fcall, actualParams, refSem, hasCandidate);
#ifndef NDEBUG
            if (!hasCandidate && r == nullptr)
                _debug_make_parameter(formal, call, refSem);
#endif
        } else {
            TypeTP *f = static_cast<TypeTP *>(formal);
            r         = _getCallTPAssign(f, fcall, actualParams, refSem, hasCandidate);
#ifndef NDEBUG
            if (!hasCandidate && r == nullptr)
                _debug_make_parameter(formal, call, refSem);
#endif
        }
    } else if (pcall != nullptr) {
        if (dynamic_cast<ValueTP *>(formal) != nullptr) {
            ValueTP *f = static_cast<ValueTP *>(formal);
            r          = _getCallTPAssign(f, pcall, actualParams, refSem, hasCandidate);
#ifndef NDEBUG
            if (!hasCandidate && r == nullptr)
                _debug_make_parameter(formal, call, refSem);
#endif
        } else {
            TypeTP *f = static_cast<TypeTP *>(formal);
            r         = _getCallTPAssign(f, pcall, actualParams, refSem, hasCandidate);
#ifndef NDEBUG
            if (!hasCandidate && r == nullptr)
                _debug_make_parameter(formal, call, refSem);
#endif
        }
    } else // viewref or typeref
    {
        if (dynamic_cast<ValueTP *>(formal) != nullptr) {
            ValueTP *f = static_cast<ValueTP *>(formal);
            if (f->getValue() == nullptr)
                return nullptr;

            ValueTPAssign *ret = new ValueTPAssign();
            ret->setName(f->getName());
            hif::semantics::updateDeclarations(f->getValue(), refSem);
            ret->setValue(hif::copy(f->getValue()));
            hif::semantics::setDeclaration(ret, f);

            r = ret;
#ifndef NDEBUG
            if (!hasCandidate && r == nullptr)
                _debug_make_parameter(formal, call, refSem);
#endif
        } else {
            TypeTP *f = static_cast<TypeTP *>(formal);
            if (f->getType() == nullptr)
                return nullptr;

            TypeTPAssign *ret = new TypeTPAssign();
            ret->setName(f->getName());
            hif::semantics::updateDeclarations(f->getType(), refSem);
            ret->setType(hif::copy(f->getType()));
            hif::semantics::setDeclaration(ret, f);
            r = ret;
#ifndef NDEBUG
            if (!hasCandidate && r == nullptr)
                _debug_make_parameter(formal, call, refSem);
#endif
        }
    }

    return r;
}
PortAssign *_make_parameter(
    Port *formal,
    Object * /*call*/,
    BList<PortAssign> & /*actualParams*/,
    hif::semantics::ILanguageSemantics *refSem,
    bool /*hasCandidate*/)
{
    if (formal->getValue() == nullptr)
        return nullptr;

    hif::semantics::updateDeclarations(formal->getValue(), refSem);

    PortAssign *ret = new PortAssign();
    ret->setName(formal->getName());
    ret->setValue(hif::copy(formal->getValue()));

    return ret;
}
ParameterAssign *_make_parameter(
    DataDeclaration *formal,
    Object * /*call*/,
    BList<ParameterAssign> & /*actualParams*/,
    hif::semantics::ILanguageSemantics *refSem,
    bool /*hasCandidate*/)
{
    if (formal->getValue() == nullptr)
        return nullptr;

    hif::semantics::updateDeclarations(formal->getValue(), refSem);

    ParameterAssign *ret = new ParameterAssign();
    ret->setName(formal->getName());
    ret->setValue(hif::copy(formal->getValue()));

    return ret;
}
template <typename Actual, typename Formal>
bool _sortParameters(
    Object *call,
    BList<Actual> &actualParams,
    BList<Formal> &formalParams,
    bool set_names,
    SortMissingKind missingType,
    hif::semantics::ILanguageSemantics *refSem,
    bool hasCandidate = false)
{
    messageDebugIfFails(
        formalParams.size() >= actualParams.size() || hasCandidate, "Actuals:", formalParams.getParent(), refSem);
    messageDebugAssert(
        formalParams.size() >= actualParams.size() || hasCandidate, "Formals are less than actuals.", call, refSem);
    typedef std::list<Actual *> ActualList;
    ActualList named;
    ActualList unnamed;

    for (typename BList<Actual>::iterator i = actualParams.begin(); i != actualParams.end(); ++i) {
        Actual *actual = *i;
        if (actual->getName().empty() || actual->getName() == NameTable::getInstance()->none()) {
            unnamed.push_back(hif::copy(actual));
        } else {
            named.push_back(hif::copy(actual));
        }
    }
    actualParams.clear();

    bool ret = true;

    for (typename BList<Formal>::iterator i = formalParams.begin(); i != formalParams.end(); ++i) {
        bool found = false;
        for (typename ActualList::iterator n = named.begin(); n != named.end(); ++n) {
            if ((*i)->getName() != (*n)->getName())
                continue;

            // found in list of named
            actualParams.push_back(*n);
            named.erase(n);
            _updateActualDeclarations(actualParams.back(), refSem);
            found = true;
            break;
        }
        if (found)
            continue;

        // unnamed
        if (!unnamed.empty()) {
            // set formal name if option is true.
            if (set_names) {
                unnamed.front()->setName((*i)->getName());
            }
            actualParams.push_back(unnamed.front());
            _updateActualDeclarations(actualParams.back(), refSem);
            unnamed.pop_front();
        } else if (missingType == SortMissingKind::ALL) {
            Actual *pass = _make_parameter(*i, call, actualParams, refSem, hasCandidate);
            if (pass == nullptr) {
                ret = false;
                break;
            }
            actualParams.push_back(pass);

            ValueTP *formal          = dynamic_cast<ValueTP *>(*i);
            ValueTPAssign *valuePass = dynamic_cast<ValueTPAssign *>(pass);
            if (formal != nullptr && valuePass != nullptr) {
                _addEventualCast(formal, valuePass, refSem);
            }
        } else if (missingType == SortMissingKind::LIMITED && (!named.empty() /*|| !unnamed.empty()*/)) {
            Actual *pass = _make_parameter(*i, call, actualParams, refSem, hasCandidate);
            if (pass == nullptr) {
                ret = false;
                break;
            }
            actualParams.push_back(pass);

            ValueTP *formal          = dynamic_cast<ValueTP *>(*i);
            ValueTPAssign *valuePass = dynamic_cast<ValueTPAssign *>(pass);
            if (formal != nullptr && valuePass != nullptr) {
                _addEventualCast(formal, valuePass, refSem);
            }
        }
    }

    messageAssert((hasCandidate || (named.empty() && unnamed.empty())), "Expecting lists to be empty", call, refSem);
    for (typename ActualList::iterator it(named.begin()); it != named.end(); ++it) {
        delete *it;
    }
    for (typename ActualList::iterator it(unnamed.begin()); it != unnamed.end(); ++it) {
        delete *it;
    }

    return ret;
}
} // namespace
Object *getImplicitTemplate(
    Declaration *tp,
    Object *formalParameterType,
    Object *actualParameterType,
    hif::semantics::ILanguageSemantics *sem,
    bool hasCandidate)
{
    hif::semantics::ReferencesSet refs;
    hif::semantics::getReferences(tp, refs, sem, formalParameterType);
    if (refs.empty())
        return nullptr;

    if (dynamic_cast<TypeTP *>(tp) == nullptr && dynamic_cast<ValueTP *>(tp) == nullptr) {
        messageError("Given declaration is not a template declaration.", tp, sem);
    }

    Object *ref = *refs.begin();

    hif::manipulation::MatchObjectOptions opt;
    opt.matchStructure = true;
    opt.skipReferences = true;
    Object *obj        = hif::manipulation::matchObject(ref, formalParameterType, actualParameterType, sem, opt);

    if (obj == nullptr) {
        if (!hasCandidate) {
#ifndef NDEBUG
            _debug_getImplicitValueParameter(*refs.begin(), formalParameterType, actualParameterType, sem);
#endif
        }
        return nullptr;
    }

    return obj;
}
bool sortParameters(
    BList<ParameterAssign> &actualParams,
    BList<Parameter> &formalParams,
    bool set_formal_names,
    SortMissingKind missingType,
    hif::semantics::ILanguageSemantics *refSem,
    bool hasCandidate)
{
    hif::application_utils::initializeLogHeader("HIF", "sortParameters");
    bool ret = _sortParameters(
        actualParams.getParent(), actualParams, formalParams, set_formal_names, missingType, refSem, hasCandidate);
    hif::application_utils::restoreLogHeader();
    return ret;
}
bool sortParameters(
    BList<TPAssign> &actualParams,
    BList<Declaration> &formalParams,
    bool set_formal_names,
    SortMissingKind missingType,
    hif::semantics::ILanguageSemantics *refSem,
    bool hasCandidate)
{
    hif::application_utils::initializeLogHeader("HIF", "sortParameters");
    bool ret = _sortParameters(
        actualParams.getParent(), actualParams, formalParams, set_formal_names, missingType, refSem, hasCandidate);
    hif::application_utils::restoreLogHeader();
    return ret;
}
bool sortParameters(
    BList<PortAssign> &actualParams,
    BList<Port> &formalParams,
    bool set_formal_names,
    SortMissingKind missingType,
    hif::semantics::ILanguageSemantics *refSem)
{
    hif::application_utils::initializeLogHeader("HIF", "sortParameters");
    bool ret = _sortParameters(nullptr, actualParams, formalParams, set_formal_names, missingType, refSem);
    hif::application_utils::restoreLogHeader();
    return ret;
}
} // namespace manipulation
} // namespace hif
