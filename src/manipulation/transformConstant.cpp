/// @file transformConstant.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>
#include <cstring>
#include <sstream>
#include <stdint.h>
#include <utility>

#include "hif/BiVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

// temporary pragma for disable warnings
#if (defined __GNUC__)
#    if HIF_DIAGNOSTIC_PUSH_POP
#        pragma GCC diagnostic push
#    endif
#    pragma GCC diagnostic ignored "-Winline"
#endif
#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#    pragma clang diagnostic ignored "-Wmissing-noreturn"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

// //////////////////////////////////////////////////////////////////////////
// WARNING
// Constant for maximum allowed bit expansion.
// otherwise we could create huge bitvectors in memory, with memory expansion.
// //////////////////////////////////////////////////////////////////////////
#define HIF_MAX_ALLOWED_SPAN_SIZE 1024

namespace hif
{
namespace manipulation
{

namespace
{ // unnamed

// ////////////////////////////////////////////////////////////////////////////
// TransformOptions
// ////////////////////////////////////////////////////////////////////////////

struct TransformOptions {
    TransformOptions();
    ~TransformOptions();
    TransformOptions(const TransformOptions &other);
    TransformOptions &operator=(TransformOptions other);
    void swap(TransformOptions &other);

    bool allowTruncation;
    bool requiredConstValue;
    Value *result;
    hif::semantics::ILanguageSemantics *sem;
};

TransformOptions::TransformOptions()
    : allowTruncation(false)
    , requiredConstValue(false)
    , result(nullptr)
    , sem(nullptr)
{
    // ntd
}

TransformOptions::~TransformOptions()
{
    // ntd
}

TransformOptions::TransformOptions(const TransformOptions &other)
    : allowTruncation(other.allowTruncation)
    , requiredConstValue(other.requiredConstValue)
    , result(other.result)
    , sem(other.sem)
{
    // ntd
}

TransformOptions &TransformOptions::operator=(TransformOptions other)
{
    swap(other);
    return *this;
}

void TransformOptions::swap(TransformOptions &other)
{
    std::swap(allowTruncation, other.allowTruncation);
    std::swap(requiredConstValue, other.requiredConstValue);
    std::swap(result, other.result);
    std::swap(sem, other.sem);
}
// ////////////////////////////////////////////////////////////////////////////
// Support methods.
// ////////////////////////////////////////////////////////////////////////////

std::uint64_t _spanGetBitwidth(Range *range, hif::semantics::ILanguageSemantics *sem)
{
    std::uint64_t size = hif::semantics::spanGetBitwidth(range, sem);
    if (size > HIF_MAX_ALLOWED_SPAN_SIZE) {
        std::string s;
        std::stringstream ss;
        ss << "Found a span greater than ";
        ss << HIF_MAX_ALLOWED_SPAN_SIZE;
        ss << ": current span is ";
        ss << size;
        s = ss.str();

        messageWarning(s, nullptr, nullptr);
        size = 0;
    }
    return size;
}

char _logic2bit(const char c)
{
    if (c == '1' || c == 'H' || c == 'h')
        return '1';
    return '0'; // as VHDL
}

std::string _logic2bit(const std::string &s)
{
    std::string ret(s);
    for (std::string::size_type i = 0; i < ret.size(); ++i) {
        ret[i] = _logic2bit(ret[i]);
    }
    return ret;
}

bool _isZeros(const std::string &s)
{
    for (std::string::size_type i = 0; i < s.size(); ++i) {
        if (s[i] != '0')
            return false;
    }

    return true;
}

std::string _int2BinString(std::uint64_t i)
{
    std::string ret;

    do {
        if (i % 2)
            ret = '1' + ret;
        else
            ret = '0' + ret;
        i = i >> 1;
    } while (i != 0);

    return ret;
}

double _string2double(std::string s)
{
    const std::string::size_type size = s.size();
    if (size == 0)
        return 0.0;

    double ret = 0.0;

    // Managing different formats:
    // hex   (0x or 0X)
    // octal (0)
    // bits  (simple string of 0 & 1 )
    // int & double

    // Is binary?
    const bool isBinary = size > 3 && (s[0] == 'b' || s[0] == 'B') && s[1] == '\'';
    if (isBinary) {
        for (std::string::size_type i = 2; i < size; ++i) {
            // Converting:
            double d = (s[i] == '0') ? 0.0 : 1.0;
            ret      = 2 * ret + d;
        }

        return ret;
    }

    // Then is octal, hex or integer:
    std::stringstream ss;
    ss << s;
    ss >> ret;

    return ret;
}

// ////////////////////////////////////////////////////////////////////////////
// Support methods.
// ////////////////////////////////////////////////////////////////////////////
class TransformConstant : public BiVisitor<TransformConstant>
{
public:
    TransformConstant(TransformOptions &opt);
    ~TransformConstant();

    Value *getResult() const;

    /// Maps declarations
    void map(Object *, Object *);
    void map(Value *, Type *);

    void map(BitvectorValue *v, Array *t);
    void map(IntValue *v, Array *t);

    void map(BitvectorValue *v, Bit *t);
    void map(BitValue *v, Bit *t);
    void map(IntValue *v, Bit *bt);
    void map(BoolValue *v, Bit *t);
    void map(CharValue *v, Bit *t);
    void map(RealValue *v, Bit *t);
    void map(StringValue *v, Bit *t);

    void map(BitvectorValue *v, Bitvector *t);
    void map(IntValue *v, Bitvector *t);
    void map(RealValue *v, Bitvector *t);
    void map(BitValue *v, Bitvector *t);
    void map(StringValue *v, Bitvector *t);

