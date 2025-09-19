/// @file XmlParser.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <iostream>
#include <sstream>

#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/Exception.h"
#include "Poco/SAX/Attributes.h"
#include "Poco/SAX/ContentHandler.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/SAX/LexicalHandler.h"
#include "Poco/SAX/Locator.h"
#include "Poco/SAX/SAXParser.h"

#include "hif/hif.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

namespace hif
{

// Just to shut up compiler warnings.
Object *parse_xml(std::istream &in, hif::semantics::ILanguageSemantics *sem);

namespace
{
class XmlParser
{
public:
    XmlParser(Object *&o, std::istream &in, hif::semantics::ILanguageSemantics *sem);
    ~XmlParser();

private:
    Action *_visitAction(Poco::XML::Node *n);
    Aggregate *_visitAggregate(Poco::XML::Node *n);
    AggregateAlt *_visitAggregateAlt(Poco::XML::Node *n);
    Alias *_visitAlias(Poco::XML::Node *n);
    Array *_visitArray(Poco::XML::Node *n);
    Assign *_visitAssign(Poco::XML::Node *n);
    Bit *_visitBit(Poco::XML::Node *n);
    BitValue *_visitBitval(Poco::XML::Node *n);
    Bitvector *_visitBitvector(Poco::XML::Node *n);
    BitvectorValue *_visitBitvectorValue(Poco::XML::Node *n);
    Bool *_visitBool(Poco::XML::Node *n);
    BoolValue *_visitBoolValue(Poco::XML::Node *n);
    Cast *_visitCast(Poco::XML::Node *n);
    Char *_visitChar(Poco::XML::Node *n);
    CharValue *_visitCharValue(Poco::XML::Node *n);
    Const *_visitConst(Poco::XML::Node *n);
    Contents *_visitContents(Poco::XML::Node *n);
    DataDeclaration *_visitDataDeclaration(Poco::XML::Node *n);
    Declaration *_visitDeclaration(Poco::XML::Node *n);
    DesignUnit *_visitDesignUnit(Poco::XML::Node *n);
    Enum *_visitEnum(Poco::XML::Node *n);
    EnumValue *_visitEnumValue(Poco::XML::Node *n);
    Break *_visitBreak(Poco::XML::Node *n);
    Event *_visitEvent(Poco::XML::Node *n);
    Expression *_visitExpression(Poco::XML::Node *n);
    FunctionCall *_visitFunctionCall(Poco::XML::Node *n);
    Field *_visitField(Poco::XML::Node *n);
    FieldReference *_visitFieldReference(Poco::XML::Node *n);
    File *_visitFile(Poco::XML::Node *n);
    For *_visitFor(Poco::XML::Node *n);
    ForGenerate *_visitForGenerate(Poco::XML::Node *n);
    Function *_visitFunction(Poco::XML::Node *n);
    Generate *_visitGenerate(Poco::XML::Node *n);
    GlobalAction *_visitGlobalAction(Poco::XML::Node *n);
    Entity *_visitEntity(Poco::XML::Node *n);
    If *_visitIf(Poco::XML::Node *n);
    IfAlt *_visitIfAlt(Poco::XML::Node *n);
    IfGenerate *_visitIfGenerate(Poco::XML::Node *n);
    Instance *_visitInstance(Poco::XML::Node *n);
    Int *_visitInt(Poco::XML::Node *n);
    IntValue *_visitIntValue(Poco::XML::Node *n);
    Library *_visitLibrary(Poco::XML::Node *n);
    LibraryDef *_visitLibraryDef(Poco::XML::Node *n);
    Member *_visitMember(Poco::XML::Node *n);
    Identifier *_visitIdentifier(Poco::XML::Node *n);
    Continue *_visitContinue(Poco::XML::Node *n);
    Null *_visitNull(Poco::XML::Node *n);
    Transition *_visitTransition(Poco::XML::Node *n);
    Parameter *_visitParameter(Poco::XML::Node *n);
    ParameterAssign *_visitParameterAssign(Poco::XML::Node *n);
    ProcedureCall *_visitProcedureCall(Poco::XML::Node *n);
    Pointer *_visitPointer(Poco::XML::Node *n);
    PortAssign *_visitPortAssign(Poco::XML::Node *n);
    Port *_visitPort(Poco::XML::Node *n);
    Procedure *_visitProcedure(Poco::XML::Node *n);
    PPAssign *_visitPPAssign(Poco::XML::Node *n);
    Range *_visitRange(Poco::XML::Node *n);
    Real *_visitReal(Poco::XML::Node *n);
    RealValue *_visitRealValue(Poco::XML::Node *n);
    Record *_visitRecord(Poco::XML::Node *n);
    RecordValue *_visitRecordValue(Poco::XML::Node *n);
    RecordValueAlt *_visitRecordValueAlt(Poco::XML::Node *n);
    Reference *_visitReference(Poco::XML::Node *n);
    Return *_visitReturn(Poco::XML::Node *n);
    Signal *_visitSignal(Poco::XML::Node *n);
    Signed *_visitSigned(Poco::XML::Node *n);
    Slice *_visitSlice(Poco::XML::Node *n);
    State *_visitState(Poco::XML::Node *n);
    StateTable *_visitStateTable(Poco::XML::Node *n);
    String *_visitString(Poco::XML::Node *n);
    Switch *_visitSwitch(Poco::XML::Node *n);
    SwitchAlt *_visitSwitchAlt(Poco::XML::Node *n);
    System *_visitSystem(Poco::XML::Node *n);
    StringValue *_visitStringValue(Poco::XML::Node *n);
    Time *_visitTime(Poco::XML::Node *n);
    TimeValue *_visitTimeValue(Poco::XML::Node *n);
    TPAssign *_visitTPAssign(Poco::XML::Node *n);
    Type *_visitType(Poco::XML::Node *n);
    ReferencedType *_visitReferencedType(Poco::XML::Node *n);
    TypeDef *_visitTypeDef(Poco::XML::Node *n);
    TypedObject *_visitTypedObject(Poco::XML::Node *n);
    TypeTP *_visitTypeTP(Poco::XML::Node *n);
    TypeReference *_visitTypeReference(Poco::XML::Node *n);
    TypeTPAssign *_visitTypeTPAssign(Poco::XML::Node *n);
    Unsigned *_visitUnsigned(Poco::XML::Node *n);
    Value *_visitValue(Poco::XML::Node *n);
    ValueStatement *_visitValueStatement(Poco::XML::Node *n);
    ValueTP *_visitValueTP(Poco::XML::Node *n);
    ValueTPAssign *_visitValueTPAssign(Poco::XML::Node *n);
    Variable *_visitVariable(Poco::XML::Node *n);
    View *_visitView(Poco::XML::Node *n);
    ViewReference *_visitViewReference(Poco::XML::Node *n);
    Wait *_visitWait(Poco::XML::Node *n);
    When *_visitWhen(Poco::XML::Node *n);
    WhenAlt *_visitWhenAlt(Poco::XML::Node *n);
    While *_visitWhile(Poco::XML::Node *n);
    With *_visitWith(Poco::XML::Node *n);
    WithAlt *_visitWithAlt(Poco::XML::Node *n);

    /// Parse element \p n that has a type object as child.
    Type *_visitInnerType(Poco::XML::Node *n);

    /// Parse element \p n that has a value object as child.
    Value *_visitInnerValue(Poco::XML::Node *n);

    /// Parse element \p n that has a typed object as child.
    TypedObject *_visitInnerTypedObject(Poco::XML::Node *n);

    /// Check if node has only 1 child, if number is different
    /// print an error. Il \p s is set, check also that element has \p s
    /// as tag name.
    void _checkExactlyOneChild(Poco::XML::Node *n, const std::string &s = "", Object *obj = nullptr);

    /// Add code line info (if present) to the object \p o. After that, code info
    /// childs are removed.
    void _addCodeInfoPropertiesComments(Poco::XML::Node *n, Object *o);

    /// Return true if element name s is a name of a action object element.
    bool _isActionElement(const std::string &s);

    /// Return true if element name s is a name of a declaration object element.
    bool _isDataDeclElement(const std::string &s);

    /// Return true if element name s is a name of a declaration object element.
    bool _isDeclElement(const std::string &s);

    /// Return true if element name s is a name of a generate object element.
    bool _isGenerateElement(const std::string &s);

    /// Return true if element name s is a name of a PPAssign object element.
    bool _isPPAssignElement(const std::string &s);

    /// Return true if element name s is a name of a type object element.
    bool _isTypeElement(const std::string &s);

    /// Return true if element name s is a name of a value object element.
    bool _isValueElement(const std::string &s);
    bool _isConstValue(const std::string &s);
    bool _isPrefixedReference(const std::string &s);

    /// Return true if element name s is a name of a TPAssign object element.
    bool _isTPAssignElement(const std::string &s);

    /// Return true if element name s is a name of a Viewref object element.
    bool _isViewrefElement(const std::string &s);

    /// Return true if element name s is a name of a TypedObject object element.
    bool _isTypedObjectElement(const std::string &s);

    /// Base call for all getAttribute functions.
    Poco::XML::XMLString _getAttributeByName(Poco::XML::Node *n, const std::string &s, bool required = true);

    /// Check whether given nodehas an attribute with specified name.
    bool _hasAttributeWithName(Poco::XML::Node *n, const std::string &s);

    /// Return a string corresponding to the value of attribute \p s, nullptr if
    /// Attribute is not found.
    /// If attribute is required, print and exit with error if it is not found.
    std::string _getStringAttributeByName(
        Poco::XML::Node *n,
        const std::string &s,
        bool required                   = true,
        const std::string &defaultValue = "");

    /// Return a bool corresponding to the value of attribute \p s, nullptr if
    /// Attribute is not found.
    /// If attribute is required, print and exit with error if it is not found.
    bool _getBoolAttributeByName(Poco::XML::Node *n, const std::string &s, bool required = false);

    /// Return a int corresponding to the value of attribute \p s, nullptr if
    /// Attribute is not found.
    /// If attribute is required, print and exit with error if it is not found.
    std::int64_t _getIntAttributeByName(Poco::XML::Node *n, const std::string &s, bool required = false);

    /// Return a int corresponding to the value of attribute \p s, nullptr if
    /// Attribute is not found.
    /// If attribute is required, print and exit with error if it is not found.
    double _getDoubleAttributeByName(Poco::XML::Node *n, const std::string &s, bool required = false);

    /// Returns the typeVariant enum value corresponding to the value of attribute
    /// \p s.
    /// If attribute is required, print and exit with error if it is not found.
    Type::TypeVariant _getTypeVariantAttributeByName(Poco::XML::Node *n, const std::string &s, bool required = false);

    /// Returns the typeVariant enum value corresponding to the value of attribute
    /// \p s.
    /// If attribute is required, print and exit with error if it is not found.
    bool _getConstexprAttributeByName(Poco::XML::Node *n, const std::string &s, bool required = true);

    Object *_visitGenericObject(Poco::XML::Node *n);

    void _visitDataDeclImpl(DataDeclaration *d, Poco::XML::Node *n);

    typedef std::set<LibraryDef *> LibrarySet;
    static LibrarySet _stdLibrarySet;

    hif::semantics::ILanguageSemantics *_sem;

