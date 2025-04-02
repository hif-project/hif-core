/// @file HIFSemantics.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <iostream>
#include <limits>
#include <stdint.h>

#include "hif/BiVisitor.hpp"
#include "hif/hif.hpp"
#include "hif/semantics/HIFSemantics.hpp"
#include "hif/semantics/SemanticAnalysis.hpp"

namespace hif
{
namespace semantics
{

namespace
{

class HIFAnalysis : public SemanticAnalysis, public BiVisitor<HIFAnalysis>
{
public:
    HIFAnalysis(Operator currOperator, Object *srcObj);
    virtual ~HIFAnalysis();

    /// @name Unary operators
    /// @{

    /// Base case for unary operators.
    void map(Object *);

    void map(Array *array);
    void map(Bitvector *array);
    void map(Bit *bit);
    void map(Bool *b);
    void map(Char *c);
    void map(Event *e);
    void map(Int *i);
    void map(Pointer *p);
    void map(Real *r);
    void map(Reference *r);
    void map(Signed *s);
    void map(String *s);
    void map(Unsigned *u);
    void map(TypeReference *op);

    /// @}
    /// @name Binary operators
    /// @{

    /// Base case for binary operators.
    void map(Object *op1, Object *op2);

    // Maps from Type
    void map(Type *op1, Int *op2);
    void map(Type *op1, TypeReference *op2);
    void map(Type *op1, Reference *op2);

    // Maps from Array
    void map(Array *op1, Array *op2);
    void map(Array *op1, Bit *op2);
    void map(Array *op1, Signed *op2);
    void map(Array *op1, Unsigned *op2);
    void map(Array *op1, Bitvector *op2);
    void map(Array *op1, Int *op2);
    void map(Array *op1, ViewReference *op2);

    // Maps from Bit
    void map(Bit *op1, Array *op2);
    void map(Bit *op1, Bit *op2);
    void map(Bit *op1, Bitvector *op2);
    void map(Bit *op1, Signed *op2);
    void map(Bit *op1, Unsigned *op2);

    // Maps from Bool
    void map(Bool *op1, Bitvector *op2);
    void map(Bool *op1, Bool *op2);

    // Maps from Bitvectpr
    void map(Bitvector *op1, Bool *op2);
    void map(Bitvector *op1, Bit *op2);
    void map(Bitvector *op1, Bitvector *op2);
    void map(Bitvector *op1, Signed *op2);
    void map(Bitvector *op1, Time *op2);
    void map(Bitvector *op1, Unsigned *op2);
    void map(Bitvector *op1, Array *op2);

    // Maps from Char
    void map(Char *op1, Char *op2);

    // Maps from Enum
    void map(Enum *op1, Enum *op2);

    // Maps from File
    void map(File *op1, File *op2);

    // Maps from Int
    void map(Int *op1, Array *op2);
    void map(Int *op1, Int *op2);
    void map(Int *op1, Time *op2);
    void map(Int *op1, Pointer *op2);

    // Maps from Pointer
    void map(Pointer *op1, Pointer *op2);
    void map(Pointer *op1, String *op2);
    void map(Pointer *op1, Int *op2);

    // Maps from Real
    void map(Real *op1, Real *op2);
    void map(Real *op1, Time *op2);

    // Maps from Record
    void map(Record *op1, Record *op2);

    // Maps from Reference
    void map(Reference *op1, Int *op2);
    void map(Reference *op1, Reference *op2);
    void map(Reference *op1, Type *op2);
    void map(Reference *op1, TypeReference *op2);

    // Maps from Signed
    void map(Signed *op1, Array *op2);
    void map(Signed *op1, Bit *op2);
    void map(Signed *op1, Bitvector *op2);
    void map(Signed *op1, Signed *op2);

    // Maps from String
    void map(String *op1, String *op2);
    void map(String *op1, Pointer *op2);

    // Maps from Time
    void map(Time *op1, Bitvector *op2);
    void map(Time *op1, Int *op2);
    void map(Time *op1, Real *op2);
    void map(Time *op1, Time *op2);

    // Maps from TypeReference
    void map(TypeReference *op1, Int *op2);
    void map(TypeReference *op1, Reference *op2);
    void map(TypeReference *op1, Type *op2);
    void map(TypeReference *op1, TypeReference *op2);

    // Maps from Unsigned
    void map(Unsigned *op1, Array *op2);
    void map(Unsigned *op1, Bit *op2);
    void map(Unsigned *op1, Bitvector *op2);
    void map(Unsigned *op1, Unsigned *op2);

    // Maps from ViewReference
    void map(ViewReference *op1, ViewReference *op2);
    void map(ViewReference *op1, Array *op2);

    /// @}

private:
    HIFSemantics *_hifSem;

    virtual void _callMap(Object *op1);
    virtual void _callMap(Object *op1, Object *op2);

    /// This function handles all the common operations with non boolean
    /// types. It checks that the operation is unary and not logical and
    /// sets the returned and precision types according to the current
    /// operation. If it is relational the returned type is boolean. It
    /// is a copy of t otherwise.
    void _baseMap(Type *t);

    /// This function handles all the common operations with non boolean
    /// types, where the two types are equal. It checks that the operation
    /// is not logical and sets the returned and precision types to t1.
    void _baseMap(Type *t1, Type *t2);

    /// Check that the operation is a shift or a rotate. The integer must
    /// not be signed. The return type is the first operand and the
    /// precision is the integer.
    void _baseShiftAndRotate(Type *t1, Int *i);

