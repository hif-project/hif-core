/// @file PrintHifVisitor.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <ostream>

#include "hif/hif.hpp"
#include "hif/hifPrinter.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

namespace hif
{

namespace /*anon*/
{
/// PrintVisitor
///
/// @brief This class prints the HIF object subtree into an output stream.
///
/// Design Patterns:
/// - Visitor
///
class PrintHifVisitor : public GuideVisitor
{
public:
    /// @name Utility typedefs
    /// @{

    typedef hif::backends::IndentedStream IndentedStream;
    typedef std::list<std::string> StringList;
    typedef std::map<Object *, std::string> ObjectStringMap;
    typedef std::set<Object *> ObjectSet;
    typedef std::set<BList<Object> *> BListSet;

    /// @}

    /// @brief Support structure for printing.
    struct Data {
        Data();
        ~Data();
        Data(const Data &other);
        Data &operator=(Data other);
        void swap(Data &other);

        std::string streamRestore;
        bool mustPrintParentField;
        bool hasPrintedParentField;
        bool printParentField;
        bool printClassName;
        bool printNameAtEndOfObject;
        bool printListIndex;
        bool mustPrintListIndex;
        IndentedStream::Size indentation;
        StringList children;
    };

    typedef std::vector<Data> DataStack;

    /// @brief The object specific print options.
    struct PushOptions {
        PushOptions();
        ~PushOptions();
        PushOptions(const PushOptions &other);
        PushOptions &operator=(PushOptions other);
        void swap(PushOptions &other);

        /// @brief Print field into which current object is situated. Default true.
        bool printParentField;
        /// @brief Print object class name. Default true.
        bool printClassName;
        /// @brief Print the object class name (and eventual object name) at the
        /// end of its printing.
        bool printNameAtEndOfObject;
        /// @brief Print indexes of object childen situated inside BLists.
        bool printListIndex;
    };
    /// @brief Constructor.
    PrintHifVisitor(const Object *root, const PrintHifOptions &opt);
    /// @brief Destructor.
    virtual ~PrintHifVisitor();

    /// @brief Print the result on given stream.
    void print(std::ostream &o) const;

    /// @name Hif Objects visitors
    /// @{

    virtual int visitAggregate(Aggregate &o);
    virtual int visitAggregateAlt(AggregateAlt &o);
    virtual int visitAlias(Alias &o);
    virtual int visitArray(Array &o);
    virtual int visitAssign(Assign &o);
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
    virtual int visitEntity(Entity &o);
    virtual int visitEnum(Enum &o);
    virtual int visitEnumValue(EnumValue &o);
    virtual int visitEvent(Event &o);
    virtual int visitExpression(Expression &o);
    virtual int visitField(Field &o);
    virtual int visitFieldReference(FieldReference &o);
    virtual int visitFile(File &o);
    virtual int visitFor(For &o);
    virtual int visitForGenerate(ForGenerate &o);
    virtual int visitFunction(Function &o);
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitGlobalAction(GlobalAction &o);
    virtual int visitIdentifier(Identifier &o);
    virtual int visitIfAlt(IfAlt &o);
    virtual int visitIf(If &o);
    virtual int visitIfGenerate(IfGenerate &o);
    virtual int visitInt(Int &o);
    virtual int visitIntValue(IntValue &o);
    virtual int visitInstance(Instance &o);
    virtual int visitLibraryDef(LibraryDef &o);
    virtual int visitLibrary(Library &o);
    virtual int visitMember(Member &o);
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
    virtual int visitString(String &o);
    virtual int visitStateTable(StateTable &o);
    virtual int visitSystem(System &o);
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

    /// @}

    /// @brief Override of visit of BLists.
    virtual int visitList(BList<Object> &l);

    /// @name Wrappers for enum printing
    /// @{

    void printHifEnum(
        const std::string &eVal,
        const std::string &before = " [",
        const std::string &after  = "]",
        const bool manageEmpty    = false);

    static std::string stringTypeVariant(const Type::TypeVariant t);

    /// @}

    /// @name Parents visits
    /// @{

    int visitAction(Action *o);
    int visitAlt(Alt *o);
    int visitBaseContents(BaseContents *o);
    int visitCompositeType(CompositeType *o);
    int visitConstValue(ConstValue *o);
    int visitDataDeclaration(DataDeclaration *o);
    int visitDeclaration(Declaration *o);
    int visitGenerate(Generate *o);
    int visitPPAssign(PPAssign *o);
    int visitPrefixedReference(PrefixedReference *o);
    int visitReferencedAssign(ReferencedAssign *o);
    int visitReferencedType(ReferencedType *o);
    int visitScope(Scope *o);
    int visitScopedType(ScopedType *o);
    int visitSimpleType(SimpleType *o);
    int visitSubProgram(SubProgram *o);
    int visitTPAssign(TPAssign *o);
    int visitType(Type *o);
    int visitTypeDeclaration(TypeDeclaration *o);
    int visitTypedObject(TypedObject *o);
    int visitValue(Value *o);

    /// @}

private:
    /// @brief The support stringstream.
    std::stringstream _stream;

    /// @brief Output stream of HIF description.
    IndentedStream _indentedStream;

    /// @brief Output stream of HIF description.
    std::ostream &_out;

    /// @brief The data stack used to print.
    DataStack _stack;

    /// @brief Contains a map from objects pointers to custom printing string.
    ObjectStringMap _customPrintMap;

