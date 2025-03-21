/// @file assignUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/assignUtils.hpp"

#include "hif/BiVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

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

namespace /*anon*/
{
class AssignTargetVisitor : public MonoVisitor<AssignTargetVisitor>
{
public:
    AssignTargetVisitor(Object *_caller, const LeftHandSideOptions &_opt);
    virtual ~AssignTargetVisitor();

    /// @brief Base Map.
    void map(Object *obj);

    void map(Action *obj);
    void map(Alt *obj);
    void map(Declaration *obj);
    void map(GlobalAction *obj);
    void map(Type *obj);
    void map(ReferencedAssign *obj);

    void map(Assign *obj);
    void map(PortAssign *obj);
    void map(WithAlt *obj);
    void map(WhenAlt *obj);
    void map(RecordValueAlt *obj);
    void map(AggregateAlt *obj);

    void map(Aggregate *obj);
    void map(Cast *obj);
    void map(ConstValue *obj);
    void map(Expression *obj);
    void map(FunctionCall *obj);
    void map(Identifier *obj);
    void map(Instance *obj);
    void map(FieldReference *obj);
    void map(Member *obj);
    void map(Slice *obj);
    void map(Range *obj);
    void map(RecordValue *obj);
    void map(When *obj);
    void map(With *obj);

    bool isAssignTarget() const;

private:
    /// @brief The last object visited.
    Object *_caller;
    /// @brief The result of elaboration.
    bool _isAssignTarget;
    /// @brief Reference to the assign target.
    Value *_target;
    /// @brief The options
    const LeftHandSideOptions &_opt;

    // disabled
    AssignTargetVisitor(const AssignTargetVisitor &o);
    AssignTargetVisitor &operator=(const AssignTargetVisitor &o);
};

// function implementations
AssignTargetVisitor::AssignTargetVisitor(Object *caller, const LeftHandSideOptions &opt)
    : _caller(caller)
    , _isAssignTarget(false)
    , _target(nullptr)
    , _opt(opt)
{
    // ntd
}

AssignTargetVisitor::~AssignTargetVisitor()
{
    // ntd
}

void AssignTargetVisitor::map(Object *obj)
{
    messageError("Unexpected object reached by AssignTargetMap_t", obj, nullptr);
}

void AssignTargetVisitor::map(Action * /*obj*/) { _isAssignTarget = false; }

void AssignTargetVisitor::map(Alt * /*obj*/) { _isAssignTarget = false; }

void AssignTargetVisitor::map(Declaration * /*obj*/) { _isAssignTarget = false; }

void AssignTargetVisitor::map(GlobalAction * /*obj*/) { _isAssignTarget = false; }

void AssignTargetVisitor::map(Type * /*obj*/) { _isAssignTarget = false; }

void AssignTargetVisitor::map(ReferencedAssign * /*obj*/) { _isAssignTarget = false; }
void AssignTargetVisitor::map(Assign *obj)
{
    if (_caller == obj->getLeftHandSide()) {
        _isAssignTarget = true;
        _target         = obj->getLeftHandSide();
    } else
        _isAssignTarget = false;
}

void AssignTargetVisitor::map(PortAssign *obj)
{
    if (!_opt.considerPortassigns) {
        _isAssignTarget = false;
        return;
    }

    messageAssert(_caller == obj->getValue(), "Unexpected case", obj, _opt.sem);
    Port *decl = hif::semantics::getDeclaration(obj, _opt.sem);
    messageAssert(decl != nullptr, "Cannot find declaration", obj, _opt.sem);

    _isAssignTarget = (decl->getDirection() != hif::dir_in);
}

void AssignTargetVisitor::map(WithAlt *obj)
{
    if (_caller == obj || _caller == obj->getValue()) {
        _caller = obj;
        callMap(obj->getParent());
        return;
    }
    _isAssignTarget = false;
}

void AssignTargetVisitor::map(WhenAlt *obj)
{
    if (_caller == obj || _caller == obj->getValue()) {
        _caller = obj;
        callMap(obj->getParent());
        return;
    }
    _isAssignTarget = false;
}

void AssignTargetVisitor::map(RecordValueAlt *obj)
{
    if (_caller == obj || _caller == obj->getValue()) {
        _caller = obj;
        callMap(obj->getParent());
        return;
    }
    _isAssignTarget = false;
}

void AssignTargetVisitor::map(AggregateAlt *obj)
{
    if (_caller == obj || _caller == obj->getValue()) {
        _caller = obj;
        callMap(obj->getParent());
        return;
    }
    _isAssignTarget = false;
}

void AssignTargetVisitor::map(Aggregate *obj)
{
    _caller = obj;
    callMap(obj->getParent());
}

void AssignTargetVisitor::map(Cast *obj)
{
    if (_caller == obj || _caller == obj->getValue()) {
        _caller = obj;
        callMap(obj->getParent());
        return;
    }

    _isAssignTarget = false;
}

void AssignTargetVisitor::map(ConstValue * /*obj*/) { _isAssignTarget = false; }

void AssignTargetVisitor::map(Expression *obj)
{
    if (_caller == obj || obj->getOperator() == op_concat) {
        _caller = obj;
        callMap(obj->getParent());
        return;
    }
    _isAssignTarget = false;
}

void AssignTargetVisitor::map(FunctionCall *obj)
{
    // May be foo() = val .. with foo returning a reference.
    _caller = obj;
    callMap(obj->getParent());
}

void AssignTargetVisitor::map(Identifier *obj)
{
    _caller = obj;
    callMap(obj->getParent());
}

void AssignTargetVisitor::map(Instance * /*obj*/) { _isAssignTarget = false; }

void AssignTargetVisitor::map(FieldReference *obj)
{
    if (_caller == obj || (_opt.considerRecord && _caller == obj->getPrefix())) {
        _caller = obj;
        callMap(obj->getParent());
    } else
        _isAssignTarget = false;
}

void AssignTargetVisitor::map(Member *obj)
{
    if (_caller == obj || _caller == obj->getPrefix()) {
        _caller = obj;
        callMap(obj->getParent());
    } else
        _isAssignTarget = false;
}

void AssignTargetVisitor::map(Slice *obj)
{
    if (_caller == obj || _caller == obj->getPrefix()) {
        _caller = obj;
        callMap(obj->getParent());
    } else
        _isAssignTarget = false;
}

void AssignTargetVisitor::map(Range * /*obj*/) { _isAssignTarget = false; }

void AssignTargetVisitor::map(RecordValue *obj)
{
    _caller = obj;
    callMap(obj->getParent());
}

void AssignTargetVisitor::map(When *obj)
{
    _caller = obj;
    callMap(obj->getParent());
}

void AssignTargetVisitor::map(With *obj)
{
    if (_caller == obj->getCondition()) {
        _isAssignTarget = false;
        return;
    }

    _caller = obj;
    callMap(obj->getParent());
}

bool AssignTargetVisitor::isAssignTarget() const { return _isAssignTarget; }

} // namespace

