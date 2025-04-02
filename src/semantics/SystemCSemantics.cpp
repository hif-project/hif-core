/// @file SystemCSemantics.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdint.h>
#include <string>

#include "hif/BiVisitor.hpp"
#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/backends/backends.hpp"
#include "hif/hifIOUtils.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/SemanticAnalysis.hpp"
#include "hif/semantics/semantics.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#    pragma clang diagnostic ignored "-Wunused-function"
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

// ///////////////////////////////////////////////////////////////////
// SystemCAnalysis
// ///////////////////////////////////////////////////////////////////
class SystemCAnalysis : public SemanticAnalysis, public BiVisitor<SystemCAnalysis>
{
public:
    SystemCAnalysis(Operator _currOperator, Object *_srcObj);
    virtual ~SystemCAnalysis();

    /// @name Unary operators
    /// @{

    void map(Object *);
    void map(Bitvector *op1);
    void map(Bit *op1);
    void map(Event *op1);
    void map(Int *op1);
    void map(Real *op1);
    void map(Bool *op1);
    void map(TypeReference *op1);
    void map(String *op1);

    /// @}

    /// @name Binary operators
    /// @{

    // base map
    void map(Object *op1, Object *op2);

    // Maps to Type.
    void map(Reference *op1, Type *op2);
    void map(TypeReference *op1, Type *op2);
    void map(ViewReference *op1, Type *op2);

    // Maps to Array
    void map(Array *op1, Array *op2);
    void map(Int *op1, Array *op2);
    void map(Pointer *op1, Array *op2);
    void map(ViewReference *op1, Array *op2);

    // Maps to Bitvector
    void map(Bit *op1, Bitvector *op2);
    void map(Bitvector *op1, Bitvector *op2);
    void map(Bool *op1, Bitvector *op2);
    void map(Real *op1, Bitvector *op2);
    void map(Int *op1, Bitvector *op2);

    // Maps to Int
    void map(Int *op1, Int *op2);
    void map(Bitvector *op1, Int *op2);
    void map(Bit *op1, Int *op2);
    void map(Real *op1, Int *op2);
    void map(Bool *op1, Int *op2);
    void map(Time *op1, Int *op2);
    void map(Array *op1, Int *op2);
    void map(Pointer *op1, Int *op2);

    // Maps to Bit
    void map(Bit *op1, Bit *op2);
    void map(Bitvector *op1, Bit *op2);
    void map(Int *op1, Bit *op2);
    void map(Real *op1, Bit *op2);
    void map(Bool *op1, Bit *op2);

    // Maps to Bool
    void map(Bool *op1, Bool *op2);
    void map(Bitvector *op1, Bool *op2);
    void map(Bit *op1, Bool *op2);
    void map(Real *op1, Bool *op2);
    void map(Int *op1, Bool *op2);

    // Maps to Real
    void map(Real *op1, Real *op2);
    void map(Bool *op1, Real *op2);
    void map(Bitvector *op1, Real *op2);
    void map(Int *op1, Real *op2);
    void map(Bit *op1, Real *op2);
    void map(Time *op1, Real *op2);

    // Maps to String
    void map(String *op1, String *op2);
    void map(Pointer *op1, String *op2);

    // Maps to TypeReference
    void map(TypeReference *op1, TypeReference *op2);
    void map(Type *op1, TypeReference *op2);
    void map(Reference *op1, TypeReference *op2);
    void map(ViewReference *op1, TypeReference *op2);

    // Maps to Reference
    void map(Reference *op1, Reference *op2);
    void map(Type *op1, Reference *op2);
    void map(TypeReference *op1, Reference *op2);
    void map(ViewReference *op1, Reference *op2);

    // Maps to Record
    void map(Record *op1, Record *op2);

    // Maps to Pointer
    void map(Array *op1, Pointer *op2);
    void map(Pointer *op1, Pointer *op2);
    void map(String *op1, Pointer *op2);
    void map(Int *op1, Pointer *op2);

    // Maps to Enum
    void map(Enum *op1, Enum *op2);

    // Maps to ViewReference
    void map(ViewReference *op1, ViewReference *op2);
    void map(Array *op1, ViewReference *op2);
    void map(Bool *op1, ViewReference *op2);

    // Maps to Time
    void map(Time *op1, Time *op2);
    void map(Int *op1, Time *op2);
    void map(Real *op1, Time *op2);

    // Maps to Char
    void map(Char *op1, Char *op2);
    void map(String *op1, Char *op2);

    // Maps to File
    void map(File *op1, File *op2);

    /// @}

private:
    SystemCSemantics *_systemcSem;

    virtual void _callMap(Object *op1);
    virtual void _callMap(Object *op1, Object *op2);

    /// @name Mapper utiliy functions
    /// @{

    bool _manageSmallBuiltinIntImplicitConversion(Int *op1);
    bool _manageSmallBuiltinIntImplicitConversion(Int *op1, Int *op2);
    bool _manageSmallBuiltinIntImplicitConversion(Int *op1, Type *op2);
    bool _manageSmallBuiltinIntImplicitConversion(Type *op1, Int *op2);

    /// @}

    /// @name Allowed operation checks
    /// @{

    bool _isAllowedUnaryOpOnBitvector(Bitvector *array);
    bool _isAllowedUnaryOpOnBit(Bit *b);
    bool _isAllowedUnaryOpOnInt(Int *i);
    bool _isAllowedUnaryOpOnReal(Real *r);
    bool _isAllowedUnaryOpOnBool(Bool *b);

    bool _isAllowedOnArray_Array(Array *op1, Array *op2);
    bool _isAllowedOnInt_Array(Int *op1, Array *op2);
    bool _isAllowedOnPointer_Array(Pointer *op1, Array *op2);
    bool _isAllowedOnViewReference_Array(ViewReference *op1, Array *op2);
    bool _isAllowedOnBit_Bitvector(Bit *op1, Bitvector *op2);
    bool _isAllowedOnBitvector_Bitvector(Bitvector *op1, Bitvector *op2);
    bool _isAllowedOnBool_Bitvector(Bool *op1, Bitvector *op2);
    bool _isAllowedOnReal_Bitvector(Real *op1, Bitvector *op2);
    bool _isAllowedOnInt_Bitvector(Int *op1, Bitvector *op2);
    bool _isAllowedOnInt_Int(Int *op1, Int *op2);
    bool _isAllowedOnBitvector_Int(Bitvector *op1, Int *op2);
    bool _isAllowedOnBit_Int(Bit *op1, Int *op2);
    bool _isAllowedOnReal_Int(Real *op1, Int *op2);
    bool _isAllowedOnBool_Int(Bool *op1, Int *op2);
    bool _isAllowedOnTime_Int(Time *t1, Int *op2);
    bool _isAllowedOnArray_Int(Array *op1, Int *op2);
    bool _isAllowedOnBit_Bit(Bit *op1, Bit *op2);
    bool _isAllowedOnBitvector_Bit(Bitvector *op1, Bit *op2);
    bool _isAllowedOnInt_Bit(Int *op1, Bit *op2);
    bool _isAllowedOnReal_Bit(Real *op1, Bit *op2);
    bool _isAllowedOnBool_Bit(Bool *op1, Bit *op2);
    bool _isAllowedOnBool_Bool(Bool *op1, Bool *op2);
    bool _isAllowedOnBitvector_Bool(Bitvector *op1, Bool *op2);
    bool _isAllowedOnBit_Bool(Bit *op1, Bool *op2);
    bool _isAllowedOnReal_Bool(Real *op1, Bool *op2);
    bool _isAllowedOnInt_Bool(Int *op1, Bool *op2);
    bool _isAllowedOnReal_Real(Real *op1, Real *op2);
    bool _isAllowedOnBool_Real(Bool *op1, Real *op2);
    bool _isAllowedOnBitvector_Real(Bitvector *op1, Real *op2);
    bool _isAllowedOnInt_Real(Int *op1, Real *op2);
    bool _isAllowedOnBit_Real(Bit *op1, Real *op2);
    bool _isAllowedOnTime_Real(Time *op1, Real *op2);
    bool _isAllowedOnString_String(String *op1, String *op2);
    bool _isAllowedOnRecord_Record(Record *op1, Record *op2);
    bool _isAllowedOnPointer_Pointer(Pointer *op1, Pointer *op2);
    bool _isAllowedOnString_Pointer(String *op1, Pointer *op2);
    bool _isAllowedOnPointer_String(Pointer *op1, String *op2);
    bool _isAllowedOnEnum_Enum(Enum *op1, Enum *op2);
    bool _isAllowedOnViewReference_ViewReference(ViewReference *op1, ViewReference *op2);
    bool _isAllowedOnArray_ViewReference(Array *op1, ViewReference *op2);
    bool _isAllowedOnBool_ViewReference(Bool *op1, ViewReference *op2);
    bool _isAllowedOnTime_Time(Time *op1, Time *op2);
    bool _isAllowedOnInt_Time(Int *op1, Time *op2);
    bool _isAllowedOnReal_Time(Real *op1, Time *op2);
    bool _isAllowedOnChar_Char(Char *op1, Char *op2);
    bool _isAllowedOnString_Char(String *op1, Char *op2);
    bool _isAllowedOnFile_File(File *op1, File *op2);

    /// @}

    /// @brief SystemC assign on signal or port are implict calls on sc_port or
    /// sc_signal instance assign operator.
    /// Therefore the operator is an implict conversion.
    void _checkSignalOrPortAssign();

    // disabled.
    SystemCAnalysis(const SystemCAnalysis &);
    SystemCAnalysis &operator=(const SystemCAnalysis &);
};
// ///////////////////////////////////////////////////////////////////
// SystemCCastMapper
// ///////////////////////////////////////////////////////////////////
class SystemCCastAnalysis : public BiVisitor<SystemCCastAnalysis>
{
public:
    SystemCCastAnalysis(Value *valueToCast, Type *srcType);
    virtual ~SystemCCastAnalysis();

    /// @brief Main function used to return the Value resulting from cast
    /// of the valueToCast parameter, to the castType parameter.
    Value *explicitCast(Type *castType);

    /// @brief Function to add collected includes.
    void generateIncludes(Value *startingObj);

    /// @name Cast maps
    /// @{

    // Default map.
    void map(Object *, Object *);

    // Maps to generic types
    void map(Reference *vi, Type *t);
    void map(TypeReference *vi, Type *t);

    // Maps for casts to integer type.
    void map(Int *vi, Int *i);
    void map(Bit *vi, Int *i);
    void map(Bool *vi, Int *i);
    void map(Real *vi, Int *i);
    void map(Bitvector *vi, Int *i);

    // Maps for cast to array type
    void map(Array *vi, Array *array);
    void map(Bitvector *vi, Array *array);
    void map(Pointer *vi, Array *array);

    // Maps for cast to Bitvector type
    void map(Int *vi, Bitvector *bv);
    void map(Bit *vi, Bitvector *bv);
    void map(Bool *vi, Bitvector *bv);
    void map(Real *vi, Bitvector *bv);
    void map(Pointer *vi, Bitvector *bv);
    void map(Bitvector *vi, Bitvector *bv);
    void map(Array *vi, Bitvector *bv);
    void map(ViewReference *vi, Bitvector *bv);
    void map(TypeReference *vi, Bitvector *bv);

    // Maps for cast to Bit type
    void map(Bit *vi, Bit *bit);
    void map(Bitvector *vi, Bit *bit);
    void map(Char *vi, Bit *bit);
    void map(Int *vi, Bit *bit);
    void map(Bool *vi, Bit *bit);
    void map(Real *vi, Bit *bit);
    void map(ViewReference *vi, Bit *bit);

    // Maps for cast to bool type
    void map(Bool *vi, Bool *b);
    void map(Bit *vi, Bool *b);
    void map(Bitvector *vi, Bool *b);
    void map(Int *vi, Bool *b);
    void map(Real *vi, Bool *b);
    void map(ViewReference *vi, Bool *b);

    // Maps for cast to string type
    void map(Bitvector *vi, String *s);
    void map(Int *vi, String *s);
    void map(Char *vi, String *s);
    void map(String *vi, String *s);

    // Maps to cast to pointer type
    void map(Pointer *vi, Pointer *ptr);
    void map(Int *vi, Pointer *ptr);
    void map(Array *vi, Pointer *ptr);
    void map(String *vi, Pointer *ptr);

    // Maps to cast to reference types
    void map(Type *vi, Reference *ref);
    void map(Reference *vi, Reference *ref);
    void map(TypeReference *vi, Reference *ref);

    // Maps to times
    void map(Record *vi, Time *t);
    void map(Time *vi, Time *t);

    // Maps to real
    void map(Bit *vi, Real *r);
    void map(Bool *vi, Real *r);
    void map(Bitvector *vi, Real *r);
    void map(Int *vi, Real *r);
    void map(Real *vi, Real *r);
    void map(Time *vi, Real *r);

    // Maps to File
    void map(String *vi, File *r);

    // Maps to ViewReference
    void map(Bit *vi, ViewReference *vr);
    void map(Bitvector *vi, ViewReference *vr);
    void map(ViewReference *vi, ViewReference *vr);

    // Maps from Enum
    void map(Enum *vi, Type *t);
    void map(Enum *vi, Reference *t);

    // Maps to Char
    void map(Bit *vi, Char *c);
    void map(Bool *vi, Char *c);
    void map(String *vi, Char *c);

    // Maps to Record
    void map(Record *vi, Record *r);

    // Maps to Typerefs
    void map(TypeReference *vi, TypeReference *tr);
    void map(Type *vi, TypeReference *tr);
    void map(Enum *vi, TypeReference *tr);
    void map(Reference *vi, TypeReference *tr);

    /// @}

private:
    SystemCSemantics *_sem;
    Value *_inputVal;
    Type *_sourceType;
    Value *_result;
    std::set<std::string> _neededIncludes;
    HifFactory _factory;

    /// @name Mapper utiliy functions
    /// @{

    Value *_getCastFromValueToType(Value *from, Type *to);
    std::string _getToIntFCallName(Int *i);

    /// @}

    // disabled.
    SystemCCastAnalysis(const SystemCCastAnalysis &);
    SystemCCastAnalysis &operator=(const SystemCCastAnalysis &);
};
// ///////////////////////////////////////////////////////////////////
// SystemCTypeVisitor
// ///////////////////////////////////////////////////////////////////
class SystemCTypeVisitor : public HifVisitor
{
public:
    SystemCTypeVisitor();
    virtual ~SystemCTypeVisitor();

    Type *getResult();

    virtual int visitBool(Bool &o);
    virtual int visitBit(Bit &o);
    virtual int visitInt(Int &o);
    virtual int visitReal(Real &o);
    virtual int visitUnsigned(Unsigned &o);
    virtual int visitSigned(Signed &o);
    virtual int visitArray(Array &o);
    virtual int visitBitvector(Bitvector &o);
    virtual int visitString(String &o);
    virtual int visitTypeReference(TypeReference &o);
    virtual int visitPointer(Pointer &o);
    virtual int visitReference(Reference &o);
    virtual int visitEnum(Enum &o);
    virtual int visitRecord(Record &o);
    virtual int visitViewReference(ViewReference &o);
    virtual int visitTime(Time &o);
    virtual int visitChar(Char &o);
    virtual int visitFile(File &o);
    virtual int visitEvent(Event &o);

private:
    SystemCSemantics *_sem;
    HifFactory _factory;
    Type *_result;
    CopyOptions _copts;

    Type *_getStandardMappedType(Type *t);

    SystemCTypeVisitor(const SystemCTypeVisitor &);
    SystemCTypeVisitor &operator=(const SystemCTypeVisitor &);
};
// ///////////////////////////////////////////////////////////////////
// SystemCTemplateTypeVisitor
// ///////////////////////////////////////////////////////////////////
class SystemCTemplateTypeVisitor : public HifVisitor
{
public:
    SystemCTemplateTypeVisitor();
    virtual ~SystemCTemplateTypeVisitor();

    Type *getResult();

    virtual int visitArray(Array &o);
    virtual int visitBit(Bit &o);
    virtual int visitBitvector(Bitvector &o);
    virtual int visitInt(Int &o);
    virtual int visitReal(Real &o);
    virtual int visitRecord(Record &o);
    virtual int visitString(String &o);
    virtual int visitTime(Time &o);
    virtual int visitTypeReference(TypeReference &o);

private:
    SystemCSemantics *_sem;
    HifFactory _factory;
    Type *_result;

    Type *_getSuitableIntegerByType(Type *t);

    SystemCTemplateTypeVisitor(const SystemCTemplateTypeVisitor &);
    SystemCTemplateTypeVisitor &operator=(const SystemCTemplateTypeVisitor &);
};
// ///////////////////////////////////////////////////////////////////
// SystemCTypeDefaultValueVisitor
// ///////////////////////////////////////////////////////////////////
class SystemCTypeDefaultValueVisitor : public HifVisitor
{
public:
    SystemCTypeDefaultValueVisitor(Type *originalType, Declaration *d);
    virtual ~SystemCTypeDefaultValueVisitor();

    Value *getResult();

    virtual int visitArray(Array &o);
    virtual int visitBool(Bool &o);
    virtual int visitBit(Bit &o);
    virtual int visitBitvector(Bitvector &o);
    virtual int visitInt(Int &o);
    virtual int visitReal(Real &o);
    virtual int visitRecord(Record &o);
    virtual int visitString(String &o);
    virtual int visitTime(Time &o);

private:
    Type *_originalType;
    Declaration *_decl;
    SystemCSemantics *_sem;
    HifFactory _factory;
    Value *_result;

    ConstValue *_makeBitvectorValueOfBitValue(std::uint64_t size, BitConstant b);

    SystemCTypeDefaultValueVisitor(const SystemCTypeDefaultValueVisitor &);
    SystemCTypeDefaultValueVisitor &operator=(const SystemCTypeDefaultValueVisitor &);
};
// ///////////////////////////////////////////////////////////////////
// SystemCSuggestedTypeVisitor
// ///////////////////////////////////////////////////////////////////
class SystemCSuggestedTypeVisitor : public HifVisitor
{
public:
    SystemCSuggestedTypeVisitor(Operator operation, Type *opType, Object *startingObject, const bool isOp1);
    virtual ~SystemCSuggestedTypeVisitor();

    Type *getResult();

    virtual int visitArray(Array &o);
    virtual int visitBool(Bool &o);
    virtual int visitBit(Bit &o);
    virtual int visitBitvector(Bitvector &o);
    virtual int visitInt(Int &o);
    virtual int visitTime(Time &o);

private:
    Operator _oper;
    Type *_opType;
    Object *_startingObject;
    const bool _isOp1;
    SystemCSemantics *_sem;
    HifFactory _factory;
    Type *_result;

    SystemCSuggestedTypeVisitor(const SystemCSuggestedTypeVisitor &);
    SystemCSuggestedTypeVisitor &operator=(const SystemCSuggestedTypeVisitor &);
};
// ///////////////////////////////////////////////////////////////////
// SystemCConstantTypeVisitor
// ///////////////////////////////////////////////////////////////////
class SystemCConstantTypeVisitor : public HifVisitor
{
public:
    SystemCConstantTypeVisitor();
    virtual ~SystemCConstantTypeVisitor();

    Type *getResult();

    virtual int visitBitValue(BitValue &o);
    virtual int visitBitvectorValue(BitvectorValue &o);
    virtual int visitBoolValue(BoolValue &o);
    virtual int visitCharValue(CharValue &o);
    virtual int visitIntValue(IntValue &o);
    virtual int visitRealValue(RealValue &o);
    virtual int visitStringValue(StringValue &o);
    virtual int visitTimeValue(TimeValue &o);

private:
    SystemCSemantics *_sem;
    HifFactory _factory;
    Type *_result;

    SystemCConstantTypeVisitor(const SystemCConstantTypeVisitor &);
    SystemCConstantTypeVisitor &operator=(const SystemCConstantTypeVisitor &);
};
// ///////////////////////////////////////////////////////////////////
// SystemCBoolConversionVisitor
// ///////////////////////////////////////////////////////////////////
class SystemCBoolConversionVisitor : public HifVisitor
{
public:
    SystemCBoolConversionVisitor(Value *value);
    virtual ~SystemCBoolConversionVisitor();

    Value *getResult();

    virtual int visitArray(Array &o);
    virtual int visitBit(Bit &o);
    virtual int visitBitvector(Bitvector &o);
    virtual int visitEnum(Enum &o);
    virtual int visitFile(File &o);
    virtual int visitLibrary(Library &o);
    virtual int visitRecord(Record &o);
    virtual int visitReference(Reference &o);
    virtual int visitString(String &o);
    virtual int visitSigned(Signed &o);
    virtual int visitTime(Time &o);
    virtual int visitUnsigned(Unsigned &o);
    virtual int visitViewReference(ViewReference &o);

private:
    Value *_value;
    SystemCSemantics *_sem;
    HifFactory _factory;
    Value *_result;

    Expression *_getVectorsConversion(Type *t);

    SystemCBoolConversionVisitor(const SystemCBoolConversionVisitor &);
    SystemCBoolConversionVisitor &operator=(const SystemCBoolConversionVisitor &);
};
// ///////////////////////////////////////////////////////////////////
// Common utility functions
// ///////////////////////////////////////////////////////////////////

bool _isBuiltinIntSize(std::uint64_t size) { return (size == 8 || size == 16 || size == 32 || size == 64); }

Type::TypeVariant _getSuitableIntTypeVariantBySize(std::uint64_t size)
{
    if (_isBuiltinIntSize(size))
        return Type::NATIVE_TYPE;
    else if (size == 0 || size <= 64)
        return Type::SYSTEMC_INT_SC_INT;
    else
        return Type::SYSTEMC_INT_SC_BIGINT;
}

bool _isSmallBuiltinInt(Int *i)
{
    std::uint64_t size = spanGetBitwidth(i->getSpan(), SystemCSemantics::getInstance());
    if (i->getTypeVariant() == Type::NATIVE_TYPE) {
        return (size == 8 || size == 16);
    } else if (i->getTypeVariant() == Type::SYSTEMC_INT_BITFIELD) {
        return (size != 0 && size < 32);
    }

    return false;
}

bool _isBuiltinInt(Int *i)
{
    if (i == nullptr)
        return false;
    if (i->getTypeVariant() != Type::NATIVE_TYPE)
        return false;
    std::uint64_t size = spanGetBitwidth(i->getSpan(), SystemCSemantics::getInstance());
    return _isBuiltinIntSize(size);
}

Int *_getBuiltinInt(Int *i, const bool fresh, const bool atLeast32 = false)
{
    Int *ret = fresh ? hif::copy(i) : i;
    ret->setTypeVariant(Type::NATIVE_TYPE);
    std::uint64_t size = spanGetBitwidth(i->getSpan(), SystemCSemantics::getInstance());
    if (atLeast32) {
        if (size != 0 && size <= 32)
            delete ret->setSpan(new Range(31, 0));
        else
            delete ret->setSpan(new Range(63, 0));
    } else {
        if (size == 0 || size > 64)
            delete ret->setSpan(new Range(63, 0));
        else if (size <= 8)
            delete ret->setSpan(new Range(7, 0));
        else if (size <= 16)
            delete ret->setSpan(new Range(15, 0));
        else if (size <= 32)
            delete ret->setSpan(new Range(31, 0));
        else /*if (size <= 64)*/
            delete ret->setSpan(new Range(63, 0));
    }

    return ret;
}

Int *_getBuiltinInt(Type *i, const bool atLeast32 = false)
{
    if (i == nullptr)
        return nullptr;
    ILanguageSemantics *sem = SystemCSemantics::getInstance();
    Int ret;
    ret.setSpan(hif::copy(hif::typeGetSpan(i, sem)));
    ret.setSigned(hif::typeIsSigned(i, sem));
    ret.setConstexpr(hif::typeIsConstexpr(i, sem));
    return _getBuiltinInt(&ret, true, atLeast32);
}

void _setLogicFlagByTypes(Type *ref, Type *t1, Type *t2, const bool orMode = true)
{
    SystemCSemantics *sem = SystemCSemantics::getInstance();
    messageAssert(ref != nullptr && t1 != nullptr && t2 != nullptr, "Expected instantiated objects", nullptr, sem);
    if (orMode) {
        typeSetLogic(ref, typeIsLogic(t1, sem) || typeIsLogic(t2, sem), sem);
    } else // andMode
    {
        typeSetLogic(ref, typeIsLogic(t1, sem) && typeIsLogic(t2, sem), sem);
    }
}

void _setResolveFlagByTypes(Type *ref, Type *t1, Type *t2, const bool orMode = true)
{
    SystemCSemantics *sem = SystemCSemantics::getInstance();
    messageAssert(ref != nullptr && t1 != nullptr && t2 != nullptr, "Expected instantiated objects", nullptr, sem);
    if (orMode) {
        typeSetResolved(ref, typeIsResolved(t1, sem) || typeIsResolved(t2, sem), sem);
    } else // andMode
    {
        typeSetResolved(ref, typeIsResolved(t1, sem) && typeIsResolved(t2, sem), sem);
    }
}

void _setSignedFlagByTypes(Type *ref, Type *t1, Type *t2, const bool orMode = true)
{
    SystemCSemantics *sem = SystemCSemantics::getInstance();
    messageAssert(ref != nullptr && t1 != nullptr && t2 != nullptr, "Expected instantiated objects", nullptr, sem);
    if (orMode) {
        typeSetSigned(ref, typeIsSigned(t1, sem) || typeIsSigned(t2, sem), sem);
    } else // andMode
    {
        typeSetSigned(ref, typeIsSigned(t1, sem) && typeIsSigned(t2, sem), sem);
    }
}

void _setConstexprFlagByTypes(Type *ref, Type *t1, Type *t2, const bool orMode = false)
{
    SystemCSemantics *sem = SystemCSemantics::getInstance();
    messageAssert(ref != nullptr && t1 != nullptr && t2 != nullptr, "Expected instantiated objects", nullptr, sem);
    if (orMode) {
        typeSetConstexpr(ref, typeIsConstexpr(t1, sem) || typeIsConstexpr(t2, sem));
    } else // andMode
    {
        typeSetConstexpr(ref, typeIsConstexpr(t1, sem) && typeIsConstexpr(t2, sem));
    }
}

void _setTypeVariantFlagByTypes(Type *ref, Type *t1, Type *t2, const bool error = true)
{
    SystemCSemantics *sem = SystemCSemantics::getInstance();
    messageAssert(ref != nullptr && t1 != nullptr && t2 != nullptr, "Expected instantiated objects", nullptr, sem);

    if (t1->getTypeVariant() == t2->getTypeVariant()) {
        ref->setTypeVariant(t1->getTypeVariant());
        return;
    }

    messageAssert(!error, "Unexpected different type variants", t1, sem);
}

