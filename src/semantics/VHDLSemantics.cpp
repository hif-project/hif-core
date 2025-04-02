/// @file VHDLSemantics.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>

#include "hif/BiVisitor.hpp"
#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hifIOUtils.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/SemanticAnalysis.hpp"
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
namespace semantics
{

namespace
{ // unnamed

char _toLower(const char c) { return char(::tolower(int(c))); }

// ///////////////////////////////////////////////////////////////////
// VHDLAnalysis
// ///////////////////////////////////////////////////////////////////
class VHDLAnalysis : public SemanticAnalysis, public BiVisitor<VHDLAnalysis>
{
public:
    VHDLAnalysis(Operator currOperator, Object *srcObj);
    virtual ~VHDLAnalysis();

    /// @name Unary operators
    /// @{

    /// base map
    void map(Object *);

    /// others map
    void map(Bitvector *op);
    void map(Signed *op);
    void map(Unsigned *op);
    void map(Int *op);
    void map(Bool *op);
    void map(Bit *op);
    void map(TypeReference *op);
    void map(Real *op);

    /// @}

    /// @name Binary operators
    /// @{

    /// base map
    void map(Object *, Object *);

    /// Maps to Type.
    void map(Array *op1, Type *op2);
    void map(TypeReference *op1, Type *op2);

    /// Maps to Array.
    void map(Array *op1, Array *op2);
    void map(Type *op1, Array *op2);
    void map(Bitvector *op1, Array *op2);
    void map(Unsigned *op1, Array *op2);
    void map(Signed *op1, Array *op2);
    void map(TypeReference *op1, Array *op2);
    void map(Record *op1, Array *op2);
    void map(String *op1, Array *op2);

    /// Maps to Bitvector.
    void map(Bitvector *array1, Bitvector *t2);
    void map(Array *op1, Bitvector *op2);
    void map(Bit *op1, Bitvector *op2);
    void map(Signed *op1, Bitvector *array1);
    void map(Unsigned *op1, Bitvector *op2);
    void map(TypeReference *op1, Bitvector *op2);
    void map(String *op1, Bitvector *op2);

    /// Maps to Signed.
    void map(Signed *op1, Signed *op2);
    void map(Unsigned *op1, Signed *op2);
    void map(Int *, Signed *op2);
    void map(Bit *op1, Signed *op2);
    void map(Array *op1, Signed *op2);
    void map(Bitvector *op1, Signed *sign);

    /// Maps to Unsigned.
    void map(Unsigned *op1, Unsigned *op2);
    void map(Signed *op1, Unsigned *op2);
    void map(Int *, Unsigned *op2);
    void map(Bit *op1, Unsigned *op2);
    void map(Array *op1, Unsigned *op2);
    void map(Bitvector *op1, Unsigned *op2);

    /// Maps to Int.
    void map(Int *op1, Int *op2);
    void map(Signed *op1, Int *op2);
    void map(Unsigned *op1, Int *op2);
    void map(Real *op1, Int *op2);
    void map(Time *op1, Int *op2);

    /// Maps to Bit.
    void map(Bit *op1, Bit *op2);
    void map(Bool *op1, Bit *op2);
    void map(Unsigned *op1, Bit *op2);
    void map(Signed *op1, Bit *op2);
    void map(Bitvector *op1, Bit *op2);

    /// Maps to Bool.
    void map(Bool *op1, Bool *op2);
    void map(Bit *op1, Bool *op2);

    /// Maps to TypeReference.
    void map(TypeReference *op1, TypeReference *op2);
    void map(Type *op1, TypeReference *op2);
    void map(Bitvector *op1, TypeReference *op2);
    void map(Array *op1, TypeReference *op2);

    /// Maps to Record.
    void map(Record *op1, Record *op2);

    /// Maps to Real
    void map(Real *op1, Real *op2);
    void map(Int *op1, Real *op2);
    void map(Time *op1, Real *op2);

    /// Maps to Time
    void map(Time *op1, Time *op2);
    void map(Int *op1, Time *op2);
    void map(Real *op1, Time *op2);

    /// Maps to String.
    void map(String *op1, String *op2);
    void map(Bitvector *op1, String *op2);
    void map(Char *op1, String *op2);
    void map(Array *op1, String *op2);

    /// Maps to Pointer.
    void map(Pointer *op1, Pointer *op2);

    /// Maps to File.
    void map(File *op1, File *op2);

    /// Maps to Enum.
    void map(Enum *op1, Enum *op2);

    /// Maps to Char.
    void map(Char *op1, Char *op2);
    void map(String *op1, Char *op2);

    /// @}

private:
    /// @brief The semantics.
    VHDLSemantics *_vhdlSem;

    virtual void _callMap(Object *op1);
    virtual void _callMap(Object *op1, Object *op2);

    /// @brief Function to detect if an array represents a standard logic
    /// vector.
    bool _isStandardLogicVector(Bitvector *o);

    /// @brief Function to detect if an array represents a standard ulogic
    /// vector.
    bool _isStandardUlogicVector(Bitvector *o);

    /// @brief Function to detect if an array represents a bit vector
    bool _isBitVectorType(Bitvector *o);

    /// @brief Function to detect if a bit is a VHDL logic bit.
    /// It must have the logic and the resolved flag
    bool _isStandardLogicBit(Bit *b);

    /// @brief Function to detect if a bit is a VHDL ulogic bit.
    /// It must have the logic and not the resolved flag
    bool _isStandardUlogicBit(Bit *b);

    /// Returns true if the operator is a shift.
    bool _isShiftNoRotate(Operator operation);

    /// Returns true if the operator is arithmetic.
    bool _isAllowedArithmeticOrConcat(Operator operation);

    /// Returns a new Array that represent a standard logic vector, with
    /// the given range.
    Bitvector *_makeStandardLogicVector(Range *range);

    /// Returns a new Array that represent a standard ulogic vector, with
    /// the given range.
    Bitvector *_makeStandardUlogicVector(Range *range);

    // disabled.
    VHDLAnalysis(const VHDLAnalysis &);
    VHDLAnalysis &operator=(const VHDLAnalysis &);
};
// ///////////////////////////////////////////////////////////////////
// VHDLCastAnalysis
// ///////////////////////////////////////////////////////////////////
/// This class implements the explicit cast function. Each map function
/// manages a different input values.
class VHDLCastAnalysis : public BiVisitor<VHDLCastAnalysis>
{
public:
    VHDLCastAnalysis(Value *valueToCast, Type *srcType);
    virtual ~VHDLCastAnalysis();

    /// This function return the Value resulting from cast
    /// of the valueToCast parameter, to the castType parameter
    Value *explicitCast(Type *castType);

    // utility functions
    void generateInclude(Value *startingObj);

    // Default map
    void map(Object *, Object *);

    // Maps for casts to typeRef type.
    void map(Type *vi, TypeReference *tr);

    // Maps for casts to array type.
    void map(Array *vi, Array *a);

    // Maps for casts to integer type.
    void map(Int *vi, Int *i);
    void map(Bitvector *vi, Int *i);
    void map(Signed *vi, Int *i);
    void map(Unsigned *vi, Int *i);

    // Maps for cast to Bitvector type
    void map(Bitvector *v1, Bitvector *vector);
    void map(Int *vi, Bitvector *vector);
    void map(Signed *v1, Bitvector *v2);
    void map(Unsigned *v1, Bitvector *vector);
    void map(Bit *vi, Bitvector *vector);

    // Maps for cast to Signed type
    void map(Signed *v1, Signed *v2);
    void map(Int *vi, Signed *vector);
    void map(Bitvector *v1, Signed *v2);
    void map(Unsigned *v1, Signed *v2);

    // Maps for cast to Unsigned type
    void map(Unsigned *vi, Unsigned *vector);
    void map(Int *vi, Unsigned *vector);
    void map(Signed *v1, Unsigned *v2);
    void map(Bitvector *v1, Unsigned *v2);

    // Maps for cast to Pointer type
    void map(Pointer *vi, Pointer *p);
    void map(Int *vi, Pointer *p);

    // Maps for cast to File type
    void map(File *vi, File *f);
    void map(String *vi, File *f);

    // Maps for cast to Bit type
    void map(Bit *bo, Bit *bi);
    void map(Bool *bo, Bit *bi);

    // Maps for cast to Bit type
    void map(Bit *bit, Bool *bo);

private:
    /// The semantics
    ILanguageSemantics *_sem;

    /// The input value object
    Value *_inputVal;

    Type *_sourceType;

    /// The result of cast the mapping
    Value *_result;

    /// The set of libraries to include (eventually).
    std::set<std::string> _neededIncludes;

    /// The factory
    hif::HifFactory _factory;

    Value *_getCastFromValueToType(Value *from, Type *to);
    void _mapVectors(Type *t1, Type *t2);

