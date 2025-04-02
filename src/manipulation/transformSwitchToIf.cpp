/// @file transformSwitchToIf.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/transformSwitchToIf.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace
{

// ///////////////////////////////////////////////////////////////////
// condition management
// ///////////////////////////////////////////////////////////////////

template <typename T>
void _makeSupportVariable(T *sw, hif::semantics::ILanguageSemantics *sem, TransformCaseOptions &opt)
{
    // Check if switch value is a FunctionCall. If so, the translation in if-then-else
    // can cause side effects due multiple calling of function.
    // So this visitor create a Variable for each FunctionCall of switch value and
    // replace FunctionCalls with appropriate Identifier switch value.
    // Example:
    // 		switch( foo1() + foo2() ){ case a: [..] case b: [..] }
    // Translation without this visitor is:
    //		if( foo1() + foo2() == a) [..]
    //		else if( foo1() + foo2() == b) [..]
    // Translation with this visitor is:
    //		foo1_switch_var = foo1();
    //		foo2_switch_var = foo2();
    //		if( foo1_switch_var + foo2_switch_var == a) [..]
    //		else if( foo1_switch_var + foo2_switch_var == b) [..]

    hif::HifFactory factory(sem);
    // Refine switch value in case of expressions creating support variable and assignment.
    // This is necessary since complex expression may be repeated many times inside
    // if conditions potentially causing a huge expanding of the HIF tree
    // and a loss of running performance.
    // Ref. design verilog/openCores/des
    Value *cond                  = hif::getChildSkippingCasts(sw->getCondition());
    hif::features::ISymbol *symb = dynamic_cast<hif::features::ISymbol *>(cond);
    if (symb != nullptr) {
        Identifier *id         = dynamic_cast<Identifier *>(cond);
        FieldReference *fr     = dynamic_cast<FieldReference *>(cond);
        Declaration *decl      = hif::semantics::getDeclaration(cond, sem);
        Port *port             = dynamic_cast<Port *>(decl);
        Signal *signal         = dynamic_cast<Signal *>(decl);
        bool isSigOrPort = (port != nullptr || signal != nullptr) && !opt.fixSignalOrPortCondition;
        if ((id != nullptr || fr != nullptr) && !isSigOrPort)
            return;
    }

    Type *switchValueType = hif::semantics::getSemanticType(sw->getCondition(), sem);
    messageAssert(switchValueType != nullptr, "Cannot type switch value", sw->getCondition(), sem);

    // creating and adding support var
    Variable *var = new Variable();
    var->setName(NameTable::getInstance()->getFreshName("support_switch_var"));
    var->setType(hif::copy(switchValueType));
    var->setValue(sem->getTypeDefaultValue(switchValueType, var));

    // fixing created variable if needed
    opt.createdVariable = var;

    StateTable *st = hif::getNearestParent<StateTable>(sw);
    messageAssert(st != nullptr, "Cannot find parent state table", sw, sem);
    st->declarations.push_back(var);

    // creating and adding assignment var = switch value
    Assign *a = factory.assignment(factory.identifier(var->getName()), sw->setCondition(nullptr));

    Action *parentAct = hif::getNearestParent<Action>(sw, true);
    messageAssert(parentAct != nullptr, "Cannot find parent action", sw, sem);
    BList<Action>::iterator it(parentAct);
    it.insert_before(a);

    // fixing created assignement if needed
    opt.createdAssign = a;

    // substite the switch value with var
    sw->setCondition(factory.identifier(var->getName()));
}

// ///////////////////////////////////////////////////////////////////
// Switch to if fixes
// ///////////////////////////////////////////////////////////////////

Value *_makeInternalCond(
    Value *switchValue,
    Value *newCondition,
    Value *currentCondition,
    Type *mappedBool,
    hif::semantics::ILanguageSemantics *sem)
{
    Expression *expr_eq = new Expression();
    expr_eq->setOperator(op_case_eq);
    expr_eq->setValue1(hif::copy(switchValue));
    hif::manipulation::assureSyntacticType(expr_eq->getValue1(), sem);
    expr_eq->setValue2(currentCondition);

    if (newCondition != nullptr) {
        Expression *expr_or = new Expression();
        expr_or->setOperator(op_or);
        expr_or->setValue1(newCondition);
        expr_or->setValue2(expr_eq);

        // Safely add cast to mapped bool to operands
        Cast *boolCast1 = new Cast();
        boolCast1->setValue(expr_or->getValue1());
        expr_or->setValue1(boolCast1);
        boolCast1->setType(hif::copy(mappedBool));

        Cast *boolCast2 = new Cast();
        boolCast2->setValue(expr_or->getValue2());
        expr_or->setValue2(boolCast2);
        boolCast2->setType(hif::copy(mappedBool));

        newCondition = expr_or;
    } else {
        newCondition = expr_eq;
    }

    return newCondition;
}

Value *_makeInternalRangeCond(
    Value *switchValue,
    Value *newCondition,
    Range *range,
    Type *mappedBool,
    hif::semantics::ILanguageSemantics *sem)
{
    Expression *expr_left = new Expression();
    expr_left->setValue1(hif::copy(switchValue));
    hif::manipulation::assureSyntacticType(expr_left->getValue1(), sem);
    expr_left->setValue2(range->setLeftBound(nullptr));
    hif::manipulation::assureSyntacticType(expr_left->getValue2(), sem);

    Expression *expr_right = new Expression();
    expr_right->setValue1(hif::copy(switchValue));
    hif::manipulation::assureSyntacticType(expr_right->getValue1(), sem);
    expr_right->setValue2(range->setRightBound(nullptr));
    hif::manipulation::assureSyntacticType(expr_right->getValue2(), sem);

    Expression *expr_range = new Expression();
    expr_range->setOperator(op_and);
    expr_range->setValue1(expr_left);
    expr_range->setValue2(expr_right);

    // Safely add cast to mapped bool to operands
    {
        Cast *boolCast1 = new Cast();
        boolCast1->setValue(expr_range->getValue1());
        expr_range->setValue1(boolCast1);
        boolCast1->setType(hif::copy(mappedBool));

        Cast *boolCast2 = new Cast();
        boolCast2->setValue(expr_range->getValue2());
        expr_range->setValue2(boolCast2);
        boolCast2->setType(hif::copy(mappedBool));
    }

    if (range->getDirection() == dir_upto) {
        expr_left->setOperator(op_ge);
        expr_right->setOperator(op_le);
    } else if (range->getDirection() == dir_downto) {
        expr_left->setOperator(op_le);
        expr_right->setOperator(op_ge);
    } else {
        messageError("Unexpected range direction", range, nullptr);
    }

    if (newCondition != nullptr) {
        Expression *expr_or = new Expression();
        expr_or->setOperator(op_or);
        expr_or->setValue1(newCondition);
        expr_or->setValue2(expr_range);

        // Safely add cast to mapped bool to operands
        Cast *boolCast1 = new Cast();
        boolCast1->setValue(expr_or->getValue1());
        expr_or->setValue1(boolCast1);
        boolCast1->setType(hif::copy(mappedBool));

        Cast *boolCast2 = new Cast();
        boolCast2->setValue(expr_or->getValue2());
        expr_or->setValue2(boolCast2);
        boolCast2->setType(hif::copy(mappedBool));

        newCondition = expr_or;
    } else {
        newCondition = expr_range;
    }

    return newCondition;
}

Value *_makeCond(Value *switchValue, BList<Value> &list_val, hif::semantics::ILanguageSemantics *sem)
{
    Value *newCondition = nullptr;

    // Assuming this kind of manipulation is made in HIF only.
    messageDebugAssert(
        sem == hif::semantics::HIFSemantics::getInstance(), "At the moment is assumed only HIF semantics", nullptr,
        sem);
    Type *mappedBool = new Bool();

    hif::EqualsOptions opt;
    opt.checkOnlyTypes = true;

    for (BList<Value>::iterator i = list_val.begin(); i != list_val.end();) {
        Value *sv = *i;
        i         = i.remove();
        if (dynamic_cast<Range *>(sv) != nullptr) {
            Range *r     = static_cast<Range *>(sv);
            newCondition = _makeInternalRangeCond(switchValue, newCondition, r, mappedBool, sem);
            delete r;
        } else {
            newCondition = _makeInternalCond(switchValue, newCondition, sv, mappedBool, sem);
        }
    }

    delete mappedBool;

    if (newCondition == nullptr)
        return nullptr;

    // Return value is casted to bool, since it could be a bit if operands
    // are logic.
    Cast *boolCast = new Cast();
    boolCast->setValue(newCondition);
    boolCast->setType(new Bool());

    return boolCast;
}

Type *_checkCaseType(Value *v, hif::semantics::ILanguageSemantics *sem)
{
    Type *t     = hif::semantics::getSemanticType(v, sem);
    t           = hif::semantics::getBaseType(t, false, sem, false);
    Signed *s   = dynamic_cast<Signed *>(t);
    Unsigned *u = dynamic_cast<Unsigned *>(t);

    if (s == nullptr && u == nullptr)
        return nullptr;

    Bitvector *ret = new Bitvector();
    ret->setLogic(true);
    ret->setResolved(true);
    ret->setSigned(s != nullptr);
    ret->setSpan(hif::copy(s != nullptr ? s->getSpan() : u->getSpan()));

    return ret;
}

template <class T> void _mapSignedUnsigned(T *obj, hif::semantics::ILanguageSemantics *sem)
{
    typedef typename T::AltType AltType;
    // Map signed and unsigned to corresponding logic vector since:
    // Overloading the operator “=” has no effect on the semantics of case statement execution.
    Value *condValue = obj->getCondition();
    Type *svt        = _checkCaseType(condValue, sem);
    if (svt != nullptr) {
        Cast *c = new Cast();
        c->setType(svt);
        condValue->replace(c);
        c->setValue(condValue);
    }

    for (typename BList<AltType>::iterator i = obj->alts.begin(); i != obj->alts.end(); ++i) {
        AltType *sa = *i;
        for (BList<Value>::iterator j = sa->conditions.begin(); j != sa->conditions.end(); ++j) {
            Value *cond = *j;

            if (dynamic_cast<Range *>(cond) != nullptr) {
                Range *r      = static_cast<Range *>(cond);
                Type *retLeft = _checkCaseType(r->getLeftBound(), sem);
                if (retLeft != nullptr) {
                    Cast *c = new Cast();
                    c->setType(retLeft);
                    c->setValue(r->setLeftBound(nullptr));
                    r->setLeftBound(c);
                }
                Type *retRight = _checkCaseType(r->getRightBound(), sem);
                if (retRight != nullptr) {
                    Cast *c = new Cast();
                    c->setType(retRight);
                    c->setValue(r->setRightBound(nullptr));
                    r->setRightBound(c);
                }
            } else {
                Type *sact = _checkCaseType(cond, sem);
                if (sact != nullptr) {
                    Cast *c = new Cast();
                    c->setType(sact);
                    cond->replace(c);
                    c->setValue(cond);
                }
            }
        }
    }
}

void _copyPropertiesAndComments(Object *from, Object *to)
{
    for (Object::PropertyMapIterator it = from->getPropertyBeginIterator(); it != from->getPropertyEndIterator();
         ++it) {
        to->addProperty(it->first, hif::copy(it->second));
    }

    if (from->hasComments()) {
        to->addComments(from->getComments());
    }
}

} // namespace

