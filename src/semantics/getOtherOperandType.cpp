/// @file getOtherOperandType.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/getOtherOperandType.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/TypeVisitor.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

typedef std::set<Object *> VisitedSet;
typedef std::list<VisitedSet> VisitedSetList;

VisitedSetList _visitedSetList;

Type *_getOtherOperandType(
    Object *o,
    ILanguageSemantics *sem,
    bool considerOverloading,
    bool looseTypeChecks,
    bool needOnlyDirection,
    bool isRecursive);

bool _isUpto   = false;
bool _isDownto = false;

bool _isVisited(Object *o)
{
    for (VisitedSetList::iterator i = _visitedSetList.begin(); i != _visitedSetList.end(); ++i) {
        VisitedSet &set = *i;
        if (set.find(o) == set.end())
            continue;
        return true;
    }

    return false;
}
Type *_getWhenOtherOperandType(
    When *w,
    Object * /*starting*/,
    ILanguageSemantics *sem,
    bool considerOverloading,
    bool looseTypeChecks,
    bool needOnlyDirection,
    bool isRecursive)
{
    if (w->getDefault() != nullptr && !_isVisited(w->getDefault())) {
        return getSemanticType(w->getDefault(), sem);
    }

    BList<WhenAlt>::iterator i = w->alts.begin();
    for (; i != w->alts.end(); ++i) {
        WhenAlt *wa = *i;
        if (_isVisited(wa->getValue()))
            continue;
        return getSemanticType(wa->getValue(), sem);
    }

    // all visited or no default (can happends during standardization) => search into parent
    return _getOtherOperandType(w, sem, considerOverloading, looseTypeChecks, needOnlyDirection, isRecursive);
}

