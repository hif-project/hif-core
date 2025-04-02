/// @file resetTypes.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/resetTypes.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

// //////////////////////////////////////////////////////////////////////////////
// Reset type visitor.
//
// Support class to reset the type of every Value
// contained in an HIF subtree.
//
// //////////////////////////////////////////////////////////////////////////////
class ResetTypeVisitor : public GuideVisitor
{
public:
    ResetTypeVisitor(bool recursive);

    /// @brief Destructor
    virtual ~ResetTypeVisitor();

    virtual int visitAggregate(Aggregate &o);
    virtual int visitArray(Array &o);
    virtual int visitBitValue(BitValue &o);
    virtual int visitBitvectorValue(BitvectorValue &o);
    virtual int visitBoolValue(BoolValue &o);
    virtual int visitCast(Cast &o);
    virtual int visitCharValue(CharValue &o);
    virtual int visitExpression(Expression &o);
    virtual int visitFieldReference(FieldReference &o);
    virtual int visitFile(File &o);
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitIdentifier(Identifier &o);
    virtual int visitInstance(Instance &o);
    virtual int visitIntValue(IntValue &o);
    virtual int visitMember(Member &o);
    virtual int visitParameterAssign(ParameterAssign &o);
    virtual int visitPointer(Pointer &o);
    virtual int visitPortAssign(PortAssign &o);
    virtual int visitRange(Range &o);
    virtual int visitRealValue(RealValue &o);
    virtual int visitRecord(Record &o);
    virtual int visitRecordValue(RecordValue &o);
    virtual int visitReference(Reference &o);
    virtual int visitSlice(Slice &o);
    virtual int visitStringValue(StringValue &o);
    virtual int visitTimeValue(TimeValue &o);
    virtual int visitTypeTPAssign(TypeTPAssign &o);
    virtual int visitValueTPAssign(ValueTPAssign &o);
    virtual int visitWhen(When &o);
    virtual int visitWith(With &o);

private:
    void _resetType(TypedObject &);
    template <typename T> void _resetBaseTypes(T &);
    bool _recursive;

    ResetTypeVisitor(const ResetTypeVisitor &);
    ResetTypeVisitor operator=(const ResetTypeVisitor &);
};

// Constructor
ResetTypeVisitor::ResetTypeVisitor(bool recursive)
    : GuideVisitor()
    , _recursive(recursive)
{
    // ntd
}

ResetTypeVisitor::~ResetTypeVisitor()
{
    // ntd
}

void ResetTypeVisitor::_resetType(TypedObject &o)
{
    // Reset previous type (if set) and set new type to nullptr
    delete o.setSemanticType(nullptr);
}

template <typename T> void ResetTypeVisitor::_resetBaseTypes(T &o)
{
    delete o.setBaseType(nullptr, false);
    delete o.setBaseType(nullptr, true);
}

int ResetTypeVisitor::visitAggregate(Aggregate &o)
{
    if (_recursive)
        GuideVisitor::visitAggregate(o);
    _resetType(o);
    return 0;
}

