/// @file HifFactory.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

#if (defined _MSC_VER)
#else
#    if HIF_DIAGNOSTIC_PUSH_POP
#        pragma GCC diagnostic push
#    endif
#    pragma GCC diagnostic ignored "-Weffc++"
#endif
namespace hif
{
namespace semantics
{
class ILanguageSemantics;
}
} // namespace hif

namespace hif
{

/// @class ListElementHost
/// @brief A helper class for managing a list of objects in a HIF factory.
/// @details Provides list operations with special semantics, including conversion
/// to `Object *` and concatenation of lists.
class ListElementHost
{
public:
    /// @brief Default destructor.
    virtual ~ListElementHost();

    /// @brief Copy constructor.
    /// @warning This may involve move-like behavior; ensure correctness in usage.
    ListElementHost(const ListElementHost &t);

    /// @brief Copy assignment operator.
    /// @warning This may involve move-like behavior; ensure correctness in usage.
    ListElementHost &operator=(const ListElementHost &t);

    /// @brief Concatenation operator for combining lists.
    /// @details Appends the elements of the given `ListElementHost` to the current one.
    /// @warning Semantics resemble move-like behavior; ensure the source remains valid.
    ListElementHost operator,(const ListElementHost &t2);

    /// @brief Conversion operator to `Object *`.
    /// @warning Returns a pointer to a single `Object`. Ensure the object exists in the list.
    operator Object *() const;

protected:
    /// @brief Default constructor.
    ListElementHost();

    /// @brief Stores the list of `Object` pointers managed by this class.
    BList<Object> _elements;

    /// @brief Grants access to private members to the `HifFactory` class.
    friend class HifFactory;
};

/// @class ListElement
/// @brief A templated class for managing lists of objects in a HIF factory.
/// @details
/// This class is a specialization of `ListElementHost` for specific types.
/// It provides functionality for storing and managing a list of elements of type `T`,
/// along with type-specific operations.
/// @note This class is not intended for direct use.
template <class T> class ListElement : public ListElementHost
{
public:
    /// @brief Default constructor.
    /// @details Creates an empty `ListElement` instance.
    ListElement();

    /// @brief Constructor with a single element.
    /// @param e Pointer to an object of type `T` to initialize the list with.
    ListElement(T *e);

    /// @brief Constructor with an existing list.
    /// @param l Reference to a `BList` containing elements of type `T`.
    ListElement(BList<T> &l);

    /// @brief Default destructor.
    /// @details Cleans up resources associated with the list.
    virtual ~ListElement();

    /// @brief Copy constructor.
    /// @warning May involve move-like semantics; ensure proper use.
    /// @param t Reference to the `ListElement` to copy.
    ListElement(const ListElement<T> &t);

    /// @brief Copy assignment operator.
    /// @warning May involve move-like semantics; ensure proper use.
    /// @param t Reference to the `ListElement` to assign from.
    /// @return Reference to the assigned object.
    ListElement<T> &operator=(const ListElement<T> &t);

    /// @brief Concatenation operator.
    /// @details Combines two `ListElement` instances by appending the elements of `t2` to the current list.
    /// @param t2 The second `ListElement` to concatenate.
    /// @return A new `ListElement` containing the combined elements.
    /// @warning May involve move-like semantics; ensure proper use.
    ListElement<T> operator,(const ListElement<T> &t2);

    /// @brief Conversion operator to `T *`.
    /// @details Provides access to the first element in the list.
    /// @warning Ensure the list is not empty before using this operator.
    operator T *() const;

    /// @brief Gets the internal list of elements.
    /// @return A reference to the `BList` of elements.
    BList<T> &getElements();

    /// @brief Retrieves the first object in the list.
    /// @return A pointer to the first object of type `T`, or `nullptr` if the list is empty.
    T *getObject();

protected:
    /// @brief Allows `HifFactory` to access private members.
    friend class HifFactory;
};

/// @brief This class helps to create hierarchies of HIF objects quickly.
///
/// @details
/// It provides methods and types to be used as C++-like declarations.
/// For example, given an instance f of this class, to create the procedure void foo(int i ),
/// just write:
/// f.subprogram(///    f.noType(),
///    "foo",
///    f.noTemplates(),
///    (f.parameter(f.integer(), "i"))
///);
///
class HifFactory
{
public:
    /// @name Traits & types.
    /// @{