    // disabled.
    VHDLCastAnalysis(const VHDLCastAnalysis &);
    VHDLCastAnalysis &operator=(const VHDLCastAnalysis &);
}; // end of VHDLCastMapper_t
// ///////////////////////////////////////////////////////////////////
// ValueListAnalysis methods implementation
// ///////////////////////////////////////////////////////////////////

VHDLAnalysis::VHDLAnalysis(Operator currOperator, Object *srcObj)
    : SemanticAnalysis(VHDLSemantics::getInstance(), currOperator, srcObj)
    , BiVisitor<VHDLAnalysis>()
    , _vhdlSem(VHDLSemantics::getInstance())
{
    // ntd
}

VHDLAnalysis::~VHDLAnalysis()
{
    // ntd
}

// //////////////////////////
// Utility functions
// //////////////////////////
bool VHDLAnalysis::_isStandardLogicVector(Bitvector *o) { return (o && o->isLogic() && o->isResolved()); }

bool VHDLAnalysis::_isStandardUlogicVector(Bitvector *o) { return (o && o->isLogic() && !o->isResolved()); }

bool VHDLAnalysis::_isBitVectorType(Bitvector *o) { return (o && !o->isLogic() && !o->isResolved()); }

bool VHDLAnalysis::_isStandardLogicBit(Bit *b) { return (b->isLogic() && b->isResolved()); }

bool VHDLAnalysis::_isStandardUlogicBit(Bit *b) { return (b->isLogic() && !b->isResolved()); }

bool VHDLAnalysis::_isShiftNoRotate(Operator operation)
{
    return hif::operatorIsShift(operation) && !hif::operatorIsRotate(operation);
}

bool VHDLAnalysis::_isAllowedArithmeticOrConcat(Operator operation)
{
    return (hif::operatorIsArithmetic(operation) || operation == op_concat) && operation != op_abs &&
           operation != op_rem && operation != op_log;
}

Bitvector *VHDLAnalysis::_makeStandardLogicVector(Range *range)
{
    Bitvector *array = new Bitvector();
    array->setSpan(hif::copy(range));
    array->setLogic(true);
    array->setResolved(true);
    return array;
}

Bitvector *VHDLAnalysis::_makeStandardUlogicVector(Range *range)
{
    Bitvector *array = new Bitvector();
    array->setSpan(hif::copy(range));
    array->setLogic(true);
    array->setResolved(false);
    return array;
}

// //////////////////////////
// Unary maps
// //////////////////////////
void VHDLAnalysis::map(Object *)
{
    // ntd
}

void VHDLAnalysis::map(Bitvector *op)
{
    const bool isStdLV = _isStandardLogicVector(op);

    // VHDL Logical not returns 0, if value is not zero, 1 otherwise.
    if (_currOperator == op_bnot) {
        if (!_isBitVectorType(op) && !isStdLV)
            return;

        _result.operationPrecision = hif::copy(op);
        _result.returnedType       = hif::copy(op);
        return;
    }

    if (_currOperator == op_plus || _currOperator == op_minus) {
        if (!isStdLV && !_isStandardUlogicVector(op) && !op->isConstexpr()) {
            return;
        }

        _result.operationPrecision = hif::copy(op);
        _result.returnedType       = hif::copy(op);
        return;
    }

    if (hif::operatorIsReduce(_currOperator)) {
        if (!isStdLV)
            return;

        // Creating return type "ux01"
        Bit *rt = new Bit();
        rt->setLogic(true);
        rt->setResolved(true);

        _result.operationPrecision = hif::copy(op);
        _result.returnedType       = rt;
        return;
    }
}

void VHDLAnalysis::map(Signed *op)
{
    if (_currOperator != op_plus && _currOperator != op_minus && _currOperator != op_bnot && _currOperator != op_abs) {
        return;
    }

    // result and precision always the same signed
    _result.operationPrecision = hif::copy(op);
    _result.returnedType       = hif::copy(op);
}

void VHDLAnalysis::map(Unsigned *op)
{
    if (_currOperator != op_plus && _currOperator != op_minus && _currOperator != op_bnot) {
        return;
    }

    // result and precision always the same unsigned
    _result.operationPrecision = hif::copy(op);
    _result.returnedType       = hif::copy(op);
    return;
}

void VHDLAnalysis::map(Int *op)
{
    // non unary operators not allowed
    if (_currOperator != op_plus && _currOperator != op_minus)
        return;

    _result.returnedType = hif::copy(op);
    ;
    _result.operationPrecision = hif::copy(op);
}

void VHDLAnalysis::map(Bool *op)
{
    if (_currOperator != op_not)
        return;

    _result.returnedType       = hif::copy(op);
    _result.operationPrecision = hif::copy(op);
}

void VHDLAnalysis::map(Bit *op)
{
    // for all
    if ((_currOperator == op_not && !op->isLogic()) || (_currOperator == op_bnot)) {
        _result.returnedType       = hif::copy(op);
        _result.operationPrecision = hif::copy(op);
        return;
    }

    // for ulogic
    if (_isStandardUlogicBit(op) || op->isConstexpr()) {
        // check library and operation
        if (_currOperator != op_plus && _currOperator != op_minus)
            return;

        _result.returnedType       = hif::copy(op);
        _result.operationPrecision = hif::copy(op);
    }
}

void VHDLAnalysis::map(TypeReference *op) { _map(op); }

void VHDLAnalysis::map(Real *op)
{
    _result.returnedType       = hif::copy(op);
    _result.operationPrecision = hif::copy(op);
}
// //////////////////////////
// Bynary maps
// //////////////////////////
void VHDLAnalysis::map(Object *, Object *)
{
    // ntd
}

void VHDLAnalysis::map(Array *op1, Type *op2)
{
    if (_currOperator != op_concat)
        return;

    Type *t1  = op1->getType();
    Range *r1 = op1->getSpan();
    if (t1 == nullptr || r1 == nullptr)
        return;
    if (!hif::equals(t1, op2))
        return;

    // Build the result
    Array *resultArray = new Array();
    resultArray->setSpan(hif::manipulation::rangeGetIncremented(r1, _sem, 1));
    resultArray->setSigned(op1->isSigned());
    resultArray->setType(hif::copy(op2));
    _result.returnedType       = resultArray;
    _result.operationPrecision = hif::copy(op1);
}

void VHDLAnalysis::map(Array *op1, Array *op2)
{
    Range *span1 = op1->getSpan();
    Range *span2 = op2->getSpan();
    messageAssert(span1 != nullptr && span2 != nullptr, "Missing type span(s)", _srcObj, _sem);

    // NB: use of return inside the if (and not if else if else if), is
    // because there could be more than one library set. If the checks
    // for the operations in one library fails, there could be the
    // possibility that the check success for another library.
    //
    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (isRelational || isAssignment) {
        Range *resultSpan = rangeGetMax(span1, span2, _sem);
        Array *result     = new Array();
        result->setSpan(resultSpan);
        result->setSigned(op1->isSigned() || op2->isSigned());

        ILanguageSemantics::ExpressionTypeInfo r =
            _sem->getExprType(op1->getType(), op2->getType(), _currOperator, _srcObj);
        result->setType(r.operationPrecision);
        r.operationPrecision = nullptr;

        _result.operationPrecision = result;
        if (isAssignment)
            _result.returnedType = hif::copy(op1);
        else /*relational*/
            _result.returnedType = new Bool();
        return;
    }

    // Result is set to the precision of the array which is not native
    // concatenation permitted on array of the same types
    if (_currOperator == op_concat) {
        if (dynamic_cast<Array *>(op1->getType()) != nullptr || dynamic_cast<Array *>(op2->getType()) != nullptr) {
            // not allowed concat between arrays of array
            return;
        }

        Type *t1 = op1->getType();
        Type *t2 = op2->getType();
        messageAssert(t1 != nullptr && t2 != nullptr, "Missing array type(s)", _srcObj, _sem);
        analyzeOperands(t1, t2);
        if (_result.returnedType == nullptr) {
            // operation not allowed
            return;
        }

        Type *resultArrayType   = nullptr;
        // NOTE: was not recoursive
        Type *precisionBaseType = getBaseType(_result.operationPrecision, false, _sem, false);
        Array *arr              = dynamic_cast<Array *>(precisionBaseType);
        Bitvector *bv           = dynamic_cast<Bitvector *>(precisionBaseType);

        if (arr == nullptr && bv == nullptr) {
            delete _result.operationPrecision;
            delete _result.returnedType;
            _result.operationPrecision = nullptr;
            _result.returnedType       = nullptr;
            messageError("Unexpected case", precisionBaseType, nullptr);
        }

        // NOTE: was not recoursive
        Type *arr1BaseType = getBaseType(t1, false, _sem, false);
        Type *arr2BaseType = getBaseType(t2, false, _sem, false);
        const bool isBit1  = (dynamic_cast<Bit *>(arr1BaseType) != nullptr);
        const bool isBit2  = (dynamic_cast<Bit *>(arr2BaseType) != nullptr);
        if (bv != nullptr) {
            if (isBit1 && isBit2) {
                Bit *ret = new Bit();
                ret->setLogic(bv->isLogic());
                ret->setConstexpr(bv->isConstexpr());
                ret->setResolved(bv->isResolved());
                resultArrayType = ret;
            } else // arrays of arrays/bv
            {
                resultArrayType            = _result.operationPrecision;
                _result.operationPrecision = nullptr;
            }
        } else // arr != nullptr
        {
            resultArrayType = arr->getType();
            arr->setType(nullptr);
        }
        delete _result.operationPrecision;
        delete _result.returnedType;
        _result.returnedType = nullptr;

        Range *resultSpan = rangeGetSum(span1, span2, _sem);
        if (resultSpan == nullptr) {
            delete resultArrayType;
            return;
        }

        // Build the result and the precision.
        Array *resultArray = new Array();
        resultArray->setSpan(resultSpan);
        resultArray->setType(resultArrayType);
        resultArray->setSigned(op1->isSigned() || op2->isSigned());
        _result.returnedType       = resultArray;
        _result.operationPrecision = hif::copy(op1);
        return;
    }
}

void VHDLAnalysis::map(Type *op1, Array *op2) { map(op2, op1); }

void VHDLAnalysis::map(TypeReference *op1, Type *op2) { _map(op1, op2); }

void VHDLAnalysis::map(Bitvector *op1, Array *op2)
{
    Type *arrType = op2->getType();
    Bit *b        = dynamic_cast<Bit *>(arrType);
    if (b != nullptr) {
        Bitvector *dummyBV = new Bitvector();
        dummyBV->setLogic(b->isLogic() || op1->isLogic());
        dummyBV->setResolved(b->isResolved() || op1->isResolved());
        dummyBV->setSigned(op1->isSigned() || op2->isSigned());

        Range *resSpan = nullptr;
        if (_currOperator == op_concat || _currOperator == op_mult) {
            resSpan = rangeGetSum(op1->getSpan(), op2->getSpan(), _sem);
        } else {
            resSpan = rangeGetMax(op1->getSpan(), op2->getSpan(), _sem);
        }

        if (hif::operatorIsRelational(_currOperator)) {
            _result.returnedType = new Bool();
            dummyBV->setSpan(resSpan);
            _result.operationPrecision = dummyBV;
        } else {
            dummyBV->setSpan(resSpan);
            _result.returnedType       = dummyBV;
            _result.operationPrecision = hif::copy(dummyBV);
        }

        return;
    }

    // No bit arrays
    if (_currOperator == op_concat) {
        _currOperator = op_conv;
        analyzeOperands(arrType, op1);
        _currOperator = op_concat;

        if (_result.returnedType == nullptr)
            return;
        delete _result.returnedType;
        _result.returnedType = nullptr;
        delete _result.operationPrecision;
        _result.operationPrecision = nullptr;

        Array *ret = hif::copy(op2);
        delete ret->setSpan(hif::manipulation::rangeGetIncremented(op2->getSpan(), _sem, 1));
        _result.returnedType       = ret;
        _result.operationPrecision = hif::copy(ret);
        return;
    }
}

void VHDLAnalysis::map(Unsigned *op1, Array *op2) { map(op2, op1); }

void VHDLAnalysis::map(Signed *op1, Array *op2) { map(op2, op1); }

void VHDLAnalysis::map(TypeReference *op1, Array *op2) { map(op1, static_cast<Type *>(op2)); }

void VHDLAnalysis::map(Record *op1, Array *op2)
{
    const bool strict = _sem->getStrictTypeChecks();
    if (strict || (!hif::operatorIsAssignment(_currOperator) && _currOperator != op_eq && _currOperator != op_case_eq &&
                   _currOperator != op_neq && _currOperator != op_case_neq)) {
        map(static_cast<Type *>(op1), op2);
        return;
    }

    _result.returnedType       = hif::copy(op1);
    _result.operationPrecision = hif::copy(op1);
}

void VHDLAnalysis::map(String *op1, Array *op2)
{
    Type *op2Base = getBaseType(op2->getType(), false, _sem, false);
    if (dynamic_cast<Char *>(op2Base) == nullptr)
        return;

    if (hif::operatorIsRelational(_currOperator)) {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = new Bool();
        return;
    }

    if (hif::operatorIsAssignment(_currOperator)) {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = hif::copy(op1);
        return;
    }

    if (_currOperator == op_concat) {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = hif::copy(op1);
        return;
    }
}

void VHDLAnalysis::map(Bitvector *op1, Bitvector *op2)
{
    Range *span1 = op1->getSpan();
    Range *span2 = op2->getSpan();
    messageAssert(span1 != nullptr && span2 != nullptr, "Missing type span(s)", _srcObj, _sem);

    // NB: use of return inside the if (and not if else if else if), is
    // because there could be more than one library set. If the checks
    // for the operations in one library fails, there could be the
    // possibility that the check success for another library.
    //
    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (isRelational || isAssignment) {
        Range *resultRange = nullptr;
        if (isAssignment) {
            Value *s1            = spanGetSize(span1, _sem);
            Value *s2            = spanGetSize(span2, _sem);
            const bool areEquals = hif::equals(s1, s2);
            delete s1;
            delete s2;
            if (!areEquals && _sem->getStrictTypeChecks())
                return;
            resultRange = hif::copy(span1);
        } else {
            resultRange = rangeGetMax(span1, span2, _sem);
        }

        messageAssert(resultRange != nullptr, "Unexpected case", _srcObj, _sem);
        Bitvector *resultArray = new Bitvector();
        resultArray->setSpan(resultRange);
        resultArray->setSigned(op1->isSigned() || op2->isSigned());
        resultArray->setResolved(op1->isResolved() || op2->isResolved());
        resultArray->setConstexpr(op1->isConstexpr() && op2->isConstexpr());
        resultArray->setLogic(op1->isLogic() || op2->isLogic());

        _result.operationPrecision = resultArray;
        if (isAssignment)
            _result.returnedType = hif::copy(op1);
        else /*relational*/
            _result.returnedType = new Bool();
        return;
    }
    // Result is set to the precision of the array which is not native
    Bitvector *resultPrecision = op1->isConstexpr() ? op2 : op1;

    // concatenation permitted on array of the same types
    if (_currOperator == op_concat) {
        hif::EqualsOptions opt;
        opt.checkSpans      = false;
        opt.checkSignedFlag = false;
        // The result is an array with a range equal to the sum to the
        // ranges of the operands
        Range *resultRange  = rangeGetSum(span1, span2, _sem);

        messageAssert(resultRange != nullptr, "Unexpected case (2)", _srcObj, _sem);
        // Build the result and the precision.
        Bitvector *resultArray = hif::copy(resultPrecision);
        delete resultArray->setSpan(resultRange);
        _result.returnedType       = resultArray;
        _result.operationPrecision = hif::copy(resultPrecision);
        return;
    }

    const bool isBv1      = _isBitVectorType(op1);
    const bool isBv2      = _isBitVectorType(op2);
    const bool std_logic  = _isStandardLogicVector(op1) && _isStandardLogicVector(op2);
    const bool std_ulogic = _isStandardUlogicVector(op1) && _isStandardUlogicVector(op2);

    // logical operators permitted on bit vectors
    if (hif::operatorIsBitwise(_currOperator)) {
        if (isBv1 && isBv2) {
            Range *result_range = rangeGetMax(span1, span2, _sem);
            if (!result_range)
                return;
            Bitvector *result_array = new Bitvector();
            result_array->setSpan(result_range);
            result_array->setSigned(op1->isSigned() || op2->isSigned());
            result_array->setLogic(false);
            result_array->setResolved(false);
            _result.operationPrecision = result_array;
            _result.returnedType       = hif::copy(result_array);
        } else if (std_logic || std_ulogic || op1->isConstexpr() || op2->isConstexpr()) {
            // logical operations on std_logic_vector and std_ulogic_vector are in
            // std_logic_1164 library

            std::uint64_t r1 = spanGetBitwidth(span1, _sem);
            std::uint64_t r2 = spanGetBitwidth(span2, _sem);

            if (r1 != 0 && r2 != 0 && r1 != r2)
                return;
            Range *resultRange = rangeGetMax(span1, span2, _sem);
            messageAssert(resultRange != nullptr, "Unexpected case (3)", _srcObj, _sem);
            Bitvector *resultArray = new Bitvector();
            resultArray->setSpan(resultRange);
            resultArray->setSigned(op1->isSigned() || op2->isSigned());
            resultArray->setLogic(op1->isLogic() || op2->isLogic());
            resultArray->setResolved(std_logic);
            _result.operationPrecision = resultArray;
            _result.returnedType       = hif::copy(resultArray);
        }

        return;
    }

    if (_sem->getStrictTypeChecks()) {
        // Should be signed/unsigned.. no other operation allowed with strict checks
        return;
    }

    if (!std_logic && !std_ulogic && !op1->isConstexpr() && !op2->isConstexpr())
        return;

    Range *resRange = nullptr;
    // determine range looking the operation
    if (_currOperator == op_plus || _currOperator == op_minus) {
        resRange = rangeGetMax(span1, span2, _sem);
    } else if (_currOperator == op_mult) {
        resRange = rangeGetSum(span1, span2, _sem);
    }

    // build result and precision
    if (resRange != nullptr) {
        Bitvector *resultType = std_logic ? _makeStandardLogicVector(resRange) : _makeStandardUlogicVector(resRange);
        resultType->setSigned(op1->isSigned() || op2->isSigned());
        delete resRange;
        _result.returnedType       = hif::copy(resultType);
        _result.operationPrecision = resultType;
    }
}

void VHDLAnalysis::map(Array *op1, Bitvector *op2) { map(op2, op1); }

void VHDLAnalysis::map(Bit *op1, Bitvector *op2) { map(op2, op1); }

void VHDLAnalysis::map(Signed *op1, Bitvector *op2)
{
    // like map(signed, signed)
    Signed dummySigned;
    dummySigned.setSpan(hif::copy(op2->getSpan()));
    map(op1, &dummySigned);
}

void VHDLAnalysis::map(Unsigned *op1, Bitvector *op2)
{
    // like map(unsigned, unsigned)
    Unsigned dummyUnsigned;
    dummyUnsigned.setSpan(hif::copy(op2->getSpan()));
    map(op1, &dummyUnsigned);
}

void VHDLAnalysis::map(TypeReference *op1, Bitvector *op2) { map(op1, static_cast<Type *>(op2)); }

void VHDLAnalysis::map(String *op1, Bitvector *op2)
{
    if (_sem->getStrictTypeChecks())
        return;

    // only without strict checks
    if (_currOperator == op_concat) {
        String *ret = new String();
        ret->setConstexpr(op1->isConstexpr() && op2->isConstexpr());
        if (op1->getSpanInformation() != nullptr && op2->getSpan() != nullptr) {
            ret->setSpanInformation(rangeGetSum(op1->getSpanInformation(), op2->getSpan(), _sem));
        }
        _result.returnedType       = ret;
        _result.operationPrecision = hif::copy(ret);
        return;
    }

    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (isRelational || isAssignment) {
        // arbitrary precision
        _result.operationPrecision = new String();
        if (isAssignment)
            _result.returnedType = hif::copy(op1);
        else /*relational*/
            _result.returnedType = new Bool();
        return;
    }
}

void VHDLAnalysis::map(Signed *op1, Signed *op2)
{
    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (isRelational || isAssignment) {
        if (isAssignment) {
            // only equals spans
            Value *s1            = spanGetSize(op1->getSpan(), _sem);
            Value *s2            = spanGetSize(op2->getSpan(), _sem);
            const bool areEquals = hif::equals(s1, s2);
            delete s1;
            delete s2;
            if (!areEquals && _sem->getStrictTypeChecks())
                return;
        }

        Type *resultType      = nullptr;
        Type *resultPrecision = hif::copy(op1);
        if (isAssignment) {
            resultType = hif::copy(op1);
        } else {
            resultType         = new Bool();
            Range *resultRange = rangeGetMax(op1->getSpan(), op2->getSpan(), _sem);
            hif::typeSetSpan(resultPrecision, resultRange, _sem);
        }
        _result.operationPrecision = resultPrecision;
        _result.returnedType       = resultType;
        return;
    }

    if (op1->getSpan() == nullptr || op2->getSpan() == nullptr)
        return;

    // Returns a Signed with range dependent by operation
    // If range contains an expression operations are allowed
    // only on equivalent range.
    if (_isAllowedArithmeticOrConcat(_currOperator) || hif::operatorIsBitwise(_currOperator) ||
        _isShiftNoRotate(_currOperator)) {
        Range *resRange = nullptr;
        if (_currOperator == op_div || _currOperator == op_rem || _currOperator == op_mod) {
            // result range: op1'length - 1
            resRange = hif::copy(op1->getSpan());
        } else if (_currOperator == op_mult || _currOperator == op_concat) {
            // result range: op1'length + op2'length - 1
            resRange = rangeGetSum(op1->getSpan(), op2->getSpan(), _sem);
        } else {
            // result range: max(op1'length , op2'length) - 1
            resRange = rangeGetMax(op1->getSpan(), op2->getSpan(), _sem);
        }

        // Control that returned range have been established.
        if (resRange == nullptr)
            return;

        Signed *result = new Signed();
        result->setSpan(resRange);

        _result.operationPrecision = hif::copy(result);
        _result.returnedType       = result;
        return;
    }
}

void VHDLAnalysis::map(Unsigned *op1, Signed *op2) { map(op2, op1); }

void VHDLAnalysis::map(Int *op1, Signed *op2) { map(op2, op1); }

void VHDLAnalysis::map(Bit *op1, Signed *op2) { map(op2, op1); }

void VHDLAnalysis::map(Array *op1, Signed *op2)
{
    if (_currOperator == op_eq) {
        // like map(signed, signed)
        if (dynamic_cast<Bit *>(op1->getType()) == nullptr)
            return;

        Signed dummySigned;
        dummySigned.setSpan(hif::copy(op1->getSpan()));
        map(&dummySigned, op2);
        return;
    }

    if (hif::operatorIsAssignment(_currOperator)) {
        if (dynamic_cast<Bit *>(op1->getType()) == nullptr)
            return;
        Signed *signedResult = new Signed();
        signedResult->setSpan(hif::copy(op1->getSpan()));
        _result.returnedType       = signedResult;
        _result.operationPrecision = hif::copy(signedResult);
        return;
    }

    if (_currOperator == op_concat) {
        Type *arrType = op1->getType();

        _currOperator = op_conv;
        analyzeOperands(arrType, op2);
        _currOperator = op_concat;

        if (_result.returnedType == nullptr)
            return;
        delete _result.returnedType;
        _result.returnedType = nullptr;
        delete _result.operationPrecision;
        _result.operationPrecision = nullptr;

        Array *ret = hif::copy(op1);
        delete ret->setSpan(hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem, 1));
        _result.returnedType       = ret;
        _result.operationPrecision = hif::copy(ret);
        return;
    }
}

void VHDLAnalysis::map(Bitvector *op1, Signed *op2)
{
    // like map(signed, signed)
    if (hif::operatorIsAssignment(_currOperator) && _sem->getStrictTypeChecks()) {
        // ref design can_oc, mlite
        return;
    }

    Signed dummySigned;
    dummySigned.setSpan(hif::copy(op1->getSpan()));
    map(&dummySigned, op2);
}

void VHDLAnalysis::map(Unsigned *op1, Unsigned *op2)
{
    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (isRelational || isAssignment) {
        if (hif::operatorIsAssignment(_currOperator)) {
            // only equals spans
            Value *s1            = spanGetSize(op1->getSpan(), _sem);
            Value *s2            = spanGetSize(op2->getSpan(), _sem);
            const bool areEquals = hif::equals(s1, s2);
            delete s1;
            delete s2;
            if (!areEquals && _sem->getStrictTypeChecks())
                return;
        }

        Type *resultType      = nullptr;
        Type *resultPrecision = hif::copy(op1);
        if (isAssignment) {
            resultType = hif::copy(op1);
        } else {
            resultType         = new Bool();
            Range *resultRange = rangeGetMax(op1->getSpan(), op2->getSpan(), _sem);
            hif::typeSetSpan(resultPrecision, resultRange, _sem);
        }
        _result.operationPrecision = resultPrecision;
        _result.returnedType       = resultType;
        return;
    }

    if (op1->getSpan() == nullptr || op2->getSpan() == nullptr)
        return;

    // Returns an Unsigned with range dependent by operation
    // If range contains an expression operations are allowed
    // only on equivalent range.
    if (_isAllowedArithmeticOrConcat(_currOperator) || hif::operatorIsBitwise(_currOperator) ||
        _isShiftNoRotate(_currOperator)) {
        Range *resRange = nullptr;
        if (_currOperator == op_div || _currOperator == op_rem || _currOperator == op_mod) {
            // result range: op1'length - 1
            resRange = hif::copy(op1->getSpan());
        } else if (_currOperator == op_mult || _currOperator == op_concat) {
            // result range: op1'length + op2'length - 1
            resRange = rangeGetSum(op1->getSpan(), op2->getSpan(), _sem);
        } else {
            // result range: max(op1'length , op2'length) - 1
            resRange = rangeGetMax(op1->getSpan(), op2->getSpan(), _sem);
        }

        // Control that returned range have been established.
        if (resRange == nullptr)
            return;
        Unsigned *result = new Unsigned();
        result->setSpan(resRange);

        _result.operationPrecision = hif::copy(result);
        _result.returnedType       = result;
        return;
    }
}

void VHDLAnalysis::map(Signed *op1, Unsigned *op2)
{
    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (isRelational || isAssignment) {
        _result.operationPrecision = hif::copy(op1);
        if (isAssignment)
            _result.returnedType = hif::copy(op1);
        else /*relational*/
            _result.returnedType = new Bool();
        return;
    }

    if (op1->getSpan() == nullptr || op2->getSpan() == nullptr)
        return;

    // check libraries
    if (_currOperator == op_plus || _currOperator == op_minus || _currOperator == op_mult ||
        _isShiftNoRotate(_currOperator)) {
        // Returns a Signed with range dependent by operation
        // If range contains an expression operations are allowed
        // only on equivalent range.

        /// the unsigned is converted to signed adding one bit
        Range *resRange = nullptr;
        Range *r1       = op1->getSpan();
        Range *r2       = hif::manipulation::rangeGetIncremented(op2->getSpan(), _sem, 1);
        if (_currOperator == op_mult) {
            // result range: op1'length + op2'length - 1
            resRange = rangeGetSum(r1, r2, _sem);
            delete r2;
        } else {
            // result range: max(op1'length , op2'length) - 1
            resRange = rangeGetMax(r1, r2, _sem);
            delete r2;
        }

        // Check that returned range have been established.
        messageAssert(resRange != nullptr, "Expected result range", _srcObj, _sem);
        Signed *result = new Signed();
        result->setSpan(resRange);

        _result.operationPrecision = hif::copy(result);
        _result.returnedType       = result;
        return;
    }
}

void VHDLAnalysis::map(Int *op1, Unsigned *op2) { map(op2, op1); }

void VHDLAnalysis::map(Bit *op1, Unsigned *op2) { map(op2, op1); }

void VHDLAnalysis::map(Array *op1, Unsigned *op2)
{
    if (_currOperator == op_eq) {
        // like map(unsigned, unsigned)
        if (dynamic_cast<Bit *>(op1->getType()) == nullptr)
            return;

        Unsigned dummyUnsigned;
        dummyUnsigned.setSpan(hif::copy(op1->getSpan()));
        map(&dummyUnsigned, op2);
        return;
    }

    if (hif::operatorIsAssignment(_currOperator)) {
        if (dynamic_cast<Bit *>(op1->getType()) == nullptr)
            return;
        Unsigned *unsignedResult = new Unsigned();
        unsignedResult->setSpan(hif::copy(op1->getSpan()));
        _result.returnedType       = unsignedResult;
        _result.operationPrecision = hif::copy(unsignedResult);
        return;
    }

    if (_currOperator == op_concat) {
        Type *arrType = op1->getType();

        _currOperator = op_conv;
        analyzeOperands(arrType, op2);
        _currOperator = op_concat;

        if (_result.returnedType == nullptr)
            return;
        delete _result.returnedType;
        _result.returnedType = nullptr;
        delete _result.operationPrecision;
        _result.operationPrecision = nullptr;

        Array *ret = hif::copy(op1);
        delete ret->setSpan(hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem));
        _result.returnedType       = ret;
        _result.operationPrecision = hif::copy(ret);
        return;
    }
}

