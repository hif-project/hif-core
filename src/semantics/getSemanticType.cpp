/// @file getSemanticType.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/getSemanticType.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hifIOUtils.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/TypeVisitor.hpp"
#include "hif/semantics/semantics.hpp"
#include "hif/trash.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

// ///////////////////////////////////////////////////////////////////
// Cache management
// ///////////////////////////////////////////////////////////////////
struct TypeEntry {
    TypeEntry();
    ~TypeEntry();

    Scope *scope;
    Type *rawType;
    Type *simplifiedType;
    hif::semantics::ILanguageSemantics *sem;

    TypeEntry(const TypeEntry &o);
    TypeEntry &operator=(const TypeEntry &o);
    bool operator<(const TypeEntry &other) const;
};

TypeEntry::TypeEntry()
    : scope(nullptr)
    , rawType(nullptr)
    , simplifiedType(nullptr)
    , sem(nullptr)
{
    // ntd
}

TypeEntry::~TypeEntry()
{
    delete rawType;
    delete simplifiedType;
}

TypeEntry::TypeEntry(const TypeEntry &o)
    : scope(o.scope)
    , rawType(o.rawType)
    , simplifiedType(o.simplifiedType)
    , sem(o.sem)
{
    // warning move semantics
    TypeEntry *i      = const_cast<TypeEntry *>(&o);
    i->scope          = nullptr;
    i->rawType        = nullptr;
    i->simplifiedType = nullptr;
    i->sem            = nullptr;
}

TypeEntry &TypeEntry::operator=(const TypeEntry &o)
{
    // warning move semantics
    if (this == &o)
        return *this;
    this->~TypeEntry();
    new (this) TypeEntry(o);
    return *this;
}

bool TypeEntry::operator<(const TypeEntry &o) const
{
    hif::EqualsOptions opt;
    opt.assureSameSymbolDeclarations = true;

    if (sem != o.sem)
        return sem < o.sem;
    if (scope != o.scope)
        return scope < o.scope;

    if (hif::equals(rawType, o.rawType, opt)) {
        return false;
    }

    return (rawType < o.rawType);
}
typedef std::set<TypeEntry> EntriesSet;
EntriesSet entriesSet;
EntriesSet trash;
hif::Trash generalTrash;

bool _isInTypeCache(Scope *scope, Type *rawType, hif::semantics::ILanguageSemantics *sem)
{
    TypeEntry e;
    e.scope          = scope;
    e.rawType        = rawType;
    e.sem            = sem;
    const bool found = (entriesSet.find(e) != entriesSet.end());
    e.rawType        = nullptr;
    return found;
}

Type *searchTypeCacheEntry(Scope *scope, Type *rawType, hif::semantics::ILanguageSemantics *sem)
{
    TypeEntry e;
    e.scope                 = scope;
    e.rawType               = rawType;
    e.sem                   = sem;
    EntriesSet::iterator it = entriesSet.find(e);
    e.rawType               = nullptr;
    if (it == entriesSet.end())
        return nullptr;
    return (*it).simplifiedType;
}

void addTypeCacheEntry(Scope *scope, Type *rawType, hif::semantics::ILanguageSemantics *sem, Type *simplifiedType)
{
    if (_isInTypeCache(scope, rawType, sem)) {
        delete rawType;
        delete simplifiedType;
        return;
    }
    TypeEntry e;
    e.scope          = scope;
    e.rawType        = rawType;
    e.sem            = sem;
    e.simplifiedType = simplifiedType;
    entriesSet.insert(e);
}

// ///////////////////////////////////////////////////////////////////
// Utility methods
// ///////////////////////////////////////////////////////////////////

void _checkError(const bool error, TypedObject *o, ILanguageSemantics *sem)
{
    if (o->getSemanticType() != nullptr)
        return;
    if (!error)
        return;

    messageError("Cannot type object.", o, sem);
}

} // namespace
// /////////////////////////////////////////////////////////////////////
// Type visitor.
//
// Support class to calculate the type of a Value
// (and of the subtree having the passed node as root).
//
// /////////////////////////////////////////////////////////////////////
// Constructor
TypeVisitor::TypeVisitor(ILanguageSemantics *ref, const bool error)
    : GuideVisitor(0)
    , _sem(ref)
    , _error(error)
    , _factory(_sem)
{
    hif::application_utils::initializeLogHeader("HIF", "TypeVisitor");
}
TypeVisitor::~TypeVisitor() { hif::application_utils::restoreLogHeader(); }

bool TypeVisitor::_getTypeForConstant(ConstValue *o)
{
    if (o->getType() != nullptr) {
        hif::semantics::updateDeclarations(o->getType(), _sem);
        o->setSemanticType(hif::copy(o->getType()));
        return false;
    }

    Type *t = _sem->getTypeForConstant(o);
    if (t != nullptr) {
        o->setSemanticType(t);
        return true;
    }

    t = getOtherOperandType(o, _sem);
    o->setSemanticType(hif::copy(t));
    return false;
}

void TypeVisitor::_updateCachedDeclarations(TypedObject *obj, const bool onlySignature)
{
    // See ticket #1311.
    Type *t = obj->getSemanticType();
    if (t == nullptr)
        return;

    Declaration *decl = hif::semantics::getDeclaration(obj, _sem);
    // if (decl == nullptr) return;

    // Get view in cache, used as root:
    Object *o = obj;
    if (dynamic_cast<ReferencedAssign *>(obj) != nullptr) {
        o = obj->getParent();
    }

    Object *inst          = hif::objectGetInstance(o);
    ViewReference *instVr = dynamic_cast<ViewReference *>(inst);
    if (instVr == nullptr) {
        Value *v = dynamic_cast<Value *>(inst);
        if (v != nullptr) {
            Type *vt = getSemanticType(v, _sem);
            instVr   = dynamic_cast<ViewReference *>(vt);
        }
    }

    hif::manipulation::InstantiateOptions instOpt;
    instOpt.onlySignature = onlySignature;
    View *instView        = hif::manipulation::instantiate(instVr, _sem, instOpt);

    bool isPrefixVr = false;
    if (instVr != nullptr) {
        const bool isSymb  = (dynamic_cast<hif::features::ISymbol *>(t) != nullptr);
        Declaration *tdecl = nullptr;
        if (isSymb) {
            tdecl = getDeclaration(t, _sem);
        }

        isPrefixVr = (hif::isSubNode(tdecl, instView));
    }

    if (dynamic_cast<ViewReference *>(t) != nullptr) {
        // When the semantic type is a vr,
        // then we must ensure the declaration is reset.
        // Ref design: vhdl/gaysler/can_oc
        ViewReference *vr = static_cast<ViewReference *>(t);
        hif::semantics::ResetDeclarationsOptions ropt;
        ropt.onlyCurrent = true;
        ropt.onlyVisible = true;
        hif::semantics::resetDeclarations(vr, ropt);
        hif::semantics::resetDeclarations(vr->templateParameterAssigns, ropt);
    }

    // Updating decls:
    hif::semantics::UpdateDeclarationOptions dopt;
    dopt.onlyVisible = true;
    dopt.location    = decl;
    dopt.root        = instView;
    hif::semantics::updateDeclarations(t, _sem, dopt);

    // *SEEMS* that setting the prefix before updateDeclaration(onlyVisible)
    // does not allow to fix internal symbol declarations.
    // Conversely, the prefix vr must be set to allow correct prefixing,
    // and it is already fine, since it is a semantic type (and thus, already
    // fixed recusively).
    // Ref design: vhdl/openCores/uart, w/ and w/o a2t
    if (isPrefixVr) {
        ViewReference *typePrefix = hif::copy(instVr);
        const bool instanceSet    = hif::objectSetInstance(t, typePrefix);
        if (!instanceSet)
            delete typePrefix;
    }
}

