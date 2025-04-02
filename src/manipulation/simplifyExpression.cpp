/// @file simplifyExpression.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cmath>

#include "hif/manipulation/simplifyExpression.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
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

#if (defined _MSC_VER)
double log2(double d) { return log(d) / log(2.0); }
#else
#    pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

// ///////////////////////////////////////////////////////////////////
// SimpifyMapData
// ///////////////////////////////////////////////////////////////////
struct SimplifyMapData {
    hif::Operator oper;
    hif::semantics::ILanguageSemantics *sem;
    hif::Value *result;
    bool cannotBeSimplified;
    hif::Object *startingObject;
    SimplifyOptions options;

    SimplifyMapData();
    ~SimplifyMapData();
    SimplifyMapData(const SimplifyMapData &k);
    SimplifyMapData &operator=(const SimplifyMapData &k);
};

SimplifyMapData::SimplifyMapData()
    : oper(op_none)
    , sem(nullptr)
    , result(nullptr)
    , cannotBeSimplified(false)
    , startingObject(nullptr)
    , options()
{
    // ntd
}

SimplifyMapData::~SimplifyMapData()
{
    // ntd
}

SimplifyMapData::SimplifyMapData(const SimplifyMapData &k)
    : oper(k.oper)
    , sem(k.sem)
    , result(k.result)
    , cannotBeSimplified(k.cannotBeSimplified)
    , startingObject(k.startingObject)
    , options(k.options)
{
    // ntd
}

SimplifyMapData &SimplifyMapData::operator=(const SimplifyMapData &k)
{
    if (this == &k)
        return *this;

    oper               = k.oper;
    sem                = k.sem;
    result             = k.result;
    cannotBeSimplified = k.cannotBeSimplified;
    startingObject     = k.startingObject;
    options            = k.options;

    return *this;
}

// ///////////////////////////////////////////////////////////////////
// SimplifyMap
// ///////////////////////////////////////////////////////////////////
class SimplifyMap : public BiVisitor<SimplifyMap>
{
public:
    SimplifyMap(SimplifyMapData &d);
    ~SimplifyMap();

    Value *getResult() const;
    bool cannotBeSimplified() const;

    // default maps
    void map(Object *v1);
    void map(Object *v1, Object *v2);

    // unary maps
    void map(BitValue *v1);
    void map(BitvectorValue *v1);
    void map(BoolValue *v1);
    void map(CharValue *v1);
    void map(IntValue *v1);
    void map(RealValue *v1);
    void map(StringValue *v1);
    void map(Expression *v1);

    // binary maps for bit value
    void map(BitValue *v1, BitValue *v2);
    void map(BitValue *v1, BitvectorValue *v2);
    void map(BitValue *v1, BoolValue *v2);
    void map(BitValue *v1, CharValue *v2);
    void map(BitValue *v1, IntValue *v2);
    void map(BitValue *v1, RealValue *v2);
    void map(BitValue *v1, StringValue *v2);
    void map(BitValue *v1, Value *v2);
    void map(Value *v1, BitValue *v2);

    // binary maps for bit vector value
    void map(BitvectorValue *v1, BitValue *v2);
    void map(BitvectorValue *v1, BitvectorValue *v2);
    void map(BitvectorValue *v1, BoolValue *v2);
    void map(BitvectorValue *v1, CharValue *v2);
    void map(BitvectorValue *v1, IntValue *v2);
    void map(BitvectorValue *v1, RealValue *v2);
    void map(BitvectorValue *v1, StringValue *v2);
    void map(BitvectorValue *v1, Value *v2);
    void map(Value *v1, BitvectorValue *v2);

    // binary maps for bool value
    void map(BoolValue *v1, BitValue *v2);
    void map(BoolValue *v1, BitvectorValue *v2);
    void map(BoolValue *v1, BoolValue *v2);
    void map(BoolValue *v1, CharValue *v2);
    void map(BoolValue *v1, IntValue *v2);
    void map(BoolValue *v1, RealValue *v2);
    void map(BoolValue *v1, StringValue *v2);
    void map(BoolValue *v1, Value *v2);
    void map(Value *v1, BoolValue *v2);

    // binary maps for char value
    void map(CharValue *v1, BitValue *v2);
    void map(CharValue *v1, BitvectorValue *v2);
    void map(CharValue *v1, BoolValue *v2);
    void map(CharValue *v1, CharValue *v2);
    void map(CharValue *v1, IntValue *v2);
    void map(CharValue *v1, RealValue *v2);
    void map(CharValue *v1, StringValue *v2);

    // binary maps for int value
    void map(IntValue *v1, BitValue *v2);
    void map(IntValue *v1, BitvectorValue *v2);
    void map(IntValue *v1, BoolValue *v2);
    void map(IntValue *v1, CharValue *v2);
    void map(IntValue *v1, IntValue *v2);
    void map(IntValue *v1, RealValue *v2);
    void map(IntValue *v1, StringValue *v2);
    void map(IntValue *v1, TimeValue *v2);
    void map(IntValue *v1, Value *v2);
    void map(Value *v1, IntValue *v2);

    // binary maps for real value
    void map(RealValue *v1, BitValue *v2);
    void map(RealValue *v1, BitvectorValue *v2);
    void map(RealValue *v1, BoolValue *v2);
    void map(RealValue *v1, CharValue *v2);
    void map(RealValue *v1, IntValue *v2);
    void map(RealValue *v1, RealValue *v2);
    void map(RealValue *v1, StringValue *v2);
    void map(RealValue *v1, TimeValue *v2);
    void map(RealValue *v1, Value *v2);
    void map(Value *v1, RealValue *v2);

    // binary maps for text value
    void map(StringValue *v1, BitValue *v2);
    void map(StringValue *v1, BitvectorValue *v2);
    void map(StringValue *v1, BoolValue *v2);
    void map(StringValue *v1, CharValue *v2);
    void map(StringValue *v1, IntValue *v2);
    void map(StringValue *v1, RealValue *v2);
    void map(StringValue *v1, StringValue *v2);

    // binary maps for time value
    void map(TimeValue *v1, TimeValue *v2);
    void map(TimeValue *v1, IntValue *v2);
    void map(TimeValue *v1, RealValue *v2);

    // general
    void map(Value *v1);
    void map(Value *v1, Value *v2);

private:
    SimplifyMapData &_data;
    HifFactory _factory;

    /// @brief Return the operation type calling semantics function with the
    /// given values and the current operator.
    /// @param v1 The first operand.
    /// @param v2 The second operand.
    /// @return The type of operation.
    Type *_getOperationType(Value *v1, Value *v2);

    /// @brief Return the operation precision calling semantics function with the
    /// given values and the current operator.
    /// @param v1 The first operand.
    /// @param v2 The second operand.
    /// @return The precision of operation.
    Type *_getOperationPrecision(Value *v1, Value *v2);

    /// @brief This function resolve simplification of expression with values
    /// can be mapped into doubles. These type are: char, int, real.
    /// It store the result into <tt>data.result</tt> of current class.
    /// For example: 2 * 5 is translated as a constant with value 10.
    /// @param r1 The first double.
    /// @param r2 The second double.
    /// @param v1 The first value.
    /// @param v2 The second value.
    ///
    void _resolveRealExpr(double r1, double r2, Value *v1, Value *v2);

    /// @brief This function resolve simplification of expression with values
    /// can be mapped into bits. These type are: bool, bit.
    /// It store the result into <tt>data.result</tt> of current class.
    /// @param a The first bit value.
    /// @param b The second bit value.
    /// @param v1 The first value.
    /// @param v2 The second value.
    ///
    void _resolveBitExpr(BitConstant a, BitConstant b, Value *v1, Value *v2);

    /// @brief This function resolve simplification of expression where only
    /// the first value can be mapped into a double value.
    /// For example: 0 * var is translated as a constant with value 0.
    /// It store the result into <tt>data.result</tt> of current class.
    /// @param r1 The double value of first operator.
    /// @param v1 The first (and only) const value.
    /// @param v2 The second value.
    ///
    void _resolveConstRealExpr(double r1, Value *v1, Value *v2);

    /// @brief This function resolve simplification of expression where only
    /// the second value can be mapped into a double value.
    /// For example: var - 0 is translated as value "var"
    /// It store the result into <tt>data.result</tt> of current class.
    /// @param v1 The first value.
    /// @param r2 The double value of second operator.
    /// @param v2 The second (and only) const value.
    ///
    void _resolveConstRealExpr(Value *v1, double r2, Value *v2);

    /// @brief This method translate an array to corresponding integer in
    /// order to simplify some checks. This is not a conversion related to
    /// direct tree semplification.
    /// @param a The array.
    /// @param i The integer.
    ///
    void _translateBitvectorToInt(Type *a, Int *i);

    /// @brief Sets given result in data. If given result is a const value
    /// this method tries to convert it to the result type of the original operation.
    /// @param result The result to set.
    /// @param v1 The value1 of original expression.
    /// @param v2 The value2 of original expression.
    ///
    void _setConstValueResult(Value *result, Value *v1, Value *v2);

    SimplifyMap(const SimplifyMap &);
    SimplifyMap &operator=(const SimplifyMap &);
};

SimplifyMap::SimplifyMap(SimplifyMapData &d)
    : _data(d)
    , _factory(d.sem)
{
    // ntd
}

SimplifyMap::~SimplifyMap()
{
    // ntd
}

// //////////////////////////
// public methods
// //////////////////////////

Value *SimplifyMap::getResult() const { return _data.result; }

bool SimplifyMap::cannotBeSimplified() const { return _data.cannotBeSimplified; }

// //////////////////////////
// private utility methods
// //////////////////////////

Type *SimplifyMap::_getOperationType(Value *v1, Value *v2)
{
    Type *t1 = hif::semantics::getSemanticType(v1, _data.sem);
    Type *t2 = (v2 != nullptr) ? hif::semantics::getSemanticType(v2, _data.sem) : nullptr;
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo res =
        _data.sem->getExprType(t1, t2, _data.oper, _data.startingObject);
    Type *ret        = res.returnedType;
    res.returnedType = nullptr;

    // recoursive call is needed since type can have references to template
    // parameters that must be simplified according with simplify options.
    // For example from an instantiate in Verilog since operation precision
    // depends by the context. Ref design mios/Transceiver.
    Type *simplified = hif::manipulation::simplify(ret, _data.sem, _data.options);
    if (simplified != ret)
        delete ret;
    ret = simplified;

    Type *base = hif::semantics::getBaseType(ret, false, _data.sem);
    if (base != ret) {
        delete ret;
        ret = hif::copy(base);
    }

    return ret;
}