    typedef ListElement<Action> action_t;
    typedef ListElement<AggregateAlt> aggregatealt_t;
    typedef ListElement<Declaration> declaration_t;
    typedef ListElement<Declaration> template_t;
    typedef ListElement<EnumValue> enumValue_t;
    typedef ListElement<Field> field_t;
    typedef ListElement<Generate> generate_t;
    typedef ListElement<IfAlt> ifAlt_t;
    typedef ListElement<Instance> instance_t;
    typedef ListElement<Library> library_t;
    typedef ListElement<RecordValueAlt> recordvaluealt_t;
    typedef ListElement<Parameter> parameter_t;
    typedef ListElement<ParameterAssign> parameterArgument_t;
    typedef ListElement<PortAssign> portassign_t;
    typedef ListElement<StateTable> stateTable_t;
    typedef ListElement<TPAssign> templateArgument_t;
    typedef ListElement<WhenAlt> whenAlt_t;
    typedef ListElement<ViewReference> viewReference_t;

    /// @}

    HifFactory();

    explicit HifFactory(hif::semantics::ILanguageSemantics *sem);

    ~HifFactory();

    HifFactory(const HifFactory &);

    HifFactory &operator=(const HifFactory &);

    void setSemantics(hif::semantics::ILanguageSemantics *sem);
    hif::semantics::ILanguageSemantics *getSemantics();
    /// @name Helper methods.
    /// @{

    /// @brief Used to avoid actions.
    action_t noActions();

    /// @brief Used to avoid AggregateAlts.
    aggregatealt_t noAggregateAlts();

    /// @brief Used to avoid declarations.
    declaration_t noDeclarations();

    /// @brief Used to avoid generates.
    generate_t noGenerates();

    /// @brief Used to avoid alternatives.
    ifAlt_t noIfAlts();

    /// @brief Used to avoid instances.
    instance_t noInstances();

    /// @brief Used to avoid libraries.
    library_t noLibraries();

    /// @brief Used to avoid parameters.
    parameter_t noParameters();

    /// @brief Used to avoid parameter arguments.
    parameterArgument_t noParameterArguments();

    /// @brief Used to avoid portassign arguments.
    portassign_t noPortAssigns();

    /// @brief Used to avoid alternatives.
    recordvaluealt_t noRecordValueAlts();

    /// @brief Used to avoid StateTables.
    stateTable_t noStateTables();

    /// @brief Used to avoid template arguments.
    templateArgument_t noTemplateArguments();

    /// @brief Used to avoid templates.
    template_t noTemplates();

    /// @brief Used to avoid the type.
    Type *noType();

    /// @brief Used to avoid the values.
    Value *noValue();

    /// @brief Used to avoid alternatives.
    whenAlt_t noWhenAlts();

    /// @brief Used to avoid fields.
    field_t noFields();

    /// @brief Used to avoid fields.
    static viewReference_t noViewReferences();

    Object *codeInfo(Object *o, const std::string &fileName, unsigned int line, unsigned int col = 0);

    template <typename T> T *codeInfo(T *o, const std::string &fileName, unsigned int line, unsigned int col = 0);

    template <typename T> T *codeInfo(T *o, const Object::CodeInfo &info);

    /// @}
    /// @name Types.
    /// @{

    /// @brief Creates an array.
    /// @param span the span of the array.
    /// @param type the type of the array.
    /// @param signed_ true if the array is signed, false otherwise.
    Array *array(Range *span = nullptr, Type *type = nullptr, const bool signed_ = false);

    /// @brief Creates a bit.
    /// @param logic true if the bit type is logic, false otherwise.
    /// @param resolved true if the bit type is resolved, false otherwise.
    /// @param const_expr true if the bit type is const_expr, false otherwise.
    /// @param variant The type variant.
    Bit *
    bit(const bool logic                = false,
        const bool resolved             = false,
        const bool const_expr           = false,
        const Type::TypeVariant variant = Type::NATIVE_TYPE);

    /// @brief Creates a bitvector.
    /// @param span the span of the bitvector.
    /// @param logic true if the bitvector type is logic, false otherwise.
    /// @param resolved true if the bitvector type is resolved, false otherwise.
    /// @param const_expr true if the bitvector type is const_expr, false otherwise.
    /// @param isSigned true if the bitvector type is signed, false otherwise.
    /// @param variant The type variant.
    Bitvector *bitvector(
        Range *span                     = nullptr,
        const bool logic                = false,
        const bool resolved             = false,
        const bool const_expr           = false,
        const bool isSigned             = false,
        const Type::TypeVariant variant = Type::NATIVE_TYPE);

    /// @brief Creates a boolean type.
    /// @param isConstexpr True if const_expr.
    /// @param variant The type variant.
    Bool *boolean(const bool isConstexpr = false, const Type::TypeVariant variant = Type::NATIVE_TYPE);

    /// @brief Creates a character type.
    /// @param isConstexpr True if const_expr.
    /// @param variant The type variant.
    Char *character(const bool isConstexpr = false, const Type::TypeVariant variant = Type::NATIVE_TYPE);

