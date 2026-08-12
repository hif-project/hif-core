/// @file BiVisitor.i.hpp
/// @brief Implements MonoVisitor and BiVisitor for object traversal.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <stdint.h>

#include "hif/BiVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/classes/classes.hpp"

namespace hif
{

// ///////////////////////////////////////////////////////////////////
// MonoVisitor
// ///////////////////////////////////////////////////////////////////
template <class Child> MonoVisitor<Child>::MonoVisitor()
{
    // ntd
}

template <class Child> MonoVisitor<Child>::~MonoVisitor()
{
    // ntd
}

template <class Child> void MonoVisitor<Child>::callMap(Object *o1)
{
    switch (o1->getClassId()) {
    case CLASSID_AGGREGATEALT:
        static_cast<Child *>(this)->map(static_cast<AggregateAlt *>(o1));
        break;
    case CLASSID_AGGREGATE:
        static_cast<Child *>(this)->map(static_cast<Aggregate *>(o1));
        break;
    case CLASSID_ALIAS:
        static_cast<Child *>(this)->map(static_cast<Alias *>(o1));
        break;
    case CLASSID_ARRAY:
        static_cast<Child *>(this)->map(static_cast<Array *>(o1));
        break;
    case CLASSID_ASSIGN:
        static_cast<Child *>(this)->map(static_cast<Assign *>(o1));
        break;
    case CLASSID_BIT:
        static_cast<Child *>(this)->map(static_cast<Bit *>(o1));
        break;
    case CLASSID_BITVALUE:
        static_cast<Child *>(this)->map(static_cast<BitValue *>(o1));
        break;
    case CLASSID_BITVECTOR:
        static_cast<Child *>(this)->map(static_cast<Bitvector *>(o1));
        break;
    case CLASSID_BITVECTORVALUE:
        static_cast<Child *>(this)->map(static_cast<BitvectorValue *>(o1));
        break;
    case CLASSID_BOOL:
        static_cast<Child *>(this)->map(static_cast<Bool *>(o1));
        break;
    case CLASSID_BOOLVALUE:
        static_cast<Child *>(this)->map(static_cast<BoolValue *>(o1));
        break;
    case CLASSID_BREAK:
        static_cast<Child *>(this)->map(static_cast<Break *>(o1));
        break;
    case CLASSID_CAST:
        static_cast<Child *>(this)->map(static_cast<Cast *>(o1));
        break;
    case CLASSID_CHAR:
        static_cast<Child *>(this)->map(static_cast<Char *>(o1));
        break;
    case CLASSID_CHARVALUE:
        static_cast<Child *>(this)->map(static_cast<CharValue *>(o1));
        break;
    case CLASSID_CONST:
        static_cast<Child *>(this)->map(static_cast<Const *>(o1));
        break;
    case CLASSID_CONTENTS:
        static_cast<Child *>(this)->map(static_cast<Contents *>(o1));
        break;
    case CLASSID_CONTINUE:
        static_cast<Child *>(this)->map(static_cast<Continue *>(o1));
        break;
    case CLASSID_DESIGNUNIT:
        static_cast<Child *>(this)->map(static_cast<DesignUnit *>(o1));
        break;
    case CLASSID_ENTITY:
        static_cast<Child *>(this)->map(static_cast<Entity *>(o1));
        break;
    case CLASSID_ENUM:
        static_cast<Child *>(this)->map(static_cast<Enum *>(o1));
        break;
    case CLASSID_ENUMVALUE:
        static_cast<Child *>(this)->map(static_cast<EnumValue *>(o1));
        break;
    case CLASSID_EVENT:
        static_cast<Child *>(this)->map(static_cast<Event *>(o1));
        break;
    case CLASSID_EXPRESSION:
        static_cast<Child *>(this)->map(static_cast<Expression *>(o1));
        break;
    case CLASSID_FIELD:
        static_cast<Child *>(this)->map(static_cast<Field *>(o1));
        break;
    case CLASSID_FIELDREFERENCE:
        static_cast<Child *>(this)->map(static_cast<FieldReference *>(o1));
        break;
    case CLASSID_FILE:
        static_cast<Child *>(this)->map(static_cast<File *>(o1));
        break;
    case CLASSID_FORGENERATE:
        static_cast<Child *>(this)->map(static_cast<ForGenerate *>(o1));
        break;
    case CLASSID_FOR:
        static_cast<Child *>(this)->map(static_cast<For *>(o1));
        break;
    case CLASSID_FUNCTIONCALL:
        static_cast<Child *>(this)->map(static_cast<FunctionCall *>(o1));
        break;
    case CLASSID_FUNCTION:
        static_cast<Child *>(this)->map(static_cast<Function *>(o1));
        break;
    case CLASSID_GLOBALACTION:
        static_cast<Child *>(this)->map(static_cast<GlobalAction *>(o1));
        break;
    case CLASSID_IDENTIFIER:
        static_cast<Child *>(this)->map(static_cast<Identifier *>(o1));
        break;
    case CLASSID_IFALT:
        static_cast<Child *>(this)->map(static_cast<IfAlt *>(o1));
        break;
    case CLASSID_IFGENERATE:
        static_cast<Child *>(this)->map(static_cast<IfGenerate *>(o1));
        break;
    case CLASSID_IF:
        static_cast<Child *>(this)->map(static_cast<If *>(o1));
        break;
    case CLASSID_INSTANCE:
        static_cast<Child *>(this)->map(static_cast<Instance *>(o1));
        break;
    case CLASSID_INT:
        static_cast<Child *>(this)->map(static_cast<Int *>(o1));
        break;
    case CLASSID_INTVALUE:
        static_cast<Child *>(this)->map(static_cast<IntValue *>(o1));
        break;
    case CLASSID_LIBRARYDEF:
        static_cast<Child *>(this)->map(static_cast<LibraryDef *>(o1));
        break;
    case CLASSID_LIBRARY:
        static_cast<Child *>(this)->map(static_cast<Library *>(o1));
        break;
    case CLASSID_MEMBER:
        static_cast<Child *>(this)->map(static_cast<Member *>(o1));
        break;
    case CLASSID_NULL:
        static_cast<Child *>(this)->map(static_cast<Null *>(o1));
        break;
    case CLASSID_PARAMETERASSIGN:
        static_cast<Child *>(this)->map(static_cast<ParameterAssign *>(o1));
        break;
    case CLASSID_PARAMETER:
        static_cast<Child *>(this)->map(static_cast<Parameter *>(o1));
        break;
    case CLASSID_POINTER:
        static_cast<Child *>(this)->map(static_cast<Pointer *>(o1));
        break;
    case CLASSID_PORTASSIGN:
        static_cast<Child *>(this)->map(static_cast<PortAssign *>(o1));
        break;
    case CLASSID_PORT:
        static_cast<Child *>(this)->map(static_cast<Port *>(o1));
        break;
    case CLASSID_PROCEDURECALL:
        static_cast<Child *>(this)->map(static_cast<ProcedureCall *>(o1));
        break;
    case CLASSID_PROCEDURE:
        static_cast<Child *>(this)->map(static_cast<Procedure *>(o1));
        break;
    case CLASSID_RANGE:
        static_cast<Child *>(this)->map(static_cast<Range *>(o1));
        break;
    case CLASSID_REAL:
        static_cast<Child *>(this)->map(static_cast<Real *>(o1));
        break;
    case CLASSID_REALVALUE:
        static_cast<Child *>(this)->map(static_cast<RealValue *>(o1));
        break;
    case CLASSID_RECORD:
        static_cast<Child *>(this)->map(static_cast<Record *>(o1));
        break;
    case CLASSID_RECORDVALUEALT:
        static_cast<Child *>(this)->map(static_cast<RecordValueAlt *>(o1));
        break;
    case CLASSID_RECORDVALUE:
        static_cast<Child *>(this)->map(static_cast<RecordValue *>(o1));
        break;
    case CLASSID_REFERENCE:
        static_cast<Child *>(this)->map(static_cast<Reference *>(o1));
        break;
    case CLASSID_RETURN:
        static_cast<Child *>(this)->map(static_cast<Return *>(o1));
        break;
    case CLASSID_SIGNAL:
        static_cast<Child *>(this)->map(static_cast<Signal *>(o1));
        break;
    case CLASSID_SIGNED:
        static_cast<Child *>(this)->map(static_cast<Signed *>(o1));
        break;
    case CLASSID_SLICE:
        static_cast<Child *>(this)->map(static_cast<Slice *>(o1));
        break;
    case CLASSID_STATE:
        static_cast<Child *>(this)->map(static_cast<State *>(o1));
        break;
    case CLASSID_STATETABLE:
        static_cast<Child *>(this)->map(static_cast<StateTable *>(o1));
        break;
    case CLASSID_STRING:
        static_cast<Child *>(this)->map(static_cast<String *>(o1));
        break;
    case CLASSID_STRINGVALUE:
        static_cast<Child *>(this)->map(static_cast<StringValue *>(o1));
        break;
    case CLASSID_SWITCHALT:
        static_cast<Child *>(this)->map(static_cast<SwitchAlt *>(o1));
        break;
    case CLASSID_SWITCH:
        static_cast<Child *>(this)->map(static_cast<Switch *>(o1));
        break;
    case CLASSID_SYSTEM:
        static_cast<Child *>(this)->map(static_cast<System *>(o1));
        break;
    case CLASSID_TIME:
        static_cast<Child *>(this)->map(static_cast<Time *>(o1));
        break;
    case CLASSID_TIMEVALUE:
        static_cast<Child *>(this)->map(static_cast<TimeValue *>(o1));
        break;
    case CLASSID_TRANSITION:
        static_cast<Child *>(this)->map(static_cast<Transition *>(o1));
        break;
    case CLASSID_TYPEDEF:
        static_cast<Child *>(this)->map(static_cast<TypeDef *>(o1));
        break;
    case CLASSID_TYPEREFERENCE:
        static_cast<Child *>(this)->map(static_cast<TypeReference *>(o1));
        break;
    case CLASSID_TYPETPASSIGN:
        static_cast<Child *>(this)->map(static_cast<TypeTPAssign *>(o1));
        break;
    case CLASSID_TYPETP:
        static_cast<Child *>(this)->map(static_cast<TypeTP *>(o1));
        break;
    case CLASSID_UNSIGNED:
        static_cast<Child *>(this)->map(static_cast<Unsigned *>(o1));
        break;
    case CLASSID_VALUESTATEMENT:
        static_cast<Child *>(this)->map(static_cast<ValueStatement *>(o1));
        break;
    case CLASSID_VALUETPASSIGN:
        static_cast<Child *>(this)->map(static_cast<ValueTPAssign *>(o1));
        break;
    case CLASSID_VALUETP:
        static_cast<Child *>(this)->map(static_cast<ValueTP *>(o1));
        break;
    case CLASSID_VARIABLE:
        static_cast<Child *>(this)->map(static_cast<Variable *>(o1));
        break;
    case CLASSID_VIEW:
        static_cast<Child *>(this)->map(static_cast<View *>(o1));
        break;
    case CLASSID_VIEWREFERENCE:
        static_cast<Child *>(this)->map(static_cast<ViewReference *>(o1));
        break;
    case CLASSID_WAIT:
        static_cast<Child *>(this)->map(static_cast<Wait *>(o1));
        break;
    case CLASSID_WHENALT:
        static_cast<Child *>(this)->map(static_cast<WhenAlt *>(o1));
        break;
    case CLASSID_WHEN:
        static_cast<Child *>(this)->map(static_cast<When *>(o1));
        break;
    case CLASSID_WHILE:
        static_cast<Child *>(this)->map(static_cast<While *>(o1));
        break;
    case CLASSID_WITHALT:
        static_cast<Child *>(this)->map(static_cast<WithAlt *>(o1));
        break;
    case CLASSID_WITH:
        static_cast<Child *>(this)->map(static_cast<With *>(o1));
        break;
    default:
        messageError("Unexpected object", o1, nullptr);
    }
}

// ///////////////////////////////////////////////////////////////////
// BiVisitor
// ///////////////////////////////////////////////////////////////////

template <class Child>
BiVisitor<Child>::BiVisitor()
    : MonoVisitor<Child>()
{
    // ntd
}

template <class Child> BiVisitor<Child>::~BiVisitor()
{
    // ntd
}

template <class Child> void BiVisitor<Child>::callMap(Object *o1, Object *o2)
{
    switch (o1->getClassId()) {
    case CLASSID_AGGREGATEALT:
        _rebind(static_cast<AggregateAlt *>(o1), o2);
        break;
    case CLASSID_AGGREGATE:
        _rebind(static_cast<Aggregate *>(o1), o2);
        break;
    case CLASSID_ALIAS:
        _rebind(static_cast<Alias *>(o1), o2);
        break;
    case CLASSID_ARRAY:
        _rebind(static_cast<Array *>(o1), o2);
        break;
    case CLASSID_ASSIGN:
        _rebind(static_cast<Assign *>(o1), o2);
        break;
    case CLASSID_BIT:
        _rebind(static_cast<Bit *>(o1), o2);
        break;
    case CLASSID_BITVALUE:
        _rebind(static_cast<BitValue *>(o1), o2);
        break;
    case CLASSID_BITVECTOR:
        _rebind(static_cast<Bitvector *>(o1), o2);
        break;
    case CLASSID_BITVECTORVALUE:
        _rebind(static_cast<BitvectorValue *>(o1), o2);
        break;
    case CLASSID_BOOL:
        _rebind(static_cast<Bool *>(o1), o2);
        break;
    case CLASSID_BOOLVALUE:
        _rebind(static_cast<BoolValue *>(o1), o2);
        break;
    case CLASSID_BREAK:
        _rebind(static_cast<Break *>(o1), o2);
        break;
    case CLASSID_CAST:
        _rebind(static_cast<Cast *>(o1), o2);
        break;
    case CLASSID_CHAR:
        _rebind(static_cast<Char *>(o1), o2);
        break;
    case CLASSID_CHARVALUE:
        _rebind(static_cast<CharValue *>(o1), o2);
        break;
    case CLASSID_CONST:
        _rebind(static_cast<Const *>(o1), o2);
        break;
    case CLASSID_CONTENTS:
        _rebind(static_cast<Contents *>(o1), o2);
        break;
    case CLASSID_CONTINUE:
        _rebind(static_cast<Continue *>(o1), o2);
        break;
    case CLASSID_DESIGNUNIT:
        _rebind(static_cast<DesignUnit *>(o1), o2);
        break;
    case CLASSID_ENTITY:
        _rebind(static_cast<Entity *>(o1), o2);
        break;
    case CLASSID_ENUM:
        _rebind(static_cast<Enum *>(o1), o2);
        break;
    case CLASSID_ENUMVALUE:
        _rebind(static_cast<EnumValue *>(o1), o2);
        break;
    case CLASSID_EVENT:
        _rebind(static_cast<Event *>(o1), o2);
        break;
    case CLASSID_EXPRESSION:
        _rebind(static_cast<Expression *>(o1), o2);
        break;
    case CLASSID_FIELD:
        _rebind(static_cast<Field *>(o1), o2);
        break;
    case CLASSID_FIELDREFERENCE:
        _rebind(static_cast<FieldReference *>(o1), o2);
        break;
    case CLASSID_FILE:
        _rebind(static_cast<File *>(o1), o2);
        break;
    case CLASSID_FORGENERATE:
        _rebind(static_cast<ForGenerate *>(o1), o2);
        break;
    case CLASSID_FOR:
        _rebind(static_cast<For *>(o1), o2);
        break;
    case CLASSID_FUNCTIONCALL:
        _rebind(static_cast<FunctionCall *>(o1), o2);
        break;
    case CLASSID_FUNCTION:
        _rebind(static_cast<Function *>(o1), o2);
        break;
    case CLASSID_GLOBALACTION:
        _rebind(static_cast<GlobalAction *>(o1), o2);
        break;
    case CLASSID_IDENTIFIER:
        _rebind(static_cast<Identifier *>(o1), o2);
        break;
    case CLASSID_IFALT:
        _rebind(static_cast<IfAlt *>(o1), o2);
        break;
    case CLASSID_IFGENERATE:
        _rebind(static_cast<IfGenerate *>(o1), o2);
        break;
    case CLASSID_IF:
        _rebind(static_cast<If *>(o1), o2);
        break;
    case CLASSID_INSTANCE:
        _rebind(static_cast<Instance *>(o1), o2);
        break;
    case CLASSID_INT:
        _rebind(static_cast<Int *>(o1), o2);
        break;
    case CLASSID_INTVALUE:
        _rebind(static_cast<IntValue *>(o1), o2);
        break;
    case CLASSID_LIBRARYDEF:
        _rebind(static_cast<LibraryDef *>(o1), o2);
        break;
    case CLASSID_LIBRARY:
        _rebind(static_cast<Library *>(o1), o2);
        break;
    case CLASSID_MEMBER:
        _rebind(static_cast<Member *>(o1), o2);
        break;
    case CLASSID_NULL:
        _rebind(static_cast<Null *>(o1), o2);
        break;
    case CLASSID_PARAMETERASSIGN:
        _rebind(static_cast<ParameterAssign *>(o1), o2);
        break;
    case CLASSID_PARAMETER:
        _rebind(static_cast<Parameter *>(o1), o2);
        break;
    case CLASSID_POINTER:
        _rebind(static_cast<Pointer *>(o1), o2);
        break;
    case CLASSID_PORTASSIGN:
        _rebind(static_cast<PortAssign *>(o1), o2);
        break;
    case CLASSID_PORT:
        _rebind(static_cast<Port *>(o1), o2);
        break;
    case CLASSID_PROCEDURECALL:
        _rebind(static_cast<ProcedureCall *>(o1), o2);
        break;
    case CLASSID_PROCEDURE:
        _rebind(static_cast<Procedure *>(o1), o2);
        break;
    case CLASSID_RANGE:
        _rebind(static_cast<Range *>(o1), o2);
        break;
    case CLASSID_REAL:
        _rebind(static_cast<Real *>(o1), o2);
        break;
    case CLASSID_REALVALUE:
        _rebind(static_cast<RealValue *>(o1), o2);
        break;
    case CLASSID_RECORD:
        _rebind(static_cast<Record *>(o1), o2);
        break;
    case CLASSID_RECORDVALUEALT:
        _rebind(static_cast<RecordValueAlt *>(o1), o2);
        break;
    case CLASSID_RECORDVALUE:
        _rebind(static_cast<RecordValue *>(o1), o2);
        break;
    case CLASSID_REFERENCE:
        _rebind(static_cast<Reference *>(o1), o2);
        break;
    case CLASSID_RETURN:
        _rebind(static_cast<Return *>(o1), o2);
        break;
    case CLASSID_SIGNAL:
        _rebind(static_cast<Signal *>(o1), o2);
        break;
    case CLASSID_SIGNED:
        _rebind(static_cast<Signed *>(o1), o2);
        break;
    case CLASSID_SLICE:
        _rebind(static_cast<Slice *>(o1), o2);
        break;
    case CLASSID_STATE:
        _rebind(static_cast<State *>(o1), o2);
        break;
    case CLASSID_STATETABLE:
        _rebind(static_cast<StateTable *>(o1), o2);
        break;
    case CLASSID_STRING:
        _rebind(static_cast<String *>(o1), o2);
        break;
    case CLASSID_STRINGVALUE:
        _rebind(static_cast<StringValue *>(o1), o2);
        break;
    case CLASSID_SWITCHALT:
        _rebind(static_cast<SwitchAlt *>(o1), o2);
        break;
    case CLASSID_SWITCH:
        _rebind(static_cast<Switch *>(o1), o2);
        break;
    case CLASSID_SYSTEM:
        _rebind(static_cast<System *>(o1), o2);
        break;
    case CLASSID_TIME:
        _rebind(static_cast<Time *>(o1), o2);
        break;
    case CLASSID_TIMEVALUE:
        _rebind(static_cast<TimeValue *>(o1), o2);
        break;
    case CLASSID_TRANSITION:
        _rebind(static_cast<Transition *>(o1), o2);
        break;
    case CLASSID_TYPEDEF:
        _rebind(static_cast<TypeDef *>(o1), o2);
        break;
    case CLASSID_TYPEREFERENCE:
        _rebind(static_cast<TypeReference *>(o1), o2);
        break;
    case CLASSID_TYPETPASSIGN:
        _rebind(static_cast<TypeTPAssign *>(o1), o2);
        break;
    case CLASSID_TYPETP:
        _rebind(static_cast<TypeTP *>(o1), o2);
        break;
    case CLASSID_UNSIGNED:
        _rebind(static_cast<Unsigned *>(o1), o2);
        break;
    case CLASSID_VALUESTATEMENT:
        _rebind(static_cast<ValueStatement *>(o1), o2);
        break;
    case CLASSID_VALUETPASSIGN:
        _rebind(static_cast<ValueTPAssign *>(o1), o2);
        break;
    case CLASSID_VALUETP:
        _rebind(static_cast<ValueTP *>(o1), o2);
        break;
    case CLASSID_VARIABLE:
        _rebind(static_cast<Variable *>(o1), o2);
        break;
    case CLASSID_VIEW:
        _rebind(static_cast<View *>(o1), o2);
        break;
    case CLASSID_VIEWREFERENCE:
        _rebind(static_cast<ViewReference *>(o1), o2);
        break;
    case CLASSID_WAIT:
        _rebind(static_cast<Wait *>(o1), o2);
        break;
    case CLASSID_WHENALT:
        _rebind(static_cast<WhenAlt *>(o1), o2);
        break;
    case CLASSID_WHEN:
        _rebind(static_cast<When *>(o1), o2);
        break;
    case CLASSID_WHILE:
        _rebind(static_cast<While *>(o1), o2);
        break;
    case CLASSID_WITHALT:
        _rebind(static_cast<WithAlt *>(o1), o2);
        break;
    case CLASSID_WITH:
        _rebind(static_cast<With *>(o1), o2);
        break;
    default:
        messageError("Unexpected object", o1, nullptr);
        ;
    }
}

template <class Child> template <typename T> void BiVisitor<Child>::_rebind(T *o1, Object *o2)
{
    switch (o2->getClassId()) {
    case CLASSID_AGGREGATEALT:
        static_cast<Child *>(this)->map(o1, static_cast<AggregateAlt *>(o2));
        break;
    case CLASSID_AGGREGATE:
        static_cast<Child *>(this)->map(o1, static_cast<Aggregate *>(o2));
        break;
    case CLASSID_ALIAS:
        static_cast<Child *>(this)->map(o1, static_cast<Alias *>(o2));
        break;
    case CLASSID_ARRAY:
        static_cast<Child *>(this)->map(o1, static_cast<Array *>(o2));
        break;
    case CLASSID_ASSIGN:
        static_cast<Child *>(this)->map(o1, static_cast<Assign *>(o2));
        break;
    case CLASSID_BIT:
        static_cast<Child *>(this)->map(o1, static_cast<Bit *>(o2));
        break;
    case CLASSID_BITVALUE:
        static_cast<Child *>(this)->map(o1, static_cast<BitValue *>(o2));
        break;
    case CLASSID_BITVECTOR:
        static_cast<Child *>(this)->map(o1, static_cast<Bitvector *>(o2));
        break;
    case CLASSID_BITVECTORVALUE:
        static_cast<Child *>(this)->map(o1, static_cast<BitvectorValue *>(o2));
        break;
    case CLASSID_BOOL:
        static_cast<Child *>(this)->map(o1, static_cast<Bool *>(o2));
        break;
    case CLASSID_BOOLVALUE:
        static_cast<Child *>(this)->map(o1, static_cast<BoolValue *>(o2));
        break;
    case CLASSID_BREAK:
        static_cast<Child *>(this)->map(o1, static_cast<Break *>(o2));
        break;
    case CLASSID_CAST:
        static_cast<Child *>(this)->map(o1, static_cast<Cast *>(o2));
        break;
    case CLASSID_CHAR:
        static_cast<Child *>(this)->map(o1, static_cast<Char *>(o2));
        break;
    case CLASSID_CHARVALUE:
        static_cast<Child *>(this)->map(o1, static_cast<CharValue *>(o2));
        break;
    case CLASSID_CONST:
        static_cast<Child *>(this)->map(o1, static_cast<Const *>(o2));
        break;
    case CLASSID_CONTENTS:
        static_cast<Child *>(this)->map(o1, static_cast<Contents *>(o2));
        break;
    case CLASSID_CONTINUE:
        static_cast<Child *>(this)->map(o1, static_cast<Continue *>(o2));
        break;
    case CLASSID_DESIGNUNIT:
        static_cast<Child *>(this)->map(o1, static_cast<DesignUnit *>(o2));
        break;
    case CLASSID_ENTITY:
        static_cast<Child *>(this)->map(o1, static_cast<Entity *>(o2));
        break;
    case CLASSID_ENUM:
        static_cast<Child *>(this)->map(o1, static_cast<Enum *>(o2));
        break;
    case CLASSID_ENUMVALUE:
        static_cast<Child *>(this)->map(o1, static_cast<EnumValue *>(o2));
        break;
    case CLASSID_EVENT:
        static_cast<Child *>(this)->map(o1, static_cast<Event *>(o2));
        break;
    case CLASSID_EXPRESSION:
        static_cast<Child *>(this)->map(o1, static_cast<Expression *>(o2));
        break;
    case CLASSID_FIELD:
        static_cast<Child *>(this)->map(o1, static_cast<Field *>(o2));
        break;
    case CLASSID_FIELDREFERENCE:
        static_cast<Child *>(this)->map(o1, static_cast<FieldReference *>(o2));
        break;
    case CLASSID_FILE:
        static_cast<Child *>(this)->map(o1, static_cast<File *>(o2));
        break;
    case CLASSID_FORGENERATE:
        static_cast<Child *>(this)->map(o1, static_cast<ForGenerate *>(o2));
        break;
    case CLASSID_FOR:
        static_cast<Child *>(this)->map(o1, static_cast<For *>(o2));
        break;
    case CLASSID_FUNCTIONCALL:
        static_cast<Child *>(this)->map(o1, static_cast<FunctionCall *>(o2));
        break;
    case CLASSID_FUNCTION:
        static_cast<Child *>(this)->map(o1, static_cast<Function *>(o2));
        break;
    case CLASSID_GLOBALACTION:
        static_cast<Child *>(this)->map(o1, static_cast<GlobalAction *>(o2));
        break;
    case CLASSID_IDENTIFIER:
        static_cast<Child *>(this)->map(o1, static_cast<Identifier *>(o2));
        break;
    case CLASSID_IFALT:
        static_cast<Child *>(this)->map(o1, static_cast<IfAlt *>(o2));
        break;
    case CLASSID_IFGENERATE:
        static_cast<Child *>(this)->map(o1, static_cast<IfGenerate *>(o2));
        break;
    case CLASSID_IF:
        static_cast<Child *>(this)->map(o1, static_cast<If *>(o2));
        break;
    case CLASSID_INSTANCE:
        static_cast<Child *>(this)->map(o1, static_cast<Instance *>(o2));
        break;
    case CLASSID_INT:
        static_cast<Child *>(this)->map(o1, static_cast<Int *>(o2));
        break;
    case CLASSID_INTVALUE:
        static_cast<Child *>(this)->map(o1, static_cast<IntValue *>(o2));
        break;
    case CLASSID_LIBRARYDEF:
        static_cast<Child *>(this)->map(o1, static_cast<LibraryDef *>(o2));
        break;
    case CLASSID_LIBRARY:
        static_cast<Child *>(this)->map(o1, static_cast<Library *>(o2));
        break;
    case CLASSID_MEMBER:
        static_cast<Child *>(this)->map(o1, static_cast<Member *>(o2));
        break;
    case CLASSID_NULL:
        static_cast<Child *>(this)->map(o1, static_cast<Null *>(o2));
        break;
    case CLASSID_PARAMETERASSIGN:
        static_cast<Child *>(this)->map(o1, static_cast<ParameterAssign *>(o2));
        break;
    case CLASSID_PARAMETER:
        static_cast<Child *>(this)->map(o1, static_cast<Parameter *>(o2));
        break;
    case CLASSID_POINTER:
        static_cast<Child *>(this)->map(o1, static_cast<Pointer *>(o2));
        break;
    case CLASSID_PORTASSIGN:
        static_cast<Child *>(this)->map(o1, static_cast<PortAssign *>(o2));
        break;
    case CLASSID_PORT:
        static_cast<Child *>(this)->map(o1, static_cast<Port *>(o2));
        break;
    case CLASSID_PROCEDURECALL:
        static_cast<Child *>(this)->map(o1, static_cast<ProcedureCall *>(o2));
        break;
    case CLASSID_PROCEDURE:
        static_cast<Child *>(this)->map(o1, static_cast<Procedure *>(o2));
        break;
    case CLASSID_RANGE:
        static_cast<Child *>(this)->map(o1, static_cast<Range *>(o2));
        break;
    case CLASSID_REAL:
        static_cast<Child *>(this)->map(o1, static_cast<Real *>(o2));
        break;
    case CLASSID_REALVALUE:
        static_cast<Child *>(this)->map(o1, static_cast<RealValue *>(o2));
        break;
    case CLASSID_RECORD:
        static_cast<Child *>(this)->map(o1, static_cast<Record *>(o2));
        break;
    case CLASSID_RECORDVALUEALT:
        static_cast<Child *>(this)->map(o1, static_cast<RecordValueAlt *>(o2));
        break;
    case CLASSID_RECORDVALUE:
        static_cast<Child *>(this)->map(o1, static_cast<RecordValue *>(o2));
        break;
    case CLASSID_REFERENCE:
        static_cast<Child *>(this)->map(o1, static_cast<Reference *>(o2));
        break;
    case CLASSID_RETURN:
        static_cast<Child *>(this)->map(o1, static_cast<Return *>(o2));
        break;
    case CLASSID_SIGNAL:
        static_cast<Child *>(this)->map(o1, static_cast<Signal *>(o2));
        break;
    case CLASSID_SIGNED:
        static_cast<Child *>(this)->map(o1, static_cast<Signed *>(o2));
        break;
    case CLASSID_SLICE:
        static_cast<Child *>(this)->map(o1, static_cast<Slice *>(o2));
        break;
    case CLASSID_STATE:
        static_cast<Child *>(this)->map(o1, static_cast<State *>(o2));
        break;
    case CLASSID_STATETABLE:
        static_cast<Child *>(this)->map(o1, static_cast<StateTable *>(o2));
        break;
    case CLASSID_STRING:
        static_cast<Child *>(this)->map(o1, static_cast<String *>(o2));
        break;
    case CLASSID_STRINGVALUE:
        static_cast<Child *>(this)->map(o1, static_cast<StringValue *>(o2));
        break;
    case CLASSID_SWITCHALT:
        static_cast<Child *>(this)->map(o1, static_cast<SwitchAlt *>(o2));
        break;
    case CLASSID_SWITCH:
        static_cast<Child *>(this)->map(o1, static_cast<Switch *>(o2));
        break;
    case CLASSID_SYSTEM:
        static_cast<Child *>(this)->map(o1, static_cast<System *>(o2));
        break;
    case CLASSID_TIME:
        static_cast<Child *>(this)->map(o1, static_cast<Time *>(o2));
        break;
    case CLASSID_TIMEVALUE:
        static_cast<Child *>(this)->map(o1, static_cast<TimeValue *>(o2));
        break;
    case CLASSID_TRANSITION:
        static_cast<Child *>(this)->map(o1, static_cast<Transition *>(o2));
        break;
    case CLASSID_TYPEDEF:
        static_cast<Child *>(this)->map(o1, static_cast<TypeDef *>(o2));
        break;
    case CLASSID_TYPEREFERENCE:
        static_cast<Child *>(this)->map(o1, static_cast<TypeReference *>(o2));
        break;
    case CLASSID_TYPETPASSIGN:
        static_cast<Child *>(this)->map(o1, static_cast<TypeTPAssign *>(o2));
        break;
    case CLASSID_TYPETP:
        static_cast<Child *>(this)->map(o1, static_cast<TypeTP *>(o2));
        break;
    case CLASSID_UNSIGNED:
        static_cast<Child *>(this)->map(o1, static_cast<Unsigned *>(o2));
        break;
    case CLASSID_VALUESTATEMENT:
        static_cast<Child *>(this)->map(o1, static_cast<ValueStatement *>(o2));
        break;
    case CLASSID_VALUETPASSIGN:
        static_cast<Child *>(this)->map(o1, static_cast<ValueTPAssign *>(o2));
        break;
    case CLASSID_VALUETP:
        static_cast<Child *>(this)->map(o1, static_cast<ValueTP *>(o2));
        break;
    case CLASSID_VARIABLE:
        static_cast<Child *>(this)->map(o1, static_cast<Variable *>(o2));
        break;
    case CLASSID_VIEW:
        static_cast<Child *>(this)->map(o1, static_cast<View *>(o2));
        break;
    case CLASSID_VIEWREFERENCE:
        static_cast<Child *>(this)->map(o1, static_cast<ViewReference *>(o2));
        break;
    case CLASSID_WAIT:
        static_cast<Child *>(this)->map(o1, static_cast<Wait *>(o2));
        break;
    case CLASSID_WHENALT:
        static_cast<Child *>(this)->map(o1, static_cast<WhenAlt *>(o2));
        break;
    case CLASSID_WHEN:
        static_cast<Child *>(this)->map(o1, static_cast<When *>(o2));
        break;
    case CLASSID_WHILE:
        static_cast<Child *>(this)->map(o1, static_cast<While *>(o2));
        break;
    case CLASSID_WITHALT:
        static_cast<Child *>(this)->map(o1, static_cast<WithAlt *>(o2));
        break;
    case CLASSID_WITH:
        static_cast<Child *>(this)->map(o1, static_cast<With *>(o2));
        break;
    default:
        break;
    }
}

} // namespace hif
