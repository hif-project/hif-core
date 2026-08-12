/// @file operatorUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cassert>

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"

namespace hif
{

namespace
{
class OperatorFeatures
{
public:
    OperatorFeatures();
    ~OperatorFeatures();
    void init(const hif::Operator o);

    bool _isArithmetic;
    bool _isUnary;
    bool _isBinary;
    bool _isCommutative;
    bool _isLogical;
    bool _isBitwise;
    bool _isAddress;
    bool _isAssignment;

    /// @brief Identifies the subset of reduce operators.
    /// TODO May be considered bitwise?
    bool _isReduce;
    bool _isRelational;
    bool _isEquality;

    /// @brief Subset of shift operators.
    bool _isRotate;
    bool _isShift;

    hif::Operator _inverse;
    hif::Operator _logicBitwiseConversion;

private:
    // Disable.
    OperatorFeatures(const OperatorFeatures &);
    const OperatorFeatures &operator=(const OperatorFeatures &);

    void _reset();
};
OperatorFeatures::OperatorFeatures()
    : _isArithmetic(false)
    , _isUnary(false)
    , _isBinary(false)
    , _isCommutative(false)
    , _isLogical(false)
    , _isBitwise(false)
    , _isAddress(false)
    , _isAssignment(false)
    , _isReduce(false)
    , _isRelational(false)
    , _isEquality(false)
    , _isRotate(false)
    , _isShift(false)
    , _inverse(op_none)
    , _logicBitwiseConversion(op_none)
{
    // ntd
}

OperatorFeatures::~OperatorFeatures()
{
    // ntd
}

void OperatorFeatures::_reset()
{
    _isArithmetic           = false;
    _isUnary                = false;
    _isBinary               = false;
    _isCommutative          = false;
    _isLogical              = false;
    _isBitwise              = false;
    _isAddress              = false;
    _isAssignment           = false;
    _isReduce               = false;
    _isEquality             = false;
    _isRelational           = false;
    _isRotate               = false;
    _isShift                = false;
    _inverse                = op_none;
    _logicBitwiseConversion = op_none;
}

void OperatorFeatures::init(const hif::Operator o)
{
    _reset();

    switch (o) {
    case op_abs:
        _isUnary      = true;
        _isArithmetic = true;
        break;
    case op_and:
        _isBinary               = true;
        _isLogical              = true;
        _logicBitwiseConversion = op_band;
        // Not commutative due to lazy valuation.
        break;
    case op_andrd:
        _isUnary  = true;
        _isReduce = true;
        break;
    case op_assign:
    case op_conv:
    case op_bind:
        _isBinary     = true;
        _isAssignment = true;
        // Not logic.
        // Not arithmetic.
        break;
    case op_band:
        _isBinary               = true;
        _isCommutative          = true;
        _isBitwise              = true;
        _logicBitwiseConversion = op_and;
        break;
    case op_bnot:
        _isUnary                = true;
        _isBitwise              = true;
        _logicBitwiseConversion = op_not;
        break;
    case op_bor:
        _isBinary               = true;
        _isCommutative          = true;
        _isBitwise              = true;
        _logicBitwiseConversion = op_or;
        break;
    case op_bxor:
        _isBinary               = true;
        _isCommutative          = true;
        _isBitwise              = true;
        _logicBitwiseConversion = op_xor;
        break;
    case op_case_eq:
        _isBinary      = true;
        _isCommutative = true;
        _isRelational  = true;
        _isEquality    = true;
        _inverse       = op_case_neq;
        break;
    case op_case_neq:
        _isBinary      = true;
        _isRelational  = true;
        _isEquality    = true;
        _isCommutative = true;
        _inverse       = op_case_eq;
        break;
    case op_concat:
        _isBinary = true;
        // Not logic.
        // Not arithmetic.
        break;
    case op_deref: // TODO check
        _isUnary   = true;
        _isAddress = true;
        _inverse   = op_ref;
        break;
    case op_div:
        _isBinary     = true;
        _isArithmetic = true;
        _inverse      = op_mult;
        break;
    case op_eq:
        _isBinary      = true;
        _isCommutative = true;
        _isRelational  = true;
        _isEquality    = true;
        _inverse       = op_neq;
        break;
    case op_ge:
        _isBinary     = true;
        _isRelational = true;
        _inverse      = op_lt;
        break;
    case op_gt:
        _isBinary     = true;
        _isRelational = true;
        _inverse      = op_le;
        break;
    case op_le:
        _isBinary     = true;
        _isRelational = true;
        _inverse      = op_gt;
        break;
    case op_log:
        _isBinary     = true;
        _isArithmetic = true;
        break;
    case op_lt:
        _isBinary     = true;
        _isRelational = true;
        _inverse      = op_ge;
        break;
    case op_minus:
        _isUnary      = true;
        _isBinary     = true;
        _isArithmetic = true;
        _inverse      = op_plus;
        break;
    case op_mod:
        _isBinary     = true;
        _isArithmetic = true;
        break;
    case op_mult:
        _isBinary      = true;
        _isArithmetic  = true;
        _isCommutative = true;
        _inverse       = op_div;
        break;
    case op_neq:
        _isBinary      = true;
        _isRelational  = true;
        _isEquality    = true;
        _isCommutative = true;
        _inverse       = op_eq;
        break;
    case op_none:
        _isUnary = true;
        // Nothing else.
        break;
    case op_not:
        _isUnary                = true;
        _isLogical              = true;
        _logicBitwiseConversion = op_bnot;
        break;
    case op_or:
        _isBinary               = true;
        _isLogical              = true;
        _logicBitwiseConversion = op_bor;
        // Not commutative due to lazy valuation.
        break;
    case op_orrd:
        _isUnary  = true;
        _isReduce = true;
        break;
    case op_plus:
        _isUnary       = true;
        _isBinary      = true;
        _isArithmetic  = true;
        _isCommutative = true;
        _inverse       = op_minus;
        break;
    case op_pow:
        _isBinary     = true;
        _isArithmetic = true;
        break;
    case op_ref: // TODO check
        _isUnary   = true;
        _isAddress = true;
        _inverse   = op_deref;
        break;
    case op_rem:
        _isBinary     = true;
        _isArithmetic = true;
        break;
    case op_rol:
        _isBinary = true;
        _isRotate = true;
        _isShift  = true;
        break;
    case op_ror:
        _isBinary = true;
        _isRotate = true;
        _isShift  = true;
        break;
    case op_sla:
        _isBinary = true;
        _isShift  = true;
        // Note: slr is not reverse operation, due to loss of bits.
        break;
    case op_sll:
        _isBinary = true;
        _isShift  = true;
        // Note: srl is not reverse operation, due to loss of bits.
        break;
    case op_sra:
        _isBinary = true;
        _isShift  = true;
        // Note: sra is not reverse operation, due to loss of bits.
        break;
    case op_srl:
        _isBinary = true;
        _isShift  = true;
        // Note: sla is not reverse operation, due to loss of bits.
        break;
    case op_xor:
        _isBinary               = true;
        _isLogical              = true;
        _logicBitwiseConversion = op_bxor;
        // Is commutative, since result always depends on both values
        _isCommutative          = true;
        break;
    case op_xorrd:
        _isUnary  = true;
        _isReduce = true;
        break;
    case op_reverse:
        _isUnary = true;
        break;
    case op_size:
        _isUnary = true;
        break;
    default:
        messageDebugAssert(false, "Unexpected case", nullptr, nullptr);
    }
}

} // namespace
bool operatorIsArithmetic(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isArithmetic;
}
bool operatorIsBinary(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isBinary;
}
bool operatorIsCommutative(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isCommutative;
}
bool operatorIsLogical(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isLogical;
}
bool operatorIsBitwise(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isBitwise;
}
bool operatorIsReduce(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isReduce;
}
bool operatorIsRelational(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isRelational;
}
bool operatorIsEquality(hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isEquality;
}
bool operatorIsRotate(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isRotate;
}
bool operatorIsShift(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isShift;
}
bool operatorIsAddress(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isAddress;
}
bool operatorIsUnary(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isUnary;
}
bool operatorIsAssignment(hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._isAssignment;
}
hif::Operator operatorGetInverse(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._inverse;
}

hif::Operator operatorGetLogicBitwiseConversion(const hif::Operator oper)
{
    OperatorFeatures opFeatures;
    opFeatures.init(oper);
    return opFeatures._logicBitwiseConversion;
}

} // namespace hif
