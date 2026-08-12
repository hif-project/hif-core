/// @file mapToNative.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <iostream>

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/manipulation/mapToNative.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace
{
class MapToNativeVisitor : public GuideVisitor
{
public:
    MapToNativeVisitor(hif::semantics::ILanguageSemantics *sem, hif::semantics::ILanguageSemantics *checkSem);
    ~MapToNativeVisitor();

    virtual int visitAggregate(Aggregate &o);
    virtual int visitMember(Member &o);
    virtual int visitSlice(Slice &o);
    virtual int visitCast(Cast &);

    // constant
    virtual int visitBitValue(BitValue &o);
    virtual int visitBitvectorValue(BitvectorValue &o);
    virtual int visitBoolValue(BoolValue &o);
    virtual int visitCharValue(CharValue &o);
    virtual int visitIntValue(IntValue &o);
    virtual int visitRealValue(RealValue &o);
    virtual int visitStringValue(StringValue &o);

private:
    void _performConstMapping(ConstValue &o);

    hif::semantics::ILanguageSemantics *_sem;
    hif::semantics::ILanguageSemantics *_checkSem;
    hif::manipulation::SimplifyOptions _opt;

    MapToNativeVisitor(const MapToNativeVisitor &);
    MapToNativeVisitor &operator=(const MapToNativeVisitor &);
};

MapToNativeVisitor::MapToNativeVisitor(
    hif::semantics::ILanguageSemantics *sem,
    hif::semantics::ILanguageSemantics *checkSem)
    : GuideVisitor()
    , _sem(sem)
    , _checkSem(checkSem)
    , _opt()
{
    _opt.simplify_constants = true;
}
MapToNativeVisitor::~MapToNativeVisitor()
{
    // ntd
}
void MapToNativeVisitor::_performConstMapping(ConstValue &o)
{
    Type *t = hif::semantics::getSemanticType(&o, _sem);

    if (_checkSem->isTemplateAllowedType(t))
        return;

    Type *mapped = _checkSem->getTemplateAllowedType(t);

    auto cvBw     = hif::semantics::spanGetBitwidth(hif::typeGetSpan(o.getType(), _sem), _sem);
    auto mappedBw = hif::semantics::spanGetBitwidth(hif::typeGetSpan(mapped, _sem), _sem);
    if (mappedBw < cvBw) {
        raiseUniqueWarning("Found at least a ConstValue that must be cast to native type "
                           "resulting in loss of precision.");
    }

    // In case of logic values, transform constant fails:
    // fixing this by mapping them to zeros:
    bool riseWarning = false;
    if (dynamic_cast<BitvectorValue *>(&o) != nullptr) {
        BitvectorValue *local = static_cast<BitvectorValue *>(&o);
        std::string val       = local->getValue();
        for (std::string::iterator i = val.begin(); i != val.end(); ++i) {
            char &cn = *i;
            switch (cn) {
            case '1':
            case 'H':
                cn = '1';
                break;
            case '0':
            case 'L':
                cn = '0';
                break;
            default:
                cn          = '0';
                riseWarning = true;
            };
        }
        local->setValue(val);
    } else if (dynamic_cast<BitValue *>(&o) != nullptr) {
        BitValue *local = static_cast<BitValue *>(&o);
        switch (local->getValue()) {
        case bit_one:
        case bit_h:
            local->setValue(bit_one);
            break;
        case bit_zero:
        case bit_l:
            local->setValue(bit_zero);
            break;
        case bit_u:
        case bit_x:
        case bit_z:
        case bit_w:
        case bit_dontcare:
            local->setValue(bit_zero);
            riseWarning = true;
            break;
        default:
            messageError("Unexpected bit value", local, _sem);
        }
    }

    if (riseWarning)
        raiseUniqueWarning("Found at least a not zero or one bit"
                           " inside a native mapped expression."
                           " They have been mapped to zero."
                           " Generated description could be not equivalent.");

    ConstValue *c = hif::manipulation::transformConstant(&o, mapped, _sem);
    delete mapped;
    messageAssert(c != nullptr, "ConstValue cannot be transformed", &o, _sem);

    o.replace(c);
    hif::manipulation::assureSyntacticType(c, _sem);
    delete &o;
}

int MapToNativeVisitor::visitAggregate(Aggregate &o)
{
    GuideVisitor::visitAggregate(o);

    for (BList<AggregateAlt>::iterator i = o.alts.begin(); i != o.alts.end(); ++i) {
        AggregateAlt *a = *i;
        for (BList<Value>::iterator j = a->indices.begin(); j != a->indices.end(); ++j) {
            Value *ind = *j;
            if (dynamic_cast<ConstValue *>(ind) != nullptr)
                continue;
            if (dynamic_cast<Range *>(ind) != nullptr) {
                Range *r         = static_cast<Range *>(ind);
                std::uint64_t ss = hif::semantics::spanGetBitwidth(r, _sem);
                if (ss != 0)
                    continue;
            }

            // fix indices in case of expressions.
            // ref. design: vhdl/gaisler/can_oc
            hif::manipulation::simplify(ind, _sem, _opt);
        }
    }

    return 0;
}

int MapToNativeVisitor::visitMember(Member &o)
{
    GuideVisitor::visitMember(o);

    Value *v = o.getPrefix();
    if (dynamic_cast<ConstValue *>(v) == nullptr) {
        hif::manipulation::simplify(v, _sem, _opt);
    }

    v = o.getIndex();
    if (dynamic_cast<ConstValue *>(v) == nullptr) {
        hif::manipulation::simplify(v, _sem, _opt);
    }

    return 0;
}

int MapToNativeVisitor::visitSlice(Slice &o)
{
    GuideVisitor::visitSlice(o);

    Value *v = o.getPrefix();
    if (dynamic_cast<ConstValue *>(v) == nullptr) {
        hif::manipulation::simplify(v, _sem, _opt);
    }

    v = o.getSpan()->getLeftBound();
    if (dynamic_cast<ConstValue *>(v) == nullptr) {
        hif::manipulation::simplify(v, _sem, _opt);
    }

    v = o.getSpan()->getRightBound();
    if (dynamic_cast<ConstValue *>(v) == nullptr) {
        hif::manipulation::simplify(v, _sem, _opt);
    }

    return 0;
}

int MapToNativeVisitor::visitCast(Cast &o)
{
    GuideVisitor::visitCast(o);

    if (_checkSem->isTemplateAllowedType(o.getType()))
        return 0;
    Type *good = _checkSem->getTemplateAllowedType(o.getType());
    delete o.setType(good);

    return 0;
}
int MapToNativeVisitor::visitBitValue(BitValue &o)
{
    GuideVisitor::visitBitValue(o);
    _performConstMapping(o);
    return 0;
}
int MapToNativeVisitor::visitBitvectorValue(BitvectorValue &o)
{
    GuideVisitor::visitBitvectorValue(o);
    _performConstMapping(o);
    return 0;
}
int MapToNativeVisitor::visitBoolValue(BoolValue &o)
{
    GuideVisitor::visitBoolValue(o);
    _performConstMapping(o);
    return 0;
}
int MapToNativeVisitor::visitCharValue(CharValue &o)
{
    GuideVisitor::visitCharValue(o);
    _performConstMapping(o);
    return 0;
}
int MapToNativeVisitor::visitIntValue(IntValue &o)
{
    GuideVisitor::visitIntValue(o);
    _performConstMapping(o);
    return 0;
}
int MapToNativeVisitor::visitRealValue(RealValue &o)
{
    GuideVisitor::visitRealValue(o);
    _performConstMapping(o);
    return 0;
}
int MapToNativeVisitor::visitStringValue(StringValue &o)
{
    GuideVisitor::visitStringValue(o);
    _performConstMapping(o);
    return 0;
}

} // end namespace
void mapToNative(Object *v, hif::semantics::ILanguageSemantics *sem, hif::semantics::ILanguageSemantics *checkSem)
{
    MapToNativeVisitor m(sem, checkSem);
    v->acceptVisitor(m);
}

} // namespace manipulation
} // namespace hif