Type *SimplifyMap::_getOperationPrecision(Value *v1, Value *v2)
{
    Type *t1 = hif::semantics::getSemanticType(v1, _data.sem);
    Type *t2 = (v2 != nullptr) ? hif::semantics::getSemanticType(v2, _data.sem) : nullptr;
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo res =
        _data.sem->getExprType(t1, t2, _data.oper, _data.startingObject);
    Type *ret              = res.operationPrecision;
    res.operationPrecision = nullptr;

    // recoursive call is needed since type can have references to template
    // parameters that must be simplified according with simplify options.
    // For example from an instantiate in Verilog since operation precision
    // depends by the context. Ref design mios/Transceiver.
    Type *simplified = hif::manipulation::simplify(ret, _data.sem, _data.options);
    if (simplified != ret)
        delete ret;
    ret = simplified;

    Type *base = hif::semantics::getBaseType(ret, false, _data.sem);
    if (base != ret) {
        delete ret;
        ret = hif::copy(base);
    }

    return ret;
}

void SimplifyMap::_resolveRealExpr(double r1, double r2, Value *v1, Value *v2)
{
    bool only_integers = (dynamic_cast<IntValue *>(v1) != nullptr && dynamic_cast<IntValue *>(v2) != nullptr);

    if (hif::operatorIsArithmetic(_data.oper) || hif::operatorIsShift(_data.oper)) {
        double result = 0;
        if (_data.oper == op_plus)
            result = r1 + r2;
        else if (_data.oper == op_minus)
            result = r1 - r2;
        else if (_data.oper == op_mult)
            result = r1 * r2;
        else if (_data.oper == op_div) {
            if (r2 >= 0 && r2 <= 0)
                return;
            if (only_integers
                // behavior check: ref. design: vhdl/custom/generates
                && _data.options.behavior != SimplifyOptions::BEHAVIOR_AGGRESSIVE &&
                (static_cast<int64_t>(r1) % static_cast<int64_t>(r2)) != 0) {
                // This simplification may be unsafe in case division has reminder and
                // integers operands since the result would be rounded to nearest integer.
                // The round should be correct in case the original expression has the
                // integers divisions BUT is not correct in case the integer division
                // is created by tree rebalancing.
                _data.cannotBeSimplified = true;
                return;
            }

            result = r1 / r2;
        } else if (_data.oper == op_pow) {
            result = pow(r1, r2);
        } else if ((_data.oper == op_sll || _data.oper == op_sla) && only_integers) {
            IntValue *iv1 = dynamic_cast<IntValue *>(v1);
            messageAssert(iv1 != nullptr, "Unexpected non int value", v1, _data.sem);
            std::int64_t intResult = 0;

            if (static_cast<std::int64_t>(r2) >= 64LL) {
                intResult = 0;
            } else {
                intResult = iv1->getValue() << static_cast<std::int64_t>(r2);
            }

            IntValue *ivRes = new IntValue(intResult);
            _setConstValueResult(ivRes, v1, v2);
            return;
        } else if (_data.oper == op_srl && only_integers) {
            // Cast the first operand to an integer.
            IntValue *iv1 = static_cast<IntValue *>(v1);
            // Ensure iv1 is not null and is indeed an integer value.
            messageAssert(iv1 != nullptr, "Unexpected non int value", v1, _data.sem);

            std::int64_t intResult = 0;

            std::int64_t cv2 = static_cast<std::int64_t>(r2);
            if (cv2 >= 64LL) {
                intResult = 0;
            } else if (cv2 == 0 || iv1->getValue() == 0) {
                intResult = iv1->getValue();
            } else {
                std::int64_t cv1 = iv1->getValue();
                Type *type1      = iv1->getType();
                if (type1 == nullptr) {
                    type1 = _data.sem->getTypeForConstant(iv1);
                }
                // Get the span bitwidth of the type.
                auto trunc = hif::semantics::typeGetSpanBitwidth(type1, _data.sem);
                //
                auto mask  = (trunc >= 64) ? ~0ULL : ((1ULL << trunc) - 1);
                cv1        = cv1 & mask;
                intResult  = cv1 >> cv2;
            }

            IntValue *ivRes = new IntValue(intResult);
            _setConstValueResult(ivRes, v1, v2);
            return;
        } else if (_data.oper == op_sra && only_integers) {
            IntValue *iv1 = dynamic_cast<IntValue *>(v1);
            messageAssert(iv1 != nullptr, "Unexpected non int value", v1, _data.sem);
            std::int64_t intResult = 0;

            Type *t1            = hif::semantics::getSemanticType(v1, _data.sem);
            const bool isSigned = hif::typeIsSigned(t1, _data.sem);
            if (static_cast<std::int64_t>(r2) >= 64LL) {
                std::int64_t tmp = static_cast<std::int64_t>(r1);
                if (!isSigned)
                    intResult = 0;
                else if (tmp >= 0)
                    intResult = 0;
                else
                    intResult = -1;
            } else if (static_cast<std::int64_t>(r2) == 0 || iv1->getValue() == 0) {
                intResult = iv1->getValue();
            } else {
                if (isSigned) {
                    intResult = iv1->getValue() >> static_cast<std::int64_t>(r2);
                } else {
                    std::int64_t cv1 = iv1->getValue();
                    Type *type1      = iv1->getType();
                    if (type1 == nullptr) {
                        type1 = _data.sem->getTypeForConstant(iv1);
                    }
                    // Get the span bitwidth of the type.
                    auto trunc = hif::semantics::typeGetSpanBitwidth(type1, _data.sem);
                    auto mask  = (trunc >= 64) ? ~0ULL : ((1ULL << trunc) - 1);
                    cv1        = cv1 & mask;
                    intResult  = cv1 >> (static_cast<std::int64_t>(r2));
                }
            }

            IntValue *ivRes = new IntValue(intResult);
            _setConstValueResult(ivRes, v1, v2);
            return;
        } else if (_data.oper == op_log) {
            if (static_cast<std::int64_t>(r2) == 2ll)
                result = log2(r1);
            else if (static_cast<std::int64_t>(r2) == 10)
                result = log(r1);
            else
                return;
        } else if (_data.oper == op_rem && only_integers) {
            IntValue *iv1 = dynamic_cast<IntValue *>(v1);
            messageAssert(iv1 != nullptr, "Unexpected non int value", v1, _data.sem);
            std::int64_t intResult = iv1->getValue() % (static_cast<std::int64_t>(r2));

            IntValue *ivRes = new IntValue(intResult);
            _setConstValueResult(ivRes, v1, v2);
            return;
        } else if (_data.oper == op_mod && only_integers) {
            IntValue *iv1 = dynamic_cast<IntValue *>(v1);
            messageAssert(iv1 != nullptr, "Unexpected non int value", v1, _data.sem);

            std::int64_t a = iv1->getValue();
            std::int64_t n = static_cast<std::int64_t>(r2);
            std::int64_t r = 0;

            // implementation taken from hif_mod
            if (a >= 0 && n >= 0) {
                r = a % n;
            } else if (a < 0 && n < 0) {
                r = -((-a) % (-n));
            } else if (a < 0 && n >= 0) {
                r = ((n - ((-a) % (n))) % n);
            } else //if(a >= 0 && n < 0)
            {
                r = ((((a) % (-n)) + n) % n);
            }

            IntValue *ivRes = new IntValue(r);
            _setConstValueResult(ivRes, v1, v2);
            return;
        } else {
            // not allowed
            return;
        }

        RealValue *rVal = new RealValue();
        rVal->setValue(result);
        _setConstValueResult(rVal, v1, v2);
        return;
    } else if (hif::operatorIsRelational(_data.oper)) {
        bool result = false;
        if (_data.oper == op_eq || _data.oper == op_case_eq)
            result = (r1 >= r2 && r1 <= r2);
        else if (_data.oper == op_neq || _data.oper == op_case_neq)
            result = (r1 > r2 || r1 < r2);
        else if (_data.oper == op_gt)
            result = (r1 > r2);
        else if (_data.oper == op_lt)
            result = (r1 < r2);
        else if (_data.oper == op_ge)
            result = (r1 >= r2);
        else if (_data.oper == op_le)
            result = (r1 <= r2);
        else
            return;

        BoolValue *rVal = new BoolValue();
        rVal->setValue(result);
        _setConstValueResult(rVal, v1, v2);
        return;
    } else if (hif::operatorIsBitwise(_data.oper)) {
        IntValue *iv1 = dynamic_cast<IntValue *>(v1);
        messageAssert(iv1 != nullptr, "Unexpected non int value", v1, _data.sem);
        IntValue *iv2 = dynamic_cast<IntValue *>(v2);
        messageAssert(iv2 != nullptr, "Unexpected non int value", v2, _data.sem);
        std::int64_t result = 0;

        if (_data.oper == op_band && only_integers) {
            result = iv1->getValue() & iv2->getValue();
        } else if (_data.oper == op_bor && only_integers) {
            result = iv1->getValue() | iv2->getValue();
        } else if (_data.oper == op_bxor && only_integers) {
            result = iv1->getValue() ^ iv2->getValue();
        } else {
            return;
        }

        IntValue *ivRes = new IntValue(result);
        _setConstValueResult(ivRes, v1, v2);
        return;
    }
}