    /// @brief Creates an enum typedef.
    /// @param n the name of the enum.
    /// @param e the enum values making up the enum.
    TypeDef *enumTypeDef(const std::string &n, enumValue_t e);

    /// @brief Creates a file.
    /// @param t the type of the file (see VHDL).
    File *file(Type *t = nullptr);

    /// @brief Creates an integer type.
    /// By default, it creates a int32.
    /// @param span the span for the integer type.
    /// @param is_signed the signed flag for the integer type.
    /// @param isConstexpr the const_expr flag for the integer type.
    /// @param variant The type variant.
    Int *integer(
        Range *span                     = nullptr,
        const bool is_signed            = true,
        bool isConstexpr                = true,
        const Type::TypeVariant variant = Type::NATIVE_TYPE);

    /// @brief Creates a reference to a LibraryDef.
    /// @param n The name of the library.
    /// @param i Indicates the Library scope (instance).
    /// @param fn The name of referenced LibraryDef file.
    /// @param s Tells whether the referenced LibraryDef is standard.
    /// @param sy Tells whether the referenced LibraryDef is system.
    Library *library(
        const std::string &n,
        ReferencedType *i     = nullptr,
        const std::string &fn = std::string(),
        const bool s          = false,
        const bool sy         = false);

    /// @brief Creates a pointer type.
    /// @param type the type the pointer points to.
    Pointer *pointer(Type *type);

    /// @brief Creates a real type.
    /// By default, it creates a double.
    /// @param span the span for the real type.
    /// @param isConstexpr the const_expr flag for the real type.
    /// @param variant The type variant.
    Real *real(Range *span = nullptr, bool isConstexpr = true, const Type::TypeVariant variant = Type::NATIVE_TYPE);

    /// @brief Creates a record typedef (i.e., a struct in C/C++).
    /// @param n the name of the record typedef.
    /// @param f the field(s) making up the record.
    /// @param external The external flag. Default is false.
    TypeDef *recordTypeDef(const std::string &n, field_t f, const bool external = false);

    /// @brief Creates a reference (like C++ reference types).
    /// @param type the type being referenced.
    Reference *reference(Type *type);

    /// @brief Creates a signed.
    /// @param span the span of the signed.
    /// @param const_expr true if the signed type is const_expr, false otherwise.
    Signed *signedType(Range *span = nullptr, const bool const_expr = false);

    /// @brief Creates a string.
    /// @param span the span of the string.
    /// @param const_expr true if the string type is const_expr, false otherwise.
    /// @param variant The type variant.
    String *
    string(const bool const_expr = true, const Type::TypeVariant variant = Type::NATIVE_TYPE, Range *span = nullptr);

    /// @brief Creates a Time type.
    /// @param isConstexpr True if const_expr.
    /// @param variant The type variant.
    Time *time(const bool isConstexpr = true, const Type::TypeVariant variant = Type::NATIVE_TYPE);

    /// @brief Creates a type reference (without template arguments).
    /// @param n the name of the type to be referenced.
    /// @param rt the instance of the typeref
    TypeReference *typeRef(const std::string &n, ReferencedType *rt = nullptr);

    /// @brief Creates a type reference (with template arguments).
    /// @param n the name of the type to be referenced.
    /// @param t the template argument(s) for the type to be referenced.
    /// @param rt the instance of the typeref
    TypeReference *typeRef(const std::string &n, templateArgument_t t, ReferencedType *rt = nullptr);

    /// @brief Creates an unsigned char (i.e. 8-bit byte) type.
    /// In HIF unsigned char is represented as an 8-bit unsigned integer.
    Int *unsignedChar();

    /// @brief Creates a unsigned.
    /// @param span the span of the unsigned.
    /// @param const_expr true if the unsigned type is const_expr, false otherwise.
    Unsigned *unsignedType(Range *span = nullptr, const bool const_expr = false);

    /// @brief Creates a reference to a view.
    /// @param du the name of the design unit being referenced.
    /// @param v the name of the view being referenced.
    /// @param i Indicates the view scope (instance).
    viewReference_t viewRef(const std::string &du, const std::string &v, ReferencedType *i = nullptr);

    /// @brief Creates a reference to a view.
    /// @param du the name of the design unit being referenced.
    /// @param v the name of the view being referenced.
    /// @param i Indicates the view scope (instance).
    /// @param t The list of template arguments.
    viewReference_t viewRef(const std::string &du, const std::string &v, ReferencedType *i, templateArgument_t t);

    /// @}
    /// @name Values.
    /// @{

    /// @brief Creates an Aggregate object.
    /// @param o the value provided to others.
    /// @param alts The list of AggregateAlts.
    Aggregate *aggregate(Value *o, aggregatealt_t alts);

