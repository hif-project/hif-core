/// @file PrintXmlVisitor.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stack>
#include <string>

#include "hif/hifPrinter.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/application_utils/portability.hpp"
#include "hif/semantics/semantics.hpp"

#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/Exception.h"
#include "Poco/XML/XMLWriter.h"

namespace hif
{

namespace /*Anonymous*/
{

/// PrintXMLVisitor
///
/// @brief This class prints the HIF object subtree into an output stream.
///
/// Design Patterns:
/// - Visitor
///
class PrintXmlVisitor : public GuideVisitor
{
public:
    /// Constructor
    ///
    /// @brief Constructor of PrintVisitor class. Initialize fields to print out
    /// an HIF description.
    ///
    /// @param o streams inside which will print the ASCII description of the HIF tree.
    ///
    PrintXmlVisitor(std::ostream &o, Poco::XML::AutoPtr<Poco::XML::Document> pDoc, const PrintHifOptions &opt);

    /// Virtual Destructor
    ///
    virtual ~PrintXmlVisitor();

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
    virtual int visitCast(Cast &o);
    virtual int visitChar(Char &o);
    virtual int visitCharValue(CharValue &o);
    virtual int visitConst(Const &o);
    virtual int visitContents(Contents &o);
    virtual int visitDesignUnit(DesignUnit &o);
    virtual int visitEntity(Entity &o);
    virtual int visitEnum(Enum &o);
    virtual int visitEnumValue(EnumValue &o);
    virtual int visitBreak(Break &o);
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
    virtual int visitIf(If &o);
    virtual int visitIfAlt(IfAlt &o);
    virtual int visitIfGenerate(IfGenerate &o);
    virtual int visitInt(Int &o);
    virtual int visitIntValue(IntValue &o);
    virtual int visitInstance(Instance &o);
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

private:
    void _appendElement(Poco::XML::AutoPtr<Poco::XML::Element> e);
    void _createAndAppendElement(Object *o, std::string elName);
    void _addCodeLineInfoAndProperties(Object *o);

    /// @brief Output stream of HIF description.
    ///
    std::ostream &out;

    std::stack<Poco::XML::AutoPtr<Poco::XML::Element>> _visitStack;

    Poco::XML::AutoPtr<Poco::XML::Document> _pDoc;

    const PrintHifOptions &_opt;

    // K: disabled.
    PrintXmlVisitor(const PrintXmlVisitor &);
    PrintXmlVisitor &operator=(const PrintXmlVisitor &);
};

//#define DISABLE_CODE_INFO

PrintXmlVisitor::PrintXmlVisitor(
    std::ostream &o,
    Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
    const PrintHifOptions &opt)
    : out(o)
    , _visitStack()
    , _pDoc(pDoc)
    , _opt(opt)
{
    // Assures maximum precision for printing numbers.
    out << std::setprecision(std::numeric_limits<double>::digits10 + 1);
#if (defined _MSC_VER) && (_MSC_VER < 1900)
    // Forcing exponent to be printed with two digits, as in Linux
    _set_output_format(_TWO_DIGIT_EXPONENT);
#endif
}

PrintXmlVisitor::~PrintXmlVisitor() {}

//***********************************************************
// Hif Objects

void PrintXmlVisitor::_appendElement(Poco::XML::AutoPtr<Poco::XML::Element> e)
{
    if (_visitStack.empty())
        _pDoc->appendChild(e);
    else
        _visitStack.top()->appendChild(e);
}
void PrintXmlVisitor::_createAndAppendElement(Object *o, std::string elName)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement(elName);

    _visitStack.push(e);
    o->acceptVisitor(*this);
    _visitStack.pop();