void SimplifyMap::_resolveBitExpr(BitConstant b1, BitConstant b2, Value *v1, Value *v2)
{
    if (!hif::operatorIsLogical(_data.oper) && !hif::operatorIsRelational(_data.oper) &&
        !hif::operatorIsBitwise(_data.oper)) {
        return;
    }

    // dont care not supported at the moment
    if (b1 == bit_dontcare || b2 == bit_dontcare) {
        return;
    }

    const bool a  = (b1 == bit_one || b1 == bit_h);
    const bool b  = (b2 == bit_one || b2 == bit_h);
    const bool ax = (b1 == bit_x || b1 == bit_u || b1 == bit_z || b1 == bit_w);
    const bool bx = (b2 == bit_x || b2 == bit_u || b2 == bit_z || b1 == bit_w);

    bool res     = false;
    bool unknown = false;
    if (_data.oper == op_and || _data.oper == op_band) {
        res     = a && b;
        unknown = (ax || bx) && (!a || !b);
    } else if (_data.oper == op_or || _data.oper == op_bor) {
        res     = a || b;
        unknown = (ax && !b) || (bx && !a);
    } else if (_data.oper == op_xor || _data.oper == op_bxor) {
        res     = a ^ b;
        unknown = ax || bx;
    } else if (_data.oper == op_eq) {
        res     = (a == b);
        unknown = ax || bx;
    } else if (_data.oper == op_case_eq) {
        res     = (b1 == b2);
        unknown = false;
    } else if (_data.oper == op_neq) {
        res     = (a != b);
        unknown = ax || bx;
    } else if (_data.oper == op_case_neq) {
        res     = (b1 != b2);
        unknown = false;
    } else if (_data.oper == op_le) {
        res     = a <= b;
        unknown = ax || bx;
    } else if (_data.oper == op_lt) {
        res     = a < b;
        unknown = ax || bx;
    } else if (_data.oper == op_ge) {
        res     = a >= b;
        unknown = ax || bx;
    } else if (_data.oper == op_gt) {
        res     = a > b;
        unknown = ax || bx;
    }

    BitValue *bb = new BitValue();
    if (unknown)
        bb->setValue(bit_x);
    else
        bb->setValue(res ? bit_one : bit_zero);
    _setConstValueResult(bb, v1, v2);
}

void SimplifyMap::_resolveConstRealExpr(double r1, Value *v1, Value *v2)
{
    Type *t2           = hif::semantics::getSemanticType(v2, _data.sem);
    bool only_integers = (dynamic_cast<Int *>(t2) && dynamic_cast<IntValue *>(v1));

    if (hif::operatorIsArithmetic(_data.oper) || hif::operatorIsShift(_data.oper)) {
        Value *result = nullptr;
        if (_data.oper == op_plus) {
            if (r1 >= 0 && r1 <= 0)
                result = hif::copy(v2);
        } else if (_data.oper == op_minus) {
            if (r1 >= 0 && r1 <= 0) {
                Expression *res = new Expression(op_minus, hif::copy(v2));
                result          = simplifyExpression(res, _data.sem, _data.options);
                if (result != res)
                    delete res;
            }
        } else if (_data.oper == op_mult) {
            if (r1 >= 0 && r1 <= 0)
                result = _factory.intval(0);
            else if (r1 >= 1 && r1 <= 1)
                result = hif::copy(v2);
            else if (r1 >= -1 && r1 <= -1) {
                Expression *res = new Expression(op_minus, hif::copy(v2));
                result          = simplifyExpression(res, _data.sem, _data.options);
                if (result != res)
                    delete res;
            }
        } else if (_data.oper == op_div) {
            if (r1 >= 0 && r1 <= 0)
                result = _factory.intval(0);
        } else if (_data.oper == op_pow) {
            if (r1 >= 0 && r1 <= 0)
                result = _factory.intval(0);
            else if (r1 >= 1 && r1 <= 1)
                result = _factory.intval(1);
        } else if (_data.oper == op_sll) {
            if (r1 >= 0 && r1 <= 0)
                result = _factory.intval(0);
        } else if (_data.oper == op_srl) {
            if (r1 >= 0 && r1 <= 0)
                result = _factory.intval(0);
        } else if (_data.oper == op_sla) {
            if (r1 >= 0 && r1 <= 0)
                result = _factory.intval(0);
        } else if (_data.oper == op_sra) {
            if (r1 >= 0 && r1 <= 0)
                result = _factory.intval(0);
            else if (r1 >= -1 && r1 <= -1)
                result = _factory.intval(-1);
        } else if (_data.oper == op_log) {
            if (r1 >= 1 && r1 <= 1)
                result = _factory.intval(0);
        } else {
            // not allowed
            return;
        }

        _setConstValueResult(result, v1, v2);
    } else if (hif::operatorIsBitwise(_data.oper)) {
        Value *res              = nullptr;
        Type *operandType       = hif::semantics::getBaseType(v2, false, _data.sem);
        const bool operandIsExr = (dynamic_cast<Expression *>(operandType) != nullptr);
        Expression *innerExpr   = operandIsExr ? static_cast<Expression *>(v2) : nullptr;
        const bool isIntOperand =
            (innerExpr != nullptr) ? (dynamic_cast<IntValue *>(innerExpr->getValue2()) != nullptr) : false;

        if (_data.oper == op_band && only_integers) {
            IntValue *iv1 = dynamic_cast<IntValue *>(v1);
            messageAssert(iv1 != nullptr, "Unexpected non int value", v1, _data.sem);

            if (iv1->getValue() <= 0 && iv1->getValue() >= 0) {
                res = new IntValue(0);
            } else if (iv1->getValue() == static_cast<std::int64_t>(-1)) {
                res = hif::copy(v2);
            } else if (iv1->getValue() == static_cast<std::int64_t>(1) && dynamic_cast<Cast *>(v2) != nullptr) {
                Cast *c2       = static_cast<Cast *>(v2);
                Value *castVal = c2->getValue();
                Type *valType  = hif::semantics::getBaseType(castVal, false, _data.sem);
                if (dynamic_cast<Bool *>(valType) == nullptr)
                    return;
                res = hif::copy(v2);
            } else if (isIntOperand) {
                IntValue *iv         = dynamic_cast<IntValue *>(innerExpr->getValue2());
                std::uint64_t orVal  = static_cast<std::uint64_t>(iv->getValue());
                std::uint64_t andVal = static_cast<std::uint64_t>(iv1->getValue());
                if (orVal >= andVal) {
                    res = _factory.expression(innerExpr->getValue1(), op_band, v1);
                }
            }
            _setConstValueResult(res, v1, v2);
        } else if (_data.oper == op_bor && only_integers) {
            IntValue *iv1 = dynamic_cast<IntValue *>(v1);
            messageAssert(iv1 != nullptr, "Unexpected non int value", v1, _data.sem);

            if (iv1->getValue() <= 0 && iv1->getValue() >= 0) {
                res = hif::copy(v2);
            } else if (iv1->getValue() == static_cast<std::int64_t>(-1)) {
                res = new IntValue(static_cast<std::int64_t>(-1));
            }
            _setConstValueResult(res, v1, v2);
        } else if (_data.oper == op_bxor && only_integers) {
            return;
        }

        if (res == nullptr)
            return;
        Type *rType = _getOperationType(v1, v2);
        messageAssert(rType != nullptr, "Cannot type original expression.", v2->getParent(), _data.sem);

        Cast *c;
        c = new Cast();
        c->setValue(_data.result);
        c->setType(rType);
        hif::manipulation::assureSyntacticType(c->getValue(), _data.sem);

        _data.result = c;
        return;
    } else if (hif::operatorIsRelational(_data.oper)) {
        ConstValue *res   = nullptr;
        const bool isZero = (r1 <= 0.0 && r1 >= 0.0);

        Type *prec            = _getOperationPrecision(v1, v2);
        const bool isUnsigned = !hif::typeIsSigned(prec, _data.sem);
        delete prec;

        Cast *c2            = dynamic_cast<Cast *>(v2);
        Type *castInnerType = (c2 == nullptr) ? nullptr : hif::semantics::getBaseType(c2->getValue(), false, _data.sem);
        const bool isCastOfBool = (dynamic_cast<Bool *>(castInnerType) != nullptr);
        const bool isOne        = (r1 <= 1.0 && r1 >= 1.0);
        const bool isPositive   = (r1 >= 0.0);

        bool isNegativeBitwise = false;
        Expression *expr2      = dynamic_cast<Expression *>(v2);
        if (expr2 != nullptr && expr2->getOperator() == op_bor &&
            dynamic_cast<IntValue *>(expr2->getValue2()) != nullptr) {
            IntValue *val = dynamic_cast<IntValue *>(expr2->getValue2());
            if (val->getValue() < 0)
                isNegativeBitwise = true;
        }

        if (_data.oper == op_lt && isZero && isUnsigned) {
            res = new BoolValue(false);
        } else if (_data.oper == op_ge && isZero && isUnsigned) {
            res = new BoolValue(true);
        } else if (isNegativeBitwise && isZero) {
            if (_data.oper == op_lt || _data.oper == op_le || _data.oper == op_neq || _data.oper == op_case_neq)
                res = _factory.boolval(true);
            else if (_data.oper == op_gt || _data.oper == op_ge || _data.oper == op_eq || _data.oper == op_case_eq)
                res = _factory.boolval(false);
            else
                messageError("Unexpected case", nullptr, nullptr);
        } else if (isCastOfBool) {
            Value *resCast = nullptr;
            if (isZero) {
                if (_data.oper == op_eq || _data.oper == op_case_eq) {
                    resCast = _factory.expression(op_not, hif::copy(c2->getValue()));
                } else if (_data.oper == op_neq || _data.oper == op_case_neq) {
                    resCast = hif::copy(c2->getValue());
                } else if (_data.oper == op_gt) {
                    resCast = hif::copy(c2->getValue());
                } else if (_data.oper == op_lt) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_ge) {
                    resCast = _factory.boolval(true);
                } else if (_data.oper == op_le) {
                    resCast = _factory.expression(op_not, hif::copy(c2->getValue()));
                } else {
                    messageError("Unexpected case", nullptr, nullptr);
                }
            } else if (isOne) {
                if (_data.oper == op_eq || _data.oper == op_case_eq) {
                    resCast = hif::copy(c2->getValue());
                } else if (_data.oper == op_neq || _data.oper == op_case_neq) {
                    resCast = _factory.expression(op_not, hif::copy(c2->getValue()));
                } else if (_data.oper == op_gt) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_lt) {
                    resCast = _factory.expression(op_not, hif::copy(c2->getValue()));
                } else if (_data.oper == op_ge) {
                    resCast = hif::copy(c2->getValue());
                } else if (_data.oper == op_le) {
                    resCast = _factory.boolval(true);
                } else {
                    messageError("Unexpected case", nullptr, nullptr);
                }
            } else if (isPositive) {
                if (_data.oper == op_eq || _data.oper == op_case_eq) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_neq || _data.oper == op_case_neq) {
                    resCast = _factory.boolval(true);
                } else if (_data.oper == op_gt) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_lt) {
                    resCast = _factory.boolval(true);
                } else if (_data.oper == op_ge) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_le) {
                    resCast = _factory.boolval(true);
                } else {
                    messageError("Unexpected case", nullptr, nullptr);
                }
            } else {
                if (_data.oper == op_eq || _data.oper == op_case_eq) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_neq || _data.oper == op_case_neq) {
                    resCast = _factory.boolval(true);
                } else if (_data.oper == op_gt) {
                    resCast = _factory.boolval(true);
                } else if (_data.oper == op_lt) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_ge) {
                    resCast = _factory.boolval(true);
                } else if (_data.oper == op_le) {
                    resCast = _factory.boolval(false);
                } else {
                    messageError("Unexpected case", nullptr, nullptr);
                }
            }

            Type *rType = _getOperationType(v1, v2);
            messageAssert(rType != nullptr, "Cannot type original expression.", v2->getParent(), _data.sem);
            Cast *c = new Cast();
            c->setValue(resCast);
            c->setType(rType);
            hif::manipulation::assureSyntacticType(c->getValue(), _data.sem);
            _data.result = c;
            return;
        }

        _setConstValueResult(res, v1, v2);
    }
}