    System::VersionInfo::VersionNumber _formatVersionMajor;
    System::VersionInfo::VersionNumber _formatVersionMinor;

private:
    XmlParser(const XmlParser &);
    XmlParser &operator=(const XmlParser &);
};

XmlParser::LibrarySet XmlParser::_stdLibrarySet;

XmlParser::XmlParser(Object *&o, std::istream &in, hif::semantics::ILanguageSemantics *sem)
    : _sem(sem)
    , _formatVersionMajor(0)
    , _formatVersionMinor(0)
{
    hif::application_utils::initializeLogHeader("HIF", "XML_PARSER");

    if (in.fail()) {
        messageError("No such file or directory.", nullptr, nullptr);
    }

    Poco::XML::InputSource src(in);
    try {
        Poco::XML::DOMParser parser;
        parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);

        Poco::XML::AutoPtr<Poco::XML::Document> pDoc = parser.parse(&src);
        Poco::XML::NodeIterator docIt(pDoc, static_cast<unsigned long>(Poco::XML::NodeFilter::SHOW_ALL));

        Poco::XML::NodeList *l = pDoc->childNodes();
        o                      = _visitGenericObject(l->item(0));
        l->release();
    } catch (Poco::Exception &e) {
        messageError(e.displayText(), nullptr, nullptr);
    }