    void map(BitvectorValue *v, Signed *t);
    void map(IntValue *v, Signed *t);
    void map(RealValue *v, Signed *t);
    void map(StringValue *v, Signed *t);

    void map(BitvectorValue *v, Unsigned *t);
    void map(IntValue *v, Unsigned *t);
    void map(RealValue *v, Unsigned *t);
    void map(StringValue *v, Unsigned *t);

    void map(BitvectorValue *v, Bool *t);
    void map(BoolValue *v, Bool *t);
    void map(IntValue *v, Bool *t);
    void map(BitValue *v, Bool *t);
    void map(CharValue *v, Bool *t);
    void map(RealValue *v, Bool *t);
    void map(StringValue *v, Bool *t);

    void map(CharValue *v, Char *t);
    void map(IntValue *v, Char *t);
    void map(BoolValue *v, Char *t);
    void map(BitValue *v, Char *t);
    void map(RealValue *v, Char *t);
    void map(StringValue *v, Char *t);

    void map(BitValue *v, Int *t);
    void map(BoolValue *v, Int *);
    void map(CharValue *v, Int *t);
    void map(IntValue *v, Int *t);
    void map(RealValue *v, Int *);
    void map(StringValue *v, Int *t);
    void map(BitvectorValue *v, Int *t);

    void map(BitvectorValue *v, Pointer *t);

    void map(IntValue *v, Real *t);
    void map(BitValue *v, Real *t);
    void map(BoolValue *v, Real *t);
    void map(CharValue *v, Real *t);
    void map(RealValue *v, Real *t);
    void map(StringValue *v, Real *t);
    void map(BitvectorValue *v, Real *t);

    void map(IntValue *v, String *t);
    void map(BitValue *v, String *t);
    void map(BoolValue *v, String *t);
    void map(CharValue *v, String *t);
    void map(RealValue *v, String *t);
    void map(StringValue *v, String *t);

    void map(IntValue *v, Time *t);
    void map(RealValue *v, Time *t);

private:
    TransformOptions &_opt;
    HifFactory _factory;