    /// @brief Contains list of objects that must be printed in summary in addition
    /// of print summary rules.
    ObjectSet _objsMustPrintInBrief;

    /// @brief Contains list of BLists that must be printed in summary in addition
    /// of print summary rules.
    BListSet _listsMustPrintInBrief;

    /// @brief The root object
    const Object *const _root;

    /// @brief The printer options.
    const PrintHifOptions _opt;

    /// @brief Convert given string in upper case.
    std::string _toUpperCase(const std::string &s);

    /// @brief Push current object data w.r.t. given options.
    void _push(const PushOptions &pushOpt = PushOptions());
    /// @brief Push BList data copying some options from last added object.
    void _pushBList();
    /// @brief Pop back object updating indentation.
    void _pop();

    /// @brief Returns true if object printing should be skipped due to summary.
    bool _skipIfSummary(Object *o);
    /// @brief Performs initial printings of parent field.
    void _printParentFieldBegin(Object *o);
    /// @brief Performs final printing of parent field.
    void _printParentFieldEnd(Object *o);
    /// @brief Performs initial printings of current object.
    bool _printObjectInit(Object *o);
    /// @brief Performs final printings of current object.
    void _printObjectEnd(Object *o);
    /// @brief Performs printing of object childen.
    void _printChildenStrings();
    /// @brief Return true if childen string must be printed in multiple lines.
    bool _checkMultiline();

    /// @brief Utility function to print flag.
    void _printFlag(const bool value, const std::string &name, const bool printAnyway = false);
    /// @brief Utility function to print string.
    void _printString(const std::string &value, const std::string &name, const bool printAnyway = false);
    /// @brief Utility function to print integer.
    void _printInt(long long value, const std::string &name);
    /// @brief Utility function to print Hif name.
    void _printName(const std::string &value, const std::string &name, const bool printAnyway = false);

    /// @name Custom printing methods
    /// @{

    void _addObjectMustPrintInBrief(Object *o);
    void _addBListMustPrintInBrief(BList<Object> *l);
    void _addCustomPrint(Object *o, const std::string &customString);

    /// @}

