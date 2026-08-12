/// @file compare.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/compare.hpp"

#include "hif/BiVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

#ifdef __clang__
#    pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

namespace hif
{

namespace
{

enum ObjectOrderType : unsigned char {
    ExpressionNode,
    FunctionCallNode,
    SliceNode,
    MemberNode,
    FieldReferenceNode,
    IdentifierNode, // Must be smaller than ConstValues
    CastNode,       // Must be smaller than ConstValues
    ActionNode,
    AliasNode,
    ArrayNode,
    AssignNode,
    BadNode,
    BaseContentsNode,
    SystemNode,
    BitNode,
    BitvectorNode,
    BoolNode,
    BitValueNode,
    BitvectorValueNode,
    BoolValueNode,
    CharValueNode,
    IntValueNode,
    RealValueNode,
    StringValueNode,
    TimeValueNode,
    AggregateAltNode,
    AggregateNode,
    RecordValueNode,
    RecordValueAltNode,
    IfAltNode,
    IfNode,
    CharNode,
    CodeInfoNode,
    CompositeTypeNode,
    ConstNode,
    ContentsNode,
    DesignUnitNode,
    DataDeclarationNode,
    DeclarationNode,
    DefaultNode,
    DirNode,
    EnumNode,
    EnumValueNode,
    EventNode,
    BreakNode,
    FieldNode,
    FileNode,
    ForGenerateNode,
    ForNode,
    FunctionNode,
    GenerateNode,
    GlobalActionNode,
    IfGenerateNode,
    EntityNode,
    InitvalNode,
    InstanceNode,
    IntNode,
    LibraryDefNode,
    LibraryNode,
    ContinueNode,
    NullNode,
    TransitionNode,
    ObjectNode,
    ProcedureCallNode,
    PPAssignNode,
    ParameterNode,
    ParameterAssignNode,
    PointerNode,
    PortNode,
    PortAssignNode,
    ProcedureNode,
    RangeNode,
    RealNode,
    RecordNode,
    RefNode,
    ReferenceNode,
    ReferencedTypeNode,
    ReturnNode,
    StateTableNode,
    ScopeNode,
    SignalNode,
    SignedNode,
    SimpactNode,
    SimpleTypeNode,
    StateNode,
    StaterefNode,
    StringNode,
    SubProgramNode,
    SwitchAltNode,
    SwitchNode,
    TimeNode,
    TypeDeclarationNode,
    TypeDefNode,
    TypeNode,
    TypeReferenceNode,
    TypeTPNode,
    TypeTPAssignNode,
    TPAssignNode,
    UnsignedNode,
    ValueNode,
    ValueTPNode,
    ValueTPAssignNode,
    VariableNode,
    ViewNode,
    ViewReferenceNode,
    WaitNode,
    WhenAltNode,
    WhenNode,
    WhileNode,
    WithAltNode,
    WithNode
};

typedef std::map<std::string, ObjectOrderType> ClassMap;

ClassMap classMap;
void initMap()
{
    static bool initialized = false;
    if (initialized)
        return;
    initialized = true;

    classMap["Expression"]      = ExpressionNode;
    classMap["FunctionCall"]    = FunctionCallNode;
    classMap["Slice"]           = SliceNode;
    classMap["Member"]          = MemberNode;
    classMap["FieldReference"]  = FieldReferenceNode;
    classMap["Identifier"]      = IdentifierNode;
    classMap["Action"]          = ActionNode;
    classMap["AggregateAlt"]    = AggregateAltNode;
    classMap["Aggregate"]       = AggregateNode;
    classMap["Alias"]           = AliasNode;
    classMap["Array"]           = ArrayNode;
    classMap["Assign"]          = AssignNode;
    classMap["Bad"]             = BadNode;
    classMap["BaseContents"]    = BaseContentsNode;
    classMap["System"]          = SystemNode;
    classMap["Bit"]             = BitNode;
    classMap["BitValue"]        = BitValueNode;
    classMap["Bitvector"]       = BitvectorNode;
    classMap["BitvectorValue"]  = BitvectorValueNode;
    classMap["Bool"]            = BoolNode;
    classMap["BoolValue"]       = BoolValueNode;
    classMap["IfAlt"]           = IfAltNode;
    classMap["If"]              = IfNode;
    classMap["Cast"]            = CastNode;
    classMap["Char"]            = CharNode;
    classMap["CharValue"]       = CharValueNode;
    classMap["CodeInfo"]        = CodeInfoNode;
    classMap["CompositeType"]   = CompositeTypeNode;
    classMap["Const"]           = ConstNode;
    classMap["Contents"]        = ContentsNode;
    classMap["DesignUnit"]      = DesignUnitNode;
    classMap["DataDeclaration"] = DataDeclarationNode;
    classMap["Declaration"]     = DeclarationNode;
    classMap["Default"]         = DefaultNode;
    classMap["Dir"]             = DirNode;
    classMap["Enum"]            = EnumNode;
    classMap["EnumValue"]       = EnumValueNode;
    classMap["Event"]           = EventNode;
    classMap["Break"]           = BreakNode;
    classMap["Field"]           = FieldNode;
    classMap["File"]            = FileNode;
    classMap["ForGenerate"]     = ForGenerateNode;
    classMap["For"]             = ForNode;
    classMap["Function"]        = FunctionNode;
    classMap["Generate"]        = GenerateNode;
    classMap["GlobalAction"]    = GlobalActionNode;
    classMap["IfGenerate"]      = IfGenerateNode;
    classMap["Entity"]          = EntityNode;
    classMap["Initval"]         = InitvalNode;
    classMap["Instance"]        = InstanceNode;
    classMap["Int"]             = IntNode;
    classMap["IntValue"]        = IntValueNode;
    classMap["LibraryDef"]      = LibraryDefNode;
    classMap["Library"]         = LibraryNode;
    classMap["Continue"]        = ContinueNode;
    classMap["Null"]            = NullNode;
    classMap["Transition"]      = TransitionNode;
    classMap["Object"]          = ObjectNode;
    classMap["ProcedureCall"]   = ProcedureCallNode;
    classMap["PPAssign"]        = PPAssignNode;
    classMap["Parameter"]       = ParameterNode;
    classMap["ParameterAssign"] = ParameterAssignNode;
    classMap["Pointer"]         = PointerNode;
    classMap["Port"]            = PortNode;
    classMap["PortAssign"]      = PortAssignNode;
    classMap["Procedure"]       = ProcedureNode;
    classMap["Range"]           = RangeNode;
    classMap["Real"]            = RealNode;
    classMap["RealValue"]       = RealValueNode;
    classMap["Record"]          = RecordNode;
    classMap["RecordValue"]     = RecordValueNode;
    classMap["RecordValueAlt"]  = RecordValueAltNode;
    classMap["Ref"]             = RefNode;
    classMap["Reference"]       = ReferenceNode;
    classMap["ReferencedType"]  = ReferencedTypeNode;
    classMap["Return"]          = ReturnNode;
    classMap["StateTable"]      = StateTableNode;
    classMap["Scope"]           = ScopeNode;
    classMap["Signal"]          = SignalNode;
    classMap["Signed"]          = SignedNode;
    classMap["Simpact"]         = SimpactNode;
    classMap["SimpleType"]      = SimpleTypeNode;
    classMap["State"]           = StateNode;
    classMap["Stateref"]        = StaterefNode;
    classMap["String"]          = StringNode;
    classMap["SubProgram"]      = SubProgramNode;
    classMap["SwitchAlt"]       = SwitchAltNode;
    classMap["Switch"]          = SwitchNode;
    classMap["StringValue"]     = StringValueNode;
    classMap["Time"]            = TimeNode;
    classMap["TimeValue"]       = TimeValueNode;
    classMap["TypeDeclaration"] = TypeDeclarationNode;
    classMap["TypeDef"]         = TypeDefNode;
    classMap["Type"]            = TypeNode;
    classMap["TypeReference"]   = TypeReferenceNode;
    classMap["TypeTP"]          = TypeTPNode;
    classMap["TypeTPAssign"]    = TypeTPAssignNode;
    classMap["TPAssign"]        = TPAssignNode;
    classMap["Unsigned"]        = UnsignedNode;
    classMap["Value"]           = ValueNode;
    classMap["ValueTP"]         = ValueTPNode;
    classMap["ValueTPAssign"]   = ValueTPAssignNode;
    classMap["Variable"]        = VariableNode;
    classMap["View"]            = ViewNode;
    classMap["ViewReference"]   = ViewReferenceNode;
    classMap["Wait"]            = WaitNode;
    classMap["WhenAlt"]         = WhenAltNode;
    classMap["When"]            = WhenNode;
    classMap["While"]           = WhileNode;
    classMap["WithAlt"]         = WithAltNode;
    classMap["With"]            = WithNode;
}

int getObjectValue(Object *o)
{
    ClassMap::iterator i = classMap.find(hif::classIDToString(o->getClassId()));
    if (i == classMap.end()) {
        messageError("Not found class name of given object", o, nullptr);
    }

    return i->second;
}

/// @brief Used to compare enumerations.
template <typename T> int _compareActualValue(T enum1, T enum2)
{
    if (enum1 < enum2)
        return -1;
    else if (enum1 > enum2)
        return 1;
    return 0;
}
/// @brief Used to compare lists.
template <typename T> int _compareList(BList<T> &list1, BList<T> &list2)
{
    int ret = _compareActualValue(list1.size(), list2.size());
    if (ret != 0)
        return ret;

    typename BList<T>::iterator it1(list1.begin());
    typename BList<T>::iterator it2(list2.begin());
    for (; it1 != list1.end(); ++it1, ++it2) {
        ret = compare(*it1, *it2);
        if (ret != 0)
            return ret;
    }

    return 0;
}
/// @brief Used to compare ConstValues.
template <typename T> int _compareConstValue(T *obj1, T *obj2)
{
    int ret = _compareActualValue(obj1->getValue(), obj2->getValue());
    if (ret != 0)
        return ret;

    return compare(obj1->getType(), obj2->getType());
}
/// @brief Compare two objects basing on the total ordering defined by IsA
/// function.
/// TODO At the moment, only support: Values, Types.
class CompareVisitor : public BiVisitor<CompareVisitor>
{
public:
    CompareVisitor();
    virtual ~CompareVisitor();