void VHDLAnalysis::map(Bitvector *op1, Unsigned *op2)
{
    // like map(unsigned, unsigned)
    if (hif::operatorIsAssignment(_currOperator) && _sem->getStrictTypeChecks()) {
        // ref design can_oc, mlite
        return;
    }

    Unsigned dummyUnsigned;
    dummyUnsigned.setSpan(hif::copy(op1->getSpan()));
    map(&dummyUnsigned, op2);
}

void VHDLAnalysis::map(Int *op1, Int *op2)
{
    bool sign = op1->isSigned() || op2->isSigned();

    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (isRelational || isAssignment) {
        // returned precision is the max precision between operands
        Int *retPrecision = new Int();
        retPrecision->setSigned(sign);
        retPrecision->setConstexpr(op1->isConstexpr() && op2->isConstexpr());
        Range *retSpan = rangeGetMax(op1->getSpan(), op2->getSpan(), _sem);
        retPrecision->setSpan(retSpan);
        _result.operationPrecision = retPrecision;
        if (isAssignment)
            _result.returnedType = hif::copy(op1);
        else /*relational*/
            _result.returnedType = new Bool();
        return;
    }

    // check signed
    Int *ret = new Int();
    ret->setSpan(new Range(31, 0));

    _result.operationPrecision = ret;
    _result.returnedType       = hif::copy(ret);
}