    _appendElement(e);
}
void PrintXmlVisitor::_addCodeLineInfoAndProperties(Object *o)
{
#ifndef DISABLE_CODE_INFO
    // add code info
    if (o->getSourceLineNumber() != 0 || o->getSourceFileName() != "") {
        Poco::XML::AutoPtr<Poco::XML::Element> el = _pDoc->createElement("CODE_INFO");
        el->setAttribute("file", o->getSourceFileName());

        std::stringstream val1;
        val1 << o->getSourceLineNumber();
        el->setAttribute("line_number", val1.str());

        std::stringstream val2;
        val2 << o->getSourceColumnNumber();
        el->setAttribute("column_number", val2.str());

        _appendElement(el);
    }
#endif
    // add properties
    Object::PropertyMapIterator it    = o->getPropertyBeginIterator();
    Object::PropertyMapIterator endIt = o->getPropertyEndIterator();
    if (it != endIt) {
        Poco::XML::AutoPtr<Poco::XML::Element> pList = _pDoc->createElement("PROPERTIES");
        for (; it != endIt; ++it) {
            Poco::XML::AutoPtr<Poco::XML::Element> p = _pDoc->createElement("PROPERTY");
            p->setAttribute("name", it->first);
            pList->appendChild(p);
            _visitStack.push(p);
            if (it->second != nullptr) {
                _createAndAppendElement(it->second, "VALUE");
            }
            _visitStack.pop();
        }
        _appendElement(pList);
    }

    // add comments
    if (o->hasComments()) {
        Poco::XML::AutoPtr<Poco::XML::Element> el = _pDoc->createElement("COMMENTS");
        for (Object::StringList::iterator i = o->getComments().begin(); i != o->getComments().end(); ++i) {
            Poco::XML::AutoPtr<Poco::XML::Element> elc = _pDoc->createElement("COMMENT");
            elc->setAttribute("text", *i);
            el->appendChild(elc);
        }
        _appendElement(el);
    }

    // add additional keywords
    if (dynamic_cast<Declaration *>(o) != nullptr) {
        Declaration *d = static_cast<Declaration *>(o);
        if (d->hasAdditionalKeywords()) {
            Declaration::KeywordList::iterator kwit      = d->getAdditionalKeywordsBeginIterator();
            Declaration::KeywordList::iterator kwend     = d->getAdditionalKeywordsEndIterator();
            Poco::XML::AutoPtr<Poco::XML::Element> pList = _pDoc->createElement("ADDITIONAL_KEYWORDS");
            for (; kwit != kwend; ++kwit) {
                Poco::XML::AutoPtr<Poco::XML::Element> p = _pDoc->createElement("ADDITIONAL_KEYWORD");
                p->setAttribute("name", *kwit);
                pList->appendChild(p);
            }
            _appendElement(pList);
        }
    }

    return;
}
int PrintXmlVisitor::visitAggregate(Aggregate &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("AGGREGATE");
    _visitStack.push(e);

    if (!o.alts.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("ALTS");

        _visitStack.push(ei);
        visitList(o.alts);
        _visitStack.pop();

        _appendElement(ei);
    }

    if (o.getOthers())
        _createAndAppendElement(o.getOthers(), "OTHERS");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitAggregateAlt(AggregateAlt &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("ALT");
    _visitStack.push(e);

    //GuideVisitor::visitAggregateAlt(o);

    if (!o.indices.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("INDICES");

        _visitStack.push(ei);
        visitList(o.indices);
        _visitStack.pop();

        _appendElement(ei);
    }

    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitAlias(Alias &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("ALIAS");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.getRange())
        _createAndAppendElement(o.getRange(), "RANGE");
    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");
    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    if (o.isStandard())
        e->setAttribute("standard", "true");
    else
        e->setAttribute("standard", "false");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitArray(Array &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("ARRAY");
    _visitStack.push(e);

    // set attributes
    if (o.isSigned())
        e->setAttribute("signed", "true");
    else
        e->setAttribute("signed", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    if (o.getSpan())
        _createAndAppendElement(o.getSpan(), "RANGE");

    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitAssign(Assign &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("ASSIGN");
    _visitStack.push(e);

    if (o.getLeftHandSide())
        _createAndAppendElement(o.getLeftHandSide(), "LEFTHANDSIDE");

    if (o.getRightHandSide())
        _createAndAppendElement(o.getRightHandSide(), "RIGHTHANDSIDE");

    if (o.getDelay())
        _createAndAppendElement(o.getDelay(), "DELAY");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitSystem(System &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("SYSTEM");
    _visitStack.push(e);

    // set attributes
    e->setAttribute("name", o.getName());

    System::VersionInfo version = o.getVersionInfo();
    e->setAttribute("release", version.release);
    e->setAttribute("tool", version.tool);
    e->setAttribute("generationDate", version.generationDate);
    e->setAttribute("languageId", languageIDToString(o.getLanguageID()));
    std::stringstream ss;
    std::string s;
    ss << version.formatVersionMajor;
    ss << ".";
    ss << version.formatVersionMinor;
    ss >> s;
    e->setAttribute("formatVersion", s);

    if (!o.libraryDefs.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("LIBRARYDEFS");

        _visitStack.push(ei);
        visitList(o.libraryDefs);
        _visitStack.pop();

        _appendElement(ei);
    }

    if (!o.designUnits.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("DESIGNUNITS");

        _visitStack.push(ei);
        visitList(o.designUnits);
        _visitStack.pop();

        _appendElement(ei);
    }

    if (!o.declarations.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("DECLARATIONS");

        _visitStack.push(ei);
        visitList(o.declarations);
        _visitStack.pop();

        _appendElement(ei);
    }

    if (!o.libraries.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("LIBRARIES");

        _visitStack.push(ei);
        visitList(o.libraries);
        _visitStack.pop();

        _appendElement(ei);
    }

    if (!o.actions.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("ACTIONS");

        _visitStack.push(ei);
        visitList(o.actions);
        _visitStack.pop();

        _appendElement(ei);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    //_addCodeLineInfo( &o );
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitBit(Bit &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("BIT");
    _visitStack.push(e);

    // set attributes
    if (o.isConstexpr())
        e->setAttribute("constexpr", "true");
    else
        e->setAttribute("constexpr", "false");

    if (o.isLogic())
        e->setAttribute("logic", "true");
    else
        e->setAttribute("logic", "false");

    if (o.isResolved())
        e->setAttribute("resolved", "true");
    else
        e->setAttribute("resolved", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitBitValue(BitValue &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("BITVAL");
    _visitStack.push(e);

    // set attributes
    e->setAttribute("value", bitConstantToString(o.getValue()));

    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitBitvector(Bitvector &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("BITVECTOR");
    _visitStack.push(e);

    // set attributes
    if (o.isConstexpr())
        e->setAttribute("constexpr", "true");
    else
        e->setAttribute("constexpr", "false");

    if (o.isLogic())
        e->setAttribute("logic", "true");
    else
        e->setAttribute("logic", "false");

    if (o.isSigned())
        e->setAttribute("signed", "true");
    else
        e->setAttribute("signed", "false");

    if (o.isResolved())
        e->setAttribute("resolved", "true");
    else
        e->setAttribute("resolved", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    if (o.getSpan())
        _createAndAppendElement(o.getSpan(), "RANGE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitBitvectorValue(BitvectorValue &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("BITVECTORVAL");
    _visitStack.push(e);

    if (o.getValue().size() > 0)
        e->setAttribute("VALUE", o.getValue());

    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitBool(Bool &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("BOOLEAN");
    _visitStack.push(e);

    if (o.isConstexpr())
        e->setAttribute("constexpr", "true");
    else
        e->setAttribute("constexpr", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitBoolValue(BoolValue &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("BOOLVAL");
    _visitStack.push(e);

    // set attributes
    if (o.getValue() == true)
        e->setAttribute("value", "true");
    else
        e->setAttribute("value", "false");

    // Check if is needed
    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");
    /*	else
            cout << "Unexpected lack of syntactic type" << endl;
    */

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitIfAlt(IfAlt &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("ALT");
    _visitStack.push(e);

    if (o.getCondition())
        _createAndAppendElement(o.getCondition(), "CONDITION");

    visitList(o.actions);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitIf(If &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("IF");
    _visitStack.push(e);

    visitList(o.alts);
    if (!o.defaults.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("DEFAULTS");

        _visitStack.push(eiv);
        visitList(o.defaults);
        _visitStack.pop();

        _appendElement(eiv);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitCast(Cast &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("CAST");
    _visitStack.push(e);

    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");

    //GuideVisitor::visitCast(o);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitChar(Char &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("CHAR");
    _visitStack.push(e);

    if (o.isConstexpr())
        e->setAttribute("constexpr", "true");
    else
        e->setAttribute("constexpr", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitCharValue(CharValue &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("CHARVAL");
    _visitStack.push(e);

    std::string s;
    if (o.getValue() == '\0')
        s = "'NUL'";
    else if (o.getValue() == '\n')
        s = "'LF'";
    else
        s += o.getValue();
    e->setAttribute("value", s);

    // Check if is needed
    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitConst(Const &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("CONSTANT");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.isInstance())
        e->setAttribute("instance", "true");
    else
        e->setAttribute("instance", "false");

    if (o.isDefine())
        e->setAttribute("define", "true");
    else
        e->setAttribute("define", "false");

    if (o.isStandard())
        e->setAttribute("standard", "true");
    else
        e->setAttribute("standard", "false");

    if (o.getRange())
        _createAndAppendElement(o.getRange(), "RANGE");
    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");
    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitContents(Contents &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("CONTENTS");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (!o.libraries.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("LIBRARIES");

        _visitStack.push(ei);
        visitList(o.libraries);
        _visitStack.pop();

        _appendElement(ei);
    }

    if (!o.declarations.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("DECLARATIONS");

        _visitStack.push(ei);
        visitList(o.declarations);
        _visitStack.pop();

        _appendElement(ei);
    }

    if (!o.stateTables.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("STATETABLES");

        _visitStack.push(ei);
        visitList(o.stateTables);
        _visitStack.pop();

        _appendElement(ei);
    }

    if (!o.generates.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("GENERATES");

        _visitStack.push(ei);
        visitList(o.generates);
        _visitStack.pop();

        _appendElement(ei);
    }

    if (!o.instances.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("INSTANCES");

        _visitStack.push(ei);
        visitList(o.instances);
        _visitStack.pop();

        _appendElement(ei);
    }

    if (o.getGlobalAction())
        o.getGlobalAction()->acceptVisitor(*this);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitDesignUnit(DesignUnit &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("DESIGNUNIT");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    visitList(o.views);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitEnum(Enum &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("ENUM");
    _visitStack.push(e);

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    GuideVisitor::visitEnum(o);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitEnumValue(EnumValue &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("ENUMVAL");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.getRange())
        _createAndAppendElement(o.getRange(), "RANGE");
    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");
    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitBreak(Break &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("EXIT");
    _visitStack.push(e);

    if (o.getName() != NAME_NONE)
        e->setAttribute("name", o.getName());
    else
        e->setAttribute("name", "");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitEvent(Event &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("EVENT");
    _visitStack.push(e);

    if (o.isConstexpr())
        e->setAttribute("constexpr", "true");
    else
        e->setAttribute("constexpr", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitExpression(Expression &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("EXPRESSION");
    _visitStack.push(e);

    e->setAttribute("operator", operatorToString(o.getOperator()));

    if (o.getValue1())
        _createAndAppendElement(o.getValue1(), "VALUE1");
    if (o.getValue2())
        _createAndAppendElement(o.getValue2(), "VALUE2");
    //GuideVisitor::visitExpression(o);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitFunctionCall(FunctionCall &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("FCALL");
    _visitStack.push(e);

    // set attributes
    e->setAttribute("name", o.getName());

    if (o.getInstance() != nullptr)
        _createAndAppendElement(o.getInstance(), "INSTANCE");

    visitList(o.templateParameterAssigns);
    visitList(o.parameterAssigns);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitField(Field &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("FIELD");
    _visitStack.push(e);

    // set attributes
    e->setAttribute("name", o.getName());
    e->setAttribute("direction", portDirectionToString(o.getDirection()));

    if (o.getRange())
        _createAndAppendElement(o.getRange(), "RANGE");
    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");
    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitFieldReference(FieldReference &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("FIELDREFERENCE");
    _visitStack.push(e);

    if (o.getPrefix())
        _createAndAppendElement(o.getPrefix(), "PREFIX");

    // set attributes
    e->setAttribute("fieldName", o.getName());

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitFile(File &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("FILE");
    _visitStack.push(e);

    // set attributes
    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    GuideVisitor::visitFile(o);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitFor(For &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("FOR");
    _visitStack.push(e);

    NameTable *nameTable = NameTable::getInstance();
    if (!o.getName().empty() && o.getName() != nameTable->none())
        e->setAttribute("label", o.getName());
    else
        e->setAttribute("label", "");

    messageDebugAssert(o.initDeclarations.empty() ^ o.initValues.empty(), "Unexpected for", &o, nullptr);

    if (!o.initDeclarations.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("INITDECLARATIONS");

        _visitStack.push(eiv);
        visitList(o.initDeclarations);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.initValues.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("INITVALUES");

        _visitStack.push(eiv);
        visitList(o.initValues);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (o.getCondition() != nullptr) {
        _createAndAppendElement(o.getCondition(), "CONDITION");
    }

    if (!o.stepActions.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("STEPACTIONS");

        _visitStack.push(eiv);
        visitList(o.stepActions);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.forActions.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("ACTIONS");

        _visitStack.push(eiv);
        visitList(o.forActions);
        _visitStack.pop();

        _appendElement(eiv);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitForGenerate(ForGenerate &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("FORGENERATE");
    _visitStack.push(e);

    // set attributes
    e->setAttribute("name", o.getName());

    messageDebugAssert(o.initDeclarations.empty() ^ o.initValues.empty(), "Unexpected for generate", &o, nullptr);

    // Generate-related fields.
    if (o.getGlobalAction())
        o.getGlobalAction()->acceptVisitor(*this);

    if (!o.declarations.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("DECLARATIONS");

        _visitStack.push(eiv);
        visitList(o.declarations);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.stateTables.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("STATETABLES");

        _visitStack.push(eiv);
        visitList(o.stateTables);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.generates.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("GENERATES");

        _visitStack.push(eiv);
        visitList(o.generates);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.instances.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("INSTANCES");

        _visitStack.push(eiv);
        visitList(o.instances);
        _visitStack.pop();

        _appendElement(eiv);
    }

    // For-related fields.
    if (!o.initDeclarations.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("INITDECLARATIONS");

        _visitStack.push(eiv);
        visitList(o.initDeclarations);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.initValues.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("INITVALUES");

        _visitStack.push(eiv);
        visitList(o.initValues);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (o.getCondition() != nullptr) {
        _createAndAppendElement(o.getCondition(), "CONDITION");
    }

    if (!o.stepActions.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("STEPACTIONS");

        _visitStack.push(eiv);
        visitList(o.stepActions);
        _visitStack.pop();

        _appendElement(eiv);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitFunction(Function &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("FUNCTION");
    _visitStack.push(e);

    // set attributes
    e->setAttribute("name", o.getName());

    if (o.isStandard())
        e->setAttribute("standard", "true");
    else
        e->setAttribute("standard", "false");

    e->setAttribute("kind", SubProgram::kindToString(o.getKind()));

    if (!o.templateParameters.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("TEMPLATE_PARAMETERS");

        _visitStack.push(eiv);
        visitList(o.templateParameters);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.parameters.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("PARAMETERS");

        _visitStack.push(eiv);
        visitList(o.parameters);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");
    if (o.getStateTable())
        o.getStateTable()->acceptVisitor(*this);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitGlobalAction(GlobalAction &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("GLOBALACTION");
    _visitStack.push(e);

    visitList(o.actions);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitEntity(Entity &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("ENTITY");
    _visitStack.push(e);

    // set attributes
    e->setAttribute("name", o.getName());

    visitList(o.parameters);
    visitList(o.ports);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitIfGenerate(IfGenerate &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("IFGENERATE");
    _visitStack.push(e);

    // set attributes
    e->setAttribute("name", o.getName());
    if (o.getCondition())
        _createAndAppendElement(o.getCondition(), "CONDITION");
    if (o.getGlobalAction())
        o.getGlobalAction()->acceptVisitor(*this);

    if (!o.declarations.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("DECLARATIONS");

        _visitStack.push(eiv);
        visitList(o.declarations);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.stateTables.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("STATETABLES");

        _visitStack.push(eiv);
        visitList(o.stateTables);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.generates.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("GENERATES");

        _visitStack.push(eiv);
        visitList(o.generates);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.instances.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("INSTANCES");

        _visitStack.push(eiv);
        visitList(o.instances);
        _visitStack.pop();

        _appendElement(eiv);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitInt(Int &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("INTEGER");
    _visitStack.push(e);

    if (o.isConstexpr())
        e->setAttribute("constexpr", "true");
    else
        e->setAttribute("constexpr", "false");

    if (o.isSigned())
        e->setAttribute("signed", "true");
    else
        e->setAttribute("signed", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    if (o.getSpan())
        _createAndAppendElement(o.getSpan(), "SPAN");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitIntValue(IntValue &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("INTVAL");
    _visitStack.push(e);

    std::string s;
    std::stringstream val;
    val << o.getValue();
    e->setAttribute("value", val.str());

    if (o.getType() != nullptr)
        _createAndAppendElement(o.getType(), "TYPE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitInstance(Instance &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("INSTANCE");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.getReferencedType() != nullptr)
        o.getReferencedType()->acceptVisitor(*this);

    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    if (!o.portAssigns.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> etpa = _pDoc->createElement("PORTASSIGNS");

        _visitStack.push(etpa);
        visitList(o.portAssigns);
        _visitStack.pop();

        _appendElement(etpa);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitLibraryDef(LibraryDef &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("LIBRARYDEF");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.isStandard())
        e->setAttribute("standard", "true");
    else
        e->setAttribute("standard", "false");

    if (o.hasCLinkage())
        e->setAttribute("clinkage", "true");
    else
        e->setAttribute("clinkage", "false");

    e->setAttribute("languageId", languageIDToString(o.getLanguageID()));

    bool printLibrary = (!o.isStandard() || _opt.printHifStandardLibraries);
    if (!printLibrary && _opt.sem != nullptr) {
        LibraryDef *std = _opt.sem->getStandardLibrary(o.getName());
        if (std == nullptr)
            printLibrary = true;
    }

    if (printLibrary) {
        visitList(o.libraries);
        visitList(o.declarations);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitLibrary(Library &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("LIBRARY");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());
    e->setAttribute("filename", o.getFilename());

    if (o.getInstance() != nullptr)
        _createAndAppendElement(o.getInstance(), "INSTANCE");

    if (o.isStandard())
        e->setAttribute("standard", "true");
    else
        e->setAttribute("standard", "false");

    if (o.isSystem())
        e->setAttribute("system", "true");
    else
        e->setAttribute("system", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitMember(Member &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("MEMBER");
    _visitStack.push(e);

    if (o.getPrefix() != nullptr)
        _createAndAppendElement(o.getPrefix(), "PREFIX");
    if (o.getIndex() != nullptr)
        _createAndAppendElement(o.getIndex(), "INDEX");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitIdentifier(Identifier &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("IDENTIFIER");
    _visitStack.push(e);

    //e->setAttribute( "index", o.GetIndex() );
    e->setAttribute("name", o.getName());

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitContinue(Continue &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("NEXT");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitNull(Null &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("nullptr");
    _visitStack.push(e);

    GuideVisitor::visitNull(o);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitTransition(Transition &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("TRANSITION");
    _visitStack.push(e);

    if (!o.getPrevName().empty() && !o.getPrevName().empty())
        e->setAttribute("source", o.getPrevName().c_str());
    else
        e->setAttribute("source", "");
    e->setAttribute("destination", o.getName());

    if (o.getEnablingOrCondition())
        e->setAttribute("orMode", "true");
    else
        e->setAttribute("orMode", "false");

    std::string s;
    std::stringstream val;
    val << o.getPriority();
    e->setAttribute("priority", val.str());

    if (!o.enablingLabelList.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("ENABLING_LABEL");

        _visitStack.push(eiv);
        visitList(o.enablingLabelList);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.enablingList.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("ENABLING");

        _visitStack.push(eiv);
        visitList(o.enablingList);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.updateLabelList.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("UPDATE_LABEL");

        _visitStack.push(eiv);
        visitList(o.updateLabelList);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.updateList.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("UPDATE");

        _visitStack.push(eiv);
        visitList(o.updateList);
        _visitStack.pop();

        _appendElement(eiv);
    }

    //	visitList(o.property);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitParameterAssign(ParameterAssign &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("PARAMETERASSIGN");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());
    e->setAttribute("direction", portDirectionToString(o.getDirection()));

    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitParameter(Parameter &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("PARAMETER");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());
    e->setAttribute("direction", portDirectionToString(o.getDirection()));

    if (o.getRange())
        _createAndAppendElement(o.getRange(), "RANGE");
    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");
    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitProcedureCall(ProcedureCall &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("PCALL");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.getInstance() != nullptr)
        _createAndAppendElement(o.getInstance(), "INSTANCE");

    visitList(o.templateParameterAssigns);
    visitList(o.parameterAssigns);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitPointer(Pointer &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("POINTER");
    _visitStack.push(e);

    // set attributes
    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    GuideVisitor::visitPointer(o);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitPortAssign(PortAssign &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("PORTASSIGN");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());
    e->setAttribute("direction", portDirectionToString(o.getDirection()));

    if (o.getType())
        _createAndAppendElement(o.getValue(), "TYPE");
    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");
    if (o.getPartialBind())
        _createAndAppendElement(o.getPartialBind(), "PARTIAL_BIND");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitPort(Port &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("PORT");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());
    e->setAttribute("direction", portDirectionToString(o.getDirection()));

    if (o.isWrapper())
        e->setAttribute("wrapper", "true");
    else
        e->setAttribute("wrapper", "false");

    if (o.getRange())
        _createAndAppendElement(o.getRange(), "RANGE");
    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");
    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitProcedure(Procedure &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("PROCEDURE");
    _visitStack.push(e);

    // set attributes
    e->setAttribute("name", o.getName());

    if (o.isStandard())
        e->setAttribute("standard", "true");
    else
        e->setAttribute("standard", "false");

    e->setAttribute("kind", SubProgram::kindToString(o.getKind()));

    if (!o.templateParameters.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("TEMPLATE_PARAMETERS");

        _visitStack.push(eiv);
        visitList(o.templateParameters);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.parameters.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("PARAMETERS");

        _visitStack.push(eiv);
        visitList(o.parameters);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (o.getStateTable())
        o.getStateTable()->acceptVisitor(*this);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitRange(Range &o)
{
    //Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("RANGE");
    //_visitStack.push( e );

    std::string direction("");
    switch (o.getDirection()) {
    case dir_upto:
        direction = "UPTO";
        break;
    case dir_downto:
        direction = "DOWNTO";
        break;
    default:
        messageError("Unexpected direction", &o, nullptr);
    }

    if (_visitStack.empty() || dynamic_cast<TypeReference *>(o.getParent()) || o.isInBList()) {
        // print visit probably start from range
        // so it is created a new range element in this case.
        //

        Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("RANGE");
        _visitStack.push(e);

        e->setAttribute("direction", direction);
        if (o.getLeftBound())
            _createAndAppendElement(o.getLeftBound(), "LEFTBOUND");

        if (o.getRightBound())
            _createAndAppendElement(o.getRightBound(), "RIGHTBOUND");

        if (o.getType())
            _createAndAppendElement(o.getType(), "TYPE");

        _visitStack.pop();
        _appendElement(e);
    } else {
        _visitStack.top()->setAttribute("direction", direction);

        if (o.getLeftBound())
            _createAndAppendElement(o.getLeftBound(), "LEFTBOUND");

        if (o.getRightBound())
            _createAndAppendElement(o.getRightBound(), "RIGHTBOUND");

        if (o.getType())
            _createAndAppendElement(o.getType(), "TYPE");
    }

    return 0;
}

int PrintXmlVisitor::visitReal(Real &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("REAL");
    _visitStack.push(e);

    if (o.getSpan())
        _createAndAppendElement(o.getSpan(), "SPAN");

    if (o.isConstexpr())
        e->setAttribute("constexpr", "true");
    else
        e->setAttribute("constexpr", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitRealValue(RealValue &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("REALVAL");
    _visitStack.push(e);

    if (o.getType() != nullptr)
        _createAndAppendElement(o.getType(), "TYPE");

    std::string s;
    std::stringstream val;
    val << std::setprecision(std::numeric_limits<double>::digits10 + 1);
    val << o.getValue();
    e->setAttribute("value", val.str());

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitRecord(Record &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("RECORD");
    _visitStack.push(e);

    // set attributes
    if (o.isConstexpr())
        e->setAttribute("constexpr", "true");
    else
        e->setAttribute("constexpr", "false");

    if (o.isPacked())
        e->setAttribute("packed", "true");
    else
        e->setAttribute("packed", "false");

    if (o.isUnion())
        e->setAttribute("union", "true");
    else
        e->setAttribute("union", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    GuideVisitor::visitRecord(o);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitRecordValue(RecordValue &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("RECORDVALUE");
    _visitStack.push(e);

    if (!o.alts.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> ei = _pDoc->createElement("ALTS");

        _visitStack.push(ei);
        visitList(o.alts);
        _visitStack.pop();

        _appendElement(ei);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitRecordValueAlt(RecordValueAlt &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("RECORDVALUEALT");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitReference(Reference &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("REFERENCE");
    _visitStack.push(e);

    // set attributes

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    GuideVisitor::visitReference(o);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitReturn(Return &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("RETURN");
    _visitStack.push(e);

    GuideVisitor::visitReturn(o);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitSignal(Signal &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("SIGNAL");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.isStandard())
        e->setAttribute("standard", "true");
    else
        e->setAttribute("standard", "false");

    if (o.isWrapper())
        e->setAttribute("wrapper", "true");
    else
        e->setAttribute("wrapper", "false");

    if (o.getRange())
        _createAndAppendElement(o.getRange(), "RANGE");
    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");
    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitSigned(Signed &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("SIGNED_TYPE");
    _visitStack.push(e);

    // set attributes
    if (o.isConstexpr())
        e->setAttribute("constexpr", "true");
    else
        e->setAttribute("constexpr", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    if (o.getSpan())
        _createAndAppendElement(o.getSpan(), "RANGE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitSlice(Slice &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("SLICE");
    _visitStack.push(e);

    if (o.getPrefix())
        _createAndAppendElement(o.getPrefix(), "PREFIX");
    if (o.getSpan())
        _createAndAppendElement(o.getSpan(), "RANGE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitState(State &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("STATE");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.isAtomic())
        e->setAttribute("atomic", "true");
    else
        e->setAttribute("atomic", "false");

    std::string s;
    std::stringstream val;
    val << o.getPriority();
    e->setAttribute("priority", val.str());

    if (!o.actions.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("ACTIONS");

        _visitStack.push(eiv);
        visitList(o.actions);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.invariants.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("INVARIANTS");

        _visitStack.push(eiv);
        visitList(o.invariants);
        _visitStack.pop();

        _appendElement(eiv);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitString(String &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("STRING");
    _visitStack.push(e);

    if (o.isConstexpr())
        e->setAttribute("constexpr", "true");
    else
        e->setAttribute("constexpr", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    if (o.getSpanInformation() != nullptr)
        _createAndAppendElement(o.getSpanInformation(), "SPAN_INFORMATION");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitStateTable(StateTable &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("STATETABLE");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.isStandard())
        e->setAttribute("standard", "true");
    else
        e->setAttribute("standard", "false");

    e->setAttribute("flavour", processFlavourToString(o.getFlavour()));

    std::string dontInit = (o.getDontInitialize()) ? "true" : "false";
    e->setAttribute("dontinitialize", dontInit.c_str());

    if (o.getEntryStateName() != NAME_NONE)
        e->setAttribute("entryStateName", o.getEntryStateName().c_str());
    else
        e->setAttribute("entryStateName", "");

    if (!o.sensitivity.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("SENSITIVITY");

        _visitStack.push(eiv);
        visitList(o.sensitivity);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.sensitivityPos.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("SENSITIVITY_POS");

        _visitStack.push(eiv);
        visitList(o.sensitivityPos);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.sensitivityNeg.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("SENSITIVITY_NEG");

        _visitStack.push(eiv);
        visitList(o.sensitivityNeg);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.declarations.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("DECLARATIONS");

        _visitStack.push(eiv);
        visitList(o.declarations);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.states.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("STATES");

        _visitStack.push(eiv);
        visitList(o.states);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.edges.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("EDGES");

        _visitStack.push(eiv);
        visitList(o.edges);
        _visitStack.pop();

        _appendElement(eiv);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitSwitchAlt(SwitchAlt &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("ALT");
    _visitStack.push(e);

    visitList(o.conditions);

    if (!o.actions.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("ACTIONS");

        _visitStack.push(eiv);
        visitList(o.actions);
        _visitStack.pop();

        _appendElement(eiv);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitSwitch(Switch &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("SWITCH");
    _visitStack.push(e);

    e->setAttribute("caseSemantics", hif::caseSemanticsToString(o.getCaseSemantics()));

    if (o.getCondition())
        _createAndAppendElement(o.getCondition(), "CONDITION");

    visitList(o.alts);

    if (!o.defaults.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("DEFAULTS");

        _visitStack.push(eiv);
        visitList(o.defaults);
        _visitStack.pop();

        _appendElement(eiv);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitStringValue(StringValue &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("STRINGVAL");
    _visitStack.push(e);

    e->setAttribute("value", o.getValue());

    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitTime(Time &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("TIME");
    _visitStack.push(e);

    if (o.isConstexpr())
        e->setAttribute("constexpr", "true");
    else
        e->setAttribute("constexpr", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitTimeValue(TimeValue &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("TIMEVALUE");
    _visitStack.push(e);

    std::string s;
    std::stringstream val;
    val << o.getValue();
    e->setAttribute("value", val.str());

    e->setAttribute("unit", TimeValue::timeUnitToString(o.getUnit()));

    if (o.getType() != nullptr)
        _createAndAppendElement(o.getType(), "TYPE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}
int PrintXmlVisitor::visitTypeDef(TypeDef &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("TYPEDEF");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.isStandard())
        e->setAttribute("standard", "true");
    else
        e->setAttribute("standard", "false");

    if (o.isOpaque())
        e->setAttribute("opaque", "true");
    else
        e->setAttribute("opaque", "false");

    if (o.isExternal())
        e->setAttribute("external", "true");
    else
        e->setAttribute("external", "false");

    if (!o.templateParameters.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> etpa = _pDoc->createElement("TEMPLATE_PARAMETERS");

        _visitStack.push(etpa);
        visitList(o.templateParameters);
        _visitStack.pop();

        _appendElement(etpa);
    }

    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");
    if (o.getRange())
        _createAndAppendElement(o.getRange(), "RANGE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitTypeReference(TypeReference &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("TYPEREFERENCE");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    if (o.getInstance() != nullptr)
        _createAndAppendElement(o.getInstance(), "INSTANCE");

    if (!o.templateParameterAssigns.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> etpa = _pDoc->createElement("TEMPLATE_PARAMETER_ASSIGNS");

        _visitStack.push(etpa);
        visitList(o.templateParameterAssigns);
        _visitStack.pop();

        _appendElement(etpa);
    }

    if (!o.ranges.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> er = _pDoc->createElement("RANGES");

        _visitStack.push(er);
        visitList(o.ranges); // Note: will not print <RANGE> tag
        _visitStack.pop();

        _appendElement(er);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("TYPETPASSIGN");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitTypeTP(TypeTP &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("TYPETP");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.getType())
        o.getType()->acceptVisitor(*this);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitUnsigned(Unsigned &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("UNSIGNED_TYPE");
    _visitStack.push(e);

    // set attributes
    if (o.isConstexpr())
        e->setAttribute("constexpr", "true");
    else
        e->setAttribute("constexpr", "false");

    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    if (o.getSpan())
        _createAndAppendElement(o.getSpan(), "RANGE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitValueStatement(ValueStatement &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("VALUESTATEMENT");
    _visitStack.push(e);

    GuideVisitor::visitValueStatement(o);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("VALUETPASSIGN");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitValueTP(ValueTP &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("VALUETP");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());
    if (o.isCompileTimeConstant())
        e->setAttribute("compileTimeConstant", "true");
    else
        e->setAttribute("compileTimeConstant", "false");

    if (o.getRange())
        _createAndAppendElement(o.getRange(), "RANGE");
    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");
    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitVariable(Variable &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("VARIABLE");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.isStandard())
        e->setAttribute("standard", "true");
    else
        e->setAttribute("standard", "false");

    if (o.isInstance())
        e->setAttribute("instance", "true");
    else
        e->setAttribute("instance", "false");

    if (o.getRange())
        _createAndAppendElement(o.getRange(), "RANGE");
    if (o.getType())
        _createAndAppendElement(o.getType(), "TYPE");
    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitView(View &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("VIEW");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());

    if (o.isStandard())
        e->setAttribute("standard", "true");
    else
        e->setAttribute("standard", "false");

    e->setAttribute("filename", o.getFilename());
    e->setAttribute("languageId", languageIDToString(o.getLanguageID()));

    if (!o.templateParameters.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("TEMPLATE_PARAMETERS");

        _visitStack.push(eiv);
        visitList(o.templateParameters);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.inheritances.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("INHERITANCES");

        _visitStack.push(eiv);
        visitList(o.inheritances);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.libraries.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("LIBRARIES");

        _visitStack.push(eiv);
        visitList(o.libraries);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (!o.declarations.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("DECLARATIONS");

        _visitStack.push(eiv);
        visitList(o.declarations);
        _visitStack.pop();

        _appendElement(eiv);
    }

    if (o.getEntity())
        o.getEntity()->acceptVisitor(*this);
    if (o.getContents())
        o.getContents()->acceptVisitor(*this);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitViewReference(ViewReference &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("VIEWREFERENCE");
    _visitStack.push(e);

    e->setAttribute("name", o.getName());
    e->setAttribute("unitName", o.getDesignUnit().c_str());
    e->setAttribute("typeVariant", Type::typeVariantToString(o.getTypeVariant()));

    if (o.getInstance() != nullptr)
        _createAndAppendElement(o.getInstance(), "INSTANCE");

    visitList(o.templateParameterAssigns);

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitWait(Wait &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("WAIT");
    _visitStack.push(e);

    if (o.getTime() != nullptr)
        _createAndAppendElement(o.getTime(), "TIME");
    if (o.getCondition() != nullptr)
        _createAndAppendElement(o.getCondition(), "CONDITION");
    if (o.getRepetitions() != nullptr)
        _createAndAppendElement(o.getRepetitions(), "REPETITIONS");
    if (!o.sensitivity.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("SENSITIVITY");

        _visitStack.push(eiv);
        visitList(o.sensitivity);
        _visitStack.pop();

        _appendElement(eiv);
    }
    if (!o.sensitivityPos.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("SENSITIVITY_POS");

        _visitStack.push(eiv);
        visitList(o.sensitivityPos);
        _visitStack.pop();

        _appendElement(eiv);
    }
    if (!o.sensitivityNeg.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("SENSITIVITY_NEG");

        _visitStack.push(eiv);
        visitList(o.sensitivityNeg);
        _visitStack.pop();

        _appendElement(eiv);
    }
    if (!o.actions.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("ACTIONS");

        _visitStack.push(eiv);
        visitList(o.actions);
        _visitStack.pop();

        _appendElement(eiv);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitWhen(When &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("WHEN");
    _visitStack.push(e);

    if (o.isLogicTernary())
        e->setAttribute("logicTernary", "true");
    else
        e->setAttribute("logicTernary", "false");

    visitList(o.alts); // need node?

    if (o.getDefault())
        _createAndAppendElement(o.getDefault(), "DEFAULT");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitWhenAlt(WhenAlt &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("ALT");
    _visitStack.push(e);

    if (o.getCondition())
        _createAndAppendElement(o.getCondition(), "CONDITION");
    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitWhile(While &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("WHILE");
    _visitStack.push(e);

    NameTable *nameTable = NameTable::getInstance();
    if (!o.getName().empty() && o.getName() != nameTable->none())
        e->setAttribute("name", o.getName());
    else
        e->setAttribute("name", "");

    if (o.isDoWhile())
        e->setAttribute("doWhile", "true");
    else
        e->setAttribute("doWhile", "false");

    if (o.getCondition())
        _createAndAppendElement(o.getCondition(), "CONDITION");

    if (!o.actions.empty()) {
        Poco::XML::AutoPtr<Poco::XML::Element> eiv = _pDoc->createElement("ACTIONS");

        _visitStack.push(eiv);
        visitList(o.actions);
        _visitStack.pop();

        _appendElement(eiv);
    }

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitWith(With &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("WITH");
    _visitStack.push(e);

    e->setAttribute("caseSemantics", hif::caseSemanticsToString(o.getCaseSemantics()));

    if (o.getCondition())
        _createAndAppendElement(o.getCondition(), "CONDITION");

    visitList(o.alts);

    if (o.getDefault())
        _createAndAppendElement(o.getDefault(), "DEFAULT");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

int PrintXmlVisitor::visitWithAlt(WithAlt &o)
{
    Poco::XML::AutoPtr<Poco::XML::Element> e = _pDoc->createElement("ALT");
    _visitStack.push(e);

    visitList(o.conditions);
    if (o.getValue())
        _createAndAppendElement(o.getValue(), "VALUE");

    _addCodeLineInfoAndProperties(&o);
    _visitStack.pop();
    _appendElement(e);

    return 0;
}

} /* end namespace */
// Print XML part.
void printXml(Object &obj, std::ostream &o, const PrintHifOptions &opt)
{
    if (dynamic_cast<System *>(&obj) == nullptr) {
        messageInfo("Print XML can print only system at the moment.");
        return;
    }

    Poco::XML::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document;

    PrintXmlVisitor p(o, pDoc, opt);
    obj.acceptVisitor(p);

    Poco::XML::DOMWriter writer;
    writer.setNewLine("\n");
    writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
    writer.writeNode(o, pDoc);
}

} // namespace hif