int TypeVisitor::visitAggregate(Aggregate &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;

    GuideVisitor::visitAggregate(o);

    // 1- Check if result must be constexpr
    const bool isConstExpr = _aggregateIsConstExpr(&o);

    // 2- Calculate the aggregate internal type
    Type *internal = _aggregateGetInternalType(&o);
    if (internal == nullptr) {
        _checkError(_error, &o, _sem);
        return 0; // type error
    }

    // 3- Tryes to auto-determinate the type
    Value *sizeExpr              = nullptr;
    std::int64_t size               = 0;
    std::int64_t min                = -1;
    const bool isAutoDeterminate = _aggregateIsAutoDeterminate(&o, sizeExpr, size, min);
    messageDebugAssert(
        o.getOthers() != nullptr || isAutoDeterminate, "Unexpected aggregate without others non-autodeterminate", &o,
        _sem);

    // 4- calculate the result direction
    RangeDirection dir = getOtherOperandDirection(&o, _sem);

    // 5- calculate the result type
    if (isAutoDeterminate) {
        // calculate the size
        if (!_aggregateCalulateSize(sizeExpr, size, min)) {
            delete internal;
            _checkError(_error, &o, _sem);
            return 0; // type error
        }

        // calculate result range
        Range *arrayRange = nullptr;
        if (dir == dir_upto) {
            arrayRange = new Range(new IntValue(min), sizeExpr, dir_upto);
        } else //if (dir == dir_downto)
        {
            arrayRange = new Range(sizeExpr, new IntValue(min), dir_downto);
        }

        // build result array
        Array *retType = new Array();
        retType->setSpan(arrayRange);
        retType->setType(internal);
        o.setSemanticType(retType);
    } else // cannot auto determiante
    {
        Type *otherType = getOtherOperandType(&o, _sem);
        if (otherType == nullptr) {
            delete internal;
            _checkError(_error, &o, _sem);
            return 0; // type error
        }

        hif::semantics::updateDeclarations(otherType, _sem);
        Type *otherBaseType = getBaseType(otherType, false, _sem, false);
        if (otherBaseType == nullptr) {
            delete internal;
            _checkError(_error, &o, _sem);
            return 0; // type error
        }

        Record *rec  = dynamic_cast<Record *>(otherBaseType);
        Array *array = dynamic_cast<Array *>(otherBaseType);
        Range *span  = hif::typeGetSpan(otherBaseType, _sem);

        Type *semType = nullptr;
        if (rec != nullptr) {
            // tree may be unstable during some front-end phases
            // => return typeref to record
            delete internal;
            semType = hif::copy(otherType);
        } else if (array != nullptr) {
            // set the sub type of other operand array with calculated inner type.
            Array *tmp = hif::copy(array);
            semType    = tmp;
            if (tmp->getSpan()->getDirection() != dir)
                tmp->getSpan()->swapBounds();
            delete tmp->setType(internal);
        } else if (
            hif::semantics::isVectorType(otherBaseType, _sem) || dynamic_cast<String *>(otherBaseType) != nullptr) {
            // could be bitvector, signed, unsigned, string.
            delete internal;
            semType = hif::copy(otherBaseType);
        } else if (span != nullptr) {
            delete internal;
            const bool isSigned   = hif::typeIsSigned(otherBaseType, _sem);
            Type *othersBaseType  = hif::semantics::getBaseType(o.getOthers(), false, _sem);
            const bool isResolved = hif::typeIsResolved(othersBaseType, _sem);
            const bool isLogic    = hif::typeIsLogic(othersBaseType, _sem);
            Bitvector *bv         = _factory.bitvector(hif::copy(span), isLogic, isResolved, false, isSigned);
            semType               = bv;
        } else {
            messageError("Unexpected case", otherBaseType, _sem);
        }

        // try to restore type reference
        TypeReference *tr = dynamic_cast<TypeReference *>(otherType);
        Type *otherBB     = getBaseType(otherBaseType, false, _sem, true);
        Type *semBB       = getBaseType(semType, false, _sem, true);

        hif::EqualsOptions eopt;
        eopt.checkStringSpan = false;
        if (tr != nullptr && hif::equals(otherBB, semBB, eopt)) {
            delete semType;
            semType = hif::copy(tr);
        } else if (array != nullptr) {
            Array *aa = static_cast<Array *>(semType);
            otherBB   = getBaseType(array->getType(), false, _sem, true);
            semBB     = getBaseType(aa->getType(), false, _sem, true);
            tr        = dynamic_cast<TypeReference *>(array->getType());
            if (tr != nullptr && hif::equals(otherBB, semBB, eopt)) {
                delete aa->setType(hif::copy(tr));
            }
        }

        hif::typeSetConstexpr(semType, isConstExpr);
        o.setSemanticType(semType);
    }

    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);
    return 0;
}