bool _getExpressionValue(Object *sourceObj, std::int64_t &retval, const bool value2 = true)
{
    SystemCSemantics *sem = SystemCSemantics::getInstance();

    Expression *expr = dynamic_cast<Expression *>(sourceObj);
    if (expr == nullptr)
        return false;

    ConstValue *cv = nullptr;
    if (value2)
        cv = dynamic_cast<ConstValue *>(expr->getValue2());
    else
        cv = dynamic_cast<ConstValue *>(expr->getValue1());
    if (cv == nullptr)
        return false;

    IntValue *intCv = dynamic_cast<IntValue *>(cv);
    if (intCv != nullptr) {
        retval = intCv->getValue();
        return true;
    }

    Int tmp;
    tmp.setSpan(new Range(63, 0));
    tmp.setSigned(true);
    tmp.setConstexpr(false);

    ConstValue *transCv = hif::manipulation::transformConstant(cv, &tmp, sem);
    if (transCv == nullptr)
        return false;

    intCv = dynamic_cast<IntValue *>(transCv);
    if (intCv != nullptr) {
        retval = intCv->getValue();
        delete intCv; // is fresh
        return true;
    }

    delete transCv;
    return false;
}

bool _isShiftNoRotateOp(Operator operation)
{
    return hif::operatorIsShift(operation) && !hif::operatorIsRotate(operation);
}

bool _isEqualityOp(Operator operation)
{
    return (operation == op_eq || operation == op_case_eq || operation == op_neq || operation == op_case_neq);
}

bool _isComparisonOp(Operator operation)
{
    return (operation == op_le || operation == op_lt || operation == op_ge || operation == op_gt);
}

bool _isArithmeticAllowedOp(Operator operation)
{
    return hif::operatorIsArithmetic(operation) && operation != op_pow && operation != op_abs && operation != op_log;
}

bool _checkConditionBit(Bit *b, Object *o)
{
    WhenAlt *wa = dynamic_cast<WhenAlt *>(o);
    if (wa != nullptr) {
        When *w = dynamic_cast<When *>(wa->getParent());
        if (w != nullptr && w->isLogicTernary()) {
            return b->isLogic();
        }
    }

    //sc_bit: OK
    //sc_logic: KO
    return !b->isLogic();
}

bool _checkConditionInteger(Int *i)
{
    //	sc_int: OK
    //	sc_bigint: KO
    //	sc_uint: OK
    //	sc_biguint: KO

    if (i->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT)
        return false;
    if (i->getTypeVariant() == Type::SYSTEMC_INT_SC_INT)
        return !SystemCSemantics::getInstance()->usingHdtlib();

    return _isBuiltinInt(i);
}

bool _checkValidReal(Real *real)
{
    // These types are mapped in real objects:
    //	* float: 32
    //	* double: 64
    //	* long double: 128
    Range *span          = real->getSpan();
    std::uint64_t s = spanGetBitwidth(span, SystemCSemantics::getInstance());
    return (span != nullptr && (s == 32 || s == 64 || s == 128));
}

Pointer *_makeSystemCPointer(Type *to)
{
    Pointer *p     = new Pointer();
    Reference *ref = dynamic_cast<Reference *>(to);
    if (ref != nullptr)
        p->setType(hif::copy(ref->getType()));
    else
        p->setType(hif::copy(to));

    return p;
}

Int *_makeSystemCIntFromSize(
    std::int64_t spanSize,
    const bool sign,
    const bool constExpr,
    const Type::TypeVariant variant = Type::NATIVE_TYPE)
{
    hif::HifFactory f(SystemCSemantics::getInstance());
    return f.integer(f.range(spanSize - 1, 0), sign, constExpr, variant);
}

Int *_makeSystemCIntFromRange(
    Range *r,
    const bool sign,
    const bool constExpr,
    const Type::TypeVariant variant = Type::NATIVE_TYPE)
{
    hif::HifFactory f(SystemCSemantics::getInstance());
    return f.integer(r, sign, constExpr, variant);
}

FunctionCall *_makeMemberFunctionCall(std::string s, Value *instance, Value *param = nullptr)
{
    FunctionCall *fCall = new FunctionCall();
    fCall->setName(s.c_str());
    fCall->setInstance(hif::copy(instance));

    if (param != nullptr) {
        ParameterAssign *pass = new ParameterAssign();
        pass->setName("param");
        pass->setValue(param);
        fCall->parameterAssigns.push_back(pass);
    }

    return fCall;
}

void _printSystemCLShiftWarning()
{
    raiseUniqueWarning("System C \"sll\" or \"sla\" operators map result in a number "
                       "of bit that are the sum of left operand span and the value "
                       "of left operand.\nThis semantics implementation do not consider this case "
                       "and map result always in the number of bits of left operand.");
}

void _printSystemCLPlusOnBigsWarning()
{
    raiseUniqueWarning("System C \"plus\" operator with almost one \"big\" operand may result in "
                       "a minor number of bits based on if both operands are equal to zero."
                       "\nThis semantics implementation always consider both operands different from zero.");
}

void _printSystemCLMinusOnBigsWarning()
{
    raiseUniqueWarning("System C \"minus\" operator with almost one \"big\" operand may result in "
                       "a minor number of bits based on if the result of of operation is 0."
                       "\nThis semantics implementation always consider result different from 0.");
}

void _printSystemCLDivOnBigsWarning()
{
    raiseUniqueWarning("System C \"div\" operator with almost one \"big\" operand may result in "
                       "a minor number of bits based on if dividend is less than the divisor."
                       "\nThis semantics implementation always consider dividend greater than divisor.");
}

void _printSystemCLRemOnBigsWarning()
{
    raiseUniqueWarning("System C \"rem\" operator with almost one \"big\" operand may result in "
                       "a minor number of bits based on if the result of operation is 0."
                       "\nThis semantics implementation do not consider this case.");
}

void _printSystemCLMultOnBigsWarning()
{
    raiseUniqueWarning("System C \"mult\" operator with almost one \"big\" operator may result in  "
                       "a minor number of bits based on if almost one operand is 0 (result = 0)."
                       "\nThis semantics implementation do not consider this case.");
}
// ///////////////////////////////////////////////////////////////////
// SystemCAnalysisMapper
// ///////////////////////////////////////////////////////////////////
SystemCAnalysis::SystemCAnalysis(Operator currOperator, Object *srcObj)
    : SemanticAnalysis(SystemCSemantics::getInstance(), currOperator, srcObj)
    , BiVisitor<SystemCAnalysis>()
    , _systemcSem(SystemCSemantics::getInstance())
{
    _checkSignalOrPortAssign();
}

SystemCAnalysis::~SystemCAnalysis()
{
    // ntd
}

void SystemCAnalysis::_checkSignalOrPortAssign()
{
    // operation must be assign and  source object must be an assignement.
    /// @warning Do the same for signal/port initial value?
    Assign *assign = dynamic_cast<Assign *>(_srcObj);
    if (!hif::operatorIsAssignment(_currOperator) || assign == nullptr)
        return;

    // check if target it is a port or a signal
    // Note. target can not be a cast.
    Identifier *n = dynamic_cast<Identifier *>(hif::getTerminalPrefix(assign->getLeftHandSide()));
    if (n == nullptr)
        return;

    DataDeclaration *decl = getDeclaration(n, _sem);
    if (decl == nullptr) {
        messageError("Declaration not found!", nullptr, nullptr);
    }

    if (dynamic_cast<Port *>(decl) == nullptr && dynamic_cast<Signal *>(decl) == nullptr)
        return;

    // Different types! not allowed -> return true.
    _currOperator = op_conv;
}

bool SystemCAnalysis::_manageSmallBuiltinIntImplicitConversion(Int *op1)
{
    if (!_isSmallBuiltinInt(op1))
        return false;

    Int op1_32bit;
    op1_32bit.setSpan(new Range(31, 0));
    op1_32bit.setSigned(op1->isSigned());
    op1_32bit.setConstexpr(op1->isConstexpr());
    op1_32bit.setTypeVariant(Type::NATIVE_TYPE);
    op1 = &op1_32bit;

    map(op1);
    return true;
}

bool SystemCAnalysis::_manageSmallBuiltinIntImplicitConversion(Int *op1, Int *op2)
{
    const bool isSmall1 = _isSmallBuiltinInt(op1);
    const bool isSmall2 = _isSmallBuiltinInt(op2);
    if (!isSmall1 && !isSmall2)
        return false;

    Int op1_32bit;
    if (isSmall1) {
        op1_32bit.setSpan(new Range(31, 0));
        op1_32bit.setSigned(true);
        op1_32bit.setConstexpr(op1->isConstexpr());
        op1_32bit.setTypeVariant(Type::NATIVE_TYPE);
        op1 = &op1_32bit;
    }

    Int op2_32bit;
    if (isSmall2) {
        op2_32bit.setSpan(new Range(31, 0));
        op2_32bit.setSigned(true);
        op2_32bit.setConstexpr(op2->isConstexpr());
        op2_32bit.setTypeVariant(Type::NATIVE_TYPE);
        op2 = &op2_32bit;
    }

    map(op1, op2);
    return true;
}

bool SystemCAnalysis::_manageSmallBuiltinIntImplicitConversion(Int *op1, Type *op2)
{
    if (!_isSmallBuiltinInt(op1))
        return false;

    Int op1_32bit;
    op1_32bit.setSpan(new Range(31, 0));
    op1_32bit.setSigned(true);
    op1_32bit.setConstexpr(op1->isConstexpr());
    op1_32bit.setTypeVariant(Type::NATIVE_TYPE);
    op1 = &op1_32bit;

    map(op1, op2);
    return true;
}

bool SystemCAnalysis::_manageSmallBuiltinIntImplicitConversion(Type *op1, Int *op2)
{
    if (!_isSmallBuiltinInt(op2))
        return false;

    Int op2_32bit;
    op2_32bit.setSpan(new Range(31, 0));
    op2_32bit.setSigned(true);
    op2_32bit.setConstexpr(op2->isConstexpr());
    op2_32bit.setTypeVariant(Type::NATIVE_TYPE);
    op2 = &op2_32bit;

    map(op1, op2);
    return true;
}
// ///////////////////////////////////////////////////////////////////
// Allowed operation checks
// ///////////////////////////////////////////////////////////////////
bool SystemCAnalysis::_isAllowedUnaryOpOnBitvector(Bitvector * /*array*/)
{
    return (
        _currOperator == op_bnot || _currOperator == op_andrd || _currOperator == op_orrd || _currOperator == op_xorrd);
}

bool SystemCAnalysis::_isAllowedUnaryOpOnBit(Bit *b)
{
    if (b->isLogic()) {
        return (_currOperator == op_bnot);
    } else {
        return (_currOperator == op_plus || _currOperator == op_not || _currOperator == op_bnot);
    }
}

bool SystemCAnalysis::_isAllowedUnaryOpOnInt(Int *i)
{
    if (_isBuiltinInt(i) || i->getTypeVariant() == Type::SYSTEMC_INT_BITFIELD) {
        return (_isArithmeticAllowedOp(_currOperator) || _currOperator == op_not || _currOperator == op_bnot);
    }

    if (i->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT) {
        // sc_bigint and sc_biguint: all unary op except not
        return (_currOperator != op_not);
    } else {
        // sc_int or sc_uint
        // all unary operators are allowed
        return true;
    }
}

bool SystemCAnalysis::_isAllowedUnaryOpOnReal(Real * /*r*/)
{
    return (_isArithmeticAllowedOp(_currOperator) || _currOperator == op_not);
}

bool SystemCAnalysis::_isAllowedUnaryOpOnBool(Bool * /*b*/)
{
    // Note: arithmetic unary is only plus and minus
    return (_isArithmeticAllowedOp(_currOperator) || _currOperator == op_not || _currOperator == op_bnot);
}

bool SystemCAnalysis::_isAllowedOnArray_Array(Array * /*op1*/, Array * /*op2*/)
{
    if (!hif::operatorIsAssignment(_currOperator) && _currOperator != op_eq && _currOperator != op_case_eq) {
        return false;
    }

    return true;
}

bool SystemCAnalysis::_isAllowedOnInt_Array(Int *op1, Array *op2)
{
    // except minus map is commutative
    if (_currOperator == op_minus)
        return false;
    return _isAllowedOnArray_Int(op2, op1);
}

bool SystemCAnalysis::_isAllowedOnPointer_Array(Pointer * /*op1*/, Array * /*op2*/)
{
    // only assignment is allowed at the moment
    if (!hif::operatorIsAssignment(_currOperator))
        return false;

    return true;
}

bool SystemCAnalysis::_isAllowedOnViewReference_Array(ViewReference * /*op1*/, Array * /*op2*/)
{
    return _currOperator == op_concat;
}

bool SystemCAnalysis::_isAllowedOnBit_Bitvector(Bit *op1, Bitvector *op2)
{
    if (_isShiftNoRotateOp(_currOperator) || hif::operatorIsRotate(_currOperator))
        return false;
    return (_isAllowedOnBitvector_Bit(op2, op1));
}

bool SystemCAnalysis::_isAllowedOnBitvector_Bitvector(Bitvector *op1, Bitvector *op2)
{
    if (_currOperator == op_conv && op2->getTypeVariant() != Type::NATIVE_TYPE) {
        return op1->getTypeVariant() == op2->getTypeVariant();
    }

    return (
        _isEqualityOp(_currOperator) || hif::operatorIsAssignment(_currOperator) ||
        hif::operatorIsBitwise(_currOperator) || _currOperator == op_concat);
}

bool SystemCAnalysis::_isAllowedOnBool_Bitvector(Bool *op1, Bitvector *op2)
{
    if (_isShiftNoRotateOp(_currOperator) || hif::operatorIsRotate(_currOperator)) {
        return false;
    }
    return (_isAllowedOnBitvector_Bool(op2, op1));
}

bool SystemCAnalysis::_isAllowedOnReal_Bitvector(Real *op1, Bitvector *op2)
{
    if (_isShiftNoRotateOp(_currOperator))
        return false;
    return _isAllowedOnBitvector_Real(op2, op1);
}

bool SystemCAnalysis::_isAllowedOnInt_Bitvector(Int *op1, Bitvector *op2)
{
    messageAssert(op2->getSpan() != nullptr, "Array without span", op2, _sem);

    const bool hif_hdtlib = _systemcSem->usingHdtlib();
    if (hif_hdtlib && hif::operatorIsBitwise(_currOperator))
        return false;

    if (hif::operatorIsBitwise(_currOperator) || _isEqualityOp(_currOperator)) {
        return true;
    } else if (hif::operatorIsAssignment(_currOperator)) {
        return !_isBuiltinInt(op1);
    } else if (_currOperator == op_concat) {
        return (op1->getTypeVariant() != Type::SYSTEMC_INT_SC_BIGINT);
    }

    return false;
}

bool SystemCAnalysis::_isAllowedOnInt_Int(Int *op1, Int *op2)
{
    Type::TypeVariant tv1 = op1->getTypeVariant();
    Type::TypeVariant tv2 = op2->getTypeVariant();

    if (hif::operatorIsLogical(_currOperator)) {
        // allowed only if all operands are not "big"
        return (tv1 != Type::SYSTEMC_INT_SC_BIGINT && tv2 != Type::SYSTEMC_INT_SC_BIGINT);
    }

    if (_isShiftNoRotateOp(_currOperator)) {
        if (!_isBuiltinInt(op2)) {
            // Second operand must be builtin!
            // TODO: test seem to say that works also with sc_int..
            return false;
        }

        if (tv1 == Type::SYSTEMC_INT_SC_INT) {
            // for srl only sc_uint is allowed
            return (_currOperator != op_srl || !op1->isSigned());
        } else if (tv1 == Type::SYSTEMC_INT_SC_BIGINT) {
            // for srl only sc_biguint is allowed
            return (_currOperator != op_srl || !op1->isSigned());
        }
    }

    if (_currOperator == op_concat) {
        // only not for concatenation of 2 buitin integers
        return (!_isBuiltinInt(op1) || !_isBuiltinInt(op2));
    }

    if (_currOperator == op_pow) {
        // pow operands must be builtin!
        if (!_isBuiltinInt(op1) || !_isBuiltinInt(op2))
            return false;
        return true;
    }

    // all others currOperators are allowed
    return true;
}

bool SystemCAnalysis::_isAllowedOnBitvector_Int(Bitvector *op1, Int *op2)
{
    const bool isScBigInt = op2->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT;
    if (_isShiftNoRotateOp(_currOperator)) {
        // There are not allowed operations with big int op2
        if (isScBigInt)
            return false;

        // Arithmetic right shift allowed only with unsigned bv.
        if (_currOperator == op_sra)
            return (!op1->isSigned());

        return true;
    } else if (hif::operatorIsRotate(_currOperator)) {
        if (isScBigInt)
            return false;

        return true;
    } else if (hif::operatorIsAssignment(_currOperator)) {
        return true;
    }

    // other cases are the same of int-array case (=> eq + biwise)
    return _isAllowedOnInt_Bitvector(op2, op1);
}

bool SystemCAnalysis::_isAllowedOnBit_Int(Bit *op1, Int *op2)
{
    // Allowed currOperators are the same of int-bit except
    // - shift currOperators
    // - currOperator div
    // - currOperator rem
    const bool isScBigInt = op2->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT;

    if (_isShiftNoRotateOp(_currOperator)) {
        // Allowed for bit not logic and int not "big"
        return (!op1->isLogic() && !isScBigInt);
    } else if (_currOperator == op_rem || _currOperator == op_div || _currOperator == op_mod) {
        // not allowed
        return false;
    } else {
        /// @warning assign is actually allowed, but iff int value is 0 or 1
        return _isAllowedOnInt_Bit(op2, op1);
    }
}

bool SystemCAnalysis::_isAllowedOnReal_Int(Real *op1, Int *op2) { return _isAllowedOnInt_Real(op2, op1); }

bool SystemCAnalysis::_isAllowedOnBool_Int(Bool *op1, Int *op2)
{
    const bool isScBigInt = op2->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT;

    // all operator are allowed for int bool except:
    // - div
    // - rem
    // - shift with a sc_bigint or a sc_biguint
    if (_currOperator == op_div || _currOperator == op_rem || _currOperator == op_mod)
        return false;
    if (_isShiftNoRotateOp(_currOperator) && isScBigInt)
        return false;

    return _isAllowedOnInt_Bool(op2, op1);
}

bool SystemCAnalysis::_isAllowedOnTime_Int(Time * /*t1*/, Int * /*op2*/)
{
    return (_currOperator == op_mult || _currOperator == op_div);
}

bool SystemCAnalysis::_isAllowedOnArray_Int(Array * /*op1*/, Int *op2)
{
    if (!_isBuiltinInt(op2))
        return false;

    return (_currOperator == op_plus || _currOperator == op_minus);
}

bool SystemCAnalysis::_isAllowedOnBit_Bit(Bit *op1, Bit *op2)
{
    if (!op1->isLogic() && !op2->isLogic()) {
        // sc_bit, sc_bit
        return (
            _isEqualityOp(_currOperator) || hif::operatorIsAssignment(_currOperator) ||
            hif::operatorIsLogical(_currOperator) || hif::operatorIsBitwise(_currOperator) ||
            hif::operatorIsReduce(_currOperator) || _isShiftNoRotateOp(_currOperator) ||
            _isArithmeticAllowedOp(_currOperator));
    } else if (op1->isLogic() && op2->isLogic()) {
        // sc_logic, sc_logic
        return (
            _isEqualityOp(_currOperator) || hif::operatorIsAssignment(_currOperator) ||
            hif::operatorIsBitwise(_currOperator));
    } else {
        // sc_bit, sc_logic or viceversa
        return (hif::operatorIsBitwise(_currOperator) || _isEqualityOp(_currOperator));
    }
}

bool SystemCAnalysis::_isAllowedOnBitvector_Bit(Bitvector *op1, Bit *op2)
{
    // Concat is allowed for all operands
    if (_currOperator == op_concat)
        return true;

    // Operator rotate is valid only for not logic bit
    if (hif::operatorIsRotate(_currOperator))
        return (!op2->isLogic());

    // No more currOperator avaible for logic bit
    if (op2->isLogic() && !op2->isConstexpr())
        return false;

    // Operation right arithmetic shift is allowed
    // only if is equivalent to right logical shift.
    if (_currOperator == op_sra)
        return (!op1->isSigned());

    if (hif::operatorIsBitwise(_currOperator))
        return !op2->isLogic();

    return (_isEqualityOp(_currOperator) || _isShiftNoRotateOp(_currOperator));
}

bool SystemCAnalysis::_isAllowedOnInt_Bit(Int *op1, Bit *op2)
{
    const bool isBuiltinInt = (op1->getTypeVariant() == Type::NATIVE_TYPE);
    const bool isScBigInt   = (op1->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT);

    if (op2->isLogic()) {
        // the only allowed currOperators for logic are:
        // - bitwise with a signed native integer
        // - eq, neq with a signed native integer
        return (
            isBuiltinInt && op1->isSigned() && (hif::operatorIsBitwise(_currOperator) || _isEqualityOp(_currOperator)));
    } else // not logic bit
    {
        if (_isEqualityOp(_currOperator) || hif::operatorIsAssignment(_currOperator)) {
            // allowed only for int, bigint, biguint
            return ((isBuiltinInt && op1->isSigned()) || isScBigInt);
        } else if (_currOperator == op_sra) {
            // Operation right arithmetic shift is allowed
            // only if is equivalent to right logical shift.
            return (!op1->isSigned());
        } else {
            // all other currOperators are allowed
            // except for concat with a native int.
            return (_currOperator != op_concat && !isBuiltinInt);
        }
    }
}

bool SystemCAnalysis::_isAllowedOnReal_Bit(Real * /*op1*/, Bit * /*op2*/)
{
    return (
        hif::operatorIsLogical(_currOperator) || _isComparisonOp(_currOperator) ||
        _isArithmeticAllowedOp(_currOperator));
}

bool SystemCAnalysis::_isAllowedOnBool_Bit(Bool * /*op1*/, Bit *op2)
{
    if (_currOperator == op_concat)
        return false;
    if (op2->isLogic()) {
        // allowed only bitwise and equality
        return (hif::operatorIsBitwise(_currOperator) || _isEqualityOp(_currOperator));
    }

    // other ops are allowed
    return true;
}

bool SystemCAnalysis::_isAllowedOnBool_Bool(Bool * /*op1*/, Bool * /*op2*/) { return (_currOperator != op_concat); }

bool SystemCAnalysis::_isAllowedOnBitvector_Bool(Bitvector *op1, Bool * /*op2*/)
{
    // Operation right arithmetic shift is allowed
    // only if is equivalent to right logical shift.
    if (_currOperator == op_sra)
        return (!op1->isSigned());
    return (
        hif::operatorIsBitwise(_currOperator) || _isEqualityOp(_currOperator) ||
        hif::operatorIsAssignment(_currOperator) || _currOperator == op_concat ||
        hif::operatorIsShift(_currOperator)); // shift and rotate
}

bool SystemCAnalysis::_isAllowedOnBit_Bool(Bit *op1, Bool * /*op2*/)
{
    if (_currOperator == op_concat)
        return false;

    if ((_currOperator == op_conv || _currOperator == op_bind) && op1->isLogic()) {
        // binding or conversion of boolean to parameter/port of type logic
        // is not valid!
        return false;
    }

    if (op1->isLogic()) {
        // allowed only bitwise, equality as switched operand map
        // plus op_assign (ref. design vhdl/itc99/b10)
        return (hif::operatorIsBitwise(_currOperator) || _isEqualityOp(_currOperator) || _currOperator == op_assign);
    }

    // other ops are allowed
    return true;
}

bool SystemCAnalysis::_isAllowedOnReal_Bool(Real * /*op1*/, Bool * /*op2*/)
{
    return (!_isShiftNoRotateOp(_currOperator) && !hif::operatorIsBitwise(_currOperator) && _currOperator != op_concat);
}

bool SystemCAnalysis::_isAllowedOnInt_Bool(Int *op1, Bool * /*op2*/)
{
    const bool isScBigInt   = op1->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT;
    const bool isBuiltinInt = op1->getTypeVariant() == Type::NATIVE_TYPE;

    // all currOperator is allowed except:
    // - logical not bitwise currOperator between big integer and bool
    // - concat currOperator between native type and bool
    if (hif::operatorIsLogical(_currOperator) && isScBigInt)
        return false;
    if (_currOperator == op_concat && isBuiltinInt)
        return false;

    return true;
}

bool SystemCAnalysis::_isAllowedOnReal_Real(Real * /*op1*/, Real * /*op22*/)
{
    // Special checking for pow currOperator
    if (_currOperator == op_pow)
        return true;
    return (
        !hif::operatorIsBitwise(_currOperator) && !_isShiftNoRotateOp(_currOperator) && _currOperator != op_rem &&
        _currOperator != op_concat && _currOperator != op_mod);
}

bool SystemCAnalysis::_isAllowedOnBool_Real(Bool *op1, Real *op2) { return _isAllowedOnReal_Bool(op2, op1); }

bool SystemCAnalysis::_isAllowedOnBitvector_Real(Bitvector *op1, Real * /*op2*/)
{
    // Operation right arithmetic shift is allowed
    // only if is equivalent to right logical shift.
    if (_currOperator == op_sra)
        return (!op1->isSigned());

    return (_isShiftNoRotateOp(_currOperator) || _currOperator == op_concat);
}

bool SystemCAnalysis::_isAllowedOnInt_Real(Int *op1, Real * /*op2*/)
{
    const bool isScBigInt = op1->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT;

    if (hif::operatorIsBitwise(_currOperator) || _isShiftNoRotateOp(_currOperator) || _currOperator == op_rem ||
        _currOperator == op_mod || _currOperator == op_concat) {
        return false;
    }

    if (_isArithmeticAllowedOp(_currOperator) && _currOperator != op_plus && _currOperator != op_minus &&
        _currOperator != op_div && _currOperator != op_mult) {
        return false;
    }

    // Other operations are allowed for non-bigs
    return !isScBigInt;
}

bool SystemCAnalysis::_isAllowedOnBit_Real(Bit *op1, Real *op2) { return _isAllowedOnReal_Bit(op2, op1); }

bool SystemCAnalysis::_isAllowedOnTime_Real(Time * /*op1*/, Real * /*op2*/)
{
    return (_currOperator == op_mult || _currOperator == op_div);
}

bool SystemCAnalysis::_isAllowedOnString_String(String * /*op1*/, String * /*op2*/)
{
    return (
        hif::operatorIsRelational(_currOperator) || hif::operatorIsAssignment(_currOperator) ||
        _currOperator == op_concat);
}