    // disabled.
    HIFAnalysis(const HIFAnalysis &);
    HIFAnalysis &operator=(const HIFAnalysis &);
};

//------------------------- helper functions ------------------------------

/// Returns true if the operator is relational (<, <=, >, >=, ==, !=).
bool _isRelationalOrAssignment(Operator operation);

/// Returns true if the operator is a shift or rotatel.
bool _isShiftOrRotate(Operator operation);

} // namespace
// ---------------- HIFSemantics --------------------

// Wrapper used to suppress useless warnings:
#ifdef __GNUC__
#    pragma GCC diagnostic ignored "-Wswitch-default"
#    pragma GCC diagnostic ignored "-Wswitch-enum"
#else
#    pragma warning(disable : 4127)
#    pragma warning(disable : 4244)
#    pragma warning(disable : 4505)
#endif
HIFSemantics::HIFSemantics()
    : ILanguageSemantics()
    , _stdMap()
{
    _hifFactory.setSemantics(this);

    _initForbiddenNames();
    _initStandardSymbols();
    _initStandardFilenames();

    // Initializing semantic check options.
    _semanticOptions.port_inNoInitialValue        = true;
    _semanticOptions.port_outInitialValue         = true;
    _semanticOptions.dataDeclaration_initialValue = true;
    _semanticOptions.scopedType_insideTypedef     = true;
    _semanticOptions.int_bitfields                = true;
    _semanticOptions.designUnit_uniqueView        = true; // To support
    _semanticOptions.for_implictIndex             = true;
    _semanticOptions.for_conditionType            = SemanticOptions::RANGE_AND_EXPRESSION;
    _semanticOptions.generates_isNoAllowed        = false;
    _semanticOptions.after_isNoAllowed            = false;
    _semanticOptions.with_isNoAllowed             = false;
    _semanticOptions.globact_isNoAllowed          = false;
    _semanticOptions.valueStatement_isNoAllowed   = false;
    _semanticOptions.case_isOnlyLiteral           = false;
    _semanticOptions.lang_signPortNoBitAccess     = false;
    _semanticOptions.lang_hasDontCare             = true;
    _semanticOptions.lang_has9logic               = true;
    _semanticOptions.waitWithActions              = true;
    _semanticOptions.lang_sortKind                = hif::manipulation::SortMissingKind::NOTHING;
}
HIFSemantics::~HIFSemantics()
{
    for (DeclarationMap::iterator i = _stdMap.begin(); i != _stdMap.end(); ++i) {
        for (SubList::iterator j = i->second.begin(); j != i->second.end(); ++j) {
            delete *j;
        }
    }
}
std::string HIFSemantics::getName() const { return "hif"; }
ILanguageSemantics::ExpressionTypeInfo
HIFSemantics::getExprType(Type *op1Type, Type *op2Type, Operator operation, Object *sourceObj)
{
    ILanguageSemantics::ExpressionTypeInfo ret;
    // common support for unary operator ref.
    if (operation == op_ref && op1Type != nullptr && op2Type == nullptr) {
        Pointer *p     = new Pointer();
        Reference *ref = dynamic_cast<Reference *>(op1Type);
        if (ref != nullptr) {
            p->setType(hif::copy(ref->getType()));
        } else {
            p->setType(hif::copy(op1Type));
        }

        ret.returnedType       = p;
        ret.operationPrecision = hif::copy(op1Type);
    }
    // common support for unary operator deref.
    else if (
        operation == op_deref && op1Type != nullptr && op2Type == nullptr &&
        dynamic_cast<Pointer *>(op1Type) != nullptr) {
        ret.returnedType       = hif::copy(static_cast<Pointer *>(op1Type)->getType());
        ret.operationPrecision = hif::copy(op1Type);
    } else {
        HIFAnalysis hifAnalysis(operation, sourceObj);
        hifAnalysis.analyzeOperands(op1Type, op2Type);
        ret = hifAnalysis.getResult();
    }

    return ret;
}
bool HIFSemantics::isTemplateAllowedType(Type * /*t*/)
{
    // all types are allowed
    return true;
}
Type *HIFSemantics::getTemplateAllowedType(Type *t) { return hif::copy(t); }
HIFSemantics *HIFSemantics::getInstance()
{
    static HIFSemantics instance;
    return &instance;
}
bool HIFSemantics::isForbiddenName(Declaration * /*decl*/) { return false; }
bool HIFSemantics::isSliceTypeRebased()
{
    return true;
    //    return false;
}

bool HIFSemantics::isSyntacticTypeRebased()
{
    //return true;
    return false;
}

Type *HIFSemantics::getMemberSemanticType(Member *m)
{
    Type *prefixType = getBaseType(m->getPrefix(), false, this, false);
    if (prefixType == nullptr)
        return nullptr;

    if (dynamic_cast<Pointer *>(prefixType) != nullptr) {
        Pointer *p = static_cast<Pointer *>(prefixType);
        return hif::copy(p->getType());
    }

    return ILanguageSemantics::getMemberSemanticType(m);
}

LibraryDef *HIFSemantics::getStandardPackage()
{
    bool hifFormat = true;
    LibraryDef *ld       = new LibraryDef();
    ld->setName(_makeHifName("hif_standard", hifFormat));
    ld->setStandard(true);
    ld->setLanguageID(hif::c);

    hif::HifFactory factory(this);

    // hif_string_size()
    SubProgram *hif_string_size = factory.subprogram(
        factory.integer(nullptr, false), _makeHifName("hif_string_size", hifFormat).c_str(), factory.noTemplates(),
        factory.noParameters());
    hif_string_size->setKind(SubProgram::IMPLICIT_INSTANCE);
    ld->declarations.push_back(hif_string_size);

    return ld;
}
Value *HIFSemantics::getTypeDefaultValue(Type *t, Declaration *d)
{
    Type *type = getBaseType(t, false, this, false);

    if (dynamic_cast<Array *>(type)) {
        Array *tt = static_cast<Array *>(type);
        if (tt->getType() == nullptr)
            return nullptr;
        // create aggregate
        Value *v = getTypeDefaultValue(tt->getType(), d);
        if (v == nullptr)
            return nullptr;
        Aggregate *ret = new Aggregate();
        ret->setOthers(v);
        return ret;
    } else if (dynamic_cast<Bitvector *>(type)) {
        Bitvector *tt = static_cast<Bitvector *>(type);

        // it is a bit vector or a logic vector. Its default value is "UU...U"
        Range *range = tt->getSpan();
        messageAssert(range != nullptr, "Unexpected bitvector", tt, nullptr);
        std::uint64_t size = spanGetBitwidth(range, this);
        if (size == 0) {
            // Failed to determine range: create an Aggregate with others = 'U'
            Aggregate *ret     = new Aggregate();
            BitValue *bitvalue = new BitValue();
            if (tt->isLogic() && !_useNativeSemantics)
                bitvalue->setValue(bit_u);
            else
                bitvalue->setValue(bit_zero);
            Bit *b = _factory.bit(tt->isLogic(), tt->isResolved(), true);
            bitvalue->setType(b);
            ret->setOthers(bitvalue);
            return ret;
        } else {
            // span size > 0: build the std_logic "00...0"
            std::string s;
            s.reserve(std::string::size_type(size + 1));
            char c = '0';
            if (tt->isLogic() && !_useNativeSemantics)
                c = 'u';

            for (std::uint64_t i = 0; i < size; ++i)
                s.push_back(c);
            BitvectorValue *ret = new BitvectorValue(s);
            tt                  = hif::copy(tt);
            tt->setConstexpr(true);
            ret->setType(tt);
            return ret;
        }
    } else if (dynamic_cast<Record *>(type)) {
        Record *tt = static_cast<Record *>(type);

        RecordValue *ret = new RecordValue();
        for (BList<Field>::iterator i = tt->fields.begin(); i != tt->fields.end(); ++i) {
            Value *v = getTypeDefaultValue((*i)->getType(), d);
            messageAssert(v != nullptr, "Not found default value for type", (*i)->getType(), this);
            RecordValueAlt *rva = new RecordValueAlt();
            rva->setName((*i)->getName());
            rva->setValue(v);

            ret->alts.push_back(rva);
        }
        return ret;
    } else if (dynamic_cast<Bit *>(type)) {
        // check if it's a bit or a std_logic
        Bit *tt = static_cast<Bit *>(type);
        tt      = hif::copy(tt);
        tt->setConstexpr(true);
        ;
        BitValue *ret = new BitValue();
        ret->setType(tt);
        if (tt->isLogic() && !_useNativeSemantics) {
            // it's a std_logic. It's initial value is "U"
            ret->setValue(bit_u);
        } else {
            // it's a bit. It's initial value is "0"
            ret->setValue(bit_zero);
        }
        return ret;
    } else if (dynamic_cast<Bool *>(type)) {
        // the initial value is false
        Bool *tt = static_cast<Bool *>(type);
        tt       = hif::copy(tt);
        tt->setConstexpr(true);
        BoolValue *ret = new BoolValue(false);
        ret->setType(tt);
        return ret;
    } else if (dynamic_cast<Time *>(type)) {
        // the initial value is false
        Time *tt = static_cast<Time *>(type);
        tt       = hif::copy(tt);
        tt->setConstexpr(true);
        ;
        TimeValue *ret = new TimeValue();
        ret->setValue(0.0);
        ret->setType(tt);
        return ret;
    } else if (dynamic_cast<Char *>(type)) {
        // the initial value is '\0'
        Char *tt = static_cast<Char *>(type);
        tt       = hif::copy(tt);
        tt->setConstexpr(true);
        CharValue *ret = new CharValue('\0');
        ret->setType(tt);
        return ret;
    } else if (dynamic_cast<Enum *>(type)) {
        Enum *eo         = static_cast<Enum *>(type);
        EnumValue *front = eo->values.front();
        return front ? new Identifier(front->getName()) : nullptr;
    } else if (dynamic_cast<Int *>(type)) {
        Int *tt = static_cast<Int *>(type);
        tt      = hif::copy(tt);
        tt->setConstexpr(true);
        IntValue *ret = new IntValue(0);
        ret->setType(tt);
        return ret;
    } else if (dynamic_cast<Real *>(type)) {
        Real *tt = static_cast<Real *>(type);
        tt       = hif::copy(tt);
        tt->setConstexpr(true);
        RealValue *ret = new RealValue(0.0);
        ret->setType(tt);
        return ret;
    } else if (dynamic_cast<Unsigned *>(type) || dynamic_cast<Signed *>(type)) {
        Range *span             = hif::typeGetSpan(type, this);
        std::uint64_t size = spanGetBitwidth(span, this);
        if (size == 0) {
            // Failed to determine range: create an Aggregate with others = 'U'
            Bit *b = new Bit();
            b->setLogic(true);
            b->setResolved(true);
            b->setConstexpr(true);
            BitValue *bit;
            if (!_useNativeSemantics)
                bit = new BitValue(bit_u);
            else
                bit = new BitValue(bit_zero);
            bit->setType(b);
            Aggregate *ret = new Aggregate();
            ret->setOthers(bit);
            return ret;
        } else {
            // span size > 0: build the std_logic "00...0"
            std::string s;
            s.reserve(std::string::size_type(size + 1));
            char c = 'u';
            if (_useNativeSemantics)
                c = '0';
            for (std::uint64_t i = 0; i < size; ++i)
                s.push_back(c);
            BitvectorValue *ret = new BitvectorValue(s);
            type                = hif::copy(type);
            hif::typeSetConstexpr(type, true);
            ret->setType(type);
            return ret;
        }
    } else if (dynamic_cast<TypeReference *>(type)) {
        // get the type def init value
        TypeReference *tt = static_cast<TypeReference *>(type);
        // We do not consider opacity
        Type *bt          = getBaseType(tt, false, this, false);

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
    } else if (dynamic_cast<File *>(type)) {
        StringValue *retConst = new StringValue();
        retConst->setValue("");
        String *tt = new String();
        tt->setConstexpr(true);
        retConst->setType(tt);

        Cast *ret = new Cast();
        ret->setType(hif::copy(type));
        ret->setValue(retConst);

        return ret;
    } else if (dynamic_cast<String *>(type)) {
        String *tt = static_cast<String *>(type);
        tt         = hif::copy(tt);
        tt->setConstexpr(true);
        StringValue *ret = new StringValue();
        ret->setValue("");
        ret->setType(tt);

        return ret;
    } else if (dynamic_cast<Pointer *>(type)) {
        return _factory.cast(hif::copy(type), _factory.intval(0));
    } else if (dynamic_cast<ViewReference *>(type) != nullptr) {
        auto vr   = static_cast<ViewReference *>(type);
        auto view = hif::semantics::getDeclaration(vr, this);
        if (view == nullptr)
            return nullptr;
        if (view->getLanguageID() != hif::cpp)
            return nullptr;
        // @TODO: check that:
        // - there is the default ctor
        // - otherwise there is only a single ctor and recursive call
        //   on its parameters --> consider possible default values
        // - in this phase, check against infinite loops (possibly)
        auto du = dynamic_cast<DesignUnit *>(view->getParent());
        if (du == nullptr)
            return nullptr;
        auto ctor = _factory.classConstructorCall(
            du, vr->getDesignUnit(), _factory.noParameterArguments(), _factory.noTemplateArguments());
        return ctor;
    } else if (dynamic_cast<Event *>(type) != nullptr) {
        return nullptr;
    }

    // otherwise not supported
    messageError("Unexpected type", type, this);
}
Type *HIFSemantics::getMapForType(Type *t)
{
    // the mapping is always the same type in HIF
    if (t == nullptr)
        return nullptr;

    hif::CopyOptions opt;
    if (dynamic_cast<Int *>(t)) {
        // check the range and the span
        Int *i      = static_cast<Int *>(hif::copy(t, opt));
        Range *span = i->getSpan();
        if (span == nullptr) {
            span = new Range(31, 0);
            i->setSpan(span);
        }

        return i;
    }
    Type *ret = hif::copy(t, opt);
    return ret;
}
Operator HIFSemantics::getMapForOperator(
    Operator srcOperation,
    Type * /*srcT1*/,
    Type * /*srcT2*/,
    Type * /*dstT1*/,
    Type * /*dstT2*/)
{
    return srcOperation;
}
Type *HIFSemantics::getSuggestedTypeForOp(Type *t, Operator operation, Type *opType, Object *, bool isOp1)
{
    if (operation == op_sll || operation == op_sla || operation == op_srl || operation == op_sra) {
        if (!isOp1) {
            // Int for rhv
            Int *io = new Int();
            io->setConstexpr(true);
            io->setSigned(false);
            io->setSpan(hif::copy(typeGetSpan(t, this)));
            return io;
        }
    } else if (operation == op_mult || operation == op_div) {
        if (dynamic_cast<Time *>(t) != nullptr) {
            if (dynamic_cast<Bitvector *>(opType) != nullptr) {
                Int *io = new Int();
                io->setConstexpr(true);
                io->setSigned(false);
                io->setSpan(new Range(63, 0));
                return io;
            }
        }
    }

    return hif::copy(t);
}
Type *HIFSemantics::getTypeForConstant(ConstValue *c)
{
    // Otherwise try to figure out the type
    if (dynamic_cast<IntValue *>(c) != nullptr) {
        IntValue *iv      = dynamic_cast<IntValue *>(c);
        int32_t i32 = int32_t(iv->getValue());
        int64_t i64 = int64_t(iv->getValue());
        std::int64_t left    = int64_t(i32) == i64 ? 31 : 63;
        Int *iType        = new Int();
        iType->setConstexpr(true);
        // if the value is less than zero we are sure that the given intval is
        // signed, otherwise we don'know
        iType->setSigned(true);
        // Force span to 32 bit
        iType->setSpan(new Range(new IntValue(left), new IntValue(0), dir_downto));
        return iType;
    } else if (dynamic_cast<BoolValue *>(c)) {
        Bool *bType = new Bool();
        bType->setConstexpr(true);
        return bType;
    } else if (dynamic_cast<BitValue *>(c)) {
        BitValue *bvo   = static_cast<BitValue *>(c);
        BitConstant val = static_cast<BitValue *>(c)->getValue();
        Bit *bt         = dynamic_cast<Bit *>(bvo->getType());
        Bit *bType      = nullptr;
        if (bt) {
            bType = hif::copy(bt);
            bType->setConstexpr(true);
        } else if (val != bit_one && val != bit_zero) {
            // if the value is not 1 or 0 we are sure that the given bitval
            // is resolved
            bType = new Bit();
            bType->setConstexpr(true);
            bType->setLogic(true);
        } else {
            bType = new Bit();
            bType->setConstexpr(true);
            bType->setLogic(false);
        }
        return bType;
    } else if (dynamic_cast<BitvectorValue *>(c)) {
        BitvectorValue *bv = static_cast<BitvectorValue *>(c);
        std::string val    = bv->getValue();
        // determine if it's logic
        bool logic         = false;
        for (std::string::iterator it = val.begin(); it != val.end(); ++it) {
            switch (*it) {
            case '0':
            case '1':
                break;
            default:
                logic = true;
                break;
            }
        }

        Range *ro      = new Range(static_cast<int>(bv->getValue().size() - 1), 0);
        Bitvector *ret = new Bitvector();
        ret->setSpan(ro);
        ret->setSigned(false);
        ret->setConstexpr(true);
        ret->setLogic(logic);
        ret->setResolved(true); // Arbitrary.
        return ret;
    } else if (dynamic_cast<CharValue *>(c)) {
        Char *cType = new Char();
        cType->setConstexpr(true);
        return cType;
    } else if (dynamic_cast<RealValue *>(c)) {
        Real *rType = new Real();
        rType->setConstexpr(true);
        rType->setSpan(new Range(63, 0));
        return rType;
    } else if (dynamic_cast<StringValue *>(c)) {
        String *sType = new String();
        sType->setConstexpr(true);
        return sType;
    } else if (dynamic_cast<TimeValue *>(c)) {
        Time *tType = new Time();
        tType->setConstexpr(true);
        return tType;
    }

    return nullptr;
}
bool HIFSemantics::checkCondition(Type *t, Object *o)
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