void TypeVisitor::_simplify(Type *o, const bool simplified)
{
    if (o == nullptr)
        return;

    hif::manipulation::SimplifyOptions opt;
    opt.context                      = o;
    opt.simplify_constants           = true;
    opt.simplify_template_parameters = true;
    opt.simplify_parameters          = true;
    //opt.simplify_semantics_types = true;

    Scope *s = getNearestParent<Scope>(o);
    if (s == nullptr) {
        if (!simplified)
            hif::manipulation::simplify(o, _sem, opt);
        return;
    }

    Type *ret = searchTypeCacheEntry(s, o, _sem);
    if (ret == nullptr) {
        Type *rawType = hif::copy(o);

        hif::manipulation::PrefixTreeOptions ptopt;
        ptopt.recursive    = true;
        ptopt.strictChecks = false;
        hif::manipulation::prefixTree(o, _sem, ptopt);

        // Warning: this could be unsafe (see ticket #622).
        // Resetting of only visible symbols declaration is unsafe and no
        // more required after introduction of _updateCachedDeclarations().
        // Ref. design vhdl/unott/dig_proc
        // Ref. design verilog/openCores/cfg_crc.
        hif::semantics::UpdateDeclarationOptions dopt;
        hif::semantics::updateDeclarations(o, _sem, dopt);

        Type *simplifiedType = nullptr;
        if (!simplified)
            simplifiedType = hif::manipulation::simplify(o, _sem, opt);
        else
            simplifiedType = o;
        messageAssert(simplifiedType != nullptr, "Unexpected simplification", o, _sem);
        addTypeCacheEntry(s, rawType, _sem, hif::copy(simplifiedType));
    } else {
        o->replace(hif::copy(ret));
        delete o;
    }
}

void TypeVisitor::_getTypeOfParameterAssign(
    ParameterAssign *o,
    const bool checkAllCandidates,
    const bool looseTypeChecks)
{
    FunctionCall *fc  = dynamic_cast<FunctionCall *>(o->getParent());
    ProcedureCall *pc = dynamic_cast<ProcedureCall *>(o->getParent());
    messageAssert(fc != nullptr || pc != nullptr, "Unexpected parameter assign parent", o->getParent(), _sem);

    hif::manipulation::InstantiateOptions instOpt;
    instOpt.onlySignature = true;

    SubProgram *f_signature = nullptr;

    if (checkAllCandidates) {
        typedef std::list<Declaration *> Declarations;
        Declarations decls;

        GetCandidatesOptions gcOpt;
        gcOpt.getAllAssignables = true;
        gcOpt.looseTypeChecks   = looseTypeChecks;
        gcOpt.location          = nullptr;
        hif::semantics::getCandidates(decls, o->getParent(), _sem, gcOpt);

        if (decls.size() > 1) {
            Type *t = getOtherOperandType(o->getParent(), _sem);
            if (t == nullptr)
                return;

            EqualsOptions opt;
            opt.checkOnlyTypes = true;

            for (Declarations::iterator it(decls.begin()); it != decls.end();) {
                Function *sp = dynamic_cast<Function *>(*it);
                if (sp == nullptr) {
                    it = decls.erase(it);
                    continue;
                }

                Type *bt = getBaseType(sp->getType(), false, _sem);
                if (!hif::equals(bt, t, opt)) {
                    it = decls.erase(it);
                    continue;
                }

                ++it;
            }

            typedef std::list<SubProgram *> Instances;
            Instances instances;
            for (Declarations::iterator it(decls.begin()); it != decls.end();) {
                // instantiate signature with the proper candidate
                hif::manipulation::InstantiateOptions instOpt2(instOpt);
                instOpt2.candidate = static_cast<SubProgram *>(*it);
                if (fc != nullptr)
                    f_signature = hif::manipulation::instantiate(fc, _sem, instOpt2);
                else
                    f_signature = hif::manipulation::instantiate(pc, _sem, instOpt2);

                if (f_signature == nullptr) {
                    it = decls.erase(it);
                    continue;
                }

                bool found = false;
                for (BList<Parameter>::iterator i = f_signature->parameters.begin(); i != f_signature->parameters.end();
                     ++i) {
                    if ((*i)->getName() != o->getName())
                        continue;
                    found = true;
                    break;
                }

                if (!found) {
                    it = decls.erase(it);
                    continue;
                }

                instances.push_back(f_signature);
                ++it;
            }

            // At this point there is no other way to understand which is the expecte type.
            // Therefore, we just return.
            if (decls.size() != 1)
                return;

            f_signature = instances.front();
            hif::semantics::resetDeclarations(o->getParent());
            hif::semantics::setDeclaration(o->getParent(), decls.front());
        } else {
            // generic procedure
            if (fc != nullptr)
                f_signature = hif::manipulation::instantiate(fc, _sem, instOpt);
            else
                f_signature = hif::manipulation::instantiate(pc, _sem, instOpt);
        }
    } else {
        // first of all get the signature of parent
        if (fc != nullptr)
            f_signature = hif::manipulation::instantiate(fc, _sem, instOpt);
        else
            f_signature = hif::manipulation::instantiate(pc, _sem, instOpt);
    }

    if (f_signature == nullptr) {
        _checkError(_error, o, _sem);
        return; // type error
    }
    // now we have the function signature, therefore we can get the
    // instantiate the return type with actual parameters

    for (BList<Parameter>::iterator i = f_signature->parameters.begin(); i != f_signature->parameters.end(); ++i) {
        if ((*i)->getName() != o->getName())
            continue;
        hif::semantics::updateDeclarations((*i)->getType(), _sem);
        o->setSemanticType(hif::copy((*i)->getType()));
        break;
    }

    _updateCachedDeclarations(o);
    _simplify(o->getSemanticType());
}

void TypeVisitor::_getTypeOfPortAssign(PortAssign *o)
{
    Port *decl = getDeclaration(o, _sem);
    if (decl == nullptr) {
        _checkError(_error, o, _sem);
        return;
    }

    Instance *inst = dynamic_cast<Instance *>(o->getParent());
    if (inst == nullptr) {
        _checkError(_error, o, _sem);
        messageError("Unexpected case", o, _sem);
    }

    ViewReference *vref = dynamic_cast<ViewReference *>(inst->getReferencedType());
    if (vref == nullptr)
        return;

    Type *t = nullptr;
    hif::manipulation::InstantiateOptions instOpt;
    instOpt.onlySignature = true;
    Port *p               = hif::manipulation::instantiate(o, _sem, instOpt);
    if (p == nullptr) {
        // TODO: Check why not consider opacity..
        Type *tb = getBaseType(decl->getType(), true, _sem);
        hif::semantics::updateDeclarations(tb, _sem);
        t = hif::copy(tb);
    } else {
        // TODO: Check why not consider opacity..
        Type *tb = getBaseType(p->getType(), true, _sem);
        hif::semantics::updateDeclarations(tb, _sem);
        t = hif::copy(tb);
    }

    o->setSemanticType(t);
    _updateCachedDeclarations(o, true);
    _simplify(t);
}