bool SystemCAnalysis::_isAllowedOnRecord_Record(Record *op1, Record *op2)
{
    if (!hif::operatorIsAssignment(_currOperator) && !_isEqualityOp(_currOperator))
        return false;
    if (op1->fields.size() != op2->fields.size())
        return false;

    return true;
}

bool SystemCAnalysis::_isAllowedOnPointer_Pointer(Pointer * /*op1*/, Pointer * /*op2*/)
{
    return (hif::operatorIsAssignment(_currOperator) || _isEqualityOp(_currOperator));
}

bool SystemCAnalysis::_isAllowedOnString_Pointer(String * /*op1*/, Pointer *op2)
{
    Char *c = dynamic_cast<Char *>(hif::semantics::getBaseType(op2->getType(), false, HIFSemantics::getInstance()));
    return (c != nullptr);
}

bool SystemCAnalysis::_isAllowedOnPointer_String(Pointer *op1, String *op2)
{
    if (hif::operatorIsAssignment(_currOperator))
        return false;

    return _isAllowedOnString_Pointer(op2, op1);
}

bool SystemCAnalysis::_isAllowedOnEnum_Enum(Enum * /*op1*/, Enum * /*op2*/)
{
    return (hif::operatorIsAssignment(_currOperator) || _isEqualityOp(_currOperator));
}

bool SystemCAnalysis::_isAllowedOnViewReference_ViewReference(ViewReference *op1, ViewReference *op2)
{
    if (!hif::operatorIsAssignment(_currOperator) && !_isEqualityOp(_currOperator) && _currOperator != op_concat) {
        return false;
    }

    if (!isSubType(op2, op1, _sem))
        return false;

    return true;
}

bool SystemCAnalysis::_isAllowedOnArray_ViewReference(Array *op1, ViewReference *op2)
{
    return _isAllowedOnViewReference_Array(op2, op1);
}

bool SystemCAnalysis::_isAllowedOnBool_ViewReference(Bool * /*op1*/, ViewReference *op2)
{
    if (!hif::operatorIsAssignment(_currOperator))
        return false;
    if (op2->getDesignUnit() != "sc_clock")
        return false;

    return true;
}

bool SystemCAnalysis::_isAllowedOnTime_Time(Time * /*op1*/, Time * /*op2*/)
{
    if (!hif::operatorIsAssignment(_currOperator) && !_isEqualityOp(_currOperator) && !_isComparisonOp(_currOperator) &&
        _currOperator != op_plus && _currOperator != op_minus) {
        return false;
    }

    return true;
}

bool SystemCAnalysis::_isAllowedOnInt_Time(Int *op1, Time *op2) { return _isAllowedOnTime_Int(op2, op1); }

bool SystemCAnalysis::_isAllowedOnReal_Time(Real * /*op1*/, Time * /*op2*/)
{
    return (_currOperator == op_mult || _currOperator == op_div);
}

bool SystemCAnalysis::_isAllowedOnChar_Char(Char * /*op1*/, Char * /*op2*/)
{
    if (!hif::operatorIsAssignment(_currOperator) && !hif::operatorIsRelational(_currOperator) &&
        !hif::operatorIsAssignment(_currOperator)) {
        return false;
    }

    return true;
}

bool SystemCAnalysis::_isAllowedOnString_Char(String * /*op1*/, Char * /*op2*/)
{
    return hif::operatorIsAssignment(_currOperator);
}

bool SystemCAnalysis::_isAllowedOnFile_File(File *op1, File *op2)
{
    if (!hif::operatorIsAssignment(_currOperator))
        return false;
    return (hif::equals(op1, op2));
}

// <----------------------------------------------
// ///////////////////////////////////////////////////////////////////
// Unary maps implementations
// ///////////////////////////////////////////////////////////////////
void SystemCAnalysis::map(Object *)
{
    // ntd
}