    // HIF supports only boolean type as guard condition
    return dynamic_cast<Bool *>(t) != nullptr;
}
Value *HIFSemantics::explicitBoolConversion(Value *c)
{
    // check if the value passed as condition is valid
    Type *type = getSemanticType(c, this);
    if (type == nullptr)
        return nullptr;
    type = getBaseType(type, false, this, false);
    if (type == nullptr)
        return nullptr;
    if (checkCondition(type, nullptr)) {
        return hif::copy(c);
    }

    // Here bool and typerefs cannot appear anymore

    // the first operand is the given value object
    Expression *cond = new Expression();
    cond->setValue1(hif::copy(c));

    // if it is bit (from bit), replace it with (bit == 1)
    if (dynamic_cast<Bit *>(type) != nullptr) {
        Bit *bit = static_cast<Bit *>(type);
        bit      = hif::copy(bit);
        bit->setConstexpr(true);
        BitValue *bvo = new BitValue(bit_one);
        bvo->setType(bit);

        cond->setValue2(bvo);
        cond->setOperator(op_case_eq);

        Cast *cc = new Cast();
        cc->setType(_factory.boolean());
        cc->setValue(cond);
        return cc;
    } else if (
        dynamic_cast<ViewReference *>(type) || dynamic_cast<File *>(type) || dynamic_cast<Library *>(type) ||
        dynamic_cast<Record *>(type) || dynamic_cast<Reference *>(type) || dynamic_cast<Time *>(type)) {
        messageError("Unexpected case.", type, this);
    } else if (dynamic_cast<Enum *>(type)) {
        Enum *e = static_cast<Enum *>(type);
        messageAssert(!e->values.empty(), "Unexpected case.", type, this);

        Identifier *id = new Identifier(e->values.front()->getName());
        setDeclaration(id, e->values.front());
        cond->setOperator(op_case_neq);
        cond->setValue2(id);
    } else if (dynamic_cast<String *>(type)) {
        cond->setOperator(op_case_neq);
        cond->setValue2(_factory.stringval(""));
    } else if (dynamic_cast<Array *>(type)) {
        delete cond;
        messageError("Unexpected case.", type, this);
    } else if (dynamic_cast<Bitvector *>(type) || dynamic_cast<Signed *>(type) || dynamic_cast<Unsigned *>(type)) {
        IntValue int1(0);
        Value *cost1 = hif::manipulation::transformConstant(&int1, type, this);
        messageAssert(cost1 != nullptr, "Transform constant failed.", type, this);
        cond->setValue2(_factory.expression(op_bnot, cost1));
        cond->setOperator(op_case_eq);
    } else {
        // For all other types compare with corresponding zero value.
        IntValue int1(0);
        Value *cost1 = hif::manipulation::transformConstant(&int1, type, this);
        messageAssert(cost1 != nullptr, "Transform constant failed.", type, this);
        cond->setValue2(cost1);
        cond->setOperator(op_case_neq);
    }
    return cond;
}
Value *HIFSemantics::explicitCast(Value *valueToCast, Type *castType, Type * /*srcType*/)
{
    Cast *ret = new Cast();
    ret->setValue(hif::copy(valueToCast));
    ret->setType(hif::copy(castType));
    return ret;
}
std::int64_t HIFSemantics::transformRealToInt(const double v) { return static_cast<std::int64_t>(v); }
Type *HIFSemantics::isTypeAllowedAsBound(Type *t)
{
    messageDebugAssert(t != nullptr, "Unexpected nullptr type", nullptr, this);

    //    Type * bt = getBaseType(t, false, this);
    //    assert( bt != nullptr );

    Int *intT       = dynamic_cast<Int *>(t);
    Value *spanSize = nullptr;
    if (intT != nullptr)
        spanSize = spanGetSize(intT->getSpan(), this);
    IntValue *iSpanSize = dynamic_cast<IntValue *>(spanSize);
    if (intT != nullptr && intT->getTypeVariant() == Type::NATIVE_TYPE) {
        if (iSpanSize == nullptr) {
            delete spanSize;
            return nullptr;
        }

        switch (iSpanSize->getValue()) {
        case 8:
        case 16:
        case 32:
        case 64:
            delete spanSize;
            return nullptr;
        default:
            break;
        }
    }

    Int *i = _factory.integer(hif::copy(hif::typeGetSpan(t, this)), hif::typeIsSigned(t, this), false);

    if (iSpanSize != nullptr) {
        std::int64_t val     = iSpanSize->getValue();
        std::int64_t retSize = 0;
        if (val <= 8)
            retSize = 7;
        else if (val <= 16)
            retSize = 15;
        else if (val <= 32)
            retSize = 31;
        else if (val <= 64)
            retSize = 63;
        else {
            messageError("Unexpected span size", i, this);
        }

        if (retSize != 0) {
            if (i->getSpan()->getDirection() == dir_upto) {
                delete i->getSpan()->setLeftBound(new IntValue(0));
                delete i->getSpan()->setRightBound(new IntValue(retSize));
            } else {
                messageDebugAssert(i->getSpan()->getDirection() == dir_downto, "Unexpected case", i->getSpan(), this);
                delete i->getSpan()->setLeftBound(new IntValue(retSize));
                delete i->getSpan()->setRightBound(new IntValue(0));
            }
        }
    }
    delete spanSize;

    return i;
}

bool HIFSemantics::isCastAllowed(Type *target, Type *source)
{
    if (dynamic_cast<Bool *>(target) != nullptr && dynamic_cast<TypeReference *>(source) != nullptr) {
        // special case: sc_logic_value_t
        TypeReference *tr = static_cast<TypeReference *>(source);
        Type *base        = getBaseType(tr, false, this);
        if (dynamic_cast<Enum *>(base) != nullptr) {
            TypeDef *td = dynamic_cast<TypeDef *>(base->getParent());
            messageAssert(td != nullptr, "Unexpected parent", base->getParent(), this);
            if (td->getName() == "hif_systemc_sc_logic_value_t")
                return false;
        }
    }

    return true;
}

bool HIFSemantics::isTypeAllowed(Type *t)
{
    if (t == nullptr)
        return false;
    Type::TypeVariant tv = t->getTypeVariant();
    if (tv != Type::NATIVE_TYPE) {
        return false;
    }

    return true;
}

bool HIFSemantics::isRangeDirectionAllowed(RangeDirection r) { return r == dir_upto || r == dir_downto; }

bool HIFSemantics::isTypeAllowedAsCase(Type * /*t*/) { return true; }

bool HIFSemantics::isTypeAllowedAsPort(Type *t) { return isTypeAllowed(t); }

bool HIFSemantics::isLanguageIdAllowed(LanguageID /*id*/) { return true; }

bool HIFSemantics::isValueAllowedInPortBinding(Value * /*o*/) { return true; }

bool HIFSemantics::isTypeAllowedForConstValue(ConstValue *cv, Type *synType)
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