void VHDLAnalysis::map(Signed *op1, Int * /*op2*/)
{
    // relational operators permitted
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    if (isAssignment)
        return;

    const bool isRelational = hif::operatorIsRelational(_currOperator);
    if (isRelational) {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = new Bool();
        return;
    }

    if (_isShiftNoRotate(_currOperator)) {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = hif::copy(op1);
        return;
    }

    if (op1->getSpan() == nullptr)
        return;

    // check libraries
    if (_isAllowedArithmeticOrConcat(_currOperator)) {
        // Returns a Signed with range dependent by operation
        // If range contains an expression operations are allowed
        // only on equivalent range.
        Range *resRange = nullptr;
        if (_currOperator == op_mult) {
            // result range: 2 * op1'length
            resRange = rangeGetSum(op1->getSpan(), op1->getSpan(), _sem);
        } else {
            // result range: op1'length - 1
            resRange = hif::copy(op1->getSpan());
        }

        messageAssert(resRange != nullptr, "Expected result range", _srcObj, _sem);
        Signed *result = new Signed();
        result->setSpan(resRange);
        _result.operationPrecision = hif::copy(result);
        _result.returnedType       = result;
        return;
    }
}

void VHDLAnalysis::map(Unsigned *op1, Int * /*op2*/)
{
    // relational operators permitted
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    if (isAssignment)
        return;

    const bool isRelational = hif::operatorIsRelational(_currOperator);
    if (isRelational) {
        if (hif::operatorIsAssignment(_currOperator))
            return;
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = new Bool();
        return;
    }

    if (_isShiftNoRotate(_currOperator)) {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = hif::copy(op1);
        return;
    }

    if (op1->getSpan() == nullptr)
        return;

    // check libraries
    if (_isAllowedArithmeticOrConcat(_currOperator)) {
        // Returns a Signed with range dependent by operation
        // If range contains an expression operations are allowed
        // only on equivalent range.
        Range *resRange = nullptr;
        if (_currOperator == op_mult) {
            // result range: 2 * op1'length
            resRange = rangeGetSum(op1->getSpan(), op1->getSpan(), _sem);
        } else {
            // result range: op1'length - 1
            resRange = hif::copy(op1->getSpan());
        }

        messageAssert(resRange != nullptr, "Expected result range", _srcObj, _sem);
        Unsigned *result = new Unsigned();
        result->setSpan(resRange);

        _result.operationPrecision = hif::copy(result);
        _result.returnedType       = result;
    }
}

void VHDLAnalysis::map(Real *op1, Int *op2) { map(op2, op1); }

void VHDLAnalysis::map(Time * /*op1*/, Int * /*op2*/)
{
    if (_currOperator != op_mult && _currOperator != op_div)
        return;

    _result.returnedType       = new Time();
    _result.operationPrecision = new Time();
}

void VHDLAnalysis::map(Bit *op1, Bit *op2)
{
    // Different types are allowed only if at least an operator is native
    if (!op1->isConstexpr() && !op2->isConstexpr() && !hif::equals(op1, op2)) {
        if (op1->isLogic() != op2->isLogic())
            return;
    }

    // Result is set to the precision of bit which is not native
    Bit *resultPrecision = op1->isConstexpr() ? hif::copy(op2) : hif::copy(op1);
    resultPrecision->setLogic(op1->isLogic() || op2->isLogic());
    resultPrecision->setResolved(op1->isResolved() || op2->isResolved());

    // relational operators permitted
    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    if (isRelational || isAssignment) {
        _result.operationPrecision = resultPrecision;
        if (isAssignment)
            _result.returnedType = hif::copy(op1);
        else /*relational*/
            _result.returnedType = new Bool();
        return;
    }

    if (_currOperator == op_concat) {
        // concatenation returns a 2 bit array
        Bitvector *resultType = new Bitvector();
        resultType->setSpan(new Range(1, 0));
        resultType->setSigned(false);
        resultType->setLogic(resultPrecision->isLogic());
        resultType->setResolved(resultPrecision->isResolved());
        delete resultPrecision;
        _result.returnedType       = resultType;
        _result.operationPrecision = hif::copy(resultType);
        return;
    }

    if ((hif::operatorIsLogical(_currOperator) && !resultPrecision->isLogic()) || (_currOperator == op_bxor)) {
        // BIT or LOGIC or ULOGIC
        _result.operationPrecision = resultPrecision;
        _result.returnedType       = hif::copy(resultPrecision);
        return;
    }

    if (resultPrecision->isLogic() && !resultPrecision->isResolved() &&
        (_currOperator == op_plus || _currOperator == op_minus || _currOperator == op_mult)) {
        // ULOGIC: also +, -, *
        _result.operationPrecision = resultPrecision;
        _result.returnedType       = hif::copy(resultPrecision);
        return;
    }

    if (resultPrecision->isLogic() && (hif::operatorIsBitwise(_currOperator) || _currOperator == op_xor)) {
        _result.operationPrecision = resultPrecision;
        _result.returnedType       = hif::copy(resultPrecision);
        return;
    }
}

void VHDLAnalysis::map(Bool *op1, Bit *op2)
{
    if (!_vhdlSem->usingPsl())
        return;

    Bit b;
    b.setConstexpr(op1->isConstexpr());
    b.setLogic(op2->isLogic());
    map(&b, op2);
}

void VHDLAnalysis::map(Unsigned *op1, Bit *op2)
{
    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (isRelational || isAssignment) {
        _result.operationPrecision = hif::copy(op1);
        if (isAssignment)
            _result.returnedType = hif::copy(op1);
        else /*relational*/
            _result.returnedType = new Bool();
        return;
    }

    // only Ulogic/logic and native are supported
    if (!op2->isLogic() && !op2->isConstexpr())
        return;

    if (_currOperator == op_concat) {
        Range *r1 = op1->getSpan();
        if (r1 == nullptr)
            return;
        Range *resultSpan = hif::manipulation::rangeGetIncremented(r1, _sem, 1);
        Unsigned *result  = new Unsigned();
        result->setSpan(resultSpan);
        _result.operationPrecision = hif::copy(op1); // TODO check
        _result.returnedType       = result;
        return;
    }

    if (_currOperator == op_plus || _currOperator == op_minus) {
        // Returns a STD_LOGIC_VECTOR of op1'length.
        Bitvector *result          = _makeStandardLogicVector(op1->getSpan());
        _result.operationPrecision = hif::copy(result);
        _result.returnedType       = result;
        return;
    }
}

void VHDLAnalysis::map(Signed *op1, Bit *op2)
{
    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (isRelational || isAssignment) {
        _result.operationPrecision = hif::copy(op1);
        if (isAssignment)
            _result.returnedType = hif::copy(op1);
        else /*relational*/
            _result.returnedType = new Bool();
        return;
    }

    // only Ulogic/logic and native are supported
    if (!op2->isLogic() && !op2->isConstexpr())
        return;

    if (_currOperator == op_concat) {
        Range *r1 = op1->getSpan();
        if (r1 == nullptr)
            return;
        Range *resultSpan = hif::manipulation::rangeGetIncremented(r1, _sem, 1);
        Signed *result    = new Signed();
        result->setSpan(resultSpan);
        _result.operationPrecision = hif::copy(op1); // TODO check
        _result.returnedType       = result;
        return;
    }

    if (_currOperator == op_plus || _currOperator == op_minus) {
        // Returns a STD_LOGIC_VECTOR of op1'length.
        Bitvector *res             = _makeStandardLogicVector(hif::copy(op1->getSpan()));
        _result.operationPrecision = hif::copy(res);
        _result.returnedType       = res;
        return;
    }
}