// ///////////////////////////////////////////////////////////////////
// TransformSwitchToIfOptions
// ///////////////////////////////////////////////////////////////////

TransformCaseOptions::TransformCaseOptions()
    : fixCondition(false)
    , fixSignalOrPortCondition(false)
    , splitCases(false)
    , simplify(true)
    , createdVariable(nullptr)
    , createdAssign(nullptr)
{
    // ntd
}

TransformCaseOptions::~TransformCaseOptions()
{
    // ntd
}

TransformCaseOptions::TransformCaseOptions(const TransformCaseOptions &other)
    : fixCondition(other.fixCondition)
    , fixSignalOrPortCondition(other.fixSignalOrPortCondition)
    , splitCases(other.splitCases)
    , simplify(other.simplify)
    , createdVariable(other.createdVariable)
    , createdAssign(other.createdAssign)
{
    // ntd
}

TransformCaseOptions &TransformCaseOptions::operator=(TransformCaseOptions other)
{
    swap(other);
    return *this;
}

void TransformCaseOptions::swap(TransformCaseOptions &other)
{
    std::swap(fixCondition, other.fixCondition);
    std::swap(fixSignalOrPortCondition, other.fixSignalOrPortCondition);
    std::swap(splitCases, other.splitCases);
    std::swap(simplify, other.simplify);
    std::swap(createdVariable, other.createdVariable);
    std::swap(createdAssign, other.createdAssign);
}
// ///////////////////////////////////////////////////////////////////
// transformSwitchToIf()
// ///////////////////////////////////////////////////////////////////