    /// @brief Creates an AggregateAlt object.
    /// @param i the index provided.
    /// @param v the value provided.
    aggregatealt_t aggregatealt(Value *i, Value *v);

    /// @brief Creates an initial value for an array through an aggregate object.
    /// @param v the value provided to initialize array items.
    Aggregate *arrayInitVal(Value *v);

    /// @brief Creates a bit value.
    /// @param b the bit value.
    /// @param syntactic_type The syntactic type.
    BitValue *bitval(const hif::BitConstant b, Bit *syntactic_type = nullptr);

    /// @brief Creates a bit value.
    /// @param b the bit value.
    /// @param syntactic_type The syntactic type.
    BitValue *bitval(const char b, Bit *syntactic_type = nullptr);

    /// @brief Creates a bitvector value.
    /// @param b the bit value.
    /// @param syntactic_type The syntactic type.
    BitvectorValue *bitvectorval(const std::string &b, Bitvector *syntactic_type = nullptr);

    /// @brief Creates a boolean value.
    /// @param b the boolean value.
    /// @param syntactic_type The syntactic type.
    BoolValue *boolval(const bool b, Bool *syntactic_type = nullptr);

    /// @brief Creates an explicit cast.
    /// @param t the target type of the cast.
    /// @param v the object being cast.
    Cast *cast(Type *t, Value *v);

    /// @brief Creates a character value. By default, the syntactic type is char.
    /// @param c the character value.
    /// @param syntactic_type the syntactic type of the integer value.
    CharValue *charval(char c, Char *syntactic_type = nullptr);

    /// @brief Creates a Event type.
    /// @param isConstexpr True if const_expr.
    /// @param variant The type variant.
    Event *event(const bool isConstexpr = false, const Type::TypeVariant variant = Type::NATIVE_TYPE);

    /// @brief Creates a unary expression.
    /// @param o the operator in the expression.
    /// @param v the operand in the expression.
    Expression *expression(Operator o, Value *v);

    /// @brief Creates a binary expression.
    /// @param v1 the first operand in the expression.
    /// @param o the operator in the expression.
    /// @param v2 the second operand in the expression.
    Expression *expression(Value *v1, Operator o, Value *v2);

    /// @brief Creates a identifier with given id.
    /// @param id The id.
    Identifier *identifier(const std::string &id);

    /// @brief Creates an integer value. By default, the syntactic type is
    /// 32-bit integer, signed if \p n is < 0, unsigned otherwise.
    /// @param n the integer value.
    /// @param syntactic_type the syntactic type of the integer value.
    IntValue *intval(long long n, Type *syntactic_type = nullptr);

    /// @brief Creates an integer value. By default, the syntactic type is
    /// 32-bit integer, signed if \p n is < 0, unsigned otherwise.
    /// @param n the integer value.
    /// @param syntactic_type the syntactic type of the integer value.
    IntValue *intval(unsigned long long n, Type *syntactic_type = nullptr);

    /// @brief Creates an integer value. By default, the syntactic type is
    /// 32-bit integer, signed if \p n is < 0, unsigned otherwise.
    /// @param n the integer value.
    /// @param syntactic_type the syntactic type of the integer value.
    IntValue *intval(long n, Type *syntactic_type = nullptr);

    /// @brief Creates an integer value. By default, the syntactic type is
    /// 32-bit integer, signed if \p n is < 0, unsigned otherwise.
    /// @param n the integer value.
    /// @param syntactic_type the syntactic type of the integer value.
    IntValue *intval(unsigned long n, Type *syntactic_type = nullptr);

    /// @brief Creates an integer value. By default, the syntactic type is
    /// 32-bit integer, signed if \p n is < 0, unsigned otherwise.
    /// @param n the integer value.
    /// @param syntactic_type the syntactic type of the integer value.
    IntValue *intval(int n, Type *syntactic_type = nullptr);

    /// @brief Creates an integer value. By default, the syntactic type is
    /// 32-bit integer, signed if \p n is < 0, unsigned otherwise.
    /// @param n the integer value.
    /// @param syntactic_type the syntactic type of the integer value.
    IntValue *intval(unsigned int n, Type *syntactic_type = nullptr);

    /// @brief Creates a field reference.
    /// @param prefix the name of the prefix (i.e., the structure containing the field being referenced).
    /// @param field_name the name of the field.
    FieldReference *fieldRef(const std::string &prefix, const std::string &field_name);

    /// @brief Creates a field reference.
    /// @param prefix the prefix (i.e., the structure containing the field being referenced).
    /// @param field_name the name of the field.
    FieldReference *fieldRef(Value *prefix, const std::string &field_name);

    /// @brief Creates a function call.
    /// @param n the name of the function called.
    /// @param inst the eventual instance object.
    /// @param t the template argument(s) for the function call.
    /// @param p the parameter argument(s) for the function call.
    FunctionCall *functionCall(const std::string &n, Value *inst, templateArgument_t t, parameterArgument_t p);