void VHDLAnalysis::map(Bitvector *op1, Bit *op2)
{
    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (isRelational || isAssignment) {
        _result.operationPrecision = hif::copy(op1);
        ;
        if (isAssignment)
            _result.returnedType = hif::copy(op1);
        else /*relational*/
            _result.returnedType = new Bool();
        return;
    }

    // concatenation permitted on same types
    if (_currOperator == op_concat) {
        const bool op1IsConst = op1->isConstexpr();
        const bool op2IsConst = op2->isConstexpr();
        if (!op1IsConst && !op2IsConst) {
            // must be both logic or both ulogic
            if (op2->isLogic() != op1->isLogic())
                return;
        }

        // the result range is incremented by one.
        Range *resultSpan = hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem, 1);
        messageAssert(resultSpan != nullptr, "Unexpected case", _srcObj, _sem);

        // build the result array
        Bitvector *arrayResult = new Bitvector();
        arrayResult->setSpan(resultSpan);
        arrayResult->setSigned(op1->isSigned());
        arrayResult->setLogic(op2->isLogic() || op1->isLogic());
        arrayResult->setConstexpr(op1IsConst && op2IsConst);

        if (!op1IsConst && !op2IsConst)
            arrayResult->setResolved(op2->isResolved() || op1->isResolved());
        else if (!op1IsConst && op2IsConst)
            arrayResult->setResolved(op1->isResolved());
        else if (op1IsConst && !op2IsConst)
            arrayResult->setResolved(op2->isResolved());
        else //if (op1IsConst && op2IsConst)
            arrayResult->setResolved(op2->isResolved() || op1->isResolved());

        Bitvector *arrayPrecision = hif::copy(arrayResult);
        delete arrayPrecision->setSpan(hif::copy(op1->getSpan()));
        // set the returned and the precision
        _result.operationPrecision = arrayPrecision;
        _result.returnedType       = arrayResult;
        return;
    }

    // logical operators permitted on bit vectors
    if (hif::operatorIsBitwise(_currOperator)) {
        if (!_isBitVectorType(op1) && !op1->isConstexpr())
            return;
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = hif::copy(op1);
        return;
    }

    // there could be std_logic_vector or std_ulogic_vector
    if (_currOperator == op_plus || _currOperator == op_minus || _currOperator == op_mult) {
        if (!_isStandardLogicVector(op1) && !_isStandardUlogicVector(op1) && !op1->isConstexpr()) {
            return;
        }

        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = hif::copy(op1);
        return;
    }
}

void VHDLAnalysis::map(Bool * /*op1*/, Bool * /*op2*/)
{
    const bool isLogical    = hif::operatorIsLogical(_currOperator);
    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (!isLogical && !isRelational && !isAssignment)
        return;

    // the result and the precision are booleans
    _result.operationPrecision = new Bool();
    _result.returnedType       = new Bool();
}

void VHDLAnalysis::map(Bit *op1, Bool *op2)
{
    if (!_vhdlSem->usingPsl())
        return;

    Bit b;
    b.setConstexpr(op2->isConstexpr());
    b.setLogic(op1->isLogic());
    map(op1, &b);
}

void VHDLAnalysis::map(TypeReference *op1, TypeReference *op2) { _map(op1, op2, true); }

void VHDLAnalysis::map(Type *op1, TypeReference *op2) { _map(op1, op2); }

void VHDLAnalysis::map(Bitvector *op1, TypeReference *op2) { map(static_cast<Type *>(op1), op2); }

void VHDLAnalysis::map(Array *op1, TypeReference *op2) { map(static_cast<Type *>(op1), op2); }

void VHDLAnalysis::map(Record *op1, Record *op2)
{
    if (!hif::operatorIsAssignment(_currOperator) && !hif::operatorIsEquality(_currOperator)) {
        // unsupported
        return;
    }

    // mus have same fields
    if (op1->fields.size() != op2->fields.size())
        return;

    BList<Field>::iterator i = op1->fields.begin();
    BList<Field>::iterator j = op2->fields.begin();
    BList<Field> types;
    for (; i != op1->fields.end(); ++i, ++j) {
        Field *f1 = *i;
        Field *f2 = *j;
        if (f1->getName() != f2->getName())
            return;

        analyzeOperands(f1->getType(), f2->getType());
        if (_result.returnedType == nullptr)
            return;
        delete _result.returnedType;
        _result.returnedType = nullptr;

        Field *f = new Field();
        f->setName(f1->getName());
        f->setType(_result.operationPrecision);
        types.push_back(f);
        _result.operationPrecision = nullptr;
    }

    Record *ret = new Record();
    ret->fields.merge(types);

    if (hif::operatorIsAssignment(_currOperator)) {
        _result.operationPrecision = ret;
        _result.returnedType       = hif::copy(op1);
    } else // equality
    {
        _result.operationPrecision = ret;
        _result.returnedType       = new Bool();
    }
}

void VHDLAnalysis::map(Real *op1, Real *op2)
{
    hif::EqualsOptions opt;
    opt.checkConstexprFlag = false;
    if (!hif::equals(op1, op2, opt))
        return;

    if (!hif::operatorIsLogical(_currOperator) && !hif::operatorIsRelational(_currOperator)) {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = hif::copy(op1);
    } else // logical or relational => bool returned type
    {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = new Bool();
    }
}

void VHDLAnalysis::map(Int * /*op1*/, Real *op2)
{
    if (!hif::operatorIsArithmetic(_currOperator))
        return;

    _result.returnedType       = hif::copy(op2);
    _result.operationPrecision = hif::copy(op2);
}

void VHDLAnalysis::map(Time * /*op1*/, Real * /*op2*/)
{
    if (_currOperator != op_mult && _currOperator != op_div)
        return;

    _result.returnedType       = new Time();
    _result.operationPrecision = new Time();
}

void VHDLAnalysis::map(Time * /*op1*/, Time * /*op2*/)
{
    if (!hif::operatorIsAssignment(_currOperator) && _currOperator != op_eq && _currOperator != op_neq &&
        _currOperator != op_le && _currOperator != op_ge && _currOperator != op_lt && _currOperator != op_gt &&
        _currOperator != op_plus && _currOperator != op_minus && _currOperator != op_le && _currOperator != op_ge &&
        _currOperator != op_lt && _currOperator != op_gt) {
        return;
    }

    if (hif::operatorIsRelational(_currOperator)) {
        _result.returnedType = new Bool();
    } else {
        _result.returnedType = new Time();
    }

    _result.operationPrecision = new Time();
}

void VHDLAnalysis::map(Int *op1, Time *op2) { map(op2, op1); }

void VHDLAnalysis::map(Real *op1, Time *op2) { map(op2, op1); }