void SimplifyMap::_resolveConstRealExpr(Value *v1, double r2, Value *v2)
{
    Type *t1           = hif::semantics::getSemanticType(v1, _data.sem);
    bool only_integers = (dynamic_cast<Int *>(t1) && dynamic_cast<IntValue *>(v2));

    if (hif::operatorIsArithmetic(_data.oper) || hif::operatorIsShift(_data.oper)) {
        Value *result = nullptr;

        std::uint64_t span = 0;
        if (t1 != nullptr) {
            span = hif::semantics::spanGetBitwidth(hif::typeGetSpan(t1, _data.sem), _data.sem);
        }

        Type *prec             = _getOperationPrecision(v1, v2);
        std::uint64_t precSpan = hif::semantics::spanGetBitwidth(hif::typeGetSpan(prec, _data.sem), _data.sem);
        delete prec;

        if (_data.oper == op_plus) {
            if (r2 >= 0 && r2 <= 0)
                result = hif::copy(v1);
        } else if (_data.oper == op_minus) {
            if (r2 >= 0 && r2 <= 0)
                result = hif::copy(v1);
        } else if (_data.oper == op_mult) {
            if (r2 >= 0 && r2 <= 0)
                result = _factory.intval(0);
            else if (r2 >= 1 && r2 <= 1)
                result = hif::copy(v1);
            else if (r2 >= -1 && r2 <= -1) {
                Expression *res = new Expression(op_minus, hif::copy(v1));
                result          = simplifyExpression(res, _data.sem, _data.options);
            }
        } else if (_data.oper == op_div) {
            if (r2 >= 1 && r2 <= 1)
                result = hif::copy(v1);
            else if (r2 >= -1 && r2 <= -1) {
                Expression *res = new Expression(op_minus, hif::copy(v1));
                result          = simplifyExpression(res, _data.sem, _data.options);
            }
        } else if (_data.oper == op_pow) {
            if (r2 >= 0 && r2 <= 0)
                result = _factory.intval(1);
            else if (r2 >= 1 && r2 <= 1)
                result = hif::copy(v1);
        } else if (_data.oper == op_sll) {
            if (r2 >= 0 && r2 <= 0)
                result = hif::copy(v1);
            else if (span != 0 && r2 >= double(span) && precSpan != 0 && r2 >= double(precSpan))
                result = _factory.intval(0);
        } else if (_data.oper == op_srl) {
            if (r2 >= 0 && r2 <= 0)
                result = hif::copy(v1);
            else if (span != 0 && r2 >= double(span))
                result = _factory.intval(0);
        } else if (_data.oper == op_sla) {
            if (r2 >= 0 && r2 <= 0)
                result = hif::copy(v1);
            else if (span != 0 && r2 >= double(span) && precSpan != 0 && r2 >= double(precSpan))
                result = _factory.intval(0);
        } else if (_data.oper == op_sra) {
            if (r2 >= 0 && r2 <= 0)
                result = hif::copy(v1);
        } else {
            // not allowed
            return;
        }

        _setConstValueResult(result, v1, v2);
    } else if (hif::operatorIsBitwise(_data.oper)) {
        Value *res              = nullptr;
        Type *operandType       = hif::semantics::getBaseType(v1, false, _data.sem);
        const bool operandIsExr = (dynamic_cast<Expression *>(operandType) != nullptr);
        Expression *innerExpr   = operandIsExr ? static_cast<Expression *>(v1) : nullptr;
        const bool isIntOperand =
            (innerExpr != nullptr) ? (dynamic_cast<IntValue *>(innerExpr->getValue2()) != nullptr) : false;
        if (_data.oper == op_band && only_integers) {
            IntValue *iv2 = dynamic_cast<IntValue *>(v2);
            messageAssert(iv2 != nullptr, "Unexpected non int value", v2, _data.sem);

            if (iv2->getValue() <= 0 && iv2->getValue() >= 0) {
                res = new IntValue(0);
            } else if (iv2->getValue() == static_cast<std::int64_t>(-1)) {
                res = hif::copy(v1);
            } else if (iv2->getValue() == static_cast<std::int64_t>(1) && dynamic_cast<Cast *>(v1) != nullptr) {
                Cast *c1       = static_cast<Cast *>(v1);
                Value *castVal = c1->getValue();
                Type *valType  = hif::semantics::getBaseType(castVal, false, _data.sem);
                if (dynamic_cast<Bool *>(valType) == nullptr)
                    return;
                res = hif::copy(v1);
            } else if (isIntOperand) {
                IntValue *iv         = dynamic_cast<IntValue *>(innerExpr->getValue2());
                std::uint64_t orVal  = static_cast<std::uint64_t>(iv->getValue());
                std::uint64_t andVal = static_cast<std::uint64_t>(iv2->getValue());
                if (orVal >= andVal) {
                    res = _factory.expression(innerExpr->getValue1(), op_band, v2);
                }
            }
            _setConstValueResult(res, v1, v2);
        } else if (_data.oper == op_bor && only_integers) {
            IntValue *iv2 = dynamic_cast<IntValue *>(v2);
            messageAssert(iv2 != nullptr, "Unexpected non int value", v2, _data.sem);

            if (iv2->getValue() <= 0 && iv2->getValue() >= 0) {
                res = hif::copy(v1);
            } else if (iv2->getValue() == static_cast<std::int64_t>(-1)) {
                res = new IntValue(static_cast<std::int64_t>(-1));
            }
            _setConstValueResult(res, v1, v2);
        } else if (_data.oper == op_bxor && only_integers) {
            return;
        }

        if (res == nullptr)
            return;
        Type *rType = _getOperationType(v1, v2);
        messageAssert(rType != nullptr, "Cannot type original expression.", v1->getParent(), _data.sem);

        Cast *c;
        c = new Cast();
        c->setValue(_data.result);
        c->setType(rType);
        hif::manipulation::assureSyntacticType(c->getValue(), _data.sem);

        _data.result = c;
        return;
    } else if (hif::operatorIsRelational(_data.oper)) {
        ConstValue *res   = nullptr;
        const bool isZero = (r2 <= 0.0 && r2 >= 0.0);

        Type *prec            = _getOperationPrecision(v1, v2);
        const bool isUnsigned = !hif::typeIsSigned(prec, _data.sem);
        delete prec;

        Cast *c1            = dynamic_cast<Cast *>(v1);
        Type *castInnerType = (c1 == nullptr) ? nullptr : hif::semantics::getBaseType(c1->getValue(), false, _data.sem);
        const bool isCastOfBool = (dynamic_cast<Bool *>(castInnerType) != nullptr);
        const bool isOne        = (r2 <= 1.0 && r2 >= 1.0);
        const bool isPositive   = (r2 >= 0.0);

        bool isNegativeBitwise = false;
        Expression *expr1      = dynamic_cast<Expression *>(v1);
        if (expr1 != nullptr && expr1->getOperator() == op_bor &&
            dynamic_cast<IntValue *>(expr1->getValue2()) != nullptr) {
            IntValue *val = dynamic_cast<IntValue *>(expr1->getValue2());
            if (val->getValue() < 0)
                isNegativeBitwise = true;
        }

        if (_data.oper == op_lt && isZero && isUnsigned) {
            res = new BoolValue(false);
        } else if (_data.oper == op_ge && isZero && isUnsigned) {
            res = new BoolValue(true);
        } else if (isNegativeBitwise && isZero) {
            if (_data.oper == op_lt || _data.oper == op_le || _data.oper == op_neq || _data.oper == op_case_neq)
                res = _factory.boolval(true);
            else if (_data.oper == op_gt || _data.oper == op_ge || _data.oper == op_eq || _data.oper == op_case_eq)
                res = _factory.boolval(false);
            else
                messageError("Unexpected case", nullptr, nullptr);
        } else if (isCastOfBool) {
            Value *resCast = nullptr;
            if (isZero) {
                if (_data.oper == op_eq || _data.oper == op_case_eq) {
                    resCast = _factory.expression(op_not, hif::copy(c1->getValue()));
                } else if (_data.oper == op_neq || _data.oper == op_case_neq) {
                    resCast = hif::copy(c1->getValue());
                } else if (_data.oper == op_gt) {
                    resCast = hif::copy(c1->getValue());
                } else if (_data.oper == op_lt) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_ge) {
                    resCast = _factory.boolval(true);
                } else if (_data.oper == op_le) {
                    resCast = _factory.expression(op_not, hif::copy(c1->getValue()));
                } else {
                    messageError("Unexpected case", nullptr, nullptr);
                }
            } else if (isOne) {
                if (_data.oper == op_eq || _data.oper == op_case_eq) {
                    resCast = hif::copy(c1->getValue());
                } else if (_data.oper == op_neq || _data.oper == op_case_neq) {
                    resCast = _factory.expression(op_not, hif::copy(c1->getValue()));
                } else if (_data.oper == op_gt) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_lt) {
                    resCast = _factory.expression(op_not, hif::copy(c1->getValue()));
                } else if (_data.oper == op_ge) {
                    resCast = hif::copy(c1->getValue());
                } else if (_data.oper == op_le) {
                    resCast = _factory.boolval(true);
                } else {
                    messageError("Unexpected case", nullptr, nullptr);
                }
            } else if (isPositive) {
                if (_data.oper == op_eq || _data.oper == op_case_eq) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_neq || _data.oper == op_case_neq) {
                    resCast = _factory.boolval(true);
                } else if (_data.oper == op_gt) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_lt) {
                    resCast = _factory.boolval(true);
                } else if (_data.oper == op_ge) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_le) {
                    resCast = _factory.boolval(true);
                } else {
                    messageError("Unexpected case", nullptr, nullptr);
                }
            } else {
                if (_data.oper == op_eq || _data.oper == op_case_eq) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_neq || _data.oper == op_case_neq) {
                    resCast = _factory.boolval(true);
                } else if (_data.oper == op_gt) {
                    resCast = _factory.boolval(true);
                } else if (_data.oper == op_lt) {
                    resCast = _factory.boolval(false);
                } else if (_data.oper == op_ge) {
                    resCast = _factory.boolval(true);
                } else if (_data.oper == op_le) {
                    resCast = _factory.boolval(false);
                } else {
                    messageError("Unexpected case", nullptr, nullptr);
                }
            }

            Type *rType = _getOperationType(v1, v2);
            messageAssert(rType != nullptr, "Cannot type original expression.", v2->getParent(), _data.sem);
            Cast *c = new Cast();
            c->setValue(resCast);
            c->setType(rType);
            hif::manipulation::assureSyntacticType(c->getValue(), _data.sem);
            _data.result = c;
            return;
        }

        _setConstValueResult(res, v1, v2);
    }
}
void SimplifyMap::_translateBitvectorToInt(Type *a, Int *i)
{
    // Here we just want to convert the representation from any kind of value
    // to an IntValue. Therefore, we MUST always use the HIF semantics,
    // since the actual semantics could not have a Int type (e.g. Verilog).
    // After conversion, we roll back the syntactic type to the original type.
    hif::HifFactory f;
    f.setSemantics(hif::semantics::HIFSemantics::getInstance());
    Int *ii = f.integer(hif::copy(hif::typeGetSpan(a, _data.sem)), hif::typeIsSigned(a, _data.sem), true);
    i->setSpan(ii->getSpan());
    i->setConstexpr(ii->isConstexpr());
    i->setSigned(ii->isSigned());
    ii->setSpan(nullptr);
    delete ii;
}