    bool res = hif::equals(dt, baseSynType, opt);
    delete dt;

    return res;
}

// ---------------- map implementations --------------------
namespace
{ // anon namespace

HIFAnalysis::HIFAnalysis(Operator currOperator, Object *srcObj)
    : SemanticAnalysis(HIFSemantics::getInstance(), currOperator, srcObj)
    , BiVisitor<HIFAnalysis>()
    , _hifSem(HIFSemantics::getInstance())
{
    // ntd
}

HIFAnalysis::~HIFAnalysis()
{
    // ntd
}

void HIFAnalysis::map(Object *)
{
    // ntd
}

void HIFAnalysis::map(Array *array) { _baseMap(array); }

void HIFAnalysis::map(Bitvector *array)
{
    if (_currOperator == op_reverse) {
        _result.returnedType       = hif::copy(array);
        _result.operationPrecision = hif::copy(array);
        return;
    }

    _baseMap(array);
}

void HIFAnalysis::map(Bit *bit) { _baseMap(bit); }

void HIFAnalysis::map(Bool *b)
{
    if (!hif::operatorIsUnary(_currOperator) || !hif::operatorIsLogical(_currOperator)) {
        // error, non-unary or non-logical operator applied to a boolean
        return;
    }

    _result.returnedType       = hif::copy(b);
    _result.operationPrecision = hif::copy(b);
}

void HIFAnalysis::map(Char *c) { _baseMap(c); }

void HIFAnalysis::map(Int *i)
{
    if (!hif::operatorIsUnary(_currOperator) || hif::operatorIsLogical(_currOperator)) {
        // error, non-unary operator or logical operator applied to an int
        return;
    }

    if (_currOperator == op_minus) {
        // unary minus force the signed flag
        Int *returned = hif::copy(i);
        // Preserved signed for issue #117
        //returned->setSigned( true );

        _result.returnedType       = returned;
        _result.operationPrecision = hif::copy(returned);
    } else if (hif::operatorIsReduce(_currOperator)) {
        _result.returnedType       = _factory.boolean();
        _result.operationPrecision = hif::copy(i);
    } else {
        _result.returnedType       = hif::copy(i);
        _result.operationPrecision = hif::copy(i);
    }
}

void HIFAnalysis::map(Pointer *p)
{
    if (_currOperator != op_deref) {
        _baseMap(p);
        return;
    }

    Type *t                    = p->getType();
    _result.returnedType       = hif::copy(t);
    _result.operationPrecision = hif::copy(p);
}

void HIFAnalysis::map(Real *r) { _baseMap(r); }

void HIFAnalysis::map(Reference *r)
{
    Type *type = r->getType();

    // it needs a dynamic dispatch: don't call the map directly
    analyzeOperands(type, nullptr);
}

void HIFAnalysis::map(Signed *s)
{
    if (_currOperator == op_reverse) {
        _result.returnedType       = hif::copy(s);
        _result.operationPrecision = hif::copy(s);
        return;
    }

    _baseMap(s);
}

void HIFAnalysis::map(String *s)
{
    if (_currOperator == op_size) {
        _result.returnedType       = _factory.integer(nullptr, false);
        _result.operationPrecision = hif::copy(s);
        return;
    }

    if (_currOperator == op_reverse) {
        _result.returnedType       = hif::copy(s);
        _result.operationPrecision = hif::copy(s);
        return;
    }
}

void HIFAnalysis::map(Unsigned *u)
{
    if (_currOperator == op_reverse) {
        _result.returnedType       = hif::copy(u);
        _result.operationPrecision = hif::copy(u);
        return;
    }

    _baseMap(u);
}

void HIFAnalysis::map(TypeReference *op) { _map(op); }

void HIFAnalysis::map(Event *e)
{
    if (_currOperator != op_deref)
        return;

    _result.returnedType       = new Event();
    _result.operationPrecision = hif::copy(e);
}
void HIFAnalysis::map(Object * /*op1*/, Object * /*op2*/)
{
    // Parent maps
    // void _map(Type* op1, Int* op2);
    // void _map(Type* op1, TypeReference* op2);
    // void _map(Type* op1, Reference* op2);
    // void _map(Reference* op1, Type* op2);
    // void _map(TypeReference* op1, Type* op2);
#if 0
    Type * t1 = dynamic_cast<Type *>(op1);
    messageAssert(t1 != nullptr, "Expected type 1", op1, _sem);
    Type * t2 = dynamic_cast<Type *>(op2);
    messageAssert(t2 != nullptr, "Expected type 2", op2, _sem);

    if (dynamic_cast<Int *>(op2) != nullptr)
    {
        Int * b2 = static_cast<Int *>(op2);
        map(t1, b2);
    }
    else if (dynamic_cast<TypeReference *>(op2) != nullptr)
    {
        TypeReference * b2 = static_cast<TypeReference *>(op2);
        map(t1, b2);
    }
    else if (dynamic_cast<Reference *>(op2) != nullptr)
    {
        Reference * b2 = static_cast<Reference *>(op2);
        map(t1, b2);
    }
    else if (dynamic_cast<Reference *>(op1) != nullptr)
    {
        Reference * b1 = static_cast<Reference *>(op1);
        map(b1, t2);
    }
    else if (dynamic_cast<TypeReference *>(op1) != nullptr)
    {
        TypeReference * b1 = static_cast<TypeReference *>(op1);
        map(b1, t2);
    }
#endif
}
void HIFAnalysis::map(Array *op1, Array *op2)
{
    // check the operation
    if (hif::operatorIsLogical(_currOperator) || _isShiftOrRotate(_currOperator))
        return;

    Range *span1 = op1->getSpan();
    Range *span2 = op2->getSpan();
    messageAssert(span1 != nullptr && span2 != nullptr, "Missing type span(s)", _srcObj, _sem);

    bool isConstexpr1 = hif::typeIsConstexpr(op1, _sem);
    bool isConstexpr2 = hif::typeIsConstexpr(op2, _sem);

    if (_currOperator == op_concat) {
        if (dynamic_cast<Array *>(op1->getType()) != nullptr || dynamic_cast<Array *>(op2->getType()) != nullptr) {
            // not allowed concat between arrays of array
            return;
        }
    }

    // check if operation is allowed between single elements.
    analyzeOperands(op1->getType(), op2->getType());
    if (_result.returnedType == nullptr) {
        // operation not allowed
        return;
    }

    Type *resultArrayType = nullptr;
    if (_currOperator == op_concat) {
        Array *arr    = dynamic_cast<Array *>(_result.operationPrecision);
        Bitvector *bv = dynamic_cast<Bitvector *>(_result.operationPrecision);

        Type *arr1BaseType = getBaseType(op1->getType(), false, _sem);
        Type *arr2BaseType = getBaseType(op2->getType(), false, _sem);
        bool isBit1  = dynamic_cast<Bit *>(arr1BaseType) != nullptr;
        bool isBit2  = dynamic_cast<Bit *>(arr2BaseType) != nullptr;
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
        } else if (arr != nullptr) {
            resultArrayType = arr->getType();
            arr->setType(nullptr);
        } else {
            // generic concat between arrays of type T
            hif::EqualsOptions eOpts;
            eOpts.checkConstexprFlag = false;
            eOpts.checkStringSpan    = false;
            if (!hif::equals(arr1BaseType, arr2BaseType, eOpts)) {
                delete _result.operationPrecision;
                delete _result.returnedType;
                _result.operationPrecision = nullptr;
                _result.returnedType       = nullptr;
                return;
            }
            resultArrayType = hif::copy(arr1BaseType);
            hif::typeSetConstexpr(
                resultArrayType,
                (hif::typeIsConstexpr(arr1BaseType, _sem) && hif::typeIsConstexpr(arr2BaseType, _sem)));
        }
        delete _result.operationPrecision;
    } else {
        resultArrayType = _result.operationPrecision;
    }
    _result.operationPrecision = nullptr;