void VHDLAnalysis::map(String *op1, String * /*op2*/)
{
    if (_currOperator == op_concat) {
        _result.returnedType       = new String();
        _result.operationPrecision = new String();
        return;
    }

    if (hif::operatorIsAssignment(_currOperator)) {
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (operatorIsRelational(_currOperator)) {
        _result.returnedType       = new Bool();
        _result.operationPrecision = new String();
        return;
    }
}

void VHDLAnalysis::map(Bitvector *op1, String *op2)
{
    if (_sem->getStrictTypeChecks())
        return;
    // only without strict checks
    if (_currOperator == op_concat) {
        String *ret = new String();
        ret->setConstexpr(op1->isConstexpr() && op2->isConstexpr());
        if (op1->getSpan() != nullptr && op2->getSpanInformation() != nullptr) {
            ret->setSpanInformation(rangeGetSum(op1->getSpan(), op2->getSpanInformation(), _sem));
        }
        _result.returnedType       = ret;
        _result.operationPrecision = hif::copy(ret);
        return;
    }

    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (isRelational || isAssignment) {
        // arbitrary precision
        _result.operationPrecision = new String();
        if (isAssignment)
            _result.returnedType = hif::copy(op1);
        else /*relational*/
            _result.returnedType = new Bool();
        return;
    }
}

void VHDLAnalysis::map(Char *op1, String *op2)
{
    if (hif::operatorIsAssignment(_currOperator)) {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = hif::copy(op1);
        return;
    }

    map(op2, op1);
}

void VHDLAnalysis::map(Array *op1, String *op2)
{
    Type *op2Base = getBaseType(op1->getType(), false, _sem, false);
    if (dynamic_cast<Char *>(op2Base) == nullptr)
        return;

    if (hif::operatorIsAssignment(_currOperator)) {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = hif::copy(op1);
        return;
    }

    map(op2, op1);
}

void VHDLAnalysis::map(Pointer *op1, Pointer *op2)
{
    if (op2 == nullptr)
        return;
    if (!hif::operatorIsRelational(_currOperator) && !hif::operatorIsAssignment(_currOperator)) {
        return;
    }

    callMap(op1->getType(), op2->getType());
}

void VHDLAnalysis::map(File *op1, File *op2)
{
    if (op2 == nullptr)
        return;
    if (operatorIsRelational(_currOperator)) {
        callMap(op1->getType(), op2->getType());
    } else if (hif::operatorIsAssignment(_currOperator)) {
        // Always assignable:
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
    }
}

void VHDLAnalysis::map(Enum *op1, Enum *op2)
{
    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    if (!isRelational && !isAssignment)
        return;
    if (!hif::equals(op1, op2))
        return;

    _result.operationPrecision = hif::copy(op1);
    if (isAssignment)
        _result.returnedType = hif::copy(op1);
    else /*relational*/
        _result.returnedType = new Bool();
}

void VHDLAnalysis::map(Char *op1, Char *op2)
{
    const bool isRelational = hif::operatorIsRelational(_currOperator);
    const bool isAssignment = hif::operatorIsAssignment(_currOperator);
    // relational operators permitted
    if (isRelational || isAssignment) {
        // arbitrary precision
        _result.operationPrecision = hif::copy(op1);
        if (isAssignment)
            _result.returnedType = hif::copy(op1);
        else /*relational*/
            _result.returnedType = new Bool();
        return;
    }

    if (_currOperator == op_concat) {
        // concatenation works for all types (if they are equal) and the
        // result is an array with range 2 of the two types
        if (!hif::equals(op1, op2))
            return;
        //Array* result_array = new Array();
        //result_array->setSpan(new Range(1, 0));
        //result_array->setType(hif::copy(t1));
        String *ret = new String();
        ret->setSpanInformation(new Range(1, 0));
        _result.returnedType       = ret;
        _result.operationPrecision = hif::copy(op1);
    }
}

void VHDLAnalysis::map(String *op1, Char * /*op2*/)
{
    if (hif::operatorIsRelational(_currOperator)) {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = new Bool();
        return;
    }

    if (hif::operatorIsAssignment(_currOperator)) {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = hif::copy(op1);
        return;
    }

    if (_currOperator == op_concat) {
        _result.operationPrecision = hif::copy(op1);
        _result.returnedType       = hif::copy(op1);
        return;
    }
}

void VHDLAnalysis::_callMap(Object *op1) { callMap(op1); }

void VHDLAnalysis::_callMap(Object *op1, Object *op2) { callMap(op1, op2); }

// ///////////////////////////////////////////////////////////////////
// VHDLCastMapper_t
// ///////////////////////////////////////////////////////////////////

VHDLCastAnalysis::VHDLCastAnalysis(Value *valueToCast, Type *srcType)
    : _sem(VHDLSemantics::getInstance())
    , _inputVal(valueToCast)
    , _sourceType(srcType)
    , _result(nullptr)
    , _neededIncludes()
    , _factory(_sem)
{
    // ntd
}

VHDLCastAnalysis::~VHDLCastAnalysis()
{
    // ntd
}

Value *VHDLCastAnalysis::explicitCast(Type *castType)
{
    // if at least one parameter is null, return null
    if (_inputVal == nullptr || getSemanticType(_inputVal, _sem) == nullptr || castType == nullptr) {
        return nullptr;
    }

    Value *restoreInputVal = _inputVal;

    Type *valueType = getSemanticType(_inputVal, _sem);
    Type *baseVt    = getBaseType(valueType, false, _sem);
    Type *baseCt    = getBaseType(castType, true, _sem);

    // if type to cast is the same of value, return value
    if (hif::equals(baseVt, baseCt)) {
        Value *ret = hif::copy(_inputVal);
        _inputVal  = restoreInputVal;

        return ret;
    }

    callMap(baseVt, baseCt);

    _inputVal = restoreInputVal;

    if (!hif::equals(castType, baseCt)) {
        // try to restore eventual typerefs
        std::list<Object *> list;
        hif::EqualsOptions opt;
        opt.skipNullBranches = true;
        hif::manipulation::matchPatternInTree(baseCt, _result, list, opt);

        for (std::list<Object *>::iterator i = list.begin(); i != list.end(); ++i) {
            (*i)->replace(hif::copy(castType));
            delete (*i);
        }
    }

    return _result;
}

void VHDLCastAnalysis::generateInclude(Value *startingObj)
{
    for (std::set<std::string>::iterator i(_neededIncludes.begin()); i != _neededIncludes.end(); ++i) {
        std::string libraryName = *i;

        Scope *s = hif::getNearestScope(startingObj, false, true, false);
        if (dynamic_cast<Contents *>(s) != nullptr)
            s = dynamic_cast<Scope *>(s->getParent());

        Library *l = new Library();
        l->setName(libraryName);
        l->setStandard(false);
        l->setSystem(true);

        hif::manipulation::AddUniqueObjectOptions addOpt;
        addOpt.equalsOptions.checkOnlyNames = true;
        addOpt.deleteIfNotAdded             = true;
        hif::manipulation::addUniqueObject(l, s, addOpt);

        if (!_sem->isNativeLibrary(libraryName))
            continue;
        System *sys = hif::getNearestParent<System>(startingObj);
        if (sys == nullptr)
            return;

        hif::manipulation::AddUniqueObjectOptions addOpt2;
        addOpt2.equalsOptions.checkOnlyNames = true;
        hif::manipulation::addUniqueObject(_sem->getStandardLibrary(libraryName), sys->libraryDefs, addOpt2);
    }
}

Value *VHDLCastAnalysis::_getCastFromValueToType(Value *from, Type *to)
{
    Cast *ret = new Cast();
    ret->setValue(hif::copy(from));
    ret->setType(hif::copy(to));

    return ret;
}

void VHDLCastAnalysis::_mapVectors(Type *t1, Type *t2)
{
    Bitvector *bv1 = dynamic_cast<Bitvector *>(t1);
    Bitvector *bv2 = dynamic_cast<Bitvector *>(t2);
    if (bv1 != nullptr && bv2 != nullptr) {
        if (hif::equals(bv1->getSpan(), bv2->getSpan())) {
            _result = _getCastFromValueToType(_inputVal, t2);
            return;
        }
        // Otherwise, we have to resize
    }

    // 1 - call resize() to fix span dimension, possibly adding intermediate casts
    Range *s1 = hif::typeGetSpan(t1, _sem);
    Range *s2 = hif::typeGetSpan(t2, _sem);
    Value *v  = hif::copy(_inputVal);
    if (!hif::equals(s1, s2)) {
        if (bv1 != nullptr) {
            // to call resize(), types must be signed/unsigned
            Type *tt = nullptr;
            if (bv1->isSigned())
                tt = new Signed();
            else
                tt = new Unsigned();
            hif::typeSetSpan(tt, hif::copy(s1), _sem);
            v = _factory.cast(tt, v);
        }

        Value *spanSize = spanGetSize(s2, _sem);
        v               = _factory.functionCall(
            "resize", nullptr, _factory.noTemplateArguments(),
            (_factory.parameterArgument("param1", v), _factory.parameterArgument("param2", spanSize)));

        _neededIncludes.insert("ieee_numeric_std");

        if (bv1 != nullptr) {
            // possibly cast to original bitvector
            Bitvector *tt = hif::copy(bv1);
            delete tt->setSpan(hif::copy(s2));
            v = _factory.cast(tt, v);
        }
    }

    // 2 - cast to destination type
    hif::EqualsOptions opt;
    opt.checkOnlyTypes = true;
    if (!hif::equals(t1, t2, opt)) {
        v = _factory.cast(hif::copy(t2), v);
    }

    _result = v;
}
// //////////////////////////
// Maps implementation
// //////////////////////////
#ifdef NDEBUG
void VHDLCastAnalysis::map(Object * /*s*/, Object *d)
#else
void VHDLCastAnalysis::map(Object *s, Object *d)
#endif
{
    messageDebug("Source type:", s, _sem);
    messageError("Cast not managed", d, _sem);
}

void VHDLCastAnalysis::map(Type *, TypeReference *tr)
{
    // tr is referred to an opaque typedef.

    Type *baseCastType = getBaseType(tr, false, _sem);
    Value *v           = explicitCast(baseCastType);

    if (hif::equals(v, _inputVal)) {
        // cast was simplified
        _result = _getCastFromValueToType(_inputVal, tr);
    } else {
        _result = v;
    }
}

void VHDLCastAnalysis::map(Array *, Array *) { _result = hif::copy(_inputVal); }

void VHDLCastAnalysis::map(Int *, Int *) { _result = hif::copy(_inputVal); }

void VHDLCastAnalysis::map(Bitvector *vi, Int *)
{
    std::string fName = (vi->isSigned()) ? "to_signed" : "to_unsigned";
    _result           = _factory.functionCall(
        "to_integer", nullptr, _factory.noTemplateArguments(),
        _factory.parameterArgument(
            "param1", _factory.functionCall(
                          fName.c_str(), nullptr, _factory.noTemplateArguments(),
                          _factory.parameterArgument("param1", hif::copy(_inputVal)))));

    _neededIncludes.insert("ieee_numeric_std");
}

void VHDLCastAnalysis::map(Signed *, Int *)
{
    _result = _factory.functionCall(
        "to_integer", nullptr, _factory.noTemplateArguments(),
        _factory.parameterArgument("param1", hif::copy(_inputVal)));

    _neededIncludes.insert("ieee_numeric_std");
}

void VHDLCastAnalysis::map(Unsigned *, Int *)
{
    _result = _factory.functionCall(
        "to_integer", nullptr, _factory.noTemplateArguments(),
        _factory.parameterArgument("param1", hif::copy(_inputVal)));

    _neededIncludes.insert("ieee_numeric_std");
}

void VHDLCastAnalysis::map(Bitvector *v1, Bitvector *v2)
{
    _mapVectors(v1, v2);
    //result = hif::copy(inputVal);
}

void VHDLCastAnalysis::map(Int *vi, Bitvector *vector)
{
    std::string fName = (vi->isSigned()) ? "to_signed" : "to_unsigned";
    FunctionCall *f   = _factory.functionCall(
        fName.c_str(), nullptr, _factory.noTemplateArguments(),
        (_factory.parameterArgument("param1", hif::copy(_inputVal)),
         _factory.parameterArgument("param2", spanGetSize(vector->getSpan(), _sem))));

    _result = _factory.cast(vector, f);

    _neededIncludes.insert("ieee_numeric_std");
}

void VHDLCastAnalysis::map(Signed *v1, Bitvector *v2)
{
    _mapVectors(v1, v2);
    //result = getCastFromValueToType(inputVal, vector);
}

void VHDLCastAnalysis::map(Unsigned *v1, Bitvector *v2)
{
    _mapVectors(v1, v2);
    //result = getCastFromValueToType(inputVal, vector);
}

void VHDLCastAnalysis::map(Bit *vi, Bitvector *vector)
{
    if (vi->isLogic() == vector->isLogic()) {
        _result = hif::copy(_inputVal);
    } else {
        // not managed yet
    }
}

void VHDLCastAnalysis::map(Signed *v1, Signed *v2)
{
    _mapVectors(v1, v2);
    //result = hif::copy(inputVal);
}

void VHDLCastAnalysis::map(Int *, Signed *vector)
{
    _result = _factory.functionCall(
        "to_signed", nullptr, _factory.noTemplateArguments(),
        (_factory.parameterArgument("param1", hif::copy(_inputVal)),
         _factory.parameterArgument("param2", spanGetSize(vector->getSpan(), _sem))));

    _neededIncludes.insert("ieee_numeric_std");
}

void VHDLCastAnalysis::map(Bitvector *v1, Signed *v2)
{
    _mapVectors(v1, v2);
    //    if (dynamic_cast<BitvectorValue*>(inputVal) != nullptr)
    //        result = hif::copy(inputVal);
    //    else
    //        result = getCastFromValueToType(inputVal, vector);
}

void VHDLCastAnalysis::map(Unsigned *v1, Signed *v2)
{
    _mapVectors(v1, v2);
    //result = getCastFromValueToType(inputVal, vector);
}

void VHDLCastAnalysis::map(Unsigned *v1, Unsigned *v2)
{
    _mapVectors(v1, v2);
    //result = hif::copy(inputVal);
}

void VHDLCastAnalysis::map(Int *, Unsigned *vector)
{
    _result = _factory.functionCall(
        "to_unsigned", nullptr, _factory.noTemplateArguments(),
        (_factory.parameterArgument("param1", hif::copy(_inputVal)),
         _factory.parameterArgument("param2", spanGetSize(vector->getSpan(), _sem))));

    _neededIncludes.insert("ieee_numeric_std");
}

void VHDLCastAnalysis::map(Signed *v1, Unsigned *v2)
{
    _mapVectors(v1, v2);
    //result = getCastFromValueToType(inputVal, vector);
}

void VHDLCastAnalysis::map(Bitvector *v1, Unsigned *v2)
{
    _mapVectors(v1, v2);
    //    if (dynamic_cast<BitvectorValue*>(inputVal) != nullptr)
    //        result = hif::copy(inputVal);
    //    else
    //        result = getCastFromValueToType(inputVal, vector);
}

void VHDLCastAnalysis::map(Pointer *, Pointer *) { _result = hif::copy(_inputVal); }

void VHDLCastAnalysis::map(Int *, Pointer *p)
{
    // Usually this is the nullptr pointer.
    _result = _getCastFromValueToType(_inputVal, p);
}

void VHDLCastAnalysis::map(File *, File *) { _result = hif::copy(_inputVal); }

void VHDLCastAnalysis::map(String *, File *) { _result = hif::copy(_inputVal); }

void VHDLCastAnalysis::map(Bit * /*bo*/, Bit * /*bi*/) { _result = hif::copy(_inputVal); }

void VHDLCastAnalysis::map(Bool * /*bo*/, Bit *bi)
{
    // TODO: actually this cast does not exist in VHDL
    _result = _getCastFromValueToType(_inputVal, bi);
}

void VHDLCastAnalysis::map(Bit *bit, Bool * /*bo*/)
{
    BitValue *bvo = new BitValue(bit_one);
    bvo->setType(hif::copy(bit));

    // the first operand is the given value object
    Expression *cond = new Expression();
    cond->setValue1(hif::copy(_inputVal));
    cond->setValue2(bvo);
    cond->setOperator(op_case_eq);

    _result = cond;
}

// ///////////////////////////////////////////////////////////////////
// DefaultValueVisitor
// ///////////////////////////////////////////////////////////////////
class DefaultValueVisitor : public hif::GuideVisitor
{
public:
    DefaultValueVisitor(Type *originalType, Declaration *refDecl);
    virtual ~DefaultValueVisitor();

    Value *getResult();

    int visitArray(Array &o);
    int visitBit(Bit &o);
    int visitBool(Bool &o);
    int visitBitvector(Bitvector &o);
    int visitChar(Char &o);
    int visitEnum(Enum &o);
    int visitFile(File &o);
    int visitInt(Int &o);
    int visitPointer(Pointer &o);
    int visitReal(Real &o);
    int visitRecord(Record &o);
    int visitSigned(Signed &o);
    int visitString(String &o);
    int visitTime(Time &o);
    int visitTypeReference(TypeReference &o);
    int visitUnsigned(Unsigned &o);
    int visitViewReference(ViewReference &o);

private:
    Value *_getSignedUnsignedDefaultValue(Type *t);

    Declaration *_refDecl;
    Value *_ret;

    VHDLSemantics *_sem;
    hif::HifFactory _factory;
    Type *_originalType;

    // disabled
    DefaultValueVisitor(const DefaultValueVisitor &);
    DefaultValueVisitor &operator=(const DefaultValueVisitor &);
};

DefaultValueVisitor::DefaultValueVisitor(Type *originalType, Declaration *refDecl)
    : hif::GuideVisitor()
    , _refDecl(refDecl)
    , _ret(nullptr)
    , _sem(VHDLSemantics::getInstance())
    , _factory(_sem)
    , _originalType(originalType)
{
    // ntd
}

DefaultValueVisitor::~DefaultValueVisitor()
{
    // ntd
}

Value *DefaultValueVisitor::getResult() { return _ret; }

Value *DefaultValueVisitor::_getSignedUnsignedDefaultValue(Type *type)
{
    Range *range = hif::typeGetSpan(type, _sem);
    messageAssert(range != nullptr, "Unexpected case", type, _sem);

    std::uint64_t size = spanGetBitwidth(range, VHDLSemantics::getInstance());
    if (size == 0) {
        // Failed to determine range: create an Aggregate with others = 'U'
        Bit *b = new Bit();
        b->setLogic(true);
        b->setResolved(true);
        b->setConstexpr(true);
        BitValue *bv = new BitValue(bit_u);
        bv->setType(b);
        Aggregate *agg = new Aggregate();
        agg->setOthers(bv);
        return agg;
    }

    // span size > 0: build the std_logic "00...0"
    std::string s;
    s.reserve(static_cast<std::string::size_type>(size + 1));
    char c = 'u';

    for (std::uint64_t i = 0; i < size; ++i)
        s.push_back(c);
    BitvectorValue *bv = new BitvectorValue(s);
    type               = hif::copy(type);
    hif::typeSetConstexpr(type, true);
    bv->setType(type);
    return bv;
}

int DefaultValueVisitor::visitArray(Array &o)
{
    Aggregate *ao = new Aggregate();
    ao->setOthers(_sem->getTypeDefaultValue(o.getType(), _refDecl));
    _ret = ao;

    return 0;
}

int DefaultValueVisitor::visitBit(Bit &o)
{
    // check if it's a bit or a std_logic
    BitValue *ret = new BitValue();
    Bit *tt       = hif::copy(&o);
    tt->setConstexpr(true);
    ret->setType(tt);
    if (o.isLogic()) {
        // it's a std_logic. It's initial value is "U"
        ret->setValue(bit_u);
    } else {
        // it's a bit. It's initial value is "0"
        ret->setValue(bit_zero);
    }

    _ret = ret;
    return 0;
}

int DefaultValueVisitor::visitBool(Bool &o)
{
    BoolValue *b = new BoolValue(false);
    Bool *tt     = hif::copy(&o);
    tt->setConstexpr(true);
    b->setType(tt);

    _ret = b;
    return 0;
}

int DefaultValueVisitor::visitBitvector(Bitvector &o)
{
    // it is a bit vector or a logic vector. Its default value is "00...0"
    Range *range = o.getSpan();
    if (range == nullptr)
        return 0;

    std::uint64_t size = spanGetBitwidth(range, VHDLSemantics::getInstance());
    if (size == 0) {
        // Failed to determine range: create an Aggregate with others = 'U' or '0'
        BitValue *bitvalue;
        Aggregate *agg = new Aggregate();
        if (o.isLogic()) {
            bitvalue = new BitValue(bit_u);
        } else {
            bitvalue = new BitValue(bit_zero);
        }

        Bit *b = _factory.bit(o.isLogic(), o.isResolved(), true);
        bitvalue->setType(b);
        agg->setOthers(bitvalue);
        _ret = agg;
        return 0;
    }

    // span size > 0: build the std_logic "00...0"
    std::string s;
    s.reserve(static_cast<std::string::size_type>(size + 1));
    char c = '0';
    if (o.isLogic())
        c = 'U';

    for (std::uint64_t i = 0; i < size; ++i)
        s.push_back(c);

    BitvectorValue *bValue = new BitvectorValue(s);
    Bitvector *tt          = hif::copy(&o);
    tt->setConstexpr(true);
    bValue->setType(tt);

    _ret = bValue;
    return 0;
}

int DefaultValueVisitor::visitChar(Char &o)
{
    // the initial value is '\0'
    CharValue *c = new CharValue('\0');
    Char *tt     = hif::copy(&o);
    tt->setConstexpr(true);
    c->setType(tt);

    _ret = c;
    return 0;
}

int DefaultValueVisitor::visitEnum(Enum &o)
{
    EnumValue *front = o.values.front();
    Identifier *ret  = nullptr;
    if (front != nullptr) {
        ret = new Identifier(front->getName());
    }
    _ret = ret;
    return 0;
}

int DefaultValueVisitor::visitFile(File &o)
{
    StringValue *t = new StringValue();
    t->setValue("");
    String *tt = new String();
    tt->setConstexpr(true);
    ;
    t->setType(tt);

    Cast *c = new Cast();
    c->setType(hif::copy(&o));
    c->setValue(t);

    _ret = c;
    return 0;
}

int DefaultValueVisitor::visitInt(Int &o)
{
    // the initial value is always the LEFT bound
    DataDeclaration *decl = hif::getNearestParent<DataDeclaration>(&o);
    TypeDef *td           = hif::getNearestParent<TypeDef>(&o);
    if (decl != nullptr && decl->getRange() != nullptr) {
        Value *leftValue = hif::copy(decl->getRange()->getLeftBound());
        ConstValue *cv   = dynamic_cast<ConstValue *>(leftValue);
        if (cv != nullptr) {
            delete cv->setType(_sem->getTypeForConstant(cv));
        }
        _ret = leftValue;
    } else if (td != nullptr && td->getRange() != nullptr) {
        Value *leftValue = hif::copy(td->getRange()->getLeftBound());
        ConstValue *cv   = dynamic_cast<ConstValue *>(leftValue);
        if (cv != nullptr) {
            delete cv->setType(_sem->getTypeForConstant(cv));
        }

        _ret = leftValue;
    } else {
        IntValue *ivo = new IntValue(-2147483647LL);
        ivo->setType(_sem->getTypeForConstant(ivo));
        _ret = ivo;
    }

    return 0;
}

int DefaultValueVisitor::visitPointer(Pointer &o)
{
    _ret = _factory.cast(hif::copy(&o), _factory.intval(0));
    return 0;
}

int DefaultValueVisitor::visitReal(Real &o)
{
    // the init val is -1e+308
    RealValue *rvo = new RealValue(-1e+308);
    Real *tt       = hif::copy(&o);
    tt->setConstexpr(true);
    rvo->setType(tt);

    _ret = rvo;
    return 0;
}

int DefaultValueVisitor::visitRecord(Record &o)
{
    RecordValue *recVal = new RecordValue();
    for (BList<Field>::iterator i = o.fields.begin(); i != o.fields.end(); ++i) {
        RecordValueAlt *rva = new RecordValueAlt();
        rva->setName((*i)->getName());

        Value *v = _sem->getTypeDefaultValue((*i)->getType(), _refDecl);
        messageAssert(v != nullptr, "Unexpected case", &o, _sem);

        rva->setValue(v);
        recVal->alts.push_back(rva);
    }

    _ret = recVal;
    return 0;
}

int DefaultValueVisitor::visitSigned(Signed &o)
{
    _ret = _getSignedUnsignedDefaultValue(&o);
    return 0;
}

int DefaultValueVisitor::visitString(String &o)
{
    if (o.getSpanInformation() == nullptr) {
        StringValue *t = new StringValue();
        t->setValue("");
        String *tt = hif::copy(&o);
        tt->setConstexpr(true);
        t->setType(tt);

        _ret = t;
    } else {
        std::uint64_t s = spanGetBitwidth(o.getSpanInformation(), _sem);
        if (s != 0) {
            StringValue *t = new StringValue();
            t->setValue(std::string(std::string::size_type(s), ' '));
            String *tt = hif::copy(&o);
            tt->setConstexpr(true);
            t->setType(tt);

            _ret = t;
        } else {
            Aggregate *a = new Aggregate();
            a->setOthers(_factory.charval(' '));
            _ret = a;
        }
    }

    return 0;
}

int DefaultValueVisitor::visitTime(Time &o)
{
    TimeValue *t = new TimeValue();
    t->setValue(0.0);
    Time *tt = hif::copy(&o);
    tt->setConstexpr(true);
    t->setType(tt);

    _ret = t;
    return 0;
}

int DefaultValueVisitor::visitTypeReference(TypeReference &o)
{
    // get the type def init value w/o consider opacity
    Type *bt = getBaseType(&o, false, _sem);
    if (bt == nullptr)
        return 0;

    // call this function recursively
    _ret = _sem->getTypeDefaultValue(bt, _refDecl);
    return 0;
}

int DefaultValueVisitor::visitUnsigned(Unsigned &o)
{
    _ret = _getSignedUnsignedDefaultValue(&o);
    return 0;
}

int DefaultValueVisitor::visitViewReference(ViewReference & /*o*/)
{
    _ret = nullptr;
    return 0;
}

// ///////////////////////////////////////////////////////////////////
// DefaultValueVisitor
// ///////////////////////////////////////////////////////////////////
class TypeForConstantVisitor : public hif::GuideVisitor
{
public:
    TypeForConstantVisitor();
    virtual ~TypeForConstantVisitor();

    Type *getResult();

    int visitBoolValue(BoolValue &o);
    int visitBitValue(BitValue &o);
    int visitBitvectorValue(BitvectorValue &o);
    int visitCharValue(CharValue &o);
    int visitIntValue(IntValue &o);
    int visitRealValue(RealValue &o);
    int visitStringValue(StringValue &o);
    int visitTimeValue(TimeValue &o);

private:
    Type *_ret;

    VHDLSemantics *_sem;
    hif::HifFactory _factory;

    // disabled
    TypeForConstantVisitor(const TypeForConstantVisitor &);
    TypeForConstantVisitor &operator=(const TypeForConstantVisitor &);
};

TypeForConstantVisitor::TypeForConstantVisitor()
    : hif::GuideVisitor()
    , _ret(nullptr)
    , _sem(VHDLSemantics::getInstance())
    , _factory(_sem)
{
    // ntd
}

TypeForConstantVisitor::~TypeForConstantVisitor()
{
    // ntd
}

Type *TypeForConstantVisitor::getResult() { return _ret; }

int TypeForConstantVisitor::visitBoolValue(BoolValue &)
{
    Bool *b_type = new Bool();
    b_type->setConstexpr(true);
    _ret = b_type;
    return 0;
}

int TypeForConstantVisitor::visitBitValue(BitValue &o)
{
    Bit *b_type     = new Bit();
    BitConstant val = o.getValue();
    // if the value is not 1 or 0 we are sure that the given bitval
    // is logic, otherwise we don't know
    if (val != bit_one && val != bit_zero)
        b_type->setLogic(true);
    b_type->setConstexpr(true);
    _ret = b_type;
    return 0;
}

int TypeForConstantVisitor::visitBitvectorValue(BitvectorValue &o)
{
    Range *ro      = new Range(static_cast<int>(o.getValue().size() - 1), 0);
    Bitvector *ret = new Bitvector();
    ret->setSpan(ro);
    ret->setSigned(false);
    ret->setConstexpr(true);
    ret->setLogic(!o.is01());
    ret->setResolved(true); // Arbitrary.
    _ret = ret;
    return 0;
}

int TypeForConstantVisitor::visitCharValue(CharValue &)
{
    Char *ret = new Char();
    ret->setConstexpr(true);
    _ret = ret;
    return 0;
}

int TypeForConstantVisitor::visitIntValue(IntValue &)
{
    Int *ret = new Int();
    // By default constant value is assumed as signed
    ret->setSigned(true);
    ret->setConstexpr(true);
    Range *span = new Range(new IntValue(31), new IntValue(0), dir_downto);
    ret->setSpan(span);
    _ret = ret;
    return 0;
}

int TypeForConstantVisitor::visitRealValue(RealValue &)
{
    Real *ret = new Real(); // Already verified on ModelSim
    ret->setConstexpr(true);
    ret->setSpan(new Range(63, 0));
    _ret = ret;
    return 0;
}

int TypeForConstantVisitor::visitStringValue(StringValue &t)
{
    String *ret = new String();
    ret->setConstexpr(true);
    std::string s = t.getValue();
    if (s != "") {
        Range *r = new Range();
        r->setDirection(dir_upto);
        r->setLeftBound(new IntValue(1LL));
        r->setRightBound(new IntValue(static_cast<std::int64_t>(s.size())));
        ret->setSpanInformation(r);
    } else {
        Range *r = new Range();
        r->setDirection(dir_upto);
        r->setLeftBound(new IntValue(1));
        ret->setSpanInformation(r);
    }

    _ret = ret;
    return 0;
}

int TypeForConstantVisitor::visitTimeValue(TimeValue &)
{
    Time *ret = _factory.time(true);
    _ret      = ret;
    return 0;
}

} // namespace
// ///////////////////////////////////////////////////////////////////
// VHDL Semantics public methods
// ///////////////////////////////////////////////////////////////////
VHDLSemantics::VHDLSemantics()
    : ILanguageSemantics()
    , _stdMap()
    , _usingPsl(false)
{
    _hifFactory.setSemantics(HIFSemantics::getInstance());

    _initForbiddenNames();
    _initStandardSymbols();
    _initStandardFilenames();

    // Initializing semantic check options.
    _semanticOptions.port_inNoInitialValue        = false;
    _semanticOptions.port_outInitialValue         = false;
    _semanticOptions.dataDeclaration_initialValue = false;
    _semanticOptions.scopedType_insideTypedef     = true;
    _semanticOptions.int_bitfields                = false;
    _semanticOptions.designUnit_uniqueView        = false;
    _semanticOptions.for_implictIndex             = true;
    _semanticOptions.for_conditionType            = SemanticOptions::RANGE;
    _semanticOptions.generates_isNoAllowed        = false;
    _semanticOptions.after_isNoAllowed            = false;
    _semanticOptions.with_isNoAllowed             = false;
    _semanticOptions.globact_isNoAllowed          = false;
    _semanticOptions.valueStatement_isNoAllowed   = true;
    _semanticOptions.case_isOnlyLiteral           = true;
    _semanticOptions.lang_signPortNoBitAccess     = false;
    _semanticOptions.lang_hasDontCare             = true;
    _semanticOptions.lang_has9logic               = true;
    _semanticOptions.waitWithActions              = false;
    _semanticOptions.lang_sortKind                = hif::manipulation::SortMissingKind::NOTHING;
}
VHDLSemantics::~VHDLSemantics()
{
    for (DeclarationMap::iterator i = _stdMap.begin(); i != _stdMap.end(); ++i) {
        for (SubList::iterator j = i->second.begin(); j != i->second.end(); ++j) {
            delete *j;
        }
    }
}
std::string VHDLSemantics::getName() const { return "vhdl"; }
ILanguageSemantics::ExpressionTypeInfo
VHDLSemantics::getExprType(Type *op1Type, Type *op2Type, Operator operation, Object *sourceObj)
{
    ILanguageSemantics::ExpressionTypeInfo ret;

    VHDLAnalysis vhdlAnalysis(operation, sourceObj);
    vhdlAnalysis.analyzeOperands(op1Type, op2Type);
    ret = vhdlAnalysis.getResult();

    return ret;
}
Value *VHDLSemantics::getTypeDefaultValue(Type *type, Declaration *d)
{
    // Do not recouse to ensure that declaration are found in the given tree.
    // Ref. design: vhdl/ips/ppm_interpreter_logic
    Type *bt = getBaseType(type, false, this, false);

    DefaultValueVisitor vis(type, d);
    bt->acceptVisitor(vis);
    Value *v = vis.getResult();
    if (dynamic_cast<ConstValue *>(v) != nullptr && dynamic_cast<TypeReference *>(type) != nullptr) {
        ConstValue *local = static_cast<ConstValue *>(v);
        local->setType(hif::copy(type));
    }
    return v;
}
Type *VHDLSemantics::getMapForType(Type *t)
{
    if (dynamic_cast<Bitvector *>(t) != nullptr) {
        Bitvector *bv = static_cast<Bitvector *>(t);
        if (bv->isSigned() && bv->isLogic() /* && bv->IsResolved()*/) {
            Signed *s = new Signed();
            s->setConstexpr(bv->isConstexpr());
            s->setSpan(hif::copy(bv->getSpan()));
            return s;
        }
    }

    hif::CopyOptions opt;
    return hif::copy(t, opt);
}
Operator VHDLSemantics::getMapForOperator(
    Operator srcOperation,
    Type * /*srcT1*/,
    Type * /*srcT2*/,
    Type * /*dstT1*/,
    Type * /*dstT2*/)
{
    return srcOperation;
}
Type *VHDLSemantics::getSuggestedTypeForOp(
    Type *t,
    Operator operation,
    Type * /*opType*/,
    Object *startingObject,
    const bool /*isOp1*/)
{
    if (hif::operatorIsArithmetic(operation)) {
        if (dynamic_cast<Bitvector *>(t) != nullptr) {
            Bitvector *bv = static_cast<Bitvector *>(t);
            if (bv->isSigned()) {
                Signed *s = new Signed();
                s->setSpan(hif::copy(bv->getSpan()));
                s->setConstexpr(bv->isConstexpr());
                t = s;
            } else {
                Unsigned *u = new Unsigned();
                u->setSpan(hif::copy(bv->getSpan()));
                u->setConstexpr(bv->isConstexpr());
                t = u;
            }
        }
    }
    ILanguageSemantics::ExpressionTypeInfo info;
    info                    = getExprType(t, t, operation, startingObject);
    Type *ret               = info.operationPrecision;
    info.operationPrecision = nullptr;
    return ret;
}
bool VHDLSemantics::isTemplateAllowedType(Type * /*t*/)
{
    // all types are allowed at the moment
    return true;
}
Type *VHDLSemantics::getTemplateAllowedType(Type *t) { return hif::copy(t); }
VHDLSemantics *VHDLSemantics::getInstance()
{
    static VHDLSemantics instance;
    return &instance;
}
Type *VHDLSemantics::getTypeForConstant(ConstValue *c)
{
    TypeForConstantVisitor v;
    c->acceptVisitor(v);

    Type *ret = v.getResult();
    return ret;
}
bool VHDLSemantics::checkCondition(Type *t, Object *o)
{
    if (t == nullptr)
        return false;

    WhenAlt *wa = dynamic_cast<WhenAlt *>(o);
    if (wa != nullptr) {
        When *w = dynamic_cast<When *>(wa->getParent());
        if (w != nullptr && w->isLogicTernary()) {
            Bit *b = dynamic_cast<Bit *>(t);
            return b != nullptr && b->isLogic();
        }
    }

    // VHDL supports only boolean as condition guard
    return dynamic_cast<Bool *>(t) != nullptr;
}
Value *VHDLSemantics::explicitBoolConversion(Value *c)
{
    // condition is valid, explicit value as boolean condition
    // retrieve value type to build the second operand
    Type *type = getSemanticType(c, this);
    if (type == nullptr)
        return nullptr;
    type = getBaseType(type, false, this, false);
    if (type == nullptr)
        return nullptr;

    // if it is bit, replace it with (bit == 1)
    if (dynamic_cast<Bit *>(type) != nullptr) {
        Bit *bit = static_cast<Bit *>(type);
        bit      = hif::copy(bit);
        bit->setConstexpr(true);
        BitValue *bvo = new BitValue(bit_one);
        bvo->setType(bit);

        // the first operand is the given value object
        Expression *cond = new Expression();
        cond->setValue1(hif::copy(c));
        cond->setValue2(bvo);
        cond->setOperator(op_case_eq);

        return cond;
    } else if (
        dynamic_cast<Bitvector *>(type) != nullptr || dynamic_cast<Signed *>(type) != nullptr ||
        dynamic_cast<Unsigned *>(type) != nullptr) {
        Expression *cond = new Expression();
        IntValue int1(0);
        Value *cost1 = hif::manipulation::transformConstant(&int1, type, this);
        messageAssert(cost1 != nullptr, "Transform constant failed.", type, this);
        cond->setValue2(_factory.expression(op_bnot, cost1));
        cond->setOperator(op_case_eq);

        return cond;
    } else if (dynamic_cast<Bool *>(type) != nullptr) {
        return hif::copy(c);
    }

    // otherwise not supported
    return nullptr;
}
Value *VHDLSemantics::explicitCast(Value *valueToCast, Type *castType, Type *srcType)
{
    // In some cases, no need to keep the cast
    hif::EqualsOptions opt;
    opt.checkConstexprFlag = false;
    //opt.check_spans = false;
    opt.checkSignedFlag    = false;

    Type *baseVt = getBaseType(getSemanticType(valueToCast, this), true, this);
    Type *baseCt = getBaseType(castType, true, this);

    if (hif::equals(baseVt, baseCt, opt)) {
        return hif::copy(valueToCast);
    }

    // Checking other cases with the map.
    VHDLCastAnalysis vca(valueToCast, srcType);
    Value *ret = vca.explicitCast(castType);

    vca.generateInclude(valueToCast);
    return ret;
}