    /// @brief Creates an instance.
    /// @param refType the referenced type (i.e., view reference or library).
    /// @param name the name of the instance.
    /// @param portassigns THe list of portassigns.
    /// @param initialValue The initial value.
    Instance *
    instance(ReferencedType *refType, const std::string &name, portassign_t portassigns, Value *initialValue = nullptr);

    /// @brief Creates an instance of a library with the same name.
    /// @param name the name of the instance and library.
    /// @param s If library is standard.
    /// @param sy If library is system.
    Instance *libraryInstance(const std::string &name, const bool s = false, const bool sy = false);

    /// @brief Creates a member.
    /// @param prefix the prefix of the member (i.e., the object containing the member).
    /// @param index the index of the member.
    Member *member(Value *prefix, Value *index);

    /// @brief Creates a nullptr value (exploiting cast).
    /// @param t The type to use for cast.
    /// @param autoPtr When true automatically adds pointer to t. Default is true
    Cast *nullval(Type *t, const bool autoPtr = true);

    /// @brief Creates a parameter argument (i.e., an actual parameter).
    /// @param n the name of the formal parameter.
    /// @param v the value provided as parameter argument.
    parameterArgument_t parameterArgument(const std::string &n, Value *v);

    /// @brief Creates a simple range.
    /// @param l the left bound of the range.
    /// @param r the right bound of the range.
    Range *range(long long l, long long r);

    /// @brief Creates a simple range.
    /// @param l the left bound of the range.
    /// @param r the right bound of the range.
    Range *range(unsigned long long l, unsigned long long r);

    /// @brief Creates a simple range.
    /// @param l the left bound of the range.
    /// @param r the right bound of the range.
    Range *range(const int l, const int r);

    /// @brief Creates a simple range.
    /// @param l the left bound of the range.
    /// @param r the right bound of the range.
    Range *range(unsigned int l, unsigned int r);

    /// @brief Creates a simple range.
    /// @param l the left bound of the range.
    /// @param r the right bound of the range.
    Range *range(long long l, const int r);

    /// @brief Creates a simple range.
    /// @param l the left bound of the range.
    /// @param r the right bound of the range.
    Range *range(const int l, long long r);

    /// @brief Creates a simple range.
    /// @param l the left bound of the range.
    /// @param dir The direction.
    /// @param r the right bound of the range.
    Range *range(Value *l, RangeDirection dir, Value *r);

    /// @brief Creates a real value.
    /// @param d the real value.
    /// @param syntactic_type the syntactic type of the real value.
    RealValue *realval(const double d, Real *syntactic_type = nullptr);

    /// @brief Creates a record value.
    /// @param rv the record value alts.
    RecordValue *recordval(recordvaluealt_t rv);

    /// @brief Creates a record value alt.
    /// @param n the name of the record field.
    /// @param v the value provided.
    recordvaluealt_t recordvaluealt(const std::string &n, Value *v);

    /// @brief Creates a slice.
    /// @param prefix the prefix of the slice (i.e., the object to be sliced).
    /// @param span the span of the slice.
    Slice *slice(Value *prefix, Range *span);

    /// @brief Creates a type template argument.
    /// @param n the name of the type template argument.
    /// @param t the type provided as template argument.
    templateArgument_t templateTypeArgument(const std::string &n, Type *t);

    /// @brief Creates a value template argument.
    /// @param n the name of the value template parameter.
    /// @param v the value provided as template argument.
    templateArgument_t templateValueArgument(const std::string &n, Value *v);

    /// @brief Creates a text. By default, it is not plain.
    /// @param s the text.
    /// @param plain true if the text is plain, false otherwise.
    /// @param syntactic_type The syntactic type.
    StringValue *stringval(const std::string &s, bool plain = false, Type *syntactic_type = nullptr);

    /// @brief Creates a time value.
    /// @param v the value of the time value
    /// @param u the time unit
    /// @param syntactic_type the syntactic type of the time value.
    TimeValue *timeval(double v, TimeValue::TimeUnit u = TimeValue::time_fs, Time *syntactic_type = nullptr);

    /// @brief Creates a value statement.
    /// @param value the value of the value statement.
    ValueStatement *valueStatement(Value *value);

    /// @brief Creates a when.
    /// @param alts the list of alternatives.
    /// @param def The default value.
    /// @param logicTernary If is logic ternary.
    When *when(whenAlt_t alts, Value *def, const bool logicTernary = false);

    /// @brief Creates a when alt.
    /// @param cond The condition.
    /// @param value The value.
    whenAlt_t whenalt(Value *cond, Value *value);