    delete _result.returnedType;
    _result.returnedType = nullptr;

    // build the result span
    Range *resultSpan = nullptr;

    if (_currOperator == op_concat || _currOperator == op_mult) {
        // the result range is the sum of the two ranges
        resultSpan = rangeGetSum(span1, span2, _sem);
    } else {
        if (isConstexpr1 && isConstexpr2) {
            resultSpan = rangeGetMax(span1, span2, _sem);
        } else if (!isConstexpr1 && isConstexpr2) {
            resultSpan = hif::copy(span1);
        } else if (isConstexpr1 && !isConstexpr2) {
            resultSpan = hif::copy(span2);
        } else //if( !isConstexpr1 && !isConstexpr2 )
        {
            // TODO: check if needed handle_constexpr_types
            if (!hif::equals(span1, span2)) {
                delete resultArrayType;
                return;
            }
            resultSpan = hif::copy(span1);
        }
    }

    // if the range cannot be established error
    messageAssert(resultSpan != nullptr, "cannot find result span", nullptr, nullptr);

    // build the array with the new range
    Array *arrayResult = new Array();
    arrayResult->setSpan(resultSpan);
    arrayResult->setType(resultArrayType);
    arrayResult->setSigned(op1->isSigned() || op2->isSigned());

    // set the result
    if (_isRelationalOrAssignment(_currOperator)) {
        Bit *bType = dynamic_cast<Bit *>(arrayResult->getType());
        if (bType != nullptr && bType->isLogic())
            _result.returnedType = hif::copy(bType);
        else
            _result.returnedType = new Bool();
    } else {
        _result.returnedType = hif::copy(arrayResult);
    }
    _result.operationPrecision = arrayResult;
}
void HIFAnalysis::map(Bitvector *op1, Bitvector *op2)
{
    // check the operation
    if (hif::operatorIsLogical(_currOperator))
        return;

    if (_isShiftOrRotate(_currOperator)) {
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    Range *span1 = op1->getSpan();
    Range *span2 = op2->getSpan();
    if (span1 == nullptr || span2 == nullptr) {
        // failed to get the ranges
        return;
    }

    bool isConstexpr1 = hif::typeIsConstexpr(op1, _sem);
    bool isConstexpr2 = hif::typeIsConstexpr(op2, _sem);

    // build the result span
    Range *resultSpan = nullptr;
    // NOTE: handleOnlySymbolsDeclarations added since
    // semantics type (fully qualified) can be compared with
    // syntactic type (not fully qualified).
    // Ref. design: vhdl/gaisler/can_oc
    hif::EqualsOptions eqOpt;
    eqOpt.checkConstexprFlag = false;
    //eqOpt.checkOnlySymbolsDeclarations = true;

    if (_currOperator == op_concat) {
        // the result range is the sum of the two ranges
        resultSpan = rangeGetSum(span1, span2, _sem);
    } else {
        if (op1->isLogic() != op2->isLogic()) {
            // TODO: allow in case of constant?
            return;
        }

        hif::EqualsOptions opt;
        // the two types must be equal considering constexpr flag.
        opt.handleConstexprTypes = (!hif::operatorIsAssignment(_currOperator));
        opt.checkConstexprFlag   = false;
        opt.sem                  = _sem;
        if (!hif::equals(op1, op2, opt))
            return;

        if (_currOperator == hif::op_mult) {
            // the result range is the sum of the two ranges
            resultSpan = rangeGetSum(span1, span2, _sem);
        } else if (isConstexpr1 && isConstexpr2) {
            resultSpan = rangeGetMax(span1, span2, _sem);
        } else if (!isConstexpr1 && isConstexpr2) {
            resultSpan = hif::copy(span1);
        } else if (isConstexpr1 && !isConstexpr2) {
            resultSpan = hif::copy(span2);
        } else //if( !isConstexpr1 && !isConstexpr2 )
        {
            // TODO: check if needed handle_constexpr_types
            if (!hif::equals(span1, span2, eqOpt))
                return;
            resultSpan = hif::copy(span1);
        }
    }

    // if the range cannot be established error
    messageAssert(resultSpan != nullptr, "cannot find result span", nullptr, nullptr);

    // build the array with the new range
    Bitvector *arrayResult = new Bitvector();
    arrayResult->setSpan(resultSpan);
    arrayResult->setConstexpr(isConstexpr1 && isConstexpr2);
    // Promoting to unsigned as verilog and c++
    arrayResult->setSigned(op1->isSigned() && op2->isSigned());
    arrayResult->setResolved(op1->isResolved() || op2->isResolved());
    arrayResult->setLogic(op1->isLogic() || op2->isLogic());

    // set the result
    if (_isRelationalOrAssignment(_currOperator)) {
        if (!hif::equals(span1, span2, eqOpt)) {
            delete arrayResult;
            return;
        }

        if (arrayResult->isLogic()) {
            Bit *bType = new Bit();
            bType->setConstexpr(arrayResult->isConstexpr());
            bType->setLogic(arrayResult->isLogic());
            bType->setResolved(arrayResult->isResolved());
            _result.returnedType = bType;
        } else {
            _result.returnedType = new Bool();
        }
    } else
        _result.returnedType = hif::copy(arrayResult);
    _result.operationPrecision = arrayResult;
}
void HIFAnalysis::map(Int *op1, Int *op2)
{
    if (_currOperator == op_concat) {
        // check that the types are equal (unless the range)
        // constexpr skips this check
        hif::EqualsOptions opt;
        opt.checkSpans = false;
        if (!op1->isConstexpr() && !op2->isConstexpr() && !hif::equals(op1, op2, opt)) {
            // if not constants must have equals types
            return;
        }
        // build the result range
        Range *range1 = op1->getSpan();
        Range *range2 = op2->getSpan();
        if (range1 == nullptr || range2 == nullptr)
            return;
        Range *rangeRes = rangeGetSum(range1, range2, _sem);
        if (rangeRes == nullptr)
            return;
        Int *intRes = new Int();
        // the reference int is the integer non constexpr
        Int *refInt = op1->isConstexpr() ? op2 : op1;
        intRes->setConstexpr(refInt->isConstexpr());
        intRes->setSigned(refInt->isSigned());
        intRes->setSpan(rangeRes);
        // set result
        _result.returnedType       = hif::copy(intRes);
        _result.operationPrecision = intRes;
    } else if (_isShiftOrRotate(_currOperator) || hif::operatorIsRotate(_currOperator)) {
        _baseShiftAndRotate(op1, op2);
    } else {
        _baseMap(op1, op2);
    }
}
void HIFAnalysis::map(Bit *op1, Bit *op2)
{
    if (_currOperator == op_concat) {
        hif::EqualsOptions opt;
        opt.checkConstexprFlag = false;
        if (!hif::equals(op1, op2, opt))
            return;

        // the result is a two bit array
        Bitvector *array_res = new Bitvector();
        array_res->setSpan(new Range(1, 0));
        array_res->setSigned(false);
        array_res->setResolved(op1->isResolved());
        array_res->setLogic(op1->isLogic());
        array_res->setConstexpr(op1->isConstexpr());
        // set the result
        _result.returnedType       = hif::copy(array_res);
        _result.operationPrecision = array_res;
    } else {
        _baseMap(op1, op2);
    }
}
void HIFAnalysis::map(Array *op1, Bit *op2)
{
    // the operator must be concatenation
    if (_currOperator != op_concat)
        return;

    // the array type must be the bit (unless constexpr)
    Bit *arrayType = dynamic_cast<Bit *>(op1->getType());
    if (arrayType == nullptr)
        return;
    if (!hif::equals(arrayType, op2))
        return;

    // retrieve the range length
    Range *range1 = op1->getSpan();
    if (range1 == nullptr)
        return;

    Range *resultRange  = nullptr;
    std::int64_t rangeSize = static_cast<std::int64_t>(spanGetBitwidth(range1, _sem));
    if (rangeSize > 0) {
        if (range1->getDirection() == dir_downto) {
            // the range is [ range1_length + bit_length - 1 ; 0 ]
            // simplified in [ range1_length ; 0 ]
            resultRange = new Range(rangeSize, 0);
        } else {
            // it is the opposite
            resultRange = new Range(0, rangeSize);
        }
    } else {
        // if a size is equal to zero means that the range bound is a general
        // expression.
        // check range bounds
        if (range1->getLeftBound() == nullptr || range1->getRightBound() == nullptr)
            return;

        // the range becomes [ upper + 1 ; lower ]
        // build the upper and the lower bound
        Expression *upperBoundResult = new Expression();
        Value *lowerBoundResult;
        if (range1->getDirection() == dir_downto) {
            upperBoundResult->setValue1(hif::copy(range1->getLeftBound()));
            IntValue *ivo = new IntValue(1);
            ivo->setType(_sem->getTypeForConstant(ivo));
            upperBoundResult->setValue2(ivo);
            upperBoundResult->setOperator(op_plus);
            lowerBoundResult = hif::copy(range1->getRightBound());
            resultRange      = new Range(upperBoundResult, lowerBoundResult, dir_downto);
        } else {
            upperBoundResult->setValue1(hif::copy(range1->getRightBound()));
            IntValue *ivo = new IntValue(1);
            ivo->setType(_sem->getTypeForConstant(ivo));
            upperBoundResult->setValue2(ivo);
            upperBoundResult->setOperator(op_plus);
            lowerBoundResult = hif::copy(range1->getLeftBound());
            resultRange      = new Range(lowerBoundResult, upperBoundResult, dir_upto);
        }
    }
    // build the result array
    Array *arrayResult = new Array();
    arrayResult->setSpan(resultRange);
    arrayResult->setSigned(op1->isSigned());
    // the type is the non constexpr
    arrayResult->setType(hif::copy(op2->isConstexpr() ? arrayType : op2));

    // set the returned and the precision
    _result.operationPrecision = hif::copy(op1);
    _result.returnedType       = arrayResult;
}
void HIFAnalysis::map(Bit *op1, Array *op2) { map(op2, op1); }
void HIFAnalysis::map(Bool *op1, Bitvector *op2)
{
    Bit bit;
    bit.setConstexpr(op1->isConstexpr());
    bit.setLogic(false);
    bit.setResolved(false);

    map(&bit, op2);
}

void HIFAnalysis::map(Bitvector *op1, Bool *op2)
{
    Bit bit;
    bit.setConstexpr(op2->isConstexpr());
    bit.setLogic(false);
    bit.setResolved(false);

    map(op1, &bit);
}

void HIFAnalysis::map(Bitvector *op1, Bit *op2)
{
    // the operator must be concatenation
    if (_currOperator != op_concat)
        return;

    Bitvector *ret = new Bitvector();
    ret->setSigned(op1->isSigned());
    ret->setConstexpr(op1->isConstexpr() && op2->isConstexpr());
    ret->setResolved(op1->isResolved() || op2->isResolved());
    ret->setLogic(op1->isLogic() || op2->isLogic());
    ret->setSpan(hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem));

    // set the returned and the precision
    _result.operationPrecision = hif::copy(ret);
    _result.returnedType       = ret;
}

