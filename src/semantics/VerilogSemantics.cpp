/// @file VerilogSemantics.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <iostream>
#include <limits>
#include <stdint.h>

#include "hif/BiVisitor.hpp"
#include "hif/HifFactory.hpp"
#include "hif/hif.hpp"
#include "hif/semantics/SemanticAnalysis.hpp"
#include "hif/semantics/VerilogSemantics.hpp"

namespace hif
{
namespace semantics
{

namespace
{ // unnamed

/// This class implements the analysis of the Verilog types. It implements
/// the analyzeOperands setting the result field.
class VerilogAnalysis : public SemanticAnalysis, public BiVisitor<VerilogAnalysis>
{
public:
    VerilogAnalysis(Operator currOperator, Object *srcObj);
    virtual ~VerilogAnalysis();

    /// Function to analyze the type of the operands involved in current
    /// operation.
    virtual void analyzeOperands(Type *op1Type, Type *op2Type);

    /// @name Unary operators
    /// @{

    /// Base case for unary operators.
    void map(Object *);

    void map(Array *array);
    void map(Bitvector *array);
    void map(Bit *bit);
    void map(Real *real);
    void map(TypeReference *op);
    void map(Event *e);

    /// @}
    /// @name Binary operators
    /// @{

    /// Base case for binary operators.
    void map(Object *, Object *);

    // Maps to Array
    void map(Array *array1, Array *array2);
    void map(Bit *bit, Array *array);
    void map(Bitvector *bv, Array *array2);
    void map(Real *real, Array *array);
    void map(ViewReference *vr, Array *array);

    // Maps to Bitvector
    void map(Bitvector *array1, Bitvector *array2);
    void map(Array *array, Bitvector *bv);
    void map(Time *t1, Bitvector *bv);
    void map(Bit *bit, Bitvector *array);
    void map(Real *real, Bitvector *bv);
    void map(String *s, Bitvector *bv);

    // Maps to Bit
    void map(Bit *bit1, Bit *bit2);
    void map(Array *array, Bit *bit);
    void map(Bitvector *array, Bit *bit);
    void map(Real *real, Bit *b);

    // Maps to Real
    void map(Real *real1, Real *real2);
    void map(Array *array, Real *real);
    void map(Bit *b, Real *real);
    void map(Bitvector *bv, Real *real);
    void map(Time *t1, Real *r);

    // Maps to String
    void map(String *s1, String *s2);
    void map(Bitvector *bv, String *s);

    // Maps to Time
    void map(Time *t1, Time *t2);
    void map(Real *r, Time *t2);
    void map(Bitvector *bv, Time *t2);

    // Maps involving Typerefs
    void map(TypeReference *op1, TypeReference *op2);
    void map(TypeReference *op1, Type *op2);
    void map(Type *op1, TypeReference *op2);

    // Maps involving Viewrefs
    void map(ViewReference *t1, ViewReference *t2);
    void map(Array *array, ViewReference *vr);

    /// @}

private:
    VerilogSemantics *_verilogSem;

    virtual void _callMap(Object *op1);
    virtual void _callMap(Object *op1, Object *op2);

    // disabled.
    VerilogAnalysis(const VerilogAnalysis &);
    VerilogAnalysis &operator=(const VerilogAnalysis &);
};
/// This class implements the getMapForType function. Each map function
/// manages a different input type.
class VerilogTypeVisitor : public HifVisitor
{
public:
    VerilogTypeVisitor();
    virtual ~VerilogTypeVisitor();

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
    virtual int visitTime(Time &o);
    virtual int visitTypeReference(TypeReference &o);

private:
    VerilogSemantics *_sem;
    HifFactory _factory;
    Type *_result;

