/// @file HifFactory.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cassert>

#include "hif/hif.hpp"

#if (defined _MSC_VER)
#else
#    pragma GCC diagnostic ignored "-Weffc++"
#endif
#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-template-vtables"
#endif
namespace hif
{

namespace /*anon*/
{
class CodeInfoVisitor : public GuideVisitor
{
public:
    typedef Object::CodeInfo Infos;

    CodeInfoVisitor(const std::string &s, unsigned int l, unsigned int col);

    ~CodeInfoVisitor();

    bool BeforeVisit(Object &o);

    int AfterVisit(Object &o);

private:
    std::list<Infos> _infos;
};

CodeInfoVisitor::CodeInfoVisitor(const std::string &s, unsigned int l, unsigned int col)
    : GuideVisitor()
    , _infos()
{
    _infos.push_back(Infos(s, l, col));
}

CodeInfoVisitor::~CodeInfoVisitor()
{
    // ntd
}

bool CodeInfoVisitor::BeforeVisit(Object &o)
{
    if (o.getSourceLineNumber() != 0) {
        _infos.push_back(Infos(o.getSourceFileName(), o.getSourceLineNumber(), o.getSourceColumnNumber()));
    }

    return false;
}

int CodeInfoVisitor::AfterVisit(Object &o)
{
    messageAssert(!_infos.empty(), "Unexpected case", nullptr, nullptr);

    if (o.getSourceLineNumber() == 0) {
        Infos &infos = _infos.back();
        o.setSourceFileName(infos.filename);
        o.setSourceLineNumber(infos.lineNumber);
        o.setSourceColumnNumber(infos.columnNumber);
    } else {
        _infos.pop_back();
    }

    return 0;
}
} // namespace

ListElementHost::ListElementHost()
    : _elements()
{
    // Nothing to do.
}

ListElementHost::~ListElementHost()
{
    // Nothing to do.
}

ListElementHost::ListElementHost(const ListElementHost &t)
    : _elements()
{
    _elements.merge(const_cast<ListElementHost &>(t)._elements);
}

ListElementHost &ListElementHost::operator=(const ListElementHost &t)
{
    if (&t == this)
        return *this;
    _elements.merge(const_cast<ListElementHost &>(t)._elements);

    return *this;
}

ListElementHost::operator Object *() const
{
    if (_elements.empty())
        return nullptr;
    messageDebugAssert(_elements.size() == 1, "Unexpected list size", nullptr, nullptr);
    Object *p             = _elements.front();
    ListElementHost *that = const_cast<ListElementHost *>(this);
    that->_elements.remove(p);
    return p;
}

ListElementHost ListElementHost::operator,(const ListElementHost &t2)
{
    _elements.merge(const_cast<ListElementHost &>(t2)._elements);
    return *this;
}
template <class T>
ListElement<T>::ListElement()
    : ListElementHost()
{
    // Nothing to do.
}

template <class T>
ListElement<T>::ListElement(T *e)
    : ListElementHost()
{
    _elements.push_back(e);
}

template <class T>
ListElement<T>::ListElement(BList<T> &l)
    : ListElementHost()
{
    _elements.merge(l);
}

template <class T> ListElement<T>::~ListElement()
{
    // Nothing to do.
}

template <class T>
ListElement<T>::ListElement(const ListElement<T> &t)
    : ListElementHost(t)
{
    // Nothing to do.
}

template <class T> ListElement<T> &ListElement<T>::operator=(const ListElement<T> &t)
{
    ListElementHost::operator=(t);
    return *this;
}

template <class T> ListElement<T>::operator T *() const
{
    Object *ret = ListElementHost::operator hif::Object *();
    return static_cast<T *>(ret);
}

template <class T> ListElement<T> ListElement<T>::operator,(const ListElement<T> &t2)
{
    ListElementHost tmp(ListElementHost::operator,(t2));
    return *static_cast<const ListElement<T> *>(&tmp);
}

template <class T> BList<T> &ListElement<T>::getElements() { return _elements.toOtherBList<T>(); }

template <class T> T *ListElement<T>::getObject()
{
    T *ret = *this;
    return ret;
}

HifFactory::HifFactory()
    : _sem(hif::semantics::HIFSemantics::getInstance())
{
    // Nothing to do.
}
HifFactory::HifFactory(hif::semantics::ILanguageSemantics *sem)
    : _sem(sem)
{
    // Nothing to do.
}
HifFactory::~HifFactory()
{
    // Nothing to do.
}

HifFactory::HifFactory(const HifFactory &f)
    : _sem(f._sem)
{
    // Nohting to do.
}

HifFactory &HifFactory::operator=(const HifFactory &f)
{
    if (this == &f)
        return *this;
    _sem = f._sem;
    return *this;
}

void HifFactory::setSemantics(hif::semantics::ILanguageSemantics *sem) { _sem = sem; }

semantics::ILanguageSemantics *HifFactory::getSemantics() { return _sem; }

// /////////////////////////////////////////////////////////////
// Helper methods.
// /////////////////////////////////////////////////////////////
HifFactory::recordvaluealt_t HifFactory::noRecordValueAlts() { return recordvaluealt_t(); }

HifFactory::whenAlt_t HifFactory::noWhenAlts() { return whenAlt_t(); }

HifFactory::field_t HifFactory::noFields() { return field_t(); }

HifFactory::viewReference_t HifFactory::noViewReferences() { return viewReference_t(); }

HifFactory::template_t HifFactory::noTemplates() { return template_t(); }

HifFactory::action_t HifFactory::noActions() { return action_t(); }

HifFactory::parameter_t HifFactory::noParameters() { return parameter_t(); }

Type *HifFactory::noType() { return nullptr; }

Value *HifFactory::noValue() { return nullptr; }

HifFactory::parameterArgument_t HifFactory::noParameterArguments() { return parameterArgument_t(); }

HifFactory::portassign_t HifFactory::noPortAssigns() { return portassign_t(); }

HifFactory::templateArgument_t HifFactory::noTemplateArguments() { return templateArgument_t(); }

Object *HifFactory::codeInfo(Object *o, const std::string &fileName, unsigned int line, unsigned int col)
{
    CodeInfoVisitor v(fileName, line, col);
    o->acceptVisitor(v);

    return o;
}

template <typename T> T *HifFactory::codeInfo(T *o, const std::string &fileName, unsigned int line, unsigned int col)
{
    return static_cast<T *>(codeInfo(static_cast<Object *>(o), fileName, line, col));
}

template <typename T> T *HifFactory::codeInfo(T *o, const Object::CodeInfo &info)
{
    return static_cast<T *>(codeInfo(static_cast<Object *>(o), info.filename, info.lineNumber, info.columnNumber));
}

HifFactory::library_t HifFactory::noLibraries() { return library_t(); }

HifFactory::declaration_t HifFactory::noDeclarations() { return declaration_t(); }

HifFactory::generate_t HifFactory::noGenerates() { return generate_t(); }

HifFactory::ifAlt_t HifFactory::noIfAlts() { return ifAlt_t(); }

HifFactory::instance_t HifFactory::noInstances() { return instance_t(); }

HifFactory::stateTable_t HifFactory::noStateTables() { return stateTable_t(); }

HifFactory::aggregatealt_t HifFactory::noAggregateAlts() { return aggregatealt_t(); }
// /////////////////////////////////////////////////////////////
// Types.
// /////////////////////////////////////////////////////////////
Int *HifFactory::integer(Range *span, const bool is_signed, bool isConstexpr, const Type::TypeVariant variant)
{
    messageAssert(_sem != nullptr, "Unexpected case", nullptr, nullptr);

    Int *ret = new Int();
    ret->setSpan(span);
    ret->setSigned(is_signed);
    ret->setConstexpr(isConstexpr);
    ret->setTypeVariant(variant);

    if (span == nullptr) {
        IntValue ivo(is_signed ? -1ll : 0ll);
        Type *tt = _sem->getTypeForConstant(&ivo);

        Range *s = hif::typeGetSpan(tt, _sem);
        messageAssert(s != nullptr, "Unable to get span.", tt, _sem);
        ret->setSpan(hif::copy(s));

        delete tt;
    }

    return ret;
}

Bool *HifFactory::boolean(const bool isConstexpr, const Type::TypeVariant variant)
{
    Bool *ret = new Bool();
    ret->setConstexpr(isConstexpr);
    ret->setTypeVariant(variant);
    return ret;
}

Char *HifFactory::character(const bool isConstexpr, const Type::TypeVariant variant)
{
    Char *ret = new Char();
    ret->setConstexpr(isConstexpr);
    ret->setTypeVariant(variant);
    return ret;
}

Real *HifFactory::real(Range *span, bool isConstexpr, const Type::TypeVariant variant)
{
    Real *ret = new Real();
    if (span == nullptr) {
        ret->setSpan(range(63ll, 0ll));
    } else {
        ret->setSpan(span);
    }
    ret->setConstexpr(isConstexpr);
    ret->setTypeVariant(variant);

    return ret;
}
Pointer *HifFactory::pointer(Type *type)
{
    Pointer *ret = new Pointer();
    ret->setType(type);

    return ret;
}
Time *HifFactory::time(const bool isConstexpr, const Type::TypeVariant variant)
{
    Time *ret = new Time();
    ret->setConstexpr(isConstexpr);
    ret->setTypeVariant(variant);

    return ret;
}
TypeReference *HifFactory::typeRef(const std::string &n, ReferencedType *rt)
{
    TypeReference *ret = new TypeReference();
    ret->setName(n);
    ret->setInstance(rt);

    return ret;
}

TypeReference *HifFactory::typeRef(const std::string &n, templateArgument_t t, ReferencedType *rt)
{
    TypeReference *ret = new TypeReference();
    ret->setName(n);
    ret->templateParameterAssigns.merge(t.getElements());
    ret->setInstance(rt);

    return ret;
}

Int *HifFactory::unsignedChar()
{
    Int *ret = new Int();

    ret->setSpan(range(7ll, 0ll));
    ret->setSigned(false);
    ret->setConstexpr(true);

    return ret;
}

TypeDef *HifFactory::enumTypeDef(const std::string &n, enumValue_t e)
{
    TypeDef *ret = new TypeDef();
    ret->setName(n);
    Enum *enumObj = new Enum();
    enumObj->values.merge(e.getElements());
    ret->setType(enumObj);
    // enum typedefs must be opaque
    ret->setOpaque(true);

    return ret;
}

File *HifFactory::file(Type *t)
{
    File *ret = new File();
    ret->setType(t);
    return ret;
}

TypeDef *HifFactory::recordTypeDef(const std::string &n, field_t f, const bool external)
{
    TypeDef *ret = new TypeDef();
    ret->setName(n);
    Record *record = new Record();
    record->fields.merge(f.getElements());
    ret->setType(record);
    // record typedef must be enum
    ret->setOpaque(true);
    ret->setExternal(external);

    return ret;
}

Bit *HifFactory::bit(const bool logic, const bool resolved, const bool const_expr, const Type::TypeVariant variant)
{
    Bit *b = new Bit();
    b->setLogic(logic);
    b->setResolved(resolved);
    b->setConstexpr(const_expr);
    b->setTypeVariant(variant);
    return b;
}

Bitvector *HifFactory::bitvector(
    Range *span,
    const bool logic,
    const bool resolved,
    const bool const_expr,
    const bool isSigned,
    const Type::TypeVariant variant)
{
    Bitvector *b = new Bitvector();
    b->setSpan(span);
    b->setLogic(logic);
    b->setResolved(resolved);
    b->setConstexpr(const_expr);
    b->setSigned(isSigned);
    b->setTypeVariant(variant);
    return b;
}

Signed *HifFactory::signedType(Range *span, const bool const_expr)
{
    Signed *b = new Signed();
    b->setSpan(span);
    b->setConstexpr(const_expr);
    return b;
}

String *HifFactory::string(const bool const_expr, const Type::TypeVariant variant, Range *span)
{
    String *s = new String();
    s->setSpanInformation(span);
    s->setConstexpr(const_expr);
    s->setTypeVariant(variant);
    return s;
}

Unsigned *HifFactory::unsignedType(Range *span, const bool const_expr)
{
    Unsigned *b = new Unsigned();
    b->setSpan(span);
    b->setConstexpr(const_expr);
    return b;
}

Array *HifFactory::array(Range *span, Type *type, const bool signed_)
{
    Array *a = new Array();
    a->setSpan(span);
    a->setType(type);
    a->setSigned(signed_);
    return a;
}

Reference *HifFactory::reference(Type *type)
{
    messageDebugAssert(type != nullptr, "Unexpected case", nullptr, nullptr);

    Reference *ret = new Reference();
    ret->setType(type);

    return ret;
}

HifFactory::viewReference_t HifFactory::viewRef(const std::string &du, const std::string &v, ReferencedType *i)
{
    return viewRef(du, v, i, noTemplateArguments());
}

HifFactory::viewReference_t
HifFactory::viewRef(const std::string &du, const std::string &v, ReferencedType *i, templateArgument_t t)
{
    ViewReference *ret = new ViewReference();
    ret->setDesignUnit(du);
    ret->setName(v);
    ret->setInstance(i);
    ret->templateParameterAssigns.merge(t.getElements());
    return ret;
}

Library *
HifFactory::library(const std::string &n, ReferencedType *i, const std::string &fn, const bool s, const bool sy)
{
    Library *ret = new Library();
    ret->setName(n);
    ret->setInstance(i);
    if (!n.empty())
        ret->setFilename(fn);
    ret->setStandard(s);
    ret->setSystem(sy);

    return ret;
}

// /////////////////////////////////////////////////////////////
// Values.
// /////////////////////////////////////////////////////////////
Expression *HifFactory::expression(Operator o, Value *v)
{
    messageDebugAssert(v != nullptr, "Unexpected case", nullptr, nullptr);

    Expression *ret = new Expression();
    ret->setOperator(o);
    ret->setValue1(v);

    return ret;
}

Expression *HifFactory::expression(Value *v1, Operator o, Value *v2)
{
    Expression *ret = new Expression();
    ret->setOperator(o);
    ret->setValue1(v1);
    ret->setValue2(v2);

    return ret;
}

Range *HifFactory::range(long long l, long long r)
{
    Range *ret = new Range();
    ret->setLeftBound(new IntValue(l));
    ret->setRightBound(new IntValue(r));
    ret->setDirection(l >= r ? dir_downto : dir_upto);

    return ret;
}

Range *HifFactory::range(Value *l, RangeDirection dir, Value *r)
{
    Range *ret = new Range();
    ret->setLeftBound(l);
    ret->setRightBound(r);
    ret->setDirection(dir);

    return ret;
}

StringValue *HifFactory::stringval(const std::string &t, bool plain, Type *syntactic_type)
{
    StringValue *ret = new StringValue();
    ret->setValue(t);
    ret->setPlain(plain);
    if (syntactic_type == nullptr) {
        messageAssert(_sem != nullptr, "Expected semantics", nullptr, nullptr);
        ret->setType(_sem->getTypeForConstant(ret));
    } else
        ret->setType(syntactic_type);

    hif::typeSetConstexpr(ret->getType(), true);
    return ret;
}

BitValue *HifFactory::bitval(const hif::BitConstant b, Bit *syntactic_type)
{
    BitValue *ret = new BitValue(b);
    if (syntactic_type == nullptr) {
        messageAssert(_sem != nullptr, "Expected semantics", nullptr, nullptr);
        ret->setType(_sem->getTypeForConstant(ret));
    } else
        ret->setType(syntactic_type);

    hif::typeSetConstexpr(ret->getType(), true);
    return ret;
}

BitValue *HifFactory::bitval(const char b, Bit *syntactic_type)
{
    BitValue *bv = bitval(bit_zero, syntactic_type);
    bv->setValue(b);
    return bv;
}

BitvectorValue *HifFactory::bitvectorval(const std::string &b, Bitvector *syntactic_type)
{
    BitvectorValue *ret = new BitvectorValue(b);
    if (syntactic_type == nullptr) {
        messageAssert(_sem != nullptr, "Expected semantics", nullptr, nullptr);
        ret->setType(_sem->getTypeForConstant(ret));
    } else
        ret->setType(syntactic_type);

    hif::typeSetConstexpr(ret->getType(), true);
    return ret;
}

BoolValue *HifFactory::boolval(const bool b, Bool *syntactic_type)
{
    BoolValue *ret = new BoolValue(b);
    if (syntactic_type == nullptr) {
        messageAssert(_sem != nullptr, "Expected semantics", nullptr, nullptr);
        ret->setType(_sem->getTypeForConstant(ret));
    } else
        ret->setType(syntactic_type);

    hif::typeSetConstexpr(ret->getType(), true);
    return ret;
}

Identifier *HifFactory::identifier(const std::string &id) { return new Identifier(id); }

IntValue *HifFactory::intval(long long n, Type *syntactic_type)
{
    IntValue *ret = new IntValue(n);
    if (syntactic_type == nullptr) {
        messageAssert(_sem != nullptr, "Expected semantics", nullptr, nullptr);
        ret->setType(_sem->getTypeForConstant(ret));
    } else
        ret->setType(syntactic_type);

    hif::typeSetConstexpr(ret->getType(), true);
    return ret;
}

IntValue *HifFactory::intval(unsigned long long n, Type *syntactic_type)
{
    return intval(static_cast<long long>(n), syntactic_type);
}

IntValue *HifFactory::intval(long n, Type *syntactic_type) { return intval(static_cast<long long>(n), syntactic_type); }

IntValue *HifFactory::intval(unsigned long n, Type *syntactic_type)
{
    return intval(static_cast<long long>(n), syntactic_type);
}

IntValue *HifFactory::intval(int n, Type *syntactic_type) { return intval(static_cast<long long>(n), syntactic_type); }

IntValue *HifFactory::intval(unsigned int n, Type *syntactic_type)
{
    return intval(static_cast<long long>(n), syntactic_type);
}

CharValue *HifFactory::charval(char c, Char *syntactic_type)
{
    messageAssert(_sem != nullptr, "Unexpected case", nullptr, nullptr);

    CharValue *ret = new CharValue(c);
    if (syntactic_type == nullptr)
        ret->setType(_sem->getTypeForConstant(ret));
    else
        ret->setType(syntactic_type);

    return ret;
}

Event *HifFactory::event(bool isConstexpr, const Type::TypeVariant variant)
{
    Event *ret = new Event();
    ret->setConstexpr(isConstexpr);
    ret->setTypeVariant(variant);

    return ret;
}
RealValue *HifFactory::realval(const double d, Real *syntactic_type)
{
    RealValue *ret = new RealValue(d);
    if (syntactic_type == nullptr) {
        messageAssert(_sem != nullptr, "Expected semantics", nullptr, nullptr);
        ret->setType(_sem->getTypeForConstant(ret));
    } else
        ret->setType(syntactic_type);

    hif::typeSetConstexpr(ret->getType(), true);
    return ret;
}

RecordValue *HifFactory::recordval(recordvaluealt_t rv)
{
    RecordValue *ret = new RecordValue();
    ret->alts.merge(rv.getElements());

    return ret;
}

HifFactory::recordvaluealt_t HifFactory::recordvaluealt(const std::string &n, Value *v)
{
    recordvaluealt_t ret;

    RecordValueAlt *r = new RecordValueAlt();
    r->setName(n);
    r->setValue(v);

    ret._elements.push_back(r);
    return ret;
}

FieldReference *HifFactory::fieldRef(const std::string &prefix, const std::string &field_name)
{
    FieldReference *ret = new FieldReference();
    ret->setPrefix(new Identifier(prefix));
    ret->setName(field_name);

    return ret;
}

FieldReference *HifFactory::fieldRef(Value *prefix, const std::string &field_name)
{
    messageAssert(prefix != nullptr, "Unexpected case (1)", nullptr, nullptr);

    FieldReference *ret = new FieldReference();
    ret->setPrefix(prefix);
    ret->setName(field_name);

    return ret;
}

Instance *
HifFactory::instance(ReferencedType *refType, const std::string &name, portassign_t portassigns, Value *initialValue)
{
    messageAssert(refType != nullptr, "Unexpected case (1)", nullptr, nullptr);

    Instance *ret = new Instance();
    ret->setReferencedType(refType);
    ret->setName(name);
    ret->setValue(initialValue);
    ret->portAssigns.merge(portassigns.getElements());

    return ret;
}

Instance *HifFactory::libraryInstance(const std::string &name, const bool s, const bool sy)
{
    Library *refType = new Library();
    refType->setName(name);
    refType->setStandard(s);
    refType->setSystem(sy);

    Instance *ret = new Instance();
    ret->setReferencedType(refType);
    ret->setName(name);

    return ret;
}

Member *HifFactory::member(Value *prefix, Value *index)
{
    messageDebugAssert(prefix != nullptr, "Unexpected case (1)", nullptr, nullptr);
    messageDebugAssert(index != nullptr, "Unexpected case (2)", nullptr, nullptr);

    Member *ret = new Member();
    ret->setPrefix(prefix);
    ret->setIndex(index);

    return ret;
}

FunctionCall *HifFactory::functionCall(const std::string &n, Value *inst, templateArgument_t t, parameterArgument_t p)
{
    FunctionCall *ret = new FunctionCall();
    ret->setName(n);
    ret->templateParameterAssigns.merge(t.getElements());
    ret->parameterAssigns.merge(p.getElements());
    ret->setInstance(inst);

    return ret;
}

Cast *HifFactory::cast(Type *t, Value *v)
{
    messageDebugAssert(t != nullptr, "Unexpected case (1)", nullptr, nullptr);
    messageDebugAssert(v != nullptr, "Unexpected case (2)", nullptr, nullptr);

    Cast *ret = new Cast();
    ret->setType(t);
    ret->setValue(v);

    return ret;
}

HifFactory::templateArgument_t HifFactory::templateTypeArgument(const std::string &n, Type *t)
{
    templateArgument_t ret;
    TypeTPAssign *ttpa = new TypeTPAssign();
    ttpa->setName(n);
    ttpa->setType(t);
    ret._elements.push_back(ttpa);

    return ret;
}

HifFactory::templateArgument_t HifFactory::templateValueArgument(const std::string &n, Value *v)
{
    messageDebugAssert(v != nullptr, "Unexpected case (2)", nullptr, nullptr);

    templateArgument_t ret;
    ValueTPAssign *vtpa = new ValueTPAssign();
    vtpa->setName(n);
    vtpa->setValue(v);
    ret._elements.push_back(vtpa);

    return ret;
}

HifFactory::parameterArgument_t HifFactory::parameterArgument(const std::string &n, Value *v)
{
    parameterArgument_t ret;
    ParameterAssign *pa = new ParameterAssign();
    pa->setName(n);
    pa->setValue(v);
    ret._elements.push_back(pa);

    return ret;
}

Range *HifFactory::range(unsigned long long l, unsigned long long r)
{
    return range(static_cast<long long>(l), static_cast<long long>(r));
}

Range *HifFactory::range(const int l, const int r)
{
    return range(static_cast<long long>(l), static_cast<long long>(r));
}

Range *HifFactory::range(unsigned int l, unsigned int r)
{
    return range(static_cast<long long>(l), static_cast<long long>(r));
}

Range *HifFactory::range(long long l, const int r) { return range(l, static_cast<long long>(r)); }

Range *HifFactory::range(const int l, long long r) { return range(static_cast<long long>(l), r); }

When *HifFactory::when(whenAlt_t alts, Value *def, const bool logicTernary)
{
    When *w = new When();
    w->setLogicTernary(logicTernary);
    w->setDefault(def);
    w->alts.merge(alts.getElements());
    return w;
}

HifFactory::whenAlt_t HifFactory::whenalt(Value *cond, Value *value)
{
    whenAlt_t ret;
    WhenAlt *wa = new WhenAlt();
    wa->setCondition(cond);
    wa->setValue(value);
    ret._elements.push_back(wa);
    return ret;
}

While *HifFactory::whileLoop(Value *cond, HifFactory::action_t actions, const std::string &label, const bool doWhile)
{
    While *ret = new While();
    ret->setCondition(cond);
    ret->actions.merge(actions.getElements());
    if (!label.empty())
        ret->setName(label);
    ret->setDoWhile(doWhile);
    return ret;
}

Alias *HifFactory::alias(const std::string &name, Type *t, Value *v)
{
    Alias *ret = new Alias();
    ret->setType(t);
    ret->setName(name);
    ret->setValue(v);
    return ret;
}

Aggregate *HifFactory::aggregate(Value *o, aggregatealt_t alts)
{
    Aggregate *ret = new Aggregate();
    ret->setOthers(o);
    ret->alts.merge(alts.getElements());
    return ret;
}

HifFactory::aggregatealt_t HifFactory::aggregatealt(Value *i, Value *v)
{
    AggregateAlt *alt = new AggregateAlt();
    alt->setValue(v);
    alt->indices.push_back(i);

    aggregatealt_t ret;
    ret._elements.push_back(alt);
    return ret;
}

Aggregate *HifFactory::arrayInitVal(Value *v)
{
    messageDebugAssert(v != nullptr, "Unexpected case", nullptr, nullptr);

    Aggregate *ret = new Aggregate();
    ret->setOthers(v);

    return ret;
}

TimeValue *HifFactory::timeval(double v, TimeValue::TimeUnit u, Time *syntactic_type)
{
    TimeValue *ret = new TimeValue();
    ret->setValue(v);
    ret->setUnit(u);
    if (syntactic_type == nullptr) {
        messageAssert(_sem != nullptr, "Expected semantics", nullptr, nullptr);
        ret->setType(_sem->getTypeForConstant(ret));
    } else
        ret->setType(syntactic_type);

    hif::typeSetConstexpr(ret->getType(), true);
    return ret;
}

ValueStatement *HifFactory::valueStatement(Value *value)
{
    ValueStatement *ret = new ValueStatement();
    ret->setValue(value);
    return ret;
}

Cast *HifFactory::nullval(Type *t, const bool autoPtr)
{
    messageDebugAssert(t != nullptr, "Unexpected case", nullptr, nullptr);
    if (autoPtr)
        return cast(pointer(t), intval(0ll, integer()));

    return cast(t, intval(0ll, integer()));
}

Slice *HifFactory::slice(Value *prefix, Range *span)
{
    messageDebugAssert(prefix != nullptr, "Unexpected case (1)", nullptr, nullptr);
    messageDebugAssert(span != nullptr, "Unexpected case (2)", nullptr, nullptr);

    Slice *ret = new Slice();
    ret->setPrefix(prefix);
    ret->setSpan(span);

    return ret;
}
// /////////////////////////////////////////////////////////////
// Declarations.
// /////////////////////////////////////////////////////////////
HifFactory::parameter_t
HifFactory::parameter(Type *t, const std::string &n, Value *v, Range *r, const PortDirection dir)
{
    messageDebugAssert(t != nullptr, "Unexpected case (1)", nullptr, nullptr);

    parameter_t ret;
    Parameter *tp = new Parameter();
    tp->setName(n);
    tp->setType(t);
    tp->setValue(v);
    tp->setRange(r);
    tp->setDirection(dir);
    ret._elements.push_back(tp);

    return ret;
}

Port *HifFactory::port(Type *t, const std::string &n, PortDirection d, Value *init, Range *r, const bool w)
{
    messageDebugAssert(t != nullptr, "Unexpected case (1)", nullptr, nullptr);

    Port *ret = new Port();
    ret->setType(t);
    ret->setName(n);
    ret->setDirection(d);
    ret->setRange(r);
    if (init)
        ret->setValue(init);
    ret->setWrapper(w);

    return ret;
}

HifFactory::portassign_t HifFactory::portAssign(const std::string &n, Value *v, PortDirection d, Type *t)
{
    portassign_t ret;

    PortAssign *p = new PortAssign();
    p->setName(n);
    p->setValue(v);
    p->setDirection(d);
    p->setType(t);
    ret._elements.push_back(p);

    return ret;
}

Signal *HifFactory::signal(Type *t, const std::string &n, Value *init, Range *r, const bool s, const bool w)
{
    messageDebugAssert(t != nullptr, "Unexpected case (1)", nullptr, nullptr);

    Signal *ret = new Signal();
    ret->setType(t);
    ret->setName(n);
    ret->setRange(r);
    if (init)
        ret->setValue(init);
    ret->setStandard(s);
    ret->setWrapper(w);

    return ret;
}

HifFactory::stateTable_t HifFactory::stateTable(
    const std::string &n,
    HifFactory::declaration_t decls,
    HifFactory::action_t actions,
    const bool dontInitialize,
    const ProcessFlavour pf)
{
    stateTable_t ret;
    StateTable *st = new StateTable();
    st->setName(n);

    State *s = new State();
    s->setName(st->getName());
    s->actions.merge(actions.getElements());
    st->states.push_back(s);

    st->declarations.merge(decls.getElements());
    st->setDontInitialize(dontInitialize);
    st->setFlavour(pf);

    ret._elements.push_back(st);
    return ret;
}

SubProgram *HifFactory::subprogram(Type *t, const std::string &n, template_t tp, parameter_t p)
{
    SubProgram *ret =
        (t != nullptr) ? static_cast<SubProgram *>(new Function()) : static_cast<SubProgram *>(new Procedure());
    ret->setName(n);
    ret->templateParameters.merge(tp.getElements());
    ret->parameters.merge(p.getElements());
    if (t != nullptr)
        static_cast<Function *>(ret)->setType(t);

    return ret;
}

HifFactory::template_t HifFactory::templateTypeParameter(Type *t, const std::string &n)
{
    template_t ret;
    TypeTP *tp = new TypeTP();
    tp->setName(n);
    tp->setType(t);
    ret._elements.push_back(tp);

    return ret;
}

HifFactory::template_t
HifFactory::templateValueParameter(Type *t, const std::string &n, Value *v, Range *r, const bool compileTimeConstant)
{
    messageDebugAssert(t != nullptr, "Unexpected case (1)", nullptr, nullptr);

    template_t ret;
    ValueTP *tp = new ValueTP();
    tp->setName(n);
    tp->setType(t);
    tp->setValue(v);
    tp->setRange(r);
    tp->setCompileTimeConstant(compileTimeConstant);
    ret._elements.push_back(tp);

    return ret;
}

TypeDef *HifFactory::typeDef(const std::string &n, Type *t, const bool opaque, Range *r, const bool external)
{
    TypeDef *ret = new TypeDef();
    ret->setName(n);
    ret->setType(t);
    ret->setOpaque(opaque);
    ret->setRange(r);
    ret->setExternal(external);

    return ret;
}

TypeDef *HifFactory::typeDef(const std::string &n, Type *t, const bool opaque, template_t tp, const bool external)
{
    TypeDef *ret = new TypeDef();
    ret->setName(n);
    ret->setType(t);
    ret->setOpaque(opaque);
    ret->templateParameters.merge(tp.getElements());
    ret->setExternal(external);

    return ret;
}

HifFactory::enumValue_t HifFactory::enumValue(TypeReference *tr, const std::string &n, Value *v, Range *r)
{
    enumValue_t ret;
    EnumValue *ev = new EnumValue();
    ev->setName(n);
    ev->setType(tr);
    ev->setValue(v);
    ev->setRange(r);
    ret._elements.push_back(ev);

    return ret;
}

Variable *HifFactory::variable(Type *t, const std::string &n, Value *init, Range *r)
{
    messageDebugAssert(t != nullptr, "Unexpected case (1)", nullptr, nullptr);

    Variable *ret = new Variable();
    ret->setType(t);
    ret->setName(n);
    ret->setRange(r);
    if (init)
        ret->setValue(init);

    return ret;
}

HifFactory::declaration_t HifFactory::variableDecl(Type *t, const std::string &n, Value *init, Range *r)
{
    Variable *v = variable(t, n, init, r);
    declaration_t ret;
    ret._elements.push_back(v);

    return ret;
}

Contents *
HifFactory::contents(GlobalAction *ga, declaration_t d, generate_t g, instance_t i, stateTable_t s, library_t l)
{
    Contents *ret      = new Contents();
    GlobalAction *gact = (ga != nullptr) ? ga : new GlobalAction();
    ret->setGlobalAction(gact);
    ret->libraries.merge(l.getElements());
    ret->declarations.merge(d.getElements());
    ret->generates.merge(g.getElements());
    ret->instances.merge(i.getElements());
    ret->stateTables.merge(s.getElements());
    ret->libraries.merge(l.getElements());
    return ret;
}

View *HifFactory::view(
    const std::string &n,
    Contents *c,
    Entity *e,
    LanguageID lid,
    declaration_t d,
    library_t l,
    template_t t,
    const bool s,
    const std::string &fileName)
{
    return view(n, c, e, lid, d, l, t, s, noViewReferences(), fileName);
}

View *HifFactory::view(
    const std::string &n,
    Contents *c,
    Entity *e,
    LanguageID lid,
    declaration_t d,
    library_t l,
    template_t t,
    const bool s,
    viewReference_t parents,
    const std::string &fileName)
{
    View *ret = new View();
    ret->setName(n);
    ret->setContents((c != nullptr) ? c : new Contents());
    ret->setEntity((e != nullptr) ? e : new Entity());
    ret->setStandard(s);
    ret->setLanguageID(lid);
    ret->setFilename(fileName);
    ret->declarations.merge(d.getElements());
    ret->libraries.merge(l.getElements());
    ret->templateParameters.merge(t.getElements());
    ret->inheritances.merge(parents.getElements());
    return ret;
}

Const *HifFactory::constant(Type *t, const std::string &n, Value *init, Range *r)
{
    messageDebugAssert(t != nullptr, "Unexpected case (1)", nullptr, nullptr);
    messageDebugAssert(init != nullptr, "Unexpected case (2)", nullptr, nullptr);

    Const *ret = new Const();
    ret->setType(t);
    ret->setName(n);
    ret->setValue(init);
    ret->setRange(r);

    return ret;
}

HifFactory::field_t
HifFactory::field(Type *t, const std::string &n, Value *init, Range *r, const hif::PortDirection dir)
{
    messageDebugAssert(t != nullptr, "Unexpected case (1)", nullptr, nullptr);

    field_t ret;
    Field *f = new Field();
    f->setType(t);
    f->setName(n);
    f->setRange(r);
    f->setDirection(dir);
    if (init)
        f->setValue(init);
    ret._elements.push_back(f);

    return ret;
}

DesignUnit *HifFactory::designUnit(const std::string &u, const std::string &v, hif::LanguageID lid)
{
    DesignUnit *ret = new DesignUnit();
    ret->setName(u);
    View *vi = view(v, nullptr, nullptr, lid, noDeclarations(), noLibraries(), noTemplates());
    ret->views.push_back(vi);
    return ret;
}

DesignUnit *HifFactory::designUnit(const std::string &u, View *v)
{
    DesignUnit *ret = new DesignUnit();
    ret->setName(u);
    ret->views.push_back(v);
    return ret;
}

// /////////////////////////////////////////////////////////////
// Actions.
// /////////////////////////////////////////////////////////////

HifFactory::action_t HifFactory::assignAction(Value *left, Value *right, TimeValue *delay)
{
    HifFactory::action_t ret;
    Assign *ass = assignment(left, right, delay);
    ret._elements.push_back(ass);

    return ret;
}

Assign *HifFactory::assignment(Value *left, Value *right, TimeValue *delay)
{
    messageDebugAssert(left != nullptr, "Unexpected case (1)", nullptr, nullptr);
    messageDebugAssert(right != nullptr, "Unexpected case (2)", nullptr, nullptr);

    Assign *ret = new Assign();
    ret->setLeftHandSide(left);
    ret->setRightHandSide(right);
    ret->setDelay(delay);

    return ret;
}

HifFactory::action_t
HifFactory::procedureCallAction(const std::string &n, Value *inst, templateArgument_t t, parameterArgument_t p)
{
    HifFactory::action_t ret;
    ProcedureCall *pc = procedureCall(n, inst, t, p);
    ret._elements.push_back(pc);

    return ret;
}
ProcedureCall *HifFactory::procedureCall(const std::string &n, Value *inst, templateArgument_t t, parameterArgument_t p)
{
    ProcedureCall *ret = new ProcedureCall();
    ret->setName(n);
    ret->templateParameterAssigns.merge(t.getElements());
    ret->parameterAssigns.merge(p.getElements());
    ret->setInstance(inst);
    return ret;
}
For *HifFactory::forLoop(
    const std::string &label,
    BList<DataDeclaration> &initDecls,
    BList<Action> &initVals,
    Value *condition,
    BList<Action> &stepAct,
    BList<Action> &forAct)
{
    messageDebugAssert(initDecls.empty() || initVals.empty(), "Unexpected case", nullptr, nullptr);

    For *ret = new For();
    ret->setName(label);
    ret->initDeclarations.merge(initDecls);
    ret->initValues.merge(initVals);
    ret->setCondition(condition);
    ret->stepActions.merge(stepAct);
    ret->forActions.merge(forAct);

    return ret;
}

If *HifFactory::ifStmt(action_t d, ifAlt_t as)
{
    If *ret = new If();
    ret->defaults.merge(d.getElements());
    ret->alts.merge(as.getElements());

    messageDebugAssert(!ret->defaults.empty() || !ret->alts.empty(), "Unexpected case", nullptr, nullptr);

    return ret;
}

HifFactory::action_t HifFactory::ifAction(HifFactory::action_t d, HifFactory::ifAlt_t as)
{
    HifFactory::action_t ret;
    Action *act = ifStmt(d, as);
    ret._elements.push_back(act);

    return ret;
}

IfAlt *HifFactory::ifAlt(Value *c, action_t a)
{
    messageDebugAssert(c != nullptr, "Unexpected case (1)", nullptr, nullptr);

    IfAlt *ret = new IfAlt();
    ret->setCondition(c);
    ret->actions.merge(a.getElements());
    return ret;
}

Return *HifFactory::retStmt(Value *v, Type *t)
{
    messageDebugAssert((v != nullptr) ^ (t != nullptr), "Unexpected case", nullptr, nullptr);
    Return *r = new Return();
    if (v != nullptr)
        r->setValue(v);
    else
        r->setValue(nullval(t));
    return r;
}

HifFactory::action_t HifFactory::returnAction(Value *v, Type *t)
{
    HifFactory::action_t ret;
    Action *act = retStmt(v, t);
    ret._elements.push_back(act);

    return ret;
}

HifFactory::action_t HifFactory::waitAction(Value *timeVal, Value *cond, Value *repetitions)
{
    Wait *w = new Wait();
    w->setTime(timeVal);
    w->setCondition(cond);
    w->setRepetitions(repetitions);

    HifFactory::action_t ret;
    ret._elements.push_back(w);
    return ret;
}

HifFactory::action_t
HifFactory::waitAction(Value *timeVal, Value *cond, Value *repetitions, HifFactory::action_t &actions)
{
    auto ret = waitAction(timeVal, cond, repetitions);
    auto w   = dynamic_cast<Wait *>(ret._elements.front());
    w->actions.merge(actions.getElements());
    return ret;
}
// /////////////////////////////////////////////////////////////
// Others.
// /////////////////////////////////////////////////////////////

Function *HifFactory::classConstructor(DesignUnit *du, BList<Parameter> &pp, BList<Declaration> &tp)
{
    View *v = du->views.front();
    if (v == nullptr || v->getLanguageID() != cpp) {
        messageAssert(false, "Unexpected lang id", v, nullptr);
    }

    Function *ctor = new Function();
    ctor->setName(hif::NameTable::getInstance()->hifConstructor());
    ViewReference *vr = viewRef(du->getName(), v->getName());
    // TODO: check. Break design crc vhdl
    for (BList<Declaration>::iterator it(v->templateParameters.begin()); it != v->templateParameters.end(); ++it) {
        if (dynamic_cast<TypeTP *>(*it) != nullptr) {
            TypeTPAssign *tpa = new TypeTPAssign();
            tpa->setName((*it)->getName());
            tpa->setType(typeRef((*it)->getName()));
            vr->templateParameterAssigns.push_back(tpa);
        } else {
            ValueTPAssign *vta = new ValueTPAssign();
            vta->setName((*it)->getName());
            vta->setValue(new Identifier((*it)->getName()));
            vr->templateParameterAssigns.push_back(vta);
        }
    }
    ctor->setType(vr);
    ctor->parameters.merge(pp);
    ctor->templateParameters.merge(tp);
    return ctor;
}
Function *HifFactory::classConstructor(DesignUnit *du, parameter_t pp, template_t tp)
{
    View *v = du->views.front();
    if (v == nullptr) {
        messageError("View not found", v, nullptr);
    }

    Function *ctor = new Function();
    ctor->setName(hif::NameTable::getInstance()->hifConstructor());
    ViewReference *vr = viewRef(du->getName(), v->getName());
    // TODO: check. Break design crc vhdl
    for (BList<Declaration>::iterator it(v->templateParameters.begin()); it != v->templateParameters.end(); ++it) {
        if (dynamic_cast<TypeTP *>(*it) != nullptr) {
            TypeTPAssign *tpa = new TypeTPAssign();
            tpa->setName((*it)->getName());
            tpa->setType(typeRef((*it)->getName()));
            vr->templateParameterAssigns.push_back(tpa);
        } else {
            ValueTPAssign *vta = new ValueTPAssign();
            vta->setName((*it)->getName());
            vta->setValue(new Identifier((*it)->getName()));
            vr->templateParameterAssigns.push_back(vta);
        }
    }

    ctor->setType(vr);
    ctor->parameters.merge(pp.getElements());
    ctor->templateParameters.merge(tp.getElements());
    return ctor;
}
Procedure *HifFactory::classDestructor(DesignUnit *du)
{
    View *v = du->views.front();
    if (v == nullptr || v->getLanguageID() != cpp) {
        messageError("Unexpected lang id", v, nullptr);
    }

    Procedure *dtor = new Procedure();
    dtor->setName(hif::NameTable::getInstance()->hifDestructor());
    return dtor;
}
FunctionCall *HifFactory::classConstructorCall(
    DesignUnit *du,
    const std::string &instName,
    BList<ParameterAssign> parameterAssigns,
    BList<TPAssign> tpassigns)
{
    View *v = du->views.front();
    if (v == nullptr || v->getLanguageID() != cpp) {
        messageError("Unexpected lang id", v, nullptr);
    }

    Instance *inst = new Instance();
    inst->setName(instName);
    inst->setReferencedType(viewRef(du->getName(), v->getName()));
    FunctionCall *ctorCall = new FunctionCall();
    ctorCall->setName(hif::NameTable::getInstance()->hifConstructor());
    ctorCall->setInstance(inst);
    ctorCall->parameterAssigns.merge(parameterAssigns);
    ctorCall->templateParameterAssigns.merge(tpassigns);
    return ctorCall;
}

FunctionCall *HifFactory::classConstructorCall(
    DesignUnit *du,
    const std::string &instName,
    HifFactory::parameterArgument_t p,
    HifFactory::templateArgument_t t)
{
    return classConstructorCall(du, instName, p.getElements(), t.getElements());
}
FunctionCall *
HifFactory::classConstructorCall(const std::string &n, ReferencedType *iT, parameterArgument_t p, templateArgument_t t)
{
    FunctionCall *ctorCall = new FunctionCall();
    ctorCall->setName(hif::NameTable::getInstance()->hifConstructor());
    ctorCall->setInstance(instance(iT, n, noPortAssigns()));
    ctorCall->parameterAssigns.merge(p.getElements());
    ctorCall->templateParameterAssigns.merge(t.getElements());
    return ctorCall;
}

/// @brief Helper macro to instantiate the `codeInfo` method template for all HIF object types.
/// @details This macro generates explicit template instantiations for the
/// `HifFactory::codeInfo` method template, which associates code metadata (file name, line, and column)
/// with an object of type `T`.
#define HIF_TEMPLATE_METHOD(T)                                                                                         \
    T *HifFactory::codeInfo<T>(T * o, const std::string &fileName, unsigned int line, unsigned int col)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

/// @brief Helper macro to instantiate the `codeInfo` method template for all HIF object types.
/// @details This macro generates explicit template instantiations for the
/// `HifFactory::codeInfo` method template, which associates an `Object::CodeInfo` instance
/// with an object of type `T`.
#define HIF_TEMPLATE_METHOD(T) T *HifFactory::codeInfo<T>(T * o, const Object::CodeInfo &)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `Action`.
template class ListElement<Action>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `AggregateAlt`.
template class ListElement<AggregateAlt>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `Declaration`.
template class ListElement<Declaration>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `EnumValue`.
template class ListElement<EnumValue>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `Field`.
template class ListElement<Field>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `Generate`.
template class ListElement<Generate>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `If`.
template class ListElement<If>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `IfAlt`.
template class ListElement<IfAlt>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `Instance`.
template class ListElement<Instance>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `Library`.
template class ListElement<Library>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `Parameter`.
template class ListElement<Parameter>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `ParameterAssign`.
template class ListElement<ParameterAssign>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `PortAssign`.
template class ListElement<PortAssign>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `RecordValueAlt`.
template class ListElement<RecordValueAlt>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `StateTable`.
template class ListElement<StateTable>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `TPAssign`.
template class ListElement<TPAssign>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `WhenAlt`.
template class ListElement<WhenAlt>;

/// @relates ListElement
/// @brief Explicit instantiation of the `ListElement` class template for `ViewReference`.
template class ListElement<ViewReference>;

} // namespace hif