If *transformSwitchToIf(Switch *sw, const TransformCaseOptions &opt, hif::semantics::ILanguageSemantics *sem)
{
    messageAssert(sw != nullptr, "Passed nullptr switch", nullptr, sem);
    if (sw->getCaseSemantics() != hif::CASE_LITERAL)
        return nullptr;

    TransformCaseOptions &copt = const_cast<TransformCaseOptions &>(opt);
    if (opt.fixCondition)
        _makeSupportVariable(sw, sem, copt);

    _mapSignedUnsigned(sw, sem);

    // Ensuring sem type
    Value *switchValue = sw->getCondition();
    hif::semantics::getSemanticType(switchValue, sem);

    If *ifObj = new If();
    if (sw->getParent() != nullptr) {
        sw->replace(ifObj);
    }

    // transfer actions from SwitchAlt to CaseAlt
    for (BList<SwitchAlt>::iterator i = sw->alts.begin(); i != sw->alts.end(); ++i) {
        SwitchAlt *swAlt = *i;

        if (!opt.splitCases) {
            IfAlt *ifalt = new IfAlt();
            ifObj->alts.push_back(ifalt);
            ifalt->setCondition(_makeCond(switchValue, swAlt->conditions, sem));
            ifalt->actions.merge(swAlt->actions);
        } else {
            for (BList<Value>::iterator j = swAlt->conditions.begin(); j != swAlt->conditions.end();) {
                Value *cond  = *j;
                j            = j.remove();
                IfAlt *ifAlt = new IfAlt();
                ifObj->alts.push_back(ifAlt);
                BList<Value> tmp;
                tmp.push_back(cond);
                ifAlt->setCondition(_makeCond(switchValue, tmp, sem));
                hif::copy(swAlt->actions, ifAlt->actions);
            }
        }
    }

    // transfer default actions from Switch to Case
    ifObj->defaults.merge(sw->defaults);

    // transfer properties and comments from Switch to Case
    _copyPropertiesAndComments(sw, ifObj);

    delete sw;

    if (opt.simplify)
        hif::manipulation::simplify(ifObj, sem);

    return ifObj;
}