    VerilogTypeVisitor(const VerilogTypeVisitor &);
    VerilogTypeVisitor &operator=(const VerilogTypeVisitor &);
};
//------------------------- helper functions ------------------------------

/// Returns true if the operator is relational (<, <=, >, >=, ==, !=).
bool _isRelational(Operator operation);

/// Returns true if the operator is arithmetic
bool _isArithmetic(Operator operation);

/// Returns true if the operator is logical (!, &&, ||, xor).
bool _isLogical(Operator operation);

/// Returns true if the given operator is unary.
bool _isUnary(Operator operation);

/// Returns true if the given operator is a reduce operator..
bool _isReduce(Operator operation);

/// This function builds a Bit that respects the verilog semantics.
Bit *_makeVerilogBitType(bool isConstexpr = false);

/// This function builds an Array that represent a verilog reg. It is
/// an array unsigned, packed with type bit logic. The range given is
/// not copied.
/// @param range the range of the array
Bitvector *_makeVerilogRegisterType(Range *range = nullptr, bool is_constexpr = false, bool is_signed = false);

/// This function builds a Bitvectorvalue Object with value specified as
/// parameter and logic flag appropriated set.
BitvectorValue *_makeVerilogBitVectorValue(const std::string &value);
/// This function checks if the given real object respects the verilog
/// semantics.
/// @param real the object to check
/// @return true if the real is conformal, false otherwise.
bool _checkReal(Real *real);

/// This function checks if the given operator is permitted on verilog
/// reals.
/// @return true if the operation is permitted, false otherwise
bool _isPermittedOnReals(Operator operation);

} // namespace
// ---------------- VerilogSemantics --------------------

// Wrapper used to suppress useless warnings:
#ifdef __GNUC__
#    pragma GCC diagnostic ignored "-Wswitch-default"
#    pragma GCC diagnostic ignored "-Wswitch-enum"
#else
#    pragma warning(disable : 4127)
#    pragma warning(disable : 4244)
#    pragma warning(disable : 4505)
#endif
VerilogSemantics::VerilogSemantics()
    : ILanguageSemantics()
    , _stdMap()
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
    _semanticOptions.int_bitfields                = false;
    _semanticOptions.designUnit_uniqueView        = true;
    _semanticOptions.for_implictIndex             = false;
    _semanticOptions.for_conditionType            = SemanticOptions::EXPRESSION;
    _semanticOptions.generates_isNoAllowed        = true;
    _semanticOptions.after_isNoAllowed            = false;
    _semanticOptions.with_isNoAllowed             = true;
    _semanticOptions.globact_isNoAllowed          = false;
    _semanticOptions.valueStatement_isNoAllowed   = false;
    _semanticOptions.case_isOnlyLiteral           = false;
    _semanticOptions.lang_signPortNoBitAccess     = false;
    _semanticOptions.lang_hasDontCare             = true;
    _semanticOptions.lang_has9logic               = false;
    _semanticOptions.waitWithActions              = true;
    _semanticOptions.lang_sortKind                = hif::manipulation::SortMissingKind::NOTHING;
}
VerilogSemantics::~VerilogSemantics()
{
    for (DeclarationMap::iterator i = _stdMap.begin(); i != _stdMap.end(); ++i) {
        for (SubList::iterator j = i->second.begin(); j != i->second.end(); ++j) {
            delete *j;
        }
    }
}
std::string VerilogSemantics::getName() const { return "verilog"; }
ILanguageSemantics::ExpressionTypeInfo
VerilogSemantics::getExprType(Type *op1Type, Type *op2Type, Operator operation, Object *sourceObj)
{
    ILanguageSemantics::ExpressionTypeInfo ret;

    // Set current operation operand and analyze operands
    if (!isSupported(operation))
        return ret;
    VerilogAnalysis verilogAnalysis(operation, sourceObj);
    verilogAnalysis.analyzeOperands(op1Type, op2Type);
    ret = verilogAnalysis.getResult();
    return ret;
}

bool VerilogSemantics::isTemplateAllowedType(Type * /*t*/)
{
    // all types are allowed at the moment
    return true;
}
Type *VerilogSemantics::getTemplateAllowedType(Type *t) { return hif::copy(t); }
VerilogSemantics *VerilogSemantics::getInstance()
{
    static VerilogSemantics instance;
    return &instance;
}
bool VerilogSemantics::isForbiddenName(Declaration *decl)
{
    if (hif::declarationIsPartOfStandard(decl))
        return false;
    std::string key(decl->getName());
    if (_forbiddenNames.find(key) != _forbiddenNames.end())
        return true;
    return false;
}
bool VerilogSemantics::isSliceTypeRebased()
{
    // Could be also true since original verilog should not allow it.
    return false;
}

bool VerilogSemantics::isSyntacticTypeRebased() { return false; }

Value *VerilogSemantics::getTypeDefaultValue(Type *type, Declaration *d)
{
    // in case of nets, use 'Z' as default value instead of 'X'
    bool isNetDecl = (dynamic_cast<Signal *>(d) != nullptr) || (dynamic_cast<Port *>(d) != nullptr);

    if (dynamic_cast<Real *>(type) != nullptr) {
        Real *tt = static_cast<Real *>(type);
        tt       = hif::copy(tt);
        tt->setConstexpr(true);
        RealValue *ret = new RealValue(0.0);
        ret->setType(tt);
        return ret;
    } else if (dynamic_cast<Int *>(type) != nullptr) {
        // a 32 bit X (HIF:Variable Verilog:reg/trireg) - a 32 bit Z (HIF:Signal Verilog:wire/general net)
        // default: X
        std::string initV("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
        if (isNetDecl)
            initV = "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ";
        return _makeVerilogBitVectorValue(initV);
    } else if (dynamic_cast<Bit *>(type) != nullptr) {
        Bit *tt = static_cast<Bit *>(type);
        tt      = hif::copy(tt);
        tt->setConstexpr(true);
        BitValue *ret = new BitValue();
        ret->setType(tt);
        if (isNetDecl)
            ret->setValue(bit_z);
        else
            ret->setValue(bit_x);
        return ret;
    } else if (dynamic_cast<Time *>(type) != nullptr) {
        Time *tt = static_cast<Time *>(type);
        tt       = hif::copy(tt);
        tt->setConstexpr(true);
        ;
        TimeValue *ret = new TimeValue();
        ret->setValue(0.0);
        ret->setType(tt);
        return ret;
    } else if (dynamic_cast<Array *>(type) != nullptr) {
        Array *tt   = static_cast<Array *>(type);
        Value *vRec = getTypeDefaultValue(tt->getType(), d);
        if (vRec == nullptr)
            return nullptr;
        // create aggregate
        Aggregate *ao = new Aggregate();
        ao->setOthers(vRec);
        return ao;
    } else if (dynamic_cast<Bitvector *>(type) != nullptr) {
        Bitvector *tt = static_cast<Bitvector *>(type);
        // it is a logic vector
        // get the range size
        Range *range  = tt->getSpan();
        if (range == nullptr)
            return nullptr;

        std::uint64_t size = hif::semantics::spanGetBitwidth(range, this);
        if (size == 0) {
            // Failed to determine range: create an Aggregate with others = 'X'/'Z'
            BitValue *bit = new BitValue();
            if (isNetDecl)
                bit->setValue(bit_z);
            else
                bit->setValue(bit_x);

            Bit *b = _factory.bit(tt->isLogic(), tt->isResolved(), true);
            bit->setType(b);
            Aggregate *agg = new Aggregate();
            agg->setOthers(bit);
            return agg;
        }
        // build a string made of the right number of 'X'/'Z'
        std::string ret;
        ret.reserve(std::string::size_type(size + 1));
        for (std::uint64_t i = 0; i < size; ++i) {
            if (isNetDecl)
                ret.push_back('Z');
            else
                ret.push_back('X');
        }
        return _makeVerilogBitVectorValue(ret.c_str());
    } else if (dynamic_cast<Record *>(type) != nullptr) {
        Record *rec = static_cast<Record *>(type);

        RecordValue *recVal = new RecordValue();
        for (BList<Field>::iterator i = rec->fields.begin(); i != rec->fields.end(); ++i) {
            RecordValueAlt *rva = new RecordValueAlt();
            rva->setName((*i)->getName());
            Value *v = getTypeDefaultValue((*i)->getType(), d);
            messageAssert(v != nullptr, "Unepected case", v, this);
            rva->setValue(v);
            recVal->alts.push_back(rva);
        }

        return recVal;
    } else if (dynamic_cast<ViewReference *>(type) != nullptr) {
        return nullptr;
    } else if (dynamic_cast<Event *>(type) != nullptr) {
        return nullptr;
    } else if (dynamic_cast<String *>(type) != nullptr) {
        return _factory.stringval("", false, _factory.string(true));
    } else if (dynamic_cast<TypeReference *>(type) != nullptr) {
        Type *bt = hif::semantics::getBaseType(type, false, this, false);
        if (dynamic_cast<TypeReference *>(bt) != nullptr) {
            // don't know what's the correct init val
            messageError("Unexpected base type", bt, this);
        }

        // call this function recursively
        Value *v = getTypeDefaultValue(bt, d);
        if (dynamic_cast<ConstValue *>(v) != nullptr) {
            ConstValue *local = static_cast<ConstValue *>(v);
            local->setType(hif::copy(type));
        }
        return v;
    } else {
        messageError("Unexpected type", type, this);
    }
}
bool VerilogSemantics::isSupported(Operator operation)
{
    switch (operation) {
    case op_none:
        //case op_pow:
    case op_ref:
        //case op_deref: // supported only for type event as event trigger
    default:
        return true;
    }
}
Type *VerilogSemantics::getMapForType(Type *t)
{
    if (t == nullptr)
        return nullptr;

    VerilogTypeVisitor v;
    t->acceptVisitor(v);
    return v.getResult();
}
Operator VerilogSemantics::getMapForOperator(
    Operator srcOperation,
    Type * /*srcT1*/,
    Type * /*srcT2*/,
    Type * /*dstT1*/,
    Type * /*dstT2*/)
{
    return srcOperation;
}
Type *VerilogSemantics::getSuggestedTypeForOp(Type *t, Operator, Type * /*opType*/, Object *, bool /*isOp1*/)
{
    return hif::copy(t);
}
Type *VerilogSemantics::getTypeForConstant(ConstValue *c)
{
    if (dynamic_cast<IntValue *>(c))
        return _makeVerilogRegisterType(new Range(31, 0), true, true);

    if (dynamic_cast<BitValue *>(c) || dynamic_cast<BoolValue *>(c))
        return _makeVerilogBitType(true);

    if (dynamic_cast<BitvectorValue *>(c)) {
        BitvectorValue *bv = static_cast<BitvectorValue *>(c);
        Range *range       = new Range(static_cast<std::int64_t>(bv->getValue().size() - 1U), 0);
        return _makeVerilogRegisterType(range, true);
    }

    if (dynamic_cast<RealValue *>(c)) {
        Real *r = new Real();
        r->setSpan(new Range(63, 0));
        r->setConstexpr(true);
        return r;
    }

    if (dynamic_cast<StringValue *>(c)) {
        String *s = new String();
        return s;
    }

    if (dynamic_cast<TimeValue *>(c)) {
        Time *t = new Time();
        return t;
    }
    // Note: Boolean and Char types does not exist in Verilog
    messageError("Unexpected constant", c, this);
}
bool VerilogSemantics::checkCondition(Type *t, Object * /*o*/)
{
    // Verilog supports bit, integer, register and real as condition guard
    // TODO: is this getSemanticType correct?
    if (t == nullptr)
        return false;

    if (dynamic_cast<Bitvector *>(t) != nullptr)
        return true;

    Real *real = dynamic_cast<Real *>(t);
    if (real != nullptr)
        return _checkReal(real);

    return dynamic_cast<Bit *>(t) != nullptr;
}
Value *VerilogSemantics::explicitBoolConversion(Value *c)
{
    Type *type = getSemanticType(c, this);
    if (type == nullptr)
        return nullptr;
    type = getBaseType(type, false, this, false);
    if (type == nullptr)
        return nullptr;

    if (!checkCondition(type, nullptr)) {
        return nullptr;
    }

    // if it is bit, replace it with (bit == 1)
    if (dynamic_cast<Bit *>(type)) {
        BitValue *bvo = new BitValue(bit_one);
        Bit *b_type   = _makeVerilogBitType(true);
        bvo->setType(b_type);
        return new Expression(op_case_eq, hif::copy(c), bvo);
    }
    // if it is real, replace it with (real != 0.0)
    else if (dynamic_cast<Real *>(type)) {
        IntValue int1(0);
        Value *cost1 = hif::manipulation::transformConstant(&int1, type, this);
        if (!cost1)
            return nullptr;
        return new Expression(op_case_neq, hif::copy(c), cost1);
    }
    // if it is array, replace it with ( or_reduce (array) == '1' )
    // if it is integer, replace it with ( or_reduce (integer) == '1' )
    else {
        Expression *reduce = new Expression(op_orrd, hif::copy(c));
        Bit *b_type        = _makeVerilogBitType(true);
        BitValue *b        = new BitValue(bit_one);
        b->setType(b_type);
        return new Expression(op_case_eq, reduce, b);
    }
}
Value *VerilogSemantics::explicitCast(Value *valueToCast, Type *castType, Type * /*srcType*/)
{
    // For the moment, just manaing cast to bit types:
    if (dynamic_cast<Bit *>(castType)) {
        if (dynamic_cast<Bool *>(valueToCast->getSemanticType())) {
            // No cast required, since bools are mapped to bits.
            return hif::copy(valueToCast);
        } else if (dynamic_cast<Bit *>(valueToCast->getSemanticType())) {
            // No cast required, since bits are mapped to logics.
            return hif::copy(valueToCast);
        }
    } else if (dynamic_cast<Bitvector *>(castType)) {
        if (dynamic_cast<Bitvector *>(valueToCast->getSemanticType())) {
            // No cast required: it must be a bitvector casted to a logicvector,
            // but bitvectors are mapped to logicvectors.
            return hif::copy(valueToCast);
        }
    }

    // default case
    Cast *ret = new Cast();
    ret->setValue(hif::copy(valueToCast));
    ret->setType(hif::copy(castType));
    return ret;
}
std::int64_t VerilogSemantics::transformRealToInt(const double v)
{
    // TODO check, this is c++ like
    return static_cast<std::int64_t>(v);
}
Type *VerilogSemantics::isTypeAllowedAsBound(Type *t)
{
    messageAssert(t != nullptr, "Passed nullptr type", nullptr, this);

    Bit *b     = dynamic_cast<Bit *>(t);
    Array *arr = dynamic_cast<Array *>(t);

    if (b != nullptr || arr != nullptr)
        return nullptr;

    return _makeVerilogRegisterType(hif::copy(hif::typeGetSpan(t, this)), true, false); // assuming unsigned
}

bool VerilogSemantics::isCastAllowed(Type * /*target*/, Type * /*source*/) { return false; }

bool VerilogSemantics::isTypeAllowed(Type *t)
{
    if (t == nullptr)
        return false;
    if (dynamic_cast<Signed *>(t) != nullptr || dynamic_cast<Unsigned *>(t) != nullptr ||
        dynamic_cast<Bool *>(t) != nullptr || dynamic_cast<Int *>(t) != nullptr ||
        dynamic_cast<Pointer *>(t) != nullptr || dynamic_cast<Reference *>(t) != nullptr) {
        return false;
    }

    if (dynamic_cast<File *>(t) != nullptr) {
        File *f    = static_cast<File *>(t);
        String *bt = dynamic_cast<String *>(hif::semantics::getBaseType(f->getType(), false, this, false));
        return (bt != nullptr);
    }

    // only logic and resolved bits
    if (dynamic_cast<Bit *>(t) != nullptr) {
        Bit *b = static_cast<Bit *>(t);
        return b->isLogic() && b->isResolved();
    }

    Type::TypeVariant tv = t->getTypeVariant();
    if (tv != Type::NATIVE_TYPE) {
        return false;
    }

    return true;
}

bool VerilogSemantics::isRangeDirectionAllowed(RangeDirection r) { return r == dir_downto || r == dir_upto; }

bool VerilogSemantics::isTypeAllowedAsCase(Type *t)
{
    // Standard: expression is required to be a constant expression
    return isTemplateAllowedType(t);
}

bool VerilogSemantics::isTypeAllowedAsPort(Type *t) { return isTypeAllowed(t); }

bool VerilogSemantics::isLanguageIdAllowed(LanguageID id) { return id == hif::rtl; }

bool VerilogSemantics::isValueAllowedInPortBinding(Value * /*o*/)
{
    // TODO: Check if is allowed casts, slices, etc.
    return true;
}

bool VerilogSemantics::isTypeAllowedForConstValue(ConstValue *cv, Type *synType)
{
    Type *dt = this->getTypeForConstant(cv);
    hif::EqualsOptions opt;
    opt.checkOnlyTypes    = true;
    opt.handleVectorTypes = true;

    bool res = hif::equals(dt, synType, opt);
    delete dt;

    return res;
}
// ---------------- map implementations --------------------
namespace
{ // anon namespace

VerilogAnalysis::VerilogAnalysis(Operator currOperator, Object *srcObj)
    : SemanticAnalysis(VerilogSemantics::getInstance(), currOperator, srcObj)
    , BiVisitor<VerilogAnalysis>()
    , _verilogSem(VerilogSemantics::getInstance())
{
    // ntd
}

VerilogAnalysis::~VerilogAnalysis()
{
    // ntd
}

void VerilogAnalysis::analyzeOperands(Type *op1Type, Type *op2Type)
{
    SemanticAnalysis::analyzeOperands(op1Type, op2Type);

    if (_currOperator == op_concat || hif::operatorIsAssignment(_currOperator) || _result.returnedType == nullptr) {
        return;
    }

    //std::int64_t size = spanGetSize();
    Range *contextSpan = _sem->getContextPrecision(_srcObj);
    if (contextSpan != nullptr) {
        if (dynamic_cast<Bit *>(_result.returnedType) != nullptr && !_isLogical(_currOperator) &&
            !hif::operatorIsBitwise(_currOperator) && !_isReduce(_currOperator) && !_isRelational(_currOperator)) {
            Bit *b        = static_cast<Bit *>(_result.returnedType);
            Bitvector *bv = new Bitvector();
            bv->setConstexpr(b->isConstexpr());
            bv->setLogic(true);
            bv->setResolved(true);
            delete _result.returnedType;
            _result.returnedType = bv;
        }
        typeSetSpan(_result.returnedType, hif::copy(contextSpan), _sem, true);

        if (dynamic_cast<Bit *>(_result.operationPrecision) != nullptr && !_isLogical(_currOperator) &&
            !hif::operatorIsBitwise(_currOperator) && !_isReduce(_currOperator) && !_isRelational(_currOperator)) {
            Bit *b        = static_cast<Bit *>(_result.returnedType);
            Bitvector *bv = new Bitvector();
            bv->setConstexpr(b->isConstexpr());
            bv->setLogic(true);
            bv->setResolved(true);
            delete _result.operationPrecision;
            _result.operationPrecision = bv;
        }

        typeSetSpan(_result.operationPrecision, contextSpan, _sem, true);
    }
}

void VerilogAnalysis::map(Object *)
{
    // ntd
}

void VerilogAnalysis::map(Array *array)
{
    if (!_isUnary(_currOperator))
        // error, non-unary operator
        return;

    if (_isLogical(_currOperator))
        _result.returnedType = _makeVerilogBitType();
    else if (_isReduce(_currOperator))
        _result.returnedType = _makeVerilogBitType();
    else
        _result.returnedType = hif::copy(array);
    _result.operationPrecision = hif::copy(array);
}
void VerilogAnalysis::map(Bitvector *array)
{
    if (!_isUnary(_currOperator))
        // error, non-unary operator
        return;

    if (_isLogical(_currOperator))
        _result.returnedType = _makeVerilogBitType();
    else if (_isReduce(_currOperator))
        _result.returnedType = _makeVerilogBitType();
    else
        _result.returnedType = hif::copy(array);
    _result.operationPrecision = hif::copy(array);
}
void VerilogAnalysis::map(Bit *b)
{
    if (!_isUnary(_currOperator))
        // error, non-unary operator
        return;

    if (_isLogical(_currOperator))
        _result.returnedType = _makeVerilogBitType();
    else
        _result.returnedType = hif::copy(b);
    _result.operationPrecision = hif::copy(b);
}
void VerilogAnalysis::map(Real *real)
{
    if (!_isUnary(_currOperator) || !_isPermittedOnReals(_currOperator))
        // error, non-unary operator or the operation is not permitted
        return;

    if (_isLogical(_currOperator))
        _result.returnedType = _makeVerilogBitType();
    else
        _result.returnedType = hif::copy(real);
    _result.operationPrecision = hif::copy(real);
}
void VerilogAnalysis::map(TypeReference *op) { _map(op); }

void VerilogAnalysis::map(Event *e)
{
    if (_currOperator != op_deref)
        return;

    _result.returnedType       = new Event();
    _result.operationPrecision = hif::copy(e);
}

void VerilogAnalysis::map(Object *, Object *)
{
    // ntd
}

void VerilogAnalysis::map(Array *array1, Array *array2)
{
    Range *range1 = array1->getSpan();
    Range *range2 = array2->getSpan();
    if (!range1 || !range2)
        // failed to get the ranges
        return;

    // build the return range
    Range *result_range;
    // if the operator is the concatenation
    if (_currOperator == op_concat)
        // the result range is the sum of the two ranges
        result_range = rangeGetSum(range1, range2, VerilogSemantics::getInstance());
    else if (hif::operatorIsAssignment(_currOperator))
        result_range = hif::copy(range1);
    else
        // get the maximum between the two ranges
        result_range = rangeGetMax(range1, range2, VerilogSemantics::getInstance());

    // if the range cannot be established error
    if (!result_range)
        return;

    // build the array with the maximum range
    Array *array_result = new Array();
    array_result->setSpan(result_range);

    // check the Array types
    Type *type1 = array1->getType();
    Type *type2 = array2->getType();
    if (!type1 || !type2) {
        // types not present, error
        delete array_result;
        return;
    }
    // if the types are the same, the result type is the same
    hif::EqualsOptions opt;
    opt.checkSpans = false;
    if (hif::equals(type1, type2, opt)) {
        Type *rtype = hif::copy(type1);
        array_result->setType(rtype);
    } else if (hif::operatorIsAssignment(_currOperator) && dynamic_cast<Real *>(type1) != nullptr) {
        array_result->setType(hif::copy(type1));
    } else {
        // the only other possibility is to check if they are arrays of
        // different bit types (logic and not logic)
        Bit *bit1 = dynamic_cast<Bit *>(type1);
        Bit *bit2 = dynamic_cast<Bit *>(type2);
        if (!bit1 || !bit2) {
            // they are not bits! error
            delete array_result;
            return;
        }
        // they are both bits and they are different.
        // there must be a logic and a not logic bit
        array_result->setType(_makeVerilogBitType());
    }

    // check the signed attribute. If there is almost one signed, the output
    // is signed, otherwise is unsigned.
    array_result->setSigned(array1->isSigned() && array2->isSigned());

    // determine the return type
    if (_isRelational(_currOperator) || _isLogical(_currOperator))
        // if the operation is relational the return type is a bit
        _result.returnedType = _makeVerilogBitType();
    else
        // the returned type is the maximum of the two sizes
        _result.returnedType = hif::copy(array_result);

    // set the operation precision
    _result.operationPrecision = array_result;
}
void VerilogAnalysis::map(Bitvector *array1, Bitvector *array2)
{
    Range *range1 = array1->getSpan();
    Range *range2 = array2->getSpan();
    if (range1 == nullptr || range2 == nullptr)
        // failed to get the ranges
        return;

    // build the return range
    Range *result_range;
    // if the operator is the concatenation
    if (_currOperator == op_concat)
        // the result range is the sum of the two ranges
        result_range = rangeGetSum(range1, range2, VerilogSemantics::getInstance());
    else
        // get the maximum between the two ranges
        result_range = rangeGetMax(range1, range2, VerilogSemantics::getInstance());

    // if the range cannot be established error
    if (result_range == nullptr)
        return;

    // build the array with the maximum range
    Bitvector *bv_result = new Bitvector();
    bv_result->setSpan(result_range);
    bv_result->setResolved(true);
    bv_result->setLogic(true);
    bv_result->setConstexpr(array1->isConstexpr() && array2->isConstexpr());

    // check the signed attribute. If both are signed, the output
    // is signed, otherwise is unsigned.
    bv_result->setSigned(array1->isSigned() && array2->isSigned());

    // determine the return type
    if (_isRelational(_currOperator) || _isLogical(_currOperator))
        // if the operation is relational the return type is a bit
        _result.returnedType = _makeVerilogBitType();
    else
        // the returned type is the maximum of the two sizes
        _result.returnedType = hif::copy(bv_result);

    // set the operation precision
    _result.operationPrecision = bv_result;
}

void VerilogAnalysis::map(Array *array, Bitvector *bv)
{
    auto arr = dynamic_cast<Array *>(getBaseType(array, false, _sem, true));
    if (arr == nullptr)
        return;
    auto bit = dynamic_cast<Bit *>(arr->getType());
    if (bit == nullptr)
        return;
    Bitvector tmp;
    tmp.setSigned(arr->isSigned());
    tmp.setSpan(hif::copy(arr->getSpan()));
    tmp.setLogic(bit->isLogic());
    tmp.setResolved(bit->isResolved());
    map(&tmp, bv);
}
void VerilogAnalysis::map(Bit * /*bit1*/, Bit * /*bit2*/)
{
    // build the precision
    Bit *ret_bit = _makeVerilogBitType();

    // set return and precision
    if (_isRelational(_currOperator) || _isLogical(_currOperator))
        _result.returnedType = _makeVerilogBitType();
    else if (_currOperator == op_concat)
        _result.returnedType = _makeVerilogRegisterType(new Range(1, 0));
    else
        _result.returnedType = hif::copy(ret_bit);
    _result.operationPrecision = ret_bit;
}
void VerilogAnalysis::map(Array *array, Bit *bit)
{
    if (!array->getSpan())
        return;

    Array *other = new Array();
    Range *range = new Range(0, 0); // use std::int64_t
    range->setDirection(array->getSpan()->getDirection());
    other->setSpan(range);
    other->setType(hif::copy(bit));
    other->setSigned(false);
    map(array, other);
    delete other;
}
void VerilogAnalysis::map(Bit *bit, Array *array) { map(array, bit); }

void VerilogAnalysis::map(Bitvector *bv, Array *array2) { map(array2, bv); }
void VerilogAnalysis::map(Bitvector *array, Bit *bit)
{
    if (array->getSpan() == nullptr)
        return;

    Bitvector *other = new Bitvector();
    Range *range     = new Range(0, 0); // use std::int64_t
    range->setDirection(array->getSpan()->getDirection());
    other->setSpan(range);
    other->setSigned(false);
    other->setResolved(bit->isResolved());
    other->setLogic(true);
    other->setConstexpr(bit->isConstexpr());
    map(array, other);
    delete other;
}
void VerilogAnalysis::map(Bit *bit, Bitvector *array) { map(array, bit); }
void VerilogAnalysis::map(Real *real1, Real *real2)
{
    // check the ranges
    if (!_checkReal(real1) || !_checkReal(real2))
        // non-conformal real types
        return;
    // check the operation
    if (!_isPermittedOnReals(_currOperator))
        // not allowed
        return;

    // returned type
    if (_isRelational(_currOperator) || _isLogical(_currOperator))
        _result.returnedType = _makeVerilogBitType();
    else {
        Real *r = new Real();
        r->setSpan(new Range(63, 0));
        _result.returnedType = r;
    }
    // operation precision
    Real *rr = new Real();
    rr->setSpan(new Range(63, 0));
    _result.operationPrecision = rr;
}
void VerilogAnalysis::map(Array *, Real *real)
{
    Real *real1 = new Real();
    real1->setSpan(new Range(63, 0));
    map(real1, real);
    delete real1;
}
void VerilogAnalysis::map(Real *real, Array *)
{
    Real *real2 = new Real();
    real2->setSpan(new Range(63, 0));
    map(real, real2);
    delete real2;
}

void VerilogAnalysis::map(ViewReference *vr, Array *array)
{
    if (_currOperator != op_concat)
        return;

    ViewReference *arrT = dynamic_cast<ViewReference *>(array->getType());
    if (arrT == nullptr)
        return;

    // at the moment allow only same type concat.
    if (!hif::equals(vr, arrT))
        return;

    // works only with disciplines
    ViewReference::DeclarationType *d1 = hif::semantics::getDeclaration(vr, VerilogSemantics::getInstance());
    ViewReference::DeclarationType *d2 = hif::semantics::getDeclaration(arrT, VerilogSemantics::getInstance());
    if (d1 == nullptr || d2 == nullptr || d1 != d2)
        return;

    LibraryDef *ld1 = hif::getNearestParent<LibraryDef>(d1);
    if (ld1 == nullptr)
        return;
    if (ld1->getName() != "vams_disciplines")
        return;

    Array *res_array = hif::copy(array);
    Range *r         = hif::manipulation::rangeGetIncremented(res_array->getSpan(), VerilogSemantics::getInstance());
    delete res_array->setSpan(r);
    _result.returnedType       = res_array;
    _result.operationPrecision = hif::copy(res_array);
}

void VerilogAnalysis::map(Bit *, Real *real)
{
    Real *real1 = new Real();
    real1->setSpan(new Range(63, 0));
    map(real1, real);
    delete real1;
}
void VerilogAnalysis::map(Real *real, Bit *)
{
    Real *real2 = new Real();
    real2->setSpan(new Range(63, 0));
    map(real, real2);
    delete real2;
}

void VerilogAnalysis::map(TypeReference *op1, TypeReference *op2) { _map(op1, op2, false); }

void VerilogAnalysis::map(TypeReference *op1, Type *op2) { _map(op1, op2); }

void VerilogAnalysis::map(Type *op1, TypeReference *op2) { _map(op1, op2); }

void VerilogAnalysis::map(ViewReference *t1, ViewReference *t2)
{
    if (!hif::semantics::isSubType(t2, t1, VerilogSemantics::getInstance()))
        return;

    if (hif::operatorIsAssignment(_currOperator)) {
        _result.returnedType       = hif::copy(t1);
        _result.operationPrecision = hif::copy(t1);
    } else if (_currOperator == op_concat) {
        // at the moment allow only same type concat.
        if (!hif::equals(t1, t2))
            return;

        // works only with disciplines
        ViewReference::DeclarationType *d1 = hif::semantics::getDeclaration(t1, VerilogSemantics::getInstance());
        ViewReference::DeclarationType *d2 = hif::semantics::getDeclaration(t2, VerilogSemantics::getInstance());
        if (d1 == nullptr || d2 == nullptr || d1 != d2)
            return;

        LibraryDef *ld1 = hif::getNearestParent<LibraryDef>(d1);
        if (ld1 == nullptr)
            return;
        if (ld1->getName() != "vams_disciplines")
            return;

        Array *res_array = new Array();
        res_array->setSpan(new Range(1, 0));
        res_array->setType(hif::copy(t1));
        res_array->setSigned(false);
        _result.returnedType       = res_array;
        _result.operationPrecision = hif::copy(t1);
    }
}

void VerilogAnalysis::map(Array *array, ViewReference *vr) { map(vr, array); }

void VerilogAnalysis::_callMap(Object *op1) { callMap(op1); }

void VerilogAnalysis::_callMap(Object *op1, Object *op2) { callMap(op1, op2); }

void VerilogAnalysis::map(Bitvector *bv, Real *real)
{
    // check the ranges
    if (!_checkReal(real))
        // non-conformal real types
        return;

    // check the operation
    if (!_isPermittedOnReals(_currOperator))
        // not allowed
        return;

    // returned type
    if (hif::operatorIsAssignment(_currOperator)) {
        _result.returnedType = hif::copy(bv);
    } else if (_isRelational(_currOperator)) {
        _result.returnedType = _makeVerilogBitType();
    } else if (_isArithmetic(_currOperator)) {
        _result.returnedType = hif::copy(real);
    }

    // operation precision
    Real *rr = new Real();
    rr->setSpan(new Range(63, 0));
    _result.operationPrecision = rr;
}

void VerilogAnalysis::map(Real *real, Bitvector *bv)
{
    // check the ranges
    if (!_checkReal(real))
        // non-conformal real types
        return;

    // returned type
    if (hif::operatorIsAssignment(_currOperator)) {
        _result.returnedType = hif::copy(real);
        // operation precision
        Real *rr             = new Real();
        rr->setSpan(new Range(63, 0));
        _result.operationPrecision = rr;
        return;
    } else {
        map(bv, real);
    }
}

void VerilogAnalysis::map(String *s, Bitvector *bv)
{
    if (hif::operatorIsAssignment(_currOperator)) {
        _result.returnedType       = hif::copy(s);
        _result.operationPrecision = hif::copy(s);
        return;
    }

    /*
     * 3.6 Strings
     * Strings used as operands in expressions and assignments shall be treated
     * as unsigned integer constants represented by a sequence of 8-bit ASCII values,
     * with one 8-bit ASCII value representing one character.
     */
    Bitvector bvt;
    bvt.setSpan(new Range(31, 0));
    bvt.setLogic(true);
    ;
    bvt.setResolved(true);
    map(&bvt, bv);
}

void VerilogAnalysis::map(String *s1, String * /*s2*/)
{
    if (!hif::operatorIsAssignment(_currOperator))
        return;

    _result.returnedType       = hif::copy(s1);
    _result.operationPrecision = hif::copy(s1);
}

void VerilogAnalysis::map(Bitvector *bv, String * /*s*/)
{
    /*
     * 3.6 Strings
     * Strings used as operands in expressions and assignments shall be treated
     * as unsigned integer constants represented by a sequence of 8-bit ASCII values,
     * with one 8-bit ASCII value representing one character.
     */
    Bitvector bvt;
    bvt.setSpan(new Range(31, 0));
    bvt.setLogic(true);
    ;
    bvt.setResolved(true);
    map(bv, &bvt);
}

void VerilogAnalysis::map(Time *t1, Time * /*t2*/)
{
    if (!hif::operatorIsAssignment(_currOperator))
        return;

    _result.returnedType       = hif::copy(t1);
    _result.operationPrecision = hif::copy(t1);
}

void VerilogAnalysis::map(Time * /*t1*/, Real * /*r*/)
{
    if (_currOperator != op_mult && _currOperator != op_div)
        return;

    _result.returnedType       = new Time();
    _result.operationPrecision = new Time();
}

void VerilogAnalysis::map(Real *r, Time *t2) { map(t2, r); }

void VerilogAnalysis::map(Time * /*t1*/, Bitvector * /*bv*/)
{
    if (_currOperator != op_mult && _currOperator != op_div)
        return;

    _result.returnedType       = new Time();
    _result.operationPrecision = new Time();
}

void VerilogAnalysis::map(Bitvector *bv, Time *t2) { map(t2, bv); }

// ---------------- VerilogTypeMapper_t --------------------
VerilogTypeVisitor::VerilogTypeVisitor()
    : HifVisitor()
    , _sem(VerilogSemantics::getInstance())
    , _factory(_sem)
    , _result(nullptr)
{
    // ntd
}

VerilogTypeVisitor::~VerilogTypeVisitor()
{
    // ntd
}

Type *VerilogTypeVisitor::getResult() { return _result; }

int VerilogTypeVisitor::visitBool(Bool & /*o*/)
{
    _result = _makeVerilogBitType();
    return 0;
}

int VerilogTypeVisitor::visitBit(Bit & /*o*/)
{
    _result = _makeVerilogBitType();
    return 0;
}

int VerilogTypeVisitor::visitInt(Int &o)
{
    Range *range = o.getSpan();
    if (range == nullptr) {
        // no range, make a 32 bit
        _result = _makeVerilogRegisterType(new Range(31, 0), o.isConstexpr(), true);
    } else {
        //        std::uint64_t size = hif::semantics::spanGetBitwidth(range, _sem, false);
        //        if (size != 0)
        //        {
        //            // less than 32 bits or greater than 32 bits, build a bit array
        //            Bitvector* array = _makeVerilogRegisterType (
        //                        new Range (static_cast<std::int64_t>(size - 1), 0));
        //            array->setSigned(o.isSigned());
        //            _result = array;
        //        }
        //        else
        //        {
        Bitvector *array = _makeVerilogRegisterType(hif::copy(o.getSpan()));
        array->setSigned(o.isSigned());
        _result = array;
        //        }
    }
    return 0;
}

int VerilogTypeVisitor::visitReal(Real &o)
{
    //    Range* range = o.getSpan();
    //    if (range == nullptr ||
    //        hif::semantics::spanGetBitwidth(range, _sem) <= 64)
    //    {
    //        Real * real1 = new Real();
    //        real1->setSpan(new Range(63,0));
    //        _result = real1;
    //    }
    //    else
    //    {
    //        _result = nullptr;
    //    }
    _result = hif::copy(&o);
    return 0;
}

int VerilogTypeVisitor::visitUnsigned(Unsigned &o)
{
    hif::CopyOptions opt;
    Range *range = hif::copy(o.getSpan(), opt);
    _result      = _makeVerilogRegisterType(range);
    return 0;
}

int VerilogTypeVisitor::visitSigned(Signed &o)
{
    hif::CopyOptions opt;
    Range *range     = hif::copy(o.getSpan(), opt);
    Bitvector *array = _makeVerilogRegisterType(range);
    array->setSigned(true);
    _result = array;
    return 0;
}

int VerilogTypeVisitor::visitArray(Array &o)
{
    Type *type = o.getType();
    hif::CopyOptions opt;

    // call the map recursively
    Type *mappedType = _sem->getMapForType(type);
    if (mappedType != nullptr) {
        Array *ret_array = new Array();
        ret_array->setSpan(hif::copy(o.getSpan(), opt));
        ret_array->setSigned(o.isSigned());
        ret_array->setType(mappedType);
        _result = ret_array;
    } else {
        _result = nullptr;
    }
    return 0;
}

int VerilogTypeVisitor::visitBitvector(Bitvector &o)
{
    hif::CopyOptions opt;
    Bitvector *retBv = hif::copy(&o, opt);
    retBv->setLogic(true);
    retBv->setResolved(true);
    _result = retBv;
    return 0;
}

int VerilogTypeVisitor::visitString(String &o)
{
    hif::CopyOptions opt;
    _result = hif::copy(&o, opt);
    return 0;
}

int VerilogTypeVisitor::visitTime(Time &o)
{
    hif::CopyOptions opt;
    _result = hif::copy(&o, opt);
    return 0;
}

int VerilogTypeVisitor::visitTypeReference(TypeReference &o)
{
    // @TODO:
    // 1. Add previsitor to ensure TypeTp occurs only in standard def
    // 2. Check SystemVerilog vs Verilog: Verilog does not have TypeDef
    // 3. Templates make sense only in Views, not subprograms, TypeDes, etc.
    // 4. Evaluate if add here sanity checks on these cases
    _result = hif::copy(&o);
    return 0;
}

// ------------------------ helper functions --------------------------

bool _isRelational(Operator operation)
{
    return hif::operatorIsRelational(operation) || hif::operatorIsAssignment(operation);
}
bool _isArithmetic(Operator operation)
{
    return (hif::operatorIsArithmetic(operation) || operation == op_concat) && operation != op_abs &&
           operation != op_rem && operation != op_log;
}
bool _isLogical(Operator operation) { return hif::operatorIsLogical(operation); }
bool _isUnary(Operator operation) { return hif::operatorIsUnary(operation); }
bool _isReduce(Operator operation) { return hif::operatorIsReduce(operation); }
Bit *_makeVerilogBitType(bool isConstexpr)
{
    Bit *b = new Bit();
    b->setLogic(true);
    b->setResolved(true);
    b->setConstexpr(isConstexpr);
    return b;
}
Bitvector *_makeVerilogRegisterType(Range *range, bool isConstexpr, bool isSigned)
{
    Bitvector *ret = new Bitvector();
    ret->setLogic(true);
    ret->setSpan(range);
    ret->setSigned(isSigned);
    ret->setConstexpr(isConstexpr);
    ret->setResolved(true);
    return ret;
}
BitvectorValue *_makeVerilogBitVectorValue(const std::string &value)
{
    BitvectorValue *bvo = new BitvectorValue(value);
    bvo->setType(VerilogSemantics::getInstance()->getTypeForConstant(bvo));
    return bvo;
}
bool _checkReal(Real *real)
{
    // check that the range is 64 bits
    Range *range = real->getSpan();
    return (!range || hif::semantics::spanGetBitwidth(range, VerilogSemantics::getInstance()) == 64);
}
bool _isPermittedOnReals(Operator operation)
{
    switch (operation) {
    case op_or:
    case op_and:
    case op_not:
    case op_eq:
    case op_neq:
    case op_case_eq:
    case op_case_neq:
    case op_le:
    case op_ge:
    case op_lt:
    case op_gt:
    case op_plus:
    case op_minus:
    case op_mult:
    case op_div:
    case op_assign:
    case op_conv:
    case op_bind:
    case op_pow:
        return true;
    default:
        return false;
    }
}

} // namespace
} // namespace semantics
} // namespace hif