std::int64_t VHDLSemantics::transformRealToInt(const double v) { return std::llround(v); }

Type *VHDLSemantics::isTypeAllowedAsBound(Type *t)
{
    messageAssert(t != nullptr, "Passed nullptr type", nullptr, this);

    Int *intT = dynamic_cast<Int *>(t);
    if (intT != nullptr)
        return nullptr;

    return _factory.integer(hif::copy(hif::typeGetSpan(t, this)), false, true);
}

bool VHDLSemantics::isCastAllowed(Type *target, Type *source)
{
    // known allowed casts:
    // between the same type
    // between vectors and arrays with same span
    // between int/real

    hif::EqualsOptions opt;
    opt.checkConstexprFlag = false;
    if (hif::equals(target, source, opt))
        return true;

    Type *bt = getBaseType(target, false, this);
    Type *st = getBaseType(source, false, this);
    if (bt == nullptr || st == nullptr)
        return false;

    const bool tIsVector = isVectorType(bt, this);
    const bool sIsVector = isVectorType(st, this);
    if (tIsVector && sIsVector) {
        Value *spanT = typeGetSpanSize(bt, this);
        Value *spanS = typeGetSpanSize(st, this);

        const bool ret = hif::equals(spanT, spanS);

        delete spanT;
        delete spanS;

        return ret;
    }

    const bool tIsInt  = dynamic_cast<Int *>(bt) != nullptr;
    const bool sIsInt  = dynamic_cast<Int *>(st) != nullptr;
    const bool tIsReal = dynamic_cast<Real *>(bt) != nullptr;
    const bool sIsReal = dynamic_cast<Real *>(st) != nullptr;
    if ((tIsInt || tIsReal) && (sIsInt || sIsReal))
        return true;

    return false;
}