int ResetTypeVisitor::visitArray(Array &o)
{
    if (_recursive)
        GuideVisitor::visitArray(o);
    _resetBaseTypes(o);
    return 0;
}
int ResetTypeVisitor::visitBitValue(BitValue &o)
{
    if (_recursive)
        GuideVisitor::visitBitValue(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitBitvectorValue(BitvectorValue &o)
{
    if (_recursive)
        GuideVisitor::visitBitvectorValue(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitBoolValue(BoolValue &o)
{
    if (_recursive)
        GuideVisitor::visitBoolValue(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitCast(Cast &o)
{
    if (_recursive)
        GuideVisitor::visitCast(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitCharValue(CharValue &o)
{
    if (_recursive)
        GuideVisitor::visitCharValue(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitExpression(Expression &o)
{
    if (_recursive)
        GuideVisitor::visitExpression(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitFieldReference(FieldReference &o)
{
    if (_recursive)
        GuideVisitor::visitFieldReference(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitFile(File &o)
{
    if (_recursive)
        GuideVisitor::visitFile(o);
    _resetBaseTypes(o);
    return 0;
}
int ResetTypeVisitor::visitFunctionCall(FunctionCall &o)
{
    if (_recursive)
        GuideVisitor::visitFunctionCall(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitInstance(Instance &o)
{
    if (_recursive)
        GuideVisitor::visitInstance(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitIntValue(IntValue &o)
{
    if (_recursive)
        GuideVisitor::visitIntValue(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitMember(Member &o)
{
    if (_recursive)
        GuideVisitor::visitMember(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitIdentifier(Identifier &o)
{
    if (_recursive)
        GuideVisitor::visitIdentifier(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitParameterAssign(ParameterAssign &o)
{
    if (_recursive)
        GuideVisitor::visitParameterAssign(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitPointer(Pointer &o)
{
    if (_recursive)
        GuideVisitor::visitPointer(o);
    _resetBaseTypes(o);
    return 0;
}
int ResetTypeVisitor::visitPortAssign(PortAssign &o)
{
    if (_recursive)
        GuideVisitor::visitPortAssign(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitRange(Range &o)
{
    if (_recursive)
        GuideVisitor::visitRange(o);
    _resetType(o);
    return 0;
}

int ResetTypeVisitor::visitRealValue(RealValue &o)
{
    if (_recursive)
        GuideVisitor::visitRealValue(o);
    _resetType(o);
    return 0;
}

int ResetTypeVisitor::visitRecord(Record &o)
{
    if (_recursive)
        GuideVisitor::visitRecord(o);
    _resetBaseTypes(o);
    return 0;
}
int ResetTypeVisitor::visitRecordValue(RecordValue &o)
{
    if (_recursive)
        GuideVisitor::visitRecordValue(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitReference(Reference &o)
{
    if (_recursive)
        GuideVisitor::visitReference(o);
    _resetBaseTypes(o);
    return 0;
}
int ResetTypeVisitor::visitSlice(Slice &o)
{
    if (_recursive)
        GuideVisitor::visitSlice(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitStringValue(StringValue &o)
{
    if (_recursive)
        GuideVisitor::visitStringValue(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitTimeValue(TimeValue &o)
{
    if (_recursive)
        GuideVisitor::visitTimeValue(o);
    _resetType(o);
    return 0;
}

int ResetTypeVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    if (_recursive)
        GuideVisitor::visitTypeTPAssign(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    if (_recursive)
        GuideVisitor::visitValueTPAssign(o);
    _resetType(o);
    return 0;
}

int ResetTypeVisitor::visitWhen(When &o)
{
    if (_recursive)
        GuideVisitor::visitWhen(o);
    _resetType(o);
    return 0;
}
int ResetTypeVisitor::visitWith(With &o)
{
    if (_recursive)
        GuideVisitor::visitWith(o);
    _resetType(o);
    return 0;
}

} // namespace

// ///////////////////////////////////////////////////////////////////
// Public methods
// ///////////////////////////////////////////////////////////////////

void resetTypes(Object *root, bool recursive)
{
    messageDebugAssert(root != nullptr, "Passed null root", nullptr, nullptr);
    if (root == nullptr)
        return;

    ResetTypeVisitor rtv(recursive);
    root->acceptVisitor(rtv);
}
void resetTypes(BList<Object> &root, bool recursive)
{
    for (BList<Object>::iterator i = root.begin(); i != root.end(); ++i) {
        resetTypes(*i, recursive);
    }
}

template <typename T> void resetTypes(BList<T> &root, bool recursive)
{
    BList<Object> *tmp = reinterpret_cast<BList<Object> *>(&root);
    resetTypes(*tmp, recursive);
}

#define HIF_TEMPLATE_METHOD(T) void resetTypes<T>(BList<T> &, bool)

HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD
} // namespace semantics
} // namespace hif