void HIFAnalysis::map(Bit *op1, Bitvector *op2) { map(op2, op1); }

void HIFAnalysis::map(Bit *op1, Signed *op2) { map(op2, op1); }

void HIFAnalysis::map(Bit *op1, Unsigned *op2) { map(op2, op1); }

void HIFAnalysis::map(Bool *op1, Bool *op2)
{
    // check allowed operation
    if (!hif::operatorIsLogical(_currOperator) && !_isRelationalOrAssignment(_currOperator) &&
        !hif::operatorIsBitwise(_currOperator) && _currOperator != op_concat) {
        return;
    }

    if (_currOperator == op_concat) {
        // the result is a two bit array
        Bitvector *arrayResult = new Bitvector();
        arrayResult->setSpan(new Range(1, 0));
        arrayResult->setSigned(false);
        arrayResult->setResolved(false);
        arrayResult->setLogic(false);
        arrayResult->setConstexpr(op1->isConstexpr() && op2->isConstexpr());
        // set the result
        _result.returnedType       = hif::copy(arrayResult);
        _result.operationPrecision = arrayResult;
        return;
    }

    // otherwise the operation is permitted
    _result.returnedType       = new Bool();
    _result.operationPrecision = new Bool();
}
void HIFAnalysis::map(Char *op1, Char *op2) { _baseMap(op1, op2); }
void HIFAnalysis::map(Enum *op1, Enum *op2)
{
    // check the operation
    if (!_isRelationalOrAssignment(_currOperator))
        return;

    // must be equal types
    if (!hif::equals(op1, op2))
        return;

    // set the return and the precision types
    _result.returnedType       = new Bool();
    _result.operationPrecision = hif::copy(op1);
}
void HIFAnalysis::map(Pointer *op1, Pointer *op2)
{
    Type *t1 = op1->getType();
    Type *t2 = op2->getType();
    analyzeOperands(t1, t2);

    if (_result.returnedType == nullptr)
        return;
    delete _result.returnedType;
    delete _result.operationPrecision;
    _result.returnedType       = nullptr;
    _result.operationPrecision = nullptr;

    bool isAssign = hif::operatorIsAssignment(_currOperator);
    if (!isAssign && (_currOperator != op_eq) && (_currOperator != op_neq) && (_currOperator != op_case_eq) &&
        (_currOperator != op_case_neq)) {
        // only assignment and check for equality/inequality are allowed at the moment
        return;
    }

    if ((_currOperator == op_eq) || (_currOperator == op_neq) || (_currOperator == op_case_eq) ||
        (_currOperator == op_case_neq)) {
        _result.returnedType = new Bool();
    } else {
        _result.returnedType = hif::copy(op1);
    }
    _result.operationPrecision = hif::copy(op1);
}

void HIFAnalysis::map(Pointer *op1, String *op2)
{
    if (hif::operatorIsAssignment(_currOperator)) {
        Char *c = dynamic_cast<Char *>(getBaseType(op1->getType(), false, _sem));
        if (c == nullptr)
            return;

        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
        return;
    }

    map(op2, op1);
}

void HIFAnalysis::map(Pointer *op1, Int * /*op2*/)
{
    if (_currOperator != hif::op_plus && _currOperator != hif::op_minus)
        return;

    _result.returnedType       = hif::copy(op1);
    _result.operationPrecision = hif::copy(op1);
}
void HIFAnalysis::map(Real *op1, Real *op2) { _baseMap(op1, op2); }
void HIFAnalysis::map(Real *op1, Time *op2) { map(op2, op1); }
void HIFAnalysis::map(Record *op1, Record *op2)
{
    if (!hif::operatorIsAssignment(_currOperator) && _currOperator != op_eq && _currOperator != op_neq &&
        _currOperator != op_case_eq && _currOperator != op_case_neq) {
        return;
    }

    if (op1->fields.size() != op2->fields.size())
        return;

    BList<Field>::iterator i = op1->fields.begin();
    BList<Field>::iterator j = op2->fields.begin();
    BList<Field> types;
    for (; i != op1->fields.end(); ++i, ++j) {
        if ((*i)->getName() != (*j)->getName())
            return;

        analyzeOperands((*i)->getType(), (*j)->getType());

        if (_result.returnedType == nullptr)
            return;
        delete _result.returnedType;
        _result.returnedType = nullptr;

        Field *f = new Field();
        f->setName((*i)->getName());
        f->setType(_result.operationPrecision);
        types.push_back(f);
        _result.operationPrecision = nullptr;
    }

    Record *ret = new Record();
    ret->fields.merge(types);

    if (hif::operatorIsAssignment(_currOperator)) {
        _result.operationPrecision = ret;
        _result.returnedType       = hif::copy(op1);
    } else {
        _result.operationPrecision = ret;
        _result.returnedType       = new Bool();
    }
}
void HIFAnalysis::map(Reference *op1, Reference *op2)
{
    Type *t1 = op1->getType();
    Type *t2 = op2->getType();
    analyzeOperands(t1, t2);
}
void HIFAnalysis::map(Reference *op1, Type *op2)
{
    Type *t1 = op1->getType();
    analyzeOperands(t1, op2);
}
void HIFAnalysis::map(Type *op1, Reference *op2)
{
    Type *t2 = op2->getType();
    analyzeOperands(op1, t2);
}
void HIFAnalysis::map(Reference *op1, TypeReference *op2)
{
    Type *t1 = op1->getType();
    analyzeOperands(t1, op2);
}
void HIFAnalysis::map(TypeReference *op1, Reference *op2)
{
    Type *t2 = op2->getType();
    analyzeOperands(op1, t2);
}
void HIFAnalysis::map(Reference *op1, Int *op2)
{
    Type *t1 = op1->getType();
    analyzeOperands(t1, op2);
}
void HIFAnalysis::map(Signed *op1, Signed *op2)
{
    if (_currOperator == op_concat) {
        Range *range1 = op1->getSpan();
        Range *range2 = op2->getSpan();
        if (range1 == nullptr || range2 == nullptr)
            return;

        Range *resRange = rangeGetSum(range1, range2, _sem);
        if (resRange == nullptr)
            return;
        Signed *resSigned = new Signed();
        resSigned->setSpan(resRange);
        // set the result
        _result.returnedType       = hif::copy(resSigned);
        _result.operationPrecision = resSigned;
    } else {
        _baseMap(op1, op2);
    }
}

