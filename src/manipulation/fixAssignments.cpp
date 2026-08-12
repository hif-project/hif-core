/// @file fixAssignments.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/fixAssignments.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

// ///////////////////////////////////////////////////////////////////
// FixAssignmentsVisitor
// ///////////////////////////////////////////////////////////////////
class FixAssignmentsVisitor : public GuideVisitor
{
public:
    FixAssignmentsVisitor(hif::semantics::ILanguageSemantics *sem, const FixAssignmentOptions &opt);
    ~FixAssignmentsVisitor();

    virtual int visitAssign(Assign &o);

    virtual int visitAlias(Alias &o);
    virtual int visitConst(Const &o);
    virtual int visitEnumValue(EnumValue &o);
    virtual int visitField(Field &o);
    virtual int visitParameter(Parameter &o);
    virtual int visitPort(Port &o);
    virtual int visitSignal(Signal &o);
    virtual int visitVariable(Variable &o);
    virtual int visitValueTP(ValueTP &o);

    virtual int visitParameterAssign(ParameterAssign &o);

    bool isFixed();

private:
    FixAssignmentsVisitor(const FixAssignmentsVisitor &);
    FixAssignmentsVisitor &operator=(const FixAssignmentsVisitor &);

    bool _isFixed;
    hif::semantics::ILanguageSemantics *_sem;
    const FixAssignmentOptions &_opt;
    hif::HifFactory _f;

    int _visitDataDeclaration(DataDeclaration *o);
    void _fixAssignment(Type *lhsType, Value *rhs, const hif::Operator op);
};

FixAssignmentsVisitor::FixAssignmentsVisitor(hif::semantics::ILanguageSemantics *sem, const FixAssignmentOptions &opt)
    : _isFixed(false)
    , _sem(sem)
    , _opt(opt)
    , _f(sem)
{
    // ntd
}

FixAssignmentsVisitor::~FixAssignmentsVisitor()
{
    // ntd
}

int FixAssignmentsVisitor::visitAssign(Assign &o)
{
    GuideVisitor::visitAssign(o);

    if (!_opt.fixAssigns)
        return 0;

    Type *lhsType = hif::semantics::getSemanticType(o.getLeftHandSide(), _sem);
    messageAssert(lhsType != nullptr, "Cannot type assign left hand side", o.getLeftHandSide(), _sem);
    _fixAssignment(lhsType, o.getRightHandSide(), op_assign);

    return 0;
}

int FixAssignmentsVisitor::visitAlias(Alias &o)
{
    GuideVisitor::visitAlias(o);
    _visitDataDeclaration(&o);
    return 0;
}

int FixAssignmentsVisitor::visitConst(Const &o)
{
    GuideVisitor::visitConst(o);
    _visitDataDeclaration(&o);
    return 0;
}

int FixAssignmentsVisitor::visitEnumValue(EnumValue &o)
{
    GuideVisitor::visitEnumValue(o);
    _visitDataDeclaration(&o);
    return 0;
}

int FixAssignmentsVisitor::visitField(Field &o)
{
    GuideVisitor::visitField(o);
    _visitDataDeclaration(&o);
    return 0;
}

int FixAssignmentsVisitor::visitParameter(Parameter &o)
{
    GuideVisitor::visitParameter(o);
    _visitDataDeclaration(&o);
    return 0;
}

int FixAssignmentsVisitor::visitPort(Port &o)
{
    GuideVisitor::visitPort(o);
    _visitDataDeclaration(&o);
    return 0;
}

int FixAssignmentsVisitor::visitSignal(Signal &o)
{
    GuideVisitor::visitSignal(o);
    _visitDataDeclaration(&o);
    return 0;
}

int FixAssignmentsVisitor::visitVariable(Variable &o)
{
    GuideVisitor::visitVariable(o);
    _visitDataDeclaration(&o);
    return 0;
}

int FixAssignmentsVisitor::visitValueTP(ValueTP &o)
{
    GuideVisitor::visitValueTP(o);
    _visitDataDeclaration(&o);
    return 0;
}

int FixAssignmentsVisitor::visitParameterAssign(ParameterAssign &o)
{
    GuideVisitor::visitParameterAssign(o);

    if (!_opt.fixParameterAssigns)
        return 0;

    Type *lhsType = hif::semantics::getSemanticType(&o, _sem);
    messageAssert(lhsType != nullptr, "Cannot type parameter assign", &o, _sem);
    _fixAssignment(lhsType, o.getValue(), op_conv);
    return 0;
}

bool FixAssignmentsVisitor::isFixed() { return _isFixed; }

int FixAssignmentsVisitor::_visitDataDeclaration(DataDeclaration *o)
{
    if (!_opt.fixDataDeclarationsValue)
        return 0;
    Type *lhsType = o->getType();
    messageAssert(lhsType != nullptr, "Unexpected data declaration", o, _sem);
    if (o->getValue() == nullptr)
        return 0;
    _fixAssignment(lhsType, o->getValue(), op_assign);
    return 0;
}

void FixAssignmentsVisitor::_fixAssignment(Type *lhsType, Value *rhs, const Operator op)
{
    Type *rhsType = hif::semantics::getSemanticType(rhs, _sem);
    messageDebugAssert(rhsType != nullptr, "Cannot type right hand side/initial value", rhs, _sem);

    if (rhsType != nullptr && _sem->getExprType(lhsType, rhsType, op, lhsType->getParent()).returnedType != nullptr) {
        return;
    }

    Cast *cast = new Cast();
    cast->setType(hif::copy(lhsType));
    rhs->replace(cast);
    cast->setValue(rhs);

    _isFixed = true;
}

} // namespace

// ///////////////////////////////////////////////////////////////////
// FixAssignementOptions
// ///////////////////////////////////////////////////////////////////
FixAssignmentOptions::FixAssignmentOptions()
    : fixAssigns(false)
    , fixParameterAssigns(false)
    , fixDataDeclarationsValue(false)
{
    // ntd
}

FixAssignmentOptions::~FixAssignmentOptions()
{
    // ntd
}

FixAssignmentOptions::FixAssignmentOptions(const FixAssignmentOptions &o)
    : fixAssigns(o.fixAssigns)
    , fixParameterAssigns(o.fixParameterAssigns)
    , fixDataDeclarationsValue(o.fixDataDeclarationsValue)
{
    // ntd
}

FixAssignmentOptions &FixAssignmentOptions::operator=(FixAssignmentOptions o)
{
    swap(o);
    return *this;
}

void FixAssignmentOptions::swap(FixAssignmentOptions &o)
{
    std::swap(fixAssigns, o.fixAssigns);
    std::swap(fixParameterAssigns, o.fixParameterAssigns);
    std::swap(fixDataDeclarationsValue, o.fixDataDeclarationsValue);
}

bool FixAssignmentOptions::hasSomethingToFix() const
{
    return (fixAssigns || fixParameterAssigns || fixDataDeclarationsValue);
}
// ///////////////////////////////////////////////////////////////////
// fixAssignments method
// ///////////////////////////////////////////////////////////////////
bool fixAssignments(Object *root, hif::semantics::ILanguageSemantics *sem, const FixAssignmentOptions &opt)
{
    if (!opt.hasSomethingToFix())
        return false;

    FixAssignmentsVisitor fav(sem, opt);
    root->acceptVisitor(fav);

    return fav.isFixed();
}
} // namespace manipulation
} // namespace hif