bool VHDLSemantics::isTypeAllowed(Type *t)
{
    if (dynamic_cast<Reference *>(t) != nullptr) {
        return false;
    }

    Type::TypeVariant tv = t->getTypeVariant();
    if (tv != Type::NATIVE_TYPE && tv != Type::VHDL_BITVECTOR_NUMERIC_STD) {
        return false;
    }

    return true;
}

bool VHDLSemantics::isRangeDirectionAllowed(RangeDirection r) { return r == hif::dir_upto || hif::dir_downto; }

bool VHDLSemantics::isTypeAllowedAsCase(Type *t)
{
    // from standard: discrete types and mono-dimensional array of char.
    return isTemplateAllowedType(t);
}

bool VHDLSemantics::isLanguageIdAllowed(LanguageID id) { return id == hif::rtl || id == hif::psl; }

bool VHDLSemantics::isTypeAllowedAsPort(Type *t) { return isTypeAllowed(t); }

bool VHDLSemantics::isValueAllowedInPortBinding(Value * /*o*/)
{
    // TODO: Check if is allowed casts, slices, etc.
    return true;
}

bool VHDLSemantics::isTypeAllowedForConstValue(ConstValue *cv, Type *synType)
{
    Type *baseSynType = getBaseType(synType, true, this);

    Type *dt = this->getTypeForConstant(cv);
    hif::EqualsOptions opt;
    opt.checkOnlyTypes    = true;
    opt.handleVectorTypes = true;

    const bool res = hif::equals(dt, baseSynType, opt);
    delete dt;

    return res;
}

bool VHDLSemantics::hasBitwiseOperationsOnBits(const bool isLogic) const { return isLogic; }
bool VHDLSemantics::isSupported(Operator operation)
{
    return operation != op_none && operation != op_pow && operation != op_ref && operation != op_deref;
}

bool VHDLSemantics::isForbiddenName(Declaration *decl)
{
    if (hif::declarationIsPartOfStandard(decl))
        return false;
    std::string key(decl->getName());
    std::transform(key.begin(), key.end(), key.begin(), _toLower);
    if (_forbiddenNames.find(key) != _forbiddenNames.end())
        return true;

    return false;
}
bool VHDLSemantics::isSliceTypeRebased() { return false; }

bool VHDLSemantics::isSyntacticTypeRebased() { return false; }

void VHDLSemantics::setUsePsl(const bool v) { _usingPsl = v; }

bool VHDLSemantics::usingPsl() { return _usingPsl; }

} // namespace semantics
} // namespace hif