Type *_getOtherOperandType(
    Object *o,
    ILanguageSemantics *sem,
    bool considerOverloading,
    bool looseTypeChecks,
    bool needOnlyDirection,
    bool isRecursive)
{
    if (o == nullptr)
        return nullptr;
    Object *parent = o->getParent();
    if (parent == nullptr)
        return nullptr;

    if (_isVisited(o)) {
        // its a loop! try to search into parent!
        return _getOtherOperandType(parent, sem, considerOverloading, looseTypeChecks, needOnlyDirection, isRecursive);
    }
    _visitedSetList.back().insert(o);

    if (dynamic_cast<Assign *>(parent) != nullptr) {
        Assign *assign = static_cast<Assign *>(parent);
        if (assign->getLeftHandSide() == o) {
            return getSemanticType(assign->getRightHandSide(), sem);
        }

        return getSemanticType(assign->getLeftHandSide(), sem);
    } else if (dynamic_cast<Expression *>(parent) != nullptr) {
        Expression *expr = static_cast<Expression *>(parent);

        if (needOnlyDirection) {
            _isUpto = true;
            return nullptr;
        }

        if (expr->getOperator() == op_concat && !needOnlyDirection) {
            return _getOtherOperandType(
                parent, sem, considerOverloading, looseTypeChecks, needOnlyDirection, isRecursive);
        }

        Type *t = nullptr;
        if (expr->getValue1() == o) {
            if (expr->getValue2() == nullptr)
                return nullptr;
            t = getSemanticType(expr->getValue2(), sem);
        } else if (expr->getValue2() != nullptr && expr->getValue2() == o) {
            t = getSemanticType(expr->getValue1(), sem);
        }

        if (t != nullptr)
            return t;

        return _getOtherOperandType(parent, sem, false, looseTypeChecks, needOnlyDirection, isRecursive);
    } else if (dynamic_cast<DataDeclaration *>(parent) != nullptr) {
        DataDeclaration *other = static_cast<DataDeclaration *>(parent);
        if (other->getValue() == o) {
            return other->getType();
        }

        return nullptr;
    } else if (dynamic_cast<When *>(parent) != nullptr) {
        When *when = static_cast<When *>(parent);
        return _getWhenOtherOperandType(when, o, sem, false, looseTypeChecks, needOnlyDirection, isRecursive);
    } else if (dynamic_cast<WhenAlt *>(parent) != nullptr) {
        // The value is inside an alt.
        // Get the type of the default
        WhenAlt *alt_o = static_cast<WhenAlt *>(parent);
        When *when_o   = dynamic_cast<When *>(alt_o->getParent());
        if (when_o == nullptr)
            return nullptr;

        return _getWhenOtherOperandType(when_o, o, sem, false, looseTypeChecks, needOnlyDirection, isRecursive);
    } else if (dynamic_cast<With *>(parent) != nullptr) {
        With *with = dynamic_cast<With *>(parent);
        if (with->getDefault() == o) {
            // there must be at least one alt
            if (with->alts.empty())
                return nullptr;
            return getSemanticType(with->alts.front()->getValue(), sem);
        }
        return nullptr;
    } else if (dynamic_cast<WithAlt *>(parent) != nullptr) {
        // The aggregate is inside an alt.
        // Get the type of the default
        WithAlt *alt_o = static_cast<WithAlt *>(parent);
        With *with_o   = dynamic_cast<With *>(alt_o->getParent());
        if (with_o == nullptr || with_o->getDefault() == nullptr)
            return nullptr;

        return getSemanticType(with_o->getDefault(), sem);
    } else if (dynamic_cast<Aggregate *>(parent)) {
        Aggregate *parentAgg         = static_cast<Aggregate *>(parent);
        // Recursive case
        const VisitedSetList restore = _visitedSetList;
        Array *arr                   = dynamic_cast<Array *>(getBaseType(
            _getOtherOperandType(parentAgg, sem, false, looseTypeChecks, needOnlyDirection, isRecursive), false, sem,
            false));
        if (arr != nullptr) {
            hif::semantics::updateDeclarations(arr->getType(), sem);
            return arr->getType();
        }
        _visitedSetList = restore;

        Type *tt = _getOtherOperandType(parentAgg, sem, false, looseTypeChecks, needOnlyDirection, true);
        messageAssert(tt != nullptr, "Unable to calculate type of nested Aggregate", parent, nullptr);
        return tt;
    } else if (dynamic_cast<AggregateAlt *>(parent) != nullptr) {
        // Recursive case - alt is not a value
        Aggregate *grandParent = dynamic_cast<Aggregate *>(parent->getParent());
        messageAssert((grandParent != nullptr), "Unexpected parent of AggregateAlt", parent->getParent(), nullptr);

        const VisitedSetList restore = _visitedSetList;
        Type *tt   = _getOtherOperandType(grandParent, sem, false, looseTypeChecks, needOnlyDirection, isRecursive);
        Type *bt   = hif::semantics::getBaseType(tt, false, sem, false);
        Array *arr = dynamic_cast<Array *>(bt);
        if (arr != nullptr) {
            // Determine type of alt
            Type *t = arr->getType();
            hif::semantics::updateDeclarations(t, sem);
            return t;
        }
        _visitedSetList = restore;
        tt              = _getOtherOperandType(grandParent, sem, false, looseTypeChecks, needOnlyDirection, true);
        if (tt == nullptr) {
            messageDebug("Full-object:", grandParent, nullptr);
            messageDebug("Other operand of full object", tt, nullptr);
            messageError("Unable to calculate type of nested AggregateAlt", parent, nullptr);
        }
        hif::semantics::updateDeclarations(tt, sem);
        return tt;
    } else if (dynamic_cast<RecordValueAlt *>(parent) != nullptr) {
        RecordValueAlt *rva = static_cast<RecordValueAlt *>(parent);
        RecordValue *rv     = dynamic_cast<RecordValue *>(parent->getParent());
        messageAssert(rv != nullptr, "Unexpected parent", parent->getParent(), sem);

        Type *tt    = _getOtherOperandType(rv, sem, false, looseTypeChecks, needOnlyDirection, isRecursive);
        Type *base  = hif::semantics::getBaseType(tt, false, sem, false);
        Record *rec = dynamic_cast<Record *>(base);
        messageAssert(rec != nullptr, "Unexpected case", tt, sem);

        for (BList<Field>::iterator i = rec->fields.begin(); i != rec->fields.end(); ++i) {
            if ((*i)->getName() != rva->getName())
                continue;
            hif::semantics::updateDeclarations((*i)->getType(), sem);
            return (*i)->getType();
        }

        return nullptr;
    } else if (dynamic_cast<ParameterAssign *>(parent) != nullptr) {
        ParameterAssign *ref = static_cast<ParameterAssign *>(parent);
        Object *parentCall   = ref->getParent();
        if (ref->checkProperty(PROPERTY_TEMPORARY_OBJECT) ||
            (parentCall != nullptr && parentCall->checkProperty(PROPERTY_TEMPORARY_OBJECT))) {
            // When fcall is templatic, a loop can occur,
            // since the getDeclaration calls the getImplicitTemplate(),
            // which calls getSemanticType() and thus this method.
            // So we always return downto.
            // Ref design: verilog/vams_ips/SMACI,
            //             verilog/vams_std_afem/lowpass1
            return nullptr;
        }

        TypeVisitor tv(sem, false);
        tv._getTypeOfParameterAssign(ref, considerOverloading, looseTypeChecks);
        return ref->getSemanticType();
    } else if (dynamic_cast<PortAssign *>(parent) != nullptr) {
        PortAssign *ref = static_cast<PortAssign *>(parent);
        TypeVisitor tv(sem, false);
        tv._getTypeOfPortAssign(ref);
        return ref->getSemanticType();
    } else if (dynamic_cast<ValueTPAssign *>(parent) != nullptr) {
        ValueTPAssign *ref = static_cast<ValueTPAssign *>(parent);
        TypeVisitor tv(sem, false);
        tv._getTypeOfValueTPAssign(ref);
        return ref->getSemanticType();
    } else if (dynamic_cast<Cast *>(parent) != nullptr) {
        Cast *c = dynamic_cast<Cast *>(parent);
        hif::semantics::updateDeclarations(c->getType(), sem);
        return c->getType();
    } else if (dynamic_cast<FunctionCall *>(parent) != nullptr) {
        return _getOtherOperandType(parent, sem, false, looseTypeChecks, needOnlyDirection, isRecursive);
    } else if (dynamic_cast<SwitchAlt *>(parent) != nullptr) {
        return _getOtherOperandType(parent, sem, false, looseTypeChecks, needOnlyDirection, isRecursive);
    } else if (dynamic_cast<Switch *>(parent) != nullptr) {
        Switch *s = static_cast<Switch *>(parent);
        if (o == s->getCondition())
            return nullptr;
        Type *t = hif::semantics::getSemanticType(s->getCondition(), sem);
        return t;
    } else if (dynamic_cast<Return *>(parent) != nullptr) {
        Return *r      = static_cast<Return *>(parent);
        Function *func = hif::getNearestParent<Function>(r);
        messageAssert(func != nullptr, "Expected function parent", r, sem);
        return func->getType();
    } else if (dynamic_cast<Slice *>(parent) != nullptr) {
        // Ref design: vhdl/openCores/PresetEnc + ddt
        Type *other = _getOtherOperandType(parent, sem, false, looseTypeChecks, needOnlyDirection, isRecursive);
        if (needOnlyDirection)
            return other;
        if (other == nullptr)
            return other;
        // This is a leak! Avoid leak!
        other = hif::copy(other);
        hif::semantics::addInTypeCache(other);
        Slice *s = static_cast<Slice *>(parent);
        hif::typeSetSpan(other, hif::copy(s->getSpan()), sem, true);

        return other;
    } else if (dynamic_cast<Member *>(parent) != nullptr) {
        // ref. design vhdl_openCoresFunbase_hibi_udp
        if (!isRecursive)
            return nullptr;
        Type *tt = _getOtherOperandType(parent, sem, false, looseTypeChecks, needOnlyDirection, false);
        if (tt == nullptr) {
            messageDebug("Full-object:", parent, nullptr);
            messageDebug("Other operand of full object", tt, nullptr);
            messageError("Unable to calculate type of member other operand type", parent, nullptr);
        }

        // Determine type of member
        hif::semantics::updateDeclarations(tt, sem);
        return tt;
    }
    return nullptr;
}

} // namespace