void HIFAnalysis::map(String *op1, String *op2)
{
    if (_currOperator == op_concat) {
        _result.returnedType = new String();
        String *precision    = new String();
        if (op1->getSpanInformation() != nullptr && op2->getSpanInformation() != nullptr) {
            Range *sum = rangeGetSum(op1->getSpanInformation(), op2->getSpanInformation(), _sem);
            precision->setSpanInformation(sum);
        }
        _result.operationPrecision = precision;
    } else if (hif::operatorIsAssignment(_currOperator)) {
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
    } else if (hif::operatorIsRelational(_currOperator)) {
        _result.returnedType = new Bool();
        String *precision    = new String();
        if (op1->getSpanInformation() != nullptr && op2->getSpanInformation() != nullptr) {
            Range *max = rangeGetMax(op1->getSpanInformation(), op2->getSpanInformation(), _sem);
            precision->setSpanInformation(max);
        }
        _result.operationPrecision = precision;
    }
}
void HIFAnalysis::map(Signed *op1, Array *op2)
{
    if (_currOperator != op_concat) {
        // like map (signed, signed)
        if (dynamic_cast<Bit *>(op2->getType()) == nullptr)
            return;

        Signed dummy_signed;
        dummy_signed.setSpan(hif::copy(op2->getSpan()));
        map(op1, &dummy_signed);
        return;
    }

    Type *arrType = op2->getType();
    _currOperator = op_conv;
    analyzeOperands(op1, arrType);
    _currOperator = op_concat;
    if (_result.returnedType == nullptr)
        return;
    delete _result.returnedType;
    _result.returnedType = nullptr;
    delete _result.operationPrecision;
    _result.operationPrecision = nullptr;

    Array *ret = hif::copy(op2);
    delete ret->setSpan(hif::manipulation::rangeGetIncremented(op2->getSpan(), _sem));
    _result.returnedType       = ret;
    _result.operationPrecision = hif::copy(ret);
}

void HIFAnalysis::map(Signed *op1, Bit *op2)
{
    // the operator must be concatenation
    if (_currOperator != op_concat)
        return;

    Signed *ret = new Signed();
    ret->setConstexpr(op1->isConstexpr() && op2->isConstexpr());
    ret->setSpan(hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem));

    // set the returned and the precision
    _result.operationPrecision = hif::copy(ret);
    _result.returnedType       = ret;
}
void HIFAnalysis::map(Array *op1, Signed *op2) { map(op2, op1); }

void HIFAnalysis::map(Signed *op1, Bitvector *op2)
{
    if (!op2->isConstexpr())
        return;

    // build a fake signed object
    Signed dummy_signed;
    dummy_signed.setSpan(hif::copy(op2->getSpan()));
    map(op1, &dummy_signed);
}

void HIFAnalysis::map(Bitvector *op1, Signed *op2) { map(op2, op1); }
void HIFAnalysis::map(Unsigned *op1, Array *op2)
{
    if (_currOperator != op_concat) {
        if (dynamic_cast<Bit *>(op2->getType()) == nullptr)
            return;

        Unsigned dummy_unsigned;
        dummy_unsigned.setSpan(hif::copy(op2->getSpan()));
        map(op1, &dummy_unsigned);
        return;
    }

    Type *arrType = op2->getType();
    _currOperator = op_conv;
    analyzeOperands(op1, arrType);
    _currOperator = op_concat;
    if (_result.returnedType == nullptr)
        return;
    delete _result.returnedType;
    _result.returnedType = nullptr;
    delete _result.operationPrecision;
    _result.operationPrecision = nullptr;

    Array *ret = hif::copy(op2);
    delete ret->setSpan(hif::manipulation::rangeGetIncremented(op2->getSpan(), _sem));
    _result.returnedType       = ret;
    _result.operationPrecision = hif::copy(ret);
}

void HIFAnalysis::map(Unsigned *op1, Bit *op2)
{
    // the operator must be concatenation
    if (_currOperator != op_concat)
        return;

    Unsigned *ret = new Unsigned();
    ret->setConstexpr(op1->isConstexpr() && op2->isConstexpr());
    ret->setSpan(hif::manipulation::rangeGetIncremented(op1->getSpan(), _sem));

    // set the returned and the precision
    _result.operationPrecision = hif::copy(ret);
    _result.returnedType       = ret;
}
void HIFAnalysis::map(Array *op1, Unsigned *op2) { map(op2, op1); }

void HIFAnalysis::map(Array *op1, Bitvector *op2)
{
    if (_currOperator != op_concat)
        return;

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
}

void HIFAnalysis::map(Array *op1, Int *op2)
{
    if (_currOperator == hif::op_concat) {
        // If op1 is array of Int then concat is a sort of push_back of op2
        Type *arrType     = op1->getType();
        Type *arrBaseType = hif::semantics::getBaseType(arrType, false, _sem);
        if (hif::equals(arrBaseType, op2)) {
            Array *ret = hif::copy(op1);
            delete ret->setSpan(hif::manipulation::rangeGetIncremented(ret->getSpan(), _sem));
            _result.returnedType       = ret;
            _result.operationPrecision = hif::copy(ret);
            return;
        }
    }

    if (!op2->isConstexpr())
        return;
    if (_currOperator != op_plus && _currOperator != op_minus)
        return;

    Pointer *p = new Pointer();
    p->setType(hif::copy(op1->getType()));

    _result.returnedType       = p;
    _result.operationPrecision = hif::copy(op2);
}

void HIFAnalysis::map(Array *op1, ViewReference *op2) { map(op2, op1); }

void HIFAnalysis::map(Unsigned *op1, Bitvector *op2)
{
    if (!op2->isConstexpr())
        return;

    // build a fake unsigned object
    Unsigned dummy_unsigned;
    dummy_unsigned.setSpan(hif::copy(op2->getSpan()));
    map(op1, &dummy_unsigned);
}

void HIFAnalysis::map(Bitvector *op1, Unsigned *op2) { map(op2, op1); }

void HIFAnalysis::map(Bitvector *op1, Array *op2) { map(op2, op1); }

void HIFAnalysis::map(String *op1, Pointer *op2)
{
    Char *c = dynamic_cast<Char *>(getBaseType(op2->getType(), false, _sem));
    if (c == nullptr)
        return;

    if (_currOperator == op_concat) {
        _result.returnedType       = new String();
        _result.operationPrecision = new String();
    } else if (hif::operatorIsAssignment(_currOperator)) {
        _result.returnedType       = hif::copy(op1);
        _result.operationPrecision = hif::copy(op1);
    } else if (hif::operatorIsRelational(_currOperator)) {
        _result.returnedType       = new Bool();
        _result.operationPrecision = new String();
    }
}
void HIFAnalysis::map(Unsigned *op1, Unsigned *op2)
{
    if (_currOperator == op_concat) {
        Range *range1   = op1->getSpan();
        Range *range2   = op2->getSpan();
        Range *resRange = rangeGetSum(range1, range2, _sem);
        if (resRange == nullptr)
            return;
        Unsigned *resUnsigned = new Unsigned();
        resUnsigned->setSpan(resRange);
        // set the result
        _result.returnedType       = hif::copy(resUnsigned);
        _result.operationPrecision = resUnsigned;
    } else {
        _baseMap(op1, op2);
    }
}
void HIFAnalysis::map(Type *op1, Int *op2) { _baseShiftAndRotate(op1, op2); }

void HIFAnalysis::map(TypeReference *op1, TypeReference *op2) { _map(op1, op2, true); }

void HIFAnalysis::map(TypeReference *op1, Type *op2) { _map(op1, op2); }
void HIFAnalysis::map(Type *op1, TypeReference *op2) { _map(op1, op2); }

void HIFAnalysis::map(TypeReference *op1, Int *op2)
{
    // resolve ambiguity
    map(op1, static_cast<Type *>(op2));
}

void HIFAnalysis::map(ViewReference *op1, ViewReference *op2)
{
    if (!hif::operatorIsAssignment(_currOperator) && _currOperator != op_eq && _currOperator != op_neq &&
        _currOperator != op_case_eq && _currOperator != op_case_neq && _currOperator != op_concat) {
        return;
    }

    if (!isSubType(op2, op1, _sem))
        return;

    if (_currOperator == op_concat) {
        // at the moment allow only same type concat.
        if (!hif::equals(op1, op2))
            return;

        // works only with disciplines
        ViewReference::DeclarationType *d1 = getDeclaration(op1, _sem);
        ViewReference::DeclarationType *d2 = getDeclaration(op2, _sem);
        if (d1 == nullptr || d2 == nullptr || d1 != d2)
            return;

        LibraryDef *ld1 = hif::getNearestParent<LibraryDef>(d1);
        if (ld1 == nullptr)
            return;
        if (ld1->getName() != "hif_verilog_vams_disciplines" && ld1->getName() != "vams_disciplines")
            return;

        Array *res_array = new Array();
        res_array->setSpan(new Range(1, 0));
        res_array->setType(hif::copy(op1));
        res_array->setSigned(false);
        _result.returnedType       = res_array;
        _result.operationPrecision = hif::copy(op1);

        return;
    }

    if (hif::operatorIsRelational(_currOperator))
        _result.returnedType = new Bool();
    else
        _result.returnedType = hif::copy(op1);
    _result.operationPrecision = hif::copy(op1);
}