void SystemCAnalysis::map(Bitvector *op1)
{
    if (!hif::operatorIsUnary(_currOperator))
        return;
    if (!_isAllowedUnaryOpOnBitvector(op1))
        return;

    const bool hif_hdtlib = _systemcSem->usingHdtlib();

    if (hif::operatorIsReduce(_currOperator)) {
        // reduction, return type is bool, precision is array.
        if (hif_hdtlib && !op1->isLogic()) {
            // for hdtlib not logic => bool
            _result.returnedType = new Bool();
        } else {
            // logic
            _result.returnedType = _factory.bit(true, op1->isResolved(), op1->isConstexpr());
        }
    } else if (_currOperator == op_bnot) {
        if (hif_hdtlib) {
            // hl_bv or hl_lv
            _result.returnedType =
                _factory.bitvector(hif::copy(op1->getSpan()), op1->isLogic(), op1->isResolved(), false);
        } else {
            // always sc_proxy_lv
            Bitvector *ret = _factory.bitvector(hif::copy(op1->getSpan()), true, op1->isResolved(), false);
            ret->setTypeVariant(Type::SYSTEMC_BITVECTOR_PROXY);
            _result.returnedType = ret;
        }
    } else {
        messageError(
            ("Unexpected unary operator for array: " + hif::operatorToString(_currOperator)).c_str(), nullptr, nullptr);
    }

    // precision is always a copy of input bv
    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(Bit *op1)
{
    if (!hif::operatorIsUnary(_currOperator))
        return;
    if (!_isAllowedUnaryOpOnBit(op1))
        return;

    if (_currOperator == op_bnot) {
        // same type
        _result.returnedType = hif::copy(op1);
    } else if (_currOperator == op_not) {
        // always bool
        _result.returnedType = new Bool();
    } else if (_currOperator == op_plus) {
        // int32 only for sc_bit
        _result.returnedType = _makeSystemCIntFromSize(32, true, false);
    } else {
        messageError("Unexpected operator for bit: " + hif::operatorToString(_currOperator), nullptr, nullptr);
    }

    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(Event *op1)
{
    if (_currOperator != op_deref)
        return;

    _result.returnedType       = new Event();
    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(Int *op1)
{
    if (!hif::operatorIsUnary(_currOperator))
        return;
    if (!_isAllowedUnaryOpOnInt(op1))
        return;

    const bool hif_hdtlib = _systemcSem->usingHdtlib();

    if (_isBuiltinInt(op1) || op1->getTypeVariant() == Type::SYSTEMC_INT_BITFIELD) {
        if (_currOperator == op_not) {
            // returned type is bool
            _result.returnedType       = new Bool();
            _result.operationPrecision = hif::copy(op1);
        } else {
            // special management for small int
            if (_manageSmallBuiltinIntImplicitConversion(op1))
                return;

            // returned type is the same type
            Int *ret                = hif::copy(op1);
            std::uint64_t size = spanGetBitwidth(ret->getSpan(), _sem);
            if (size < 32) {
                delete ret->setSpan(new Range(31, 0));
            }
            _result.returnedType       = ret;
            _result.operationPrecision = hif::copy(_result.returnedType);
        }

        return;
    }

    // systemc types ( sc_int, sc_uint, sc_bigint, sc_biguint )
    if (_currOperator == op_orrd || _currOperator == op_andrd || _currOperator == op_xorrd || _currOperator == op_not) {
        // common return type bool for all types w.r.t. allowed operations.
        _result.returnedType       = new Bool();
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (op1->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT) {
        // result type is always the same type except for operator minus
        // on a sc_biguint that return a sc_bigint
        if (_currOperator == op_minus && !op1->isSigned()) {
            // size is span size + 1
            Range *r             = hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem);
            _result.returnedType = _makeSystemCIntFromRange(r, true, false, Type::SYSTEMC_INT_SC_BIGINT);
        } else {
            // other case return the same type
            _result.returnedType = hif::copy(op1);
        }

        _result.operationPrecision = hif::copy(op1);
        return;
    }

    // sc_int or sc_uint or non constant span:
    if (hif_hdtlib) {
        // hdtlib returns same type
        _result.returnedType = hif::copy(op1);
    } else {
        if (!op1->isSigned() && _currOperator == op_plus) {
            // result type is std::int64_t uint
            _result.returnedType = _makeSystemCIntFromSize(64, false, op1->isConstexpr());
        } else {
            // in all other allowed cases the result type is std::int64_t
            _result.returnedType = _makeSystemCIntFromSize(64, true, op1->isConstexpr());
        }
    }

    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(Real *op1)
{
    if (!hif::operatorIsUnary(_currOperator))
        return;
    if (!_isAllowedUnaryOpOnReal(op1))
        return;

    if (_currOperator == op_not) {
        _result.returnedType = new Bool();
    } else if (_isArithmeticAllowedOp(_currOperator)) {
        _result.returnedType = hif::copy(op1);
    } else {
        messageError("Unexpected operator for real: " + hif::operatorToString(_currOperator), nullptr, nullptr);
    }

    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(Bool *op1)
{
    if (!hif::operatorIsUnary(_currOperator))
        return;
    if (!_isAllowedUnaryOpOnBool(op1))
        return;

    if (_currOperator == op_not) {
        // return type is bool
        _result.returnedType = hif::copy(op1);
    } else if (_currOperator == op_bnot || _currOperator == op_plus) {
        // return type is int32
        _result.returnedType       = _makeSystemCIntFromSize(32, true, op1->isConstexpr());
        _result.operationPrecision = hif::copy(_result.returnedType);
        return;
    } else {
        messageError("Unexpected operator for bool: " + hif::operatorToString(_currOperator), nullptr, nullptr);
    }

    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(TypeReference *op1)
{
    if (!hif::operatorIsUnary(_currOperator))
        return;
    Type *base = getBaseType(op1, false, _sem);

    if (dynamic_cast<TypeReference *>(base) != nullptr) {
        // type TP
        return;
    }

    analyzeOperands(base, nullptr);
    _restoreTypeReference(op1, base);
}

void SystemCAnalysis::map(String *op1)
{
    if (_currOperator == op_size) {
        _result.returnedType       = _factory.integer(nullptr, false);
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (_currOperator == op_reverse) {
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
        return;
    }
}

// ///////////////////////////////////////////////////////////////////
// Binary maps implementation
// ///////////////////////////////////////////////////////////////////

void SystemCAnalysis::map(Object *, Object *)
{
    // ntd
}

void SystemCAnalysis::map(Array *op1, Array *op2)
{
    if (!_isAllowedOnArray_Array(op1, op2))
        return;

    Range *span1 = op1->getSpan();
    Range *span2 = op2->getSpan();
    if (span1 == nullptr || span2 == nullptr)
        return;

    ILanguageSemantics *sem = SystemCSemantics::getInstance();

    analyzeOperands(op1->getType(), op2->getType());
    if (_result.returnedType == nullptr)
        return;
    Type *resultArrayType = _result.operationPrecision;

    _result.operationPrecision = nullptr;
    delete _result.returnedType;
    _result.returnedType = nullptr;

    if (hif::operatorIsAssignment(_currOperator)) {
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
        delete resultArrayType;
        return;
    }

    // build the result span
    Range *resultSpan = rangeGetMax(span1, span2, sem);
    messageAssert(resultSpan != nullptr, "cannot find result span", nullptr, nullptr);

    // build the array with the new range
    Array *arrayResult = new Array();
    arrayResult->setSpan(resultSpan);
    arrayResult->setType(resultArrayType);
    arrayResult->setSigned(op1->isSigned() || op2->isSigned());

    // set the result
    _result.operationPrecision = arrayResult;
    _result.returnedType       = new Bool();
}

void SystemCAnalysis::map(Int *op1, Array *op2)
{
    if (!_isAllowedOnInt_Array(op1, op2))
        return;

    map(op2, op1);
}

void SystemCAnalysis::map(Bitvector *op1, Bitvector *op2)
{
    if (!_isAllowedOnBitvector_Bitvector(op1, op2))
        return;

    Range *resultRange = nullptr;
    if (hif::operatorIsLogical(_currOperator) || hif::operatorIsBitwise(_currOperator) ||
        hif::operatorIsRotate(_currOperator)) {
        // band, bor, bxor: get the maximum between the two ranges
        resultRange = rangeGetMax(op1->getSpan(), op2->getSpan(), _sem);
    } else if (_currOperator == op_concat) {
        // concat: the result range is the sum of the two ranges
        resultRange = rangeGetSum(op1->getSpan(), op2->getSpan(), _sem);
    } else if (hif::operatorIsRelational(_currOperator) || hif::operatorIsAssignment(_currOperator)) {
        // returned type is boolean, precision is the bv with max range
        std::uint64_t size1 = spanGetBitwidth(op1->getSpan(), _sem, true);
        std::uint64_t size2 = spanGetBitwidth(op2->getSpan(), _sem, true);

        // If array2 is unsigned, or assign implies a truncation, padding with 0 is ok
        const bool checkSize       = !hif::operatorIsAssignment(_currOperator) || (op2->isSigned() && size1 > size2);
        Bitvector *resultPrecision = nullptr;
        if (size1 == 0 || size2 == 0) {
            Value *s1 = spanGetSize(op1->getSpan(), _sem);
            Value *s2 = spanGetSize(op2->getSpan(), _sem);

            hif::EqualsOptions eqOpt;
            eqOpt.checkConstexprFlag = false;
            const bool error         = checkSize && !hif::equals(s1, s2, eqOpt);
            delete s1;
            delete s2;
            if (error) {
                // not allowed!
                return;
            }
            resultPrecision = hif::copy(op1);
        } else {
            // precision depends by span size.
            // is span are equals, if there are one lv vs. 1 bv,
            // choose lv rather than bv.
            if (checkSize && (size1 != size2)) {
                // not allowed!
                return;
            }

            resultPrecision = hif::copy(op1);
        }

        if (_currOperator != op_assign)
            _result.returnedType = new Bool();
        else
            _result.returnedType = hif::copy(op1);

        resultRange = rangeGetMax(op1->getSpan(), op2->getSpan(), _sem);
        typeSetSpan(resultPrecision, resultRange, _sem);
        _setLogicFlagByTypes(resultPrecision, op1, op2);
        _setSignedFlagByTypes(resultPrecision, op1, op2);
        _result.operationPrecision = resultPrecision;
        return;
    } else {
        messageError("Unexpected operator for bitvector: " + hif::operatorToString(_currOperator), nullptr, nullptr);
    }

    if (!resultRange)
        return;

    // The result type is always a logic vector.
    Bitvector *resultBitvector = new Bitvector();
    resultBitvector->setSpan(resultRange);
    resultBitvector->setLogic(op1->isLogic() || op2->isLogic()); // previous comment says always true?

    if (_currOperator == op_concat && !_systemcSem->usingHdtlib()) {
        // TODO: check why ternary does not work,
        // Ref design: LPF top module process st_Ct756(line: 950) assign d_var.
        resultBitvector->setTypeVariant(Type::SYSTEMC_BITVECTOR_PROXY);
        resultBitvector->setLogic(true); // always sc_proxy_lv
    } else if (hif::operatorIsBitwise(_currOperator) && !_systemcSem->usingHdtlib()) {
        resultBitvector->setTypeVariant(Type::SYSTEMC_BITVECTOR_BASE);
        resultBitvector->setLogic(true); // always sc_lv_base
    }

    _result.returnedType       = hif::copy(resultBitvector);
    _result.operationPrecision = resultBitvector;
}

void SystemCAnalysis::map(Bit *op1, Bit *op2)
{
    if (!_isAllowedOnBit_Bit(op1, op2))
        return;

    if (hif::operatorIsLogical(_currOperator) || hif::operatorIsRotate(_currOperator)) {
        // same type of first operand (only sc_bit allowed)
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (_isArithmeticAllowedOp(_currOperator) || _isShiftNoRotateOp(_currOperator)) {
        // returned type is integer 32 bit
        _result.returnedType       = _makeSystemCIntFromSize(32, true, op1->isConstexpr() && op2->isConstexpr());
        _result.operationPrecision = hif::copy(_result.returnedType);
        return;
    }

    if (_currOperator == op_concat) {
        Type *ret = nullptr;
        if (_systemcSem->usingHdtlib()) {
            ret = _factory.bitvector(new Range(1, 0));
            _setLogicFlagByTypes(ret, op1, op2);
            _setConstexprFlagByTypes(ret, op1, op2);
            _setResolveFlagByTypes(ret, op1, op2);
        } else {
            // results is sc_proxy_lv or sc_proxy_bv
            ret = _factory.bitvector(new Range(1, 0));
            _setLogicFlagByTypes(ret, op1, op2);
            _setConstexprFlagByTypes(ret, op1, op2);
            _setResolveFlagByTypes(ret, op1, op2);
            ret->setTypeVariant(Type::SYSTEMC_BITVECTOR_PROXY);
        }

        _result.operationPrecision = hif::copy(ret);
        _result.returnedType       = ret;
        return;
    }

    Type *resultType = nullptr;
    if (hif::operatorIsRelational(_currOperator) || hif::operatorIsAssignment(_currOperator)) {
        // returned type is boolean
        resultType = new Bool();
    } else if (hif::operatorIsBitwise(_currOperator)) {
        // returned type is sc_bit only if bits are both sc_bit. The same for resolved
        resultType = new Bit();
        _setLogicFlagByTypes(resultType, op1, op2);
        _setConstexprFlagByTypes(resultType, op1, op2);
        _setResolveFlagByTypes(resultType, op1, op2);
    }

    messageAssert(resultType != nullptr, "Expected result type", _srcObj, _sem);

    // other ramaining operations:
    _result.returnedType = resultType;
    Type *precision      = new Bit();
    _setLogicFlagByTypes(precision, op1, op2);
    _setConstexprFlagByTypes(precision, op1, op2);
    _setResolveFlagByTypes(precision, op1, op2, false); ///@warning: was and condition
    /// @warning in case of when alts type variant may be required.
    _result.operationPrecision = precision;
}

void SystemCAnalysis::map(Bool *op1, Bool *op2)
{
    if (!_isAllowedOnBool_Bool(op1, op2))
        return;

    Type *resultType = nullptr;
    if (_isArithmeticAllowedOp(_currOperator) || _isShiftNoRotateOp(_currOperator) ||
        hif::operatorIsBitwise(_currOperator)) {
        resultType = _makeSystemCIntFromSize(32, true, op1->isConstexpr() && op2->isConstexpr());
    } else if (
        hif::operatorIsRelational(_currOperator) || hif::operatorIsAssignment(_currOperator) ||
        hif::operatorIsLogical(_currOperator)) {
        resultType = new Bool();
    }

    messageAssert(resultType != nullptr, "Expected result type", _srcObj, _sem);

    // result precision is the same of result type in all cases
    _result.returnedType       = hif::copy(resultType);
    _result.operationPrecision = resultType;
}

void SystemCAnalysis::map(Bitvector *op1, Bit *op2)
{
    if (!_isAllowedOnBitvector_Bit(op1, op2))
        return;

    const bool hif_hdtlib = _systemcSem->usingHdtlib();

    if (hif::operatorIsRelational(_currOperator) || hif::operatorIsAssignment(_currOperator)) {
        // Op is == or != => boolean type.
        _result.returnedType       = new Bool();
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (_isShiftNoRotateOp(_currOperator)) {
        // returned type is logical bv with same range
        _result.returnedType = _factory.bitvector(hif::copy(op1->getSpan()));
        typeSetLogic(_result.returnedType, true, _sem);
        _setConstexprFlagByTypes(_result.returnedType, op1, op2);
        _setResolveFlagByTypes(_result.returnedType, op1, op2);
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (hif::operatorIsLogical(_currOperator)) {
        // returned precision is logical bv with same range
        _result.returnedType       = new Bool();
        _result.operationPrecision = _factory.bitvector(hif::copy(op1->getSpan()));
        typeSetLogic(_result.operationPrecision, true, _sem);
        _setConstexprFlagByTypes(_result.operationPrecision, op1, op2);
        _setResolveFlagByTypes(_result.operationPrecision, op1, op2);
        return;
    }

    if (hif::operatorIsBitwise(_currOperator)) {
        if (hif_hdtlib) {
            // returned type is logical array with same range
            _result.returnedType = _factory.bitvector(hif::copy(op1->getSpan()));
            typeSetLogic(_result.returnedType, true, _sem);
            _setConstexprFlagByTypes(_result.returnedType, op1, op2);
            _setResolveFlagByTypes(_result.returnedType, op1, op2);
            _result.operationPrecision = hif::copy(op1);
        } else {
            // returned type is sc_lv_base
            _result.returnedType = _factory.bitvector(hif::copy(op1->getSpan()));
            typeSetLogic(_result.returnedType, true, _sem);
            _setConstexprFlagByTypes(_result.returnedType, op1, op2);
            _setResolveFlagByTypes(_result.returnedType, op1, op2);
            _result.returnedType->setTypeVariant(Type::SYSTEMC_BITVECTOR_BASE);
            _result.operationPrecision = hif::copy(op1);
        }

        return;
    }

    if (_currOperator == op_concat) {
        _result.returnedType = _factory.bitvector(hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem));
        _setLogicFlagByTypes(_result.returnedType, op1, op2);
        _setConstexprFlagByTypes(_result.returnedType, op1, op2);
        _setResolveFlagByTypes(_result.returnedType, op1, op2);
        typeSetSigned(_result.returnedType, op1->isSigned(), _sem);

        if (!hif_hdtlib) {
            // without hdtlib => sc_proxy_lv or sc_proxy_bv
            _result.returnedType->setTypeVariant(Type::SYSTEMC_BITVECTOR_PROXY);
        }

        _result.operationPrecision = hif::copy(_result.returnedType);
        return;
    }

    if (_currOperator == op_sll || _currOperator == op_sla) {
        // returned type is logical array with same range
        Expression *e1 = dynamic_cast<Expression *>(_srcObj);
        if (e1 == nullptr || e1->getValue2() == nullptr) {
            _result.returnedType = _factory.bitvector(hif::copy(op1->getSpan()));
            typeSetLogic(_result.returnedType, true, _sem);
            typeSetConstexpr(_result.returnedType, false);
            typeSetResolved(_result.returnedType, op1->isResolved(), _sem);
            _result.operationPrecision = hif::copy(_result.returnedType);
        } else {
            /// @warning: is the same .. but was so!
            _result.returnedType = _factory.bitvector(hif::copy(op1->getSpan()));
            typeSetLogic(_result.returnedType, true, _sem);
            typeSetConstexpr(_result.returnedType, false);
            typeSetResolved(_result.returnedType, op1->isResolved(), _sem);
            _result.operationPrecision = hif::copy(_result.returnedType);
        }
        return;
    }

    if (hif::operatorIsRotate(_currOperator)) {
        // returned type is the same of the array
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(_result.returnedType);
        return;
    }
}

void SystemCAnalysis::map(Bit *op1, Bitvector *op2)
{
    if (!_isAllowedOnBit_Bitvector(op1, op2))
        return;

    map(op2, op1);
}

void SystemCAnalysis::map(Int *op1, Int *op2)
{
    if (!_isAllowedOnInt_Int(op1, op2))
        return;

    const bool hif_hdtlib = _systemcSem->usingHdtlib();

    Type::TypeVariant tv1 = op1->getTypeVariant();
    Type::TypeVariant tv2 = op2->getTypeVariant();

    const bool isBuiltin1  = _isBuiltinInt(op1);
    const bool isBuiltin2  = _isBuiltinInt(op2);
    const bool isScInt1    = tv1 == Type::SYSTEMC_INT_SC_INT;
    const bool isScInt2    = tv2 == Type::SYSTEMC_INT_SC_INT;
    const bool isScBigInt1 = tv1 == Type::SYSTEMC_INT_SC_BIGINT;
    const bool isScBigInt2 = tv2 == Type::SYSTEMC_INT_SC_BIGINT;

    messageAssert(isBuiltin1 || isScInt1 || isScBigInt1, "Unexpected operand 1", op1, _sem);
    messageAssert(isBuiltin2 || isScInt2 || isScBigInt2, "Unexpected operand 2", op2, _sem);

    std::uint64_t size1 = spanGetBitwidth(op1->getSpan(), _sem);
    std::uint64_t size2 = spanGetBitwidth(op2->getSpan(), _sem);

    // Special managing of concat operation both in case of hdtlib and not-hdtlib.
    if (_currOperator == op_concat) {
        // system c concat operation return a "const concatref"
        // that will be mapped in any systemc type
        // so the result type is calculated locally
        // based on operands type.
        Range *sum = rangeGetSum(op1->getSpan(), op2->getSpan(), _sem);
        if (!hif_hdtlib) {
            // return sc_proxy_bv
            _result.returnedType = _factory.bitvector(
                sum, false, false,
                op1->isSigned() || op2->isSigned(), /// @warning: should not be always unsigned?
                false, Type::SYSTEMC_BITVECTOR_PROXY);
        } else // hif_hdtlib
        {
            // allowed only for non-builtin => result is hl_int
            _result.returnedType =
                _factory.integer(sum, op1->isSigned() || op2->isSigned(), false, Type::SYSTEMC_INT_SC_INT);
        }

        _result.operationPrecision = hif::copy(_result.returnedType);
        return;
    }

    // Special management with hdtlib and at least one non-builtin int.
    // - in case of only one of the types is not builtin => return the not-builtin one.
    // - in case all types are not builtin, allow the operations only when they have
    // the same span size.
    if (hif_hdtlib && (!isBuiltin1 || !isBuiltin2)) {
        if (!isBuiltin1 && !isBuiltin2) {
            Value *s1            = typeGetSpanSize(op1, _sem);
            Value *s2            = typeGetSpanSize(op2, _sem);
            const bool areEquals = hif::equals(s1, s2);
            delete s1;
            delete s2;
            if (!areEquals)
                return;
        }

        // Switch on operation type
        if (hif::operatorIsRelational(_currOperator)) {
            Int *ret = nullptr;
            if (isBuiltin1)
                ret = hif::copy(op2);
            else
                ret = hif::copy(op1);

            _result.returnedType       = new Bool();
            _result.operationPrecision = ret;
        } else if (hif::operatorIsArithmetic(_currOperator)) {
            Int *ret = nullptr;
            if (isBuiltin1)
                ret = hif::copy(op2);
            else
                ret = hif::copy(op1);

            _result.returnedType       = ret;
            _result.operationPrecision = hif::copy(ret);
        } else if (hif::operatorIsBitwise(_currOperator)) {
            Int *ret = nullptr;
            if (isBuiltin1)
                ret = hif::copy(op2);
            else
                ret = hif::copy(op1);

            _result.returnedType       = ret;
            _result.operationPrecision = hif::copy(ret);
        } else if (hif::operatorIsAssignment(_currOperator)) {
            if (isBuiltin1)
                return; // assignment builtin <= hl_int not allowed.

            _result.returnedType       = hif::copy(op1);
            _result.operationPrecision = hif::copy(op1);
        } else {
            messageError("Unsupported operation with hdtlib types", nullptr, nullptr);
        }
        return;
    }

    // Relational, Assignment and Logical operations management
    if (hif::operatorIsRelational(_currOperator) || hif::operatorIsAssignment(_currOperator) ||
        hif::operatorIsLogical(_currOperator)) {
        // Logical NOT bitwise operations.
        // returned type is bool.
        _result.returnedType = new Bool();
        // precision is a copy of integer with maximum span
        Range *max           = rangeGetMax(op1->getSpan(), op2->getSpan(), _sem);
        if (hif::equals(max, op1->getSpan())) {
            _result.operationPrecision = hif::copy(op1);
            delete max;
        } else if (hif::equals(max, op2->getSpan())) {
            _result.operationPrecision = hif::copy(op2);
            delete max;
        } else {
            messageError("Cannot determinate the integer with max range", _srcObj, _sem);
        }
        return;
    }

    // Pow operations management: return type is real 64 bit.
    if (_currOperator == op_pow) {
        _result.returnedType       = _factory.real();
        _result.operationPrecision = _factory.real();
        return;
    }

    // Shift operation management: return type is left.
    if (_isShiftNoRotateOp(_currOperator)) {
        const bool opIsLeftShift = (_currOperator == op_sla || _currOperator == op_sll);
        if (isBuiltin1) {
            // special management for small int
            if (_manageSmallBuiltinIntImplicitConversion(op1, op2))
                return;

            // first operand is builtin:
            // int-int, int-uint, int-sc_int, int-sc_uint => int(size)
            // uint-int, uint-uint, uint-sc_int, uint-sc_uint => uint(size)
            _result.returnedType = hif::copy(op1);
        } else if (isScInt1) {
            // sc_int-int, sc_int-uint, sc_int-sc_int, sc_int-sc_uint => std::int64_t
            // sc_uint-int, sc_uint-uint, sc_uint-sc_int, sc_uint-sc_uint => std::int64_t uint
            _result.returnedType = _makeSystemCIntFromSize(64, op1->isSigned(), op1->isConstexpr());
        } else // isScBigInt1
        {
            if (!opIsLeftShift) {
                _result.returnedType = hif::copy(op1);
            } else {
                // bigint and left shift should return a bigint with range incremented
                // of the value 2 value.
                std::int64_t retval = 0;
                if (_getExpressionValue(_srcObj, retval, true) && retval != 0) {
                    // Op2 value can be established from srcObj!
                    // result range is incremented by value2..
                    Int *retType = hif::copy(op1);
                    delete retType->setSpan(hif::manipulation::rangeGetIncremented(retType->getSpan(), _sem, retval));
                    _result.returnedType = retType;
                } else {
                    // Cannot establish. Approximate to op1.
                    _printSystemCLShiftWarning();
                    _result.returnedType = hif::copy(op1);
                }
            }
        }

        messageAssert(_result.returnedType != nullptr, "Expected returned type", _srcObj, _sem);
        _result.operationPrecision = hif::copy(_result.returnedType);
        return;
    }

    // Common part for others operations
    Range *retTypeSpan               = nullptr;
    bool retTypeSigned               = true;
    Type::TypeVariant retTypeVariant = Type::NATIVE_TYPE;
    std::int64_t spanSizeFinalVariation = 0;

    // Determinate result size and sign
    if (isBuiltin1 && isBuiltin2) {
        // special management for small int
        if (_manageSmallBuiltinIntImplicitConversion(op1, op2))
            return;

        // int-int => int.
        // int-uint, uint-int, uint-uint => uint.
        retTypeVariant = Type::NATIVE_TYPE;
        retTypeSigned  = (op1->isSigned() && op2->isSigned());
        retTypeSpan    = size1 > size2 ? hif::copy(op1->getSpan()) : hif::copy(op2->getSpan());
    } else if (isBuiltin1 && isScInt2) {
        // int-sc_int, int-sc_uint => std::int64_t
        // uint-sc_int, uint-sc_uint => std::int64_t uint
        retTypeVariant = Type::NATIVE_TYPE;
        retTypeSigned  = op2->isSigned();
        retTypeSpan    = _factory.range(63, 0);
    } else if (isScInt1 && isBuiltin2) {
        // sc_int-int, sc_int-uint => std::int64_t
        // sc_uint-int, sc_uint-uint => std::int64_t uint
        retTypeVariant = Type::NATIVE_TYPE;
        retTypeSigned  = op1->isSigned();
        retTypeSpan    = _factory.range(63, 0);
    } else if (isScInt1 && isScInt2) {
        // sc_int-sc_int => std::int64_t
        // sc_int-sc_uint, sc_uint-sc_int, sc_uint-sc_uint => std::int64_t uint
        retTypeVariant = Type::NATIVE_TYPE;
        retTypeSigned  = op1->isSigned() && op2->isSigned();
        retTypeSpan    = _factory.range(63, 0);
    } else if ((isBuiltin1 || isScInt1) && isScBigInt2) {
        if (op2->isSigned()) {
            // int-sc_bigint, uint-sc_bigint, sc_int-sc_bigint, sc_uint-sc_bigint
            // => sc_bigint<sizeof(sc_bigint)>
            retTypeVariant = Type::SYSTEMC_INT_SC_BIGINT;
            retTypeSigned  = true;
            retTypeSpan    = hif::copy(op2->getSpan());
        } else if (op1->isSigned()) // && !op2->isSigned()
        {
            // int-sc_biguint, sc_int-sc_biguint => sc_bigint<sizeof(sc_biguint)+1>
            retTypeVariant = Type::SYSTEMC_INT_SC_BIGINT;
            retTypeSigned  = true;
            retTypeSpan    = hif::manipulation::rangeGetIncremented(op2->getSpan(), _sem, 1);
        } else // !op1->isSigned() && !op2->isSigned()
        {
            // uint-sc_biguint, sc_uint-sc_biguint => sc_biguint<sizeof(sc_biguint)>
            retTypeVariant = Type::SYSTEMC_INT_SC_BIGINT;
            retTypeSigned  = false;
            retTypeSpan    = hif::copy(op2->getSpan());
        }
    } else if (isScBigInt1 && (isBuiltin2 || isScInt2)) {
        if (op1->isSigned()) {
            // sc_bigint-int, sc_bigint-uint, sc_bigint-sc_int, sc_bigint-sc_uint
            // => sc_bigint<sizeof(sc_bigint)>
            retTypeVariant = Type::SYSTEMC_INT_SC_BIGINT;
            retTypeSigned  = true;
            retTypeSpan    = hif::copy(op1->getSpan());
        } else if (op2->isSigned()) // && !op1->isSigned()
        {
            // int-sc_biguint, sc_int-sc_biguint => sc_bigint<sizeof(sc_biguint)+1>
            retTypeVariant = Type::SYSTEMC_INT_SC_BIGINT;
            retTypeSigned  = true;
            retTypeSpan    = hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem, 1);
        } else // !op1->isSigned() && !op2->isSigned()
        {
            // uint-sc_biguint, sc_uint-sc_biguint => sc_biguint<sizeof(sc_biguint)>
            retTypeVariant = Type::SYSTEMC_INT_SC_BIGINT;
            retTypeSigned  = false;
            retTypeSpan    = hif::copy(op1->getSpan());
        }
    } else if (isScBigInt1 && isScBigInt2) {
        // sc_bigint-sc_bigint => sc_bigint<max(size1,size2)>
        // sc_biguint-sc_biguint => sc_biguint<max(size1,size2)>
        // sc_bigint-sc_biguint => sc_bigint<size*>
        // sc_biguint-sc_bigint => sc_bigint<size*>
        //
        // NOTE*: size is the size of the max between the 2 big int:
        // +1 if the max is the signed or -1 if the max is unsigned.
        retTypeVariant = Type::SYSTEMC_INT_SC_BIGINT;
        retTypeSigned  = (op1->isSigned() || op2->isSigned());
        if (_currOperator == op_mult) {
            // for mult between bigs the result span is the sum of the operands spans.
            retTypeSpan = rangeGetSum(op1->getSpan(), op2->getSpan(), _sem);
        } else if ((_currOperator == op_mod || _currOperator == op_rem) && !op1->isSigned() && !op2->isSigned()) {
            // for rem or mod operations in case of both unsigned big the result type
            // span is minimum size of two operands spans.
            retTypeSpan = rangeGetMin(op1->getSpan(), op2->getSpan(), _sem);
        } else {
            retTypeSpan = rangeGetMax(op1->getSpan(), op2->getSpan(), _sem);
        }

        // try to set the correct span in case of mixed operations between
        // signed and unsigned and max span is determinable.
        const bool canFindGreather  = (size1 != 0 && size2 != 0 && size1 != size2);
        const bool isMixedOperation = (op1->isSigned() != !op2->isSigned());
        if (canFindGreather && isMixedOperation) {
            bool greatherIsSigned  = (size1 > size2) ? op1->isSigned() : op2->isSigned();
            spanSizeFinalVariation = greatherIsSigned ? 1 : -1;
        }
    }

    if (retTypeVariant == Type::SYSTEMC_INT_SC_BIGINT) {
        // NOTE: result type variant is big only if at least one operand is big!
        // In this is necessary to check for operation that modifies span size
        // and raise some warnings since some operations may lead to different
        // span size in some peculiar cases.
        const bool bothBig = isScBigInt1 && isScBigInt2;
        if (_currOperator == op_plus) {
            spanSizeFinalVariation += 1;
            _printSystemCLPlusOnBigsWarning();
        } else if (_currOperator == op_mult) {
            if (!bothBig) {
                // for mult between big and not big always add 64 bits.
                spanSizeFinalVariation += 64;
            } else {
                if (!op1->isSigned() || !op2->isSigned()) {
                    // if there is almost ad unsigned operand, add 1 bit.
                    spanSizeFinalVariation += 1;
                }
            }
            _printSystemCLMultOnBigsWarning();
        } else if (_currOperator == op_minus) {
            // for minus operator the result type changes if all operands
            // are big and unsigned:
            // sc_biguint-sc_biguint => sc_bigint<max(size1,size2)+1>
            if (bothBig && !op1->isSigned() && !op2->isSigned()) {
                spanSizeFinalVariation += 1;
            }
            _printSystemCLMinusOnBigsWarning();
        } else if (_currOperator == op_div) {
            _printSystemCLDivOnBigsWarning();
        } else if (_currOperator == op_rem) {
            _printSystemCLRemOnBigsWarning();
        }
    }

    // Apply the possible variation
    if (spanSizeFinalVariation != 0) {
        Range *originalSpan = retTypeSpan;
        retTypeSpan         = hif::manipulation::rangeGetIncremented(retTypeSpan, _sem, spanSizeFinalVariation);
        delete originalSpan;
    }

    // Build the result type
    _result.returnedType =
        _makeSystemCIntFromRange(retTypeSpan, retTypeSigned, op1->isConstexpr() && op2->isConstexpr(), retTypeVariant);
    _result.operationPrecision = hif::copy(_result.returnedType);
}

void SystemCAnalysis::map(Bitvector *op1, Int *op2)
{
    if (!_isAllowedOnBitvector_Int(op1, op2))
        return;

    if (hif::operatorIsAssignment(_currOperator)) {
        // returned type is the same of the array
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(_result.returnedType);
        return;
    }

    if (_isShiftNoRotateOp(_currOperator)) {
        if (_currOperator == op_sll || _currOperator == op_sla) {
            // Left shift returned type is sc_lv_base with size of
            // original bitvector + the value of the left shift!
            Bitvector *retType = nullptr;
            std::int64_t retval   = 0;
            if (_getExpressionValue(_srcObj, retval, true) && retval != 0) {
                // Op2 value can be established from srcObj!
                // result range is incremented by value2..
                retType = hif::copy(op1);
                delete retType->setSpan(hif::manipulation::rangeGetIncremented(retType->getSpan(), _sem, retval));
            } else {
                // Cannot establish. Approximate to op1.
                _printSystemCLShiftWarning();
                retType = hif::copy(op1);
            }

            retType->setTypeVariant(Type::SYSTEMC_BITVECTOR_BASE);
            _result.returnedType = retType;

            // The precision is the op1
            _result.operationPrecision = hif::copy(op1);
        } else // other shift
        {
            // result is always sc_lv_base
            _result.returnedType = hif::copy(op1);
            _result.returnedType->setTypeVariant(Type::SYSTEMC_BITVECTOR_BASE);

            // The precision is the op1
            _result.operationPrecision = hif::copy(op1);
        }
        return;
    }

    if (hif::operatorIsRotate(_currOperator)) {
        // returned type is the same of the array
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(_result.returnedType);
        return;
    }

    // otherwise there result type and precision are the same of swapped operands map.
    map(op2, op1);
}

void SystemCAnalysis::map(Int *op1, Bitvector *op2)
{
    if (!_isAllowedOnInt_Bitvector(op1, op2))
        return;

    const bool hif_hdtlib = _systemcSem->usingHdtlib();
    if (_isEqualityOp(_currOperator) || hif::operatorIsAssignment(_currOperator)) {
        /// @warning This case was was disabled before refectoring..
        /// Don't know why! Operations are possible.. therefore reactivated!
        if (hif::operatorIsAssignment(_currOperator))
            _result.returnedType = hif::copy(op1);
        else
            _result.returnedType = new Bool();
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (hif::operatorIsBitwise(_currOperator)) {
        // returned type is logical bv with range of bv
        _result.returnedType       = _factory.bitvector(hif::copy(op2->getSpan()), true);
        _result.operationPrecision = hif::copy(op2);
        return;
    }

    if (_currOperator == op_concat) {
        // Returned type is sc_proxy_bv/lv with span size of the bitvector + 1.
        // With hdtlib returned type is hl_bv/hl_lv
        Bitvector *retType = hif::copy(op2);
        delete retType->setSpan(hif::manipulation::rangeGetIncremented(op2->getSpan(), _sem, 1));
        if (!hif_hdtlib)
            retType->setTypeVariant(Type::SYSTEMC_BITVECTOR_PROXY);
        _result.returnedType       = retType;
        _result.operationPrecision = hif::copy(retType);
        return;
    }
}

void SystemCAnalysis::map(Int *op1, Bit *op2)
{
    if (!_isAllowedOnInt_Bit(op1, op2))
        return;

    const bool isBuiltinInt = _isBuiltinInt(op1);
    const bool isScInt      = (op1->getTypeVariant() == Type::SYSTEMC_INT_SC_INT);

    // Relational and assignment op management
    if (hif::operatorIsRelational(_currOperator) || hif::operatorIsAssignment(_currOperator)) {
        // For allowed operations returned type is bool
        _result.returnedType       = new Bool();
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    // Bitwise op management
    if (hif::operatorIsBitwise(_currOperator)) {
        if (isBuiltinInt) {
            // special management for small int
            if (_manageSmallBuiltinIntImplicitConversion(op1, op2))
                return;

            // int-sc_bit => sc_bit
            // int-sc_logic => sc_logic
            _result.returnedType       = hif::copy(op2);
            _result.operationPrecision = hif::copy(op1);
        } else {
            // Allowed only bigint and biguint
            Int *retType = nullptr;
            if (op1->isSigned()) {
                // sc_bigint-sc_bit => sc_bigint<sizeof(sc_bigint)>
                retType = hif::copy(op1);
            } else {
                // sc_biguint-sc_bit => sc_bigint<sizeof(sc_biguint)+1>
                retType = hif::copy(op1);
                retType->setSigned(true);
                delete retType->setSpan(hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem, 1));
            }

            messageAssert(retType != nullptr, "Expected returned type", _srcObj, _sem);
            _result.returnedType       = retType;
            _result.operationPrecision = hif::copy(_result.returnedType);
        }
        return;
    }

    // Logical op management
    if (hif::operatorIsLogical(_currOperator)) {
        // returned type is bool
        // precision is the same of int
        _result.returnedType       = new Bool();
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    // Shift op management
    if (_isShiftNoRotateOp(_currOperator)) {
        Int *retType = nullptr;
        if (isBuiltinInt) {
            // special management for small int
            if (_manageSmallBuiltinIntImplicitConversion(op1, op2))
                return;

            // int-bool => type of int (SAME)
            // uint-bool => type of uint (SAME)
            retType = hif::copy(op1);
        } else if (isScInt) {
            // sc_int-bool => std::int64_t
            // sc_uint-bool => std::int64_t uint
            retType = _makeSystemCIntFromSize(64, op1->isSigned(), op1->isConstexpr());
        } else // ScBigInt
        {
            messageAssert(op1->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT, "Expected systemc big int", op1, _sem);

            // sc_bigint-bool => sc_bigint<sizeof(sc_bigint)>  (SAME)
            // sc_biguint-bool => sc_biguint<sizeof(sc_biguint)>  (SAME)
            if (_currOperator == op_sll || _currOperator == op_sla) {
                std::int64_t retval = 0;
                if (_getExpressionValue(_srcObj, retval, true) && retval != 0) {
                    // Op2 value can be established from srcObj!
                    // result range is incremented by value2..
                    retType = hif::copy(op1);
                    delete retType->setSpan(hif::manipulation::rangeGetIncremented(retType->getSpan(), _sem, retval));
                } else {
                    // Cannot establish. Approximate to op1.
                    _printSystemCLShiftWarning();
                    retType = hif::copy(op1);
                }
            } else // other shift
            {
                retType = hif::copy(op1);
            }
        }

        messageAssert(retType != nullptr, "Expected returned type", _srcObj, _sem);
        _result.returnedType       = retType;
        _result.operationPrecision = hif::copy(op1); // precision is always op1
        return;
    }

    // Arithmetical op management
    if (_isArithmeticAllowedOp(_currOperator)) {
        Int *retType = nullptr;
        if (isBuiltinInt) {
            // special management for small int
            if (_manageSmallBuiltinIntImplicitConversion(op1, op2))
                return;

            // int-sc_bit => type of int (SAME)
            // uint-sc_bit => type of uint (SAME)
            retType = hif::copy(op1);
        } else if (isScInt) {
            // sc_int-sc_bit => std::int64_t
            // sc_uint-sc_bit => std::int64_t uint
            retType = _makeSystemCIntFromSize(64, op1->isSigned(), op1->isConstexpr());
        } else // scBigInt
        {
            messageAssert(op1->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT, "Expected systemc big int", op1, _sem);
            retType             = hif::copy(op1);
            std::int64_t increment = 0;

            if (_currOperator == op_mult) {
                // sc_bigint * sc_bit  => sc_bigint<sizeof(sc_bigint)+64>
                // sc_biguint * sc_bit => sc_bigint<sizeof(sc_biguint)+64+1>
                increment = op1->isSigned() ? 64 : (64 + 1);
                _printSystemCLMultOnBigsWarning();
            } else if (_currOperator == op_plus) {
                // sc_bigint-sc_bit => sc_bigint<sizeof(sc_bigint)*>
                // sc_biguint-sc_bit => sc_bigint<sizeof(sc_biguint)+1+*>
                // * +1 if sc_bit==1

                if (!op1->isSigned())
                    increment = 1;
                std::int64_t retval = 0;
                if (_getExpressionValue(_srcObj, retval, true) && retval != 0) {
                    // Op2 value can be established from srcObj!
                    // result range is incremented by value2..
                    messageAssert(retval == 0 || retval == 1, "Unexpected sc_int value conversion", _srcObj, _sem);
                    increment += retval;
                } else {
                    // Cannot establish. Approximate to sc_bit==0.
                }
                _printSystemCLPlusOnBigsWarning();
            } else // arithmetic no mult and no plus operations
            {
                // sc_bigint#sc_bit => sc_bigint<sizeof(sc_bigint)> (SAME)
                // sc_biguint#sc_bit => sc_bigint<sizeof(sc_biguint)+1>
                if (!op1->isSigned())
                    increment = 1;
            }

            if (increment != 0) {
                delete retType->setSpan(hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem, increment));
            }
        }

        messageAssert(retType != nullptr, "Expected returned type", _srcObj, _sem);
        _result.returnedType       = retType;
        _result.operationPrecision = hif::copy(retType);
        return;
    }

    // Concat op management
    if (_currOperator == op_concat) {
        // concat operation return a concatref or hl_int/unit in case of hdtlib
        /// @warning Approximated with sc_proxy_bv/lv and hl_bv/lv?
        Type *retType =
            _factory.bitvector(hif::copy(op1->getSpan()), op2->isLogic(), op2->isResolved(), false, op1->isSigned());
        if (!_systemcSem->usingHdtlib())
            retType->setTypeVariant(Type::SYSTEMC_BITVECTOR_PROXY);
        _result.returnedType       = retType;
        _result.operationPrecision = hif::copy(retType);
        return;
    }
}

void SystemCAnalysis::map(Bit *op1, Int *op2)
{
    if (!_isAllowedOnBit_Int(op1, op2))
        return;

    if (_isShiftNoRotateOp(_currOperator)) {
        // sc_bit-int, sc_bit-uint, sc_bit-sc_int, sc_bit-sc_uint => int32
        _result.returnedType = _makeSystemCIntFromSize(32, true, false);
        _setConstexprFlagByTypes(_result.returnedType, op1, op2);
        _result.operationPrecision = hif::copy(_result.returnedType);
        return;
    }

    map(op2, op1);
}

void SystemCAnalysis::map(Real *op1, Real *op2)
{
    if (!_isAllowedOnReal_Real(op1, op2))
        return;

    // check the ranges
    if (!_checkValidReal(op1) || !_checkValidReal(op2)) {
        messageError("NOT conformal real.", _srcObj, _sem);
    }

    // Special managing of op pow: return type of base
    if (_currOperator == op_pow) {
        _result.returnedType       = _factory.real();
        _result.operationPrecision = _factory.real();
        return;
    }

    // Other allowed operations: result type is array with max range of 2 operands.
    Range *span1 = op1->getSpan();
    Range *span2 = op2->getSpan();
    if (span1 == nullptr || span2 == nullptr)
        return;
    Range *resultSpan = rangeGetMax(span1, span2, _sem);
    if (resultSpan == nullptr)
        return;
    Real *retType = new Real();
    retType->setSpan(resultSpan);

    // returned type
    if (hif::operatorIsRelational(_currOperator) || hif::operatorIsAssignment(_currOperator) ||
        hif::operatorIsLogical(_currOperator) || hif::operatorIsBitwise(_currOperator) ||
        hif::operatorIsRotate(_currOperator)) {
        _result.returnedType = new Bool();
    } else {
        _result.returnedType = hif::copy(retType);
    }

    // operation precision is always max real
    _result.operationPrecision = retType;
}

void SystemCAnalysis::map(Bitvector *op1, Real *op2)
{
    if (!_isAllowedOnBitvector_Real(op1, op2))
        return;

    const bool hif_hdtlib = _systemcSem->usingHdtlib();

    // Shift op management
    if (_isShiftNoRotateOp(_currOperator)) {
        Bitvector *retType       = nullptr;
        const bool opIsLeftShift = (_currOperator == op_sla || _currOperator == op_sll);
        if (opIsLeftShift) {
            // returned type is the op1 with range incremented
            // of the value 2 value.
            std::int64_t retval = 0;
            if (_getExpressionValue(_srcObj, retval, true) && retval != 0) {
                // Op2 value can be established from srcObj!
                // result range is incremented by value2..
                retType = hif::copy(op1);
                delete retType->setSpan(hif::manipulation::rangeGetIncremented(retType->getSpan(), _sem, retval));
            } else {
                // Cannot establish. Approximate to op1.
                _printSystemCLShiftWarning();
                retType = hif::copy(op1);
            }
        } else // other allowed shifts
        {
            retType = hif::copy(op1);
        }

        messageAssert(retType != nullptr, "Expected returned type", _srcObj, _sem);
        _result.returnedType       = hif::copy(retType);
        _result.operationPrecision = hif::copy(op1); // precision is always op1
        return;
    }

    // Concat operation management
    if (_currOperator == op_concat) {
        // Returned type is sc_proxy_bv/lv with span size of the bitvector + 1.
        // With hdtlib returned type is hl_bv/hl_lv
        Bitvector *retType = hif::copy(op1);
        delete retType->setSpan(hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem, 1));
        if (!hif_hdtlib)
            retType->setTypeVariant(Type::SYSTEMC_BITVECTOR_PROXY);
        _result.returnedType       = retType;
        _result.operationPrecision = hif::copy(retType);
        return;
    }
}

void SystemCAnalysis::map(Real *op1, Bitvector *op2)
{
    if (!_isAllowedOnReal_Bitvector(op1, op2))
        return;

    // The same of switched map
    map(op1, op2);
}

void SystemCAnalysis::map(Int *op1, Real *op2)
{
    if (!_isAllowedOnInt_Real(op1, op2))
        return;

    // Special managing of op pow: return type of base
    if (_currOperator == op_pow) {
        _result.returnedType = _factory.real();
        _result.returnedType = _factory.real();
        return;
    }

    if (hif::operatorIsBitwise(_currOperator) || hif::operatorIsRelational(_currOperator) ||
        hif::operatorIsAssignment(_currOperator)) {
        // returned type is bool
        _result.returnedType       = new Bool();
        _result.operationPrecision = hif::copy(op2);
        return;
    }

    if (_isArithmeticAllowedOp(_currOperator)) {
        // returned type is the real type
        _result.returnedType       = hif::copy(op2);
        _result.operationPrecision = hif::copy(op2);
        return;
    }
}

void SystemCAnalysis::map(Real *op1, Int *op2)
{
    if (!_isAllowedOnReal_Int(op1, op2))
        return;

    // The same of switched map
    map(op2, op1);
}

void SystemCAnalysis::map(Bit *op1, Real *op2)
{
    if (!_isAllowedOnBit_Real(op1, op2))
        return;

    // The same of switched map
    map(op2, op1);
}

void SystemCAnalysis::map(Time *op1, Real *op2)
{
    if (!_isAllowedOnTime_Real(op1, op2))
        return;

    _result.returnedType       = hif::copy(op1);
    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(Real *op1, Bit *op2)
{
    if (!_isAllowedOnReal_Bit(op1, op2))
        return;

    if (hif::operatorIsLogical(_currOperator) || _isComparisonOp(_currOperator)) {
        // logical non bitwise, le, ge, lt, gt => result type is bool type
        _result.returnedType       = new Bool();
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (_isArithmeticAllowedOp(_currOperator)) {
        // result type is the same of the real
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
        return;
    }
}

void SystemCAnalysis::map(Bitvector *op1, Bool *op2)
{
    if (!_isAllowedOnBitvector_Bool(op1, op2))
        return;

    if (hif::operatorIsRelational(_currOperator) || hif::operatorIsAssignment(_currOperator)) {
        // result type is bool and precision is the precision of array
        _result.returnedType       = new Bool();
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (hif::operatorIsLogical(_currOperator)) {
        // return type is logic array with same range
        _result.returnedType = hif::copy(op1);
        hif::typeSetLogic(_result.returnedType, true, _sem);
        _result.operationPrecision = hif::copy(op2);
        return;
    }

    if (hif::operatorIsBitwise(_currOperator)) {
        // return type is logic array with same range
        _result.returnedType = hif::copy(op1);
        hif::typeSetLogic(_result.returnedType, true, _sem);
        if (!_systemcSem->usingHdtlib())
            _result.returnedType->setTypeVariant(Type::SYSTEMC_BITVECTOR_BASE);
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (_currOperator == op_sll || _currOperator == op_sla) {
        // return type is logic array with same range
        _result.returnedType = hif::copy(op1);
        hif::typeSetLogic(_result.returnedType, true, _sem);
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (_currOperator == op_srl || _currOperator == op_sra) {
        // returned type is logical array with range = array.range + 1
        Bitvector *retType = hif::copy(op1);
        hif::typeSetLogic(_result.returnedType, true, _sem);
        delete retType->setSpan(hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem, 1));
        _result.returnedType       = retType;
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (_currOperator == op_concat) {
        // returned type is logical array with range = array.range + 1
        // returned type is logical array with range = array.range + 1
        Bitvector *retType = hif::copy(op1);
        hif::typeSetLogic(_result.returnedType, true, _sem);
        delete retType->setSpan(hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem, 1));
        if (!_systemcSem->usingHdtlib())
            retType->setTypeVariant(Type::SYSTEMC_BITVECTOR_PROXY);
        _result.returnedType       = retType;
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (hif::operatorIsRotate(_currOperator)) {
        // returned type is the same of the array
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
        return;
    }
}

void SystemCAnalysis::map(Bool *op1, Bitvector *op2)
{
    if (!_isAllowedOnBool_Bitvector(op1, op2))
        return;

    // The same of switched
    map(op2, op1);
}

void SystemCAnalysis::map(Bool *op1, Bit *op2)
{
    if (!_isAllowedOnBool_Bit(op1, op2))
        return;

    if (hif::operatorIsLogical(_currOperator) || hif::operatorIsRotate(_currOperator)) {
        // returned type is bool
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op2);
        return;
    }

    if (hif::operatorIsBitwise(_currOperator)) {
        // same type of bit
        _result.returnedType       = hif::copy(op2);
        _result.operationPrecision = hif::copy(op2);
        return;
    }

    if (hif::operatorIsRelational(_currOperator) || hif::operatorIsAssignment(_currOperator)) {
        // returned type is bool
        _result.returnedType       = new Bool();
        _result.operationPrecision = new Bool();
        return;
    }

    if (_isShiftNoRotateOp(_currOperator) || _isArithmeticAllowedOp(_currOperator)) {
        // returned type is integer 32
        _result.returnedType       = _makeSystemCIntFromSize(32, true, op1->isConstexpr() && op2->isConstexpr());
        _result.operationPrecision = hif::copy(_result.returnedType);
        return;
    }
}

void SystemCAnalysis::map(Bit *op1, Bool *op2)
{
    if (!_isAllowedOnBit_Bool(op1, op2))
        return;

    if (hif::operatorIsRelational(_currOperator) || hif::operatorIsAssignment(_currOperator)) {
        // returned type is bool
        _result.returnedType       = new Bool();
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    // The same of switched map
    map(op2, op1);
}

void SystemCAnalysis::map(Real *op1, Bool *op2)
{
    if (!_isAllowedOnReal_Bool(op1, op2))
        return;

    if (hif::operatorIsLogical(_currOperator) || hif::operatorIsRelational(_currOperator) ||
        hif::operatorIsAssignment(_currOperator) || hif::operatorIsRotate(_currOperator)) {
        // returned type is bool, precision of real
        _result.returnedType       = new Bool();
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    if (_isArithmeticAllowedOp(_currOperator)) {
        // returned type is the same of the real
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
        return;
    }
}

void SystemCAnalysis::map(Bool *op1, Real *op2)
{
    if (!_isAllowedOnBool_Real(op1, op2))
        return;

    // The same of switched
    map(op2, op1);
}

void SystemCAnalysis::map(Int *op1, Bool *op2)
{
    if (!_isAllowedOnInt_Bool(op1, op2))
        return;

    const bool isBuiltinInt = _isBuiltinInt(op1);
    const bool isScBigInt   = op1->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT;

    // Bitwise op managment
    if (hif::operatorIsBitwise(_currOperator)) {
        Int *retType = nullptr;
        if (isBuiltinInt) {
            // special management for small int
            if (_manageSmallBuiltinIntImplicitConversion(op1, op2))
                return;

            // int, uint => same type
            retType = hif::copy(op1);
        } else if (isScBigInt) {
            // sc_bigint => same type
            // sc_bigint => same type +1span
            retType = hif::copy(op1);
            if (!op1->isSigned()) {
                delete retType->setSpan(hif::manipulation::rangeGetIncremented(retType->getSpan(), _sem, 1));
            }
        } else // sc_int
        {
            // sc_int => std::int64_t
            // sc_uint => std::int64_t uint
            retType = _makeSystemCIntFromSize(32, op1->isSigned(), op1->isConstexpr() && op2->isConstexpr());
        }

        messageAssert(retType != nullptr, "Expected returned type", _srcObj, _sem);
        _result.returnedType       = retType;
        _result.operationPrecision = hif::copy(_result.returnedType);
        return;
    }

    // Logical, Relational, Assignment adn rotate ops managment
    if (hif::operatorIsLogical(_currOperator) || hif::operatorIsRelational(_currOperator) ||
        hif::operatorIsAssignment(_currOperator) || hif::operatorIsRotate(_currOperator)) {
        // returned type is bool
        _result.returnedType       = new Bool();
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    // Shift op managment
    if (_isShiftNoRotateOp(_currOperator)) {
        Int *retType = nullptr;
        if (isBuiltinInt) {
            // special management for small int
            if (_manageSmallBuiltinIntImplicitConversion(op1, op2))
                return;

            // int-bool => type of int (SAME)
            // uint-bool => type of uint (SAME)
            retType = hif::copy(op1);
        } else if (!isScBigInt) {
            messageAssert(op1->getTypeVariant() == Type::SYSTEMC_INT_SC_INT, "Expected systemc int", op1, _sem);
            // sc_int-bool => std::int64_t
            // sc_uint-bool => std::int64_t uint
            retType = _makeSystemCIntFromSize(32, op1->isSigned(), op1->isConstexpr() && op2->isConstexpr());
        } else // isscBigInt
        {
            messageAssert(op1->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT, "Expected systemc big int", op1, _sem);
            // sc_bigint-bool => sc_bigint<sizeof(sc_bigint)>
            // sc_biguint-bool => sc_biguint<sizeof(sc_biguint)>
            if (_currOperator == op_sll || _currOperator == op_sla) {
                std::int64_t retval = 0;
                if (_getExpressionValue(_srcObj, retval, true) && retval != 0) {
                    // Op2 value can be established from srcObj!
                    // result range is incremented by value2..
                    retType = hif::copy(op1);
                    delete retType->setSpan(hif::manipulation::rangeGetIncremented(retType->getSpan(), _sem, retval));
                } else {
                    // Cannot establish. Approximate to op1.
                    _printSystemCLShiftWarning();
                    retType = hif::copy(op1);
                }
            } else // other shift
            {
                retType = hif::copy(op1);
            }
        }

        messageAssert(retType != nullptr, "Expected returned type", _srcObj, _sem);
        _result.returnedType       = retType;
        _result.operationPrecision = hif::copy(op1); // precision is always op1
        return;
    }

    // Arithmetic op managment
    if (_isArithmeticAllowedOp(_currOperator)) {
        Int *retType = nullptr;
        if (isBuiltinInt) {
            // special management for small int
            if (_manageSmallBuiltinIntImplicitConversion(op1, op2))
                return;

            // int-bool => type of int (SAME)
            // uint-bool => type of uint (SAME)
            retType = hif::copy(op1);
        } else if (!isScBigInt) // sc_int
        {
            messageAssert(op1->getTypeVariant() == Type::SYSTEMC_INT_SC_INT, "Expected systemc int", op1, _sem);
            // sc_int-bool => std::int64_t
            // sc_uint-bool => std::int64_t uint
            retType = _makeSystemCIntFromSize(32, op1->isSigned(), op1->isConstexpr() && op2->isConstexpr());
        } else // big int
        {
            messageAssert(op1->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT, "Expected systemc big int", op1, _sem);

            retType             = hif::copy(op1);
            std::int64_t increment = 0;
            if (_currOperator == op_mult) {
                // sc_bigint*bool => sc_bigint<sizeof(sc_bigint)+64>
                // sc_biguint-bool => sc_bigint<sizeof(sc_biguint)+64+1>
                increment = op1->isSigned() ? 64 : (64 + 1);
                _printSystemCLMultOnBigsWarning();
            } else if (_currOperator == op_plus) {
                // sc_bigint-sc_bit => sc_bigint<sizeof(sc_bigint)*>
                // sc_biguint-sc_bit => sc_bigint<sizeof(sc_biguint)+1*>
                if (!op1->isSigned())
                    increment = 1;
                std::int64_t retval = 0;
                if (_getExpressionValue(_srcObj, retval, true) && retval != 0) {
                    // Op2 value can be established from srcObj!
                    // result range is incremented by value2..
                    messageAssert(retval == 0 || retval == 1, "Unexpected sc_int value conversion", _srcObj, _sem);
                    increment += retval;
                } else {
                    // Cannot establish. Approximate to sc_bit==0.
                }
                _printSystemCLPlusOnBigsWarning();
            } else // arithmetic no mult and no plus operations
            {
                // sc_bigint#sc_bit => sc_bigint<sizeof(sc_bigint)> (SAME)
                // sc_biguint#sc_bit => sc_bigint<sizeof(sc_biguint)+1>
                if (!op1->isSigned())
                    increment = 1;
            }

            if (increment != 0) {
                delete retType->setSpan(hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem, increment));
            }
        }

        messageAssert(retType != nullptr, "Expected returned type", _srcObj, _sem);
        _result.returnedType       = retType;
        _result.operationPrecision = hif::copy(retType);
        return;
    }

    // Concat op management
    if (_currOperator == op_concat) {
        // concat operation return a concatref or hl_int/unit in case of hdtlib
        /// @warning Approximated with sc_proxy_bv and hl_bv?
        Type *retType = _factory.bitvector(hif::copy(op1->getSpan()), false, false, false, op1->isSigned());
        if (!_systemcSem->usingHdtlib())
            retType->setTypeVariant(Type::SYSTEMC_BITVECTOR_PROXY);
        _result.returnedType       = retType;
        _result.operationPrecision = hif::copy(retType);
        return;
    }
}

void SystemCAnalysis::map(Bool *op1, Int *op2)
{
    if (!_isAllowedOnBool_Int(op1, op2))
        return;

    if (_isShiftNoRotateOp(_currOperator)) {
        // bool-int, bool-uint, bool-sc_int, bool-sc_uint => int32
        _result.returnedType = _makeSystemCIntFromSize(32, true, false);
        _setConstexprFlagByTypes(_result.returnedType, op1, op2);
        _result.operationPrecision = hif::copy(_result.returnedType);
    } else {
        map(op2, op1);
    }
}

void SystemCAnalysis::map(String *op1, String *op2)
{
    if (!_isAllowedOnString_String(op1, op2))
        return;

    if (hif::operatorIsRelational(_currOperator)) {
        _result.returnedType       = new Bool();
        _result.operationPrecision = new String();
        return;
    }

    if (hif::operatorIsAssignment(_currOperator)) {
        _result.returnedType       = new String();
        _result.operationPrecision = new String();
        return;
    }

    if (_currOperator == op_concat) {
        _result.returnedType       = new String();
        _result.operationPrecision = new String();
        return;
    }
}

void SystemCAnalysis::map(Pointer *op1, String *op2)
{
    if (!_isAllowedOnPointer_String(op1, op2))
        return;

    map(op2, op1);
}

void SystemCAnalysis::map(TypeReference *op1, TypeReference *op2) { _map(op1, op2, false); }

void SystemCAnalysis::map(TypeReference *op1, Type *op2) { _map(op1, op2); }

void SystemCAnalysis::map(ViewReference * /*op1*/, Type *op2)
{
    Expression *e = dynamic_cast<Expression *>(_srcObj);
    if (e == nullptr)
        return;
    const std::string &op = hif::operatorToPlainString(_currOperator, "__systemc_");
    FunctionCall fc;
    fc.setName(op);
    fc.setInstance(hif::copy(e->getValue1()));
    if (e->getValue2() != nullptr) {
        fc.parameterAssigns.push_back(_factory.parameterArgument("param1", hif::copy(e->getValue2())));
    }

    DeclarationOptions dopt;
    dopt.location                       = _srcObj;
    FunctionCall::DeclarationType *decl = getDeclaration(&fc, _sem, dopt);
    if (decl == nullptr)
        return;
    FunctionCall::DeclarationType *inst = hif::manipulation::instantiate(&fc, _sem);
    if (inst == nullptr)
        return;

    _result.operationPrecision = hif::copy(op2); // warning: wtf?
    _result.returnedType       = hif::copy(inst->getType());
}

void SystemCAnalysis::map(Type *op1, TypeReference *op2) { _map(op1, op2); }

void SystemCAnalysis::map(Record *op1, Record *op2)
{
    if (!_isAllowedOnRecord_Record(op1, op2))
        return;

    if (op1->fields.size() != op2->fields.size())
        return;

    BList<Field>::iterator i = op1->fields.begin();
    BList<Field>::iterator j = op2->fields.begin();
    BList<Field> types;

    const hif::Operator restore = _currOperator;
    if (_currOperator == op_assign)
        _currOperator = op_conv;

    for (; i != op1->fields.end(); ++i, ++j) {
        Field *iField = *i;
        Field *jField = *i;
        if (iField->getName() != jField->getName())
            return;
        analyzeOperands(iField->getType(), jField->getType());
        if (_result.returnedType == nullptr)
            return;
        delete _result.returnedType;
        _result.returnedType = nullptr;
        Field *f             = new Field();
        f->setName(iField->getName());
        f->setType(_result.operationPrecision);
        types.push_back(f);
        _result.operationPrecision = nullptr;
    }

    _currOperator = restore;

    Record *ret = new Record();
    ret->fields.merge(types);

    if (hif::operatorIsAssignment(_currOperator)) {
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = ret;
    } else {
        _result.returnedType       = new Bool();
        _result.operationPrecision = ret;
    }
}

void SystemCAnalysis::map(Array *op1, Pointer *op2)
{
    Type *t1 = op1->getType();
    Type *t2 = op2->getType();
    if (_currOperator != hif::op_conv)
        return;
    if (!hif::equals(t1, t2))
        return;
    _result.returnedType       = hif::copy(t1);
    _result.operationPrecision = hif::copy(t1);
}

void SystemCAnalysis::map(Reference *op1, Reference *op2)
{
    Type *t1 = op1->getType();
    Type *t2 = op2->getType();
    analyzeOperands(t1, t2);
}

void SystemCAnalysis::map(Reference *op1, Type *op2)
{
    Type *t1 = op1->getType();
    analyzeOperands(t1, op2);
}

void SystemCAnalysis::map(Reference *op1, TypeReference *op2)
{
    Type *t1 = op1->getType();
    analyzeOperands(t1, op2);
}

void SystemCAnalysis::map(ViewReference *op1, TypeReference *op2)
{
    Type *t1 = op1;
    Type *t2 = getBaseType(op2, false, _sem);

    if (t2 == nullptr)
        return;
    TypeReference *tt2 = dynamic_cast<TypeReference *>(t2);
    if (tt2 != nullptr)
        t2 = t1;

    analyzeOperands(t1, t2);
    _restoreTypeReference(op2, t2);
}

void SystemCAnalysis::map(Type *op1, Reference *op2)
{
    Type *t2 = op2->getType();
    analyzeOperands(op1, t2);
}

void SystemCAnalysis::map(TypeReference *op1, Reference *op2)
{
    Type *t2 = op2->getType();
    analyzeOperands(op1, t2);
}

void SystemCAnalysis::map(ViewReference *op1, Reference *op2)
{
    Type *t2 = op2->getType();
    analyzeOperands(op1, t2);
}

void SystemCAnalysis::map(Pointer *op1, Pointer *op2)
{
    // only assignment and check for equality/inequality are allowed at the moment
    if (!_isAllowedOnPointer_Pointer(op1, op2))
        return;

    Type *t1 = getBaseType(op1->getType(), false, _sem);
    Type *t2 = getBaseType(op2->getType(), false, _sem);

    ReferencedType *r1 = dynamic_cast<ReferencedType *>(t1);
    ReferencedType *r2 = dynamic_cast<ReferencedType *>(t2);

    if (r1 == nullptr && r2 != nullptr)
        return;
    if (r1 != nullptr && r2 == nullptr)
        return;
    if (r1 != nullptr) {
        Declaration *d1 = getDeclaration(r1, _sem);
        Declaration *d2 = getDeclaration(r2, _sem);
        if (d1 != d2)
            return;
    } else {
        hif::EqualsOptions opt;
        opt.checkConstexprFlag = false;
        if (!hif::equals(t1, t2, opt))
            return;
    }

    if (_isEqualityOp(_currOperator)) {
        _result.returnedType = new Bool();
    } else // assignments
    {
        _result.returnedType = hif::copy(op1);
    }
    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(String *op1, Pointer *op2)
{
    if (!_isAllowedOnString_Pointer(op1, op2))
        return;

    if (_currOperator == op_concat || hif::operatorIsAssignment(_currOperator)) {
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
    } else if (hif::operatorIsRelational(_currOperator)) {
        _result.returnedType       = new Bool();
        _result.operationPrecision = hif::copy(op1);
    }
}

void SystemCAnalysis::map(Int *op1, Pointer *op2) { map(op2, op1); }

void SystemCAnalysis::map(Pointer *op1, Array *op2)
{
    if (!_isAllowedOnPointer_Array(op1, op2))
        return;

    if (hif::equals(op1->getType(), op2->getType())) {
        _result.returnedType       = hif::copy(op1->getType());
        _result.operationPrecision = hif::copy(op1->getType());
    }
}

void SystemCAnalysis::map(ViewReference *op1, Array *op2)
{
    if (!_isAllowedOnViewReference_Array(op1, op2))
        return;

    ViewReference *arrT = dynamic_cast<ViewReference *>(op2->getType());
    if (arrT == nullptr)
        return;

    // at the moment allow only same type concat.
    if (!hif::equals(op1, arrT))
        return;

    // NOTE: works only with disciplines
    ViewReference::DeclarationType *d1 = getDeclaration(op1, _sem);
    ViewReference::DeclarationType *d2 = getDeclaration(arrT, _sem);
    if (d1 == nullptr || d2 == nullptr || d1 != d2)
        return;

    LibraryDef *ld1 = hif::getNearestParent<LibraryDef>(d1);
    if (ld1 == nullptr)
        return;
    if (ld1->getName() != "hif_verilog_vams_disciplines")
        return;

    Array *resArray = hif::copy(op2);
    Range *r        = hif::manipulation::rangeGetIncremented(resArray->getSpan(), _sem);
    delete resArray->setSpan(r);
    _result.returnedType       = resArray;
    _result.operationPrecision = hif::copy(resArray);
}

void SystemCAnalysis::map(Enum *op1, Enum *op2)
{
    if (!_isAllowedOnEnum_Enum(op1, op2))
        return;

    if (_isEqualityOp(_currOperator)) {
        _result.returnedType = new Bool();
    } else // assignments
    {
        _result.returnedType = hif::copy(op1);
    }

    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(ViewReference *op1, ViewReference *op2)
{
    if (!_isAllowedOnViewReference_ViewReference(op1, op2))
        return;

    // Concat management
    if (_currOperator == op_concat) {
        // at the moment allow only same type concat.
        if (!hif::equals(op1, op2))
            return;

        // NOTE: works only with disciplines
        ViewReference::DeclarationType *d1 = getDeclaration(op1, _sem);
        ViewReference::DeclarationType *d2 = getDeclaration(op2, _sem);
        if (d1 == nullptr || d2 == nullptr || d1 != d2)
            return;

        LibraryDef *ld1 = hif::getNearestParent<LibraryDef>(d1);
        if (ld1 == nullptr)
            return;
        if (ld1->getName() != "hif_verilog_vams_disciplines")
            return;

        Array *resArray = new Array();
        resArray->setSpan(new Range(1, 0));
        resArray->setType(hif::copy(op1));
        resArray->setSigned(false);
        _result.returnedType       = resArray;
        _result.operationPrecision = hif::copy(op1);

        return;
    }

    if (hif::operatorIsRelational(_currOperator)) {
        _result.returnedType = new Bool();
    } else {
        _result.returnedType = hif::copy(op1);
    }

    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(Array *op1, ViewReference *op2)
{
    if (!_isAllowedOnArray_ViewReference(op1, op2))
        return;

    // Same of the switched
    map(op2, op1);
}

void SystemCAnalysis::map(Bool *op1, ViewReference *op2)
{
    if (!_isAllowedOnBool_ViewReference(op1, op2))
        return;

    // Special management for sc_clock used in binding.
    // cast is not needed.
    _result.returnedType       = hif::copy(op1);
    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(Time *op1, Time *op2)
{
    if (!_isAllowedOnTime_Time(op1, op2))
        return;

    if (hif::operatorIsRelational(_currOperator)) {
        _result.returnedType = new Bool();
    } else {
        _result.returnedType = new Time();
    }

    _result.operationPrecision = new Time();
}

void SystemCAnalysis::map(Time *op1, Int *op2)
{
    if (!_isAllowedOnTime_Int(op1, op2))
        return;

    const bool hif_hdtlib   = _systemcSem->usingHdtlib();
    const bool isBuiltinInt = _isBuiltinInt(op2);

    if (hif_hdtlib && !isBuiltinInt)
        return;

    _result.returnedType       = new Time();
    _result.operationPrecision = new Time();
}

void SystemCAnalysis::map(Array *op1, Int *op2)
{
    if (!_isAllowedOnArray_Int(op1, op2))
        return;

    Pointer *p = new Pointer();
    p->setType(hif::copy(op1->getType()));

    _result.returnedType       = p;
    _result.operationPrecision = hif::copy(op2);
}

void SystemCAnalysis::map(Pointer *op1, Int * /*op2*/)
{
    if (_currOperator != hif::op_plus && _currOperator != hif::op_minus)
        return;

    _result.returnedType       = hif::copy(op1);
    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(Int *op1, Time *op2)
{
    if (!_isAllowedOnInt_Time(op1, op2))
        return;

    // Same of the switched
    map(op2, op1);
}

void SystemCAnalysis::map(Real *op1, Time *op2)
{
    if (!_isAllowedOnReal_Time(op1, op2))
        return;

    _result.returnedType       = hif::copy(op2);
    _result.operationPrecision = hif::copy(op2);
}

void SystemCAnalysis::map(Char *op1, Char *op2)
{
    if (!_isAllowedOnChar_Char(op1, op2))
        return;

    // at the moment only assign and relational
    if (hif::operatorIsAssignment(_currOperator)) {
        _result.returnedType = hif::copy(op1);
    } else if (hif::operatorIsRelational(_currOperator)) {
        _result.returnedType = new Bool();
    }

    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(String *op1, Char *op2)
{
    if (!_isAllowedOnString_Char(op1, op2))
        return;

    _result.returnedType       = hif::copy(op1);
    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::map(File *op1, File *op2)
{
    if (!_isAllowedOnFile_File(op1, op2))
        return;

    _result.returnedType       = hif::copy(op1);
    _result.operationPrecision = hif::copy(op1);
}

void SystemCAnalysis::_callMap(Object *op1) { callMap(op1); }

void SystemCAnalysis::_callMap(Object *op1, Object *op2) { callMap(op1, op2); }

// ///////////////////////////////////////////////////////////////////
// SystemCCastAnalysis
// ///////////////////////////////////////////////////////////////////
SystemCCastAnalysis::SystemCCastAnalysis(Value *valueToCast, Type *srcType)
    : _sem(SystemCSemantics::getInstance())
    , _inputVal(valueToCast)
    , _sourceType(srcType)
    , _result(nullptr)
    , _neededIncludes()
    , _factory(_sem)
{
    // ntd
}

SystemCCastAnalysis::~SystemCCastAnalysis()
{
    // ntd
}

Value *SystemCCastAnalysis::explicitCast(Type *castType)
{
    // if at least one parameter is null, return null
    if (_inputVal == nullptr || getSemanticType(_inputVal, _sem) == nullptr || castType == nullptr) {
        return nullptr;
    }

    Type *valueType = getSemanticType(_inputVal, _sem);
    Type *baseVt    = getBaseType(valueType, false, _sem);
    Type *baseCt    = getBaseType(castType, false, _sem);

    // if type to cast is the same of value, return value
    if (hif::equals(baseVt, baseCt)) {
        Value *ret = hif::copy(_inputVal);
        return ret;
    }

    callMap(baseVt, baseCt);

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

// ///////////////////////////////////////////////////////////////////
// Utility functions
// ///////////////////////////////////////////////////////////////////
void SystemCCastAnalysis::generateIncludes(Value *startingObj)
{
    System *sys = hif::getNearestParent<System>(startingObj);
    messageAssert(sys != nullptr, "Cannot find system", startingObj, _sem);

    for (std::set<std::string>::iterator i(_neededIncludes.begin()); i != _neededIncludes.end(); ++i) {
        std::string libraryName = *i;

        Scope *s = hif::getNearestScope(startingObj, false, true, false);
        if (dynamic_cast<Contents *>(s) != nullptr)
            s = dynamic_cast<Scope *>(s->getParent());
        hif::backends::addHifLibrary(std::string(libraryName), s, sys, _sem, false);
    }
}

Value *SystemCCastAnalysis::_getCastFromValueToType(Value *from, Type *to)
{
    Cast *ret = new Cast();
    ret->setValue(hif::copy(from));
    ret->setType(hif::copy(to));

    return ret;
}

std::string SystemCCastAnalysis::_getToIntFCallName(Int *i)
{
    std::string fname = (i->isSigned()) ? "to_int" : "to_uint";
    ;
    std::uint64_t size = spanGetBitwidth(i->getSpan(), _sem, true);
    if (size > 32 || size == 0)
        fname += "64";
    return fname;
}

// ///////////////////////////////////////////////////////////////////
// Maps implementation
// ///////////////////////////////////////////////////////////////////
#ifdef NDEBUG
void SystemCCastAnalysis::map(Object * /*s*/, Object *d)
#else
void SystemCCastAnalysis::map(Object *s, Object *d)
#endif
{
    messageDebug("Source type:", s, _sem);
    messageError("Cast not managed", d, _sem);
}

void SystemCCastAnalysis::map(Int *vi, Int *i)
{
    const bool isBuiltinInt1 = _isBuiltinInt(vi) || vi->getTypeVariant() == Type::SYSTEMC_INT_BITFIELD;
    messageAssert(
        isBuiltinInt1 || vi->getTypeVariant() == Type::SYSTEMC_INT_SC_INT ||
            vi->getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT,
        "Unexpected int", vi, _sem);

    const bool isBuiltinInt2 = _isBuiltinInt(i);
    if (isBuiltinInt1 || !isBuiltinInt2) {
        // NOTE: also for bitfield
        _result = _getCastFromValueToType(_inputVal, i);
    } else {
        // build to_int function basing on cast int dimension.
        std::string toInt(_getToIntFCallName(i));
        _neededIncludes.insert("sc_core");
        _result = _makeMemberFunctionCall(toInt, _inputVal);
    }
}

void SystemCCastAnalysis::map(Bitvector *vi, Int *i)
{
    if (vi->getTypeVariant() == Type::SYSTEMC_BITVECTOR_PROXY) {
        // remove variant and recall
        Bitvector *native = hif::copy(vi);
        native->setTypeVariant(Type::NATIVE_TYPE);
        Value *restore = _inputVal;
        _inputVal      = _getCastFromValueToType(_inputVal, native);
        callMap(native, i);
        delete _inputVal;
        _inputVal = restore;
        return;
    }

    const bool isBuiltinInt = _isBuiltinInt(i) || i->getTypeVariant() == Type::SYSTEMC_INT_BITFIELD;
    if (isBuiltinInt) {
        if (i->isSigned()) {
            Int *unsignedInt = hif::copy(i);
            unsignedInt->setSigned(false);
            callMap(vi, unsignedInt);
            _result = _getCastFromValueToType(_result, i);
        } else {
            std::string functionName = _getToIntFCallName(i);
            _result                  = _makeMemberFunctionCall(functionName, _inputVal);
            _neededIncludes.insert("sc_core");
        }
    } else {
        _result = _getCastFromValueToType(_inputVal, i);
    }
}

void SystemCCastAnalysis::map(Bit *vi, Int *i)
{
    if (vi->getTypeVariant() == Type::SYSTEMC_BIT_BITREF) {
        // remove variant and recall
        Bit *native = hif::copy(vi);
        native->setTypeVariant(Type::NATIVE_TYPE);
        Value *restore = _inputVal;
        _inputVal      = _getCastFromValueToType(_inputVal, native);
        callMap(native, i);
        delete _inputVal;
        _inputVal = restore;
        return;
    }

    Value *toCast = nullptr;
    if (vi->isLogic()) {
        toCast = _factory.expression(
            hif::copy(_inputVal), op_case_eq, _factory.bitval(bit_one, _factory.bit(true, vi->isResolved())));
    } else {
        // need to call .to_bool()
        toCast = _makeMemberFunctionCall("to_bool", _inputVal);
        _neededIncludes.insert("sc_core");
    }

    _result = _getCastFromValueToType(toCast, i);
    delete toCast;
}

void SystemCCastAnalysis::map(Bool * /*vi*/, Int *i) { _result = _getCastFromValueToType(_inputVal, i); }

void SystemCCastAnalysis::map(Real * /*vi*/, Int *i) { _result = _getCastFromValueToType(_inputVal, i); }

void SystemCCastAnalysis::map(Int * /*i*/, Bitvector *bv) { _result = _getCastFromValueToType(_inputVal, bv); }

void SystemCCastAnalysis::map(Array * /*vi*/, Array * /*array*/) { _result = hif::copy(_inputVal); }

void SystemCCastAnalysis::map(Bitvector *vi, Array *array)
{
    Type *base = getBaseType(array->getType(), false, _sem);
    if (dynamic_cast<Bit *>(base) == nullptr) {
        // only cast to array of bit
        return;
    }

    if (vi->isSigned() && array->isSigned()) {
        // If source range is greater/equal, no need to extend.
        // Otherwise, call the apposite function.
        Range *max       = rangeGetMax(vi->getSpan(), array->getSpan(), _sem);
        Value *spanVi    = spanGetSize(vi->getSpan(), _sem);
        Value *spanArray = spanGetSize(array->getSpan(), _sem);
        Value *spanMax   = spanGetSize(max, _sem);
        if (hif::equals(spanMax, spanVi) && !hif::equals(spanMax, spanArray)) {
            _result = _getCastFromValueToType(_inputVal, array);
        } else if (vi->isSigned() && hif::equals(spanArray, spanVi)) {
            _result = _getCastFromValueToType(_inputVal, array);
        }
        delete max;
        delete spanArray;
        delete spanVi;
        delete spanMax;
    } else {
        _result = _getCastFromValueToType(_inputVal, array);
    }
}

void SystemCCastAnalysis::map(Bitvector *vi, Bitvector *bv)
{
    // If source range is greater/equal, no need to extend.
    // Otherwise, call the appropriate function.
    Range *max       = rangeGetMax(vi->getSpan(), bv->getSpan(), _sem);
    Value *spanVi    = spanGetSize(vi->getSpan(), _sem);
    Value *spanArray = spanGetSize(bv->getSpan(), _sem);
    Value *spanMax   = spanGetSize(max, _sem);
    if (hif::equals(spanMax, spanVi) && !hif::equals(spanMax, spanArray)) {
        // truncating
        _result = _getCastFromValueToType(_inputVal, bv);
    } else if (hif::equals(spanArray, spanVi)) {
        // May be different bit vector type (e.g. bv vs lv)
        _result = _getCastFromValueToType(_inputVal, bv);
    } else if (typeIsSigned(_sourceType, _sem)) {
        // Sign extension
        Value *param1    = hif::copy(_inputVal);
        Bitvector *clean = hif::copy(vi);
        clean->setTypeVariant(Type::NATIVE_TYPE);
        if (_sem->getExprType(clean, vi, op_conv, _inputVal).returnedType == nullptr) {
            Cast *c = new Cast();
            c->setValue(param1);
            c->setType(clean);
            param1 = c;
        } else {
            delete clean;
        }
        FunctionCall *call = _factory.functionCall(
            "hif_sxt", _factory.libraryInstance("hif_systemc_extensions", false, true),
            (_factory.templateValueArgument("param2", hif::copy(spanArray)) // size
             ),
            (_factory.parameterArgument("param1", param1) // arg
             ));

        // add hif_systemc_extensions library
        _neededIncludes.insert("hif_systemc_extensions");
        _result = call;
    } else {
        // Extension of unsigned
        _result = _getCastFromValueToType(_inputVal, bv);
    }
    delete max;
    delete spanArray;
    delete spanVi;
    delete spanMax;
}

void SystemCCastAnalysis::map(Array * /*vi*/, Bitvector *bv)
{
    Type *elementType = hif::typeGetNestedType(_sourceType, _sem, 1);
    messageAssert(elementType != nullptr, "Unable to get array element type", _sourceType, _sem);

    std::int64_t elements = static_cast<std::int64_t>(hif::semantics::typeGetSpanBitwidth(_sourceType, _sem));
    messageAssert(elements != 0, "Could not calculate elements of array", _sourceType, _sem);

    if (hif::semantics::isVectorType(elementType, _sem)) {
        Value *concat = _factory.member(hif::copy(_inputVal), new IntValue(0));
        for (std::int64_t i = 1; i < elements; ++i) {
            Expression *e =
                _factory.expression(_factory.member(hif::copy(_inputVal), new IntValue(i)), hif::op_concat, concat);
            concat = e;
        }
        _inputVal = concat;
    } else if (dynamic_cast<Int *>(elementType) != nullptr) {
        Range *elementSpan  = hif::typeGetSpan(elementType, _sem);
        Type *innerCastType = hif::copy(bv);
        hif::typeSetSpan(innerCastType, elementSpan, _sem);

        Value *concat = _factory.cast(innerCastType, _factory.member(hif::copy(_inputVal), new IntValue(0)));
        for (std::int64_t i = 1; i < elements; ++i) {
            Expression *e = _factory.expression(
                _factory.cast(hif::copy(innerCastType), _factory.member(hif::copy(_inputVal), new IntValue(i))),
                hif::op_concat, concat);
            concat = e;
        }
        _inputVal = concat;
    } else if (dynamic_cast<Bool *>(elementType) != nullptr || dynamic_cast<Bit *>(elementType) != nullptr) {
        Type *bvType = hif::copy(bv);
        hif::typeSetSpan(bvType, new Range(0, 0), _sem);
        Value *concat = _factory.cast(bvType, _factory.member(hif::copy(_inputVal), new IntValue(0)));
        for (std::int64_t i = 1; i < elements; ++i) {
            Expression *e = _factory.expression(
                _factory.cast(hif::copy(bvType), _factory.member(hif::copy(_inputVal), new IntValue(i))),
                hif::op_concat, concat);
            concat = e;
        }
        _inputVal = concat;
    } else {
        messageError("Unexpected cast from this array type to bitvector", _sourceType, _sem);
    }
    _result = _getCastFromValueToType(_inputVal, bv);
}

void SystemCCastAnalysis::map(ViewReference *vi, Bitvector *bv)
{
#ifdef NDEBUG
    map(static_cast<Object *>(vi), static_cast<Object *>(bv));
#else
    if (vi->getName() != "ams_discipline")
        return;
    _result = _getCastFromValueToType(_inputVal, bv);
#endif
}

void SystemCCastAnalysis::map(Bit * /*b*/, Bitvector *bv)
{
    std::uint64_t size = spanGetBitwidth(bv->getSpan(), _sem);

    std::string charValue = "0";
    BitValue *bValue      = dynamic_cast<BitValue *>(_inputVal);
    if (bValue != nullptr) {
        switch (bValue->getValue()) {
        case bit_zero:
        case bit_one:
            charValue = "0";
            break;
        case bit_x:
            charValue = "X";
            break;
        case bit_z:
            charValue = "Z";
            break;
        case bit_u:
        case bit_w:
        case bit_l:
        case bit_h:
        case bit_dontcare:
        default:
            return; // TODO check
        }
    }

    if (size == 0) {
        // can not establish length of range! So create an expression like this:
        // sc_bv/lv< array.size-1 >("0") op_concat inputVal
        Bitvector *arr = hif::copy(bv);

        Value *spanSize = spanGetSize(bv->getSpan(), _sem);

        IntValue *two = new IntValue(2);
        two->setType(_sem->getTypeForConstant(two));

        Expression *minus = new Expression(op_minus, spanSize, two);
        delete arr->setSpan(new Range(minus, new IntValue(0), dir_downto));

        BitvectorValue *bvv = new BitvectorValue();
        bvv->setType(arr);
        bvv->setValue(charValue);

        Expression *e = _factory.expression(bvv, op_concat, hif::copy(_inputVal));

        // Since we do not know the context, let's add a safety cast,
        // that will be deleted by simplify() if not necessary.
        Cast *c = _factory.cast(hif::copy(bv), e);
        _result = c;
        return;
    } else if (size == 1) {
        // size 1, result will be sc_bv/lv<1>(inputVal)
        // ok, no problem of padding.
        _result = _getCastFromValueToType(_inputVal, bv);
        return;
    }

    // length established create sc_bv/lv "000" (length -1 times) concat inputVal
    std::string s("");
    for (std::uint64_t i = 0; i < size - 1; ++i) {
        s += charValue;
    }

    if (bValue != nullptr) {
        s += bValue->toString();
        Bitvector *retSyntType = hif::copy(bv);
        BitvectorValue *bvv    = new BitvectorValue();
        bvv->setValue(s);
        bvv->setType(retSyntType);
        _result = bvv;
    } else {
        Bitvector *retSyntType = hif::copy(bv);
        delete retSyntType->setSpan(new Range(static_cast<std::int64_t>(size) - 2, 0));
        BitvectorValue *bvv = new BitvectorValue();
        bvv->setValue(s.c_str());
        bvv->setType(retSyntType);

        Expression *e = _factory.expression(bvv, op_concat, hif::copy(_inputVal));
        // Since we do not know the context, let's add a safety cast,
        // that will be deleted by simplify() if not necessary
        Cast *c       = _factory.cast(hif::copy(bv), e);
        _result       = c;
    }
}

void SystemCCastAnalysis::map(Bool *, Bitvector *bv)
{
    std::uint64_t size = spanGetBitwidth(bv->getSpan(), _sem);

    std::string charValue = "0";
    BoolValue *bValue     = dynamic_cast<BoolValue *>(_inputVal);

    if (size == 0) {
        When *w = _factory.when(_factory.whenalt(hif::copy(_inputVal), _factory.intval(1)), _factory.intval(0));

        _result = _getCastFromValueToType(w, bv);
        delete w;
        return;
    } else if (size == 1) {
        // size 1, result will be sc_bv/lv<1>(inputVal)
        // ok, no problem of padding.
        _result = _getCastFromValueToType(_inputVal, bv);
        return;
    }

    // length established create sc_bv/lv "000" (length -1 times) concat inputVal
    std::string s("");
    for (std::uint64_t i = 0; i < size - 1; ++i) {
        s += charValue;
    }

    if (bValue != nullptr) {
        s += bValue->getValue() ? '1' : '0';
        Bitvector *retSyntType = hif::copy(bv);
        BitvectorValue *bvv    = new BitvectorValue();
        bvv->setValue(s);
        bvv->setType(retSyntType);
        _result = bvv;
    } else {
        Bitvector *retSyntType = hif::copy(bv);
        delete retSyntType->setSpan(new Range(static_cast<std::int64_t>(size) - 2, 0));
        BitvectorValue *bvv = new BitvectorValue();
        bvv->setValue(s.c_str());
        bvv->setType(retSyntType);
        Expression expr(op_concat, bvv, hif::copy(_inputVal));
        _result = _getCastFromValueToType(&expr, bv);
    }
}

void SystemCCastAnalysis::map(Real *vi, Bitvector *bv)
{
    // real need to be cast first to int with same number of bits
    if (!_checkValidReal(vi)) {
        // works only with valid real
        return;
    }

    Range *span = vi->getSpan();
    if (span == nullptr)
        return;
    std::uint64_t size = spanGetBitwidth(span, _sem, true);

    Int *castType = _makeSystemCIntFromSize(static_cast<std::int64_t>(size), true, false);
    if (!_isBuiltinIntSize(size)) {
        /// @warning Should never happens?
        castType->setTypeVariant(Type::SYSTEMC_INT_SC_INT);
    }

    Cast cast;
    cast.setValue(hif::copy(_inputVal));
    cast.setType(castType);
    _result = _getCastFromValueToType(&cast, bv);
}

void SystemCCastAnalysis::map(Bit *b1, Bit *b2)
{
    hif::EqualsOptions opt;
    opt.checkConstexprFlag = false;
    if (hif::equals(b1, b2, opt)) {
        _result = hif::copy(_inputVal);
    } else {
        _result = _getCastFromValueToType(_inputVal, b2);
    }
}

void SystemCCastAnalysis::map(Bitvector *vi, Bit *bit)
{
    // return member at position 0.
    Member *mem = _factory.member(hif::copy(_inputVal), _factory.intval(0));

    // if bitvector properties are different from bit to cast
    // an extra cast is needed to preserve type checks.
    if ((vi->isLogic() != bit->isLogic()) || (vi->isConstexpr() != bit->isConstexpr()) ||
        (vi->isResolved() != bit->isResolved())) {
        _result = _getCastFromValueToType(mem, bit);
        delete mem;
        return;
    }

    _result = mem;
}

void SystemCCastAnalysis::map(Char * /*vi*/, Bit *bit) { _result = _getCastFromValueToType(_inputVal, bit); }

void SystemCCastAnalysis::map(Int *i, Bit *bit)
{
    // All maps forces an intermediate cast to bool,
    // to ensure that any int value is allowed.
    // In fact, systemc accepts by default only 0 or 1.
    // Ref design: custom_t3_2_t2_32_32_conv + ddt
    Int *int32Type = _makeSystemCIntFromSize(32, true, false);
    hif::EqualsOptions eopt;
    eopt.checkConstexprFlag = false;
    const bool eqToint32    = hif::equals(int32Type, i, eopt);
    delete int32Type;
    const bool isBuiltinInt = _isBuiltinInt(i) || i->getTypeVariant() == Type::SYSTEMC_INT_BITFIELD;
    if (!isBuiltinInt) {
        // ref. design: verilog/openCores/fht with hdtlib
        std::string toInt(_getToIntFCallName(i));
        _result = _getCastFromValueToType(
            _getCastFromValueToType(_makeMemberFunctionCall(toInt, _inputVal), _factory.boolean()), bit);
    } else if (eqToint32) {
        // already of the most suitable type: plain cast
        _result = _getCastFromValueToType(_getCastFromValueToType(_inputVal, _factory.boolean()), bit);
    } else {
        // forcing intermediate cast to int32, to avoid ambiguities
        // during compiling.
        // Ref design: vhdl/intecs/micro_interface + hif2vp_ovptlm_busport)
        Int *int32Type2 = _makeSystemCIntFromSize(32, true, false);
        _result         = _getCastFromValueToType(
            _getCastFromValueToType(_getCastFromValueToType(_inputVal, int32Type2), _factory.boolean()), bit);
    }
}

void SystemCCastAnalysis::map(Bool *, Bit *bit)
{
    // sc_bit/logic(bool) OK!
    _result = _getCastFromValueToType(_inputVal, bit);
}

void SystemCCastAnalysis::map(Real *, Bit *bit)
{
    // Fine for both sc_bit and sc_logic (sc_bit could accept int64 also)
    Type *retType = _makeSystemCIntFromSize(32, true, true);
    _result       = _getCastFromValueToType(_getCastFromValueToType(_inputVal, retType), bit);
}

void SystemCCastAnalysis::map(ViewReference *vi, Bit *bit)
{
    if (vi->getDesignUnit() == "sc_clock") {
        // special management for sc_clock used in binding.
        // cast is not needed.
        _result = hif::copy(_inputVal);
    } else if (vi->getName() == "ams_discipline") {
        _result = _getCastFromValueToType(_inputVal, bit);
    }
}

void SystemCCastAnalysis::map(Bool *, Bool *) { _result = hif::copy(_inputVal); }

void SystemCCastAnalysis::map(Bit *bit, Bool * /*b*/)
{
    // NOTE: considered case sc_bitref_l and sc_bitref_b
    if (!bit->isLogic() && _sem->usingHdtlib()) {
        _result = hif::copy(_inputVal);
    } else if (!bit->isLogic()) {
        _result = _makeMemberFunctionCall("to_bool", _inputVal);
        _neededIncludes.insert("sc_core");
    } else {
        _result = _factory.expression(
            hif::copy(_inputVal), op_case_eq, _factory.bitval(bit_one, _factory.bit(true, bit->isResolved())));
    }
}

void SystemCCastAnalysis::map(Bitvector *vi, Bool *)
{
    FunctionCall *orReduce = _makeMemberFunctionCall("or_reduce", _inputVal);
    Bit *bType             = _factory.bit(vi->isLogic(), vi->isResolved(), vi->isConstexpr());
    BitValue *b            = _factory.bitval(bit_one, bType);
    _neededIncludes.insert("sc_core");

    Cast *c = new Cast();
    c->setType(hif::copy(bType));
    c->setValue(orReduce);

    _result = _factory.expression(c, op_eq, b);
}

void SystemCCastAnalysis::map(Int * /*vi*/, Bool *b) { _result = _getCastFromValueToType(_inputVal, b); }

void SystemCCastAnalysis::map(Real *, Bool *b) { _result = _getCastFromValueToType(_inputVal, b); }

void SystemCCastAnalysis::map(ViewReference *vi, Bool * /*b*/)
{
    if (vi->getDesignUnit() != "sc_clock")
        return;

    // special management for sc_clock used in binding.
    // cast is not needed.
    _result = hif::copy(_inputVal);
}

void SystemCCastAnalysis::map(Bitvector * /*vi*/, String *)
{
    // call to_string method
    _result = _makeMemberFunctionCall("to_string", _inputVal);
    _neededIncludes.insert("sc_core");
}

void SystemCCastAnalysis::map(Int *vi, String * /*s*/)
{
    const bool isBuiltinInt = _isBuiltinInt(vi) || vi->getTypeVariant() == Type::SYSTEMC_INT_BITFIELD;
    if (isBuiltinInt)
        return;

    // call to_string method
    _result = _makeMemberFunctionCall("to_string", _inputVal);
    _neededIncludes.insert("sc_core");
}

void SystemCCastAnalysis::map(Char * /*vi*/, String *s) { _result = _getCastFromValueToType(_inputVal, s); }

void SystemCCastAnalysis::map(String * /*vi*/, String * /*s*/) { _result = hif::copy(_inputVal); }

void SystemCCastAnalysis::map(Pointer * /*vi*/, Pointer *ptr) { _result = _getCastFromValueToType(_inputVal, ptr); }

void SystemCCastAnalysis::map(Int * /*vi*/, Pointer *ptr) { _result = _getCastFromValueToType(_inputVal, ptr); }

void SystemCCastAnalysis::map(Array *vi, Pointer *ptr)
{
    if (!hif::equals(vi->getType(), ptr->getType()))
        return;

    _result = hif::copy(_inputVal);
}

void SystemCCastAnalysis::map(String * /*vi*/, Pointer *ptr)
{
    Char *c = dynamic_cast<Char *>(getBaseType(ptr->getType(), false, _sem));
    messageAssert(c != nullptr, "Expected char pointer", ptr, _sem);

    if (dynamic_cast<StringValue *>(_inputVal) != nullptr) {
        StringValue *strInput = static_cast<StringValue *>(_inputVal);
        StringValue *ret      = new StringValue();
        ret->setValue(strInput->getValue());
        ret->setType(hif::copy(ptr));
        _result = ret;
    } else {
        // call c_str() method
        _result = _makeMemberFunctionCall("c_str", _inputVal);
        _neededIncludes.insert("string");
    }
}

void SystemCCastAnalysis::map(Type *vi, Reference *ref)
{
    Type *refType = ref->getType();
    // Two cases:
    // 1- the same type: useless cast
    // 2- different type: cast as without reference
    if (hif::equals(refType, vi)) {
        _result = hif::copy(_inputVal);
    } else {
        explicitCast(refType);
    }
}

void SystemCCastAnalysis::map(Reference *vi, Type *t)
{
    Type *refType = vi->getType();
    // Two cases:
    // 1- the same type: useless cast
    // 2- different type: cast as without reference
    if (hif::equals(refType, t)) {
        _result = hif::copy(_inputVal);
    } else {
        explicitCast(t);
    }
}

void SystemCCastAnalysis::map(Record * /*vi*/, Time *t) { _result = _getCastFromValueToType(_inputVal, t); }

void SystemCCastAnalysis::map(Time * /*vi*/, Time * /*t*/) { _result = hif::copy(_inputVal); }

void SystemCCastAnalysis::map(Bit *vi, Real * /*r*/)
{
    if (vi->isLogic()) {
        // call to_int64 method
        _result = _makeMemberFunctionCall("to_int64", _inputVal);
        _neededIncludes.insert("sc_core");
    } else {
        // useless
        _result = hif::copy(_inputVal);
    }
}

void SystemCCastAnalysis::map(Bool * /*vi*/, Real *r) { _result = _getCastFromValueToType(_inputVal, r); }

void SystemCCastAnalysis::map(Bitvector * /*vi*/, Real *r)
{
    // call to_int64 method
    _result = _getCastFromValueToType(_makeMemberFunctionCall("to_int64", _inputVal), r);
    _neededIncludes.insert("sc_core");
}

void SystemCCastAnalysis::map(Int *vi, Real *r)
{
    if (!_sem->usingHdtlib()) {
        _result = _getCastFromValueToType(_inputVal, r);
        _neededIncludes.insert("sc_core");
    } else {
        const bool isBuiltinInt = _isBuiltinInt(vi) || vi->getTypeVariant() == Type::SYSTEMC_INT_BITFIELD;
        if (!isBuiltinInt) {
            _result = _makeMemberFunctionCall("to_double", _inputVal);
            _neededIncludes.insert("sc_core");
        } else {
            _result = _getCastFromValueToType(_inputVal, r);
        }
    }
}

void SystemCCastAnalysis::map(Real * /*vi*/, Real *r) { _result = _getCastFromValueToType(_inputVal, r); }

void SystemCCastAnalysis::map(Time * /*vi*/, Real * /*r*/)
{
    // call to_double method
    _result = _makeMemberFunctionCall("to_double", _inputVal);
    _neededIncludes.insert("sc_core");
}

void SystemCCastAnalysis::map(String * /*vi*/, File *r) { _result = _getCastFromValueToType(_inputVal, r); }

void SystemCCastAnalysis::map(Bit * /*vi*/, ViewReference *vr)
{
#ifdef NDEBUG
    map(static_cast<Object *>(nullptr), static_cast<Object *>(vr));
#else
    if (vr->getName() != "ams_discipline")
        return;
    _result = _getCastFromValueToType(_inputVal, vr);
#endif
}

void SystemCCastAnalysis::map(Bitvector * /*vi*/, ViewReference *vr)
{
#ifdef NDEBUG
    map(static_cast<Object *>(nullptr), static_cast<Object *>(vr));
#else
    if (vr->getName() != "ams_discipline")
        return;
    _result = _getCastFromValueToType(_inputVal, vr);
#endif
}

void SystemCCastAnalysis::map(ViewReference *vi, ViewReference *vr)
{
    if (vi->getDesignUnit() == "tlm_target_socket" && vr->getDesignUnit() == "tlm_initiator_socket") {
        _result = _getCastFromValueToType(_inputVal, vr);
        return;
    }

    if (!isSubType(vi, vr, _sem) && !isSubType(vr, vi, _sem)) {
        // not subtype => not allowed
        return;
    }

    _result = _getCastFromValueToType(_inputVal, vr);
}

void SystemCCastAnalysis::map(Reference *vi, Reference *ref)
{
    // Two cases:
    // 1- the same type: useless cast
    // 2- different type: cast as without reference
    if (hif::equals(ref, vi)) {
        _result = hif::copy(_inputVal);
    } else {
        explicitCast(ref);
    }
}

void SystemCCastAnalysis::map(Pointer *vi, Array *array)
{
    if (!hif::equals(array->getType(), vi->getType()))
        return;
    _result = hif::copy(_inputVal);
}

void SystemCCastAnalysis::map(Pointer * /*vi*/, Bitvector *bv) { _result = _getCastFromValueToType(_inputVal, bv); }
void SystemCCastAnalysis::map(Enum * /*vi*/, Type *t) { _result = _getCastFromValueToType(_inputVal, t); }

void SystemCCastAnalysis::map(Enum * /*vi*/, Reference *t) { _result = _getCastFromValueToType(_inputVal, t); }

void SystemCCastAnalysis::map(Bit * /*vi*/, Char * /*c*/)
{
    _result = _makeMemberFunctionCall("to_char", _inputVal);
    _neededIncludes.insert("sc_core");
}

void SystemCCastAnalysis::map(Bool * /*vi*/, Char * /*c*/)
{
    When *when  = new When();
    _result     = when;
    WhenAlt *wa = new WhenAlt();
    when->alts.push_back(wa);
    wa->setCondition(hif::copy(_inputVal));
    wa->setValue(_factory.charval('1'));
    when->setDefault(_factory.charval('0'));
}

void SystemCCastAnalysis::map(String * /*vi*/, Char * /*c*/)
{
    _result = _factory.member(hif::copy(_inputVal), _factory.intval(0));
}

void SystemCCastAnalysis::map(Record * /*vi*/, Record *r) { _result = _getCastFromValueToType(_inputVal, r); }

void SystemCCastAnalysis::map(TypeReference *vi, Bitvector *bv)
{
    TypeDef *td = dynamic_cast<TypeDef *>(hif::manipulation::instantiate(vi, _sem));
    messageAssert(td != nullptr, "Cannot instantiate.", vi, _sem);
    Value *restore = _inputVal;
    _inputVal      = _getCastFromValueToType(_inputVal, td->getType());
    callMap(td->getType(), bv);
    delete _inputVal;
    _inputVal = restore;
}

void SystemCCastAnalysis::map(TypeReference *vi, Reference *ref) { callMap(vi, ref->getType()); }

void SystemCCastAnalysis::map(TypeReference *vi, Type *t)
{
    TypeDef *td = dynamic_cast<TypeDef *>(hif::manipulation::instantiate(vi, _sem));
    messageAssert(td != nullptr, "Cannot instantiate.", vi, _sem);
    callMap(td->getType(), t);
}

void SystemCCastAnalysis::map(TypeReference *vi, TypeReference *tr)
{
    // Always transform the cast to typeref to a normal cast:
    TypeDef *td = dynamic_cast<TypeDef *>(hif::manipulation::instantiate(tr, _sem));
    messageAssert(td != nullptr, "Cannot instantiate.", tr, _sem);
    callMap(vi, td->getType());
}

void SystemCCastAnalysis::map(Type *vi, TypeReference *tr)
{
    // Always transform the cast to typefer to a normal cast:
    TypeDef *td = dynamic_cast<TypeDef *>(hif::manipulation::instantiate(tr, _sem));
    messageAssert(td != nullptr, "Cannot instantiate.", tr, _sem);
    callMap(vi, td->getType());
}

void SystemCCastAnalysis::map(Enum *vi, TypeReference *tr)
{
    // Disambiguation map: just recall:
    map(static_cast<Type *>(vi), tr);
}

void SystemCCastAnalysis::map(Reference *vi, TypeReference *tr)
{
    // Disambiguation map: just recall:
    map(static_cast<Type *>(vi), tr);
}
// ///////////////////////////////////////////////////////////////////
// SystemCTypeVisitor
// ///////////////////////////////////////////////////////////////////
SystemCTypeVisitor::SystemCTypeVisitor()
    : HifVisitor()
    , _sem(SystemCSemantics::getInstance())
    , _factory(_sem)
    , _result(nullptr)
    , _copts()
{
    _copts.copyChildObjects = false;
}

SystemCTypeVisitor::~SystemCTypeVisitor()
{
    // ntd
}

Type *SystemCTypeVisitor::getResult() { return _result; }

Type *SystemCTypeVisitor::_getStandardMappedType(Type *t)
{
    Type::TypeVariant tv = t->getTypeVariant();
    if (tv != Type::NATIVE_TYPE) {
        messageError("Unexpected type variant", t, _sem);
    }

    return hif::copy(t);
}

int SystemCTypeVisitor::visitBool(Bool &o)
{
    _result = _getStandardMappedType(&o);
    return 0;
}

int SystemCTypeVisitor::visitBit(Bit &o)
{
    Type::TypeVariant tv = o.getTypeVariant();
    if (tv != Type::NATIVE_TYPE && tv != Type::SYSTEMC_BIT_BITREF) {
        messageError("Unexpected type variant", &o, _sem);
    }

    if (o.isLogic() || _sem->getKeepBit() || tv == Type::SYSTEMC_BIT_BITREF) {
        if (o.isResolved() && !_sem->getResolvedTypes()) {
            // remove resolved flag.
            Bit *t = hif::copy(&o);
            t->setResolved(false);
            _result = t;
        } else {
            _result = hif::copy(&o);
        }
    } else {
        // non-logic bit && !keepBit => bool
        _result = new Bool();
    }

    return 0;
}

int SystemCTypeVisitor::visitInt(Int &o)
{
    Type::TypeVariant tv = o.getTypeVariant();
    if (tv != Type::NATIVE_TYPE && tv != Type::SYSTEMC_INT_SC_INT && tv != Type::SYSTEMC_INT_SC_BIGINT &&
        tv != Type::SYSTEMC_INT_BITFIELD) {
        messageError("Unexpected type variant", &o, _sem);
    }

    Int *ret = hif::copy(&o);
    _result  = ret;

    if (o.getTypeVariant() == Type::SYSTEMC_INT_SC_INT || o.getTypeVariant() == Type::SYSTEMC_INT_SC_BIGINT ||
        o.getTypeVariant() == Type::SYSTEMC_INT_BITFIELD) {
        // keep variant (and trust in its correctness)
        return 0;
    }

    Range *old              = o.setSpan(hif::copy(o.getSpan()));
    std::uint64_t size = spanGetBitwidth(o.getSpan(), _sem);
    delete o.setSpan(old);

    if (size == 0) {
        _result->setTypeVariant(Type::SYSTEMC_INT_SC_INT);
    } else if (_isBuiltinIntSize(size)) {
        _result->setTypeVariant(Type::NATIVE_TYPE);
    } else if (size > 64) {
        if (_sem->usingHdtlib())
            _result->setTypeVariant(Type::SYSTEMC_INT_SC_INT);
        else
            _result->setTypeVariant(Type::SYSTEMC_INT_SC_BIGINT);
    } else // not standard spans
    {
        _result->setTypeVariant(Type::SYSTEMC_INT_SC_INT);
    }

    return 0;
}

int SystemCTypeVisitor::visitReal(Real &o)
{
    Type::TypeVariant tv = o.getTypeVariant();
    if (tv != Type::NATIVE_TYPE) {
        messageError("Unexpected type variant", &o, _sem);
    }

    Range *span = o.getSpan();
    messageAssert(span != nullptr, "Real without range", &o, _sem);
    Real *res               = new Real();
    std::uint64_t size = spanGetBitwidth(span, _sem);
    if (size == 0) {
        // As default, returning a double
        res->setSpan(new Range(63, 0));
        // TODO: check if copy original span
    } else if (size <= sizeof(float) * 8) {
        res->setSpan(new Range(static_cast<std::int64_t>(sizeof(float)) * 8 - 1, 0));
    } else if (size <= sizeof(double) * 8) {
        res->setSpan(new Range(static_cast<std::int64_t>(sizeof(double)) * 8 - 1, 0));
    } else //( span_size <= sizeof(long double) * 8 )
    {
        res->setSpan(new Range(static_cast<std::int64_t>(sizeof(long double)) * 8 - 1, 0));
    }
    _result = res;
    return 0;
}

int SystemCTypeVisitor::visitUnsigned(Unsigned &o)
{
    Type::TypeVariant tv = o.getTypeVariant();
    if (tv != Type::NATIVE_TYPE) {
        messageError("Unexpected type variant", &o, _sem);
    }

    Range *range = hif::copy(o.getSpan());
    _result      = _factory.bitvector(range, true, _sem->getResolvedTypes(), o.isConstexpr());
    return 0;
}

int SystemCTypeVisitor::visitSigned(Signed &o)
{
    Type::TypeVariant tv = o.getTypeVariant();
    if (tv != Type::NATIVE_TYPE) {
        messageError("Unexpected type variant", &o, _sem);
    }

    Range *range = hif::copy(o.getSpan());
    _result      = _factory.bitvector(range, true, _sem->getResolvedTypes(), o.isConstexpr());
    return 0;
}

int SystemCTypeVisitor::visitArray(Array &o)
{
    Type::TypeVariant tv = o.getTypeVariant();
    if (tv != Type::NATIVE_TYPE) {
        messageError("Unexpected type variant", &o, _sem);
    }

    Type *mappedType = _sem->getMapForType(o.getType());
    if (mappedType == nullptr)
        return 0;

    _result = _factory.array(hif::copy(o.getSpan()), mappedType, false);
    return 0;
}

int SystemCTypeVisitor::visitBitvector(Bitvector &o)
{
    Type::TypeVariant tv = o.getTypeVariant();
    if (tv != Type::NATIVE_TYPE && tv != Type::SYSTEMC_BITVECTOR_BASE && tv != Type::SYSTEMC_BITVECTOR_PROXY) {
        messageError("Unexpected type variant", &o, _sem);
    }

    Bitvector *ret_bv = hif::copy(&o);
    ret_bv->setSigned(false);
    ret_bv->setResolved(false);
    _result = ret_bv;
    return 0;
}

int SystemCTypeVisitor::visitString(String &o)
{
    hif::CopyOptions copt;
    copt.copyChildObjects = false;
    _result               = hif::copy(&o, copt);
    return 0;
}

int SystemCTypeVisitor::visitTypeReference(TypeReference &o)
{
    _result = _getStandardMappedType(&o);
    return 0;
}

int SystemCTypeVisitor::visitPointer(Pointer &o)
{
    Type *valType = hif::semantics::getBaseType(o.getType(), false, _sem, false);
    if (dynamic_cast<Array *>(valType) != nullptr) {
        Array *arr = static_cast<Array *>(valType);
        arr->getType()->acceptVisitor(*this);
        messageAssert(_result != nullptr, "Unable to map type", arr, _sem);
        Pointer *p = new Pointer();
        p->setType(_result);
        _result = p;
    } else {
        _result = _getStandardMappedType(&o);
    }
    return 0;
}

int SystemCTypeVisitor::visitReference(Reference &o)
{
    _result = _getStandardMappedType(&o);
    return 0;
}

int SystemCTypeVisitor::visitEnum(Enum &o)
{
    _result = _getStandardMappedType(&o);
    return 0;
}

int SystemCTypeVisitor::visitRecord(Record &o)
{
    Type::TypeVariant tv = o.getTypeVariant();
    if (tv != Type::NATIVE_TYPE) {
        messageError("Unexpected type variant", &o, _sem);
    }

    Record *rout = new Record();
    for (BList<Field>::iterator iter = o.fields.begin(); iter != o.fields.end(); ++iter) {
        Field *field    = *iter;
        Field *newField = hif::copy(field, _copts);
        newField->setType(_sem->getMapForType(field->getType()));
        newField->setValue(hif::copy(field->getValue()));
        rout->fields.push_back(newField);
    }
    _result = rout;
    return 0;
}

int SystemCTypeVisitor::visitViewReference(ViewReference &o)
{
    _result = _getStandardMappedType(&o);
    return 0;
}

int SystemCTypeVisitor::visitTime(Time &o)
{
    _result = _getStandardMappedType(&o);
    return 0;
}

int SystemCTypeVisitor::visitChar(Char &o)
{
    _result = _getStandardMappedType(&o);
    return 0;
}

int SystemCTypeVisitor::visitFile(File &o)
{
    _result = _getStandardMappedType(&o);
    return 0;
}

int SystemCTypeVisitor::visitEvent(Event &o)
{
    _result = _getStandardMappedType(&o);
    return 0;
}

// ///////////////////////////////////////////////////////////////////
// SystemCTemplateTypeVisitor
// ///////////////////////////////////////////////////////////////////
SystemCTemplateTypeVisitor::SystemCTemplateTypeVisitor()
    : HifVisitor()
    , _sem(SystemCSemantics::getInstance())
    , _factory(_sem)
    , _result(nullptr)
{
    // ntd
}

SystemCTemplateTypeVisitor::~SystemCTemplateTypeVisitor()
{
    // ntd
}

Type *SystemCTemplateTypeVisitor::getResult() { return _result; }

Type *SystemCTemplateTypeVisitor::_getSuitableIntegerByType(Type *t)
{
    std::uint64_t size = spanGetBitwidth(hif::typeGetSpan(t, _sem), _sem);
    std::int64_t lbound        = 0;
    if (size == 0LL)
        lbound = 31LL;
    else if (size < 8LL)
        lbound = 7LL;
    else if (size < 16LL)
        lbound = 15LL;
    else if (size < 32LL)
        lbound = 31LL;
    else
        lbound = 63LL;

    return _factory.integer(
        _factory.range(lbound, 0LL), hif::typeIsSigned(t, _sem), hif::typeIsConstexpr(t, _sem), Type::NATIVE_TYPE);
}

int SystemCTemplateTypeVisitor::visitArray(Array &o)
{
    Type *arrType = getBaseType(o.getType(), false, _sem, true);
    Bit *bit      = dynamic_cast<Bit *>(arrType);
    Bool *bol     = dynamic_cast<Bool *>(arrType);

    if (bit != nullptr || bol != nullptr) {
        _result = _getSuitableIntegerByType(&o);
        return 0;
    }

    hif::CopyOptions copt;
    copt.copyChildObjects = false;
    Array *mapped         = hif::copy(&o, copt);
    mapped->setSpan(hif::copy(o.getSpan()));
    mapped->setType(_sem->getTemplateAllowedType(o.getType()));
    _result = mapped;

    return 0;
}

int SystemCTemplateTypeVisitor::visitBit(Bit &o)
{
    _result = _factory.boolean(o.isConstexpr(), Type::NATIVE_TYPE);
    return 0;
}

int SystemCTemplateTypeVisitor::visitBitvector(Bitvector &o)
{
    _result = _getSuitableIntegerByType(&o);
    return 0;
}

int SystemCTemplateTypeVisitor::visitInt(Int &o)
{
    _result = _getSuitableIntegerByType(&o);
    return 0;
}

int SystemCTemplateTypeVisitor::visitReal(Real & /*o*/)
{
    // allowed type is int64.
    _result = _factory.integer(_factory.range(63, 0), true, true);
    return 0;
}

int SystemCTemplateTypeVisitor::visitRecord(Record &o)
{
    // allowed type is a new record where all its fields are mapped to
    // corresponding allowed types.
    Record *r = new Record();
    for (BList<Field>::iterator i = o.fields.begin(); i != o.fields.end(); ++i) {
        Field *field    = (*i);
        Type *fType     = field->getType();
        Field *newField = nullptr;

        if (_sem->isTemplateAllowedType(fType)) {
            messageDebugAssert(fType != nullptr, "Unexpected field without type", newField, _sem);
            newField = hif::copy(*i);
        } else {
            Type *goodFieldType = _sem->getTemplateAllowedType(fType);
            messageDebugAssert(goodFieldType != nullptr, "Template allowed type not found for field type", fType, _sem);
            newField = new Field();
            newField->setName(field->getName());
            newField->setType(goodFieldType);
        }

        r->fields.push_back(newField);
    }

    _result = r;
    return 0;
}

int SystemCTemplateTypeVisitor::visitString(String & /*o*/)
{
    // allowed type is global names enum.
    TypeReference *tr = new TypeReference();
    tr->setName(NameTable::getInstance()->hifStringNames());
    _result = tr;
    return 0;
}

int SystemCTemplateTypeVisitor::visitTime(Time & /*o*/)
{
    HifFactory hifFactory(HIFSemantics::getInstance());

    // allowed type is a new record where all its fields are constexpr
    TypeDef *currRec = hifFactory.recordTypeDef(
        "temp",
        (hifFactory.field(hifFactory.integer(hifFactory.range(63, 0)), "value", nullptr),
         hifFactory.field(
             hifFactory.typeRef(
                 "hif_systemc_sc_time_unit", hifFactory.library("hif_systemc_sc_core", nullptr, nullptr, false, true)),
             "unit", nullptr)));

    _result = currRec->setType(nullptr);
    delete currRec;
    return 0;
}

int SystemCTemplateTypeVisitor::visitTypeReference(TypeReference &o)
{
    // allowed type is the type of base type.
    _result = _sem->getTemplateAllowedType(getBaseType(&o, false, _sem));
    return 0;
}
// ///////////////////////////////////////////////////////////////////
// SystemCTypeDefaultValueVisitor
// ///////////////////////////////////////////////////////////////////
SystemCTypeDefaultValueVisitor::SystemCTypeDefaultValueVisitor(Type *originalType, Declaration *d)
    : HifVisitor()
    , _originalType(originalType)
    , _decl(d)
    , _sem(SystemCSemantics::getInstance())
    , _factory(_sem)
    , _result(nullptr)
{
    // ntd
}

SystemCTypeDefaultValueVisitor::~SystemCTypeDefaultValueVisitor()
{
    // ntd
}

Value *SystemCTypeDefaultValueVisitor::getResult() { return _result; }

ConstValue *SystemCTypeDefaultValueVisitor::_makeBitvectorValueOfBitValue(std::uint64_t size, BitConstant b)
{
    if (size == 0)
        return nullptr;

    char cval;
    switch (b) {
    case bit_zero:
        cval = '0';
        break;
    case bit_one:
        cval = '1';
        break;
    case bit_x:
        cval = 'x';
        break;
    case bit_z:
        cval = 'z';
        break;
    case bit_u:
    case bit_w:
    case bit_l:
    case bit_h:
    case bit_dontcare:
    default:
        return nullptr;
    }

    // Build a string made of the right number of cval
    std::string ret;
    ret.reserve(std::string::size_type(size + 1));
    for (std::uint64_t i = 0; i < size; ++i)
        ret.push_back(cval);

    return _factory.stringval(ret.c_str(), false);
}

int SystemCTypeDefaultValueVisitor::visitArray(Array &o)
{
    Range *span = hif::typeGetSpan(_originalType, _sem);
    if (span == nullptr)
        return 0;

    Aggregate *ao           = new Aggregate();
    std::uint64_t size = spanGetBitwidth(span, _sem);
    Value *defValue         = _sem->getTypeDefaultValue(o.getType(), _decl);
    if (size != 0) {
        for (std::uint64_t i = 0; i < size; ++i) {
            AggregateAlt *aao = new AggregateAlt();
            aao->indices.push_back(new IntValue(static_cast<std::int64_t>(i)));
            aao->setValue(hif::copy(defValue));
        }
    } else {
        ao->setOthers(defValue);
    }
    _result = ao;

    return 0;
}

int SystemCTypeDefaultValueVisitor::visitBool(Bool &o)
{
    Bool *tt = hif::copy(&o);
    tt->setConstexpr(true);
    _result = _factory.boolval(false, tt);
    return 0;
}

int SystemCTypeDefaultValueVisitor::visitBit(Bit &o)
{
    Bit *tt = hif::copy(&o);
    tt->setConstexpr(true);
    BitConstant defBitVal = o.isLogic() ? bit_x : bit_zero;
    _result               = _factory.bitval(defBitVal, tt);
    return 0;
}

int SystemCTypeDefaultValueVisitor::visitBitvector(Bitvector &o)
{
    // Get the range and size of range
    Range *span = o.getSpan();
    if (span == nullptr)
        return 0;

    std::uint64_t size = spanGetBitwidth(span, _sem);
    if (size != 0) {
        BitConstant defBitVal = o.isLogic() ? bit_x : bit_zero;
        _result               = _makeBitvectorValueOfBitValue(size, defBitVal);
    } else {
        Bitvector *tt = hif::copy(&o);
        tt->setConstexpr(true);
        std::string val = o.isLogic() ? "X" : "0";
        _result         = _factory.bitvectorval(val, tt);
    }

    return 0;
}

int SystemCTypeDefaultValueVisitor::visitInt(Int &o)
{
    Int *tt = hif::copy(&o);
    tt->setConstexpr(true);
    _result = _factory.intval(0, tt);
    return 0;
}

int SystemCTypeDefaultValueVisitor::visitReal(Real &o)
{
    Real *tt = hif::copy(&o);
    tt->setConstexpr(true);
    _result = _factory.realval(0.0, tt);
    return 0;
}

int SystemCTypeDefaultValueVisitor::visitRecord(Record &o)
{
    RecordValue *recVal = new RecordValue();
    for (BList<Field>::iterator i = o.fields.begin(); i != o.fields.end(); ++i) {
        Field *f            = *i;
        RecordValueAlt *rva = new RecordValueAlt();
        rva->setName(f->getName());
        Value *v = _sem->getTypeDefaultValue(f->getType(), _decl);
        messageAssert(v != nullptr, "type default value not found for record field", f->getType(), _sem);

        rva->setValue(v);
        recVal->alts.push_back(rva);
    }

    _result = recVal;
    return 0;
}

int SystemCTypeDefaultValueVisitor::visitString(String &o)
{
    String *tt = hif::copy(&o);
    tt->setConstexpr(true);
    _result = _factory.stringval("", false, tt);
    return 0;
}

int SystemCTypeDefaultValueVisitor::visitTime(Time &o)
{
    Time *tt = hif::copy(&o);
    tt->setConstexpr(true);
    _result = _factory.timeval(0.0, TimeValue::time_ps, tt);
    return 0;
}

// ///////////////////////////////////////////////////////////////////
// SystemCSuggestedTypeVisitor
// ///////////////////////////////////////////////////////////////////
SystemCSuggestedTypeVisitor::SystemCSuggestedTypeVisitor(
    Operator operation,
    Type *opType,
    Object *startingObject,
    const bool isOp1)
    : HifVisitor()
    , _oper(operation)
    , _opType(opType)
    , _startingObject(startingObject)
    , _isOp1(isOp1)
    , _sem(SystemCSemantics::getInstance())
    , _factory(_sem)
    , _result(nullptr)
{
    // ntd
}

SystemCSuggestedTypeVisitor::~SystemCSuggestedTypeVisitor()
{
    // ntd
}

Type *SystemCSuggestedTypeVisitor::getResult() { return _result; }

int SystemCSuggestedTypeVisitor::visitArray(Array &o)
{
    Bit *bit = dynamic_cast<Bit *>(o.getType());
    if (bit == nullptr) {
        return 0;
    }

    if (_isArithmeticAllowedOp(_oper) || _isShiftNoRotateOp(_oper) ||
        _oper == op_pow // TODO: move in arithmetic operation.
        || hif::operatorIsRelational(_oper) || hif::operatorIsAssignment(_oper)) {
        _result = _factory.integer(hif::copy(o.getSpan()), o.isSigned(), false);
    }

    return 0;
}

int SystemCSuggestedTypeVisitor::visitBool(Bool & /*o*/)
{
    if (_oper != op_concat)
        return 0;

    // translate to bit vector with 1 bit
    _result = _factory.bitvector(new Range(0, 0), false, false, false, false);
    return 0;
}

int SystemCSuggestedTypeVisitor::visitBit(Bit &o)
{
    Bit tmpBitProxy;
    if (o.getTypeVariant() == Type::SYSTEMC_BIT_BITREF) {
        tmpBitProxy.setLogic(o.isLogic());

        _result = _sem->getSuggestedTypeForOp(&tmpBitProxy, _oper, _opType, _startingObject, _isOp1);
        return 0;
    }

    if (_oper == op_concat) {
        // translate to bit vector with 1 bit
        _result = _factory.bitvector(new Range(0, 0), o.isLogic(), o.isResolved(), false, false);
    } else if (_isArithmeticAllowedOp(_oper)) {
        // translate to sc_uint<1>
        _result = _factory.integer(
            new Range(0, 0),
            false, /// @warning: was sc_int<1> in old semantics version
            false, Type::SYSTEMC_INT_SC_INT);
    } else if (hif::operatorIsRelational(_oper) && !o.isLogic()) {
        // translate to bool
        _result = new Bool();
    }

    return 0;
}

int SystemCSuggestedTypeVisitor::visitBitvector(Bitvector &o)
{
    Bitvector tmpProxy;
    if (o.getTypeVariant() == Type::SYSTEMC_BITVECTOR_PROXY) {
        tmpProxy.setLogic(o.isLogic());
        tmpProxy.setSpan(hif::copy(o.getSpan()));

        _result = _sem->getSuggestedTypeForOp(&tmpProxy, _oper, _opType, _startingObject, _isOp1);
        return 0;
    }

    if (_isArithmeticAllowedOp(_oper) || _isShiftNoRotateOp(_oper) ||
        _oper == op_pow) // TODO: move in arithmetic operation.
    {
        // for arrays of bits and
        // arithmetic or shift allowed operations,
        // in this case, return a systemc integer type.
        // Otherwise return a copy of \p t.
        std::uint64_t size = spanGetBitwidth(o.getSpan(), _sem);
        _result = _factory.integer(hif::copy(o.getSpan()), o.isSigned(), false, _getSuitableIntTypeVariantBySize(size));
    } else if (hif::operatorIsRelational(_oper) || hif::operatorIsAssignment(_oper)) {
        Bitvector *ret = hif::copy(&o);
        ret->setSigned(true);
        if (!o.isSigned()) {
            delete ret->setSpan(hif::manipulation::rangeGetIncremented(o.getSpan(), _sem));
        }
        _result = ret;
    }

    return 0;
}

int SystemCSuggestedTypeVisitor::visitInt(Int &o)
{
    if (_oper == op_pow) {
        _result = _getBuiltinInt(&o, true);
    } else if (_oper == op_srl && _isOp1) {
        Int *ret = hif::copy(&o);
        ret->setSigned(false);
        _result = ret;
    }

    return 0;
}

int SystemCSuggestedTypeVisitor::visitTime(Time & /*o*/)
{
    Time *tOpType = dynamic_cast<Time *>(_opType);
    if ((_oper == op_mult || _oper == op_div) && tOpType == nullptr) {
        _result = _factory.real();
    }

    return 0;
}
// ///////////////////////////////////////////////////////////////////
// SystemCConstantTypeVisitor
// ///////////////////////////////////////////////////////////////////
SystemCConstantTypeVisitor::SystemCConstantTypeVisitor()
    : HifVisitor()
    , _sem(SystemCSemantics::getInstance())
    , _factory(_sem)
    , _result(nullptr)
{
    // ntd
}

SystemCConstantTypeVisitor::~SystemCConstantTypeVisitor()
{
    // ntd
}

Type *SystemCConstantTypeVisitor::getResult() { return _result; }

int SystemCConstantTypeVisitor::visitBitValue(BitValue &o)
{
    _result = _factory.bit(!o.is01(), false, true);
    return 0;
}

int SystemCConstantTypeVisitor::visitBitvectorValue(BitvectorValue &o)
{
    Range *span = new Range(static_cast<std::int64_t>(o.getValue().size()) - 1, 0);
    _result     = _factory.bitvector(span, !o.is01(), false, true);
    return 0;
}

int SystemCConstantTypeVisitor::visitBoolValue(BoolValue & /*o*/)
{
    _result = _factory.boolean(true);
    return 0;
}

int SystemCConstantTypeVisitor::visitCharValue(CharValue & /*o*/)
{
    _result = _factory.character(true);
    return 0;
}

int SystemCConstantTypeVisitor::visitIntValue(IntValue &o)
{
    const int32_t i32  = int32_t(o.getValue());
    const int64_t i64  = int64_t(o.getValue());
    const int64_t left = int64_t(i32) == i64 ? 31 : 63;
    _result            = _factory.integer(new Range(left, 0), true, true);
    return 0;
}

int SystemCConstantTypeVisitor::visitRealValue(RealValue & /*o*/)
{
    _result = _factory.real(new Range(63, 0), true);
    return 0;
}

int SystemCConstantTypeVisitor::visitStringValue(StringValue & /*o*/)
{
    _result = _factory.string(true);
    return 0;
}

int SystemCConstantTypeVisitor::visitTimeValue(TimeValue & /*o*/)
{
    _result = _factory.time(true);
    return 0;
}
// ///////////////////////////////////////////////////////////////////
// SystemCBoolConversionVisitor
// ///////////////////////////////////////////////////////////////////
SystemCBoolConversionVisitor::SystemCBoolConversionVisitor(Value *value)
    : HifVisitor()
    , _value(value)
    , _sem(SystemCSemantics::getInstance())
    , _factory(_sem)
    , _result(nullptr)
{
    // ntd
}

SystemCBoolConversionVisitor::~SystemCBoolConversionVisitor()
{
    // ntd
}

Value *SystemCBoolConversionVisitor::getResult() { return _result; }

int SystemCBoolConversionVisitor::visitArray(Array & /*o*/)
{
    // type a[x]: if(a) is translate as (addr != 0)
    // Raise a warning because address is always different from zero.
    messageWarning(
        "type is an address (constant). Expression always true "
        "because address is always different from 0.",
        nullptr, nullptr);
    _result = _factory.boolval(true);
    return 0;
}

int SystemCBoolConversionVisitor::visitBit(Bit &o)
{
    _result = _factory.expression(hif::copy(_value), op_case_eq, _factory.bitval(bit_one, hif::copy(&o)));
    return 0;
}

int SystemCBoolConversionVisitor::visitBitvector(Bitvector &o)
{
    _result = _getVectorsConversion(&o);
    return 0;
}

int SystemCBoolConversionVisitor::visitEnum(Enum &o)
{
    messageAssert(!o.values.empty(), "Unexpected empty enum", &o, _sem);

    Identifier *id = new Identifier(o.values.front()->getName());
    setDeclaration(id, o.values.front());
    _result = _factory.expression(hif::copy(_value), op_case_neq, id);
    return 0;
}

int SystemCBoolConversionVisitor::visitFile(File &o)
{
    messageError("ExplicitBoolConversion type not handled yet.", &o, _sem);
}

int SystemCBoolConversionVisitor::visitLibrary(Library &o)
{
    messageError("ExplicitBoolConversion type not handled yet.", &o, _sem);
}

int SystemCBoolConversionVisitor::visitRecord(Record &o)
{
    messageError("ExplicitBoolConversion type not handled yet.", &o, _sem);
}

int SystemCBoolConversionVisitor::visitReference(Reference &o)
{
    messageError("ExplicitBoolConversion type not handled yet.", &o, _sem);
}

int SystemCBoolConversionVisitor::visitString(String & /*o*/)
{
    _result = _factory.expression(hif::copy(_value), op_case_neq, _factory.stringval(""));
    return 0;
}

int SystemCBoolConversionVisitor::visitSigned(Signed &o)
{
    _result = _getVectorsConversion(&o);
    return 0;
}

int SystemCBoolConversionVisitor::visitTime(Time &o)
{
    messageError("ExplicitBoolConversion type not handled yet.", &o, _sem);
}

int SystemCBoolConversionVisitor::visitUnsigned(Unsigned &o)
{
    _result = _getVectorsConversion(&o);
    return 0;
}

int SystemCBoolConversionVisitor::visitViewReference(ViewReference &o)
{
    messageError("ExplicitBoolConversion type not handled yet.", &o, _sem);
}

Expression *SystemCBoolConversionVisitor::_getVectorsConversion(Type *t)
{
    IntValue ival(0);
    Value *constant = hif::manipulation::transformConstant(&ival, t, _sem);
    messageAssert(constant != nullptr, "Transform constant failed.", t, _sem);

    _result = _factory.expression(hif::copy(_value), op_case_eq, _factory.expression(op_bnot, constant));
    return nullptr;
}
} // namespace
// ///////////////////////////////////////////////////////////////////
// SystemCSemantics public methods
// ///////////////////////////////////////////////////////////////////

// Wrapper used to suppress useless warnings:
#ifdef __GNUC__
#    pragma GCC diagnostic ignored "-Wswitch-default"
#    pragma GCC diagnostic ignored "-Wswitch-enum"
#else
#    pragma warning(disable : 4127)
#    pragma warning(disable : 4244)
#    pragma warning(disable : 4505)
#endif

SystemCSemantics::SystemCSemantics()
    : ILanguageSemantics()
    , _stdMap()
    , _useHdtlib(false)
    , _keepBit(false)
    , _resolvedTypes(false)
{
    _hifFactory.setSemantics(HIFSemantics::getInstance());

    _initForbiddenNames();
    _initStandardSymbols();
    _initStandardFilenames();

    // Initializing semantic check options.
    _semanticOptions.port_inNoInitialValue        = false;
    _semanticOptions.port_outInitialValue         = false;
    _semanticOptions.dataDeclaration_initialValue = false;
    _semanticOptions.scopedType_insideTypedef     = false;
    _semanticOptions.int_bitfields                = true;
    _semanticOptions.designUnit_uniqueView        = true;
    _semanticOptions.for_implictIndex             = false;
    _semanticOptions.for_conditionType            = SemanticOptions::EXPRESSION;
    _semanticOptions.generates_isNoAllowed        = true;
    _semanticOptions.after_isNoAllowed            = true;
    _semanticOptions.with_isNoAllowed             = false; // should be true
    _semanticOptions.globact_isNoAllowed          = true;
    _semanticOptions.valueStatement_isNoAllowed   = false;
    _semanticOptions.case_isOnlyLiteral           = true;
    _semanticOptions.lang_signPortNoBitAccess     = true;
    _semanticOptions.lang_hasDontCare             = false;
    _semanticOptions.lang_has9logic               = false;
    _semanticOptions.waitWithActions              = false;
    _semanticOptions.lang_sortKind                = hif::manipulation::SortMissingKind::LIMITED;
}

SystemCSemantics::~SystemCSemantics()
{
    for (DeclarationMap::iterator i = _stdMap.begin(); i != _stdMap.end(); ++i) {
        for (SubList::iterator j = i->second.begin(); j != i->second.end(); ++j) {
            delete *j;
        }
    }
}

SystemCSemantics *SystemCSemantics::getInstance()
{
    static SystemCSemantics instance;
    return &instance;
}

std::string SystemCSemantics::getName() const { return "systemc"; }

ILanguageSemantics::ExpressionTypeInfo
SystemCSemantics::getExprType(Type *op1Type, Type *op2Type, Operator operation, Object *sourceObj)
{
    ILanguageSemantics::ExpressionTypeInfo ret;

    if (!isSupported(operation))
        return ret;

    SystemCAnalysis syscAnalysis(operation, sourceObj);

    if (operation == op_none) {
        if (op2Type != nullptr || op1Type == nullptr)
            return ret;
        ret.returnedType       = hif::copy(op1Type);
        ret.operationPrecision = hif::copy(op1Type);
    } else if (operation == op_ref && op1Type != nullptr && op2Type == nullptr) {
        // common support for unary operator ref.
        Pointer *p             = _makeSystemCPointer(op1Type);
        ret.returnedType       = p;
        ret.operationPrecision = hif::copy(op1Type);
    } else if (
        operation == op_deref && op1Type != nullptr && op2Type == nullptr &&
        dynamic_cast<Pointer *>(op1Type) != nullptr) {
        // common support for unary operator deref.
        ret.returnedType       = hif::copy(static_cast<Pointer *>(op1Type)->getType());
        ret.operationPrecision = hif::copy(op1Type);
    } else {
        // standard analyze
        syscAnalysis.analyzeOperands(op1Type, op2Type);
        ret = syscAnalysis.getResult();
    }

    return ret;
}

bool SystemCSemantics::isTemplateAllowedType(Type *t)
{
    // valid types are integral type: char, int, bool, emum, bit (not logic),
    // and typerefs to them.
    if (dynamic_cast<Char *>(t) != nullptr) {
        Char *c = static_cast<Char *>(t);
        return (c->getTypeVariant() == Type::NATIVE_TYPE);
    } else if (dynamic_cast<Int *>(t) != nullptr) {
        // check for sc_int, sc_uint, sc_bigint, sc_ubigint
        // => only not constexpr types are bad.
        Int *i = static_cast<Int *>(t);
        return (_isBuiltinInt(i));
    } else if (dynamic_cast<Bool *>(t) != nullptr) {
        Bool *b = static_cast<Bool *>(t);
        return (b->getTypeVariant() == Type::NATIVE_TYPE);
    } else if (dynamic_cast<Enum *>(t) != nullptr) {
        return true;
    } else if (dynamic_cast<Bit *>(t) != nullptr) {
        Bit *b = static_cast<Bit *>(t);
        return (b->getTypeVariant() != Type::NATIVE_TYPE && !b->isLogic() && !_keepBit);
    } else if (dynamic_cast<TypeReference *>(t) != nullptr) {
        Type *bt = getBaseType(t, false, this);
        if (bt != t)
            return isTemplateAllowedType(bt);
    }

    return false;
}

Type *SystemCSemantics::getTemplateAllowedType(Type *t)
{
    messageAssert(t != nullptr, "Passed nullptr type", nullptr, this);
    if (isTemplateAllowedType(t))
        return hif::copy(t);

    SystemCTemplateTypeVisitor v;
    t->acceptVisitor(v);

    Type *res = v.getResult();
    messageDebugAssert(res != nullptr, "Unexpected type to be mapped as template", t, this);
    return res;
}

bool SystemCSemantics::isForbiddenName(Declaration *decl)
{
    if (hif::declarationIsPartOfStandard(decl))
        return false;
    std::string key(decl->getName());
    if (_forbiddenNames.find(key) != _forbiddenNames.end())
        return true;
    return false;
}

bool SystemCSemantics::isSliceTypeRebased() { return true; }

bool SystemCSemantics::isSyntacticTypeRebased() { return true; }

void SystemCSemantics::setUseHdtlib(const bool v) { _useHdtlib = v; }

bool SystemCSemantics::usingHdtlib() { return _useHdtlib; }

void SystemCSemantics::setKeepBit(const bool v) { _keepBit = v; }

bool SystemCSemantics::getKeepBit() { return _keepBit; }

void SystemCSemantics::setResolvedTypes(const bool v) { _resolvedTypes = v; }

bool SystemCSemantics::getResolvedTypes() { return _resolvedTypes; }

Value *SystemCSemantics::getTypeDefaultValue(Type *type, Declaration *d)
{
    Type *base = getBaseType(type, false, this, false);
    SystemCTypeDefaultValueVisitor vis(type, d);
    base->acceptVisitor(vis);
    Value *v = vis.getResult();
    if (dynamic_cast<ConstValue *>(v) != nullptr && dynamic_cast<TypeReference *>(type) != nullptr) {
        ConstValue *local = static_cast<ConstValue *>(v);
        local->setType(hif::copy(type));
    }
    return v;
}

bool SystemCSemantics::isSupported(Operator operation)
{
    // NOTE: operator pow is not supported in SystemC but it is consider
    // supported because hif2sc backend fix it in postFixVisitor
    // translating expression with op_pow in a Fcall to standard method pow.
    if (operation == op_abs
        //|| operation == op_pow
        //||operation == op_none
    ) {
        return false;
    } else {
        return true;
    }
}

Type *SystemCSemantics::getMapForType(Type *t)
{
    if (t == nullptr)
        return nullptr;

    SystemCTypeVisitor v;
    t->acceptVisitor(v);
    return v.getResult();
}

Operator
SystemCSemantics::getMapForOperator(Operator srcOperation, Type *srcT1, Type *srcT2, Type *dstT1, Type * /*dstT2*/)
{
    Pointer *srcPtr1 = dynamic_cast<Pointer *>(srcT1);
    Pointer *dstPtr1 = dynamic_cast<Pointer *>(dstT1);
    Array *arr       = srcPtr1 == nullptr ? nullptr : dynamic_cast<Array *>(srcPtr1->getType());

    if (dynamic_cast<Bit *>(srcT1) != nullptr && srcT2 == nullptr && !static_cast<Bit *>(srcT1)->isLogic() &&
        srcOperation == op_bnot) {
        if (dynamic_cast<Bool *>(dstT1) != nullptr) {
            return op_not;
        } else if (dynamic_cast<Bit *>(dstT1) != nullptr && _keepBit) {
            return op_bnot;
        } else {
            messageError("Expected mapping of not-logic bit type to bool.", dstT1, this);
        }
    } else if (dstPtr1 != nullptr && arr != nullptr && (srcOperation == hif::op_ref || srcOperation == hif::op_deref)) {
        return op_none;
    }

    return srcOperation;
}

Type *SystemCSemantics::getSuggestedTypeForOp(
    Type *t,
    Operator operation,
    Type *opType,
    Object *startingObject,
    const bool isOp1)
{
    if (t == nullptr)
        return nullptr;
    Type *ret = nullptr;

    // 1 - if allowed by semantics => use returned type.
    ILanguageSemantics::ExpressionTypeInfo info = getExprType(t, t, operation, startingObject);
    if (info.returnedType != nullptr) {
        ret               = info.returnedType;
        info.returnedType = nullptr;
        return ret;
    }

    // 2 - common part for all types
    if (!isSupported(operation)) {
        // not supported. Return same type.
        return hif::copy(t);
    }

    if (operation == op_mod) {
        return _factory.integer(new Range(31, 0), true, false);
    }

    if (hif::operatorIsShift(operation) && !isOp1) {
        ret = hif::copy(t);
        if (dynamic_cast<Int *>(ret) != nullptr) {
            Int *ii = static_cast<Int *>(ret);
            ret     = _getBuiltinInt(ii, false);
        }
        return ret;
    }

    // 3 - visitor part
    SystemCSuggestedTypeVisitor v(operation, opType, startingObject, isOp1);
    t->acceptVisitor(v);
    ret = v.getResult();

    // NOTE: visitor may return nullptr in this case the policy is to return a copy of
    // given type
    if (ret == nullptr)
        ret = hif::copy(t);
    return ret;
}

Type *SystemCSemantics::getTypeForConstant(ConstValue *c)
{
    if (c == nullptr)
        return nullptr;

    SystemCConstantTypeVisitor v;
    c->acceptVisitor(v);
    return v.getResult();
}

bool SystemCSemantics::checkCondition(Type *t, Object *o)
{
    if (!t)
        return false;

    Bit *bit = dynamic_cast<Bit *>(t);
    if (bit)
        return _checkConditionBit(bit, o);

    Array *array = dynamic_cast<Array *>(t);
    if (array)
        return true;

    Int *integer = dynamic_cast<Int *>(t);
    if (integer)
        return _checkConditionInteger(integer);

    Real *real = dynamic_cast<Real *>(t);
    if (real)
        return _checkValidReal(real);

    return dynamic_cast<Bool *>(t) != nullptr;
    ;
}

Value *SystemCSemantics::explicitBoolConversion(Value *c)
{
    Type *type = getSemanticType(c, this);
    if (type == nullptr)
        return nullptr;
    type = getBaseType(type, false, this, false);
    if (type == nullptr)
        return nullptr;

    // check if the value passed as condition is valid
    if (!checkCondition(type, nullptr))
        return nullptr;

    SystemCBoolConversionVisitor v(c);
    type->acceptVisitor(v);
    return v.getResult();
}

Value *SystemCSemantics::explicitCast(Value *valueToCast, Type *castType, Type *srcType)
{
    // call the map access method
    SystemCCastAnalysis sca(valueToCast, srcType);
    Value *ret = sca.explicitCast(castType);

    // generate collected includes
    sca.generateIncludes(valueToCast);

    return ret;
}

Type *SystemCSemantics::getMemberSemanticType(Member *m)
{
    Type *prefixType = getBaseType(m->getPrefix(), false, this, false);
    if (prefixType == nullptr)
        return nullptr;

    if (dynamic_cast<Bitvector *>(prefixType) != nullptr && !usingHdtlib()) {
        Type *tInd = getSemanticType(m->getIndex(), this);
        messageAssert(tInd != nullptr, "Cannot type index", m->getIndex(), this);
        Bitvector *bv = static_cast<Bitvector *>(prefixType);
        return _factory.bit(
            bv->isLogic(), // Ref design vhdl/openCores/uart
            false, bv->isConstexpr() && hif::typeIsConstexpr(tInd, this), Type::SYSTEMC_BIT_BITREF);
    } else if (dynamic_cast<Pointer *>(prefixType) != nullptr) {
        Pointer *p = static_cast<Pointer *>(prefixType);
        return hif::copy(p->getType());
    }

    // TODO: need to remove opacity?
    return ILanguageSemantics::getMemberSemanticType(m);
}

Type *SystemCSemantics::getSliceSemanticType(Slice *s)
{
    Type *prefixType = getSemanticType(s->getPrefix(), this);
    if (prefixType == nullptr)
        return nullptr;

    Bitvector *bv = dynamic_cast<Bitvector *>(prefixType);
    if (bv == nullptr || usingHdtlib()) {
        // use standard parent map for not-bitvector type or when is used hdtlib.
        return ILanguageSemantics::getSliceSemanticType(s);
    }

    Value *max = hif::rangeGetMaxBound(s->getSpan());
    Value *min = hif::rangeGetMinBound(s->getSpan());

    Type *tMax = getSemanticType(max, this);
    messageAssert(tMax != nullptr, "Cannot type bound 1", max, this);
    Type *tMin = getSemanticType(min, this);
    messageAssert(tMin != nullptr, "Cannot type bound 2", min, this);

    Expression *e      = _factory.expression(hif::copy(max), op_minus, hif::copy(min));
    const bool isConst = bv->isConstexpr() && hif::typeIsConstexpr(tMax, this) && hif::typeIsConstexpr(tMin, this);

    return _factory.bitvector(
        new Range(e, _factory.intval(0), dir_downto), bv->isLogic(), false, isConst, bv->isSigned(),
        Type::SYSTEMC_BITVECTOR_PROXY);
}

std::int64_t SystemCSemantics::transformRealToInt(const double v) { return static_cast<std::int64_t>(v); }

Type *SystemCSemantics::isTypeAllowedAsBound(Type *t)
{
    messageDebugAssert(t != nullptr, "Passed null type", nullptr, this);

    Int *intT = dynamic_cast<Int *>(t);
    if (_isBuiltinInt(intT)) {
        // is good for bounds
        return nullptr;
    }

    // not good => return the suitable good type.
    return _getBuiltinInt(t, true);
}

bool SystemCSemantics::isCastAllowed(Type *target, Type *source)
{
    // Not allowed cases:
    // - bit-real
    // - bit-bv
    // - bitvector-real
    // - bool-sc_logic_value_t
    // - sc_proxy-sc_int/bigint
    // - bit - uint* / int != 32
    if (dynamic_cast<Bit *>(target) != nullptr && dynamic_cast<Real *>(source) != nullptr) {
        return false;
    } else if (dynamic_cast<Bit *>(target) != nullptr && dynamic_cast<Bitvector *>(source) != nullptr) {
        return false;
    } else if (dynamic_cast<Bit *>(target) != nullptr && dynamic_cast<Int *>(source) != nullptr) {
        Int *ii = static_cast<Int *>(source);
        if (!ii->isSigned())
            return false;
        std::uint64_t size = hif::semantics::spanGetBitwidth(ii->getSpan(), this);
        return size == 32;
    } else if (dynamic_cast<Bitvector *>(target) != nullptr && dynamic_cast<Real *>(source) != nullptr) {
        return false;
    } else if (dynamic_cast<Bool *>(target) != nullptr && dynamic_cast<TypeReference *>(source) != nullptr) {
        // special case: sc_logic_value_t
        TypeReference *tr = static_cast<TypeReference *>(source);
        Type *base        = getBaseType(tr, false, this);
        if (dynamic_cast<Enum *>(base) != nullptr) {
            TypeDef *td = dynamic_cast<TypeDef *>(base->getParent());
            messageAssert(td != nullptr, "Unexpected parent", base->getParent(), this);
            if (td->getName() == "sc_logic_value_t")
                return false;
        }
    } else if (
        dynamic_cast<Int *>(target) != nullptr && !_isBuiltinInt(static_cast<Int *>(target)) &&
        dynamic_cast<Bitvector *>(source) != nullptr && (source->getTypeVariant() == Type::SYSTEMC_BITVECTOR_PROXY)) {
        // special case: sc_proxy -> sc_int
        return false;
    }

    return true;
}

bool SystemCSemantics::isTypeAllowed(Type *t)
{
    if (t == nullptr)
        return false;
    Type::TypeVariant tv = t->getTypeVariant();

    if (dynamic_cast<Signed *>(t) != nullptr || dynamic_cast<Unsigned *>(t) != nullptr) {
        // Not allowed. TODO: remove when Signed and Unsigned will be removed.
        return false;
    } else if (dynamic_cast<Bit *>(t) != nullptr && !static_cast<Bit *>(t)->isLogic()) {
        // Non-logic bits are allowed only if keebit is true.
        if (!_keepBit)
            return false;
        return (tv == Type::SYSTEMC_BIT_BITREF || tv == Type::NATIVE_TYPE);
    } else if (dynamic_cast<Int *>(t) != nullptr) {
        Int *i                      = static_cast<Int *>(t);
        std::uint64_t spanSize = spanGetBitwidth(i->getSpan(), this);

        if (tv == Type::SYSTEMC_INT_BITFIELD) {
            return (spanSize > 0 && spanSize < 64);
        } else if (tv == Type::NATIVE_TYPE) {
            return (spanSize == 8 || spanSize == 16 || spanSize == 32 || spanSize == 64);
        } else if (tv == Type::SYSTEMC_INT_SC_INT) {
            // not allowed sc_int with span greather than 64 bits
            return (spanSize == 0 || spanSize < 64);
        } else {
            return (tv == Type::SYSTEMC_INT_SC_BIGINT);
        }
    } else if (dynamic_cast<Bitvector *>(t) != nullptr) {
        return (tv == Type::NATIVE_TYPE || tv == Type::SYSTEMC_BITVECTOR_BASE || tv == Type::SYSTEMC_BITVECTOR_PROXY);
    }

    // All other types are allowed if has no variants.
    return (tv == Type::NATIVE_TYPE);
}

bool SystemCSemantics::isRangeDirectionAllowed(RangeDirection r) { return (r == dir_downto); }

bool SystemCSemantics::isTypeAllowedAsCase(Type *t) { return isTemplateAllowedType(t); }

bool SystemCSemantics::isTypeAllowedAsReturn(Type *type)
{
    if (!ILanguageSemantics::isTypeAllowedAsReturn(type))
        return false;
    Type *base = hif::semantics::getBaseType(type, false, this);
    if (dynamic_cast<Array *>(base) != nullptr)
        return false;
    return true;
}

bool SystemCSemantics::isTypeAllowedAsPort(Type *t)
{
    if (!isTypeAllowed(t))
        return false;

    t = hif::semantics::getBaseType(t, false, this, true);
    t = hif::typeGetNestedType(t, this);

    // not allowed
    if (dynamic_cast<Event *>(t) != nullptr)
        return false;
    if (dynamic_cast<String *>(t) != nullptr)
        return false;
    if (dynamic_cast<Reference *>(t) != nullptr)
        return false;
    if (dynamic_cast<Pointer *>(t) != nullptr)
        return false;
    if (dynamic_cast<File *>(t) != nullptr)
        return false;
    if (dynamic_cast<Time *>(t) != nullptr)
        return false;

    return true;
}

bool SystemCSemantics::isLanguageIdAllowed(LanguageID /*id*/) { return true; }

bool SystemCSemantics::isValueAllowedInPortBinding(Value *o)
{
    // only identifiers are allowed
    return (dynamic_cast<Identifier *>(o) != nullptr);
}

bool SystemCSemantics::isTypeAllowedForConstValue(ConstValue *cv, Type *synType)
{
    Type *baseSynType = getBaseType(synType, false, this);

    // Special case: StringValue
    if (dynamic_cast<StringValue *>(cv) != nullptr) {
        if (dynamic_cast<String *>(baseSynType) != nullptr)
            return true;

        Pointer *p = dynamic_cast<Pointer *>(baseSynType);
        if (p != nullptr && (dynamic_cast<Char *>(p->getType()) != nullptr))
            return true;
    }

    Type *dt = this->getTypeForConstant(cv);
    hif::EqualsOptions opt;
    opt.checkOnlyTypes    = true;
    opt.handleVectorTypes = true;

    const bool res = hif::equals(dt, baseSynType, opt);
    delete dt;

    return res;
}

bool SystemCSemantics::_checkConcatCasts(
    hif::Expression *e,
    Type *castT1,
    Type *castT2,
    Type *subT1,
    Type *subT2,
    ExpressionTypeInfo &exprInfo,
    ExpressionTypeInfo &info)
{
    if (!ILanguageSemantics::_checkConcatCasts(e, castT1, castT2, subT1, subT2, exprInfo, info)) {
        return false;
    }

    // Special case:
    // concat of bv and int implicitely converts int to bool.
    // Therefore cannot remove cast.
    const bool isBvCastT1 = dynamic_cast<Bitvector *>(castT1) != nullptr;
    const bool isBvCastT2 = dynamic_cast<Bitvector *>(castT2) != nullptr;
    const bool isIntSubT1 = dynamic_cast<Int *>(subT1) != nullptr;
    const bool isIntSubT2 = dynamic_cast<Int *>(subT2) != nullptr;

    if ((isBvCastT1 && isIntSubT1) || (isBvCastT2 && isIntSubT2)) {
        return false;
    }

    return true;
}

} // namespace semantics
} // namespace hif