// ///////////////////////////////////////////////////////////////////
// transformWithToWhen()
// ///////////////////////////////////////////////////////////////////

Value *transformWithToWhen(With *with, const TransformCaseOptions &opt, semantics::ILanguageSemantics *sem)
{
    messageAssert(with != nullptr, "Passed nullptr with", nullptr, sem);
    if (with->getCaseSemantics() != hif::CASE_LITERAL)
        return nullptr;

    TransformCaseOptions &copt = const_cast<TransformCaseOptions &>(opt);
    if (opt.fixCondition)
        _makeSupportVariable(with, sem, copt);

    _mapSignedUnsigned(with, sem);

    // Ensuring sem type
    Value *withValue = with->getCondition();
    hif::semantics::getSemanticType(withValue, sem);

    When *whenObj = new When();
    if (with->getParent() != nullptr) {
        with->replace(whenObj);
    }

    // transfer actions from WithAlt to WhenAlt
    for (BList<WithAlt>::iterator i = with->alts.begin(); i != with->alts.end(); ++i) {
        WithAlt *alt = *i;

        if (!opt.splitCases) {
            WhenAlt *caseAlt = new WhenAlt();
            whenObj->alts.push_back(caseAlt);
            caseAlt->setCondition(_makeCond(withValue, alt->conditions, sem));
            caseAlt->setValue(alt->setValue(nullptr));
        } else {
            for (BList<Value>::iterator j = alt->conditions.begin(); j != alt->conditions.end();) {
                Value *cond      = *j;
                j                = j.remove();
                WhenAlt *caseAlt = new WhenAlt();
                whenObj->alts.push_back(caseAlt);
                BList<Value> tmp;
                tmp.push_back(cond);
                caseAlt->setCondition(_makeCond(withValue, tmp, sem));
                caseAlt->setValue(hif::copy(alt->getValue()));
            }
        }
    }

    // transfer default actions from With to When
    whenObj->setDefault(with->setDefault(nullptr));
    // transfer properties and comments from With to When
    _copyPropertiesAndComments(with, whenObj);

    delete with;

    Value *ret = hif::manipulation::simplify(whenObj, sem);

    return ret;
}
} // namespace manipulation
} // namespace hif