LeftHandSideOptions::LeftHandSideOptions()
    : sem(hif::semantics::HIFSemantics::getInstance())
    , considerRecord(false)
    , considerPortassigns(false)
{
    // ntd
}

LeftHandSideOptions::~LeftHandSideOptions()
{
    // ntd
}

LeftHandSideOptions::LeftHandSideOptions(const LeftHandSideOptions &o)
    : sem(o.sem)
    , considerRecord(o.considerRecord)
    , considerPortassigns(o.considerPortassigns)
{
    // ntd
}

LeftHandSideOptions &LeftHandSideOptions::operator=(const LeftHandSideOptions &o)
{
    if (&o == this)
        return *this;

    sem                 = o.sem;
    considerRecord      = o.considerRecord;
    considerPortassigns = o.considerPortassigns;

    return *this;
}

bool isInLeftHandSide(Object *obj, const LeftHandSideOptions &opt)
{
    AssignTargetVisitor atv(obj, opt);
    atv.callMap(obj);

    return atv.isAssignTarget();
}

Value *getLeftHandSide(Value *obj)
{
    if (!isInLeftHandSide(obj)) {
        Assign *a = hif::getNearestParent<Assign>(obj);
        if (a == nullptr)
            return nullptr;
        return a->getLeftHandSide();
    }

    Value *ret = obj;
    Value *v   = obj;
    while (v != nullptr) {
        ret = v;
        v   = dynamic_cast<Slice *>(ret->getParent());
        if (v == nullptr)
            v = dynamic_cast<Member *>(ret->getParent());
        if (v == nullptr)
            v = dynamic_cast<FieldReference *>(ret->getParent());
    }

    return ret;
}
} // namespace manipulation
} // namespace hif