    int getResult() const;

    void map(Object *obj1, Object *obj2);

    void map(Aggregate *obj1, Aggregate *obj2);
    void map(AggregateAlt *obj1, AggregateAlt *obj2);
    void map(Alias *obj1, Alias *obj2);
    void map(Array *obj1, Array *obj2);
    void map(Assign *obj1, Assign *obj2);
    void map(System *obj1, System *obj2);
    void map(Bit *obj1, Bit *obj2);
    void map(BitValue *obj1, BitValue *obj2);
    void map(Bitvector *obj1, Bitvector *obj2);
    void map(BitvectorValue *obj1, BitvectorValue *obj2);
    void map(Bool *obj1, Bool *obj2);
    void map(BoolValue *obj1, BoolValue *obj2);
    void map(Break *obj1, Break *obj2);
    void map(Cast *obj1, Cast *obj2);
    void map(Char *obj1, Char *obj2);
    void map(CharValue *obj1, CharValue *obj2);
    void map(CompositeType *obj1, CompositeType *obj2);
    // Already performed by _compareConstValue
    //    void map( ConstValue * obj1, ConstValue * obj2 );
    void map(Const *obj1, Const *obj2);
    void map(Contents *obj1, Contents *obj2);
    void map(Continue *obj1, Continue *obj2);
    void map(DesignUnit *obj1, DesignUnit *obj2);
    void map(Enum *obj1, Enum *obj2);
    void map(EnumValue *obj1, EnumValue *obj2);
    void map(Event *obj1, Event *obj2);
    void map(Expression *obj1, Expression *obj2);
    void map(Field *obj1, Field *obj2);
    void map(FieldReference *obj1, FieldReference *obj2);
    void map(File *obj1, File *obj2);
    void map(For *obj1, For *obj2);
    void map(ForGenerate *obj1, ForGenerate *obj2);
    void map(Function *obj1, Function *obj2);
    void map(FunctionCall *obj1, FunctionCall *obj2);
    void map(GlobalAction *obj1, GlobalAction *obj2);
    void map(Entity *obj1, Entity *obj2);
    void map(Identifier *obj1, Identifier *obj2);
    void map(If *obj1, If *obj2);
    void map(IfAlt *obj1, IfAlt *obj2);
    void map(IfGenerate *obj1, IfGenerate *obj2);
    void map(Instance *obj1, Instance *obj2);
    void map(Int *obj1, Int *obj2);
    void map(IntValue *obj1, IntValue *obj2);
    void map(LibraryDef *obj1, LibraryDef *obj2);
    void map(Library *obj1, Library *obj2);
    void map(Null *obj1, Null *obj2);
    void map(Member *obj1, Member *obj2);
    void map(Transition *obj1, Transition *obj2);
    void map(ParameterAssign *obj1, ParameterAssign *obj2);
    void map(Parameter *obj1, Parameter *obj2);
    void map(Pointer *obj1, Pointer *obj2);
    void map(PortAssign *obj1, PortAssign *obj2);
    void map(Port *obj1, Port *obj2);
    void map(PrefixedReference *obj1, PrefixedReference *obj2);
    void map(Procedure *obj1, Procedure *obj2);
    void map(ProcedureCall *obj1, ProcedureCall *obj2);
    void map(PPAssign *obj1, PPAssign *obj2);
    void map(Range *obj1, Range *obj2);
    void map(Real *obj1, Real *obj2);
    void map(RealValue *obj1, RealValue *obj2);
    void map(Record *obj1, Record *obj2);
    void map(RecordValue *obj1, RecordValue *obj2);
    void map(RecordValueAlt *obj1, RecordValueAlt *obj2);
    void map(Reference *obj1, Reference *obj2);
    void map(ReferencedAssign *obj1, ReferencedAssign *obj2);
    void map(Return *obj1, Return *obj2);
    void map(ScopedType *obj1, ScopedType *obj2);
    void map(Signal *obj1, Signal *obj2);
    void map(Signed *obj1, Signed *obj2);
    void map(SimpleType *obj1, SimpleType *obj2);
    void map(Slice *obj1, Slice *obj2);
    void map(State *obj1, State *obj2);
    void map(String *obj1, String *obj2);
    void map(StateTable *obj1, StateTable *obj2);
    void map(SwitchAlt *obj1, SwitchAlt *obj2);
    void map(Switch *obj1, Switch *obj2);
    void map(StringValue *obj1, StringValue *obj2);
    void map(Time *obj1, Time *obj2);
    void map(TimeValue *obj1, TimeValue *obj2);
    void map(TPAssign *obj1, TPAssign *obj2);
    void map(Type *obj1, Type *obj2);
    void map(TypeDef *obj1, TypeDef *obj2);
    void map(TypeReference *obj1, TypeReference *obj2);
    void map(TypeTPAssign *obj1, TypeTPAssign *obj2);
    void map(TypeTP *obj1, TypeTP *obj2);
    void map(Unsigned *obj1, Unsigned *obj2);
    void map(ValueStatement *obj1, ValueStatement *obj2);
    void map(ValueTPAssign *obj1, ValueTPAssign *obj2);
    void map(ValueTP *obj1, ValueTP *obj2);
    void map(Variable *obj1, Variable *obj2);
    void map(View *obj1, View *obj2);
    void map(ViewReference *obj1, ViewReference *obj2);
    void map(Wait *obj1, Wait *obj2);
    void map(When *obj1, When *obj2);
    void map(WhenAlt *obj1, WhenAlt *obj2);
    void map(While *obj1, While *obj2);
    void map(With *obj1, With *obj2);
    void map(WithAlt *obj1, WithAlt *obj2);

private:
    int _result;