void TypeVisitor::_getTypeOfValueTPAssign(ValueTPAssign *o)
{
    FunctionCall *fc  = dynamic_cast<FunctionCall *>(o->getParent());
    ProcedureCall *pc = dynamic_cast<ProcedureCall *>(o->getParent());
    ViewReference *vr = dynamic_cast<ViewReference *>(o->getParent());
    TypeReference *tr = dynamic_cast<TypeReference *>(o->getParent());
    messageDebugAssert(
        fc != nullptr || pc != nullptr || vr != nullptr || tr != nullptr, "Unexpected value TP assign parent",
        o->getParent(), _sem);

    hif::CopyOptions opt;

    hif::manipulation::InstantiateOptions instOpt;
    instOpt.onlySignature = true;

    BList<Declaration> templateParameters;
    if (fc != nullptr) {
        SubProgram *f_signature = hif::manipulation::instantiate(fc, _sem, instOpt);
        if (!f_signature) {
            _checkError(_error, o, _sem);
            return; // error
        }
        hif::copy(f_signature->templateParameters, templateParameters, opt);
    } else if (pc != nullptr) {
        SubProgram *p_signature = hif::manipulation::instantiate(pc, _sem, instOpt);
        if (!p_signature) {
            _checkError(_error, o, _sem);
            return; // error
        }
        hif::copy(p_signature->templateParameters, templateParameters, opt);
    } else if (vr != nullptr) {
        View *instantiateView = hif::manipulation::instantiate(vr, _sem, instOpt);
        if (!instantiateView) {
            _checkError(_error, o, _sem);
            return; // error
        }
        hif::copy(instantiateView->templateParameters, templateParameters, opt);
    } else // if( tr != nullptr )
    {
        TypeDef *instantiateTypeDef = dynamic_cast<TypeDef *>(hif::manipulation::instantiate(tr, _sem, instOpt));
        if (!instantiateTypeDef) {
            _checkError(_error, o, _sem);
            return; // error
        }
        hif::copy(instantiateTypeDef->templateParameters, templateParameters, opt);
    }

    // now we have the instantiated object, therefore we can get the
    // instantiate the return type with actual parameters
    for (BList<Declaration>::iterator i = templateParameters.begin(); i != templateParameters.end(); ++i) {
        if ((*i)->getName() != o->getName())
            continue;
        ValueTP *vtp = dynamic_cast<ValueTP *>(*i);
        messageAssert(vtp != nullptr, "Unexpected type template parameter", *i, _sem);
        hif::semantics::updateDeclarations(vtp->getType(), _sem);
        o->setSemanticType(hif::copy(vtp->getType()));
    }

    _updateCachedDeclarations(o);
    _simplify(o->getSemanticType());
}

bool TypeVisitor::_aggregateIsConstExpr(Aggregate *o)
{
    bool isConstExpr = true;
    if (o->getOthers() != nullptr) {
        isConstExpr = typeIsConstexpr(o->getOthers()->getSemanticType(), _sem);
    }

    for (BList<AggregateAlt>::iterator i = o->alts.begin(); i != o->alts.end() && isConstExpr; ++i) {
        AggregateAlt *a = *i;
        Type *at        = a->getValue()->getSemanticType();
        isConstExpr     = typeIsConstexpr(at, _sem);
    }

    return isConstExpr;
}

Type *TypeVisitor::_aggregateGetInternalType(Aggregate *o)
{
    // check the types inside the alts
    // here we use the equality trick (as the assignment in
    // standard visitor)
    // all the alts must be equality comparable
    // the return type is the precision of the equality applied
    // to all the alts
    Value *ref    = o->getOthers();
    Type *refType = (ref != nullptr) ? ref->getSemanticType() : nullptr;
    refType       = hif::copy(refType);

    for (BList<AggregateAlt>::iterator i = o->alts.begin(); i != o->alts.end(); ++i) {
        AggregateAlt *current = *i;
        Value *currentValue   = current->getValue();
        if (currentValue == nullptr) {
            delete refType;
            return nullptr;
        }
        Type *currentType = currentValue->getSemanticType();
        if (currentType == nullptr) {
            delete refType;
            return nullptr;
        }

        if (refType == nullptr) {
            refType = hif::copy(currentType);
            continue;
        }

        ILanguageSemantics::ExpressionTypeInfo info = _sem->getExprType(refType, currentType, op_eq, current);
        delete refType;
        if (info.operationPrecision == nullptr)
            return nullptr;

        refType                 = info.operationPrecision;
        info.operationPrecision = nullptr;
    }

    return refType;
}

bool TypeVisitor::_getBoundMin(Value *v, std::int64_t &min)
{
    IntValue *iv   = dynamic_cast<IntValue *>(hif::getChildSkippingCasts(v));
    ConstValue *cv = dynamic_cast<ConstValue *>(hif::getChildSkippingCasts(v));
    if (iv != nullptr) {
        min = iv->getValue();
        return true;
    } else if (cv != nullptr) {
        Int *itype    = _factory.integer();
        IntValue *ivv = dynamic_cast<IntValue *>(hif::manipulation::transformConstant(cv, itype, _sem));
        if (ivv == nullptr) {
            delete itype;
            return false;
        }
        min = ivv->getValue();
        return true;
    }

    return false;
}