void HIFAnalysis::map(ViewReference *op1, Array *op2)
{
    if (_currOperator != op_concat)
        return;

    ViewReference *arrT = dynamic_cast<ViewReference *>(op2->getType());
    if (arrT == nullptr)
        return;

    // at the moment allow only same type concat.
    if (!hif::equals(op1, arrT))
        return;

    // works only with disciplines
    ViewReference::DeclarationType *d1 = getDeclaration(op1, _sem);
    ViewReference::DeclarationType *d2 = getDeclaration(arrT, _sem);
    if (d1 == nullptr || d2 == nullptr || d1 != d2)
        return;

    LibraryDef *ld1 = hif::getNearestParent<LibraryDef>(d1);
    if (ld1 == nullptr)
        return;
    if (ld1->getName() != "hif_verilog_vams_disciplines" && ld1->getName() != "vams_disciplines") {
        return;
    }

    Array *resArray = hif::copy(op2);
    Range *r        = hif::manipulation::rangeGetIncremented(resArray->getSpan(), _sem);
    delete resArray->setSpan(r);
    _result.returnedType       = resArray;
    _result.operationPrecision = hif::copy(resArray);
}

void HIFAnalysis::_callMap(Object *op1) { this->callMap(op1); }

void HIFAnalysis::_callMap(Object *op1, Object *op2) { this->callMap(op1, op2); }

void HIFAnalysis::map(Time * /*op1*/, Time * /*op2*/)
{
    if (!hif::operatorIsAssignment(_currOperator) && _currOperator != op_eq && _currOperator != op_neq &&
        _currOperator != op_le && _currOperator != op_ge && _currOperator != op_lt && _currOperator != op_gt &&
        _currOperator != op_plus && _currOperator != op_minus && _currOperator != op_le && _currOperator != op_ge &&
        _currOperator != op_lt && _currOperator != op_gt)
        return;

    if (hif::operatorIsRelational(_currOperator)) {
        _result.returnedType = new Bool();
    } else {
        _result.returnedType = new Time();
    }

    _result.operationPrecision = new Time();
}

void HIFAnalysis::map(Time * /*op1*/, Int * /*op2*/)
{
    if (_currOperator != op_mult && _currOperator != op_div)
        return;

    _result.returnedType       = new Time();
    _result.operationPrecision = new Time();
}

void HIFAnalysis::map(Time * /*op1*/, Real * /*op2*/)
{
    if (_currOperator != op_mult && _currOperator != op_div)
        return;

    _result.returnedType       = new Time();
    _result.operationPrecision = new Time();
}

void HIFAnalysis::map(Int *op1, Time *op2) { map(op2, op1); }

void HIFAnalysis::map(Int *op1, Pointer *op2) { map(op2, op1); }

void HIFAnalysis::map(Time * /*op1*/, Bitvector * /*op2*/)
{
    return; // TODO check
#if 0
    if( currOperator != op_mult
        && currOperator != op_div) return;

    result.returnedType = new Time();
    result.operationPrecision = new Time();
#endif
}

void HIFAnalysis::map(Bitvector *op1, Time *op2) { map(op2, op1); }

void HIFAnalysis::map(File *op1, File *op2)
{
    if (!hif::operatorIsAssignment(_currOperator))
        return;

    if (!hif::equals(op1, op2))
        return;

    _result.returnedType       = hif::copy(op1);
    _result.operationPrecision = hif::copy(op1);
}

void HIFAnalysis::map(Int *op1, Array *op2)
{
    if (_currOperator == op_minus)
        return;
    map(op2, op1);
}

// --------------------- private implementation ------------------
void HIFAnalysis::_baseMap(Type *t)
{
    if (!hif::operatorIsUnary(_currOperator) || hif::operatorIsLogical(_currOperator)) {
        // error, non-unary operator or logical operator applied to an array
        return;
    }

    if (hif::operatorIsReduce(_currOperator)) {
        Bitvector *ao = dynamic_cast<Bitvector *>(t);
        if (ao == nullptr)
            return;
        Bit *bo = new Bit();
        bo->setConstexpr(ao->isConstexpr());
        bo->setResolved(ao->isResolved());
        bo->setLogic(ao->isLogic());
        _result.returnedType       = bo;
        _result.operationPrecision = hif::copy(t);
        return;
    } else if (_currOperator == op_ref) {
        Pointer *p = new Pointer();
        p->setType(hif::copy(t));
        _result.returnedType       = p;
        _result.operationPrecision = hif::copy(t);
        return;
    }

    _result.returnedType       = hif::copy(t);
    _result.operationPrecision = hif::copy(t);
}
void HIFAnalysis::_baseMap(Type *t1, Type *t2)
{
    hif::EqualsOptions opt;
    // the two types must be equal considering constexpr flag.
    opt.handleConstexprTypes = (!hif::operatorIsAssignment(_currOperator));
    opt.checkConstexprFlag   = false;
    opt.sem                  = _sem;

    if (!hif::equals(t1, t2, opt))
        return;

    // the reference type is the non-constexpr one
    bool isConstexpr1 = hif::typeIsConstexpr(t1, _sem);
    bool isConstexpr2 = hif::typeIsConstexpr(t2, _sem);
    Type *refType     = nullptr;

    if (isConstexpr1 && isConstexpr2) {
        Range *span1 = typeGetSpan(t1, _sem);
        Range *span2 = typeGetSpan(t2, _sem);
        if (span1 == nullptr || span2 == nullptr) {
            messageDebug("Type1:", t1, _sem);
            messageDebug("Type2:", t2, _sem);
            messageError("Unexpected case: a span is nullptr!", nullptr, nullptr);
        }

        Range *resSpan = rangeGetMax(span1, span2, _sem);
        messageDebugAssert(resSpan != nullptr, "cannot find max span", nullptr, nullptr);

        refType = hif::copy(t1);
        typeSetSpan(refType, resSpan, _sem, true);

        bool setSigned = typeIsSigned(t1, _sem, true) || typeIsSigned(t2, _sem, true);
        typeSetSigned(refType, setSigned, _sem);
    } else if (!isConstexpr1 && isConstexpr2) {
        refType = hif::copy(t1);
    } else if (isConstexpr1 && !isConstexpr2) {
        refType = hif::copy(t2);
    } else //if (!isConstexpr1 && !isConstexpr2)
    {
        if (!hif::equals(t1, t2))
            return;
        refType = hif::copy(t1);
    }

    // check the operation
    if (hif::operatorIsLogical(_currOperator)) {
        delete refType;
        return;
    }
    // otherwise the operation is permitted
    if (_isRelationalOrAssignment(_currOperator)) {
        bool alwaysBool =
            (dynamic_cast<Signed *>(t1) != nullptr || dynamic_cast<Unsigned *>(t1) != nullptr ||
             dynamic_cast<Signed *>(t2) != nullptr || dynamic_cast<Unsigned *>(t2) != nullptr);
        if (!alwaysBool && (typeIsLogic(t1, _sem) || typeIsLogic(t2, _sem))) {
            Bit *bType = new Bit();
            bType->setConstexpr(typeIsConstexpr(refType, _sem));
            bType->setLogic(true);
            bType->setResolved(typeIsResolved(refType, _sem));
            _result.returnedType = bType;
        } else {
            _result.returnedType = new Bool();
        }
    } else {
        _result.returnedType = hif::copy(refType);
    }
    messageDebugAssert(_result.returnedType != nullptr, "Expected returned type", nullptr, nullptr);
    _result.operationPrecision = refType;
}
void HIFAnalysis::_baseShiftAndRotate(Type *t1, Int * /*i*/)
{
    if (!_isShiftOrRotate(_currOperator) && !hif::operatorIsRotate(_currOperator)) {
        // error, operation not supported
        return;
    }

    // check that the integer is unsigned or constrexpr
    //    if (! i->isConstexpr() && i->isSigned())
    //        // error, signed integer is not supported
    //        return;

    // make the result
    _result.returnedType       = hif::copy(t1);
    _result.operationPrecision = hif::copy(t1);
}

// ------------------------ helper functions --------------------------

// Wrapper used to suppress useless warnings:
#ifdef _MSC_VER
#    pragma warning(disable : 4127)
#    pragma warning(disable : 4244)
#    pragma warning(disable : 4505)
#endif

bool _isRelationalOrAssignment(Operator operation)
{
    return hif::operatorIsRelational(operation) || hif::operatorIsAssignment(operation);
}

bool _isShiftOrRotate(Operator operation)
{
    return hif::operatorIsShift(operation) && !hif::operatorIsRotate(operation);
}

} // namespace

} // namespace semantics
} // namespace hif
