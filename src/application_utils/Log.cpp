/// @file Log.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/application_utils/application.hpp"
#include "hif/hifIOUtils.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/semantics/semantics.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#    pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#    pragma clang diagnostic ignored "-Wunknown-attributes"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#    pragma GCC diagnostic ignored "-Wattributes"
#endif

namespace hif
{
namespace application_utils
{

#ifndef NDEBUG
bool hifLogDebugIsActive = false;
#endif

namespace
{

struct UniqueInfos {
    UniqueInfos();
    UniqueInfos(const std::string &file_, unsigned int line_);
    ~UniqueInfos();
    UniqueInfos(const UniqueInfos &other);
    UniqueInfos &operator=(UniqueInfos other);
    void swap(UniqueInfos &other);
    bool operator<(const UniqueInfos &other) const;

    std::string file;
    unsigned int line;
    std::uint64_t counter;
};

UniqueInfos::UniqueInfos()
    : file()
    , line(0)
    , counter(0)
{
    // ntd
}

UniqueInfos::UniqueInfos(const std::string &file_, unsigned int line_)
    : file(file_)
    , line(line_)
    , counter(1)
{
    // ntd
}

UniqueInfos::~UniqueInfos()
{
    // ntd
}

UniqueInfos::UniqueInfos(const UniqueInfos &other)
    : file(other.file)
    , line(other.line)
    , counter(other.counter)
{
}

UniqueInfos &UniqueInfos::operator=(UniqueInfos other)
{
    swap(other);
    return *this;
}

void UniqueInfos::swap(UniqueInfos &other)
{
    std::swap(file, other.file);
    std::swap(line, other.line);
    std::swap(counter, other.counter);
}

bool UniqueInfos::operator<(const UniqueInfos &other) const
{
    if (line < other.line)
        return true;
    else if (line > other.line)
        return false;

    if (file < other.file)
        return true;
    else if (file > other.file)
        return false;

    return false;
}

typedef std::list<std::string> StringList;
typedef std::set<UniqueInfos> UniqueInfoSet;
typedef std::map<std::string, UniqueInfoSet> UniqueWarnings;

/// @name These two list identifies the stack of components currently using log
/// methods. An application may have an additional information about the currently
/// used component (otherwise, empty string). A component cannot exist outside
/// application. Thus, we expect that front elements of the two lists are always
/// related.
///
//@{
/// @brief The identifier of current application.
StringList _applicationNames;

/// @brief The identifier of current application component.
StringList _componentNames;
//@}

/// @brief This map identifies the unique warnings raised during the execution.
UniqueWarnings _uniqueWarnings;

/// @brief This visitor is intended for printing objects details, depending on
/// the type of object.
class ObjectDetailVisitor : public HifVisitor
{
public:
    ObjectDetailVisitor(std::ostream &outStream, hif::semantics::ILanguageSemantics *sem, const std::string &file);
    ~ObjectDetailVisitor();

    virtual int visitAggregate(Aggregate &o);
    virtual int visitAggregateAlt(AggregateAlt &o);
    virtual int visitAlias(Alias &o);
    virtual int visitArray(Array &o);
    virtual int visitAssign(Assign &o);
    virtual int visitSystem(System &o);
    virtual int visitBit(Bit &o);
    virtual int visitBitValue(BitValue &o);
    virtual int visitBitvector(Bitvector &o);
    virtual int visitBitvectorValue(BitvectorValue &o);
    virtual int visitBool(Bool &o);
    virtual int visitBoolValue(BoolValue &o);
    virtual int visitBreak(Break &o);
    virtual int visitCast(Cast &o);
    virtual int visitChar(Char &o);
    virtual int visitCharValue(CharValue &o);
    virtual int visitConst(Const &o);
    virtual int visitContents(Contents &o);
    virtual int visitContinue(Continue &o);
    virtual int visitDesignUnit(DesignUnit &o);
    virtual int visitEnum(Enum &o);
    virtual int visitEnumValue(EnumValue &o);
    virtual int visitEvent(Event &o);
    virtual int visitExpression(Expression &o);
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitField(Field &o);
    virtual int visitFieldReference(FieldReference &o);
    virtual int visitFile(File &o);
    virtual int visitFor(For &o);
    virtual int visitForGenerate(ForGenerate &o);
    virtual int visitFunction(Function &o);
    virtual int visitGlobalAction(GlobalAction &o);
    virtual int visitEntity(Entity &o);
    virtual int visitIdentifier(Identifier &o);
    virtual int visitIf(If &o);
    virtual int visitIfAlt(IfAlt &o);
    virtual int visitIfGenerate(IfGenerate &o);
    virtual int visitInstance(Instance &o);
    virtual int visitInt(Int &o);
    virtual int visitIntValue(IntValue &o);
    virtual int visitLibraryDef(LibraryDef &o);
    virtual int visitLibrary(Library &o);
    virtual int visitNull(Null &o);
    virtual int visitMember(Member &o);
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
    virtual int visitString(String &o);
    virtual int visitStateTable(StateTable &o);
    virtual int visitSwitchAlt(SwitchAlt &o);
    virtual int visitSwitch(Switch &o);
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
    virtual int visitWhen(When &o);
    virtual int visitWhenAlt(WhenAlt &o);
    virtual int visitWhile(While &o);
    virtual int visitWith(With &o);
    virtual int visitWithAlt(WithAlt &o);