bool TypeVisitor::_aggregateIsAutoDeterminate(Aggregate *o, Value *&sizeExpr, std::int64_t &size, std::int64_t &min)
{
    if (o->getOthers() != nullptr)
        return false;

    bool isAutoDeterminable = true;
    for (BList<AggregateAlt>::iterator it = o->alts.begin(); it != o->alts.end() && isAutoDeterminable; ++it) {
        AggregateAlt *ag = *it;
        for (BList<Value>::iterator j = ag->indices.begin(); j != ag->indices.end(); ++j) {
            Value *ind        = *j;
            Value *simplified = hif::manipulation::getAggressiveSimplified(ind, _sem);
            Range *r          = dynamic_cast<Range *>(simplified);

            if (r == nullptr) {
                std::int64_t tmp    = 0;
                const bool found = _getBoundMin(simplified, tmp);
                delete simplified;
                if (!found) {
                    isAutoDeterminable = false;
                    delete sizeExpr;
                    sizeExpr = nullptr;
                    break;
                }
                ++size;
                if (min == -1 || min > tmp)
                    min = tmp;
            } else {
                std::int64_t tmp    = 0;
                const bool found = _getBoundMin(hif::rangeGetMinBound(r), tmp);
                if (!found) {
                    delete r;
                    isAutoDeterminable = false;
                    delete sizeExpr;
                    sizeExpr = nullptr;
                    break;
                }
                if (min == -1 || min > tmp)
                    min = tmp;

                hif::semantics::updateDeclarations(r, _sem);
                std::uint64_t rangeSize = hif::semantics::spanGetBitwidth(r, _sem);
                if (rangeSize != 0) {
                    delete r;
                    size += static_cast<std::int64_t>(rangeSize);
                    continue;
                }

                Value *v = hif::semantics::spanGetSize(r, _sem);
                if (sizeExpr == nullptr) {
                    sizeExpr = v;
                } else {
                    sizeExpr = new Expression(op_plus, sizeExpr, v);
                }
                delete r;
            }
        }
    }

    return isAutoDeterminable;
}

bool TypeVisitor::_aggregateCalulateSize(Value *&sizeExpr, std::int64_t size, std::int64_t min)
{
    // calculate the size
    if (sizeExpr == nullptr) {
        if (size == 0)
            return false;
        sizeExpr = new IntValue(size - 1 + min);
    } else if (size != 0) {
        IntValue *iv = new IntValue(size - 1 + min);
        hif::manipulation::assureSyntacticType(iv, _sem);
        sizeExpr = new Expression(op_plus, sizeExpr, iv);
    } else {
        IntValue *iv = new IntValue(min);
        hif::manipulation::assureSyntacticType(iv, _sem);
        sizeExpr = new Expression(op_plus, sizeExpr, iv);

        IntValue *iv1 = new IntValue(1);
        hif::manipulation::assureSyntacticType(iv1, _sem);
        sizeExpr = new Expression(op_minus, sizeExpr, iv1);
    }

    return true;
}