    messageInfo("Parsed input file.");
    hif::application_utils::restoreLogHeader();
}

XmlParser::~XmlParser()
{
    // ntd
}

Aggregate *XmlParser::_visitAggregate(Poco::XML::Node *n)
{
    Aggregate *ret = new Aggregate();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "ALTS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "ALT") {
                    ret->alts.push_back(_visitAggregateAlt(ll->item(j)));
                } else
                    messageError("Unexpected for aggregate->alts child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "OTHERS") {
            ret->setOthers(_visitInnerValue(l->item(i)));
        } else
            messageError("Unexpected aggregate child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
AggregateAlt *XmlParser::_visitAggregateAlt(Poco::XML::Node *n)
{
    AggregateAlt *ret = new AggregateAlt();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "VALUE")
            ret->setValue(_visitInnerValue(l->item(i)));
        else if (l->item(i)->nodeName() == "INDICES") {
            // get values
            Poco::XML::NodeList *vl = l->item(i)->childNodes();
            for (unsigned int j = 0; j < vl->length(); ++j) {
                if (_isValueElement(vl->item(j)->nodeName())) {
                    ret->indices.push_back(_visitValue(vl->item(j)));
                } else
                    messageError("Unexpected Aggregate->Values child " + vl->item(j)->nodeName(), ret, nullptr);
            }
            vl->release();
        } else
            messageError("Unexpected aggregate alt child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Alias *XmlParser::_visitAlias(Poco::XML::Node *n)
{
    Alias *ret = new Alias();

    /// PARENT PARSING
    _visitDataDeclImpl(ret, n);

    /// CODE INFO
    /// ATTRIBUTES
    ret->setStandard(_getBoolAttributeByName(n, "standard", false));

    /// CHILDREN

    return ret;
}
Array *XmlParser::_visitArray(Poco::XML::Node *n)
{
    Array *ret = new Array();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setSigned(_getBoolAttributeByName(n, "signed"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "TYPE") {
            ret->setType(_visitInnerType(l->item(i)));
        } else if (l->item(i)->nodeName() == "RANGE") {
            ret->setSpan(_visitRange(l->item(i)));
        } else
            messageError("Unexpected array child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Assign *XmlParser::_visitAssign(Poco::XML::Node *n)
{
    Assign *ret = new Assign();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "LEFTHANDSIDE") {
            ret->setLeftHandSide(_visitInnerValue(l->item(i)));
        } else if (l->item(i)->nodeName() == "RIGHTHANDSIDE") {
            ret->setRightHandSide(_visitInnerValue(l->item(i)));
        } else if (l->item(i)->nodeName() == "DELAY") {
            ret->setDelay(_visitInnerValue(l->item(i)));
        } else
            messageError("Unexpected assign child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Bit *XmlParser::_visitBit(Poco::XML::Node *n)
{
    Bit *ret = new Bit();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setLogic(_getBoolAttributeByName(n, "logic", true));
    ret->setResolved(_getBoolAttributeByName(n, "resolved", true));
    ret->setConstexpr(_getConstexprAttributeByName(n, "constexpr"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    messageAssert(l->length() == 0, "Unexpected bit child " + l->item(0)->nodeName(), ret, nullptr);
    l->release();

    return ret;
}
BitValue *XmlParser::_visitBitval(Poco::XML::Node *n)
{
    BitValue *ret = new BitValue();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    std::string value(_getStringAttributeByName(n, "value"));
    ret->setValue(bitConstantFromString(value));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "TYPE") {
            ret->setType(_visitInnerType(n->firstChild()));
        } else
            messageError("Unexpected bitval child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Bitvector *XmlParser::_visitBitvector(Poco::XML::Node *n)
{
    Bitvector *ret = new Bitvector();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setLogic(_getBoolAttributeByName(n, "logic", true));
    ret->setSigned(_getBoolAttributeByName(n, "signed", true));
    ret->setResolved(_getBoolAttributeByName(n, "resolved", true));
    ret->setConstexpr(_getConstexprAttributeByName(n, "constexpr"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant", true));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "RANGE") {
            ret->setSpan(_visitRange(l->item(i)));
        } else
            messageError("Unexpected bit vector child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
BitvectorValue *XmlParser::_visitBitvectorValue(Poco::XML::Node *n)
{
    BitvectorValue *ret = new BitvectorValue();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setValue(_getStringAttributeByName(n, "VALUE"));

    /// CHILDREN
    _checkExactlyOneChild(n, "TYPE", ret);
    ret->setType(_visitInnerType(n->firstChild()));

    return ret;
}
Bool *XmlParser::_visitBool(Poco::XML::Node *n)
{
    Bool *ret = new Bool();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setConstexpr(_getBoolAttributeByName(n, "constexpr"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    messageAssert(l->length() == 0, "Unexpected bool child " + l->item(0)->nodeName(), ret, nullptr);
    l->release();

    return ret;
}
BoolValue *XmlParser::_visitBoolValue(Poco::XML::Node *n)
{
    BoolValue *ret = new BoolValue();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setValue(_getBoolAttributeByName(n, "value"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "TYPE") {
            ret->setType(_visitInnerType(n->firstChild()));
        } else
            messageError("Unexpected boolval child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
If *XmlParser::_visitIf(Poco::XML::Node *n)
{
    If *ret = new If();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "ALT") {
            ret->alts.push_back(_visitIfAlt(l->item(i)));
        } else if (l->item(i)->nodeName() == "DEFAULTS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName())) {
                    ret->defaults.push_back(_visitAction(ll->item(j)));
                } else
                    messageError("Unexpected case->default child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected case child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
IfAlt *XmlParser::_visitIfAlt(Poco::XML::Node *n)
{
    IfAlt *ret = new IfAlt();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "CONDITION") {
            ret->setCondition(_visitInnerValue(l->item(i)));
        } else if (_isActionElement(l->item(i)->nodeName())) {
            ret->actions.push_back(_visitAction(l->item(i)));
        } else
            messageError("Unexpected case alt child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Cast *XmlParser::_visitCast(Poco::XML::Node *n)
{
    Cast *ret = new Cast();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "VALUE" || (l->item(i)->nodeName() == "OP" && _formatVersionMajor < 1)) {
            ret->setValue(_visitInnerValue(l->item(i)));
        } else if (l->item(i)->nodeName() == "TYPE") {
            ret->setType(_visitInnerType(l->item(i)));
        } else
            messageError("Unexpected cast child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}

Char *XmlParser::_visitChar(Poco::XML::Node *n)
{
    Char *ret = new Char();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setConstexpr(_getBoolAttributeByName(n, "constexpr"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN

    return ret;
}
CharValue *XmlParser::_visitCharValue(Poco::XML::Node *n)
{
    CharValue *ret = new CharValue();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    std::string value = _getStringAttributeByName(n, "value");
    if (value == "'NUL'")
        ret->setValue('\0');
    else if (value == "'LF'")
        ret->setValue('\n');
    else
        ret->setValue(value.at(static_cast<size_t>(0)));

    /// CHILDREN
    _checkExactlyOneChild(n, "TYPE", ret);
    ret->setType(_visitInnerType(n->firstChild()));

    return ret;
}
Const *XmlParser::_visitConst(Poco::XML::Node *n)
{
    Const *ret = new Const();

    /// PARENT PARSING
    _visitDataDeclImpl(ret, n);

    /// CODE INFO
    /// ATTRIBUTES
    ret->setInstance(_getBoolAttributeByName(n, "instance"));
    ret->setDefine(_getBoolAttributeByName(n, "define"));
    ret->setStandard(_getBoolAttributeByName(n, "standard", false));

    /// CHILDREN

    return ret;
}
Contents *XmlParser::_visitContents(Poco::XML::Node *n)
{
    Contents *ret = new Contents();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    // is optional
    ret->setName(_getStringAttributeByName(n, "name", false));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "LIBRARIES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "LIBRARY") {
                    ret->libraries.push_back(_visitLibrary(ll->item(j)));
                } else
                    messageError("Unexpected contents->library child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "DECLARATIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDeclElement(ll->item(j)->nodeName())) {
                    ret->declarations.push_back(_visitDeclaration(ll->item(j)));
                } else
                    messageError("Unexpected contents->declarations child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "STATETABLES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "STATETABLE") {
                    ret->stateTables.push_back(_visitStateTable(ll->item(j)));
                } else
                    messageError("Unexpected contents->stateTables child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "GENERATES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isGenerateElement(ll->item(j)->nodeName())) {
                    ret->generates.push_back(_visitGenerate(ll->item(j)));
                } else
                    messageError("Unexpected contents->generates child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "INSTANCES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "INSTANCE") {
                    ret->instances.push_back(_visitInstance(ll->item(j)));
                } else
                    messageError("Unexpected contents->instance child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "GLOBALACTION") {
            ret->setGlobalAction(_visitGlobalAction(l->item(i)));
        } else
            messageError("Unexpected content child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}

DataDeclaration *XmlParser::_visitDataDeclaration(Poco::XML::Node *n)
{
    DataDeclaration *ret         = nullptr;
    std::string decl_name = n->nodeName();

    if (decl_name == "ALIAS")
        ret = _visitAlias(n);
    else if (decl_name == "CONSTANT")
        ret = _visitConst(n);
    else if (decl_name == "ENUMVAL")
        ret = _visitEnumValue(n);
    else if (decl_name == "PARAMETER")
        ret = _visitParameter(n);
    else if (decl_name == "PORT")
        ret = _visitPort(n);
    else if (decl_name == "SIGNAL")
        ret = _visitSignal(n);
    else if (decl_name == "VALUETP")
        ret = _visitValueTP(n);
    else if (decl_name == "VARIABLE")
        ret = _visitVariable(n);
    else
        messageError("Unexpected or deprecated data decl node: " + decl_name, ret, nullptr);

    return ret;
}

Declaration *XmlParser::_visitDeclaration(Poco::XML::Node *n)
{
    Declaration *ret             = nullptr;
    std::string decl_name = n->nodeName();

    if (_isDataDeclElement(decl_name))
        ret = _visitDataDeclaration(n);
    else if (decl_name == "CONTENTS")
        ret = _visitContents(n);
    else if (decl_name == "FORGENERATE")
        ret = _visitForGenerate(n);
    else if (decl_name == "IFGENERATE")
        ret = _visitIfGenerate(n);
    else if (decl_name == "DESIGNUNIT")
        ret = _visitDesignUnit(n);
    else if (decl_name == "LIBRARYDEF")
        ret = _visitLibraryDef(n);
    else if (decl_name == "FUNCTION")
        ret = _visitFunction(n);
    else if (decl_name == "PROCEDURE")
        ret = _visitProcedure(n);
    else if (decl_name == "TYPEDEF")
        ret = _visitTypeDef(n);
    else if (decl_name == "TYPETP")
        ret = _visitTypeTP(n);
    else if (decl_name == "STATE")
        ret = _visitState(n);
    else
        messageError("Unexpected or deprecated decl node:" + decl_name, ret, nullptr);

    return ret;
}
DesignUnit *XmlParser::_visitDesignUnit(Poco::XML::Node *n)
{
    DesignUnit *ret = new DesignUnit();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "VIEW") {
            ret->views.push_back(_visitView(l->item(i)));
        } else
            messageError("Unexpected DU child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Enum *XmlParser::_visitEnum(Poco::XML::Node *n)
{
    Enum *ret = new Enum();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "ENUMVAL") {
            ret->values.push_back(_visitEnumValue(l->item(i)));
        } else
            messageError("Unexpected enum child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
EnumValue *XmlParser::_visitEnumValue(Poco::XML::Node *n)
{
    EnumValue *ret = new EnumValue();

    /// PARENT PARSING
    _visitDataDeclImpl(ret, n);

    /// CODE INFO
    /// ATTRIBUTES
    /// CHILDREN

    return ret;
}
Break *XmlParser::_visitBreak(Poco::XML::Node *n)
{
    Break *ret = new Break();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN

    return ret;
}

Event *XmlParser::_visitEvent(Poco::XML::Node *n)
{
    Event *ret = new Event();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setConstexpr(_getBoolAttributeByName(n, "constexpr"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN

    return ret;
}

Expression *XmlParser::_visitExpression(Poco::XML::Node *n)
{
    Expression *ret = new Expression();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    if (_formatVersionMajor < 1) {
        std::string value(_getStringAttributeByName(n, "op", false));
        if (value.empty())
            value = _getStringAttributeByName(n, "operator");
        ret->setOperator(operatorFromString(value));
    } else {
        std::string value(_getStringAttributeByName(n, "operator"));
        ret->setOperator(operatorFromString(value));
    }

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "VALUE1" || (l->item(i)->nodeName() == "OP1" && _formatVersionMajor < 1)) {
            ret->setValue1(_visitInnerValue(l->item(i)));
        } else if (l->item(i)->nodeName() == "VALUE2" || (l->item(i)->nodeName() == "OP2" && _formatVersionMajor < 1)) {
            ret->setValue2(_visitInnerValue(l->item(i)));
        } else
            messageError("Unexpected expr child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
FunctionCall *XmlParser::_visitFunctionCall(Poco::XML::Node *n)
{
    FunctionCall *ret = new FunctionCall();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (_isTPAssignElement(l->item(i)->nodeName())) {
            ret->templateParameterAssigns.push_back(_visitTPAssign(l->item(i)));
        } else if (_isPPAssignElement(l->item(i)->nodeName())) {
            ret->parameterAssigns.push_back(_visitParameterAssign(l->item(i)));
        } else if (l->item(i)->nodeName() == "INSTANCE") {
            ret->setInstance(_visitInnerValue(l->item(i)));
        } else
            messageError("Unexpected fcall child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Field *XmlParser::_visitField(Poco::XML::Node *n)
{
    Field *ret = new Field();

    /// PARENT PARSING
    _visitDataDeclImpl(ret, n);

    /// CODE INFO
    /// ATTRIBUTES
    std::string value(_getStringAttributeByName(n, "direction", false));
    ret->setDirection(portDirectionFromString(value));
    /// CHILDREN

    return ret;
}
FieldReference *XmlParser::_visitFieldReference(Poco::XML::Node *n)
{
    FieldReference *ret = new FieldReference();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "fieldName"));

    /// CHILDREN
    _checkExactlyOneChild(n, "PREFIX", ret);
    ret->setPrefix(_visitInnerValue(n->firstChild()));

    return ret;
}
File *XmlParser::_visitFile(Poco::XML::Node *n)
{
    File *ret = new File();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (_isTypeElement(l->item(i)->nodeName())) {
            ret->setType(_visitType(l->item(i)));
        } else
            messageError("Unexpected file child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
For *XmlParser::_visitFor(Poco::XML::Node *n)
{
    For *ret = new For();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "label"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "INITDECLARATIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDeclElement(ll->item(j)->nodeName())) {
                    ret->initDeclarations.push_back(_visitDataDeclaration(ll->item(j)));
                } else
                    messageError("Unexpected for->initDeclarations child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "INITVALUES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName())) {
                    ret->initValues.push_back(_visitAction(ll->item(j)));
                } else
                    messageError("Unexpected for->initValues child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "CONDITION") {
            // Check if has VHDL range notation
            if (l->item(i)->firstChild()->nodeName() == "RANGE") {
                ret->setCondition(_visitRange(l->item(i)->firstChild()));
            } else {
                ret->setCondition(_visitInnerValue(l->item(i)));
            }
        } else if (l->item(i)->nodeName() == "STEPACTIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName())) {
                    ret->stepActions.push_back(_visitAction(ll->item(j)));
                } else
                    messageError("Unexpected for->stepActions child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "ACTIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName())) {
                    ret->forActions.push_back(_visitAction(ll->item(j)));
                } else
                    messageError("Unexpected for->forActions child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected for child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
ForGenerate *XmlParser::_visitForGenerate(Poco::XML::Node *n)
{
    ForGenerate *ret = new ForGenerate();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "GLOBALACTION") {
            ret->setGlobalAction(_visitGlobalAction(l->item(i)));
        } else if (l->item(i)->nodeName() == "CONDITION") {
            // Check if has VHDL range notation
            if (l->item(i)->firstChild()->nodeName() == "RANGE") {
                ret->setCondition(_visitRange(l->item(i)->firstChild()));
            } else {
                ret->setCondition(_visitInnerValue(l->item(i)));
            }
        } else if (l->item(i)->nodeName() == "DECLARATIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDeclElement(ll->item(j)->nodeName())) {
                    ret->declarations.push_back(_visitDeclaration(ll->item(j)));
                } else
                    messageError(
                        "Unexpected for generate->declarations child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "STATETABLES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "STATETABLE") {
                    ret->stateTables.push_back(_visitStateTable(ll->item(j)));
                } else
                    messageError(
                        "Unexpected for generate->state tables child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "GENERATES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isGenerateElement(ll->item(j)->nodeName())) {
                    ret->generates.push_back(_visitGenerate(ll->item(j)));
                } else
                    messageError("Unexpected for generate->generates child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "INSTANCES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "INSTANCE") {
                    ret->instances.push_back(_visitInstance(ll->item(j)));
                } else
                    messageError("Unexpected for generate->instance child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "INITDECLARATIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDeclElement(ll->item(j)->nodeName())) {
                    ret->initDeclarations.push_back(_visitDataDeclaration(ll->item(j)));
                } else
                    messageError(
                        "Unexpected for generate->initDeclarations child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "INITVALUES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName())) {
                    ret->initValues.push_back(_visitAction(ll->item(j)));
                } else
                    messageError("Unexpected for generate->initValues child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "STEPACTIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName())) {
                    ret->stepActions.push_back(_visitAction(ll->item(j)));
                } else
                    messageError("Unexpected for generate->stepActions child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected for generate child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Function *XmlParser::_visitFunction(Poco::XML::Node *n)
{
    Function *ret = new Function();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name", true));
    ret->setStandard(_getBoolAttributeByName(n, "standard", false));
    std::string kind = _getStringAttributeByName(n, "kind");
    ret->setKind(SubProgram::kindFromString(kind));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "TYPE") {
            ret->setType(_visitInnerType(l->item(i)));
        } else if (l->item(i)->nodeName() == "STATETABLE") {
            ret->setStateTable(_visitStateTable(l->item(i)));
        } else if (l->item(i)->nodeName() == "TEMPLATE_PARAMETERS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDeclElement(ll->item(j)->nodeName())) {
                    ret->templateParameters.push_back(_visitDeclaration(ll->item(j)));
                } else
                    messageError(
                        "Unexpected for function->template params child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "PARAMETERS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDataDeclElement(ll->item(j)->nodeName())) {
                    ret->parameters.push_back(_visitParameter(ll->item(j)));
                } else
                    messageError("Unexpected for function->parameters child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected function child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Generate *XmlParser::_visitGenerate(Poco::XML::Node *n)
{
    Generate *ret          = nullptr;
    std::string gen = n->nodeName();

    if (gen == "FORGENERATE") {
        ret = _visitForGenerate(n);
    } else if (gen == "IFGENERATE") {
        ret = _visitIfGenerate(n);
    } else
        messageError("Generate: Unexpected or deprecated node: " + gen, ret, nullptr);

    return ret;
}
GlobalAction *XmlParser::_visitGlobalAction(Poco::XML::Node *n)
{
    GlobalAction *ret = new GlobalAction();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (_isActionElement(l->item(i)->nodeName())) {
            ret->actions.push_back(_visitAction(l->item(i)));
        } else
            messageError("Unexpected global child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Entity *XmlParser::_visitEntity(Poco::XML::Node *n)
{
    Entity *ret = new Entity();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name", true));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "PARAMETER") {
            ret->parameters.push_back(_visitParameter(l->item(i)));
        } else if (l->item(i)->nodeName() == "PORT") {
            ret->ports.push_back(_visitPort(l->item(i)));
        } else
            messageError("Unexpected Entity child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
IfGenerate *XmlParser::_visitIfGenerate(Poco::XML::Node *n)
{
    IfGenerate *ret = new IfGenerate();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name", true));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "GLOBALACTION") {
            ret->setGlobalAction(_visitGlobalAction(l->item(i)));
        } else if (l->item(i)->nodeName() == "CONDITION") {
            ret->setCondition(_visitInnerValue(l->item(i)));
        } else if (l->item(i)->nodeName() == "DECLARATIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDeclElement(ll->item(j)->nodeName())) {
                    ret->declarations.push_back(_visitDeclaration(ll->item(j)));
                } else
                    messageError("Unexpected if generate->declarations child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "STATETABLES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "STATETABLE") {
                    ret->stateTables.push_back(_visitStateTable(ll->item(j)));
                } else
                    messageError("Unexpected if generate->state tables child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "GENERATES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isGenerateElement(ll->item(j)->nodeName())) {
                    ret->generates.push_back(_visitGenerate(ll->item(j)));
                } else
                    messageError("Unexpected if generate->generates child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "INSTANCES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "INSTANCE")
                    ret->instances.push_back(_visitInstance(ll->item(j)));
                else
                    messageError("Unexpected if generate->instances child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected if generate child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Instance *XmlParser::_visitInstance(Poco::XML::Node *n)
{
    Instance *ret = new Instance();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "VIEWREFERENCE" || l->item(i)->nodeName() == "LIBRARY" ||
            l->item(i)->nodeName() == "TYPEREFERENCE") {
            ret->setReferencedType(_visitReferencedType(l->item(i)));
        } else if (l->item(i)->nodeName() == "VALUE") {
            ret->setValue(_visitInnerValue(l->item(i)));
        } else if (
            l->item(i)->nodeName() == "PORTASSIGNS" ||
            (l->item(i)->nodeName() == "PORTASSIGN_LIST" && _formatVersionMajor < 1)) {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "PORTASSIGN") {
                    ret->portAssigns.push_back(_visitPortAssign(ll->item(j)));
                } else
                    messageError("Unexpected instance->portassign list child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected instance child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Int *XmlParser::_visitInt(Poco::XML::Node *n)
{
    Int *ret = new Int();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setSigned(_getBoolAttributeByName(n, "signed"));
    ret->setConstexpr(_getBoolAttributeByName(n, "constexpr"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "SPAN") {
            ret->setSpan(_visitRange(l->item(i)));
        } else
            messageError("Unexpected int child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
IntValue *XmlParser::_visitIntValue(Poco::XML::Node *n)
{
    IntValue *ret = new IntValue();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setValue(_getIntAttributeByName(n, "value"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "TYPE") {
            ret->setType(_visitInnerType(l->item(i)));
        } else
            messageError("Unexpected intval child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Library *XmlParser::_visitLibrary(Poco::XML::Node *n)
{
    Library *ret = new Library();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));
    if (_formatVersionMajor >= 2)
        ret->setFilename(_getStringAttributeByName(n, "filename", false).c_str());
    else
        ret->setFilename(_getStringAttributeByName(n, "Filename", false).c_str());
    ret->setStandard(_getBoolAttributeByName(n, "standard", false));
    ret->setSystem(_getBoolAttributeByName(n, "system", false));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant", true));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "INSTANCE") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "LIBRARY")
                    ret->setInstance(_visitReferencedType(ll->item(j)));
                else
                    messageError("Unexpected type library->instance child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected library child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
LibraryDef *XmlParser::_visitLibraryDef(Poco::XML::Node *n)
{
    LibraryDef *ret = new LibraryDef();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name", true));
    ret->setStandard(_getBoolAttributeByName(n, "standard", false));
    ret->setCLinkage(_getBoolAttributeByName(n, "clinkage", false));

    std::string langID = _getStringAttributeByName(n, "languageId");
    ret->setLanguageID(languageIDFromString(langID));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "LIBRARY") {
            ret->libraries.push_back(_visitLibrary(l->item(i)));
        } else if (_isDeclElement(l->item(i)->nodeName())) {
            ret->declarations.push_back(_visitDeclaration(l->item(i)));
        } else
            messageError("Unexpected library def child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    if (ret->isStandard() && ret->declarations.empty() && ret->libraries.empty()) {
        messageAssert(_sem != nullptr, "Expected semantics", nullptr, nullptr);
        LibraryDef *tmp = _sem->getStandardLibrary(ret->getName());
        //messageAssert(tmp != nullptr, "Unknown standard library", ret, _sem);
        if (tmp == nullptr) {
            return ret;
        }
        delete ret;
        ret = tmp;

        // Standard libraryDefs returned by aliasing
        if (_stdLibrarySet.find(tmp) == _stdLibrarySet.end()) {
            _stdLibrarySet.insert(tmp);
        } else {
            ret = hif::copy(tmp);
        }
    }

    return ret;
}
Member *XmlParser::_visitMember(Poco::XML::Node *n)
{
    Member *ret = new Member();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "PREFIX") {
            ret->setPrefix(_visitInnerValue(l->item(i)));
        } else if (l->item(i)->nodeName() == "INDEX") {
            ret->setIndex(_visitInnerValue(l->item(i)));
        } else
            messageError("Unexpected Member child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Identifier *XmlParser::_visitIdentifier(Poco::XML::Node *n)
{
    Identifier *ret = new Identifier();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    //ret->SetIndex(  _getStringAttributeByName( n, "index" ).c_str() ) );
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN

    return ret;
}
Continue *XmlParser::_visitContinue(Poco::XML::Node *n)
{
    Continue *ret = new Continue();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN

    return ret;
}

Null *XmlParser::_visitNull(Poco::XML::Node *n)
{
    Null *ret = new Null();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    /// CHILDREN

    return ret;
}
Transition *XmlParser::_visitTransition(Poco::XML::Node *n)
{
    Transition *ret = new Transition();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setPrevName(_getStringAttributeByName(n, "source"));
    ret->setName(_getStringAttributeByName(n, "destination"));
    ret->setEnablingOrCondition(_getBoolAttributeByName(n, "orMode"));
    ret->setPriority(static_cast<unsigned long int>(_getIntAttributeByName(n, "priority")));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "ENABLING_LABEL") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isValueElement(ll->item(j)->nodeName())) {
                    ret->enablingLabelList.push_back(_visitValue(ll->item(j)));
                } else
                    messageError(
                        "Unexpected transition->enabling label child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "ENABLING") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isValueElement(ll->item(j)->nodeName()))
                    ret->enablingList.push_back(_visitValue(ll->item(j)));
                else
                    messageError("Unexpected transition->enabling child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "UPDATE_LABEL") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isValueElement(ll->item(j)->nodeName()))
                    ret->updateLabelList.push_back(_visitValue(ll->item(j)));
                else
                    messageError("Unexpected transition->update label child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "UPDATE") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName()))
                    ret->updateList.push_back(_visitAction(ll->item(j)));
                else
                    messageError("Unexpected transition->update child " + ll->item(j)->nodeName(), ret, nullptr);
            }
        } else
            messageError("Unexpected content child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Parameter *XmlParser::_visitParameter(Poco::XML::Node *n)
{
    Parameter *ret = new Parameter();

    /// PARENT PARSING
    _visitDataDeclImpl(ret, n);

    /// CODE INFO

    /// ATTRIBUTES
    std::string value(_getStringAttributeByName(n, "direction"));
    ret->setDirection(portDirectionFromString(value));

    /// CHILDREN

    return ret;
}
ParameterAssign *XmlParser::_visitParameterAssign(Poco::XML::Node *n)
{
    ParameterAssign *ret = new ParameterAssign();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));
    std::string value(_getStringAttributeByName(n, "direction"));
    ret->setDirection(portDirectionFromString(value));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "VALUE") {
            ret->setValue(_visitInnerValue(l->item(i)));
        } else
            messageError("Unexpected Paramass child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
ProcedureCall *XmlParser::_visitProcedureCall(Poco::XML::Node *n)
{
    ProcedureCall *ret = new ProcedureCall();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (_isTPAssignElement(l->item(i)->nodeName())) {
            ret->templateParameterAssigns.push_back(_visitTPAssign(l->item(i)));
        } else if (_isPPAssignElement(l->item(i)->nodeName())) {
            ret->parameterAssigns.push_back(_visitParameterAssign(l->item(i)));
        } else if (l->item(i)->nodeName() == "INSTANCE") {
            ret->setInstance(_visitInnerValue(l->item(i)));
        } else
            messageError("Unexpected pcall child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Pointer *XmlParser::_visitPointer(Poco::XML::Node *n)
{
    Pointer *ret = new Pointer();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (_isTypeElement(l->item(i)->nodeName()))
            ret->setType(_visitType(l->item(i)));
        else
            messageError("Unexpected pointer child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
PortAssign *XmlParser::_visitPortAssign(Poco::XML::Node *n)
{
    PortAssign *ret = new PortAssign();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));
    std::string value(_getStringAttributeByName(n, "direction"));
    ret->setDirection(portDirectionFromString(value));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "VALUE")
            ret->setValue(_visitInnerValue(l->item(i)));
        else if (l->item(i)->nodeName() == "TYPE")
            ret->setType(_visitInnerType(l->item(i)));
        else
            messageError("Unexpected Portassing child " + l->item(i)->nodeName(), ret, nullptr);
        // Note: partial bind should never be printed
    }
    l->release();

    return ret;
}
Port *XmlParser::_visitPort(Poco::XML::Node *n)
{
    Port *ret = new Port();

    /// PARENT PARSING
    _visitDataDeclImpl(ret, n);

    /// CODE INFO

    /// ATTRIBUTES
    std::string value(_getStringAttributeByName(n, "direction"));
    ret->setDirection(portDirectionFromString(value));
    ret->setWrapper(_getBoolAttributeByName(n, "wrapper", false));

    /// CHILDREN

    return ret;
}
Procedure *XmlParser::_visitProcedure(Poco::XML::Node *n)
{
    Procedure *ret = new Procedure();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name", true));
    ret->setStandard(_getBoolAttributeByName(n, "standard", false));
    std::string kind = _getStringAttributeByName(n, "kind");
    ret->setKind(SubProgram::kindFromString(kind));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "STATETABLE")
            ret->setStateTable(_visitStateTable(l->item(i)));
        else if (l->item(i)->nodeName() == "TEMPLATE_PARAMETERS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDeclElement(ll->item(j)->nodeName()))
                    ret->templateParameters.push_back(_visitDeclaration(ll->item(j)));
                else
                    messageError(
                        "Unexpected for procedure->template params child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "PARAMETERS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDataDeclElement(ll->item(j)->nodeName()))
                    ret->parameters.push_back(_visitParameter(ll->item(j)));
                else
                    messageError("Unexpected for procedure->parameters child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected procedure child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
PPAssign *XmlParser::_visitPPAssign(Poco::XML::Node *n)
{
    PPAssign *ret = nullptr;

    std::string type_name = n->nodeName();
    if (type_name == "PARAMETERASSIGN")
        ret = _visitParameterAssign(n);
    else if (type_name == "PORTASSIGN ")
        ret = _visitPortAssign(n);
    else
        messageError("PPassign: Unexpected or deprecated node: " + type_name, ret, nullptr);

    return ret;
}
Range *XmlParser::_visitRange(Poco::XML::Node *n)
{
    Range *ret = new Range();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    std::string value(_getStringAttributeByName(n, "direction"));
    ret->setDirection(rangeDirectionFromString(value));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "LEFTBOUND")
            ret->setLeftBound(_visitInnerValue(l->item(i)));
        else if (l->item(i)->nodeName() == "RIGHTBOUND")
            ret->setRightBound(_visitInnerValue(l->item(i)));
        else if (l->item(i)->nodeName() == "LBOUND" && _formatVersionMajor < 1)
            ret->setLeftBound(_visitInnerValue(l->item(i)));
        else if (l->item(i)->nodeName() == "RBOUND" && _formatVersionMajor < 1)
            ret->setRightBound(_visitInnerValue(l->item(i)));
        else if (l->item(i)->nodeName() == "TYPE")
            ret->setType(_visitInnerType(l->item(i)));
        else
            messageError("Unexpected Range child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Real *XmlParser::_visitReal(Poco::XML::Node *n)
{
    Real *ret = new Real();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setConstexpr(_getBoolAttributeByName(n, "constexpr"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    _checkExactlyOneChild(n, "SPAN", ret);
    ret->setSpan(_visitRange(n->firstChild()));

    return ret;
}
RealValue *XmlParser::_visitRealValue(Poco::XML::Node *n)
{
    RealValue *ret = new RealValue();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setValue(_getDoubleAttributeByName(n, "value"));

    /// CHILDREN
    _checkExactlyOneChild(n, "TYPE", ret);
    ret->setType(_visitInnerType(n->firstChild()));

    return ret;
}
Record *XmlParser::_visitRecord(Poco::XML::Node *n)
{
    Record *ret = new Record();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setConstexpr(_getConstexprAttributeByName(n, "constexpr"));
    ret->setPacked(_getBoolAttributeByName(n, "packed", true));
    ret->setUnion(_getBoolAttributeByName(n, "union", true));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "FIELD")
            ret->fields.push_back(_visitField(l->item(i)));
        else
            messageError("Unexpected record child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
RecordValue *XmlParser::_visitRecordValue(Poco::XML::Node *n)
{
    RecordValue *ret = new RecordValue();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "ALTS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "RECORDVALUEALT")
                    ret->alts.push_back(_visitRecordValueAlt(ll->item(j)));
                else
                    messageError("Unexpected for record value->alts child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected record value child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
RecordValueAlt *XmlParser::_visitRecordValueAlt(Poco::XML::Node *n)
{
    RecordValueAlt *ret = new RecordValueAlt();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "VALUE")
            ret->setValue(_visitInnerValue(l->item(i)));
        else
            messageError("Unexpected record value alternative child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Reference *XmlParser::_visitReference(Poco::XML::Node *n)
{
    Reference *ret = new Reference();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (_isTypeElement(l->item(i)->nodeName()))
            ret->setType(_visitType(l->item(i)));
        else
            messageError("Unexpected reference child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Return *XmlParser::_visitReturn(Poco::XML::Node *n)
{
    Return *ret = new Return();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (_isValueElement(l->item(i)->nodeName()))
            ret->setValue(_visitValue(l->item(i)));
        else
            messageError("Unexpected return child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Signal *XmlParser::_visitSignal(Poco::XML::Node *n)
{
    Signal *ret = new Signal();

    /// PARENT PARSING
    _visitDataDeclImpl(ret, n);

    /// CODE INFO

    /// ATTRIBUTES
    ret->setStandard(_getBoolAttributeByName(n, "standard", false));
    ret->setWrapper(_getBoolAttributeByName(n, "wrapper", false));

    /// CHILDREN

    return ret;
}
Signed *XmlParser::_visitSigned(Poco::XML::Node *n)
{
    Signed *ret = new Signed();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setConstexpr(_getConstexprAttributeByName(n, "constexpr"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    _checkExactlyOneChild(n, "RANGE", ret);
    ret->setSpan(_visitRange(n->firstChild()));

    return ret;
}
Action *XmlParser::_visitAction(Poco::XML::Node *n)
{
    Action *ret            = nullptr;
    std::string obj = n->nodeName();

    if (obj == "ASSIGN")
        ret = _visitAssign(n);
    else if (obj == "IF")
        ret = _visitIf(n);
    else if (obj == "EXIT")
        ret = _visitBreak(n);
    else if (obj == "FOR")
        ret = _visitFor(n);
    else if (obj == "NEXT")
        ret = _visitContinue(n);
    else if (obj == "nullptr")
        ret = _visitNull(n);
    else if (obj == "TRANSITION")
        ret = _visitTransition(n);
    else if (obj == "PCALL")
        ret = _visitProcedureCall(n);
    else if (obj == "RETURN")
        ret = _visitReturn(n);
    else if (obj == "SWITCH")
        ret = _visitSwitch(n);
    else if (obj == "VALUESTATEMENT")
        ret = _visitValueStatement(n);
    else if (obj == "WAIT")
        ret = _visitWait(n);
    else if (obj == "WHILE")
        ret = _visitWhile(n);
    else
        messageError("Action: Unexpected or deprecated node: " + obj, ret, nullptr);

    return ret;
}
Slice *XmlParser::_visitSlice(Poco::XML::Node *n)
{
    Slice *ret = new Slice();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "RANGE")
            ret->setSpan(_visitRange(l->item(i)));
        else if (l->item(i)->nodeName() == "PREFIX")
            ret->setPrefix(_visitInnerValue(l->item(i)));
        else
            messageError("Unexpected Slice child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
State *XmlParser::_visitState(Poco::XML::Node *n)
{
    State *ret = new State();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));
    ret->setAtomic(_getBoolAttributeByName(n, "atomic"));
    ret->setPriority(static_cast<unsigned long int>(_getIntAttributeByName(n, "priority")));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "ACTIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName()))
                    ret->actions.push_back(_visitAction(ll->item(j)));
                else
                    messageError("Unexpected state->action child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "INVARIANTS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isValueElement(ll->item(j)->nodeName()))
                    ret->invariants.push_back(_visitValue(ll->item(j)));
                else
                    messageError("Unexpected state->invariant child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected state child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
StateTable *XmlParser::_visitStateTable(Poco::XML::Node *n)
{
    StateTable *ret = new StateTable();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));
    // flavour
    std::string flavour(_getStringAttributeByName(n, "flavour"));
    ret->setFlavour(processFlavourFromString(flavour));
    ret->setDontInitialize(_getBoolAttributeByName(n, "dontinitialize"));
    ret->setStandard(_getBoolAttributeByName(n, "standard", false));

    // optional attribute
    ret->setEntryStateName(_getStringAttributeByName(n, "entryStateName", false));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "SENSITIVITY") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isValueElement(ll->item(j)->nodeName()))
                    ret->sensitivity.push_back(_visitValue(ll->item(j)));
                else
                    messageError("Unexpected state table->sensitivity child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "SENSITIVITY_POS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isValueElement(ll->item(j)->nodeName()))
                    ret->sensitivityPos.push_back(_visitValue(ll->item(j)));
                else
                    messageError(
                        "Unexpected state table->sensitivity_pos child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "SENSITIVITY_NEG") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isValueElement(ll->item(j)->nodeName()))
                    ret->sensitivityNeg.push_back(_visitValue(ll->item(j)));
                else
                    messageError(
                        "Unexpected state table->sensitivity_neg child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "DECLARATIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDeclElement(ll->item(j)->nodeName()))
                    ret->declarations.push_back(_visitDeclaration(ll->item(j)));
                else
                    messageError("Unexpected state table->declarations child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "STATES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "STATE")
                    ret->states.push_back(_visitState(ll->item(j)));
                else
                    messageError("Unexpected state table->states child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "EDGES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "TRANSITION")
                    ret->edges.push_back(_visitTransition(ll->item(j)));
                else
                    messageError("Unexpected state table->edges child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected state table child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
String *XmlParser::_visitString(Poco::XML::Node *n)
{
    String *ret = new String();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setConstexpr(_getBoolAttributeByName(n, "constexpr"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "RANGE" && _formatVersionMajor < 4)
            ret->setSpanInformation(_visitRange(l->item(i)));
        else if (l->item(i)->nodeName() == "SPAN_INFORMATION" && _formatVersionMajor >= 4)
            ret->setSpanInformation(_visitRange(l->item(i)));
        else
            messageError("Unexpected string child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Switch *XmlParser::_visitSwitch(Poco::XML::Node *n)
{
    Switch *ret = new Switch();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setCaseSemantics(
        hif::caseSemanticsFromString(_getStringAttributeByName(n, "caseSemantics", false, "CASE_LITERAL")));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "ALT") {
            ret->alts.push_back(_visitSwitchAlt(l->item(i)));
        } else if (l->item(i)->nodeName() == "VALUE" && _formatVersionMajor <= 2) {
            ret->setCondition(_visitInnerValue(l->item(i)));
        } else if (l->item(i)->nodeName() == "CONDITION" && _formatVersionMajor > 2) {
            ret->setCondition(_visitInnerValue(l->item(i)));
        } else if (l->item(i)->nodeName() == "DEFAULTS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName())) {
                    ret->defaults.push_back(_visitAction(ll->item(j)));
                } else
                    messageError("Unexpected switch->default child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected switch child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
SwitchAlt *XmlParser::_visitSwitchAlt(Poco::XML::Node *n)
{
    SwitchAlt *ret = new SwitchAlt();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (_isValueElement(l->item(i)->nodeName()))
            ret->conditions.push_back(_visitValue(l->item(i)));
        else if (l->item(i)->nodeName() == "ACTIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName()))
                    ret->actions.push_back(_visitAction(ll->item(j)));
                else
                    messageError("Unexpected switch alt->actions child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected switch alt child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}

System *XmlParser::_visitSystem(Poco::XML::Node *n)
{
    System *so = new System();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, so);

    ///  ATTRIBUTES
    so->setName(_getStringAttributeByName(n, "name", true));
    System::VersionInfo version;
    version.release            = _getStringAttributeByName(n, "release", false);
    version.tool               = _getStringAttributeByName(n, "tool", false);
    version.generationDate     = _getStringAttributeByName(n, "generationDate", false);
    std::string s       = _getStringAttributeByName(n, "formatVersion", false);
    version.formatVersionMajor = 0;
    version.formatVersionMinor = 0;
    if (!s.empty()) {
        std::stringstream ss;
        ss << s;
        ss >> version.formatVersionMajor;
        char dot;
        ss >> dot;
        ss >> version.formatVersionMinor;
    }
    _formatVersionMajor = version.formatVersionMajor;
    _formatVersionMinor = version.formatVersionMinor;
    so->setVersionInfo(version);

    std::string langID = _getStringAttributeByName(n, "languageId");
    so->setLanguageID(languageIDFromString(langID));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "LIBRARYDEFS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "LIBRARYDEF") {
                    so->libraryDefs.push_back(_visitLibraryDef(ll->item(j)));
                } else
                    messageError("Unexpected system->libraryDefs child " + ll->item(j)->nodeName(), nullptr, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "DESIGNUNITS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "DESIGNUNIT") {
                    so->designUnits.push_back(_visitDesignUnit(ll->item(j)));
                } else
                    messageError("Unexpected system->designUnits child " + ll->item(j)->nodeName(), nullptr, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "DECLARATIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDeclElement(ll->item(j)->nodeName())) {
                    so->declarations.push_back(_visitDeclaration(ll->item(j)));
                } else
                    messageError("Unexpected system->declarations child " + ll->item(j)->nodeName(), nullptr, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "LIBRARIES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "LIBRARY") {
                    so->libraries.push_back(_visitLibrary(ll->item(j)));
                } else
                    messageError("Unexpected system->library child " + ll->item(j)->nodeName(), nullptr, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "ACTIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName())) {
                    so->actions.push_back(_visitAction(ll->item(j)));
                } else
                    messageError("Unexpected system->actions child " + ll->item(j)->nodeName(), nullptr, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected system child " + l->item(i)->nodeName(), nullptr, nullptr);
    }

    l->release();

    return so;
}
StringValue *XmlParser::_visitStringValue(Poco::XML::Node *n)
{
    StringValue *ret = new StringValue();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setValue(_getStringAttributeByName(n, "value").c_str());

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "TYPE") {
            ret->setType(_visitInnerType(l->item(i)));
        } else
            messageError("Unexpected text child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Time *XmlParser::_visitTime(Poco::XML::Node *n)
{
    Time *ret = new Time();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setConstexpr(_getConstexprAttributeByName(n, "constexpr"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN

    return ret;
}
TimeValue *XmlParser::_visitTimeValue(Poco::XML::Node *n)
{
    TimeValue *ret = new TimeValue();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    std::string unit = _getStringAttributeByName(n, "unit");
    ret->setUnit(TimeValue::timeUnitFromString(unit));

    ret->setValue(_getDoubleAttributeByName(n, "value"));

    /// CHILDREN
    _checkExactlyOneChild(n, "TYPE", ret);
    ret->setType(_visitInnerType(n->firstChild()));

    return ret;
}
TPAssign *XmlParser::_visitTPAssign(Poco::XML::Node *n)
{
    TPAssign *ret = nullptr;

    std::string type_name = n->nodeName();
    if (type_name == "TYPETPASSIGN")
        ret = _visitTypeTPAssign(n);
    else if (type_name == "VALUETPASSIGN")
        ret = _visitValueTPAssign(n);
    else
        messageError("Unexpected or deprecated tp assign node:" + type_name, ret, nullptr);

    return ret;
}
Type *XmlParser::_visitType(Poco::XML::Node *n)
{
    Type *ret = nullptr;

    std::string type_name = n->nodeName();
    if (type_name == "ARRAY")
        ret = _visitArray(n);
    else if (type_name == "RECORD")
        ret = _visitRecord(n);
    else if (type_name == "BIT")
        ret = _visitBit(n);
    else if (type_name == "BITVECTOR")
        ret = _visitBitvector(n);
    else if (type_name == "BOOLEAN")
        ret = _visitBool(n);
    else if (type_name == "CHAR")
        ret = _visitChar(n);
    else if (type_name == "ENUM")
        ret = _visitEnum(n);
    else if (type_name == "EVENT")
        ret = _visitEvent(n);
    else if (type_name == "INTEGER")
        ret = _visitInt(n);
    else if (type_name == "LIBRARY")
        ret = _visitLibrary(n);
    else if (type_name == "POINTER")
        ret = _visitPointer(n);
    else if (type_name == "REAL")
        ret = _visitReal(n);
    else if (type_name == "REFERENCE")
        ret = _visitReference(n);
    else if (type_name == "SIGNED_TYPE")
        ret = _visitSigned(n);
    else if (type_name == "STRING")
        ret = _visitString(n);
    else if (type_name == "TIME")
        ret = _visitTime(n);
    else if (type_name == "FILE")
        ret = _visitFile(n);
    else if (type_name == "TYPEREFERENCE")
        ret = _visitTypeReference(n);
    else if (type_name == "UNSIGNED_TYPE")
        ret = _visitUnsigned(n);
    else if (type_name == "VIEWREFERENCE")
        ret = _visitViewReference(n);
    else
        messageError("Type: Unexpected or deprecated type node: " + type_name, ret, nullptr);

    return ret;
}
ReferencedType *XmlParser::_visitReferencedType(Poco::XML::Node *n)
{
    ReferencedType *ret = nullptr;

    std::string type_name = n->nodeName();

    if (type_name == "LIBRARY")
        ret = _visitLibrary(n);
    else if (type_name == "TYPEREFERENCE")
        ret = _visitTypeReference(n);
    else if (type_name == "VIEWREFERENCE")
        ret = _visitViewReference(n);
    else
        messageError("ReferencedType: Unexpected or deprecated referenced type node: " + type_name, ret, nullptr);

    return ret;
}
TypeDef *XmlParser::_visitTypeDef(Poco::XML::Node *n)
{
    TypeDef *ret = new TypeDef();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));
    ret->setOpaque(_getBoolAttributeByName(n, "opaque"));
    ret->setStandard(_getBoolAttributeByName(n, "standard", false));
    ret->setExternal(_getBoolAttributeByName(n, "external", false));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "TYPE") {
            ret->setType(_visitInnerType(l->item(i)));
        } else if (l->item(i)->nodeName() == "RANGE") {
            ret->setRange(_visitRange(l->item(i)));
        } else if (l->item(i)->nodeName() == "TEMPLATE_PARAMETERS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDeclElement(ll->item(j)->nodeName()))
                    ret->templateParameters.push_back(_visitDeclaration(ll->item(j)));
                else
                    messageError("Unexpected type def->template params child " + ll->item(j)->nodeName(), ret, nullptr);
            }
        }
    }
    l->release();

    return ret;
}
TypeTP *XmlParser::_visitTypeTP(Poco::XML::Node *n)
{
    TypeTP *ret = new TypeTP();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (_isTypeElement(l->item(i)->nodeName())) {
            ret->setType(_visitType(l->item(i)));
        } else
            messageError("Unexpected Type TP child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
TypeReference *XmlParser::_visitTypeReference(Poco::XML::Node *n)
{
    TypeReference *ret = new TypeReference();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "RANGES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                ret->ranges.push_back(_visitRange(ll->item(j)));
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "TEMPLATE_PARAMETER_ASSIGNS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isTPAssignElement(ll->item(j)->nodeName()))
                    ret->templateParameterAssigns.push_back(_visitTPAssign(ll->item(j)));
                else
                    messageError(
                        "Unexpected type ref->template param assign child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "INSTANCE") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "VIEWREFERENCE" || ll->item(j)->nodeName() == "LIBRARY")
                    ret->setInstance(_visitReferencedType(ll->item(j)));
                else
                    messageError("Unexpected type typeref->instance child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected Type ref child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
TypeTPAssign *XmlParser::_visitTypeTPAssign(Poco::XML::Node *n)
{
    TypeTPAssign *ret = new TypeTPAssign();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN
    _checkExactlyOneChild(n, "TYPE", ret);
    ret->setType(_visitInnerType(n->firstChild()));

    return ret;
}
Unsigned *XmlParser::_visitUnsigned(Poco::XML::Node *n)
{
    Unsigned *ret = new Unsigned();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setConstexpr(_getConstexprAttributeByName(n, "constexpr"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    _checkExactlyOneChild(n, "RANGE", ret);
    ret->setSpan(_visitRange(n->firstChild()));

    return ret;
}
TypedObject *XmlParser::_visitTypedObject(Poco::XML::Node *n)
{
    TypedObject *ret = nullptr;

    std::string value_name = n->nodeName();
    if (_isValueElement(value_name))
        ret = _visitValue(n);
    else if (_isTPAssignElement(value_name))
        ret = _visitTPAssign(n);
    else if (value_name == "PARAMETERASSIGN")
        ret = _visitParameterAssign(n);
    else if (value_name == "PORTASSIGN")
        ret = _visitPortAssign(n);

    return ret;
}
Value *XmlParser::_visitValue(Poco::XML::Node *n)
{
    Value *ret = nullptr;

    std::string value_name = n->nodeName();
    if (value_name == "FIELDREFERENCE")
        ret = _visitFieldReference(n);
    else if (value_name == "MEMBER")
        ret = _visitMember(n);
    else if (value_name == "IDENTIFIER")
        ret = _visitIdentifier(n);
    else if (value_name == "SLICE")
        ret = _visitSlice(n);
    else if (value_name == "AGGREGATE")
        ret = _visitAggregate(n);
    else if (value_name == "CAST")
        ret = _visitCast(n);
    else if (value_name == "BITVAL")
        ret = _visitBitval(n);
    else if (value_name == "BITVECTORVAL")
        ret = _visitBitvectorValue(n);
    else if (value_name == "BOOLVAL")
        ret = _visitBoolValue(n);
    else if (value_name == "CHARVAL")
        ret = _visitCharValue(n);
    else if (value_name == "INTVAL")
        ret = _visitIntValue(n);
    else if (value_name == "REALVAL")
        ret = _visitRealValue(n);
    else if (value_name == "STRINGVAL")
        ret = _visitStringValue(n);
    else if (value_name == "EXPRESSION")
        ret = _visitExpression(n);
    else if (value_name == "RANGE")
        ret = _visitRange(n);
    else if (value_name == "FCALL")
        ret = _visitFunctionCall(n);
    else if (value_name == "WHEN")
        ret = _visitWhen(n);
    else if (value_name == "WITH")
        ret = _visitWith(n);
    else if (value_name == "INSTANCE")
        ret = _visitInstance(n);
    else if (value_name == "RECORDVALUE")
        ret = _visitRecordValue(n);
    else if (value_name == "TIMEVALUE")
        ret = _visitTimeValue(n);
    else
        messageError("Value: Unexpected or deprecated node:" + value_name, ret, nullptr);
    return ret;
}

ValueStatement *XmlParser::_visitValueStatement(Poco::XML::Node *n)
{
    ValueStatement *ret = new ValueStatement();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (_isValueElement(l->item(i)->nodeName()))
            ret->setValue(_visitValue(l->item(i)));
        else
            messageError("Unexpected value statement child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
ValueTP *XmlParser::_visitValueTP(Poco::XML::Node *n)
{
    ValueTP *ret = new ValueTP();

    /// PARENT PARSING
    _visitDataDeclImpl(ret, n);

    /// ATTRIBUTES
    ret->setCompileTimeConstant(_getBoolAttributeByName(n, "compileTimeConstant"));

    /// CODE INFO

    /// ATTRIBUTES

    /// CHILDREN

    return ret;
}
ValueTPAssign *XmlParser::_visitValueTPAssign(Poco::XML::Node *n)
{
    ValueTPAssign *ret = new ValueTPAssign();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "VALUE") {
            ret->setValue(_visitInnerValue(l->item(i)));
        } else
            messageError("Unexpected value tp assign child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Variable *XmlParser::_visitVariable(Poco::XML::Node *n)
{
    Variable *ret = new Variable();

    /// PARENT PARSING
    _visitDataDeclImpl(ret, n);

    /// CODE INFO

    /// ATTRIBUTES
    ret->setInstance(_getBoolAttributeByName(n, "instance"));
    ret->setStandard(_getBoolAttributeByName(n, "standard", false));

    /// CHILDREN

    return ret;
}
View *XmlParser::_visitView(Poco::XML::Node *n)
{
    View *ret = new View();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));
    ret->setStandard(_getBoolAttributeByName(n, "standard", false));
    ret->setFilename(_getStringAttributeByName(n, "filename", false).c_str());

    std::string langID = _getStringAttributeByName(n, "languageId");
    ret->setLanguageID(languageIDFromString(langID));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "ENTITY") {
            ret->setEntity(_visitEntity(l->item(i)));
        } else if (l->item(i)->nodeName() == "CONTENTS") {
            ret->setContents(_visitContents(l->item(i)));
        } else if (l->item(i)->nodeName() == "TEMPLATE_PARAMETERS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDeclElement(ll->item(j)->nodeName())) {
                    ret->templateParameters.push_back(_visitDeclaration(ll->item(j)));
                } else
                    messageError(
                        "Unexpected for view->template parameters child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "LIBRARIES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "LIBRARY") {
                    ret->libraries.push_back(_visitLibrary(ll->item(j)));
                } else
                    messageError("Unexpected for view->libraries child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "DECLARATIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isDeclElement(ll->item(j)->nodeName())) {
                    ret->declarations.push_back(_visitDeclaration(ll->item(j)));
                } else
                    messageError("Unexpected for view->declarations child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "INHERITANCES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isViewrefElement(ll->item(j)->nodeName())) {
                    ret->inheritances.push_back(_visitViewReference(ll->item(j)));
                } else
                    messageError("Unexpected for view->inheritances child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected view child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
ViewReference *XmlParser::_visitViewReference(Poco::XML::Node *n)
{
    ViewReference *ret = new ViewReference();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));
    ret->setDesignUnit(_getStringAttributeByName(n, "unitName"));
    ret->setTypeVariant(_getTypeVariantAttributeByName(n, "typeVariant"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (_isTPAssignElement(l->item(i)->nodeName())) {
            ret->templateParameterAssigns.push_back(_visitTPAssign(l->item(i)));
        } else if (l->item(i)->nodeName() == "INSTANCE") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "VIEWREFERENCE" || ll->item(j)->nodeName() == "LIBRARY")
                    ret->setInstance(_visitReferencedType(ll->item(j)));
                else
                    messageError("Unexpected type viewref->instance child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected view ref child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
Wait *XmlParser::_visitWait(Poco::XML::Node *n)
{
    Wait *ret = new Wait();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "SENSITIVITY") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isValueElement(ll->item(j)->nodeName()))
                    ret->sensitivity.push_back(_visitValue(ll->item(j)));
                else
                    messageError("Unexpected wait->sensitivity child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "SENSITIVITY_POS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isValueElement(ll->item(j)->nodeName()))
                    ret->sensitivityPos.push_back(_visitValue(ll->item(j)));
                else
                    messageError("Unexpected wait->sensitivityPos child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "SENSITIVITY_NEG") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isValueElement(ll->item(j)->nodeName()))
                    ret->sensitivityNeg.push_back(_visitValue(ll->item(j)));
                else
                    messageError("Unexpected wait->sensitivityNeg child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "ACTIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName()))
                    ret->actions.push_back(_visitAction(ll->item(j)));
                else
                    messageError("Unexpected wait->actions child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else if (l->item(i)->nodeName() == "CONDITION")
            ret->setCondition(_visitInnerValue(l->item(i)));
        else if (l->item(i)->nodeName() == "REPETITIONS")
            ret->setRepetitions(_visitInnerValue(l->item(i)));
        else if (l->item(i)->nodeName() == "TIME")
            ret->setTime(_visitInnerValue(l->item(i)));
        else
            messageError("Unexpected wait child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
When *XmlParser::_visitWhen(Poco::XML::Node *n)
{
    When *ret = new When();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setLogicTernary(_getBoolAttributeByName(n, "logicTernary"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "DEFAULT")
            ret->setDefault(_visitInnerValue(l->item(i)));
        else if (l->item(i)->nodeName() == "ALT")
            ret->alts.push_back(_visitWhenAlt(l->item(i)));
        else
            messageError("Unexpected when child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}

WhenAlt *XmlParser::_visitWhenAlt(Poco::XML::Node *n)
{
    WhenAlt *ret = new WhenAlt();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "CONDITION")
            ret->setCondition(_visitInnerValue(l->item(i)));
        else if (l->item(i)->nodeName() == "VALUE")
            ret->setValue(_visitInnerValue(l->item(i)));
        else
            messageError("Unexpected when alt child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
While *XmlParser::_visitWhile(Poco::XML::Node *n)
{
    While *ret = new While();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));
    ret->setDoWhile(_getBoolAttributeByName(n, "doWhile"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        // Fix by VG: the XML node for the while condition is CONDITION (not COND)
        //if( l->item(i)->nodeName() == "CONDITION" )
        if (l->item(i)->nodeName() == "CONDITION")
            ret->setCondition(_visitInnerValue(l->item(i)));
        else if (l->item(i)->nodeName() == "ACTIONS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (_isActionElement(ll->item(j)->nodeName()))
                    ret->actions.push_back(_visitAction(ll->item(j)));
                else
                    messageError("Unexpected while->actions child " + ll->item(j)->nodeName(), ret, nullptr);
            }
            ll->release();
        } else
            messageError("Unexpected while child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
With *XmlParser::_visitWith(Poco::XML::Node *n)
{
    With *ret = new With();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setCaseSemantics(
        hif::caseSemanticsFromString(_getStringAttributeByName(n, "caseSemantics", false, "CASE_LITERAL")));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "DEFAULT")
            ret->setDefault(_visitInnerValue(l->item(i)));
        else if (l->item(i)->nodeName() == "ALT")
            ret->alts.push_back(_visitWithAlt(l->item(i)));
        else if (l->item(i)->nodeName() == "CONDITION")
            ret->setCondition(_visitInnerValue(l->item(i)));
        else
            messageError("Unexpected with child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
WithAlt *XmlParser::_visitWithAlt(Poco::XML::Node *n)
{
    WithAlt *ret = new WithAlt();

    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "VALUE")
            ret->setValue(_visitInnerValue(l->item(i)));
        else if (_isValueElement(l->item(i)->nodeName()))
            ret->conditions.push_back(_visitValue(l->item(i)));
        else
            messageError("Unexpected with alt child " + l->item(i)->nodeName(), ret, nullptr);
    }
    l->release();

    return ret;
}
// -------------------- Inner elements calls -----------------------------

Type *XmlParser::_visitInnerType(Poco::XML::Node *n)
{
    if (n == nullptr)
        return nullptr;

    // type is child of current node. Check that there is and it is one!
    _checkExactlyOneChild(n);

    return _visitType(n->firstChild());
}
Value *XmlParser::_visitInnerValue(Poco::XML::Node *n)
{
    if (n == nullptr)
        return nullptr;

    // value is child of current node. Check that there is and it is one!
    _checkExactlyOneChild(n);

    return _visitValue(n->firstChild());
}
TypedObject *XmlParser::_visitInnerTypedObject(Poco::XML::Node *n)
{
    if (n == nullptr)
        return nullptr;

    // value is child of current node. Check that there is and it is one!
    _checkExactlyOneChild(n);

    return _visitTypedObject(n->firstChild());
}
// --------------------------- utilities ---------------------------------

Poco::XML::XMLString XmlParser::_getAttributeByName(Poco::XML::Node *n, const std::string &s, bool required)
{
    Poco::XML::NamedNodeMap *attributes = n->attributes();
    if (attributes->getNamedItem(s) == nullptr) {
        attributes->release();
        messageAssert(
            !required, "not found required attribute: " + s + " for node: " + n->nodeName(), nullptr, nullptr);
        return "";
    }

    Poco::XML::XMLString ret = attributes->getNamedItem(s)->nodeValue();
    attributes->release();
    return ret;
}

bool XmlParser::_hasAttributeWithName(Poco::XML::Node *n, const std::string &s)
{
    Poco::XML::NamedNodeMap *attributes = n->attributes();
    bool ret                            = (attributes->getNamedItem(s) != nullptr);
    attributes->release();
    return ret;
}
std::string XmlParser::_getStringAttributeByName(
    Poco::XML::Node *n,
    const std::string &s,
    bool required,
    const std::string &defaultValue)
{
    std::string ret = _getAttributeByName(n, s, required);
    if (ret == "")
        return defaultValue;

    return ret;
}
bool XmlParser::_getBoolAttributeByName(Poco::XML::Node *n, const std::string &s, bool required)
{
    std::string str = _getAttributeByName(n, s, required);
    if (str == "")
        return false;
    else if (str == "true")
        return true;
    else if (str == "false")
        return false;
    messageError("Unexpected value of boolean attribute for node " + n->nodeName() + ": " + str, nullptr, nullptr);
}

std::int64_t XmlParser::_getIntAttributeByName(Poco::XML::Node *n, const std::string &s, bool required)
{
    std::int64_t ret;
    std::stringstream val;
    val << _getAttributeByName(n, s, required);
    val >> ret;
    return ret;
}
double XmlParser::_getDoubleAttributeByName(Poco::XML::Node *n, const std::string &s, bool required)
{
    double ret;
    std::stringstream val;
    val << _getAttributeByName(n, s, required);
    val >> ret;
    return ret;
}

Type::TypeVariant XmlParser::_getTypeVariantAttributeByName(Poco::XML::Node *n, const std::string &s, bool)
{
    std::string variant;
    if (_formatVersionMajor < 1) {
        variant = _getAttributeByName(n, s, false);
        if (variant == "")
            variant = "NATIVE_TYPE";
    } else {
        variant = _getAttributeByName(n, s, true);
    }
    return Type::typeVariantFromString(variant);
}

bool XmlParser::_getConstexprAttributeByName(Poco::XML::Node *n, const std::string &s, bool required)
{
    bool cexpr = false;
    if (_formatVersionMajor < 1) {
        if (_hasAttributeWithName(n, "constexpr")) {
            cexpr = _getBoolAttributeByName(n, "constexpr", required);
        } else if (_hasAttributeWithName(n, "native")) {
            cexpr = _getBoolAttributeByName(n, "native", required);
        }
    } else {
        cexpr = _getBoolAttributeByName(n, s, required);
    }

    return cexpr;
}

void XmlParser::_addCodeInfoPropertiesComments(Poco::XML::Node *n, Object *o)
{
    std::string filename("");
    unsigned int line_number   = 0;
    unsigned int column_number = 0;

    Poco::XML::NodeList *l = n->childNodes();
    std::list<Poco::XML::Node *> node_to_remove;
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "PROPERTIES") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "PROPERTY") {
                    std::string name = _getStringAttributeByName(ll->item(j), "name");
                    TypedObject *val        = nullptr;

                    /// CHILDREN
                    Poco::XML::Node *nVal = ll->item(j)->firstChild();
                    if (nVal != nullptr) {
                        _checkExactlyOneChild(ll->item(j), "", o);
                        if (nVal->nodeName() == "VALUE") {
                            val = _visitInnerTypedObject(ll->item(j)->firstChild());
                        } else
                            messageError(
                                "Unexpected property child " + ll->item(j)->firstChild()->nodeName(), o, nullptr);
                    }
                    o->addProperty(name, val);
                } else
                    messageError("Unexpected properties child " + ll->item(j)->nodeName(), o, nullptr);
            }
            ll->release();
            node_to_remove.push_back(l->item(i));
        } else if (l->item(i)->nodeName() == "CODE_INFO") {
            filename      = _getStringAttributeByName(l->item(i), "file");
            line_number   = static_cast<unsigned int>(_getIntAttributeByName(l->item(i), "line_number"));
            column_number = static_cast<unsigned int>(_getIntAttributeByName(l->item(i), "column_number"));

            node_to_remove.push_back(l->item(i));
        } else if (l->item(i)->nodeName() == "COMMENTS") {
            Poco::XML::NodeList *ll = l->item(i)->childNodes();
            for (unsigned int j = 0; j < ll->length(); ++j) {
                if (ll->item(j)->nodeName() == "COMMENT") {
                    o->addComment(_getStringAttributeByName(ll->item(j), "text"));
                } else
                    messageError("Unexpected comments child " + ll->item(j)->nodeName(), o, nullptr);
            }
            ll->release();
            node_to_remove.push_back(l->item(i));
        } else if (l->item(i)->nodeName() == "ADDITIONAL_KEYWORDS") {
            if (dynamic_cast<Declaration *>(o) != nullptr) {
                Declaration *d          = static_cast<Declaration *>(o);
                Poco::XML::NodeList *ll = l->item(i)->childNodes();
                for (unsigned int j = 0; j < ll->length(); ++j) {
                    if (ll->item(j)->nodeName() == "ADDITIONAL_KEYWORD") {
                        std::string name = _getStringAttributeByName(ll->item(j), "name");
                        d->addAdditionalKeyword(name);
                    }
                }
                ll->release();
                node_to_remove.push_back(l->item(i));
            }
        }
    }

    // Properties, Code info and comments nodes are managed only here and will
    // cause an error in further parsing, thus they are removed now.
    for (std::list<Poco::XML::Node *>::iterator it = node_to_remove.begin(); it != node_to_remove.end(); ++it) {
        n->removeChild(*it);
    }

    l->release();

    if (filename != "")
        o->setSourceFileName(filename);

    if (line_number != 0)
        o->setSourceLineNumber(line_number);

    if (column_number != 0)
        o->setSourceLineNumber(column_number);
}
void XmlParser::_checkExactlyOneChild(Poco::XML::Node *n, const std::string &s, Object *obj)
{
    Poco::XML::NodeList *l = n->childNodes();
    if (l->length() == 0) {
        messageWarning("Not found child in current node: " + n->nodeName(), obj, nullptr);
        l->release();
        return;
    } else if (l->length() > 1) {
        messageWarning("Found too many child in current node: " + n->nodeName(), obj, nullptr);
        l->release();
        return;
    }
    l->release();

    if (s != "" && s != n->firstChild()->nodeName()) {
        messageWarning("Found different child name in " + n->nodeName() + ", expected: " + s, obj, nullptr);
        return;
    }
}
bool XmlParser::_isDataDeclElement(const std::string &s)
{
    return (
        s == "ALIAS" || s == "CONSTANT" || s == "ENUMVAL" || s == "FIELD" || s == "PARAMETER" || s == "PORT" ||
        s == "SIGNAL" || s == "VALUETP" || s == "VARIABLE");
}

bool XmlParser::_isDeclElement(const std::string &s)
{
    return (
        _isDataDeclElement(s) || s == "CONTENTS" || s == "FORGENERATE" || s == "IFGENERATE" || s == "DESIGNUNIT" ||
        s == "LIBRARYDEF" || s == "FUNCTION" || s == "METHOD" || s == "PROCEDURE" || s == "SYSTEM" || s == "TYPEDEF" ||
        s == "TYPETP" || s == "INSTANCE" || s == "STATE" || s == "SYSTEM");
}
bool XmlParser::_isGenerateElement(const std::string &s) { return (s == "FORGENERATE" || s == "IFGENERATE"); }
bool XmlParser::_isPPAssignElement(const std::string &s) { return (s == "PARAMETERASSIGN" || s == "PORTASSIGN"); }

bool XmlParser::_isActionElement(const std::string &s)
{
    return (
        s == "ASSIGN" || s == "CASE" || s == "EXIT" || s == "FOR" || s == "IF" || s == "NEXT" || s == "nullptr" ||
        s == "TRANSITION" || s == "PCALL" || s == "RETURN" || s == "SWITCH" || s == "WAIT" || s == "WHILE" ||
        s == "WHILE" || s == "VALUESTATEMENT");
}
bool XmlParser::_isTypedObjectElement(const std::string &s)
{
    return (s == "PARAMASSIGN" || s == "PORTASSIGN" || _isTPAssignElement(s) || _isValueElement(s));
}
bool XmlParser::_isValueElement(const std::string &s)
{
    return (
        s == "AGGREGATE" || s == "CAST" || _isConstValue(s) || s == "EXPRESSION" || s == "FCALL" || s == "IDENTIFIER" ||
        s == "INSTANCE" || _isPrefixedReference(s) || s == "RANGE" || s == "RECORDVALUE" || s == "TIMEVALUE" ||
        s == "WHEN" || s == "WITH");
}
bool XmlParser::_isConstValue(const std::string &s)
{
    return (
        s == "BITVAL" || s == "BITVECTORVAL" || s == "BOOLVAL" || s == "CHARVAL" || s == "INTVAL" || s == "REALVAL" ||
        s == "STRINGVAL");
}
bool XmlParser::_isPrefixedReference(const std::string &s)
{
    return (s == "FIELDREFERENCE" || s == "MEMBER" || s == "SLICE");
}
bool XmlParser::_isTypeElement(const std::string &s)
{
    return (
        s == "ARRAY" || s == "RECORD" || s == "BIT" || s == "BITVECTOR" || s == "BOOLEAN" || s == "CHAR" ||
        s == "ENUM" || s == "INTEGER" || s == "LIBRARY" || s == "POINTER" || s == "REAL" || s == "REFERENCE" ||
        s == "SIGNED_TYPE" || s == "STRING" || s == "TIME" || s == "FILE" || s == "TYPEREFERENCE" ||
        s == "UNSIGNED_TYPE" || s == "VIEWREFERENCE" || s == "EVENT");
}
bool XmlParser::_isTPAssignElement(const std::string &s) { return (s == "TYPETPASSIGN" || s == "VALUETPASSIGN"); }
bool XmlParser::_isViewrefElement(const std::string &s) { return (s == "VIEWREFERENCE"); }
Object *XmlParser::_visitGenericObject(Poco::XML::Node *n)
{
    std::string s(n->nodeName());
    Object *ret = nullptr;

    if (s == "ALIAS") {
        _visitAlias(n);
    } else if (s == "CONSTANT") {
        ret = _visitConst(n);
    } else if (s == "ENUMVAL") {
        ret = _visitEnumValue(n);
    } else if (s == "BITVECTOR") {
        ret = _visitBitvector(n);
    } else if (s == "PARAMETER") {
        ret = _visitParameter(n);
    } else if (s == "PORT") {
        ret = _visitPort(n);
    } else if (s == "SIGNAL") {
        ret = _visitSignal(n);
    } else if (s == "VALUETP") {
        ret = _visitValueTP(n);
    } else if (s == "VARIABLE") {
        ret = _visitVariable(n);
    } else if (s == "CONTENTS") {
        ret = _visitContents(n);
    } else if (s == "FORGENERATE") {
        ret = _visitForGenerate(n);
    } else if (s == "IFGENERATE") {
        ret = _visitIfGenerate(n);
    } else if (s == "DESIGNUNIT") {
        ret = _visitDesignUnit(n);
    } else if (s == "LIBRARYDEF") {
        ret = _visitLibraryDef(n);
    } else if (s == "FUNCTION") {
        ret = _visitFunction(n);
    } else if (s == "PROCEDURE") {
        ret = _visitProcedure(n);
    } else if (s == "SYSTEM") {
        ret = _visitSystem(n);
    } else if (s == "TYPEDEF") {
        ret = _visitTypeDef(n);
    } else if (s == "TYPETP") {
        ret = _visitTypeTP(n);
    } else if (s == "INSTANCE") {
        ret = _visitInstance(n);
    } else if (s == "STATE") {
        ret = _visitState(n);
    } else if (s == "PARAMETERASSIGN") {
        ret = _visitParameterAssign(n);
    } else if (s == "PORTASSIGN") {
        ret = _visitPortAssign(n);
    } else if (s == "ASSIGN") {
        ret = _visitAssign(n);
    } else if (s == "CASE") {
        ret = _visitIf(n);
    } else if (s == "EXIT") {
        ret = _visitBreak(n);
    } else if (s == "FOR") {
        ret = _visitFor(n);
    } else if (s == "NEXT") {
        ret = _visitContinue(n);
    } else if (s == "nullptr") {
        ret = _visitNull(n);
    } else if (s == "TRANSITION") {
        ret = _visitTransition(n);
    } else if (s == "PCALL") {
        ret = _visitProcedureCall(n);
    } else if (s == "RETURN") {
        ret = _visitReturn(n);
    } else if (s == "SWITCH") {
        ret = _visitSwitch(n);
    } else if (s == "WAIT") {
        ret = _visitWait(n);
    } else if (s == "WHILE") {
        ret = _visitWhile(n);
    } else if (s == "FIELDREFERENCE") {
        ret = _visitFieldReference(n);
    } else if (s == "MEMBER") {
        ret = _visitMember(n);
    } else if (s == "IDENTIFIER") {
        ret = _visitIdentifier(n);
    } else if (s == "SLICE") {
        ret = _visitSlice(n);
    } else if (s == "AGGREGATE") {
        ret = _visitAggregate(n);
    } else if (s == "CAST") {
        ret = _visitCast(n);
    } else if (s == "BITVAL") {
        ret = _visitBitval(n);
    } else if (s == "BITVECTORVAL") {
        ret = _visitBitvectorValue(n);
    } else if (s == "BOOLVAL") {
        ret = _visitBoolValue(n);
    } else if (s == "CHARVAL") {
        ret = _visitCharValue(n);
    } else if (s == "INTVAL") {
        ret = _visitIntValue(n);
    } else if (s == "REALVAL") {
        ret = _visitRealValue(n);
    } else if (s == "STRINGVAL") {
        ret = _visitStringValue(n);
    } else if (s == "EXPRESSION") {
        ret = _visitExpression(n);
    } else if (s == "WHEN") {
        ret = _visitWhen(n);
    } else if (s == "WITH") {
        ret = _visitWith(n);
    } else if (s == "ARRAY") {
        ret = _visitArray(n);
    } else if (s == "RECORD") {
        ret = _visitRecord(n);
    } else if (s == "BIT") {
        ret = _visitBit(n);
    } else if (s == "BOOLEAN") {
        ret = _visitBool(n);
    } else if (s == "CHAR") {
        ret = _visitChar(n);
    } else if (s == "ENUM") {
        ret = _visitEnum(n);
    } else if (s == "INTEGER") {
        ret = _visitInt(n);
    } else if (s == "POINTER") {
        ret = _visitPointer(n);
    } else if (s == "REAL") {
        ret = _visitReal(n);
    } else if (s == "REFERENCE") {
        ret = _visitReference(n);
    } else if (s == "SIGNED") {
        ret = _visitSigned(n);
    } else if (s == "STRING") {
        ret = _visitString(n);
    } else if (s == "TYPEREFERENCE") {
        ret = _visitTypeReference(n);
    } else if (s == "UNSIGNED") {
        ret = _visitUnsigned(n);
    } else if (s == "TYPETPASSIGN") {
        ret = _visitTypeTPAssign(n);
    } else if (s == "VALUESTATEMENT") {
        ret = _visitValueStatement(n);
    } else if (s == "VALUETPASSIGN") {
        ret = _visitValueTPAssign(n);
    } else {
        messageError(("Not managed TAG: " + s), nullptr, nullptr);
    }

    return ret;
}

void XmlParser::_visitDataDeclImpl(DataDeclaration *ret, Poco::XML::Node *n)
{
    /// CODE INFO
    _addCodeInfoPropertiesComments(n, ret);

    /// ATTRIBUTES
    ret->setName(_getStringAttributeByName(n, "name"));

    /// CHILDREN
    Poco::XML::NodeList *l = n->childNodes();
    for (unsigned int i = 0; i < l->length(); ++i) {
        if (l->item(i)->nodeName() == "TYPE") {
            ret->setType(_visitInnerType(l->item(i)));
        } else if (l->item(i)->nodeName() == "VALUE") {
            ret->setValue(_visitInnerValue(l->item(i)));
        } else if (l->item(i)->nodeName() == "RANGE") {
            ret->setRange(_visitRange(l->item(i)));
        }
    }
    l->release();
}

} // namespace

/// @brief Parses an XML input stream and returns the root object of the parsed hierarchy.
/// @param in The input stream containing the XML data.
/// @param sem Pointer to the language semantics to be used during parsing.
/// @return A pointer to the root object of the parsed XML hierarchy.
Object *parse_xml(std::istream &in, hif::semantics::ILanguageSemantics *sem)
{
    Object *o = nullptr;
    XmlParser(o, in, sem);
    return o;
}

} // namespace hif