    /// @brief Creates a while.
    /// @param cond The while condition.
    /// @param actions The actions.
    /// @param label The the while label.
    /// @param doWhile true if is a do-while.
    While *whileLoop(Value *cond, action_t actions, const std::string &label = std::string(), const bool doWhile = false);

    /// @brief Creates an alias.
    /// @param name The name of the alias.
    /// @param t Type of the alias.
    /// @param v Value of the alias.
    Alias *alias(const std::string &name, Type *t, Value *v);
    /// @}
    /// @name Declarations.
    /// @{

    /// @brief Creates a constant declaration.
    /// @param t the type of the constant.
    /// @param n the name of the constant.
    /// @param init the value of the constant.
    /// @param r the eventual range.
    Const *constant(Type *t, const std::string &n, Value *init, Range *r = nullptr);

    /// @brief Creates a contents container.
    /// @param ga The GlobalAction.
    /// @param d The list of Declarations.
    /// @param g The list of Generates.
    /// @param i The list of Instances.
    /// @param s The list of StateTables.
    /// @param l The list of Libraries.
    Contents *contents(GlobalAction *ga, declaration_t d, generate_t g, instance_t i, stateTable_t s, library_t l);

    /// @brief Creates a design unit (with empty View).
    /// @param u the name of the design unit.
    /// @param v the name of the view.
    /// @param lid the language ID of the view.
    DesignUnit *designUnit(const std::string &u, const std::string &v, hif::LanguageID lid);

    /// @brief Creates a design unit specifying its name and the Views to set.
    /// @param u the name of the design unit.
    /// @param v The only View.
    DesignUnit *designUnit(const std::string &u, View *v);

    /// @brief Creates an enum value.
    /// @param tr The typeref to enum.
    /// @param n the name of the enum value.
    /// @param v The optional initial value.
    /// @param r the eventual range.
    enumValue_t enumValue(TypeReference *tr, const std::string &n, Value *v = nullptr, Range *r = nullptr);

    /// @brief Creates a field declaration.
    /// @param t the type of the field.
    /// @param n the name of the field.
    /// @param init the initial value of the field.
    /// @param r the eventual range.
    /// @param dir The direction.
    field_t field(
        Type *t,
        const std::string &n,
        Value *init                  = nullptr,
        Range *r                     = nullptr,
        const hif::PortDirection dir = hif::dir_none);

    /// @brief Creates a formal parameter.
    /// @param t the type of the formal parameter.
    /// @param n the name of the formal parameter.
    /// @param v the default value of the formal parameter.
    /// @param r the parameter range.
    /// @param dir the parameter direction.
    parameter_t parameter(
        Type *t,
        const std::string &n,
        Value *v                = nullptr,
        Range *r                = nullptr,
        const PortDirection dir = dir_none);

    /// @brief Creates a port declaration.
    /// @param t the type of the port.
    /// @param n the name of the port.
    /// @param d the direction of the port.
    /// @param init the initial value of the port.
    /// @param r the eventual range.
    /// @param w if the port is a wrapper.
    Port *port(
        Type *t,
        const std::string &n,
        PortDirection d,
        Value *init  = nullptr,
        Range *r     = nullptr,
        const bool w = false);

    /// @brief Creates a port assign.
    /// @param n the name of the (instance) port.
    /// @param v the bind of the (top) port.
    /// @param d the direction of the port.
    /// @param t the type of the port.
    portassign_t portAssign(const std::string &n, Value *v, PortDirection d = dir_none, Type *t = nullptr);

    /// @brief Creates a signal declaration.
    /// @param t the type of the signal.
    /// @param n the name of the signal.
    /// @param init the initial value of the signal.
    /// @param r the eventual range.
    /// @param s if the signal is standard.
    /// @param w if the signal is a wrapper.
    Signal *signal(
        Type *t,
        const std::string &n,
        Value *init  = nullptr,
        Range *r     = nullptr,
        const bool s = false,
        const bool w = false);

    /// @brief Creates a statetable with a signle state.
    /// @param n The name.
    /// @param decls The declarations.
    /// @param actions The actions.
    /// @param dontInitialize Don't initialize flag.
    /// @param pf The process flavour.
    stateTable_t stateTable(
        const std::string &n,
        declaration_t decls,
        action_t actions,
        const bool dontInitialize    = false,
        const hif::ProcessFlavour pf = hif::pf_hdl);

    /// @brief Creates a subprogram (i.e., function or procedure).
    /// @param t the returned type (nullptr for procedures).
    /// @param n the name of the subprogram.
    /// @param tp the list of template parameters.
    /// @param p the list of formal parameters.
    SubProgram *subprogram(Type *t, const std::string &n, template_t tp, parameter_t p);