int TypeVisitor::visitBitValue(BitValue &o)
{
    // Set the type for this constant according with
    // reference semantics
    if (o.getSemanticType() != nullptr)
        return 0;
    GuideVisitor::visitBitValue(o);
    const bool simplified = _getTypeForConstant(&o);
    _simplify(o.getSemanticType(), simplified);
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitBitvectorValue(BitvectorValue &o)
{
    // Set the type for this constant according with
    // reference semantics
    if (o.getSemanticType() != nullptr)
        return 0;
    GuideVisitor::visitBitvectorValue(o);
    const bool simplified = _getTypeForConstant(&o);
    _simplify(o.getSemanticType(), simplified);
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitBoolValue(BoolValue &o)
{
    // Set the type for this constant according with
    // reference semantics
    if (o.getSemanticType() != nullptr)
        return 0;
    GuideVisitor::visitBoolValue(o);
    const bool simplified = _getTypeForConstant(&o);
    _simplify(o.getSemanticType(), simplified);
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitCast(Cast &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;
    GuideVisitor::visitCast(o);

    // The type of the CAST is set to the type of the operator have
    // to be cast into.
    if (o.getType() != nullptr) {
        hif::semantics::updateDeclarations(o.getType(), _sem);
        o.setSemanticType(hif::copy(o.getType()));
    }

    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitCharValue(CharValue &o)
{
    // Set the type for this constant according with
    // reference semantics
    if (o.getSemanticType() != nullptr)
        return 0;
    GuideVisitor::visitCharValue(o);
    const bool simplified = _getTypeForConstant(&o);
    _simplify(o.getSemanticType(), simplified);
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitExpression(Expression &o)
{
    // Visit the expression only if the type is not set
    if (o.getSemanticType() != nullptr)
        return 0;

    GuideVisitor::visitExpression(o);

    if (o.getValue1() == nullptr || o.getValue1()->getSemanticType() == nullptr) {
        _checkError(_error, &o, _sem);
        return 0; // type error
    }

    // Establish expression type according with reference semantics.
    // Insert operator 1
    if (o.getValue2() != nullptr && o.getValue2()->getSemanticType() == nullptr) {
        _checkError(_error, &o, _sem);
        return 0; // type error
    }

    Type *top2 = o.getValue2() != nullptr ? o.getValue2()->getSemanticType() : nullptr;

    // Set expression type to the source semantics type to allow recursive
    // type calculation with nested expressions
    ILanguageSemantics::ExpressionTypeInfo exprType =
        _sem->getExprType(o.getValue1()->getSemanticType(), top2, o.getOperator(), &o);
    if (exprType.returnedType == nullptr) {
        o.setSemanticType(nullptr);

        _checkError(_error, &o, _sem);
        return 0; // type error
    }

    o.setSemanticType(exprType.returnedType);
    exprType.returnedType = nullptr;

    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitFieldReference(FieldReference &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;

    GuideVisitor::visitFieldReference(o);

    FieldReference::DeclarationType *frDecl = hif::semantics::getDeclaration(&o, _sem);

    if (frDecl == nullptr) {
        _checkError(_error, &o, _sem);
        return 0; // type error
    }

    Type *prefixType = getSemanticType(o.getPrefix(), _sem);
    if (prefixType == nullptr) {
        _checkError(_error, &o, _sem);
        return 0; // type error
    }

    // Do not recourse! Ref. design: vhdl/ips/ppm_interpreter_logic
    Type *prefixBaseType = getBaseType(prefixType, false, _sem, false);
    messageAssert(prefixBaseType != nullptr, "Unexpected case", prefixType, _sem);

    // Resolve eventual Pointer/Reference.
    prefixBaseType = typeGetNestedType(prefixBaseType, _sem);
    // Ref design: OSTC step4 a2t CPP on composed.
    prefixBaseType = getBaseType(prefixBaseType, false, _sem, false);
    messageAssert(prefixBaseType != nullptr, "Unexpected case (2)", prefixType, _sem);

    // Record          --> o is a Field.
    // ViewRef/Library --> o is a Variable/Constant
    // other           --> ???
    if (dynamic_cast<Record *>(prefixBaseType) != nullptr) {
        Record *rec = static_cast<Record *>(prefixBaseType);
        Field *f    = nullptr;
        for (BList<Field>::iterator i = rec->fields.begin(); i != rec->fields.end(); ++i) {
            if ((*i)->getName() == o.getName()) {
                f = *i;
                break;
            }
        }
        if (f == nullptr) {
            _checkError(_error, &o, _sem);
            return 0; // type error
        }

        hif::CopyOptions opt;
        //opt.copyDeclarations = false;
        hif::semantics::updateDeclarations(f->getType(), _sem);
        Type *t = hif::copy(f->getType(), opt);
        o.setSemanticType(t);
    } else if (dynamic_cast<ViewReference *>(prefixBaseType) != nullptr) {
        ViewReference *vr = static_cast<ViewReference *>(prefixBaseType);

        ViewReference::DeclarationType *origView = hif::semantics::getDeclaration(vr, _sem);
        messageAssert(origView != nullptr, "Declaration not found", vr, _sem);

        ViewReference::DeclarationType *instantiatedDecl = hif::manipulation::instantiate(vr, _sem);
        if (instantiatedDecl == nullptr) {
            messageDebug("Type is", vr, _sem);
            messageError("Instantiate failed", &o, _sem);
        }

        const bool canReplace = (origView->getParent() != nullptr);
        if (canReplace)
            origView->replace(instantiatedDecl);

        Identifier id;
        id.setName(o.getName());
        hif::semantics::DeclarationOptions dopt;
        dopt.location                     = instantiatedDecl->getContents();
        Identifier::DeclarationType *decl = hif::semantics::getDeclaration(&id, _sem, dopt);
        if (decl == nullptr) {
            if (canReplace)
                instantiatedDecl->replace(origView);
            _checkError(_error, &o, _sem);
            return 0; // type error
        }

        hif::semantics::updateDeclarations(decl->getType(), _sem);
        Type *t = hif::copy(decl->getType());
        o.setSemanticType(t);

        //        hif::manipulation::PrefixTreeOptions ptopt;
        //        ptopt.recursive = true;
        //        ptopt.strictChecks = false;
        //        hif::manipulation::prefixTree(o.getSemanticType(), _sem, ptopt);

        if (canReplace)
            instantiatedDecl->replace(origView);

        // Needed, since the original view cannot be set back in the tree before
        // prefixTree() to make all scopes visible,
        // but hence the prefix symbols cannot be reset inside simplify().
        // ref design: ostc step4, vhdl/custom/polar2rect
        _updateCachedDeclarations(&o);
        _simplify(o.getSemanticType());
        //if (canReplace) instantiatedDecl->replace(origView);
        _checkError(_error, &o, _sem);
        return 0;
    } else if (dynamic_cast<Library *>(prefixBaseType) != nullptr) {
        Library *lib                 = static_cast<Library *>(prefixBaseType);
        Library::DeclarationType *ld = hif::semantics::getDeclaration(lib, _sem);

        Identifier id;
        id.setName(o.getName());
        hif::semantics::DeclarationOptions dopt;
        dopt.location                     = ld;
        Identifier::DeclarationType *decl = hif::semantics::getDeclaration(&id, _sem, dopt);
        if (decl == nullptr) {
            _checkError(_error, &o, _sem);
            return 0; // type error
        }

        hif::semantics::updateDeclarations(decl->getType(), _sem);
        Type *t = hif::copy(decl->getType());
        o.setSemanticType(t);
    } else {
        messageError("Unexpected declaration type of a fieldref.", &o, _sem);
    }

    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);

    return 0;
}
int TypeVisitor::visitFunctionCall(FunctionCall &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;

    GuideVisitor::visitFunctionCall(o);

    // first of all get the signature
    hif::manipulation::InstantiateOptions instOpt;
    instOpt.onlySignature = true;
    Function *signature   = hif::manipulation::instantiate(&o, _sem, instOpt);
    messageAssert(
        signature != nullptr && signature->getType() != nullptr, "Cannot get function call signature.", &o,
        _sem); // TODO: may be wrong

    // return type
    hif::semantics::updateDeclarations(signature->getType(), _sem);
    Type *ret = hif::copy(signature->getType());
    o.setSemanticType(ret);

    _updateCachedDeclarations(&o);
    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitInstance(Instance &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;

    GuideVisitor::visitInstance(o);

    messageAssert(o.getReferencedType() != nullptr, "Unxpected instance", &o, _sem);
    hif::semantics::updateDeclarations(o.getReferencedType(), _sem);
    o.setSemanticType(hif::copy(o.getReferencedType()));

    // If the instance is a view reference without explicit template arguments,
    // explicitly add template arguments
    if (dynamic_cast<ViewReference *>(o.getSemanticType()) != nullptr) {
        ViewReference *vr                    = static_cast<ViewReference *>(o.getSemanticType());
        ViewReference::DeclarationType *decl = getDeclaration(vr, _sem);
        if (decl == nullptr) {
            _checkError(_error, &o, _sem);
            return 0;
        }
        hif::manipulation::sortParameters(
            vr->templateParameterAssigns, decl->templateParameters, true, hif::manipulation::SortMissingKind::ALL,
            _sem);
        for (BList<TPAssign>::iterator iter = vr->templateParameterAssigns.begin();
             iter != vr->templateParameterAssigns.end(); ++iter) {
            ValueTPAssign *arg = dynamic_cast<ValueTPAssign *>(*iter);
            if (arg == nullptr)
                continue;
            Type *t = getSemanticType(arg, _sem);
            Cast *c = new Cast();
            c->setType(hif::copy(t));
            c->setValue(arg->setValue(nullptr));
            arg->setValue(c);
        }
    }

    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitIntValue(IntValue &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;

    // Set the type for this constant according with
    // reference semantics
    GuideVisitor::visitIntValue(o);
    const bool simplified = _getTypeForConstant(&o);
    _simplify(o.getSemanticType(), simplified);
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitMember(Member &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;

    GuideVisitor::visitMember(o);

    if (o.getPrefix() == nullptr || o.getPrefix()->getSemanticType() == nullptr) {
        o.setSemanticType(nullptr);
        _checkError(_error, &o, _sem);
        return 0;
    }

    Type *res = _sem->getMemberSemanticType(&o);
    if (res == nullptr) {
        _checkError(_error, &o, _sem);
        return 0; // type error
    }

    o.setSemanticType(res);

    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitIdentifier(Identifier &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;

    DataDeclaration *d = getDeclaration(&o, _sem);

    if (d != nullptr) {
        Type *declType = d->getType();
        Type *t1       = getBaseType(declType, true, _sem);
        hif::semantics::updateDeclarations(t1, _sem);
        Type *t2 = hif::copy(t1);
        o.setSemanticType(t2);
    }

    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitParameterAssign(ParameterAssign &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;

    GuideVisitor::visitParameterAssign(o);
    _getTypeOfParameterAssign(&o);

    _checkError(_error, &o, _sem);
    return 0;
}

int TypeVisitor::visitPortAssign(PortAssign &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;

    GuideVisitor::visitPortAssign(o);
    _getTypeOfPortAssign(&o);

    _checkError(_error, &o, _sem);
    return 0;
}

int TypeVisitor::visitRealValue(RealValue &o)
{
    // Set the type for this constant according with
    // reference semantics
    if (o.getSemanticType() != nullptr)
        return 0;
    GuideVisitor::visitRealValue(o);
    const bool simplified = _getTypeForConstant(&o);
    _simplify(o.getSemanticType(), simplified);
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitRecordValue(RecordValue &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;

    GuideVisitor::visitRecordValue(o);

    bool isConstexpr  = true;
    Record *recordObj = new Record();
    for (BList<RecordValueAlt>::iterator it = o.alts.begin(); it != o.alts.end(); ++it) {
        Type *valType = (*it)->getValue()->getSemanticType();
        if (valType == nullptr) {
            delete recordObj;
            _checkError(_error, &o, _sem);
            return 0;
        }
        isConstexpr |= typeIsConstexpr(valType, _sem);
        Field *fieldObj = new Field();
        fieldObj->setName((*it)->getName());
        fieldObj->setType(hif::copy(valType));
        recordObj->fields.push_back(fieldObj);
    }

    Type *candidate     = getOtherOperandType(&o, _sem);
    Type *baseCandidate = hif::semantics::getBaseType(candidate, false, _sem);
    Record *rec         = dynamic_cast<Record *>(baseCandidate);
    if (rec == nullptr) {
        // The type is a record, as plain type.
        o.setSemanticType(recordObj);
        hif::typeSetConstexpr(recordObj, isConstexpr);
        _checkError(_error, &o, _sem);
        return 0;
    }

    // Temporary add a copy of other operand record type
    // in the same scope of the symbol to ensure the same prefix.
    // Ref design: vhdl/gaisler/can_oc
    rec = hif::copy(rec);
    o.setSemanticType(rec);
    hif::manipulation::PrefixTreeOptions prefOpt;
    prefOpt.strictChecks = false;
    hif::manipulation::prefixTree(rec, _sem, prefOpt);
    o.setSemanticType(nullptr);

    // Check if record values are assignable, then return typeref:
    messageDebugIfFails(recordObj->fields.size() == rec->fields.size(), "Record value:", &o, _sem);
    messageDebugIfFails(recordObj->fields.size() == rec->fields.size(), "Rec:", rec, _sem);
    messageAssert(
        (recordObj->fields.size() == rec->fields.size()), "Record types have incompatible size", recordObj, _sem);
    bool isAssignable         = (recordObj->fields.size() == rec->fields.size());
    BList<Field>::iterator it = recordObj->fields.begin();
    BList<Field>::iterator jt = rec->fields.begin();
    for (; it != recordObj->fields.end() && isAssignable; ++it, ++jt) {
        Field *refField = *it;
        Field *f        = *jt;
        ILanguageSemantics::ExpressionTypeInfo info =
            _sem->getExprType(f->getType(), refField->getType(), op_conv, refField);
        isAssignable = (info.returnedType != nullptr);
    }
    delete rec;

    if (isAssignable) {
        o.setSemanticType(hif::copy(candidate)); // record or typeref
        delete recordObj;
    } else {
        o.setSemanticType(recordObj);
        hif::typeSetConstexpr(recordObj, isConstexpr);
    }

    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitSlice(Slice &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;

    GuideVisitor::visitSlice(o);

    // getting range of slice
    Range *sliceRange = o.getSpan();
    if (sliceRange == nullptr) {
        _checkError(_error, &o, _sem);
        return 0; // type error
    }

    // getting type of prefix that will be type of result array
    Type *prefixType = o.getPrefix()->getSemanticType();
    if (prefixType == nullptr) {
        _checkError(_error, &o, _sem);
        return 0; // type error
    }

    Type *resultType = _sem->getSliceSemanticType(&o);
    if (resultType == nullptr) {
        _checkError(_error, &o, _sem);
        return 0; // type error
    }

    if (_sem->isSliceTypeRebased() && !hif::manipulation::rebaseTypeSpan(resultType, _sem, true)) {
        delete resultType;
        _checkError(_error, &o, _sem);
        return 0; // type error
    }

    o.setSemanticType(resultType);

    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitStringValue(StringValue &o)
{
    // Set the type for this constant according with
    // reference semantics
    if (o.getSemanticType() != nullptr)
        return 0;
    GuideVisitor::visitStringValue(o);
    const bool simplified = _getTypeForConstant(&o);
    _simplify(o.getSemanticType(), simplified);
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitTimeValue(TimeValue &o)
{
    // Set the type for this constant according with
    // reference semantics
    if (o.getSemanticType() != nullptr)
        return 0;
    GuideVisitor::visitTimeValue(o);
    const bool simplified = _getTypeForConstant(&o);
    _simplify(o.getSemanticType(), simplified);
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;
    GuideVisitor::visitTypeTPAssign(o);

    TypeTP *decl = getDeclaration(&o, _sem);
    if (decl == nullptr) {
        _checkError(_error, &o, _sem);
        return 0; // type error
    }

    TypeReference *tr = new TypeReference();
    tr->setName(decl->getName());
    hif::semantics::setDeclaration(tr, decl);
    o.setSemanticType(tr);

    _updateCachedDeclarations(&o);
    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;

    GuideVisitor::visitValueTPAssign(o);
    _getTypeOfValueTPAssign(&o);

    _checkError(_error, &o, _sem);
    return 0;
}
int TypeVisitor::visitWhen(When &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;
    GuideVisitor::visitWhen(o);

    ILanguageSemantics::ExpressionTypeInfo ret;
    if (o.getDefault() != nullptr) {
        ret.operationPrecision = hif::copy(o.getDefault()->getSemanticType());
        if (ret.operationPrecision == nullptr) {
            _checkError(_error, &o, _sem);
            return 0; // error
        }
    }

    for (BList<WhenAlt>::iterator i = o.alts.begin(); i != o.alts.end(); ++i) {
        WhenAlt *wa = *i;
        messageAssert(wa->getValue() != nullptr, "Expected when alt value", wa, _sem);

        Type *current = wa->getValue()->getSemanticType();
        if (current == nullptr) {
            _checkError(_error, &o, _sem);
            return 0; // error
        }

        if (ret.operationPrecision == nullptr) {
            // means: no default value!
            ret.operationPrecision = hif::copy(current);
            continue;
        }

        ILanguageSemantics::ExpressionTypeInfo info = _sem->getExprType(ret.operationPrecision, current, op_eq, wa);
        if (info.operationPrecision == nullptr) {
            _checkError(_error, &o, _sem);
            return 0; // error
        }

        delete ret.operationPrecision;
        ret.operationPrecision  = info.operationPrecision;
        info.operationPrecision = nullptr;
    }

    o.setSemanticType(ret.operationPrecision);
    ret.operationPrecision = nullptr;

    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);

    return 0;
}
int TypeVisitor::visitWith(With &o)
{
    if (o.getSemanticType() != nullptr)
        return 0;
    GuideVisitor::visitWith(o);

    // check the types inside the alts
    // here we use the equality trick (as the assignment in
    // standard visitor)
    // all the alts must be equality comparable
    // the return type is the precision of the equality applied
    // to all the alts
    ILanguageSemantics::ExpressionTypeInfo info;
    BList<WithAlt>::iterator it = o.alts.begin();
    Value *op1                  = (o.getDefault()) ? o.getDefault() : (*it++)->getValue();
    Value *op2                  = nullptr;
    Type *t1                    = op1->getSemanticType();
    Type *t2                    = nullptr;
    while (it != o.alts.end()) {
        op2 = (*it)->getValue();
        t2  = op2->getSemanticType();

        // check types
        if (t1 == nullptr || t2 == nullptr) {
            _checkError(_error, &o, _sem);
            return 0; // error
        }

        info = _sem->getExprType(t1, t2, op_eq, *it);

        if (info.operationPrecision == nullptr) {
            _checkError(_error, &o, _sem);
            return 0; // error
        }

        t1 = info.operationPrecision;
        ++it;
    }
    info.operationPrecision = nullptr;
    o.setSemanticType(hif::copy(t1));

    _simplify(o.getSemanticType());
    _checkError(_error, &o, _sem);
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////
// Public methods.
////////////////////////////////////////////////////////////////////////////////////

Type *getSemanticType(TypedObject *v, ILanguageSemantics *ref_sem, const bool error)
{
    messageAssert(v != nullptr, "getSemanticType() called with nullptr argument", nullptr, nullptr);

    if (v->getSemanticType() == nullptr) {
        TypeVisitor tv(ref_sem, error);
        v->acceptVisitor(tv);
    }

    return v->getSemanticType();
}

void typeTree(Object *root, ILanguageSemantics *ref_sem, const bool error)
{
    messageDebugAssert(root != nullptr, "Passed null root", nullptr, ref_sem);
    if (root == nullptr)
        return;

    TypeVisitor tv(ref_sem, error);
    root->acceptVisitor(tv);
}

void typeTree(BList<Object> &root, ILanguageSemantics *ref_sem, const bool error)
{
    for (BList<Object>::iterator i = root.begin(); i != root.end(); ++i) {
        typeTree(*i, ref_sem, error);
    }
}
template <typename T> void typeTree(BList<T> &root, ILanguageSemantics *ref_sem, const bool error)
{
    typeTree(root.template toOtherBList<Object>(), ref_sem, error);
}
void flushTypeCacheEntries()
{
    entriesSet.clear();
    trash.clear();
    generalTrash.clear();
}

bool isInTypeCache(Object *obj)
{
    messageAssert((obj != nullptr), "Given nullptr as starting object", nullptr, nullptr);

    Object *parent = obj;
    Object *tmp    = obj->getParent();
    while (tmp != nullptr) {
        parent = tmp;
        tmp    = tmp->getParent();
    }

    if (dynamic_cast<Type *>(tmp) == nullptr)
        return false;
    if (dynamic_cast<System *>(parent) != nullptr)
        return false; // is in tree

    for (EntriesSet::iterator i = entriesSet.begin(); i != entriesSet.end(); ++i) {
        if (i->simplifiedType == tmp)
            return true;
    }

    for (EntriesSet::iterator i = trash.begin(); i != trash.end(); ++i) {
        if (i->simplifiedType == tmp)
            return true;
    }

    return false;
}

void addInTypeCache(Object *obj) { generalTrash.insert(obj); }
Type *
getPrefixedType(Type *t, ILanguageSemantics *sem, const hif::manipulation::PrefixTreeOptions &opt, Object *context)
{
    if (t == nullptr)
        return nullptr;

    Object *location = nullptr;
    if (context != t)
        location = context;

    const bool simplifyByContext = hif::isInTree(context);

    Type *copy            = hif::copy(t);
    const bool canReplace = t->getParent() != nullptr;
    if (canReplace)
        t->replace(copy);
    hif::semantics::UpdateDeclarationOptions dopt;
    dopt.location        = location;
    dopt.looseTypeChecks = true;
    //dopt.error = simplifyByContext;
    hif::semantics::updateDeclarations(copy, sem, dopt);
    if (canReplace)
        copy->replace(t);

    hif::manipulation::prefixTree(copy, sem, opt);

    // TODO: check may be too strict.
    // Idea: find all symbols inside type and check whether their declarations
    // are subnodes of top-most parent (which is not System forcefully).
    // If true, allow simpify() by context.
    if (simplifyByContext) {
        // Ref designs:
        // - vhdl/openCores/color_converter_origianl + a2tool_lt + hif2vp_ovptlm
        // - vhdl/openCores/spiMaster
        hif::manipulation::SimplifyOptions sopt;
        sopt.context                      = context;
        sopt.simplify_constants           = true;
        sopt.simplify_template_parameters = true;
        sopt.simplify_parameters          = true;
        hif::manipulation::simplify(copy, sem, sopt);
    }

    return copy;
}

#define HIF_TEMPLATE_METHOD(T) void typeTree<T>(BList<T> & root, ILanguageSemantics *, const bool)

HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD
} // namespace semantics
} // namespace hif