    /// @brief Tells whether to use verblose printing.
    static bool isVerbose;

private:
    // Disabled.
    ObjectDetailVisitor(const ObjectDetailVisitor &);
    ObjectDetailVisitor &operator=(const ObjectDetailVisitor &);

    void _printObject(Object *o, bool forceSummary = false);
    void _printObjectDeclaration(Object *o, const std::string &refObject = std::string("Object"));
    void _printObjectType(TypedObject *o, const std::string &refObject = std::string("Object"));
    void _printType(Type *t, const std::string &refObject, bool printBase = true);
    template <typename T> void _printObjectBaseTypes(T *o, const std::string &refObject = std::string("Object"));
    void _printObjectParent(Object *o, const std::string &refObject = std::string("Object"));
    void _printObjectDataDeclaration(DataDeclaration *decl);

    std::ostream &_outStream;
    hif::semantics::ILanguageSemantics *_sem;
    // This is used to avoid infinite loop in case of messages raised from
    // getSemanticType() / getDeclaration()
    std::string _raisingFile;
};

bool ObjectDetailVisitor::isVerbose = false;

ObjectDetailVisitor::ObjectDetailVisitor(
    std::ostream &outStream,
    hif::semantics::ILanguageSemantics *sem,
    const std::string &file)
    : _outStream(outStream)
    , _sem(sem)
    , _raisingFile(file)
{
}

ObjectDetailVisitor::~ObjectDetailVisitor() {}

int ObjectDetailVisitor::visitAggregate(Aggregate &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitAggregateAlt(AggregateAlt &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitAlias(Alias &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectDataDeclaration(&o);
    return 0;
}

int ObjectDetailVisitor::visitArray(Array &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectBaseTypes(&o);
    return 0;
}

int ObjectDetailVisitor::visitAssign(Assign &o)
{
    _printObject(&o);
    _printObjectParent(&o);

    if (o.getLeftHandSide() != nullptr) {
        // avoid recursion
        _printObjectType(o.getLeftHandSide(), "Left hand side");
    }

    if (o.getRightHandSide() != nullptr) {
        // avoid recursion
        _printObjectType(o.getRightHandSide(), "Right hand side");
    }

    return 0;
}

int ObjectDetailVisitor::visitSystem(System &)
{
    //    _printObject( &o ); // TODO summarize informations
    return 0;
}

int ObjectDetailVisitor::visitBit(Bit &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitBitValue(BitValue &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitBitvector(Bitvector &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitBitvectorValue(BitvectorValue &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitBool(Bool &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitBoolValue(BoolValue &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitBreak(Break &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitCast(Cast &o)
{
    _printObject(&o);
    _printObjectParent(&o);

    if (o.getValue() != nullptr) {
        // avoid recursion
        _printObjectType(o.getValue(), "Cast Op");
    }

    return 0;
}

int ObjectDetailVisitor::visitChar(Char &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitCharValue(CharValue &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitConst(Const &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectDataDeclaration(&o);
    return 0;
}

int ObjectDetailVisitor::visitContents(Contents &o)
{
    _printObject(&o, true);
    //    _printObjectParent( &o );
    return 0;
}

int ObjectDetailVisitor::visitContinue(Continue &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitDesignUnit(DesignUnit &o)
{
    _printObject(&o, true);
    //    _printObjectParent( &o );
    return 0;
}

int ObjectDetailVisitor::visitEnum(Enum &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitEnumValue(EnumValue &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectDataDeclaration(&o);
    return 0;
}

int ObjectDetailVisitor::visitEvent(Event &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitExpression(Expression &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);

    if (o.getValue1() != nullptr) {
        // avoid recursion
        _printObjectType(o.getValue1(), "Expr Op1");
    }

    if (o.getValue2() != nullptr) {
        // avoid recursion
        _printObjectType(o.getValue2(), "Expr Op2");
    }

    return 0;
}

int ObjectDetailVisitor::visitFunctionCall(FunctionCall &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectDeclaration(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitField(Field &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectDataDeclaration(&o);
    return 0;
}

int ObjectDetailVisitor::visitFieldReference(FieldReference &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitFile(File &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectBaseTypes(&o);
    return 0;
}

int ObjectDetailVisitor::visitFor(For &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitForGenerate(ForGenerate &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitFunction(Function &o)
{
    _printObject(&o);
    //    _printObjectParent( &o );
    return 0;
}

int ObjectDetailVisitor::visitGlobalAction(GlobalAction &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitEntity(Entity &o)
{
    _printObject(&o);
    //    _printObjectParent( &o );
    return 0;
}

int ObjectDetailVisitor::visitIdentifier(Identifier &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectDeclaration(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitIf(If &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitIfAlt(IfAlt &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitIfGenerate(IfGenerate &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitInstance(Instance &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitInt(Int &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitIntValue(IntValue &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitLibraryDef(LibraryDef &o)
{
    _printObject(&o, true);
    //    _printObjectParent( &o );
    return 0;
}

int ObjectDetailVisitor::visitLibrary(Library &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitNull(Null &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitMember(Member &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    _printObjectType(o.getPrefix(), "Prefix");
    return 0;
}

int ObjectDetailVisitor::visitTransition(Transition &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitParameterAssign(ParameterAssign &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitParameter(Parameter &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectDataDeclaration(&o);
    return 0;
}

int ObjectDetailVisitor::visitProcedureCall(ProcedureCall &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectDeclaration(&o);
    return 0;
}

int ObjectDetailVisitor::visitPointer(Pointer &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectBaseTypes(&o);
    return 0;
}

int ObjectDetailVisitor::visitPortAssign(PortAssign &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);

    if (o.getValue() != nullptr) {
        // avoid recursion
        _printObjectType(o.getValue(), "Portassign Value");
    }

    return 0;
}

int ObjectDetailVisitor::visitPort(Port &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectDataDeclaration(&o);
    return 0;
}

int ObjectDetailVisitor::visitProcedure(Procedure &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitRange(Range &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    //    _printObjectType( &o );
    return 0;
}

int ObjectDetailVisitor::visitReal(Real &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitRealValue(RealValue &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitRecord(Record &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectBaseTypes(&o);
    return 0;
}

int ObjectDetailVisitor::visitRecordValue(RecordValue &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitRecordValueAlt(RecordValueAlt &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitReference(Reference &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectBaseTypes(&o);
    return 0;
}

int ObjectDetailVisitor::visitReturn(Return &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitSignal(Signal &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectDataDeclaration(&o);
    return 0;
}

int ObjectDetailVisitor::visitSigned(Signed &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitSlice(Slice &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    _printObjectType(o.getPrefix(), "Prefix");
    return 0;
}

int ObjectDetailVisitor::visitState(State &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitString(String &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitStateTable(StateTable &o)
{
    _printObject(&o, true);
    //    _printObjectParent( &o );
    return 0;
}

int ObjectDetailVisitor::visitSwitchAlt(SwitchAlt &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitSwitch(Switch &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitStringValue(StringValue &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitTime(Time &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitTimeValue(TimeValue &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitTypeDef(TypeDef &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitTypeReference(TypeReference &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitTypeTP(TypeTP &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitUnsigned(Unsigned &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitValueStatement(ValueStatement &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitValueTP(ValueTP &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectDataDeclaration(&o);
    return 0;
}

int ObjectDetailVisitor::visitVariable(Variable &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectDataDeclaration(&o);
    return 0;
}

int ObjectDetailVisitor::visitView(View &o)
{
    _printObject(&o);
    //    _printObjectParent( &o );
    return 0;
}

int ObjectDetailVisitor::visitViewReference(ViewReference &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitWait(Wait &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitWhen(When &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    unsigned int i = 0;
    for (BList<WhenAlt>::iterator it = o.alts.begin(); it != o.alts.end(); ++it) {
        std::stringstream ss;
        ss << "Alt #" << i;
        std::string str = ss.str();
        _printObjectType((*it)->getValue(), str);
        ++i;
    }
    _printObjectType(o.getDefault(), "When default");

    return 0;
}

int ObjectDetailVisitor::visitWhenAlt(WhenAlt &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitWhile(While &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

int ObjectDetailVisitor::visitWith(With &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    _printObjectType(&o);
    return 0;
}

int ObjectDetailVisitor::visitWithAlt(WithAlt &o)
{
    _printObject(&o);
    _printObjectParent(&o);
    return 0;
}

void ObjectDetailVisitor::_printObject(Object *o, bool forceSummary)
{
    _outStream << "\nObject: " << hif::classIDToString(o->getClassId()) << std::endl;

    hif::PrintHifOptions opt;
    opt.printSummary = forceSummary;
    hif::writeFile(_outStream, o, false, opt);
    _outStream << std::endl;
}

bool _isManagedIdentifier(Object *o)
{
    if (dynamic_cast<Identifier *>(o) == nullptr)
        return true;

    if (o->getParent() == nullptr)
        return false;

    if (dynamic_cast<Port *>(o->getParent()) != nullptr || dynamic_cast<Variable *>(o->getParent()) != nullptr ||
        dynamic_cast<Const *>(o->getParent()) != nullptr || dynamic_cast<Signal *>(o->getParent()) != nullptr ||
        dynamic_cast<ValueTP *>(o->getParent()) != nullptr)
        return true;

    return false;
}

void ObjectDetailVisitor::_printObjectDeclaration(Object *o, const std::string &refObject)
{
    if (!_isManagedIdentifier(o)) {
        _outStream << "Declaration will not be calculated for this kind of object." << std::endl;
        return;
    }

    hif::features::ISymbol *sym = dynamic_cast<hif::features::ISymbol *>(o);
    if (sym == nullptr) {
        _outStream << "Object is not a symbol, thus it cannot have a declaration." << std::endl;
        return;
    }

    if (_sem == nullptr) {
        _outStream << "Unspecified semantics. Declaration cannot be calculated." << std::endl;
        return;
    }

    std::string obj = (refObject != "Object") ? refObject : hif::classIDToString(o->getClassId());

    _outStream << "\n\n------------------------------------------------------------\n"
               << obj << " declaration:" << std::endl;
    hif::semantics::DeclarationOptions dopt;
    dopt.dontSearch = true;
    Declaration *t  = hif::semantics::getDeclaration(o, _sem, dopt);
    if (t == nullptr) {
        _outStream << " nullptr " << std::endl;
        return;
    }
    hif::writeFile(_outStream, t, false);
    _outStream << std::endl;
}

void ObjectDetailVisitor::_printObjectType(TypedObject *o, const std::string &refObject)
{
    if (o == nullptr)
        return;
    Type *t = o->getSemanticType();
    _printType(t, refObject);
}

void ObjectDetailVisitor::_printType(Type *t, const std::string &refObject, bool printBase)
{
    _outStream << "\n\n------------------------------------------------------------\n"
               << refObject << " type:" << std::endl;
    if (t == nullptr) {
        _outStream << " nullptr " << std::endl;
        return;
    }
    hif::writeFile(_outStream, t, false);
    _outStream << std::endl;

    if (_sem == nullptr) {
        _outStream << "Unspecified semantics. Base type cannot be calculated." << std::endl;
        return;
    }

    if (printBase) {
        Type *baseT = hif::semantics::getBaseType(t, false, _sem);
        assert(baseT != nullptr);

        if (hif::equals(t, baseT))
            return;

        _outStream << "\n\n------------------------------------------------------------\n"
                   << refObject << " base type:" << std::endl;
        hif::writeFile(_outStream, baseT, false);
        _outStream << std::endl;
    }
}

template <typename T> void ObjectDetailVisitor::_printObjectBaseTypes(T *o, const std::string &refObject)
{
    if (_sem == nullptr) {
        _outStream << "Unspecified semantics. Type cannot be calculated." << std::endl;
        return;
    }

    std::string obj = (refObject != "Object") ? refObject : hif::classIDToString(o->getClassId());

    _outStream << "\n\n------------------------------------------------------------\n"
               << obj << " base type:" << std::endl;
    Type *t1 = o->getBaseType(false);
    if (t1 == nullptr) {
        _outStream << " nullptr " << std::endl;
        return;
    }
    hif::writeFile(_outStream, t1, false);
    _outStream << std::endl;

    _outStream << "\n\n------------------------------------------------------------\n"
               << obj << " base opaque type:" << std::endl;
    Type *t2 = o->getBaseType(true);
    if (t2 == nullptr) {
        _outStream << " nullptr " << std::endl;
        return;
    }
    hif::writeFile(_outStream, t2, false);
    _outStream << std::endl;
}

void ObjectDetailVisitor::_printObjectParent(Object *o, const std::string & /*refObject*/)
{
    Object *p = o->getParent();
    if (p == nullptr) {
        _outStream << "Parent is nullptr " << std::endl;
        return;
    }

    CompositeType *ct     = dynamic_cast<CompositeType *>(p);
    Record *rec           = dynamic_cast<Record *>(p);
    bool isBaseComp = (ct != nullptr && (ct->getBaseType(true) == o || ct->getBaseType(false) == o));
    bool isBaseRec  = (rec != nullptr && (rec->getBaseType(true) == o || rec->getBaseType(false) == o));
    if (isBaseComp || isBaseRec) {
        _outStream << "Object is a base type" << std::endl;
        return;
    }

    hif::PrintHifOptions opt;
    opt.printSummary         = true;
    unsigned int parentDepth = 1;
    hif::writeParents(parentDepth, _outStream, o, opt);
}

void ObjectDetailVisitor::_printObjectDataDeclaration(DataDeclaration *decl)
{
    bool printBase = (dynamic_cast<EnumValue *>(decl) == nullptr);
    _printType(decl->getType(), "DataDeclaration", printBase);
    _printObjectType(decl->getValue(), "Initial value");
}

// /////////////////////////////////////////////////////////////////////////////
// Other functions
// /////////////////////////////////////////////////////////////////////////////

/// @brief The log levels of the messages.
/// * INFO indicate to print a message specifying the application name (if any),
/// the component name (if any) and the current time.
/// * WARNING indicate to print also the object characteristics, specifying
/// the name and line number of both source file and code file where the
/// message is raised.
/// * ERROR indicate to print all the part of warning, but includes also
/// peculiar characteristics depending on the type of the object, and causing
/// an assertion and an exit-error.
/// * DEBUG is similar to ERROR, but just printing the information without
/// causing assert or exit.
/// * ASSERT is similar to ERROR, but allow to pass a boolean condition that
/// is verified, eventually causing an ERROR to be raised.
///
enum LogLevel : unsigned char {
    INFO,
    WARNING,
    ERROR,
    DEBUG,
    ASSERT,
};

/// @brief Return a string corresponding to the passed log level.
std::string _getLogLevel(LogLevel logLevel)
{
    switch (logLevel) {
    case ERROR:
        return "ERROR";
    case WARNING:
        return "WARNING";
    case INFO:
        return "INFO";
    case DEBUG:
        return "DEBUG";
    case ASSERT:
        return "ASSERT";
    default:
        return "";
    }
}

/// @brief Return the standard output stream corresponding to the passed log level.
std::ostream &_getOutputStream(LogLevel logLevel)
{
    switch (logLevel) {
    case INFO:
        return std::cout;
    case WARNING:
    case DEBUG:
    case ASSERT:
    case ERROR:
        return std::clog;
    default:
        return std::cerr;
    }
}

void _printMessage(std::ostream &outStream, std::string &logLevel, const std::string &message)
{
    if (!_applicationNames.empty() && !_applicationNames.front().empty()) {
        outStream << "[" << _applicationNames.front() << "] ";

        assert(!_componentNames.empty());
        if (!_componentNames.front().empty()) {
            outStream << "[" << _componentNames.front() << "] ";
        }
    }

    outStream << hif_getCurrentTimeAsString() << " - " << logLevel << ": " << message << std::endl;
}

void _printRaisePoint(
    std::ostream &outStream,
    const std::string &file,
    unsigned int line,
    bool callFromPrintUniqueWarnings = false,
    std::uint64_t counter             = 0)
{
    // We are interested to the collected raise points, not the current one.
    if (callFromPrintUniqueWarnings)
        return;

    outStream << "- Raised by " << file << ":" << line;
    if (counter != 0)
        outStream << " [" << counter << " times]";
    outStream << std::endl;
}

void _printDetails(
    std::ostream &outStream,
    hif::Object *involvedObject,
    hif::semantics::ILanguageSemantics *sem,
    const std::string &file,
    unsigned int line,
    bool callFromPrintUniqueWarnings = false)
{
    _printRaisePoint(outStream, file, line, callFromPrintUniqueWarnings);

    // A nullptr object can be passed if we are interested to the message only.
    if (involvedObject == nullptr)
        return;

    if (!involvedObject->getSourceFileName().empty()) {
        outStream << "- Source file info - " << involvedObject->getSourceFileName() << ": line "
                  << involvedObject->getSourceLineNumber() << ", column " << involvedObject->getSourceColumnNumber()
                  << std::endl;
    } else {
        outStream << "- No source file info available - " << std::endl;
    }

    bool isInTree = hif::isInTree(involvedObject);
    outStream << "- Object in tree: " << (isInTree ? "true" : "false") << std::endl;

    auto ttmp = dynamic_cast<const Type *>(involvedObject);
    if (ttmp != nullptr) {
        bool isSemType = hif::semantics::isSemanticsType(ttmp);
        outStream << "- Object is semantic type: " << (isSemType ? "true" : "false") << std::endl;
    }

    DesignUnit *duP = hif::getNearestParent<DesignUnit>(involvedObject);
    LibraryDef *ldP = hif::getNearestParent<LibraryDef>(involvedObject);
    SubProgram *suP = hif::getNearestParent<SubProgram>(involvedObject);
    StateTable *stP = hif::getNearestParent<StateTable>(involvedObject);
    if (suP != nullptr) {
        outStream << "  -- in SubProgram: " << suP->getName() << std::endl;
    } else if (stP != nullptr) {
        outStream << "  -- in Process: " << stP->getName() << std::endl;
    }

    if (duP != nullptr) {
        outStream << "  -- in Design Unit: " << duP->getName() << std::endl;
    }
    if (ldP != nullptr) {
        outStream << "  -- in LibraryDef: " << ldP->getName() << std::endl;
    }

    outStream << "- Using semantics: " << (sem != nullptr ? sem->getName() : "null") << std::endl;

    if (ObjectDetailVisitor::isVerbose) {
        // Print additional info depending on the object type.
        outStream << "\n________________________________________________________________________________" << std::endl;
        ObjectDetailVisitor vis(outStream, sem, file);
        involvedObject->acceptVisitor(vis);
        outStream << "\n________________________________________________________________________________" << std::endl;
    }
}

void _printReport(std::ostream &outStream)
{
    outStream << "\nPlease report issues to enrico.fraccaroli@univr.it" << std::endl;
}

void _message(
    const std::string &file,
    unsigned int line,
    LogLevel logLevel,
    const std::string &message,
    hif::Object *involvedObject             = nullptr,
    hif::semantics::ILanguageSemantics *sem = nullptr,
    bool assertCondition                    = true)
{
    std::ostream &outStream = _getOutputStream(logLevel);
    std::string strLogLevel = _getLogLevel(logLevel);

    switch (logLevel) {
    case INFO:
        _printMessage(outStream, strLogLevel, message);
        break;
    case WARNING:
        _printMessage(outStream, strLogLevel, message);
        _printDetails(outStream, involvedObject, sem, file, line, !assertCondition);
        outStream << std::endl;
        break;
    case DEBUG:
        strLogLevel = _getLogLevel(DEBUG);
        _printMessage(outStream, strLogLevel, message);
        _printDetails(outStream, involvedObject, sem, file, line);
        outStream << std::endl;
        break;
    case ASSERT:
        if (assertCondition)
            break;
        // Fall into ERROR.
        [[fallthrough]];
    case ERROR:
        _printMessage(outStream, strLogLevel, message);
        _printDetails(outStream, involvedObject, sem, file, line);
        _printReport(outStream);
        outStream << std::endl;
        assert(false);
        exit(1);
    default:
        assert(false);
        exit(1);
    }
}

void _setApplicationName(const std::string &name) { _applicationNames.push_front(name); }

void _setComponentName(const std::string &name)
{
    assert(!_applicationNames.empty());
    assert(!_applicationNames.front().empty());
    _componentNames.push_front(name);
}

} // namespace

std::string getApplicationName()
{
    if (_applicationNames.empty())
        return "no_application_name";
    return _applicationNames.front();
}

std::string getComponentName()
{
    if (_componentNames.empty())
        return "no_component_name";
    return _componentNames.front();
}

void initializeLogHeader(const std::string &appName, const std::string &compName)
{
    Application::getInstance();
    _setApplicationName(appName);
    _setComponentName(compName);
}

void restoreLogHeader()
{
    if (!_applicationNames.empty())
        _applicationNames.pop_front();
    if (!_componentNames.empty())
        _componentNames.pop_front();
}

void _hif_internal_messageInfo(const std::string &file, unsigned int line, const std::string &message)
{
    _message(file, line, INFO, message);
}

void _hif_internal_messageWarning(
    const std::string &file,
    unsigned int line,
    const std::string &message,
    hif::Object *involvedObject,
    hif::semantics::ILanguageSemantics *sem)
{
    _message(file, line, WARNING, message, involvedObject, sem);
}

void _hif_internal_raiseUniqueWarning(const std::string &file, unsigned int line, const std::string &message)
{
    UniqueInfos infos(file, line);
    UniqueInfoSet::iterator it = _uniqueWarnings[message].find(infos);
    if (it == _uniqueWarnings[message].end()) {
        _uniqueWarnings[message].insert(infos);
    } else {
        // trust in following line
        UniqueInfos &tmp = *const_cast<UniqueInfos *>(&*it);
        ++tmp.counter;
    }
}

void _hif_internal_printUniqueWarnings(const std::string &file, unsigned int line, const std::string &message)
{
    if (_uniqueWarnings.empty())
        return;

    std::string msg = (!message.empty()) ? message : "One or more warning have been raised:";
    msg += "\n";
    _message(file, line, INFO, msg, nullptr, nullptr);

    for (UniqueWarnings::iterator it(_uniqueWarnings.begin()); it != _uniqueWarnings.end(); ++it) {
        msg = it->first;
        msg += "\n";

        if (!it->second.empty()) {
            std::stringstream ss;
            for (UniqueInfoSet::iterator rIt(it->second.begin()); rIt != it->second.end(); ++rIt) {
                _printRaisePoint(ss, rIt->file, rIt->line, false, rIt->counter);
            }
            msg += ss.str();
            if (msg[msg.size() - 1] == '\n') {
                msg = msg.substr(0, msg.size() - 1);
            }
        }

        // Unappropriate use of assertCondition to act as guard to avoid print of raise point.
        _message(file, line, WARNING, msg, nullptr, nullptr, false);
    }

    _uniqueWarnings.clear();
}

void _hif_internal_messageError(
    const std::string &file,
    unsigned int line,
    const std::string &message,
    hif::Object *involvedObject,
    hif::semantics::ILanguageSemantics *sem)
{
    _message(file, line, ERROR, message, involvedObject, sem);
    exit(EXIT_FAILURE);
}

void _hif_internal_messageDebug(
    const std::string &file,
    unsigned int line,
    const std::string &message,
    hif::Object *involvedObject,
    hif::semantics::ILanguageSemantics *sem,
    bool dontPrintCondition)
{
    if (dontPrintCondition)
        return;
    _message(file, line, DEBUG, message, involvedObject, sem);
}

void _hif_internal_messageAssert(
    const std::string &file,
    unsigned int line,
    const std::string &message,
    hif::Object *involvedObject,
    hif::semantics::ILanguageSemantics *sem)
{
    _message(file, line, ASSERT, message, involvedObject, sem, false);
    exit(EXIT_FAILURE);
}

void _hif_internal_messageWarningList(
    const std::string &file,
    unsigned int line,
    bool condition,
    const std::string &message,
    WarningList &objList)
{
    if (!condition)
        return;
    if (objList.empty())
        return;

    std::ostream &outStream = _getOutputStream(WARNING);
    std::string strLogLevel = _getLogLevel(WARNING);
    _printMessage(outStream, strLogLevel, message);
    //outStream << " - Warning occurrences: " << objList.size() << std::endl;
    _printRaisePoint(outStream, file, line, false);

    for (WarningList::iterator i = objList.begin(); i != objList.end(); ++i) {
        std::string s = (*i)->getSourceInfoString();
        if (s.empty()) {
            std::clog << "Unknown design line.\n";
#ifndef NDEBUG
            hif::PrintHifOptions opt;
            opt.printSummary = true;
            hif::writeFile(std::clog, *i, false, opt);
            std::clog << std::endl;
#endif
            continue;
        }
        std::string n = hif::objectGetName(*i);
        if (!n.empty() && n != NameTable::getInstance()->none())
            std::clog << n << " - ";
        std::clog << s << std::endl;
    }
    outStream << std::endl;
}

void _hif_internal_messageWarningList(
    const std::string &file,
    unsigned int line,
    bool condition,
    const std::string &message,
    WarningSet &objSet)
{
    if (!condition)
        return;
    if (objSet.empty())
        return;

    WarningList l;
    l.insert(l.end(), objSet.begin(), objSet.end());

    _hif_internal_messageWarningList(file, line, condition, message, l);
}

void _hif_internal_messageWarningList(
    const std::string &file,
    unsigned int line,
    bool condition,
    const std::string &message,
    WarningStringSet &objList)
{
    if (!condition)
        return;
    if (objList.empty())
        return;

    std::ostream &outStream = _getOutputStream(WARNING);
    std::string strLogLevel = _getLogLevel(WARNING);
    _printMessage(outStream, strLogLevel, message);
    //outStream << " - Warning occurrences: " << objList.size() << std::endl;
    _printRaisePoint(outStream, file, line, false);

    for (WarningStringSet::iterator i = objList.begin(); i != objList.end(); ++i) {
        std::clog << "  - " << *i << std::endl;
    }
    outStream << std::endl;
}

void _hif_internal_messageWarningList(
    const std::string &file,
    unsigned int line,
    bool condition,
    const std::string &message,
    WarningInfoList &objList)
{
    if (!condition)
        return;
    if (objList.empty())
        return;

    std::ostream &outStream = _getOutputStream(WARNING);
    std::string strLogLevel = _getLogLevel(WARNING);
    _printMessage(outStream, strLogLevel, message);
    //outStream << " - Warning occurrences: " << objList.size() << std::endl;
    _printRaisePoint(outStream, file, line, false);

    for (WarningInfoList::iterator i = objList.begin(); i != objList.end(); ++i) {
        WarningInfo &info = *i;
        std::string s     = info.codeInfo.getSourceInfoString();
        if (s.empty()) {
            std::clog << "Unknown design line.\n";
#ifndef NDEBUG
            std::clog << info.description << std::endl;
#endif
            continue;
        }
        std::string n = info.name;
        if (!n.empty() && n != NameTable::getInstance()->none())
            std::clog << n << " - ";
        std::clog << s << std::endl;
    }
    outStream << std::endl;
}

void _hif_internal_messageWarningList(
    const std::string &file,
    unsigned int line,
    bool condition,
    const std::string &message,
    WarningInfoSet &objSet)
{
    if (!condition)
        return;
    if (objSet.empty())
        return;

    WarningInfoList l;
    l.insert(l.end(), objSet.begin(), objSet.end());

    _hif_internal_messageWarningList(file, line, condition, message, l);
}

void setVerboseLog(bool isVerbose) { ObjectDetailVisitor::isVerbose = isVerbose; }

bool isVerboseLog() { return ObjectDetailVisitor::isVerbose; }

// ///////////////////////////////////////////////////////////////////
// WarningInfo
// ///////////////////////////////////////////////////////////////////
WarningInfo::WarningInfo()
    : name()
    , codeInfo()
    , description()
{
    // ntd
}

WarningInfo::WarningInfo(Object *o)
    : name(hif::objectGetName(o))
    , codeInfo(o->getCodeInfo())
    , description()
{
    std::stringstream ss;
    hif::PrintHifOptions opt;
    opt.printSummary = true;
    hif::writeFile(ss, o, false, opt);
    description = ss.str();
}

WarningInfo::~WarningInfo()
{
    // ntd
}

WarningInfo::WarningInfo(const WarningInfo &o)
    : name(o.name)
    , codeInfo(o.codeInfo)
    , description(o.description)
{
    // ntd
}

WarningInfo &WarningInfo::operator=(WarningInfo o)
{
    swap(o);
    return *this;
}

void WarningInfo::swap(WarningInfo &o) noexcept
{
    std::swap(name, o.name);
    std::swap(codeInfo, o.codeInfo);
    std::swap(description, o.description);
}

bool WarningInfo::operator<(const WarningInfo &o) const
{
    if (codeInfo < o.codeInfo)
        return true;
    if (o.codeInfo < codeInfo)
        return false;
    if (description < o.description)
        return true;
    if (o.description < description)
        return false;
    return false;
}

} // namespace application_utils
} // namespace hif