    /// @brief Creates a type template parameter.
    /// @param t the default type of the template parameter.
    /// @param n the name of the template parameter.
    template_t templateTypeParameter(Type *t, const std::string &n);

    /// @brief Creates a value template parameter.
    /// @param t the type of the template parameter.
    /// @param n the name of the template parameter.
    /// @param v the default value of the template parameter.
    /// @param r the eventual range.
    /// @param compileTimeConstant True if compileTimeConstant.
    template_t templateValueParameter(
        Type *t,
        const std::string &n,
        Value *v                       = nullptr,
        Range *r                       = nullptr,
        const bool compileTimeConstant = true);

    /// @brief Creates a type def.
    /// @param n the name of the type def.
    /// @param t the type of the type def.
    /// @param opaque true if opaque.
    /// @param r The eventual constraint.
    /// @param external The external flag. Default is false.
    TypeDef *
    typeDef(const std::string &n, Type *t, const bool opaque = true, Range *r = nullptr, const bool external = false);

    /// @brief Creates a type def.
    /// @param n the name of the type def.
    /// @param t the type of the type def.
    /// @param opaque true if opaque.
    /// @param tp The template parameters.
    /// @param external The external flag. Default is false.
    TypeDef *typeDef(const std::string &n, Type *t, const bool opaque, template_t tp, const bool external = false);

    /// @brief Creates a variable declaration.
    /// @param t the type of the variable.
    /// @param n the name of the variable.
    /// @param init the initial value of the variable.
    /// @param r the eventual range.
    Variable *variable(Type *t, const std::string &n, Value *init = nullptr, Range *r = nullptr);

    /// @brief Creates a variable declaration.
    /// @param t the type of the variable.
    /// @param n the name of the variable.
    /// @param init the initial value of the variable.
    /// @param r the eventual range.
    declaration_t variableDecl(Type *t, const std::string &n, Value *init = nullptr, Range *r = nullptr);

    /// @brief Creates a View.
    /// @param n the name of the view.
    /// @param c The Contents member.
    /// @param e The Entity member.
    /// @param lid the language ID of the design unit.
    /// @param d The list of Declarations.
    /// @param l The list of Libraries.
    /// @param t The list of template parameters.
    /// @param s Attribute "standard" of the view.
    /// @param fileName The fileName of the view.
    View *view(
        const std::string &n,
        Contents *c,
        Entity *e,
        LanguageID lid,
        declaration_t d,
        library_t l,
        template_t t,
        const bool s                = false,
        const std::string &fileName = std::string());

    /// @brief Creates a View.
    /// @param n the name of the view.
    /// @param c The Contents member.
    /// @param e The Entity member.
    /// @param lid the language ID of the design unit.
    /// @param d The list of Declarations.
    /// @param l The list of Libraries.
    /// @param t The list of template parameters.
    /// @param s Attribute "standard" of the view.
    /// @param parents The list of parent views.
    /// /// @param fileName The fileName of the view.
    View *view(
        const std::string &n,
        Contents *c,
        Entity *e,
        LanguageID lid,
        declaration_t d,
        library_t l,
        template_t t,
        const bool s,
        viewReference_t parents,
        const std::string &fileName = std::string());

    /// @}
    /// @name Actions.
    /// @{

    /// @brief Creates an assignment.
    /// @param left the left-hand side (i.e., the target) of the assignment.
    /// @param right the right-hand side (i.e., the source) of the assignment.
    /// @param delay the optional assignment delay.
    action_t assignAction(Value *left, Value *right, TimeValue *delay = nullptr);

    /// @brief Creates an assignment.
    /// @param left the left-hand side (i.e., the target) of the assignment.
    /// @param right the right-hand side (i.e., the source) of the assignment.
    /// @param delay the optional assignment delay.
    Assign *assignment(Value *left, Value *right, TimeValue *delay = nullptr);

    /// @brief Creates a for loop.
    /// @param label the label of the loop.
    /// @param initDecls the list of indexes declarations.
    /// @param initVals the list of indexes initial values (if no declaration is present).
    /// @param condition the loop condition.
    /// @param stepAct the list of step actions.
    /// @param forAct the list of loop-body actions.
    For *forLoop(
        const std::string &label,
        BList<DataDeclaration> &initDecls,
        BList<Action> &initVals,
        Value *condition,
        BList<Action> &stepAct,
        BList<Action> &forAct);

    /// @brief Creates an if statement.
    /// @param d the list of actions of the default case.
    /// @param as the list of alternatives.
    If *ifStmt(action_t d, ifAlt_t as);

    /// @brief Creates an if statement.
    /// @param d the list of actions of the default case.
    /// @param as the list of alternatives.
    action_t ifAction(action_t d, ifAlt_t as);

