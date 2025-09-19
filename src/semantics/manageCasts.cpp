/// @file manageCasts.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <iostream>

#include "hif/hif.hpp"
#include "hif/semantics/standardization.hpp"

//#define DEBUG_MSG

namespace hif
{
namespace semantics
{

namespace
{ // unnamed
class ManageCast_t : public GuideVisitor
{
public:
    ManageCast_t(ILanguageSemantics *source, ILanguageSemantics *dest, CastMap &castMap);

    virtual ~ManageCast_t();

    /// @name explicitBoolConversion-stuff
    /// Call explicitBoolConversion of given semantics to object conditions.
    ///@{

    virtual int visitIfAlt(IfAlt &o);
    virtual int visitIfGenerate(IfGenerate &o);
    virtual int visitFor(For &o);
    virtual int visitForGenerate(ForGenerate &o);
    virtual int visitWhenAlt(WhenAlt &o);

    ///@}

    /// @brief Call explicitCast of given cast on given semantics.
    ///
    virtual int visitCast(Cast &o);

private:
    /// @brief Handle explicitBoolConversion call.
    ///
    void _fixCondition(Value *cond);

    ILanguageSemantics *_srcSem;
    ILanguageSemantics *_dstSem;

    CastMap &_castMap;

    ManageCast_t(const ManageCast_t &);
    ManageCast_t &operator=(const ManageCast_t &);
};
ManageCast_t::ManageCast_t(ILanguageSemantics *source, ILanguageSemantics *dest, CastMap &castMap)
    : GuideVisitor()
    , _srcSem(source)
    , _dstSem(dest)
    , _castMap(castMap)
{
}
ManageCast_t::~ManageCast_t() {}
int ManageCast_t::visitIfAlt(IfAlt &o)
{
    GuideVisitor::visitIfAlt(o);

    _fixCondition(o.getCondition());

    return 0;
}

int ManageCast_t::visitIfGenerate(IfGenerate &o)
{
    GuideVisitor::visitIfGenerate(o);

    _fixCondition(o.getCondition());

    return 0;
}
int ManageCast_t::visitFor(For &o)
{
    GuideVisitor::visitFor(o);

    _fixCondition(o.getCondition());

    return 0;
}
int ManageCast_t::visitForGenerate(ForGenerate &o)
{
    GuideVisitor::visitForGenerate(o);

    _fixCondition(o.getCondition());

    return 0;
}
int ManageCast_t::visitWhenAlt(WhenAlt &o)
{
    GuideVisitor::visitWhenAlt(o);

    _fixCondition(o.getCondition());

    return 0;
}

int ManageCast_t::visitCast(Cast &o)
{
    GuideVisitor::visitCast(o);

    Value *v = o.getValue();
    Type *t  = o.getType();

    // Explicit cast
    Type *srcType       = nullptr;
    CastMap::iterator i = _castMap.find(&o);
    if (i != _castMap.end())
        srcType = i->second;
    Value *ret = _dstSem->explicitCast(v, t, srcType);
    if (ret == nullptr) {
        messageDebug("Value to cast", v, _dstSem);
        messageError("Cast not managed.", &o, _dstSem);
    }

    o.replace(ret);
    delete &o;
    return 0;
}
void ManageCast_t::_fixCondition(Value *cond)
{
    if (dynamic_cast<Range *>(cond) != nullptr)
        return;

    // Checking conditions:
    // Eventually remove cast to bool, and inserting a relation check.
    // E.g.: if( bool(x)) --> if( x != 0 )

    // sanity checks.
    // TODO: need getBaseType?
    Type *condType = hif::semantics::getSemanticType(cond, _dstSem);
    if (condType == nullptr) {
        messageError("Cannot type condition.", cond, _dstSem);
    }

    // Removing unnecessary casts is managed by the visit of the cast.
    // here we have only to explicit boolean casts
    Cast *co = dynamic_cast<Cast *>(cond);
    if (co == nullptr) {
        // ok, no need for explicit bool conversion
        return;
    }

    Bit *condBit = dynamic_cast<Bit *>(condType);
    if (condBit != nullptr && condBit->isLogic()) {
        WhenAlt *wa = dynamic_cast<WhenAlt *>(cond->getParent());
        if (wa != nullptr) {
            When *w = dynamic_cast<When *>(wa->getParent());
            if (w != nullptr && w->isLogicTernary()) {
                // ok, no need for explicit bool conversion
                return;
            }
        }
    }

    Bool b;
    Type *mappedBool = _dstSem->getMapForType(&b);

    EqualsOptions opt;
    opt.checkOnlyTypes = true;
    bool isEq    = hif::equals(co->getType(), mappedBool, opt);
    delete mappedBool;
    if (isEq) {
        // ok, no need for explicit bool conversion
        return;
    }

    Value *newCond = _srcSem->explicitBoolConversion(co->getValue());
    if (newCond == nullptr) {
        messageError("Cannot transform condition of ", co->getValue(), _dstSem);
    }

    // Replace
    co->replace(newCond);
    delete co;
}

} // End unnamed namespace.
// //////////////////////////////////////////////////////////////////////////////////////
// manage_casts() method.
// //////////////////////////////////////////////////////////////////////////////////////

void manageCasts(Object *o, ILanguageSemantics *source, ILanguageSemantics *dest, CastMap &castMap)
{
    if (o == nullptr)
        return;

    hif::application_utils::initializeLogHeader("HIF", "manageCasts");

    // Temporary fix: is necessary to reset tree semantics types because
    // at the moment explicit of some casts may be unsafe in some semantics.
    // For example cast of IntValue/Bitvector to int type in SystemCSemantics.
    // Resetting tree we force to recalculate semantics type after the
    // explicit of cast nodes (see bug #538 for more details).
    hif::semantics::resetTypes(o, true);

    ManageCast_t mc(source, dest, castMap);
    o->acceptVisitor(mc);

    hif::application_utils::restoreLogHeader();
}

} // namespace semantics
} // namespace hif