    // K: disabled.
    PrintHifVisitor(const PrintHifVisitor &);
    PrintHifVisitor &operator=(const PrintHifVisitor &);
};

PrintHifVisitor::PrintHifVisitor(const Object *root, const PrintHifOptions &opt)
    : _stream()
    , _indentedStream(_stream.rdbuf())
    , _out(_indentedStream)
    , _stack()
    , _customPrintMap()
    , _objsMustPrintInBrief()
    , _listsMustPrintInBrief()
    , _root(root)
    , _opt(opt)
{
    _stack.push_back(Data());

    // Assures maximum precision for printing numbers.
    _out << std::setprecision(std::numeric_limits<double>::digits10 + 1);
#if (defined _MSC_VER) && (_MSC_VER < 1900)
    // Forcing exponent to be printed with two digits, as in Linux
    _set_output_format(_TWO_DIGIT_EXPONENT);
#endif

    _indentedStream.setIndentationString("  ");
    _indentedStream.setColumnWidth(0);
    _indentedStream.setWrappingChars(" \t()[]{}");
}

PrintHifVisitor::~PrintHifVisitor()
{
    // ntd
}

void PrintHifVisitor::print(std::ostream &o) const
{
    if (_stack.empty())
        return;
    assert(_stack.size() == 1);

    const StringList &l = _stack.back().children;
    if (l.empty())
        return;

    assert(l.size() == 1);
    for (StringList::const_iterator i = l.begin(); i != l.end(); ++i) {
        o << *i << std::endl;
    }
}

// ///////////////////////////////////////////////////////////////////
// Data
// ///////////////////////////////////////////////////////////////////

PrintHifVisitor::Data::Data()
    : streamRestore()
    , mustPrintParentField(false)
    , hasPrintedParentField(false)
    , printParentField(false)
    , printClassName(false)
    , printNameAtEndOfObject(false)
    , printListIndex(false)
    , mustPrintListIndex(false)
    , indentation(0)
    , children()
{
    // ntd
}
PrintHifVisitor::Data::~Data()
{
    // ntd
}

PrintHifVisitor::Data::Data(const PrintHifVisitor::Data &other)
    : streamRestore(other.streamRestore)
    , mustPrintParentField(other.mustPrintParentField)
    , hasPrintedParentField(other.hasPrintedParentField)
    , printParentField(other.printParentField)
    , printClassName(other.printClassName)
    , printNameAtEndOfObject(other.printNameAtEndOfObject)
    , printListIndex(other.printListIndex)
    , mustPrintListIndex(other.mustPrintListIndex)
    , indentation(other.indentation)
    , children(other.children)
{
}

PrintHifVisitor::Data &PrintHifVisitor::Data::operator=(PrintHifVisitor::Data other)
{
    swap(other);
    return *this;
}

void PrintHifVisitor::Data::swap(PrintHifVisitor::Data &other)
{
    std::swap(streamRestore, other.streamRestore);
    std::swap(mustPrintParentField, other.mustPrintParentField);
    std::swap(hasPrintedParentField, other.hasPrintedParentField);
    std::swap(printParentField, other.printParentField);
    std::swap(printClassName, other.printClassName);
    std::swap(printNameAtEndOfObject, other.printNameAtEndOfObject);
    std::swap(printListIndex, other.printListIndex);
    std::swap(mustPrintListIndex, other.mustPrintListIndex);
    std::swap(indentation, other.indentation);
    std::swap(children, other.children);
}

// ///////////////////////////////////////////////////////////////////
// PushOptions
// ///////////////////////////////////////////////////////////////////
PrintHifVisitor::PushOptions::PushOptions()
    : printParentField(true)
    , printClassName(true)
    , printNameAtEndOfObject(false)
    , printListIndex(false)
{
    // ntd
}

PrintHifVisitor::PushOptions::~PushOptions()
{
    // ntd
}

PrintHifVisitor::PushOptions::PushOptions(const PrintHifVisitor::PushOptions &other)
    : printParentField(other.printParentField)
    , printClassName(other.printClassName)
    , printNameAtEndOfObject(other.printNameAtEndOfObject)
    , printListIndex(other.printListIndex)
{
    // ntd
}

PrintHifVisitor::PushOptions &PrintHifVisitor::PushOptions::operator=(PrintHifVisitor::PushOptions other)
{
    swap(other);
    return *this;
}

void PrintHifVisitor::PushOptions::swap(PrintHifVisitor::PushOptions &other)
{
    std::swap(printParentField, other.printParentField);
    std::swap(printClassName, other.printClassName);
    std::swap(printNameAtEndOfObject, other.printNameAtEndOfObject);
    std::swap(printListIndex, other.printListIndex);
}

// ///////////////////////////////////////////////////////////////////
// Utility methods
// ///////////////////////////////////////////////////////////////////

std::string PrintHifVisitor::stringTypeVariant(const Type::TypeVariant t)
{
    if (t == Type::NATIVE_TYPE)
        return "";
    else
        return Type::typeVariantToString(t);
}

int PrintHifVisitor::visitList(BList<Object> &l)
{
    if (l.empty())
        return 0;
    _pushBList();

    std::string listName(_toUpperCase(l.getName()));

    if (_opt.printSummary) {
        BListSet::iterator it = _listsMustPrintInBrief.find(&l);
        if (it == _listsMustPrintInBrief.end()) {
            _out << "{:" << listName << " ";
            _out << "<<List of " << l.size() << " element(s) omitted in summary>>" << " " << listName << ":}";
            _pop();
            return 0;
        } else {
            _listsMustPrintInBrief.erase(it);
            // continue printing
        }
    }

    _out << "{:" << listName;

    int ret = 0;
    for (BList<Object>::iterator i = l.begin(); i != l.end(); ++i) {
        ret |= (*i)->acceptVisitor(*this);
    }
    _printChildenStrings();

    _out << "}:" << listName;
    _pop();

    return ret;
}

void PrintHifVisitor::printHifEnum(
    const std::string &eVal,
    const std::string &before,
    const std::string &after,
    const bool manageEmpty)
{
    if (manageEmpty && eVal == "")
        return;
    _out << before << eVal << after;
}

// ///////////////////////////////////////////////////////////////////
// Parents Objects (virtual)
// ///////////////////////////////////////////////////////////////////

int PrintHifVisitor::visitAction(Action *) { return 0; }

int PrintHifVisitor::visitAlt(Alt *) { return 0; }

int PrintHifVisitor::visitBaseContents(BaseContents *o)
{
    visitScope(o);
    return 0;
}

int PrintHifVisitor::visitCompositeType(CompositeType *o)
{
    visitType(o);
    return 0;
}

int PrintHifVisitor::visitConstValue(ConstValue *o)
{
    visitValue(o);
    return 0;
}

int PrintHifVisitor::visitDataDeclaration(DataDeclaration *o)
{
    visitDeclaration(o);
    return 0;
}

int PrintHifVisitor::visitDeclaration(Declaration *) { return 0; }

int PrintHifVisitor::visitGenerate(Generate *o)
{
    visitBaseContents(o);
    return 0;
}

int PrintHifVisitor::visitPPAssign(PPAssign *o)
{
    printHifEnum(portDirectionToString(o->getDirection()));
    visitReferencedAssign(o);
    return 0;
}

int PrintHifVisitor::visitPrefixedReference(PrefixedReference *o)
{
    visitValue(o);
    return 0;
}

int PrintHifVisitor::visitReferencedAssign(ReferencedAssign *o)
{
    visitTypedObject(o);
    return 0;
}

int PrintHifVisitor::visitReferencedType(ReferencedType *o)
{
    visitType(o);
    return 0;
}

int PrintHifVisitor::visitScope(Scope *o)
{
    visitDeclaration(o);
    return 0;
}

int PrintHifVisitor::visitScopedType(ScopedType *o)
{
    _printFlag(o->isConstexpr(), "CONSTEXPR");
    visitType(o);
    return 0;
}

int PrintHifVisitor::visitSimpleType(SimpleType *o)
{
    _printFlag(o->isConstexpr(), "CONSTEXPR");
    visitType(o);
    return 0;
}

int PrintHifVisitor::visitSubProgram(SubProgram *o)
{
    _printFlag(o->isStandard(), "STANDARD");
    printHifEnum(SubProgram::kindToString(o->getKind()));
    visitScope(o);
    return 0;
}

int PrintHifVisitor::visitTPAssign(TPAssign *o)
{
    visitReferencedAssign(o);
    return 0;
}

int PrintHifVisitor::visitType(Type *o)
{
    printHifEnum(stringTypeVariant(o->getTypeVariant()), " [[[", "]]]", true);
    return 0;
}

int PrintHifVisitor::visitTypeDeclaration(TypeDeclaration *o)
{
    visitScope(o);
    return 0;
}

int PrintHifVisitor::visitTypedObject(TypedObject *) { return 0; }

int PrintHifVisitor::visitValue(Value *o)
{
    visitTypedObject(o);
    return 0;
}

// ///////////////////////////////////////////////////////////////////
// Hif Objects
// ///////////////////////////////////////////////////////////////////

int PrintHifVisitor::visitAggregate(Aggregate &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitValue(&o);

    if (guide) {
        GuideVisitor::visitAggregate(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitAggregateAlt(AggregateAlt &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitAlt(&o);

    if (guide) {
        GuideVisitor::visitAggregateAlt(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitAlias(Alias &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitDataDeclaration(&o);
    _printFlag(o.isStandard(), "STANDARD");

    if (guide) {
        GuideVisitor::visitAlias(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitArray(Array &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isSigned(), "SIGNED");
    visitCompositeType(&o);

    _addCustomPrint(o.getType(), "OF");

    if (guide) {
        GuideVisitor::visitArray(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitAssign(Assign &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    if (guide) {
        GuideVisitor::visitAssign(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitSystem(System &o)
{
    PushOptions opt;
    opt.printNameAtEndOfObject = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    printHifEnum(languageIDToString(o.getLanguageID()));
    System::VersionInfo version = o.getVersionInfo();
    _printString(version.release, "RELEASE");
    _printString(version.tool, "TOOL");
    _printString(version.generationDate, "GENERATION_DATE");
    std::stringstream ss;
    std::string s;
    ss << version.formatVersionMajor << "." << version.formatVersionMinor;
    ss >> s;
    _printString(s, "FORMAT_VERSION");

    visitScope(&o);

    if (guide) {
        GuideVisitor::visitSystem(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitBit(Bit &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isLogic(), "LOGIC");
    _printFlag(o.isResolved(), "RESOLVED");
    visitSimpleType(&o);

    if (guide) {
        GuideVisitor::visitBit(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitBitValue(BitValue &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    printHifEnum(bitConstantToString(o.getValue()), " '", "'");
    visitConstValue(&o);

    if (guide) {
        GuideVisitor::visitBitValue(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitBitvector(Bitvector &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isLogic(), "LOGIC");
    _printFlag(o.isResolved(), "RESOLVED");
    _printFlag(o.isSigned(), "SIGNED");
    visitSimpleType(&o);

    if (guide) {
        GuideVisitor::visitBitvector(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitBitvectorValue(BitvectorValue &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _out << " " << "\"" << o.getValue() << "\"";
    visitConstValue(&o);

    if (guide) {
        GuideVisitor::visitBitvectorValue(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitBool(Bool &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitSimpleType(&o);

    if (guide) {
        GuideVisitor::visitBool(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitBoolValue(BoolValue &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _out << " ";
    if (o.getValue())
        _out << "TRUE";
    else
        _out << "FALSE";
    _out << " ";
    visitConstValue(&o);

    if (guide) {
        GuideVisitor::visitBoolValue(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitIfAlt(IfAlt &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitAlt(&o);

    if (guide) {
        GuideVisitor::visitIfAlt(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitIf(If &o)
{
    PushOptions opt;
    opt.printListIndex = true;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitAction(&o);

    if (guide) {
        GuideVisitor::visitIf(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitCast(Cast &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitValue(&o);

    if (guide) {
        GuideVisitor::visitCast(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitChar(Char &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitSimpleType(&o);

    if (guide) {
        GuideVisitor::visitChar(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitCharValue(CharValue &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _out << " " << "'";
    if (o.getValue() == '\0')
        _out << "NUL";
    else if (o.getValue() == '\n')
        _out << "LF";
    else
        _out << o.getValue();
    _out << "'";
    visitConstValue(&o);

    if (guide) {
        GuideVisitor::visitCharValue(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitConst(Const &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isStandard(), "STANDARD");
    _printFlag(o.isInstance(), "INSTANCE");
    _printFlag(o.isDefine(), "DEFINE");
    visitDataDeclaration(&o);

    if (guide) {
        GuideVisitor::visitConst(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitContents(Contents &o)
{
    PushOptions opt;
    opt.printNameAtEndOfObject = true;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitBaseContents(&o);

    if (guide) {
        GuideVisitor::visitContents(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitDesignUnit(DesignUnit &o)
{
    PushOptions opt;
    opt.printNameAtEndOfObject = true;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitScope(&o);

    if (guide) {
        GuideVisitor::visitDesignUnit(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitEnum(Enum &o)
{
    PushOptions opt;
    opt.printListIndex = true;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitScopedType(&o);

    if (guide) {
        GuideVisitor::visitEnum(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitEnumValue(EnumValue &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitDataDeclaration(&o);

    if (guide) {
        GuideVisitor::visitEnumValue(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitEvent(Event &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitSimpleType(&o);

    if (guide) {
        GuideVisitor::visitEvent(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitBreak(Break &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitAction(&o);

    if (guide) {
        GuideVisitor::visitBreak(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitExpression(Expression &o)
{
    PushOptions opt;
    opt.printParentField = false;
    opt.printClassName   = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);
    printHifEnum(operatorToString(o.getOperator()), "", "");

    visitValue(&o);

    if (guide) {
        GuideVisitor::visitExpression(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitFunctionCall(FunctionCall &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitValue(&o);

    _addBListMustPrintInBrief(&o.templateParameterAssigns.toOtherBList<Object>());
    _addBListMustPrintInBrief(&o.parameterAssigns.toOtherBList<Object>());

    if (guide) {
        GuideVisitor::visitFunctionCall(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitField(Field &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitDataDeclaration(&o);

    if (o.getDirection() != hif::dir_none)
        printHifEnum(portDirectionToString(o.getDirection()));

    if (guide) {
        GuideVisitor::visitField(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitFieldReference(FieldReference &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitPrefixedReference(&o);

    if (guide) {
        GuideVisitor::visitFieldReference(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitFile(File &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitCompositeType(&o);

    _addCustomPrint(o.getType(), "OF");

    if (guide) {
        GuideVisitor::visitFile(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitFor(For &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitAction(&o);

    if (guide) {
        GuideVisitor::visitFor(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitForGenerate(ForGenerate &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitGenerate(&o);

    if (guide) {
        GuideVisitor::visitForGenerate(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitFunction(Function &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitSubProgram(&o);

    _addBListMustPrintInBrief(&o.templateParameters.toOtherBList<Object>());
    _addBListMustPrintInBrief(&o.parameters.toOtherBList<Object>());
    _addCustomPrint(o.getType(), "RETURN_TYPE");

    if (guide) {
        GuideVisitor::visitFunction(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitGlobalAction(GlobalAction &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    if (guide) {
        GuideVisitor::visitGlobalAction(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitEntity(Entity &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitScope(&o);

    if (guide) {
        GuideVisitor::visitEntity(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitIfGenerate(IfGenerate &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitGenerate(&o);

    if (guide) {
        GuideVisitor::visitIfGenerate(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitInt(Int &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isSigned(), "SIGNED");
    visitSimpleType(&o);

    if (guide) {
        GuideVisitor::visitInt(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitIntValue(IntValue &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _out << " " << o.getValue();
    visitConstValue(&o);

    if (guide) {
        GuideVisitor::visitIntValue(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitInstance(Instance &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitValue(&o);

    if (guide) {
        GuideVisitor::visitInstance(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitLibraryDef(LibraryDef &o)
{
    PushOptions opt;
    opt.printNameAtEndOfObject = true;
    _push(opt);
    bool guide = _printObjectInit(&o);

    // special management
    bool printLibrary = (!o.isStandard() || _opt.printHifStandardLibraries);
    if (!printLibrary && _opt.sem != nullptr) {
        LibraryDef *std = _opt.sem->getStandardLibrary(o.getName());
        if (std == nullptr)
            printLibrary = true;
    }

    guide |= printLibrary;

    printHifEnum(languageIDToString(o.getLanguageID()));
    _printFlag(o.isStandard(), "STANDARD");
    _printFlag(o.hasCLinkage(), "CLINKAGE");
    visitScope(&o);

    if (guide) {
        GuideVisitor::visitLibraryDef(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitLibrary(Library &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    _printString(o.getFilename(), "FILE");
    _printFlag(o.isStandard(), "STANDARD");
    _printFlag(o.isSystem(), "SYSTEM");
    visitReferencedType(&o);

    if (guide) {
        GuideVisitor::visitLibrary(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitMember(Member &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitPrefixedReference(&o);

    if (guide) {
        GuideVisitor::visitMember(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitIdentifier(Identifier &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitValue(&o);

    if (guide) {
        GuideVisitor::visitIdentifier(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitContinue(Continue &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitAction(&o);

    if (guide) {
        GuideVisitor::visitContinue(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitNull(Null &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitAction(&o);

    if (guide) {
        GuideVisitor::visitNull(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitTransition(Transition &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    _printString(o.getPrevName(), "PREV_NAME");
    _printInt(static_cast<long long>(o.getPriority()), "PRIORITY");
    visitAction(&o);

    if (guide) {
        GuideVisitor::visitTransition(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitParameterAssign(ParameterAssign &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitPPAssign(&o);

    if (guide) {
        GuideVisitor::visitParameterAssign(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitParameter(Parameter &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    printHifEnum(portDirectionToString(o.getDirection()));
    visitDataDeclaration(&o);

    if (guide) {
        GuideVisitor::visitParameter(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitProcedureCall(ProcedureCall &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitAction(&o);

    _addBListMustPrintInBrief(&o.templateParameterAssigns.toOtherBList<Object>());
    _addBListMustPrintInBrief(&o.parameterAssigns.toOtherBList<Object>());

    if (guide) {
        GuideVisitor::visitProcedureCall(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitPointer(Pointer &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitCompositeType(&o);

    if (guide) {
        GuideVisitor::visitPointer(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitPortAssign(PortAssign &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitPPAssign(&o);

    if (guide) {
        GuideVisitor::visitPortAssign(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitPort(Port &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    printHifEnum(portDirectionToString(o.getDirection()));
    _printFlag(o.isWrapper(), "WRAPPER");
    visitDataDeclaration(&o);

    if (guide) {
        GuideVisitor::visitPort(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitProcedure(Procedure &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitSubProgram(&o);

    _addBListMustPrintInBrief(&o.templateParameters.toOtherBList<Object>());
    _addBListMustPrintInBrief(&o.parameters.toOtherBList<Object>());

    if (guide) {
        GuideVisitor::visitProcedure(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitRange(Range &o)
{
    PushOptions opt;
    opt.printParentField = false;
    opt.printClassName   = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    printHifEnum(hif::rangeDirectionToString(o.getDirection()), "", "");
    visitValue(&o);

    if (guide) {
        GuideVisitor::visitRange(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitReal(Real &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitSimpleType(&o);

    if (guide) {
        GuideVisitor::visitReal(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitRealValue(RealValue &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _out << " " << o.getValue();
    visitConstValue(&o);

    if (guide) {
        GuideVisitor::visitRealValue(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitRecord(Record &o)
{
    PushOptions opt;
    opt.printListIndex = true;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isPacked(), "PACKED");
    _printFlag(o.isUnion(), "UNION");
    visitScopedType(&o);

    if (guide) {
        GuideVisitor::visitRecord(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitRecordValue(RecordValue &o)
{
    PushOptions opt;
    opt.printListIndex = true;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitValue(&o);

    if (guide) {
        GuideVisitor::visitRecordValue(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitRecordValueAlt(RecordValueAlt &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitAlt(&o);

    if (guide) {
        GuideVisitor::visitRecordValueAlt(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitReference(Reference &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitCompositeType(&o);

    if (guide) {
        GuideVisitor::visitReference(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitReturn(Return &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitAction(&o);

    if (guide) {
        GuideVisitor::visitReturn(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitSignal(Signal &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isStandard(), "STANDARD");
    _printFlag(o.isWrapper(), "WRAPPER");
    visitDataDeclaration(&o);

    if (guide) {
        GuideVisitor::visitSignal(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitSigned(Signed &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitSimpleType(&o);

    if (guide) {
        GuideVisitor::visitSigned(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitSlice(Slice &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitPrefixedReference(&o);

    if (guide) {
        GuideVisitor::visitSlice(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitState(State &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    _printInt(static_cast<long long>(o.getPriority()), "PRIORITY");
    _printFlag(o.isAtomic(), "ATOMIC");
    visitDeclaration(&o);

    if (guide) {
        GuideVisitor::visitState(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitString(String &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitSimpleType(&o);

    if (guide) {
        GuideVisitor::visitString(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitStateTable(StateTable &o)
{
    PushOptions opt;
    opt.printNameAtEndOfObject = true;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isStandard(), "STANDARD");
    printHifEnum(processFlavourToString(o.getFlavour()));
    _printFlag(o.getDontInitialize(), "DONT_INITIALIZE");
    _printName(o.getEntryStateName(), "ENTRY_STATE");
    visitScope(&o);

    if (guide) {
        GuideVisitor::visitStateTable(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitSwitchAlt(SwitchAlt &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitAlt(&o);

    if (guide) {
        GuideVisitor::visitSwitchAlt(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitSwitch(Switch &o)
{
    PushOptions opt;
    opt.printParentField = false;
    opt.printListIndex   = true;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    printHifEnum(caseSemanticsToString(o.getCaseSemantics()));
    visitAction(&o);

    if (guide) {
        GuideVisitor::visitSwitch(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitStringValue(StringValue &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _out << " " << "\"" << o.getValue() << "\"";
    _printFlag(o.isPlain(), "PLAIN");
    visitConstValue(&o);

    if (guide) {
        GuideVisitor::visitStringValue(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitTime(Time &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitSimpleType(&o);

    if (guide) {
        GuideVisitor::visitTime(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitTimeValue(TimeValue &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _out << " " << o.getValue();
    printHifEnum(TimeValue::timeUnitToString(o.getUnit()), " ", "");
    visitConstValue(&o);

    if (guide) {
        GuideVisitor::visitTimeValue(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}
int PrintHifVisitor::visitTypeDef(TypeDef &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isStandard(), "STANDARD");
    _printFlag(o.isOpaque(), "OPAQUE");
    _printFlag(o.isExternal(), "EXTERNAL");
    visitTypeDeclaration(&o);

    if (guide) {
        GuideVisitor::visitTypeDef(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitTypeReference(TypeReference &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitReferencedType(&o);

    if (guide) {
        GuideVisitor::visitTypeReference(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitTPAssign(&o);

    if (guide) {
        GuideVisitor::visitTypeTPAssign(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitTypeTP(TypeTP &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitTypeDeclaration(&o);

    if (guide) {
        GuideVisitor::visitTypeTP(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitUnsigned(Unsigned &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitSimpleType(&o);

    if (guide) {
        GuideVisitor::visitUnsigned(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitValueStatement(ValueStatement &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitAction(&o);

    if (guide) {
        GuideVisitor::visitValueStatement(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    visitTPAssign(&o);

    if (guide) {
        GuideVisitor::visitValueTPAssign(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitValueTP(ValueTP &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isCompileTimeConstant(), "CTC");
    visitDataDeclaration(&o);

    if (guide) {
        GuideVisitor::visitValueTP(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitVariable(Variable &o)
{
    PushOptions opt;
    opt.printParentField = false;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isStandard(), "STANDARD");
    _printFlag(o.isInstance(), "INSTANCE");
    visitDataDeclaration(&o);

    if (guide) {
        GuideVisitor::visitVariable(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitView(View &o)
{
    PushOptions opt;
    opt.printNameAtEndOfObject = true;
    _push(opt);
    const bool guide = _printObjectInit(&o);

    printHifEnum(languageIDToString(o.getLanguageID()));
    _printString(o.getFilename(), "FILE");
    _printFlag(o.isStandard(), "STANDARD");
    visitScope(&o);

    if (guide) {
        GuideVisitor::visitView(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitViewReference(ViewReference &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    _printName(o.getDesignUnit(), "UNIT");
    visitReferencedType(&o);

    if (guide) {
        GuideVisitor::visitViewReference(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitWait(Wait &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitAction(&o);

    if (guide) {
        GuideVisitor::visitWait(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitWhen(When &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isLogicTernary(), "LOGIC-TERNARY");

    visitValue(&o);

    if (guide) {
        GuideVisitor::visitWhen(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitWhenAlt(WhenAlt &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitAlt(&o);

    if (guide) {
        GuideVisitor::visitWhenAlt(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitWhile(While &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    _printFlag(o.isDoWhile(), "DO-WHILE");
    visitAction(&o);

    if (guide) {
        GuideVisitor::visitWhile(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitWith(With &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    printHifEnum(caseSemanticsToString(o.getCaseSemantics()));
    visitValue(&o);

    if (guide) {
        GuideVisitor::visitWith(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

int PrintHifVisitor::visitWithAlt(WithAlt &o)
{
    _push();
    const bool guide = _printObjectInit(&o);

    visitAlt(&o);

    if (guide) {
        GuideVisitor::visitWithAlt(o);
        _printChildenStrings();
    }

    _printObjectEnd(&o);
    _pop();

    return 0;
}

std::string PrintHifVisitor::_toUpperCase(const std::string &s)
{
    std::string ret(s);

    for (unsigned int i = 0; i < ret.length(); ++i) {
        if (isupper(ret[i]) && i != 0) {
            ret.insert(i, "_");
            ++i;
            continue;
        }

        ret[i] = static_cast<char>(toupper(static_cast<int>(ret[i])));
    }

    return ret;
}

void PrintHifVisitor::_push(const PrintHifVisitor::PushOptions &pushOpt)
{
    _out << std::flush;

    Data d;
    d.streamRestore          = _stream.str();
    d.mustPrintParentField   = _stack.back().printParentField;
    d.hasPrintedParentField  = false;
    d.printParentField       = pushOpt.printParentField;
    d.printClassName         = pushOpt.printClassName;
    d.printNameAtEndOfObject = pushOpt.printNameAtEndOfObject;
    d.printListIndex         = pushOpt.printListIndex;
    d.mustPrintListIndex     = _stack.back().printListIndex;
    d.indentation            = _indentedStream.getIndentation();

    _indentedStream.setIndentation(0);
    _stream.str("");
    _stack.push_back(d);
}

void PrintHifVisitor::_pushBList()
{
    Data d;
    d.streamRestore          = _stream.str();
    d.mustPrintParentField   = _stack.back().printParentField;
    d.hasPrintedParentField  = false;
    d.printParentField       = false;
    d.printClassName         = false;
    d.printNameAtEndOfObject = false;
    d.printListIndex         = _stack.back().printListIndex; // copied from parent
    d.mustPrintListIndex     = _stack.back().printListIndex;
    d.indentation            = _indentedStream.getIndentation();

    _indentedStream.setIndentation(0);
    _stream.str("");
    _stack.push_back(d);
}

void PrintHifVisitor::_pop()
{
    assert(!_stack.empty());
    const Data d = _stack.back();

    _out << std::flush;

    _stack.pop_back();

    assert(!_stack.empty());
    _stack.back().children.push_back(_stream.str());
    _stream.str("");
    const IndentedStream::Size ind = _indentedStream.getIndentation();
    _indentedStream.setIndentation(0);
    _out << d.streamRestore;
    _indentedStream.setIndentation(ind);

    _indentedStream.setIndentation(d.indentation);
}

bool PrintHifVisitor::_printObjectInit(Object *o)
{
    if (o == nullptr)
        return true;
    _printParentFieldBegin(o);
    if (_skipIfSummary(o)) {
        _out << "<<Object omitted in summary>>";
        return false;
    }

    _out << "(";

    if (_stack.back().printClassName) {
        _out << _toUpperCase(hif::classIDToString(o->getClassId()));
    }

    if (o->isInBList() && _stack.back().mustPrintListIndex) {
        const BList<Object>::size_t s = o->getBList()->getPosition(o);
        if (s < 10)
            _out << "<00" << s << ">";
        else if (s < 100)
            _out << "<0" << s << ">";
        else
            _out << "<" << s << ">";
    }

    std::string name = hif::objectGetName(o);
    if (!name.empty())
        _out << " " << name;

    return true;
}

void PrintHifVisitor::_printObjectEnd(Object *o)
{
    if (o == nullptr)
        return;
    if (_skipIfSummary(o)) {
        _printParentFieldEnd(o);
        return;
    }

    bool printedSomething = false;
    _out << hif::backends::indent;

    if (_opt.printCodeInfos && o->getSourceFileName() != "") {
        printedSomething = true;
        _out << "\n[CODE INFO: " << o->getSourceFileName() << ":" << o->getSourceLineNumber() << ":"
             << o->getSourceColumnNumber() << "]";
    }

    if (_opt.printComments && o->hasComments()) {
        printedSomething = true;
        _out << "\n[COMMENTS:" << std::endl;
        _out << hif::backends::indent;

        for (Object::StringList::iterator i = o->getComments().begin(); i != o->getComments().end(); ++i) {
            _out << "// " << *i << std::endl;
        }

        _out << hif::backends::unindent;
        _out << "]";
    }

    if (_opt.printProperties && o->hasProperties()) {
        printedSomething = true;
        _out << "\n[PROPERTIES:";

        Object::PropertyMapIterator i   = o->getPropertyBeginIterator();
        Object::PropertyMapIterator end = o->getPropertyEndIterator();

        _out << hif::backends::indent;
        for (; i != end; ++i) {
            _out << "\n-- " << i->first;
            if (i->second != nullptr) {
                const IndentedStream::Size ind = _indentedStream.getIndentation();
                _indentedStream.setIndentation(0);
                const StringList restore = _stack.back().children;
                _stack.back().children.clear();
                i->second->acceptVisitor(*this);
                _indentedStream.setIndentation(ind);
                _printChildenStrings();
                _stack.back().children = restore;
            } else
                _out << std::endl;
        }
        _out << hif::backends::unindent;
        _out << "]";
    }

    if (dynamic_cast<Declaration *>(o) != nullptr) {
        Declaration *d = static_cast<Declaration *>(o);
        if (_opt.printAdditionalKeywords && d->hasAdditionalKeywords()) {
            printedSomething = true;
            _out << "\n[ADDITIONAL_KEYWORDS:";
            _out << hif::backends::indent;
            for (Declaration::KeywordList::iterator it = d->getAdditionalKeywordsBeginIterator();
                 it != d->getAdditionalKeywordsEndIterator(); ++it) {
                _out << "\n-- " << *it;
            }
            _out << hif::backends::unindent;
            _out << "]";
        }
    }

    _out << hif::backends::unindent;

    if (printedSomething)
        _out << "\n";
    // Closing object
    _out << ")";
    if (_stack.back().printNameAtEndOfObject) {
        _out << " //" << _toUpperCase(hif::classIDToString(o->getClassId()));
        std::string name = hif::objectGetName(o);
        if (!name.empty())
            _out << " " << name;
    }

    _printParentFieldEnd(o);
}

void PrintHifVisitor::_printParentFieldBegin(Object *o)
{
    if (o == _root || o == nullptr)
        return;

    ObjectStringMap::iterator it = _customPrintMap.find(o);
    if (it != _customPrintMap.end()) {
        _out << ":" << it->second << " ";
        _customPrintMap.erase(it);
        return;
    }

    if (!_stack.back().mustPrintParentField)
        return;

    std::string parentField = _toUpperCase(o->getFieldName());
    if (parentField.empty())
        return;

    _out << ":" << parentField << std::endl;
    _stack.back().hasPrintedParentField = true;

    _out << hif::backends::indent;
}

void PrintHifVisitor::_printParentFieldEnd(Object *o)
{
    if (o == nullptr)
        return;
    if (!_stack.back().hasPrintedParentField)
        return;
    _out << hif::backends::unindent;
}

bool PrintHifVisitor::_skipIfSummary(Object *o)
{
    if (o == nullptr)
        return false;
    //    if (_opt.printSummary
    //            && o != _root
    //            && o->isInBList()
    //            && _objsMustPrintInBrief.find(o) == _objsMustPrintInBrief.end()
    //            )
    //        return true;
    return false;
}

void PrintHifVisitor::_printChildenStrings()
{
    const bool split = _checkMultiline();

    _out << hif::backends::indent;

    StringList childStrings = _stack.back().children;
    for (StringList::iterator i = childStrings.begin(); i != childStrings.end(); ++i) {
        if (!split)
            _out << " ";
        else
            _out << std::endl;
        _out << *i;
    }
    if (split)
        _out << std::endl;
    _out << hif::backends::unindent;
}

bool PrintHifVisitor::_checkMultiline()
{
    std::string::size_type size = 0;

    StringList childStrings = _stack.back().children;
    for (StringList::iterator i = childStrings.begin(); i != childStrings.end(); ++i) {
        std::string &s = *i;
        if (s.find('\n') != std::string::npos)
            return true;
        size += s.size();
    }

    return size > 100u;
}

void PrintHifVisitor::_printFlag(const bool value, const std::string &name, const bool printAnyway)
{
    if (!value && !printAnyway)
        return;
    _out << " [";
    if (!value)
        _out << "!";
    _out << name << "]";
}

void PrintHifVisitor::_printString(const std::string &value, const std::string &name, const bool printAnyway)
{
    if (value.empty() && !printAnyway)
        return;
    _out << " [" << name << ": \"" << value << "\"]";
}

void PrintHifVisitor::_printInt(long long value, const std::string &name)
{
    _out << " [" << name << ": " << value << "]";
}

void PrintHifVisitor::_printName(const std::string &value, const std::string &name, const bool printAnyway)
{
    if ((value.empty() || value == NAME_NONE) && !printAnyway)
        return;
    _out << " [" << name << ": " << value << "]";
}

void PrintHifVisitor::_addObjectMustPrintInBrief(Object *o)
{
    if (o == nullptr)
        return;
    _objsMustPrintInBrief.insert(o);
}

void PrintHifVisitor::_addBListMustPrintInBrief(BList<Object> *l)
{
    if (l == nullptr)
        return;
    _listsMustPrintInBrief.insert(l);
}

void PrintHifVisitor::_addCustomPrint(Object *o, const std::string &customString)
{
    if (o == nullptr)
        return;
    if (_customPrintMap.find(o) != _customPrintMap.end())
        return;
    _customPrintMap[o] = customString;
}

} // namespace

void printHif(Object &obj, std::ostream &o, const PrintHifOptions &opt)
{
    PrintHifVisitor p(&obj, opt);
    obj.acceptVisitor(p);

    p.print(o);
}
void printOperator(Operator oper, std::ostream &o) { o << hif::operatorToString(oper); }

} // namespace hif