Type *
getOtherOperandType(Object *o, ILanguageSemantics *sem, bool considerOverloading, bool looseTypeChecks)
{
    VisitedSet visitedSet;
    _visitedSetList.push_back(visitedSet);
    Type *ret = _getOtherOperandType(o, sem, considerOverloading, looseTypeChecks, false, false);
    _visitedSetList.pop_back();
    return ret;
}

hif::RangeDirection getOtherOperandDirection(Object *o, ILanguageSemantics *sem)
{
    VisitedSet visitedSet;
    _visitedSetList.push_back(visitedSet);

    _isUpto   = false;
    _isDownto = false;
    Type *tmp = _getOtherOperandType(o, sem, false, false, true, false);
    if (_isUpto) {
        _visitedSetList.pop_back();
        return dir_upto;
    } else if (_isDownto) {
        _visitedSetList.pop_back();
        return dir_downto;
    }

    tmp = getBaseType(tmp, false, sem, true);
    if (dynamic_cast<Record *>(tmp) != nullptr) {
        _visitedSetList.pop_back();
        return dir_upto;
    }
    while (tmp != nullptr && dynamic_cast<hif::features::ITypeSpan *>(tmp) == nullptr) {
        tmp = _getOtherOperandType(o, sem, false, false, true, false);
        tmp = getBaseType(tmp, false, sem, true);
        if (_isUpto) {
            _visitedSetList.pop_back();
            return dir_upto;
        } else if (_isDownto) {
            _visitedSetList.pop_back();
            return dir_downto;
        } else if (dynamic_cast<Record *>(tmp) != nullptr) {
            _visitedSetList.pop_back();
            return dir_upto;
        }
    }

    _visitedSetList.pop_back();

    Range *span = hif::typeGetSpan(tmp, sem);
    if (span == nullptr) {
        return dir_downto;
    }

    return span->getDirection();
}

} // namespace semantics
} // namespace hif
