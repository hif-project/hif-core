/// @file MapVisitor.hpp
/// @brief Implements runtime to compile-time polymorphism mapping utilities.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/HifVisitor.hpp"
#include "hif/application_utils/portability.hpp"

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4702)
#endif

namespace hif
{

// ////////////////////////////////////////////////////////////////////////////////////////
// Support.
// ////////////////////////////////////////////////////////////////////////////////////////

namespace __conversion
{

/// @brief Utility to check relationships between two classes.
template <class CHILD, class PARENT> class Conversion_t
{
private:
    typedef char True_t;
    class False_t
    {
        char c[2];
    };
    static True_t test(PARENT);
    static False_t test(...);
    static CHILD makeChild();

public:
    /// @brief Indicates if CHILD is a subclass of PARENT.
    enum { isChild = (sizeof(test(makeChild())) == sizeof(True_t)) };

    /// @brief Indicates if the two types are the same.
    enum { isTheSameType = false };

    Conversion_t();
    ~Conversion_t();
};

/// @brief Specialization for identical types.
template <class T> class Conversion_t<T, T>
{
public:
    /// @brief Always true for identical types.
    enum { isChild = true };

    /// @brief Indicates identical types.
    enum { isSameTyle = true };
};

/// @brief Utility to verify subclass relationships.
template <class PARENT, class CHILD> struct checkSubclass {
    /// @brief Result of subclass verification.
    enum {
        result =
            (Conversion_t<const CHILD *, const PARENT *>::isChild &&
             !Conversion_t<const PARENT *, void *>::isTheSameType)
    };
};

} // namespace __conversion

/// @name Mapping classes.
/// These classes provides transformation from runtime to compile-time polymorphism.
/// As instance, to map two input objects to their actual types,
/// it suffices to write:
/// @code
/// MapVisitor2_t< MyCallback > v2( object1, object2 );
/// @endcode
/// where MyCallback is a class implementing a static map() method,
/// able to take two objects. In this way, the map() method will know the actual
/// object types.
/// All this stuff has been done to exploit template (partial) specialization,
/// expecially if used with NTBC design pattern.
/// A simple example:
/// @code
/// template< class T1, class T2 > S {}; // default case, for all types.
/// template< class T1 > S< T1, Int > {}; // case for instances with int as second type.
/// template<> S< Int, Int > {}; // case for instances with int as type of both.
///
/// struct my_mapper_t
/// {
///    template < typename T1, typename T2 >
///    static void map( T1 * o1, T2 * o2 ) { S< T1, T2> s; ... } // do something, exploiting template partial specialization.
/// };
///
/// // Calling the map utility:
/// MapVisitor2_t< my_mapper_t > v2( o1, o2 );
/// @endcode
//@{

/// @brief Maps runtime polymorphism to compile-time polymorphism.
template <class FinalRebind> class MapVisitor1 : protected HifVisitor
{
public:
    /// @brief Constructor that initializes the visitor with an object.
    MapVisitor1(Object *o1);

    /// @brief Destructor.
    virtual ~MapVisitor1();

protected:
    virtual int visitAggregateAlt(AggregateAlt &o);

    virtual int visitAggregate(Aggregate &o);

    virtual int visitAlias(Alias &o);

    virtual int visitArray(Array &o);

    virtual int visitAssign(Assign &o);

    virtual int visitBit(Bit &o);

    virtual int visitBitValue(BitValue &o);

    virtual int visitBitvector(Bitvector &o);

    virtual int visitBitvectorValue(BitvectorValue &o);

    virtual int visitBool(Bool &o);

    virtual int visitBoolValue(BoolValue &o);

    virtual int visitIfAlt(IfAlt &o);

    virtual int visitIf(If &o);

    virtual int visitCast(Cast &o);

    virtual int visitChar(Char &o);

    virtual int visitCharValue(CharValue &o);

    virtual int visitConst(Const &o);

    virtual int visitContents(Contents &o);

    virtual int visitDesignUnit(DesignUnit &o);

    virtual int visitEnum(Enum &o);

    virtual int visitEnumValue(EnumValue &o);

    virtual int visitBreak(Break &o);

    virtual int visitEvent(Event &o);

    virtual int visitExpression(Expression &o);

    virtual int visitFunctionCall(FunctionCall &o);

    virtual int visitField(Field &o);

    virtual int visitFieldReference(FieldReference &o);

    virtual int visitFile(File &o);

    virtual int visitForGenerate(ForGenerate &o);

    virtual int visitFor(For &o);

    virtual int visitFunction(Function &o);

    virtual int visitGlobalAction(GlobalAction &o);

    virtual int visitEntity(Entity &o);

    virtual int visitIfGenerate(IfGenerate &o);

    virtual int visitInstance(Instance &o);

    virtual int visitInt(Int &o);

    virtual int visitIntValue(IntValue &o);

    virtual int visitLibraryDef(LibraryDef &o);

    virtual int visitLibrary(Library &o);

    virtual int visitMember(Member &o);

    virtual int visitIdentifier(Identifier &o);

    virtual int visitContinue(Continue &o);

    virtual int visitNull(Null &o);

    virtual int visitTransition(Transition &o);

    virtual int visitParameterAssign(ParameterAssign &o);

    virtual int visitParameter(Parameter &o);

    virtual int visitProcedureCall(ProcedureCall &o);

    virtual int visitPointer(Pointer &o);

    virtual int visitPortAssign(PortAssign &o);

    virtual int visitPort(Port &o);

    virtual int visitProcedure(Procedure &o);

    virtual int visitRange(Range &o);

    virtual int visitReal(Real &o);

    virtual int visitRealValue(RealValue &o);

    virtual int visitRecord(Record &o);

    virtual int visitRecordValue(RecordValue &o);

    virtual int visitRecordValueAlt(RecordValueAlt &o);

    virtual int visitReference(Reference &o);

    virtual int visitReturn(Return &o);

    virtual int visitSignal(Signal &o);

    virtual int visitSigned(Signed &o);

    virtual int visitSlice(Slice &o);

    virtual int visitState(State &o);

    virtual int visitStateTable(StateTable &o);

    virtual int visitString(String &o);

    virtual int visitSwitchAlt(SwitchAlt &o);

    virtual int visitSwitch(Switch &o);

    virtual int visitSystem(System &o);

    virtual int visitStringValue(StringValue &o);

    virtual int visitTime(Time &o);

    virtual int visitTimeValue(TimeValue &o);

    virtual int visitTypeDef(TypeDef &o);

    virtual int visitTypeReference(TypeReference &o);

    virtual int visitTypeTPAssign(TypeTPAssign &o);

    virtual int visitTypeTP(TypeTP &o);

    virtual int visitUnsigned(Unsigned &o);

    virtual int visitValueStatement(ValueStatement &o);

    virtual int visitValueTPAssign(ValueTPAssign &o);

    virtual int visitValueTP(ValueTP &o);

    virtual int visitVariable(Variable &o);

    virtual int visitView(View &o);

    virtual int visitViewReference(ViewReference &o);

    virtual int visitWait(Wait &o);

    virtual int visitWhenAlt(WhenAlt &o);

    virtual int visitWhen(When &o);

    virtual int visitWhile(While &o);

    virtual int visitWithAlt(WithAlt &o);

    virtual int visitWith(With &o);

private:
    MapVisitor1(const MapVisitor1<FinalRebind> &);
    MapVisitor1<MapVisitor1> &operator=(const MapVisitor1<FinalRebind> &);
};

// ////////////////////////////////////////////////////////////////////////////////////////
// Maps two objects.
// ////////////////////////////////////////////////////////////////////////////////////////

/// @brief Maps runtime polymorphism to compile-time polymorphism.
template <class FinalRebind, class Parent2> class MapVisitor2 : protected HifVisitor
{
public:
    MapVisitor2(Object *oo1, Object *oo2);

    virtual ~MapVisitor2();

    /// @brief This struct is used to map to compile-time polimorphism.
    template <class T2> struct rebind_t {
        typedef FinalRebind FR_t;
        /// @brief This method calls the FinalRebind::map() method,
        /// passing tree objects, and using their actual type.
        template <typename T1> static void map(T1 *o1);
    };

protected:
    virtual int visitAggregateAlt(AggregateAlt &o);

    virtual int visitAggregate(Aggregate &o);

    virtual int visitAlias(Alias &o);

    virtual int visitArray(Array &o);

    virtual int visitAssign(Assign &o);

    virtual int visitBit(Bit &o);

    virtual int visitBitValue(BitValue &o);

    virtual int visitBitvector(Bitvector &o);

    virtual int visitBitvectorValue(BitvectorValue &o);

    virtual int visitBool(Bool &o);

    virtual int visitBoolValue(BoolValue &o);

    virtual int visitIfAlt(IfAlt &o);

    virtual int visitIf(If &o);

    virtual int visitCast(Cast &o);

    virtual int visitChar(Char &o);

    virtual int visitCharValue(CharValue &o);

    virtual int visitConst(Const &o);

    virtual int visitContents(Contents &o);

    virtual int visitDesignUnit(DesignUnit &o);

    virtual int visitEnum(Enum &o);

    virtual int visitEnumValue(EnumValue &o);

    virtual int visitBreak(Break &o);

    virtual int visitEvent(Event &o);

    virtual int visitExpression(Expression &o);

    virtual int visitFunctionCall(FunctionCall &o);

    virtual int visitField(Field &o);

    virtual int visitFieldReference(FieldReference &o);

    virtual int visitFile(File &o);

    virtual int visitForGenerate(ForGenerate &o);

    virtual int visitFor(For &o);

    virtual int visitFunction(Function &o);

    virtual int visitGlobalAction(GlobalAction &o);

    virtual int visitEntity(Entity &o);

    virtual int visitIfGenerate(IfGenerate &o);

    virtual int visitInstance(Instance &o);

    virtual int visitInt(Int &o);

    virtual int visitIntValue(IntValue &o);

    virtual int visitLibraryDef(LibraryDef &o);

    virtual int visitLibrary(Library &o);

    virtual int visitMember(Member &o);

    virtual int visitIdentifier(Identifier &o);

    virtual int visitContinue(Continue &o);

    virtual int visitNull(Null &o);

    virtual int visitTransition(Transition &o);

    virtual int visitParameterAssign(ParameterAssign &o);

    virtual int visitParameter(Parameter &o);

    virtual int visitProcedureCall(ProcedureCall &o);

    virtual int visitPointer(Pointer &o);

    virtual int visitPortAssign(PortAssign &o);

    virtual int visitPort(Port &o);

    virtual int visitProcedure(Procedure &o);

    virtual int visitRange(Range &o);

    virtual int visitReal(Real &o);

    virtual int visitRealValue(RealValue &o);

    virtual int visitRecord(Record &o);

    virtual int visitRecordValue(RecordValue &o);

    virtual int visitRecordValueAlt(RecordValueAlt &o);

    virtual int visitReference(Reference &o);

    virtual int visitReturn(Return &o);

    virtual int visitSignal(Signal &o);

    virtual int visitSigned(Signed &o);

    virtual int visitSlice(Slice &o);

    virtual int visitState(State &o);

    virtual int visitStateTable(StateTable &o);

    virtual int visitString(String &o);

    virtual int visitSwitchAlt(SwitchAlt &o);

    virtual int visitSwitch(Switch &o);

    virtual int visitSystem(System &o);

    virtual int visitStringValue(StringValue &o);

    virtual int visitTime(Time &o);

    virtual int visitTimeValue(TimeValue &o);

    virtual int visitTypeDef(TypeDef &o);

    virtual int visitTypeReference(TypeReference &o);

    virtual int visitTypeTPAssign(TypeTPAssign &o);

    virtual int visitTypeTP(TypeTP &o);

    virtual int visitUnsigned(Unsigned &o);

    virtual int visitValueStatement(ValueStatement &o);

    virtual int visitValueTPAssign(ValueTPAssign &o);

    virtual int visitValueTP(ValueTP &o);

    virtual int visitVariable(Variable &o);

    virtual int visitView(View &o);

    virtual int visitViewReference(ViewReference &o);

    virtual int visitWait(Wait &o);

    virtual int visitWhenAlt(WhenAlt &o);

    virtual int visitWhen(When &o);

    virtual int visitWhile(While &o);

    virtual int visitWithAlt(WithAlt &o);

    virtual int visitWith(With &o);
    // Internal fields.

    Object *o1;
    /// This field must be static, to allow rebind_t struct to access it,
    /// w/o any reference to this object. This is required to allow compile-time
    /// polymorphism, since references to "this" are not constant.
    static Object *o2;

    template <class T2> friend struct rebind_t;

private:
    MapVisitor2(const MapVisitor2<FinalRebind, Parent2> &);
    MapVisitor2<MapVisitor2, Parent2> &operator=(const MapVisitor2<FinalRebind, Parent2> &);
};
// Statics definition:

template <class FinalRebind, class Parent2> Object *MapVisitor2<FinalRebind, Parent2>::o2 = nullptr;
} // namespace hif

#include "MapVisitor.i.hpp"

#ifdef _MSC_VER
#    pragma warning(pop)
#endif