    /// @brief Creates an if-statement alternative.
    /// @param c the condition.
    /// @param a the list of actions.
    IfAlt *ifAlt(Value *c, action_t a);

    /// @brief Creates a procedure call.
    /// @param n the name of the procedure called.
    /// @param inst the eventual instance object.
    /// @param t the template argument(s) for the procedure call.
    /// @param p the parameter argument(s) for the procedure call.
    action_t procedureCallAction(const std::string &n, Value *inst, templateArgument_t t, parameterArgument_t p);

    /// @brief Creates a procedure call.
    /// @param n the name of the procedure called.
    /// @param inst the eventual instance object.
    /// @param t the template argument(s) for the procedure call.
    /// @param p the parameter argument(s) for the procedure call.
    ProcedureCall *procedureCall(const std::string &n, Value *inst, templateArgument_t t, parameterArgument_t p);

    /// @brief Creates a return statement exploiting a specific value or a
    /// type to build 'return nullptr' statement. At least one of them must be
    /// specified.
    /// @param v The value of return.
    /// @param t The type to use to build a 'return nullptr' statement.
    Return *retStmt(Value *v = nullptr, Type *t = nullptr);

    /// @brief Creates a return statement exploiting a specific value or a
    /// type to build 'return nullptr' statement. At least one of them must be
    /// specified.
    /// @param v The value of return.
    /// @param t The type to use to build a 'return nullptr' statement.
    action_t returnAction(Value *v = nullptr, Type *t = nullptr);

    /// @brief Creates a wait statement.
    /// @param timeVal The time.
    /// @param cond The condition.
    /// @param repetitions The repetitions.
    action_t waitAction(Value *timeVal, Value *cond = nullptr, Value *repetitions = nullptr);

    /// @brief Creates a wait statement.
    /// @param timeVal The time.
    /// @param cond The condition.
    /// @param repetitions The repetitions.
    /// @param actions Possible wait actions.
    action_t waitAction(Value *timeVal, Value *cond, Value *repetitions, action_t &actions);

    /// @}
    /// @name Others.
    /// @{

    /// @brief Creates a class constructor. By default its name is __hif_constructor.
    /// Signature will be "DesignUnit __hif_constructor<tp>(pp)".
    /// @param du the DesignUnit representing the class.
    /// @param pp the list of formal parameters.
    /// @param tp the list of template parameters.
    Function *classConstructor(DesignUnit *du, BList<Parameter> &pp, BList<Declaration> &tp);

    /// @brief Creates a class constructor. By default its name is __hif_constructor.
    /// Signature will be "DesignUnit __hif_constructor<tp>(pp)".
    /// @param du the DesignUnit representing the class.
    /// @param pp the list of formal parameters.
    /// @param tp the list of template parameters.
    Function *classConstructor(DesignUnit *du, parameter_t pp, template_t tp);

    /// @brief Creates the call to a class constructor. Normally it is used as
    /// initial value of the variable representing a class instance.
    /// @param du is the referenced DesignUnit (class).
    /// @param instName is the name of the instance variable.
    /// @param parameterAssigns the list of actual parameters.
    /// @param tpassigns the list of template parameters assigns.
    FunctionCall *classConstructorCall(
        DesignUnit *du,
        const std::string &instName,
        BList<ParameterAssign> parameterAssigns,
        BList<TPAssign> tpassigns);

    /// @brief Creates the call to a class constructor. Normally it is used as
    /// initial value of the variable representing a class instance.
    /// @param du is the referenced DesignUnit (class).
    /// @param instName is the name of the instance variable.
    /// @param p the list of actual parameters.
    /// @param t the list of template parameters assigns.
    FunctionCall *
    classConstructorCall(DesignUnit *du, const std::string &instName, parameterArgument_t p, templateArgument_t t);

    /// @brief Creates the call to a class constructor. Normally it is used as
    /// initial value of the variable representing a class instance.
    /// @param n is the name of referenced DesignUnit (class). FunctionCall
    /// Instance also take the same name.
    /// @param iT is the referenced type of instance.
    /// @param p the list of actual parameters.
    /// @param t the list of template parameters assigns.
    FunctionCall *
    classConstructorCall(const std::string &n, ReferencedType *iT, parameterArgument_t p, templateArgument_t t);

    /// @brief Creates a class destructor. By default its name is __hif_destructor.
    /// @param du the DesignUnit representing the class.
    Procedure *classDestructor(DesignUnit *du);

    /// @}

protected:
    hif::semantics::ILanguageSemantics *_sem;
};

} // namespace hif

#if (defined _MSC_VER) && !(defined __GNUC__)
#else
#    if HIF_DIAGNOSTIC_PUSH_POP
#        pragma GCC diagnostic pop
#    else
#        pragma GCC diagnostic ignored "-Weffc++"
#    endif
#endif