void SimplifyMap::_setConstValueResult(Value *result, Value *v1, Value *v2)
{
    _data.result   = nullptr;
    ConstValue *cv = dynamic_cast<ConstValue *>(result);
    if (cv == nullptr) {
        _data.result = result;
        return;
    }

    Type *rType = _getOperationType(v1, v2);
    messageAssert(rType != nullptr, "Cannot type original expr", v1->getParent(), _data.sem);
    ConstValue *rtVal = transformConstant(cv, rType, _data.sem);
    delete cv;
    delete rType;
    _data.result = rtVal;
}

// //////////////////////////
// Maps implementation
// //////////////////////////

void SimplifyMap::map(Object * /* v1 */)
{
    // ntd
}

void SimplifyMap::map(Object * /*v1*/, Object * /*v2*/)
{
    // ntd
}

void SimplifyMap::map(BitValue *v1)
{
    // operations: op_bnot, op_not, reduce
    if (_data.oper == op_not || _data.oper == op_bnot) {
        BitValue *result = hif::copy(v1);
        BitConstant res  = bit_x;
        switch (v1->getValue()) {
        case bit_u:
            res = bit_u;
            break;
        case bit_x:
            res = bit_x;
            break;
        case bit_zero:
            res = bit_one;
            break;
        case bit_one:
            res = bit_zero;
            break;
        case bit_z:
            res = bit_x;
            break;
        case bit_w:
            res = bit_w;
            break;
        case bit_l:
            res = bit_h;
            break;
        case bit_h:
            res = bit_l;
            break;
        case bit_dontcare:
            res = bit_dontcare;
            break;
        default:
            break;
        }
        result->setValue(res);
        _data.result = result;
    }

    if (hif::operatorIsReduce(_data.oper)) {
        _data.result = hif::copy(v1);
    }

    _setConstValueResult(_data.result, v1, nullptr);
}

void SimplifyMap::map(BitvectorValue *v1)
{
    // operations: op_bnot, op_not, reduce
    if (_data.oper == op_bnot) {
        BitvectorValue *result = hif::copy(v1);
        std::string s(v1->getValue());
        for (std::string::size_type i = 0; i < s.length(); ++i) {
            switch (s[i]) {
            case 'U':
                s[i] = 'U';
                break;
            case 'X':
                s[i] = 'X';
                break;
            case '0':
                s[i] = '1';
                break;
            case '1':
                s[i] = '0';
                break;
            case 'Z':
                s[i] = 'X';
                break;
            case 'W':
                s[i] = 'W';
                break;
            case 'L':
                s[i] = 'H';
                break;
            case 'H':
                s[i] = 'L';
                break;
            case '-':
                s[i] = '-';
                break;
            default:
                messageError("Unexpected case.", v1, _data.sem);
            }
        }

        result->setValue(s);
        _data.result = result;
    }

    if (_data.oper == op_not) {
        // TODO
        return;
    }

    if (_data.oper == op_andrd || _data.oper == op_orrd || _data.oper == op_xorrd) {
        // TODO
        return;
    }

    _setConstValueResult(_data.result, v1, nullptr);
}

void SimplifyMap::map(BoolValue *v1)
{
    // operations: op_not
    if (_data.oper == op_not) {
        BoolValue *rVal = new BoolValue();
        rVal->setValue(!v1->getValue());
        _data.result = rVal;
    }

    if (hif::operatorIsReduce(_data.oper)) {
        _data.result = hif::copy(v1);
    }

    _setConstValueResult(_data.result, v1, nullptr);
}

void SimplifyMap::map(CharValue *v1)
{
    // operations: op_minus, op_plus, op_not
    if (_data.oper == op_plus) {
        _data.result = hif::copy(v1);
    }

    if (_data.oper == op_minus) {
        char value     = v1->getValue();
        IntValue *rVal = new IntValue();
        rVal->setValue(-value);
        _data.result = rVal;
    }

    if (_data.oper == op_not) {
        char value     = v1->getValue();
        IntValue *rVal = new IntValue();
        rVal->setValue((value == 0));
        _data.result = rVal;
    }

    _setConstValueResult(_data.result, v1, nullptr);
}

void SimplifyMap::map(IntValue *v1)
{
    // operations: op_minus, op_plus, op_not
    if (_data.oper == op_plus) {
        _data.result = hif::copy(v1);
    } else if (_data.oper == op_minus) {
        std::int64_t value = v1->getValue();
        IntValue *rVal     = new IntValue();
        rVal->setValue(-value);
        _data.result = rVal;
    } else if (_data.oper == op_not) {
        std::int64_t value = v1->getValue();
        IntValue *rVal     = new IntValue();
        rVal->setValue((value == 0));
        _data.result = rVal;
    } else if (_data.oper == op_bnot) {
        std::int64_t value = v1->getValue();
        IntValue *rVal     = new IntValue();
        rVal->setValue(~value);
        _data.result = rVal;
    }

    _setConstValueResult(_data.result, v1, nullptr);
}

void SimplifyMap::map(RealValue *v1)
{
    // operations: op_minus, op_plus, op_not
    if (_data.oper == op_plus) {
        _data.result = hif::copy(v1);
    }

    if (_data.oper == op_minus) {
        double value    = v1->getValue();
        RealValue *rVal = new RealValue();
        rVal->setValue(-value);
        _data.result = rVal;
    }

    if (_data.oper == op_not) {
        double value    = v1->getValue();
        RealValue *rVal = new RealValue();
        rVal->setValue((value >= 0 && value <= 0));
        _data.result = rVal;
    }

    _setConstValueResult(_data.result, v1, nullptr);
}

void SimplifyMap::map(StringValue * /*v1*/)
{
    // no operations
}

void SimplifyMap::map(Expression *v1)
{
    Value *result = nullptr;
    if (v1->getValue2() != nullptr) {
        if (_data.oper == op_minus && v1->getOperator() == op_minus) {
            // -(a - b)  --> (b - a)
            Expression *res = new Expression(op_minus, hif::copy(v1->getValue2()), hif::copy(v1->getValue1()));

            result = simplifyExpression(res, _data.sem, _data.options);
        } else if (_data.oper == op_not && hif::operatorIsRelational(v1->getOperator())) {
            // ! (a < b) --> a >= b, etc.
            const hif::Operator op = hif::operatorGetInverse(v1->getOperator());
            if (op != hif::op_none) {
                result = new Expression(op, hif::copy(v1->getValue1()), hif::copy(v1->getValue2()));
            }
        }
    } else {
        if (_data.oper == op_minus && v1->getOperator() == op_minus) {
            // -(- a) --> a
            result = hif::copy(v1->getValue1());
        } else if (_data.oper == op_minus && v1->getOperator() == op_plus) {
            // -(+ a) --> - a
            result = new Expression(op_minus, hif::copy(v1->getValue1()));
        } else if (_data.oper == op_plus && v1->getOperator() == op_minus) {
            // +(- a) --> - a
            result = new Expression(op_minus, hif::copy(v1->getValue1()));
        } else if (_data.oper == op_plus && v1->getOperator() == op_plus) {
            // +(+ a) --> a
            result = hif::copy(v1->getValue1());
        }
    }

    _data.result = result;
}