    // disabled.
    CompareVisitor(const CompareVisitor &);
    CompareVisitor &operator=(const CompareVisitor &);
};
CompareVisitor::CompareVisitor()
    : _result(0)
{
    // ntd
}

CompareVisitor::~CompareVisitor()
{
    // ntd
}

int CompareVisitor::getResult() const { return _result; }

void CompareVisitor::map(Object *obj1, Object *obj2)
{
    int obj1Val = getObjectValue(obj1);
    int obj2Val = getObjectValue(obj2);

    if (obj1Val < obj2Val) {
        _result = -1;
    } else if (obj1Val > obj2Val) {
        _result = 1;
    } else {
        messageError("Unexpected case", obj1, nullptr);
    }
}

void CompareVisitor::map(Aggregate *obj1, Aggregate *obj2)
{
    _result = compare(obj1->getOthers(), obj2->getOthers());
    if (_result != 0)
        return;

    _result = _compareList(obj1->alts, obj2->alts);
}

void CompareVisitor::map(AggregateAlt *obj1, AggregateAlt *obj2)
{
    _result = _compareList(obj1->indices, obj2->indices);
    if (_result != 0)
        return;

    _result = compare(obj1->getValue(), obj2->getValue());
}

void CompareVisitor::map(Alias *obj1, Alias * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);