    TransformConstant(const TransformConstant &);
    TransformConstant &operator=(const TransformConstant &);

}; // end of TransformConstant

TransformConstant::TransformConstant(TransformOptions &opt)
    : _opt(opt)
    , _factory(opt.sem)
{
    // ntd
}

TransformConstant::~TransformConstant()
{
    // ntd
}

Value *TransformConstant::getResult() const { return _opt.result; }

// ////////////////////////////////////////////////////////////////////////////
// Default maps
// ////////////////////////////////////////////////////////////////////////////

void TransformConstant::map(Object *, Object *) { messageError("Wrong call to transformValue", nullptr, nullptr); }

void TransformConstant::map(Value *, Type *) { _opt.result = nullptr; }
// ////////////////////////////////////////////////////////////////////////////
// Array
// ////////////////////////////////////////////////////////////////////////////

void TransformConstant::map(BitvectorValue *v, Array *t)
{
    if (_opt.requiredConstValue)
        return;

    Bit *bit               = dynamic_cast<Bit *>(t->getType());
    Bitvector *bitvector   = dynamic_cast<Bitvector *>(t->getType());
    Signed *signedType     = dynamic_cast<Signed *>(t->getType());
    Unsigned *unsignedType = dynamic_cast<Unsigned *>(t->getType());
    if (bit == nullptr && bitvector == nullptr && signedType == nullptr && unsignedType == nullptr)
        return;

    if (bit != nullptr) {
        Bitvector bv;
        bv.setSpan(hif::copy(t->getSpan()));
        bv.setSigned(t->isSigned());
        bv.setResolved(bit->isResolved());
        bv.setLogic(bit->isLogic());
        map(v, &bv);

        if (_opt.result == nullptr)
            return;
        BitvectorValue *bvRet = dynamic_cast<BitvectorValue *>(_opt.result);
        messageAssert(bvRet != nullptr, "Unexpected conversion to bitvector type", _opt.result, _opt.sem);

        _opt.result = nullptr;

        std::string s = bvRet->getValue();
        messageAssert(!s.empty(), "Unexpected empty bitvector value", bvRet, _opt.sem);
        delete bvRet;

        if (t->getSpan()->getDirection() == dir_downto) {
            // revert since the for to create the indeces starts always from zero
            std::reverse(s.begin(), s.end());
        }

        bool equals = true;
        char ref    = s[0];
        for (std::string::size_type i = 1; i < s.size(); ++i) {
            if (s[i] == ref)
                continue;
            equals = false;
            break;
        }

        // If all bits have the same value, transform in aggregate with other:
        if (equals) {
            BitValue *aggOther = new BitValue();
            aggOther->setValue(ref);
            Bit *synt = hif::copy(bit);
            synt->setConstexpr(true);
            aggOther->setType(synt);

            _opt.result = _factory.aggregate(aggOther, _factory.noAggregateAlts());
            return;
        }

        // assign to each index the matching value
        Value *min           = hif::copy(hif::rangeGetMinBound(t->getSpan()));
        ConstValue *constVal = dynamic_cast<ConstValue *>(min);
        if (constVal != nullptr) {
            constVal->setType(_opt.sem->getTypeForConstant(constVal));
        }

        Aggregate *aggr = new Aggregate();
        for (std::string::size_type i = 0; i < s.size(); ++i) {
            BitValue *aggBit = new BitValue();
            aggBit->setValue(s[i]);
            Bit *synt = hif::copy(bit);
            synt->setConstexpr(true);
            aggBit->setType(synt);

            IntValue *intVal = new IntValue();
            intVal->setValue(static_cast<std::int64_t>(i));
            intVal->setType(_opt.sem->getTypeForConstant(intVal));

            Expression *index = new Expression();
            index->setValue1(hif::copy(min));
            index->setOperator(op_plus);
            index->setValue2(intVal);

            AggregateAlt *aggrAlt = new AggregateAlt();
            aggrAlt->setValue(aggBit);
            aggrAlt->indices.push_back(index);

            aggr->alts.push_back(aggrAlt);
        }

        delete min;
        _opt.result = aggr;
    } else if (bitvector != nullptr || signedType != nullptr || unsignedType != nullptr) {
        Bitvector bv;
        Value *l = hif::semantics::typeGetTotalSpanSize(t, _opt.sem);
        if (dynamic_cast<IntValue *>(l) == nullptr) {
            delete l;
            return;
        }
        std::int64_t bvl = static_cast<IntValue *>(l)->getValue();
        delete l;
        Range *r = _factory.range(bvl - 1, 0);
        bv.setSpan(r);
        bv.setSigned(hif::typeIsSigned(t, _opt.sem));
        bv.setConstexpr(hif::typeIsConstexpr(t, _opt.sem));

        map(v, &bv);
        if (_opt.result == nullptr)
            return;

        BitvectorValue *bvRet = dynamic_cast<BitvectorValue *>(_opt.result);
        messageAssert(bvRet != nullptr, "Unexpected conversion to bitvector type", _opt.result, _opt.sem);
        _opt.result = nullptr;

        std::string s                  = bvRet->getValue();
        std::uint64_t elementSize = hif::semantics::typeGetSpanBitwidth(t->getType(), _opt.sem);
        if (elementSize == 0)
            return;
        std::uint64_t elements = static_cast<std::uint64_t>(bvl) / elementSize;

        const bool isArrayDownto = t->getSpan()->getDirection() == hif::dir_downto;

        Aggregate *agg = new Aggregate();
        for (std::uint64_t i = 0; i < elements; ++i) {
            std::uint64_t ind = isArrayDownto ? elements - i - 1 : i;
            AggregateAlt *alt      = new AggregateAlt();
            std::string sub        = s.substr(
                static_cast<std::string::size_type>(elementSize * i), static_cast<std::string::size_type>(elementSize));

            BitvectorValue *altV = _factory.bitvectorval(
                sub,
                _factory.bitvector(
                    new Range(static_cast<std::int64_t>(elementSize - 1), 0), hif::typeIsLogic(t->getType(), _opt.sem),
                    hif::typeIsResolved(t->getType(), _opt.sem), true, hif::typeIsSigned(t->getType(), _opt.sem)));

            alt->setValue(altV);
            alt->indices.push_back(new IntValue(static_cast<std::int64_t>(ind)));
            agg->alts.push_back(alt);
        }
        _opt.result = agg;
    } else {
        messageError("Unexpected case", nullptr, _opt.sem);
    }
}

void TransformConstant::map(IntValue *v, Array *t)
{
    if (_opt.requiredConstValue)
        return;

    Int *integer = dynamic_cast<Int *>(t->getType());
    if (integer == nullptr)
        return;

    Int intType;
    Value *l = hif::semantics::typeGetTotalSpanSize(t, _opt.sem);
    if (dynamic_cast<IntValue *>(l) == nullptr) {
        delete l;
        return;
    }
    std::int64_t bvl = static_cast<IntValue *>(l)->getValue();
    delete l;
    const bool isSigned = hif::typeIsSigned(t, _opt.sem);
    Range *r            = _factory.range(bvl - 1, 0);
    intType.setSpan(r);
    intType.setSigned(isSigned);
    intType.setConstexpr(hif::typeIsConstexpr(t, _opt.sem));

    map(v, &intType);
    if (_opt.result == nullptr)
        return;

    IntValue *intRet = dynamic_cast<IntValue *>(_opt.result);
    messageAssert(intRet != nullptr, "Unexpected conversion to integer type", _opt.result, _opt.sem);
    _opt.result = nullptr;

    Bitvector bv;
    bv.setSpan(hif::copy(r));
    bv.setSigned(isSigned);
    bv.setConstexpr(intType.isConstexpr());
    map(intRet, &bv);
    delete intRet;
    if (_opt.result == nullptr)
        return;

    BitvectorValue *bvRet = dynamic_cast<BitvectorValue *>(_opt.result);
    messageAssert(bvRet != nullptr, "Unexpected conversion to bitvector type", _opt.result, _opt.sem);
    _opt.result = nullptr;

    std::string s = bvRet->getValue();
    delete bvRet;

    std::uint64_t elementSize = hif::semantics::typeGetSpanBitwidth(t->getType(), _opt.sem);
    if (elementSize == 0)
        return;
    if (elementSize > 64)
        return;
    std::uint64_t elements = static_cast<std::uint64_t>(bvl) / elementSize;
    const bool isArrayDownto    = t->getSpan()->getDirection() == hif::dir_downto;
    Aggregate *agg              = new Aggregate();

    for (std::uint64_t i = 0; i < elements; ++i) {
        std::uint64_t ind = isArrayDownto ? elements - i - 1 : i;
        AggregateAlt *alt      = new AggregateAlt();
        std::string sub        = s.substr(
            static_cast<std::string::size_type>(elementSize * i), static_cast<std::string::size_type>(elementSize));
        // If elementSize is less than 64 we need to extend sign in order to
        // execute properly strtoll
        if (isSigned && sub[0] == '1') {
            while (sub.size() < 64) {
                sub = "1" + sub;
            }
        }

        char *fake;
        std::int64_t subVal = strtoll(sub.c_str(), &fake, 2);
        IntValue *altV =
            _factory.intval(subVal, _factory.integer(new Range(static_cast<std::int64_t>(elementSize - 1), 0)));

        alt->setValue(altV);
        alt->indices.push_back(new IntValue(static_cast<std::int64_t>(ind)));
        agg->alts.push_back(alt);
    }
    _opt.result = agg;
}
// ////////////////////////////////////////////////////////////////////////////
// Bit
// ////////////////////////////////////////////////////////////////////////////

void TransformConstant::map(BitvectorValue *v, Bit *t)
{
    Type *tt = hif::semantics::getSemanticType(v, _opt.sem);
    messageAssert(tt != nullptr, "Unable to type bitvector value", v, _opt.sem);
    Range *r = typeGetSpan(tt, _opt.sem);
    char c;
    if (r->getDirection() == dir_downto) {
        c = v->getValue()[v->getValue().size() - 1];
    } else {
        c = v->getValue()[0];
    }

    if (!t->isLogic()) {
        c = _logic2bit(c);
    }

    BitValue *b = new BitValue();
    b->setValue(c);
    _opt.result = b;
}

void TransformConstant::map(BitValue *v, Bit *t)
{
    BitValue *b            = new BitValue();
    BitConstant currBitval = v->getValue();
    if (!t->isLogic()) {
        if (currBitval == bit_one || currBitval == bit_h)
            b->setValue(bit_one);
        else
            b->setValue(bit_zero);
    } else // logic
    {
        b->setValue(currBitval);
    }

    _opt.result = b;
}

void TransformConstant::map(IntValue *v, Bit *)
{
    std::int64_t val    = v->getValue();
    std::int64_t newVal = (val & 1ll);
    if (newVal != val && !_opt.allowTruncation)
        return;

    BitValue *ret    = new BitValue();
    const bool isOne = (newVal == 1ll);
    if (isOne)
        ret->setValue(bit_one);
    else
        ret->setValue(bit_zero);

    _opt.result = ret;
}

void TransformConstant::map(BoolValue *v, Bit *)
{
    BitValue *ret = new BitValue();
    if (v->getValue())
        ret->setValue(bit_one);
    else
        ret->setValue(bit_zero);

    _opt.result = ret;
}

void TransformConstant::map(CharValue *v, Bit *)
{
    BitConstant retBitVal(bit_dontcare);
    switch (v->getValue()) {
    case '0':
        retBitVal = bit_zero;
        break;
    case '1':
        retBitVal = bit_one;
        break;
    case 'X':
    case 'x':
        retBitVal = bit_x;
        break;
    case 'Z':
    case 'z':
        retBitVal = bit_z;
        break;
    case 'U':
    case 'u':
        retBitVal = bit_u;
        break;
    case 'L':
    case 'l':
        retBitVal = bit_l;
        break;
    case 'H':
    case 'h':
        retBitVal = bit_h;
        break;
    case 'W':
    case 'w':
        retBitVal = bit_w;
        break;
    case '-': // vhdl
    case '?': // verilog
        retBitVal = bit_dontcare;
        break;
    default:
        // Error:
        return;
    }

    BitValue *ret = new BitValue();
    ret->setValue(retBitVal);
    _opt.result = ret;
}

void TransformConstant::map(RealValue *v, Bit *t)
{
    std::int64_t ii = _opt.sem->transformRealToInt(v->getValue());
    if ((double(ii) < v->getValue() || double(ii) > v->getValue()) && !_opt.allowTruncation) {
        return;
    }

    IntValue iv(ii);
    map(&iv, t);
}

void TransformConstant::map(StringValue *v, Bit *t)
{
    Char c;
    map(v, &c);

    if (_opt.result == nullptr)
        return;
    CharValue *cv = dynamic_cast<CharValue *>(_opt.result);
    messageAssert(cv != nullptr, "Unexpected conversion", _opt.result, _opt.sem);

    _opt.result = nullptr;
    map(cv, t);
    delete cv;
}
// ////////////////////////////////////////////////////////////////////////////
// Bitvector
// ////////////////////////////////////////////////////////////////////////////

void TransformConstant::map(BitvectorValue *v, Bitvector *t)
{
    Type *cvType = hif::semantics::getSemanticType(v, _opt.sem);
    messageAssert(cvType != nullptr, "Cannot type bitvector value", v, _opt.sem);

    // 1- resizing to t dimension
    const std::string &val     = v->getValue();
    std::uint64_t valSize = static_cast<std::uint64_t>(val.size());
    Range *cvTypeSpan          = hif::typeGetSpan(cvType, _opt.sem);
    const bool cvIsSigned      = typeIsSigned(cvType, _opt.sem);
    const bool cvIsLogic       = typeIsLogic(cvType, _opt.sem);
    const bool cvIsDownto      = (cvTypeSpan->getDirection() == dir_downto);

    std::uint64_t tSize = _spanGetBitwidth(t->getSpan(), _opt.sem);
    if (tSize == 0) {
        if (!_isZeros(val)) {
            return;
        }

        BitvectorValue *bvRet = hif::copy(v);
        std::string res       = _logic2bit(val);
        bvRet->setValue(res);
        _opt.result = bvRet;
        return;
    }

    std::string res;
    const std::string::size_type diff = (valSize > tSize) ? static_cast<std::string::size_type>(valSize - tSize)
                                                          : static_cast<std::string::size_type>(tSize - valSize);
    if (cvIsSigned) {
        if (valSize > tSize) {
            // signed truncating
            if (cvIsDownto)
                res = val.substr(diff);
            else
                res = val.substr(0, diff);

            if (!_opt.allowTruncation) {
                char sign;
                if (cvTypeSpan->getDirection() == dir_downto)
                    sign = res[0];
                else
                    sign = res[res.size() - 1];

                std::string ext(diff, sign);

                std::string tmp;
                if (cvIsDownto)
                    tmp = val.substr(0, diff);
                else
                    tmp = val.substr(diff);

                if (tmp != ext)
                    return;
            }
        } else {
            // signed extending
            char sign;
            if (cvTypeSpan->getDirection() == dir_downto)
                sign = val[0];
            else
                sign = val[static_cast<std::string::size_type>(valSize) - 1];

            std::string ext(diff, sign);
            if (cvIsDownto)
                res = ext + val;
            else
                res = val + ext;
        }
    } else // ! cvIsSigned
    {
        std::string ext(diff, '0');

        if (valSize > tSize) {
            // unsigned truncating
            if (cvIsDownto)
                res = val.substr(diff);
            else
                res = val.substr(0, diff);

            if (!_opt.allowTruncation) {
                std::string tmp;
                if (cvIsDownto)
                    tmp = val.substr(0, diff);
                else
                    tmp = val.substr(diff);

                if (tmp != ext)
                    return;
            }
        } else {
            // unsigned extending
            if (cvIsDownto)
                res = ext + val;
            else
                res = val + ext;
        }
    }

    // 2- Manage logic:
    if (cvIsLogic && !t->isLogic()) {
        res = _logic2bit(res);
    }

    // 3- Build result
    _opt.result = new BitvectorValue(res);
}

void TransformConstant::map(IntValue *v, Bitvector *t)
{
    Type *cvType = hif::semantics::getSemanticType(v, _opt.sem);
    messageAssert(cvType != nullptr, "Cannot type bitvector value", v, _opt.sem);

    std::uint64_t tSize = _spanGetBitwidth(t->getSpan(), _opt.sem);
    std::int64_t val            = v->getValue();
    if (tSize == 0) {
        if (val != 0LL) {
            return;
        }

        _opt.result = new BitvectorValue("0");
        return;
    }

    std::string valString(_int2BinString(static_cast<std::uint64_t>(val)));
    if (valString.size() < 64)
        valString = "0" + valString;
    BitvectorValue bvVal(valString);
    bvVal.setType(_factory.bitvector(
        hif::copy(hif::typeGetSpan(cvType, _opt.sem)), false, false, true, hif::typeIsSigned(cvType, _opt.sem)));

    map(&bvVal, t);
}
void TransformConstant::map(RealValue *v, Bitvector *t)
{
    std::int64_t ii = _opt.sem->transformRealToInt(v->getValue());
    if ((double(ii) < v->getValue() || double(ii) > v->getValue()) && !_opt.allowTruncation) {
        return;
    }

    IntValue ivo(ii);
    map(&ivo, t);
}

void TransformConstant::map(BitValue *v, Bitvector *t)
{
    BitvectorValue bvVal(v->toString());
    map(&bvVal, t);
}

void TransformConstant::map(StringValue *v, Bitvector *t)
{
    const std::string &val = v->getValue();
    bool valid             = true;
    for (std::string::size_type i = 0; i < val.size(); ++i) {
        const char vali = val[i];
        if (vali == '0' || vali == '1' || vali == 'h' || vali == 'H' || vali == 'l' || vali == 'L' || vali == 'x' ||
            vali == 'X' || vali == 'z' || vali == 'Z' || vali == 'w' || vali == 'W' || vali == '-' || vali == '?') {
            continue;
        }

        valid = false;
        break;
    }

    if (!valid)
        return;
    BitvectorValue bvVal(val);

    map(&bvVal, t);
}
// ///////////////////////////////////////////////////////////////////
// Signed
// ///////////////////////////////////////////////////////////////////

void TransformConstant::map(BitvectorValue *v, Signed *t)
{
    Bitvector bv;
    bv.setLogic(true);
    bv.setResolved(true);
    bv.setSigned(true);
    bv.setSpan(hif::copy(t->getSpan()));

    const bool canReplace = (t->getParent() != nullptr);
    if (canReplace)
        t->replace(&bv);
    map(v, &bv);
    if (canReplace)
        bv.replace(t);
}

void TransformConstant::map(IntValue *v, Signed *t)
{
    Bitvector bv;
    bv.setLogic(true);
    bv.setResolved(true);
    bv.setSigned(true);
    bv.setSpan(hif::copy(t->getSpan()));

    const bool canReplace = (t->getParent() != nullptr);
    if (canReplace)
        t->replace(&bv);
    map(v, &bv);
    if (canReplace)
        bv.replace(t);
}

void TransformConstant::map(RealValue *v, Signed *t)
{
    Bitvector bv;
    bv.setLogic(true);
    bv.setResolved(true);
    bv.setSigned(true);
    bv.setSpan(hif::copy(t->getSpan()));

    const bool canReplace = (t->getParent() != nullptr);
    if (canReplace)
        t->replace(&bv);
    map(v, &bv);
    if (canReplace)
        bv.replace(t);
}

void TransformConstant::map(StringValue *v, Signed *t)
{
    Bitvector bv;
    bv.setLogic(true);
    bv.setResolved(true);
    bv.setSigned(true);
    bv.setSpan(hif::copy(t->getSpan()));

    const bool canReplace = (t->getParent() != nullptr);
    if (canReplace)
        t->replace(&bv);
    map(v, &bv);
    if (canReplace)
        bv.replace(t);
}
// ///////////////////////////////////////////////////////////////////
// Unsigned
// ///////////////////////////////////////////////////////////////////

void TransformConstant::map(BitvectorValue *v, Unsigned *t)
{
    Bitvector bv;
    bv.setLogic(true);
    bv.setResolved(true);
    bv.setSigned(false);
    bv.setSpan(hif::copy(t->getSpan()));

    const bool canReplace = (t->getParent() != nullptr);
    if (canReplace)
        t->replace(&bv);
    map(v, &bv);
    if (canReplace)
        bv.replace(t);
}

void TransformConstant::map(IntValue *v, Unsigned *t)
{
    Bitvector bv;
    bv.setLogic(true);
    bv.setResolved(true);
    bv.setSigned(false);
    bv.setSpan(hif::copy(t->getSpan()));

    const bool canReplace = (t->getParent() != nullptr);
    if (canReplace)
        t->replace(&bv);
    map(v, &bv);
    if (canReplace)
        bv.replace(t);
}

void TransformConstant::map(RealValue *v, Unsigned *t)
{
    Bitvector bv;
    bv.setLogic(true);
    bv.setResolved(true);
    bv.setSigned(false);
    bv.setSpan(hif::copy(t->getSpan()));

    const bool canReplace = (t->getParent() != nullptr);
    if (canReplace)
        t->replace(&bv);
    map(v, &bv);
    if (canReplace)
        bv.replace(t);
}

void TransformConstant::map(StringValue *v, Unsigned *t)
{
    Bitvector bv;
    bv.setLogic(true);
    bv.setResolved(true);
    bv.setSigned(false);
    bv.setSpan(hif::copy(t->getSpan()));

    const bool canReplace = (t->getParent() != nullptr);
    if (canReplace)
        t->replace(&bv);
    map(v, &bv);
    if (canReplace)
        bv.replace(t);
}
// ////////////////////////////////////////////////////////////////////////////
// Bool
// ////////////////////////////////////////////////////////////////////////////

void TransformConstant::map(BitvectorValue *v, Bool *t)
{
    Bit bit;
    bit.setLogic(false);
    bit.setResolved(false);

    map(v, &bit);

    if (_opt.result == nullptr)
        return;
    BitValue *bv = dynamic_cast<BitValue *>(_opt.result);
    messageAssert(bv != nullptr, "Unexpected conversion", _opt.result, _opt.sem);

    _opt.result = nullptr;
    map(bv, t);
    delete bv;
}

void TransformConstant::map(BoolValue *v, Bool *) { _opt.result = hif::copy(v); }

void TransformConstant::map(IntValue *v, Bool *t)
{
    Bit bit;
    bit.setLogic(false);
    bit.setResolved(false);

    map(v, &bit);

    if (_opt.result == nullptr)
        return;
    BitValue *bv = dynamic_cast<BitValue *>(_opt.result);
    messageAssert(bv != nullptr, "Unexpected conversion", _opt.result, _opt.sem);

    _opt.result = nullptr;
    map(bv, t);
    delete bv;
}

void TransformConstant::map(BitValue *v, Bool *)
{
    const char valChar = v->toString()[0];
    const char bit     = _logic2bit(valChar);
    if (bit != valChar && !_opt.allowTruncation) {
        return;
    }

    _opt.result = new BoolValue(valChar == '1');
}

void TransformConstant::map(CharValue *v, Bool *t)
{
    Bit bit;
    bit.setLogic(false);
    bit.setResolved(false);

    map(v, &bit);

    if (_opt.result == nullptr)
        return;
    BitValue *bv = dynamic_cast<BitValue *>(_opt.result);
    messageAssert(bv != nullptr, "Unexpected conversion", _opt.result, _opt.sem);

    _opt.result = nullptr;
    map(bv, t);
    delete bv;
}

void TransformConstant::map(RealValue *v, Bool *t)
{
    Bit bit;
    bit.setLogic(false);
    bit.setResolved(false);

    map(v, &bit);

    if (_opt.result == nullptr)
        return;
    BitValue *bv = dynamic_cast<BitValue *>(_opt.result);
    messageAssert(bv != nullptr, "Unexpected conversion", _opt.result, _opt.sem);

    _opt.result = nullptr;
    map(bv, t);
    delete bv;
}

void TransformConstant::map(StringValue *v, Bool *t)
{
    Bit bit;
    bit.setLogic(false);
    bit.setResolved(false);

    map(v, &bit);

    if (_opt.result == nullptr)
        return;
    BitValue *bv = dynamic_cast<BitValue *>(_opt.result);
    messageAssert(bv != nullptr, "Unexpected conversion", _opt.result, _opt.sem);

    _opt.result = nullptr;
    map(bv, t);
    delete bv;
}
// ////////////////////////////////////////////////////////////////////////////
// Char
// ////////////////////////////////////////////////////////////////////////////

void TransformConstant::map(CharValue *v, Char *) { _opt.result = hif::copy(v); }

void TransformConstant::map(IntValue *v, Char *)
{
    std::int64_t val    = v->getValue();
    const char c     = static_cast<char>(val);
    std::int64_t newVal = static_cast<std::int64_t>(c);
    if (newVal != val && !_opt.allowTruncation) {
        return;
    }

    CharValue *ret = new CharValue(c);
    _opt.result    = ret;
}

void TransformConstant::map(BoolValue *v, Char *)
{
    CharValue *ret = new CharValue();
    if (v->getValue()) {
        ret->setValue('1');
    } else {
        ret->setValue('0');
    }

    _opt.result = ret;
}

void TransformConstant::map(BitValue *v, Char *)
{
    CharValue *ret = new CharValue(v->toString()[0]);
    _opt.result    = ret;
}

void TransformConstant::map(RealValue *v, Char *t)
{
    std::int64_t ii = _opt.sem->transformRealToInt(v->getValue());
    if ((double(ii) < v->getValue() || double(ii) > v->getValue()) && !_opt.allowTruncation) {
        return;
    }

    IntValue iv(ii);
    map(&iv, t);
}

void TransformConstant::map(StringValue *v, Char * /*t*/)
{
    const std::string &val = v->getValue();
    if (val.size() != 1 && !_opt.allowTruncation) {
        return;
    }

    Type *cvType     = hif::semantics::getSemanticType(v, _opt.sem);
    String *cvString = dynamic_cast<String *>(cvType);
    messageAssert(cvString != nullptr, "Unxpected string value type", v, _opt.sem);

    char c = '0';
    if (cvString->getSpanInformation() == nullptr || cvString->getSpanInformation()->getDirection() == dir_upto) {
        c = val[0];
    } else {
        c = val[val.size() - 1];
    }

    _opt.result = new CharValue(c);
}
// ////////////////////////////////////////////////////////////////////////////
// Int
// ////////////////////////////////////////////////////////////////////////////

void TransformConstant::map(BitValue *v, Int *)
{
    const char val = v->toString()[0];
    const char bit = _logic2bit(val);

    if (val != bit && !_opt.allowTruncation) {
        return;
    }

    _opt.result = new IntValue(val == '1' ? 1 : 0);
}

void TransformConstant::map(BoolValue *v, Int *) { _opt.result = new IntValue(static_cast<std::int64_t>(v->getValue())); }

void TransformConstant::map(CharValue *v, Int *t)
{
    IntValue iv(v->getValue());
    map(&iv, t);
}

void TransformConstant::map(IntValue *v, Int *t)
{
    std::uint64_t size = hif::semantics::spanGetBitwidth(t->getSpan(), _opt.sem);
    std::int64_t val           = v->getValue();
    if (size == 0 || size >= 64) {
        _opt.result = hif::copy(v);
        return;
    }

    Type *cvType = hif::semantics::getSemanticType(v, _opt.sem);
    messageAssert(cvType != nullptr, "Cannot type int value", v, _opt.sem);

    std::int64_t res      = 0;
    std::uint64_t uval = static_cast<std::uint64_t>(val);

    const std::uint64_t trunkMask = size == 0 ? 0 : std::uint64_t(-1) >> (64 - size);
    uval                          = uval & trunkMask;

    const bool isSigned = typeIsSigned(t, _opt.sem);
    const bool msb      = (size == 0 ? 0 : uval >> (size - 1)) == 1;

    if (isSigned && msb) {
        const std::uint64_t signMask = size == sizeof(std::uint64_t) * 8 ? 0 : std::uint64_t(-1) << (size);
        res                          = int64_t(uval | signMask);
    } else {
        res = int64_t(uval);
    }

    if (res != val && !_opt.allowTruncation)
        return;
    _opt.result = new IntValue(res);
}

void TransformConstant::map(RealValue *v, Int *i)
{
    std::int64_t ii = _opt.sem->transformRealToInt(v->getValue());
    if ((double(ii) < v->getValue() || double(ii) > v->getValue()) && !_opt.allowTruncation) {
        return;
    }

    IntValue iv(ii);
    Int *intType = new Int();
    intType->setSigned(true);
    intType->setSpan(new Range(63, 0));
    iv.setType(intType);
    map(&iv, i);
}

void TransformConstant::map(StringValue *v, Int *t)
{
    const double d = _string2double(v->getValue());

    RealValue rv(d);
    map(&rv, t);
}

void TransformConstant::map(BitvectorValue *v, Int *t)
{
    const std::string &val = v->getValue();
    std::string newVal     = _logic2bit(val);

    if ((val != newVal || val.size() > 64) && !_opt.allowTruncation) {
        return;
    }

    Type *cvType = hif::semantics::getSemanticType(v, _opt.sem);
    messageAssert(cvType != nullptr, "Cannot type bitvector value", v, _opt.sem);

    const bool cvIsSigned = typeIsSigned(cvType, _opt.sem);
    if (!cvIsSigned)
        newVal = '0' + newVal;

    char *ptr;
    std::int64_t newIntVal = strtoll(newVal.c_str(), &ptr, 2);

    IntValue iVal(newIntVal);
    iVal.setType(_factory.integer(_factory.range(63, 0), cvIsSigned));
    map(&iVal, t);
}
// ////////////////////////////////////////////////////////////////////////////
// Pointer
// ////////////////////////////////////////////////////////////////////////////

void TransformConstant::map(BitvectorValue *v, Pointer *t)
{
    // Works only with "char *"
    if (dynamic_cast<Char *>(t->getType()) == nullptr)
        return;
    _opt.result = new StringValue(v->getValue());
}
// ////////////////////////////////////////////////////////////////////////////
// String
// ////////////////////////////////////////////////////////////////////////////

void TransformConstant::map(IntValue *v, String *)
{
    std::stringstream ss;
    ss << v->getValue();
    std::string s;
    ss >> s;
    StringValue *ret = new StringValue(s.c_str());
    _opt.result      = ret;
}

void TransformConstant::map(BitValue *v, String *)
{
    StringValue *ret = new StringValue();

    switch (v->getValue()) {
    case bit_zero:
        ret->setValue("0");
        break;
    case bit_one:
        ret->setValue("1");
        break;
    case bit_z:
        ret->setValue("Z");
        break;
    case bit_x:
        ret->setValue("X");
        break;
    case bit_dontcare:
        ret->setValue("-");
        break;
    case bit_u:
        ret->setValue("U");
        break;
    case bit_w:
        ret->setValue("W");
        break;
    case bit_l:
        ret->setValue("L");
        break;
    case bit_h:
        ret->setValue("H");
        break;
    default:
        // Error:
        delete ret;
        _opt.result = nullptr;
        return;
    }

    _opt.result = ret;
}

void TransformConstant::map(BoolValue *v, String *)
{
    StringValue *ret = new StringValue(v->getValue() ? "1" : "0");
    _opt.result      = ret;
}

void TransformConstant::map(CharValue *v, String *)
{
    StringValue *ret = new StringValue((std::string("") + v->getValue()).c_str());
    _opt.result      = ret;
}

void TransformConstant::map(RealValue *v, String *)
{
    std::stringstream ss;
    ss << v->getValue();
    std::string s;
    ss >> s;
    StringValue *ret = new StringValue(s.c_str());
    _opt.result      = ret;
}

void TransformConstant::map(StringValue *v, String *) { _opt.result = hif::copy(v); }
// ////////////////////////////////////////////////////////////////////////////
// Real
// ////////////////////////////////////////////////////////////////////////////

void TransformConstant::map(IntValue *v, Real *) { _opt.result = new RealValue(static_cast<double>(v->getValue())); }

void TransformConstant::map(BitValue *v, Real *)
{
    const char val = v->toString()[0];
    const char bit = _logic2bit(val);

    if (val != bit && !_opt.allowTruncation) {
        return;
    }

    _opt.result = new RealValue(val == '1' ? 1.0 : 0.0);
}

void TransformConstant::map(BoolValue *v, Real *) { _opt.result = new RealValue(v->getValue()); }

void TransformConstant::map(CharValue *v, Real *) { _opt.result = new RealValue(v->getValue()); }

void TransformConstant::map(RealValue *v, Real *t)
{
    std::uint64_t size = hif::semantics::spanGetBitwidth(t->getSpan(), _opt.sem);
    const double val        = v->getValue();
    if (size == 0 || size > 32) {
        _opt.result = new RealValue(val);
        return;
    }

    const float f  = static_cast<float>(val);
    const double d = double(f);
    if (d < val && d > val && !_opt.allowTruncation) {
        return;
    }

    _opt.result = new RealValue(d);
}

void TransformConstant::map(StringValue *v, Real *)
{
    const double d = _string2double(v->getValue());
    RealValue *ret = new RealValue(d);
    _opt.result    = ret;
}

void TransformConstant::map(BitvectorValue *v, Real *t)
{
    Type *bvType = hif::semantics::getSemanticType(v, _opt.sem);
    messageAssert(bvType != nullptr, "Expected syntactic type", v, _opt.sem);
    Int *i = _factory.integer(_factory.range(63, 0), hif::typeIsSigned(bvType, _opt.sem), t->isConstexpr());

    map(v, i);
    delete i;

    if (_opt.result == nullptr)
        return;
    IntValue *iv = dynamic_cast<IntValue *>(_opt.result);
    messageAssert(iv != nullptr, "Unexpected conversion", _opt.result, _opt.sem);

    _opt.result = nullptr;
    map(iv, t);
    delete iv;
}
// ////////////////////////////////////////////////////////////////////////////
// Time
// ////////////////////////////////////////////////////////////////////////////

void TransformConstant::map(IntValue *v, Time *)
{
    TimeValue *ret = new TimeValue();
    ret->setValue(static_cast<double>(v->getValue()));
    // Time unit should be always set from caller. Set systemc default.
    ret->setUnit(TimeValue::time_sec);
    _opt.result = ret;
}

void TransformConstant::map(RealValue *v, Time *)
{
    TimeValue *ret = new TimeValue;
    ret->setValue(v->getValue());
    // Time unit should be always set from caller. Set systemc default.
    ret->setUnit(TimeValue::time_sec);
    _opt.result = ret;
}

} // namespace
// //////////////////////////////////////////////////////////////////////////////////////
// public methods
// //////////////////////////////////////////////////////////////////////////////////////
ConstValue *
transformConstant(ConstValue *cvo, Type *to, hif::semantics::ILanguageSemantics *sem, const bool allowTruncation)
{
    if (cvo == nullptr)
        return nullptr;
    messageAssert(to != nullptr, "Unexpected NULl param type", to, sem);

    TransformOptions opt;
    opt.result             = nullptr;
    opt.sem                = sem;
    opt.requiredConstValue = true;
    opt.allowTruncation    = allowTruncation;
    TransformConstant transformConstantMap(opt);

    transformConstantMap.callMap(cvo, to);
    Value *ret = transformConstantMap.getResult();

    ConstValue *retCv = dynamic_cast<ConstValue *>(ret);
    messageAssert(retCv != nullptr || ret == nullptr, "Unexpected non-ConstValue", ret, sem);
    if (retCv != nullptr) {
        delete retCv->setType(hif::copy(to));
        hif::typeSetConstexpr(retCv->getType(), true);
    }

    return retCv;
}

Value *transformValue(Value *vo, Type *to, hif::semantics::ILanguageSemantics *sem, const bool allowTruncation)
{
    if (vo == nullptr)
        return nullptr;
    messageAssert(to != nullptr, "Unexpected nullptr param Type", nullptr, sem);

    TransformOptions opt;
    opt.result             = nullptr;
    opt.sem                = sem;
    opt.requiredConstValue = false;
    opt.allowTruncation    = allowTruncation;
    TransformConstant transformConstantMap(opt);

    transformConstantMap.callMap(vo, to);
    Value *ret = transformConstantMap.getResult();
    if (ret != nullptr) {
        // Note: the type is copied here, thus if passing a fresh object
        // remember to delete it!
        ConstValue *cvo = dynamic_cast<ConstValue *>(ret);
        if (cvo != nullptr) {
            delete cvo->setType(hif::copy(to));

            // Set constexpr flag
            hif::typeSetConstexpr(cvo->getType(), true);
        }
    }

    return ret;
}

} // namespace manipulation
} // namespace hif

// temporary pragma for re-enable warnings
#if (defined __GNUC__)
#    if HIF_DIAGNOSTIC_PUSH_POP
#        pragma GCC diagnostic pop
#    else
#        pragma GCC diagnostic error "-Winline"
#    endif
#endif