void SimplifyMap::map(BitValue *v1, BitValue *v2)
{
    if (_data.oper == op_concat) {
        // TODO: not already supported
        return;
    }

    // operations: logical, logical bitwise, op_eq, op_neq
    _resolveBitExpr(v1->getValue(), v2->getValue(), v1, v2);
}

void SimplifyMap::map(BitValue *v1, BitvectorValue *v2)
{
    BitvectorValue bvv(v1->toString());
    Bitvector *bv;
    bv = new Bitvector();
    bvv.setType(bv);
    bv->setConstexpr(hif::typeIsConstexpr(v1->getType(), _data.sem));
    bv->setLogic(hif::typeIsLogic(v1->getType(), _data.sem));
    bv->setResolved(hif::typeIsConstexpr(v1->getType(), _data.sem));
    bv->setSigned(hif::typeIsSigned(v2->getType(), _data.sem));
    bv->setSpan(new Range(0, 0));

    map(&bvv, v2);
}

void SimplifyMap::map(BitValue *v1, BoolValue *v2)
{
    BitConstant a = v2->getValue() ? bit_one : bit_zero;
    BitValue b(a);
    b.setType(hif::copy(v1->getType()));

    map(v1, &b);
}

void SimplifyMap::map(BitValue * /*v1*/, CharValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(BitValue * /*v1*/, IntValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(BitValue * /*v1*/, RealValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(BitValue * /*v1*/, StringValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(BitValue *v1, Value *v2)
{
    if (!hif::operatorIsBitwise(_data.oper) && !hif::operatorIsLogical(_data.oper))
        return;

    const BitConstant bitval = v1->getValue();
    const bool val           = (bitval == bit_h) || (bitval == bit_one);
    const bool notVal        = (bitval == bit_l) || (bitval == bit_zero);
    if (_data.oper == op_band || _data.oper == op_and) {
        if (val)
            _data.result = hif::copy(v2);
        else if (notVal)
            _data.result = hif::copy(v1);
    } else if (_data.oper == op_bor || _data.oper == op_or) {
        if (notVal)
            _data.result = hif::copy(v2);
        else if (val)
            _data.result = hif::copy(v1);
    } else if (_data.oper == op_bxor || _data.oper == op_xor) {
        if (notVal)
            _data.result = hif::copy(v2);
        else if (val)
            _data.result = new Expression(op_not, hif::copy(v2));
        else // X
        {
            BitValue *b = hif::copy(v1);
            b->setValue(bit_x);
            _data.result = b;
        }
    }
}

void SimplifyMap::map(Value *v1, BitValue *v2)
{
    // Cannot simplify logical operators, since v1 must be always be evaluated.
    if (!hif::operatorIsCommutative(_data.oper))
        return;
    map(v2, v1);
}

void SimplifyMap::map(BitvectorValue *v1, BitValue *v2)
{
    BitvectorValue bvv(v2->toString());
    Bitvector *bv;
    bv = new Bitvector();
    bvv.setType(bv);
    bv->setConstexpr(hif::typeIsConstexpr(v2->getType(), _data.sem));
    bv->setLogic(hif::typeIsLogic(v2->getType(), _data.sem));
    bv->setResolved(hif::typeIsResolved(v2->getType(), _data.sem));
    bv->setSigned(hif::typeIsSigned(v1->getType(), _data.sem));
    bv->setSpan(new Range(0, 0));

    map(v1, &bvv);
}

void SimplifyMap::map(BitvectorValue *v1, BitvectorValue *v2)
{
    // arithmetic
    if (hif::operatorIsArithmetic(_data.oper) || hif::operatorIsBitwise(_data.oper) ||
        hif::operatorIsRelational(_data.oper) || hif::operatorIsShift(_data.oper)) {
        Int tint1, tint2;
        Type *t1  = hif::semantics::getBaseType(hif::semantics::getSemanticType(v1, _data.sem), false, _data.sem);
        Type *t2  = hif::semantics::getBaseType(hif::semantics::getSemanticType(v2, _data.sem), false, _data.sem);
        Array *a1 = dynamic_cast<Array *>(t1);
        Array *a2 = dynamic_cast<Array *>(t2);

        if (a1 != nullptr || a2 != nullptr)
            return;
        if (!v1->is01() || !v2->is01()) {
            if (_data.oper == op_eq || _data.oper == op_neq || _data.oper == op_case_eq || _data.oper == op_case_neq) {
                BitValue *bv    = new BitValue();
                const bool isEq = v1->getValue() == v2->getValue();
                if (_data.oper == op_eq)
                    bv->setValue(bit_x);
                else if (_data.oper == op_neq)
                    bv->setValue(bit_x);
                else if (_data.oper == op_case_eq)
                    bv->setValue(isEq ? bit_one : bit_zero);
                else
                    bv->setValue(!isEq ? bit_one : bit_zero);

                _data.result = bv;
            } else if (hif::operatorIsBitwise(_data.oper)) {
                std::string s1 = v1->getValue();
                std::string s2 = v2->getValue();
                std::string ret;
                if (s1.size() != s2.size())
                    return;

                if (_data.oper == op_band) {
                    for (std::string::size_type i = 0U; i < s1.size(); ++i) {
                        char a = s1[i];
                        char b = s2[i];
                        if (a == '0' || b == '0' || a == 'L' || b == 'L')
                            ret += '0';
                        else if ((a == '1' || a == 'H') && (b == '1' || b == 'H'))
                            ret += '1';
                        else
                            ret += 'X';
                    }
                } else if (_data.oper == op_bor) {
                    for (std::string::size_type i = 0U; i < s1.size(); ++i) {
                        char a = s1[i];
                        char b = s2[i];
                        if (a == '1' || b == '1' || a == 'H' || b == 'H')
                            ret += '1';
                        else if ((a == '0' || a == 'L') && (b == '0' || b == 'L'))
                            ret += '0';
                        else
                            ret += 'X';
                    }
                } else if (_data.oper == op_bxor) {
                    for (std::string::size_type i = 0U; i < s1.size(); ++i) {
                        char a = s1[i];
                        char b = s2[i];
                        if ((a == '1' || a == 'H') && (b == '0' || b == 'L'))
                            ret += '1';
                        else if ((a == '0' || a == 'L') && (b == '1' || b == 'H'))
                            ret += '1';
                        else if ((a == '0' || a == 'L') && (b == '0' || b == 'L'))
                            ret += '0';
                        else if ((a == '1' || a == 'H') && (b == '1' || b == 'H'))
                            ret += '0';
                        else
                            ret += 'X';
                    }
                } else {
                    return;
                }

                BitvectorValue *val = new BitvectorValue(ret);
                val->setType(_data.sem->getTypeForConstant(val));
                _data.result = val;
            } else {
                return;
            }
        } else {
            _translateBitvectorToInt(t1, &tint1);
            _translateBitvectorToInt(t2, &tint2);

            IntValue *i1 = dynamic_cast<IntValue *>(transformConstant(v1, &tint1, _data.sem));
            IntValue *i2 = dynamic_cast<IntValue *>(transformConstant(v2, &tint2, _data.sem));

            if (i1 == nullptr || i2 == nullptr)
                return;
            delete i1->setSemanticType(hif::copy(v1->getSemanticType()));
            delete i2->setSemanticType(hif::copy(v2->getSemanticType()));
            map(i1, i2);
            delete i1;
            delete i2;
        }
        _setConstValueResult(_data.result, v1, v2);
    } else if (_data.oper == op_concat) {
        BitvectorValue *val = new BitvectorValue(v1->getValue() + v2->getValue());
        val->setType(_data.sem->getTypeForConstant(val));

        _setConstValueResult(val, v1, v2);
    } else {
        // operations: logical,
        // TODO
    }
}

void SimplifyMap::map(BitvectorValue *v1, BoolValue *v2)
{
    BitConstant a = v2->getValue() ? bit_one : bit_zero;
    BitValue b(a);
    b.setType(hif::copy(v1->getType()));

    map(v1, &b);
}

void SimplifyMap::map(BitvectorValue * /*v1*/, CharValue * /*v2*/)
{
    // operations: shift
    // TODO
}

void SimplifyMap::map(BitvectorValue *v1, IntValue *v2)
{
    // arithmetical
    if (hif::operatorIsArithmetic(_data.oper) || hif::operatorIsRelational(_data.oper)) {
        Int tint1;

        Bitvector *a1 = dynamic_cast<Bitvector *>(hif::semantics::getSemanticType(v1, _data.sem));
        if (a1 == nullptr)
            return;

        _translateBitvectorToInt(a1, &tint1);

        IntValue *i1 = dynamic_cast<IntValue *>(transformConstant(v1, &tint1, _data.sem));

        if (i1 == nullptr)
            return;
        delete i1->setSemanticType(hif::copy(v1->getSemanticType()));

        map(i1, v2);
        delete i1;

        _setConstValueResult(_data.result, v1, v2);
    }
    // shift
    else if (hif::operatorIsShift(_data.oper)) {
        std::string s1(v1->getValue());
        messageAssert(v2->getValue() >= 0LL, "Unexpected shift amount", nullptr, nullptr);
        std::string::size_type i2 = static_cast<std::string::size_type>(v2->getValue());
        BitvectorValue *bbv       = hif::copy(v1);
        if (_data.oper == op_sll || _data.oper == op_sla) {
            std::string filler(i2, '0');
            std::string result = (s1 + filler).substr(i2);
            bbv->setValue(result);
        } else if (_data.oper == op_srl) {
            std::string filler(i2, '0');
            std::string result = (filler + s1).substr(0, s1.size());
            bbv->setValue(result);
        } else if (_data.oper == op_sra) {
            char sign;
            Type *t1 = hif::semantics::getSemanticType(v1, _data.sem);
            if (t1 == nullptr)
                return;
            const bool isSigned = hif::typeIsSigned(t1, _data.sem);
            // Sign extension
            if (!isSigned)
                sign = '0';
            else
                sign = s1[0];
            std::string filler(i2, sign);
            std::string result = (filler + s1).substr(0, s1.size());
            bbv->setValue(result);
        } else {
            messageError("Unexpected case", nullptr, nullptr);
        }

        _setConstValueResult(bbv, v1, v2);
    }
}

void SimplifyMap::map(BitvectorValue *v1, RealValue *v2)
{
    if (hif::operatorIsArithmetic(_data.oper)) {
        Int tint1;
        Type *a1 = hif::semantics::getSemanticType(v1, _data.sem);
        if (a1 == nullptr)
            return;

        _translateBitvectorToInt(a1, &tint1);

        IntValue *i1 = dynamic_cast<IntValue *>(transformConstant(v1, &tint1, _data.sem));

        if (i1 == nullptr)
            return;
        delete i1->setSemanticType(hif::copy(v1->getSemanticType()));
        map(i1, v2);
        delete i1;

        _setConstValueResult(_data.result, v1, v2);
    } else {
        // operations: shift
        // TODO
    }
}

void SimplifyMap::map(BitvectorValue * /*v1*/, StringValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(BitvectorValue *v1, Value *v2)
{
    // arithmetical
    if (hif::operatorIsArithmetic(_data.oper)) {
        Int tint1;
        Type *a1 = hif::semantics::getSemanticType(v1, _data.sem);
        if (a1 == nullptr)
            return;

        _translateBitvectorToInt(a1, &tint1);

        IntValue *i1 = dynamic_cast<IntValue *>(transformConstant(v1, &tint1, _data.sem));

        if (i1 == nullptr)
            return;
        delete i1->setSemanticType(hif::copy(v1->getSemanticType()));
        map(i1, v2);
        delete i1;

        _setConstValueResult(_data.result, v1, v2);
    } else {
        // operations: logical, logical bitwise, op_eq, op_neq,
        // concat
        // TODO
    }
}

void SimplifyMap::map(Value *v1, BitvectorValue *v2)
{
    // arithmetical
    if (hif::operatorIsArithmetic(_data.oper)) {
        Int tint2;
        Type *a2 = hif::semantics::getSemanticType(v1, _data.sem);
        if (a2 == nullptr)
            return;

        _translateBitvectorToInt(a2, &tint2);

        IntValue *i2 = dynamic_cast<IntValue *>(transformConstant(v2, &tint2, _data.sem));

        if (i2 == nullptr)
            return;
        delete i2->setSemanticType(hif::copy(v2->getSemanticType()));
        map(v1, i2);
        delete i2;

        _setConstValueResult(_data.result, v1, v2);
    } else {
        // operations: logical, logical bitwise, op_eq, op_neq,
        // concat
        // TODO
    }
}

void SimplifyMap::map(BoolValue *v1, BitValue *v2)
{
    BitConstant a = v1->getValue() ? bit_one : bit_zero;
    BitValue b(a);
    b.setType(hif::copy(v2->getType()));

    map(&b, v2);
}

void SimplifyMap::map(BoolValue *v1, BitvectorValue *v2)
{
    BitConstant a = v1->getValue() ? bit_one : bit_zero;
    BitValue b(a);
    b.setType(hif::copy(v2->getType()));

    map(&b, v2);
}

void SimplifyMap::map(BoolValue *v1, BoolValue *v2)
{
    // operations: logical, logical bitwise, op_eq, op_neq, relational
    BitConstant a = v1->getValue() ? bit_one : bit_zero;
    BitConstant b = v2->getValue() ? bit_one : bit_zero;

    _resolveBitExpr(a, b, v1, v2);
}

void SimplifyMap::map(BoolValue * /*v1*/, CharValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(BoolValue * /*v1*/, IntValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(BoolValue * /*v1*/, RealValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(BoolValue * /*v1*/, StringValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(BoolValue *v1, Value *v2)
{
    if (!hif::operatorIsLogical(_data.oper))
        return;

    const bool val = v1->getValue();
    if (_data.oper == op_and) {
        if (val)
            _data.result = hif::copy(v2);
        else
            _data.result = hif::copy(v1);
    } else if (_data.oper == op_or) {
        if (!val)
            _data.result = hif::copy(v2);
        else
            _data.result = hif::copy(v1);
    } else if (_data.oper == op_xor) {
        if (!val)
            _data.result = hif::copy(v2);
        else
            _data.result = new Expression(op_not, hif::copy(v2));
    }
}

void SimplifyMap::map(Value *v1, BoolValue *v2)
{
    if (!hif::operatorIsLogical(_data.oper))
        return;

    const bool val = v2->getValue();
    if (_data.oper == op_and) {
        if (val)
            _data.result = hif::copy(v1);
        else
            _data.result = hif::copy(v2);
    } else if (_data.oper == op_or) {
        if (!val)
            _data.result = hif::copy(v1);
        else
            _data.result = hif::copy(v2);
    } else if (_data.oper == op_xor) {
        if (!val)
            _data.result = hif::copy(v1);
        else
            _data.result = new Expression(op_not, hif::copy(v1));
    }
}

void SimplifyMap::map(CharValue * /*v1*/, BitValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(CharValue * /*v1*/, BitvectorValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(CharValue * /*v1*/, BoolValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(CharValue *v1, CharValue *v2)
{
    // operations: arithmetic, relational
    double r1, r2;
    r1 = static_cast<double>(v1->getValue());
    r2 = static_cast<double>(v2->getValue());

    _resolveRealExpr(r1, r2, v1, v2);
}

void SimplifyMap::map(CharValue *v1, IntValue *v2)
{
    // operations: arithmetic, relational,
    double r1, r2;
    r1 = static_cast<double>(v1->getValue());
    r2 = static_cast<double>(v2->getValue());

    _resolveRealExpr(r1, r2, v1, v2);
}

void SimplifyMap::map(CharValue *v1, RealValue *v2)
{
    // operations: arithmetic, relational,
    double r1, r2;
    r1 = static_cast<double>(v1->getValue());
    r2 = v2->getValue();

    _resolveRealExpr(r1, r2, v1, v2);
}

void SimplifyMap::map(CharValue * /*v1*/, StringValue * /*v2*/)
{
    // operations: concat
    // TODO
}
void SimplifyMap::map(IntValue * /*v1*/, BitValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(IntValue *v1, BitvectorValue *v2)
{
    // arithmetical
    if (hif::operatorIsArithmetic(_data.oper) || hif::operatorIsRelational(_data.oper) ||
        hif::operatorIsShift(_data.oper)) {
        Int tint2;
        Type *a2 = hif::semantics::getSemanticType(v1, _data.sem);
        if (a2 == nullptr)
            return;

        _translateBitvectorToInt(a2, &tint2);

        IntValue *i2 = dynamic_cast<IntValue *>(transformConstant(v2, &tint2, _data.sem));

        if (i2 == nullptr)
            return;
        delete i2->setSemanticType(hif::copy(v2->getSemanticType()));

        map(v1, i2);
        delete i2;

        _setConstValueResult(_data.result, v1, v2);
    } else {
        // TODO: no other operation?
    }
}

void SimplifyMap::map(IntValue * /*v1*/, BoolValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(IntValue *v1, CharValue *v2)
{
    // operations: arithmetic, relational,
    double r1, r2;
    r1 = static_cast<double>(v1->getValue());
    r2 = static_cast<double>(v2->getValue());

    _resolveRealExpr(r1, r2, v1, v2);
}

void SimplifyMap::map(IntValue *v1, IntValue *v2)
{
    // operations: arithmetic, relational,
    double r1, r2;
    r1 = static_cast<double>(v1->getValue());
    r2 = static_cast<double>(v2->getValue());

    _resolveRealExpr(r1, r2, v1, v2);
}

void SimplifyMap::map(IntValue *v1, RealValue *v2)
{
    // operations: arithmetic, relational,
    double r1, r2;
    r1 = static_cast<double>(v1->getValue());
    r2 = v2->getValue();

    _resolveRealExpr(r1, r2, v1, v2);
}

void SimplifyMap::map(IntValue * /*v1*/, StringValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(IntValue *v1, TimeValue *v2)
{
    double r1, r2;
    r1 = static_cast<double>(v1->getValue());
    r2 = v2->getValue();
    _resolveRealExpr(r1, r2, v1, v2);

    TimeValue *tv = dynamic_cast<TimeValue *>(_data.result);
    if (tv != nullptr)
        tv->setUnit(v2->getUnit());
}

void SimplifyMap::map(IntValue *v1, Value *v2)
{
    double r1 = static_cast<double>(v1->getValue());
    _resolveConstRealExpr(r1, v1, v2);
}

void SimplifyMap::map(Value *v1, IntValue *v2)
{
    double r2 = static_cast<double>(v2->getValue());
    _resolveConstRealExpr(v1, r2, v2);
}

void SimplifyMap::map(RealValue * /*v1*/, BitValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(RealValue *v1, BitvectorValue *v2)
{
    if (hif::operatorIsArithmetic(_data.oper)) {
        Int tint1;
        Type *a1 = hif::semantics::getSemanticType(v1, _data.sem);
        if (a1 == nullptr)
            return;

        _translateBitvectorToInt(a1, &tint1);

        IntValue *i1 = dynamic_cast<IntValue *>(transformConstant(v2, &tint1, _data.sem));

        if (i1 == nullptr)
            return;
        delete i1->setSemanticType(hif::copy(v2->getSemanticType()));
        map(v1, i1);
        delete i1;

        _setConstValueResult(_data.result, v1, v2);
    } else {
        // TODO
    }
}

void SimplifyMap::map(RealValue * /*v1*/, BoolValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(RealValue *v1, CharValue *v2)
{
    // operations: arithmetic, relational,
    double r1, r2;
    r1 = v1->getValue();
    r2 = static_cast<double>(v2->getValue());

    _resolveRealExpr(r1, r2, v1, v2);
}

void SimplifyMap::map(RealValue *v1, IntValue *v2)
{
    // operations: arithmetic, relational,
    double r1, r2;
    r1 = v1->getValue();
    r2 = static_cast<double>(v2->getValue());

    _resolveRealExpr(r1, r2, v1, v2);
}

void SimplifyMap::map(RealValue *v1, RealValue *v2)
{
    // operations: arithmetic, relational,
    double r1, r2;
    r1 = v1->getValue();
    r2 = v2->getValue();

    _resolveRealExpr(r1, r2, v1, v2);
}

void SimplifyMap::map(RealValue * /*v1*/, StringValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(RealValue *v1, TimeValue *v2)
{
    double r1, r2;
    r1 = v1->getValue();
    r2 = v2->getValue();
    _resolveRealExpr(r1, r2, v1, v2);

    TimeValue *tv = dynamic_cast<TimeValue *>(_data.result);
    if (tv != nullptr)
        tv->setUnit(v2->getUnit());
}

void SimplifyMap::map(RealValue *v1, Value *v2)
{
    double r1 = v1->getValue();
    _resolveConstRealExpr(r1, v1, v2);
}

void SimplifyMap::map(Value *v1, RealValue *v2)
{
    double r2 = v2->getValue();
    _resolveConstRealExpr(v1, r2, v2);
}
void SimplifyMap::map(StringValue * /*v1*/, BitValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(StringValue * /*v1*/, BitvectorValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(StringValue * /*v1*/, BoolValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(StringValue * /*v1*/, CharValue * /*v2*/)
{
    // operations: concat
    // TODO
}

void SimplifyMap::map(StringValue * /*v1*/, IntValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(StringValue * /*v1*/, RealValue * /*v2*/)
{
    // no operations
}

void SimplifyMap::map(StringValue *v1, StringValue *v2)
{
    // Operations: relational
    if (_data.oper == op_eq || _data.oper == op_neq) {
        std::string str1(v1->getValue());
        std::string str2(v2->getValue());
        bool result(false);

        if (_data.oper == op_eq)
            result = (str1 == str2);
        else if (_data.oper == op_neq)
            result = (str1 != str2);
        else if (_data.oper == op_lt)
            result = (str1 < str2);
        else if (_data.oper == op_gt)
            result = (str1 > str2);
        else if (_data.oper == op_le)
            result = (str1 <= str2);
        else if (_data.oper == op_ge)
            result = (str1 >= str2);

        BoolValue *bv = new BoolValue(result);
        _setConstValueResult(bv, v1, v2);
    } else if (_data.oper == op_concat || _data.oper == op_plus) {
        Type *rType = _getOperationType(v1, v2);
        if (rType == nullptr)
            return;
        std::string str1(v1->getValue());
        std::string str2(v2->getValue());
        StringValue *rtVal = new StringValue((str1 + str2).c_str());
        rtVal->setType(rType);
        _data.result = rtVal;
    }
}

void SimplifyMap::map(TimeValue *v1, TimeValue *v2)
{
    if (v1->getUnit() < v2->getUnit())
        v2->changeUnit(v1->getUnit());
    if (v1->getUnit() > v2->getUnit())
        v1->changeUnit(v2->getUnit());

    RealValue r1(v1->getValue());
    r1.setType(_data.sem->getTypeForConstant(&r1));
    RealValue r2(v2->getValue());
    r2.setType(_data.sem->getTypeForConstant(&r2));
    map(&r1, &r2);

    RealValue *rv = dynamic_cast<RealValue *>(_data.result);
    if (rv != nullptr) {
        TimeValue *ret = new TimeValue();
        ret->setUnit(v1->getUnit());
        ret->setValue(rv->getValue());
        _data.result = ret;
        delete rv;
    }
}

void SimplifyMap::map(TimeValue *v1, IntValue *v2)
{
    double r1, r2;
    r1 = v1->getValue();
    r2 = static_cast<double>(v2->getValue());
    _resolveRealExpr(r1, r2, v1, v2);

    TimeValue *tv = dynamic_cast<TimeValue *>(_data.result);
    if (tv != nullptr)
        tv->setUnit(v1->getUnit());
}

void SimplifyMap::map(TimeValue *v1, RealValue *v2)
{
    double r1, r2;
    r1 = v1->getValue();
    r2 = v2->getValue();
    _resolveRealExpr(r1, r2, v1, v2);

    TimeValue *tv = dynamic_cast<TimeValue *>(_data.result);
    if (tv != nullptr)
        tv->setUnit(v1->getUnit());
}

void SimplifyMap::map(Value *v1)
{
    Type *t = hif::semantics::getSemanticType(v1, _data.sem);

    Bit *b   = dynamic_cast<Bit *>(t);
    Bool *bo = dynamic_cast<Bool *>(t);

    if ((b != nullptr || bo != nullptr) && hif::operatorIsReduce(_data.oper)) {
        _data.result = hif::copy(v1);
    }
}

void SimplifyMap::map(Value *v1, Value *v2)
{
    EqualsOptions eqOpts;
    eqOpts.checkConstexprFlag = false;
    const bool res            = hif::equals(v1, v2, eqOpts);
    if (!res)
        return;
    Type *rType = _getOperationType(v1, v2);
    if (rType == nullptr)
        return;
    const bool isLogic = hif::typeIsLogic(rType, _data.sem);
    delete rType;
    if (_data.oper == op_eq || _data.oper == op_neq) {
        // only not logic, since 'x' == 'x' --> 'x' !!!
        if (isLogic)
            return;
        BoolValue *bv = new BoolValue();
        bv->setValue(_data.oper == op_eq || _data.oper == op_case_eq);
        _setConstValueResult(bv, v1, v2);
    } else if (_data.oper == op_case_eq || _data.oper == op_case_neq) {
        BoolValue *bv = new BoolValue();
        bv->setValue(_data.oper == op_eq || _data.oper == op_case_eq);
        _setConstValueResult(bv, v1, v2);
    } else if (_data.oper == op_gt || _data.oper == op_lt) {
        // only not logic, since 'x' < 'x' --> 'x' !!!
        if (isLogic)
            return;
        BoolValue *bv = new BoolValue();
        bv->setValue(false);
        _setConstValueResult(bv, v1, v2);
    } else if (_data.oper == op_ge || _data.oper == op_le) {
        // only not logic, since 'x' <= 'x' --> 'x' !!!
        if (isLogic)
            return;
        BoolValue *bv = new BoolValue();
        bv->setValue(true);
        _setConstValueResult(bv, v1, v2);
    } else if (_data.oper == op_and || _data.oper == op_or) {
        _data.result = hif::copy(v1);
    } else if (_data.oper == op_xor) {
        BoolValue *bv = new BoolValue();
        bv->setValue(false);
        _setConstValueResult(bv, v1, v2);
    } else if (_data.oper == op_plus) {
        Type *t1      = hif::semantics::getSemanticType(v1, _data.sem);
        Range *r      = hif::typeGetSpan(t1, _data.sem);
        IntValue *two = _factory.intval(2);
        hif::typeSetSpan(two->getType(), hif::copy(r), _data.sem);
        hif::typeSetSigned(two->getType(), hif::typeIsSigned(t1, _data.sem), _data.sem);
        Expression *e = _factory.expression(two, op_mult, hif::copy(v1));
        Type *newType = hif::semantics::getSemanticType(e, _data.sem);
        if (newType == nullptr) {
            delete e;
            return;
        }
        _data.result = e;
    } else if (_data.oper == op_minus) {
        IntValue *iv = new IntValue(0);
        _setConstValueResult(iv, v1, v2);
    } else if (_data.oper == op_div) {
        IntValue *iv = new IntValue(1);
        _setConstValueResult(iv, v1, v2);
    } else if (_data.oper == op_band || _data.oper == op_bor) {
        _data.result = hif::copy(v1);
    } else if (_data.oper == op_bxor) {
        IntValue *iv = new IntValue(0);
        _setConstValueResult(iv, v1, v2);
    }
}
} // namespace

Value *simplifyExpression(Expression *e, hif::semantics::ILanguageSemantics *sem, SimplifyOptions opt)
{
    // initialize data
    SimplifyMapData data;
    data.sem            = sem;
    data.oper           = e->getOperator();
    data.result         = nullptr;
    data.startingObject = e;
    data.options        = opt;

    // call simplify map
    SimplifyMap simplifyMap(data);
    if (e->getValue2() == nullptr) {
        simplifyMap.callMap(e->getValue1());
    } else {
        simplifyMap.callMap(e->getValue1(), e->getValue2());
    }

    Value *ret = simplifyMap.getResult();

    if (ret != nullptr)
        return ret;

#ifndef NDEBUG

    // If this check fails, improve the _simplifyExpression() method, OR add
    // checks to getNestedExpressionOperands() to avoid wrong rebalancing.
    bool cannotBeSimplified = simplifyMap.cannotBeSimplified();

    // Known case1: complexOps is when at least one operand is not a const value.
    // e.g.
    // ID * ID
    // CAST(x)(BV) + CAST(x)(BV) should be CAST(x)(BV) * 2
    // but it is not allowed by HIFSemantics (different types)
    const bool complexOps =
        (dynamic_cast<ConstValue *>(e->getValue1()) == nullptr ||
         dynamic_cast<ConstValue *>(e->getValue2()) == nullptr);

    cannotBeSimplified |=
        (complexOps && (!hif::equals(e->getValue1(), e->getValue2()) ||
                        (e->getOperator() == op_mult || e->getOperator() == op_plus)));

    // Known case2: in case of concats cannot always simplify
    cannotBeSimplified |= (e->getOperator() == op_concat);

    // Known case3: in case of logic/logic_vector op_eq/op_neq logic, it must not be simplified.
    Type *exprType     = hif::semantics::getBaseType(hif::semantics::getSemanticType(e, sem), false, sem);
    const bool isOpRel = hif::operatorIsRelational(e->getOperator()) && hif::typeIsLogic(exprType, sem);
    cannotBeSimplified |= isOpRel;

#    if 0 // ENABLE TO DEBUG SIMPLIFICATION
    if (!cannotBeSimplified)
    {
        if (e->getValue2() == nullptr)
        {
            simplifyMap.callMap(e->getValue1());
        }
        else
        {
            simplifyMap.callMap(e->getValue1(), e->getValue2());
        }

    }
#    endif

    messageAssert(cannotBeSimplified, "Unable to simplify expression.", e, sem);

#endif // ndef NDEBUG

    // not simplified
    return nullptr;
}

} // namespace manipulation
} // namespace hif