    //    _result = _compareActualValue( obj1->isStandard(), obj2->isStandard() );
    //    if (_result != 0) return;

    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Array *obj1, Array *obj2)
{
    map(static_cast<CompositeType *>(obj1), static_cast<CompositeType *>(obj2));
    if (_result != 0)
        return;

    _result = _compareActualValue(obj1->isSigned(), obj2->isSigned());
    if (_result != 0)
        return;

    _result = compare(obj1->getType(), obj2->getType());
    if (_result != 0)
        return;

    _result = compare(obj1->getSpan(), obj2->getSpan());
}

void CompareVisitor::map(Assign *obj1, Assign * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(System *obj1, System * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Bit *obj1, Bit *obj2)
{
    map(static_cast<SimpleType *>(obj1), static_cast<SimpleType *>(obj2));
    if (_result != 0)
        return;

    _result = _compareActualValue(obj1->isLogic(), obj2->isLogic());
    if (_result != 0)
        return;

    _result = _compareActualValue(obj1->isResolved(), obj2->isResolved());
}

void CompareVisitor::map(BitValue *obj1, BitValue *obj2) { _result = _compareConstValue(obj1, obj2); }

void CompareVisitor::map(Bitvector *obj1, Bitvector *obj2)
{
    map(static_cast<SimpleType *>(obj1), static_cast<SimpleType *>(obj2));
    if (_result != 0)
        return;

    _result = _compareActualValue(obj1->isLogic(), obj2->isLogic());
    if (_result != 0)
        return;

    _result = _compareActualValue(obj1->isSigned(), obj2->isSigned());
    if (_result != 0)
        return;

    _result = _compareActualValue(obj1->isResolved(), obj2->isResolved());
    if (_result != 0)
        return;

    _result = compare(obj1->getSpan(), obj2->getSpan());
}

void CompareVisitor::map(BitvectorValue *obj1, BitvectorValue *obj2) { _result = _compareConstValue(obj1, obj2); }

void CompareVisitor::map(Bool *obj1, Bool *obj2)
{
    map(static_cast<SimpleType *>(obj1), static_cast<SimpleType *>(obj2));
}

void CompareVisitor::map(BoolValue *obj1, BoolValue *obj2) { _result = _compareConstValue(obj1, obj2); }

void CompareVisitor::map(Break *obj1, Break * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Cast *obj1, Cast *obj2)
{
    _result = compare(obj1->getType(), obj2->getType());
    if (_result != 0)
        return;

    _result = compare(obj1->getValue(), obj2->getValue());
}

void CompareVisitor::map(Char *obj1, Char *obj2)
{
    map(static_cast<SimpleType *>(obj1), static_cast<SimpleType *>(obj2));
}

void CompareVisitor::map(CharValue *obj1, CharValue *obj2) { _result = _compareConstValue(obj1, obj2); }

void CompareVisitor::map(CompositeType * /*obj1*/, CompositeType * /*obj2*/)
{
    // ntd
}

void CompareVisitor::map(Const *obj1, Const * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);

    //    _result = _compareActualValue( obj1->isStandard(), obj2->isStandard() );
    //    if (_result != 0) return;

    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Contents *obj1, Contents * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Continue *obj1, Continue * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(DesignUnit *obj1, DesignUnit * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Enum *obj1, Enum *obj2)
{
    map(static_cast<ScopedType *>(obj1), static_cast<ScopedType *>(obj2));
    if (_result != 0)
        return;

    _result = _compareList(obj1->values, obj2->values);
}

void CompareVisitor::map(EnumValue *obj1, EnumValue *obj2)
{
    _result = _compareActualValue(static_cast<std::string>(obj1->getName()), static_cast<std::string>(obj2->getName()));
    if (_result != 0)
        return;

    _result = compare(obj1->getValue(), obj2->getValue());
    if (_result != 0)
        return;

    _result = compare(obj1->getRange(), obj2->getRange());
}

void CompareVisitor::map(Event *obj1, Event *obj2)
{
    map(static_cast<SimpleType *>(obj1), static_cast<SimpleType *>(obj2));
}

void CompareVisitor::map(Expression *obj1, Expression *obj2)
{
    _result = _compareActualValue(obj1->getOperator(), obj2->getOperator());
    if (_result != 0)
        return;

    _result = compare(obj1->getValue1(), obj2->getValue1());
    if (_result != 0)
        return;

    _result = compare(obj1->getValue2(), obj2->getValue2());
}

void CompareVisitor::map(FunctionCall *obj1, FunctionCall *obj2)
{
    _result = _compareActualValue(static_cast<std::string>(obj1->getName()), static_cast<std::string>(obj2->getName()));
    if (_result != 0)
        return;

    _result = compare(obj1->getInstance(), obj2->getInstance());
    if (_result != 0)
        return;

    _result = _compareList(obj1->parameterAssigns, obj2->parameterAssigns);
    if (_result != 0)
        return;

    _result = _compareList(obj1->templateParameterAssigns, obj2->templateParameterAssigns);
}

void CompareVisitor::map(Field *obj1, Field * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(FieldReference *obj1, FieldReference *obj2)
{
    map(static_cast<PrefixedReference *>(obj1), static_cast<PrefixedReference *>(obj2));
    if (_result != 0)
        return;

    _result = _compareActualValue(static_cast<std::string>(obj1->getName()), static_cast<std::string>(obj2->getName()));
}

void CompareVisitor::map(File *obj1, File *obj2)
{
    map(static_cast<CompositeType *>(obj1), static_cast<CompositeType *>(obj2));
    if (_result != 0)
        return;

    _result = compare(obj1->getType(), obj2->getType());
}

void CompareVisitor::map(For *obj1, For * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(ForGenerate *obj1, ForGenerate * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Function *obj1, Function * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);

    //    _result = _compareActualValue( obj1->isStandard(), obj2->isStandard() );
    //    if (_result != 0) return;

    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(GlobalAction *obj1, GlobalAction * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Entity *obj1, Entity * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Identifier *obj1, Identifier *obj2)
{
    _result = _compareActualValue(static_cast<std::string>(obj1->getName()), static_cast<std::string>(obj2->getName()));
}

void CompareVisitor::map(If *obj1, If * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(IfAlt *obj1, IfAlt * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(IfGenerate *obj1, IfGenerate * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Instance *obj1, Instance *obj2)
{
    _result = _compareActualValue(static_cast<std::string>(obj1->getName()), static_cast<std::string>(obj2->getName()));
    if (_result != 0)
        return;

    _result = compare(obj1->getReferencedType(), obj2->getReferencedType());
    if (_result != 0)
        return;

    _result = compare(obj1->getValue(), obj2->getValue());
    if (_result != 0)
        return;

    _result = _compareList(obj1->portAssigns, obj2->portAssigns);
}

void CompareVisitor::map(Int *obj1, Int *obj2)
{
    map(static_cast<SimpleType *>(obj1), static_cast<SimpleType *>(obj2));
    if (_result != 0)
        return;

    _result = _compareActualValue(obj1->isSigned(), obj2->isSigned());
    if (_result != 0)
        return;

    _result = compare(obj1->getSpan(), obj2->getSpan());
}

void CompareVisitor::map(IntValue *obj1, IntValue *obj2) { _result = _compareConstValue(obj1, obj2); }

void CompareVisitor::map(LibraryDef *obj1, LibraryDef * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);

    //    _result = _compareActualValue( obj1->isStandard(), obj2->isStandard() );
    //    if (_result != 0) return;

    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Library *obj1, Library *obj2)
{
    _result = _compareActualValue(static_cast<std::string>(obj1->getName()), static_cast<std::string>(obj2->getName()));
    if (_result != 0)
        return;

    // TODO compare also filename when will be std::string

    _result = _compareActualValue(obj1->isStandard(), obj2->isStandard());
    if (_result != 0)
        return;

    _result = _compareActualValue(obj1->isSystem(), obj2->isSystem());
    if (_result != 0)
        return;

    _result = compare(obj1->getInstance(), obj2->getInstance());
}

void CompareVisitor::map(Null *obj1, Null * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Member *obj1, Member *obj2)
{
    map(static_cast<PrefixedReference *>(obj1), static_cast<PrefixedReference *>(obj2));
    if (_result != 0)
        return;

    _result = compare(obj1->getIndex(), obj2->getIndex());
}

void CompareVisitor::map(Transition *obj1, Transition * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(ParameterAssign *obj1, ParameterAssign *obj2)
{
    map(static_cast<PPAssign *>(obj1), static_cast<PPAssign *>(obj2));
}

void CompareVisitor::map(Parameter *obj1, Parameter * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(PrefixedReference *obj1, PrefixedReference *obj2)
{
    _result = compare(obj1->getPrefix(), obj2->getPrefix());
}

void CompareVisitor::map(ProcedureCall *obj1, ProcedureCall * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Pointer *obj1, Pointer *obj2)
{
    map(static_cast<CompositeType *>(obj1), static_cast<CompositeType *>(obj2));
    if (_result != 0)
        return;

    _result = compare(obj1->getType(), obj2->getType());
}

void CompareVisitor::map(PortAssign *obj1, PortAssign *obj2)
{
    map(static_cast<PPAssign *>(obj1), static_cast<PPAssign *>(obj2));
    if (_result != 0)
        return;

    _result = compare(obj1->getType(), obj2->getType());
    if (_result != 0)
        return;

    _result = compare(obj1->getPartialBind(), obj2->getPartialBind());
}

void CompareVisitor::map(Port *obj1, Port * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(PPAssign *obj1, PPAssign *obj2)
{
    map(static_cast<ReferencedAssign *>(obj1), static_cast<ReferencedAssign *>(obj2));
    if (_result != 0)
        return;

    _result = _compareActualValue(obj1->getDirection(), obj2->getDirection());
    if (_result != 0)
        return;

    _result = compare(obj1->getValue(), obj2->getValue());
}

void CompareVisitor::map(Procedure *obj1, Procedure * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);

    //    _result = _compareActualValue( obj1->isStandard(), obj2->isStandard() );
    //    if (_result != 0) return;

    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Range *obj1, Range *obj2)
{
    _result = _compareActualValue(obj1->getDirection(), obj2->getDirection());
    if (_result != 0)
        return;

    _result = compare(obj1->getLeftBound(), obj2->getLeftBound());
    if (_result != 0)
        return;

    _result = compare(obj1->getType(), obj2->getType());
}

void CompareVisitor::map(Real *obj1, Real *obj2)
{
    map(static_cast<SimpleType *>(obj1), static_cast<SimpleType *>(obj2));
    if (_result != 0)
        return;

    _result = compare(obj1->getSpan(), obj2->getSpan());
}

void CompareVisitor::map(RealValue *obj1, RealValue *obj2) { _result = _compareConstValue(obj1, obj2); }

void CompareVisitor::map(Record *obj1, Record *obj2)
{
    map(static_cast<ScopedType *>(obj1), static_cast<ScopedType *>(obj2));
    if (_result != 0)
        return;

    _result = _compareActualValue(obj1->isPacked(), obj2->isPacked());
    if (_result != 0)
        return;
    _result = _compareActualValue(obj1->isUnion(), obj2->isUnion());
    if (_result != 0)
        return;

    _result = _compareList(obj1->fields, obj2->fields);
}

void CompareVisitor::map(RecordValue *obj1, RecordValue *obj2) { _result = _compareList(obj1->alts, obj2->alts); }

void CompareVisitor::map(RecordValueAlt *obj1, RecordValueAlt *obj2)
{
    _result = _compareActualValue(static_cast<std::string>(obj1->getName()), static_cast<std::string>(obj2->getName()));
    if (_result != 0)
        return;

    _result = compare(obj1->getValue(), obj2->getValue());
}

void CompareVisitor::map(Reference *obj1, Reference *obj2)
{
    map(static_cast<CompositeType *>(obj1), static_cast<CompositeType *>(obj2));
    if (_result != 0)
        return;

    _result = compare(obj1->getType(), obj2->getType());
}

void CompareVisitor::map(ReferencedAssign *obj1, ReferencedAssign *obj2)
{
    _result = _compareActualValue(static_cast<std::string>(obj1->getName()), static_cast<std::string>(obj2->getName()));
}

void CompareVisitor::map(Return *obj1, Return * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(ScopedType *obj1, ScopedType *obj2)
{
    _result = _compareActualValue(obj1->isConstexpr(), obj2->isConstexpr());
}

void CompareVisitor::map(Signal *obj1, Signal * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);

    //    _result = _compareActualValue( obj1->isStandard(), obj2->isStandard() );
    //    if (_result != 0) return;

    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Signed *obj1, Signed *obj2)
{
    map(static_cast<SimpleType *>(obj1), static_cast<SimpleType *>(obj2));
    if (_result != 0)
        return;

    _result = compare(obj1->getSpan(), obj2->getSpan());
}

void CompareVisitor::map(SimpleType *obj1, SimpleType *obj2)
{
    map(static_cast<Type *>(obj1), static_cast<Type *>(obj2));
    if (_result != 0)
        return;

    _result = _compareActualValue(obj1->isConstexpr(), obj2->isConstexpr());
}

void CompareVisitor::map(Slice *obj1, Slice *obj2)
{
    map(static_cast<PrefixedReference *>(obj1), static_cast<PrefixedReference *>(obj2));
    if (_result != 0)
        return;

    _result = compare(obj1->getSpan(), obj2->getSpan());
}

void CompareVisitor::map(State *obj1, State * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(String *obj1, String *obj2)
{
    map(static_cast<SimpleType *>(obj1), static_cast<SimpleType *>(obj2));
    if (_result != 0)
        return;

    _result = compare(obj1->getSpanInformation(), obj2->getSpanInformation());
}

void CompareVisitor::map(StateTable *obj1, StateTable * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);

    //    _result = _compareActualValue( obj1->isStandard(), obj2->isStandard() );
    //    if (_result != 0) return;

    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(SwitchAlt *obj1, SwitchAlt * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Switch *obj1, Switch * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(StringValue *obj1, StringValue *obj2) { _result = _compareConstValue(obj1, obj2); }

void CompareVisitor::map(Time *obj1, Time *obj2)
{
    map(static_cast<SimpleType *>(obj1), static_cast<SimpleType *>(obj2));
}

void CompareVisitor::map(TimeValue *obj1, TimeValue *obj2)
{
    _result = _compareActualValue(obj1->getUnit(), obj2->getUnit());
    if (_result != 0)
        return;

    _result = _compareConstValue(obj1, obj2);
}

void CompareVisitor::map(TPAssign *obj1, TPAssign *obj2)
{
    map(static_cast<ReferencedAssign *>(obj1), static_cast<ReferencedAssign *>(obj2));
    // Nothing to do.
}

void CompareVisitor::map(Type *obj1, Type *obj2)
{
    _result = _compareActualValue(obj1->getTypeVariant(), obj2->getTypeVariant());
}

void CompareVisitor::map(TypeDef *obj1, TypeDef * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);

    //    _result = _compareActualValue( obj1->isStandard(), obj2->isStandard() );
    //    _result = _compareActualValue( obj1->IsOpaque(), obj2->IsOpaque() );
    //    _result = _compareActualValue( obj1->IsExternal(), obj2->IsExternal() );
    //    _result = _compareActualValue( obj1->IsSemanticsSpecific(), obj2->IsSemanticsSpecific() );
    //    if (_result != 0) return;

    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(TypeReference *obj1, TypeReference *obj2)
{
    _result = _compareActualValue(static_cast<std::string>(obj1->getName()), static_cast<std::string>(obj2->getName()));
    if (_result != 0)
        return;

    _result = compare(obj1->getInstance(), obj2->getInstance());
    if (_result != 0)
        return;

    _result = _compareList(obj1->ranges, obj2->ranges);
    if (_result != 0)
        return;

    _result = _compareList(obj1->templateParameterAssigns, obj2->templateParameterAssigns);
}

void CompareVisitor::map(TypeTPAssign *obj1, TypeTPAssign *obj2)
{
    map(static_cast<TPAssign *>(obj1), static_cast<TPAssign *>(obj2));
    if (_result != 0)
        return;

    compare(obj1->getType(), obj2->getType());
}

void CompareVisitor::map(TypeTP *obj1, TypeTP * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Unsigned *obj1, Unsigned *obj2)
{
    map(static_cast<SimpleType *>(obj1), static_cast<SimpleType *>(obj2));
    if (_result != 0)
        return;

    _result = compare(obj1->getSpan(), obj2->getSpan());
}

void CompareVisitor::map(ValueStatement *obj1, ValueStatement * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(ValueTPAssign *obj1, ValueTPAssign *obj2)
{
    map(static_cast<TPAssign *>(obj1), static_cast<TPAssign *>(obj2));
    if (_result != 0)
        return;

    compare(obj1->getValue(), obj2->getValue());
}

void CompareVisitor::map(ValueTP *obj1, ValueTP * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(Variable *obj1, Variable * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);

    //    _result = _compareActualValue( obj1->isStandard(), obj2->isStandard() );
    //    if (_result != 0) return;

    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(View *obj1, View * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);

    //    _result = _compareActualValue( obj1->isStandard(), obj2->isStandard() );
    //    if (_result != 0) return;

    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(ViewReference *obj1, ViewReference *obj2)
{
    _result = _compareActualValue(static_cast<std::string>(obj1->getName()), static_cast<std::string>(obj2->getName()));
    if (_result != 0)
        return;

    _result = _compareActualValue(
        static_cast<std::string>(obj1->getDesignUnit().c_str()),
        static_cast<std::string>(obj2->getDesignUnit().c_str()));
    if (_result != 0)
        return;

    _result = compare(obj1->getInstance(), obj2->getInstance());
    if (_result != 0)
        return;

    _result = _compareList(obj1->templateParameterAssigns, obj2->templateParameterAssigns);
}

void CompareVisitor::map(Wait *obj1, Wait *obj2)
{
    _result = compare(obj1->getTime(), obj2->getTime());
    if (_result != 0)
        return;

    _result = compare(obj1->getCondition(), obj2->getCondition());
    if (_result != 0)
        return;

    _result = compare(obj1->getRepetitions(), obj2->getRepetitions());
    if (_result != 0)
        return;

    _result = _compareList(obj1->sensitivity, obj2->sensitivity);
    if (_result != 0)
        return;

    _result = _compareList(obj1->sensitivityPos, obj2->sensitivityPos);
    if (_result != 0)
        return;

    _result = _compareList(obj1->sensitivityNeg, obj2->sensitivityNeg);
    if (_result != 0)
        return;

    _result = _compareList(obj1->actions, obj2->actions);
    if (_result != 0)
        return;
}

void CompareVisitor::map(When *obj1, When *obj2)
{
    _result = compare(obj1->getDefault(), obj2->getDefault());
    if (_result != 0)
        return;

    _result = _compareActualValue(obj1->isLogicTernary(), obj2->isLogicTernary());
    if (_result != 0)
        return;

    _result = _compareList(obj1->alts, obj2->alts);
}

void CompareVisitor::map(WhenAlt *obj1, WhenAlt *obj2)
{
    _result = compare(obj1->getCondition(), obj2->getCondition());
    if (_result != 0)
        return;

    _result = compare(obj1->getValue(), obj2->getValue());
}

void CompareVisitor::map(While *obj1, While * /*obj2*/)
{
    messageError("Unexpected case", obj1, nullptr);
    //map( static_cast< Object* >(obj1), static_cast< Object* >(obj2) );
}

void CompareVisitor::map(With *obj1, With *obj2)
{
    _result = _compareActualValue(obj1->getCaseSemantics(), obj2->getCaseSemantics());
    if (_result != 0)
        return;

    _result = compare(obj1->getDefault(), obj2->getDefault());
    if (_result != 0)
        return;

    _result = compare(obj1->getCondition(), obj2->getCondition());
    if (_result != 0)
        return;

    _result = _compareList(obj1->alts, obj2->alts);
}

void CompareVisitor::map(WithAlt *obj1, WithAlt *obj2)
{
    _result = _compareList(obj1->conditions, obj2->conditions);
    if (_result != 0)
        return;

    _result = compare(obj1->getValue(), obj2->getValue());
}
} // namespace
int compare(Object *obj1, Object *obj2)
{
    if (obj1 == nullptr && obj2 != nullptr) {
        return -1;
    } else if (obj1 != nullptr && obj2 == nullptr) {
        return 1;
    } else if (obj1 == nullptr && obj2 == nullptr) {
        return 0;
    }

    initMap();
    CompareVisitor cv;
    cv.callMap(obj1, obj2);
    return cv.getResult();
}
} // namespace hif
