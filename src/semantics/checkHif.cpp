/// @file checkHif.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/checkHif.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/application_utils.hpp"
#include "hif/hifIOUtils.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

namespace hif
{
namespace semantics
{

namespace /* anonymous */
{

typedef std::list<Object *> ObjectList;

// ///////////////////////////////////////////////////////////////////
// CheckSemanticsType
// ///////////////////////////////////////////////////////////////////
class CheckSemanticsType : public GuideVisitor
{
public:
    CheckSemanticsType(ILanguageSemantics *sem, const CheckOptions &opt);
    virtual ~CheckSemanticsType();

    bool checkSemanticsType(Type *semType, ObjectList &symbolList);

    /// @name Symbols visits
    /// @{

    virtual int visitFieldReference(FieldReference &o);
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitIdentifier(Identifier &o);
    virtual int visitInstance(Instance &o);
    virtual int visitLibrary(Library &o);
    virtual int visitParameterAssign(ParameterAssign &o);
    virtual int visitPortAssign(PortAssign &o);
    virtual int visitProcedureCall(ProcedureCall &o);
    virtual int visitTypeReference(TypeReference &o);
    virtual int visitTypeTPAssign(TypeTPAssign &o);
    virtual int visitValueTPAssign(ValueTPAssign &o);
    virtual int visitViewReference(ViewReference &o);

    /// @}

private:
    /// @brief The semantics used for all checks
    ILanguageSemantics *_sem;
    const CheckOptions &_opt;
    const hif::semantics::ILanguageSemantics::SemanticOptions &_semOpt;
    hif::semantics::DeclarationOptions _dopt;
    ObjectList *_wrongSymbols;

    template <typename T> bool _checkSemanticsSymbolDeclaration(T *symbol);

    // disabled
    CheckSemanticsType(const CheckSemanticsType &);
    CheckSemanticsType &operator=(const CheckSemanticsType &);
};

CheckSemanticsType::CheckSemanticsType(ILanguageSemantics *sem, const CheckOptions &opt)
    : GuideVisitor()
    , _sem(sem)
    , _opt(opt)
    , _semOpt(sem->getSemanticsOptions())
    , _dopt()
    , _wrongSymbols(nullptr)
{
    _dopt.dontSearch = true;
}

CheckSemanticsType::~CheckSemanticsType()
{
    // ntd
}

bool CheckSemanticsType::checkSemanticsType(Type *semType, ObjectList &symbolList)
{
    if (semType == nullptr)
        return true;
    _wrongSymbols = &symbolList;
    semType->acceptVisitor(*this);
    return _wrongSymbols->empty();
}

template <typename T> bool CheckSemanticsType::_checkSemanticsSymbolDeclaration(T *symbol)
{
    typename T::DeclarationType *decl = getDeclaration(symbol, _sem, _dopt);
    if (decl == nullptr)
        return true;

    if (hif::isInTree(decl))
        return true;

    _wrongSymbols->push_back(symbol);
    return false;
}

int CheckSemanticsType::visitFieldReference(FieldReference &o)
{
    GuideVisitor::visitFieldReference(o);
    if (!_checkSemanticsSymbolDeclaration(&o))
        return 1;
    return 0;
}

int CheckSemanticsType::visitFunctionCall(FunctionCall &o)
{
    GuideVisitor::visitFunctionCall(o);
    if (!_checkSemanticsSymbolDeclaration(&o))
        return 1;
    return 0;
}

int CheckSemanticsType::visitIdentifier(Identifier &o)
{
    GuideVisitor::visitIdentifier(o);
    if (!_checkSemanticsSymbolDeclaration(&o))
        return 1;
    return 0;
}

int CheckSemanticsType::visitInstance(Instance &o)
{
    GuideVisitor::visitInstance(o);
    if (!_checkSemanticsSymbolDeclaration(&o))
        return 1;
    return 0;
}

int CheckSemanticsType::visitLibrary(Library &o)
{
    GuideVisitor::visitLibrary(o);
    if (!_checkSemanticsSymbolDeclaration(&o))
        return 1;
    return 0;
}

int CheckSemanticsType::visitParameterAssign(ParameterAssign &o)
{
    GuideVisitor::visitParameterAssign(o);
    if (!_checkSemanticsSymbolDeclaration(&o))
        return 1;
    return 0;
}

int CheckSemanticsType::visitPortAssign(PortAssign &o)
{
    GuideVisitor::visitPortAssign(o);
    if (!_checkSemanticsSymbolDeclaration(&o))
        return 1;
    return 0;
}

int CheckSemanticsType::visitProcedureCall(ProcedureCall &o)
{
    GuideVisitor::visitProcedureCall(o);
    if (!_checkSemanticsSymbolDeclaration(&o))
        return 1;
    return 0;
}

int CheckSemanticsType::visitTypeReference(TypeReference &o)
{
    GuideVisitor::visitTypeReference(o);
    if (!_checkSemanticsSymbolDeclaration(&o))
        return 1;
    return 0;
}

int CheckSemanticsType::visitTypeTPAssign(TypeTPAssign &o)
{
    GuideVisitor::visitTypeTPAssign(o);
    if (!_checkSemanticsSymbolDeclaration(&o))
        return 1;
    return 0;
}

int CheckSemanticsType::visitValueTPAssign(ValueTPAssign &o)
{
    GuideVisitor::visitValueTPAssign(o);
    if (!_checkSemanticsSymbolDeclaration(&o))
        return 1;
    return 0;
}

int CheckSemanticsType::visitViewReference(ViewReference &o)
{
    GuideVisitor::visitViewReference(o);
    if (!_checkSemanticsSymbolDeclaration(&o))
        return 1;
    return 0;
}

// ///////////////////////////////////////////////////////////////////
// CheckHifDescription
// ///////////////////////////////////////////////////////////////////
class CheckHifDescription : public GuideVisitor
{
public:
    CheckHifDescription(ILanguageSemantics *sem, const CheckOptions &opt);

    virtual ~CheckHifDescription();

    virtual int visitAlias(Alias &o);
    virtual int visitAggregate(Aggregate &o);
    virtual int visitAggregateAlt(AggregateAlt &o);
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
    virtual int visitContinue(Continue &o);
    virtual int visitDesignUnit(DesignUnit &o);
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
    virtual int visitIfGenerate(IfGenerate &o);
    virtual int visitInstance(Instance &o);
    virtual int visitInt(Int &o);
    virtual int visitIntValue(IntValue &o);
    virtual int visitLibrary(Library &o);
    virtual int visitLibraryDef(LibraryDef &o);
    virtual int visitMember(Member &o);
    virtual int visitParameter(Parameter &o);
    virtual int visitParameterAssign(ParameterAssign &o);
    virtual int visitPointer(Pointer &o);
    virtual int visitPort(Port &o);
    virtual int visitPortAssign(PortAssign &o);
    virtual int visitProcedure(Procedure &o);
    virtual int visitProcedureCall(ProcedureCall &o);
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
    virtual int visitStateTable(StateTable &o);
    virtual int visitString(String &o);
    virtual int visitSystem(System &o);
    virtual int visitSwitch(Switch &o);
    virtual int visitStringValue(StringValue &o);
    virtual int visitTime(Time &o);
    virtual int visitTimeValue(TimeValue &o);
    virtual int visitTransition(Transition &o);
    virtual int visitTypeDef(TypeDef &o);
    virtual int visitTypeReference(TypeReference &o);
    virtual int visitTypeTPAssign(TypeTPAssign &o);
    virtual int visitUnsigned(Unsigned &o);
    virtual int visitValueStatement(ValueStatement &o);
    virtual int visitValueTP(ValueTP &o);
    virtual int visitValueTPAssign(ValueTPAssign &o);
    virtual int visitVariable(Variable &o);
    virtual int visitView(View &o);
    virtual int visitViewReference(ViewReference &o);
    virtual int visitWait(Wait &o);
    virtual int visitWhen(When &o);
    virtual int visitWhenAlt(WhenAlt &o);
    virtual int visitWhile(While &o);
    virtual int visitWith(With &o);

private:
    /// Checks that BitValue or BitvectorValue is in a condition.
    int _checkDontCares(Value &o);

    /// @brief Print an error message. Exit on failure depends on the value of
    /// the related CheckOption exitOnError.
    void _printError(const std::string &message, Object &o);
    void _printError(const std::string &message, Object &o, const std::string &listMessage, const ObjectList &list);

    /// @brief print the message that can be error or warning.
    void _printMessage(bool is_warning, std::string object, std::string message, Object &o);

    /// @brief Check whether initial value is required.
    bool _checkDeclRequiredInitialVal(DataDeclaration *o, const bool checkFlag);

    /// @brief Check if initial value of a declaration object can be assigned
    /// to declaration with Hif semantic.
    /// @return true if it is found an error.
    bool _checkDeclInitialValAssign(DataDeclaration &o);

    /// @brief Checks that the syntactic type is set correctly.
    bool _checkSyntacticType(ConstValue &o);

    /// @brief Checks if given logic values are allowed.
    bool _checkLogicValues(Object *o, const std::string &s, const bool isLogic);

    /// @brief Check existence of declaration and its support.
    template <typename T> bool _checkDeclaration(T *o);

    /// @brief Check the result of instantiate call on given symbol.
    template <typename T> bool _checkInstantiate(T *o);

    /// @brief Check The semantics type correctness.
    bool _checkSemanticsType(TypedObject *o);

    /// @brief Used for keep track of current DU, this information is util
    /// for debugging.
    std::string currentDUName;

    /// @brief The semantics used for all checks
    ILanguageSemantics *_sem;

    /// @brief The options
    const CheckOptions &_opt;

    /// @brief The semantics options.
    const hif::semantics::ILanguageSemantics::SemanticOptions &_semOpt;

    /// @brief the semantics type visitor.
    CheckSemanticsType _checkSemTypeVisitor;

    // warning disabled
    CheckHifDescription(const CheckHifDescription &);
    CheckHifDescription &operator=(const CheckHifDescription &);
};
CheckHifDescription::CheckHifDescription(ILanguageSemantics *sem, const CheckOptions &opt)
    : GuideVisitor()
    , currentDUName("")
    , _sem(sem)
    , _opt(opt)
    , _semOpt(sem->getSemanticsOptions())
    , _checkSemTypeVisitor(sem, opt)
{
    // Nothing to do.
}
CheckHifDescription::~CheckHifDescription()
{
    // Nothing to do.
}
int CheckHifDescription::visitPortAssign(PortAssign &o)
{
    int ret = GuideVisitor::visitPortAssign(o);

    if (o.getPartialBind() != nullptr) {
        _printError("found partial binding for port assign.", o);
        return 1;
    }

    if (!_checkSemanticsType(&o))
        return 1;
    Type *pt = o.getSemanticType();

    if (o.getValue() == nullptr) {
        _printError("value not found.", o);
        return 1;
    }

    Type *vt = hif::semantics::getSemanticType(o.getValue(), _sem);
    if (vt == nullptr) {
        _printError("type not found for value.", o);
        return 1;
    }

    if (!_sem->isValueAllowedInPortBinding(o.getValue())) {
        _printError("type not allowed as portassign value.", o);
        return 1;
    }

    if (!_checkDeclaration(&o))
        return 1;

    // At the moment, PortAssigns cannot have the direction set.
    // if (o.getDirection() == dir_none || o.getDirection() != port->getDirection())
    // {
    //     _printError("Portassign direction not set.", o);
    //     return 1;
    // }

    hif::semantics::ILanguageSemantics::ExpressionTypeInfo res = _sem->getExprType(pt, vt, op_bind, &o);
    if (res.returnedType == nullptr) {
        _printError("port and value have incompatible type.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitPort(Port &o)
{
    int ret = GuideVisitor::visitPort(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (o.getType() == nullptr) {
        _printError("type not found.", o);
        return 1;
    }

    if (!_sem->isTypeAllowedAsPort(o.getType())) {
        _printError("type not allowed as port.", o);
        return 1;
    }

    if (o.getDirection() == dir_none) {
        _printError("port has not got a valid set direction.", o);
        return 1;
    }

    if (o.getDirection() == dir_in && _semOpt.port_inNoInitialValue) {
        if (o.getValue() != nullptr) {
            _printError("input port has got a default value.", o);
            return 1;
        }
    }

    const bool check = (o.getDirection() == dir_out || o.getDirection() == dir_inout) && _semOpt.port_outInitialValue;
    if (!_checkDeclRequiredInitialVal(&o, check)) {
        return 1;
    }
    if (o.getValue() != nullptr) {
        Type *tiv = hif::semantics::getSemanticType(o.getValue(), _sem);
        if (tiv == nullptr) {
            _printError("unable to type output port default value.", o);
            return 1;
        }

        if (!_checkDeclInitialValAssign(o))
            return 1;
    }

    return ret;
}

int CheckHifDescription::visitVariable(Variable &o)
{
    int ret = GuideVisitor::visitVariable(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (o.getType() == nullptr) {
        _printError("type not found.", o);
        return 1;
    }

    if (!_checkDeclRequiredInitialVal(&o, _semOpt.dataDeclaration_initialValue)) {
        return 1;
    }

    if (o.getValue() != nullptr) {
        Type *tiv = hif::semantics::getSemanticType(o.getValue(), _sem);
        if (tiv == nullptr) {
            _printError("unable to type variable default value.", o);
            return 1;
        }

        messageDebugAssert(
            !o.isStandard(),
            "Found standard Variable. This is the first use of this feature."
            " Remove this check if it is not an error.",
            &o, _sem);

        if (!_checkDeclInitialValAssign(o))
            return 1;
    }

    return ret;
}
int CheckHifDescription::visitConst(Const &o)
{
    int ret = GuideVisitor::visitConst(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (o.getType() == nullptr) {
        _printError("type not found.", o);
        return 1;
    }

    if (o.getValue() == nullptr) {
        _printError("default value not found.", o);
        return 1;
    }

    Type *tiv = hif::semantics::getSemanticType(o.getValue(), _sem);
    if (tiv == nullptr) {
        _printError("unable to type const default value.", o);
        return 1;
    }

    messageDebugAssert(
        !o.isStandard(),
        "Found standard Const. This is the first use of this feature. Remove this check if it is not an error.", &o,
        _sem);

    if (!_checkDeclInitialValAssign(o))
        return 1;

    return ret;
}

int CheckHifDescription::visitContinue(Continue &o)
{
    int ret = GuideVisitor::visitContinue(o);

    Object *scope = hif::getMatchingScope(&o);

    if (scope == nullptr) {
        _printError("Not found enclosing suitable scope", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitSignal(Signal &o)
{
    int ret = GuideVisitor::visitSignal(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (o.getType() == nullptr) {
        _printError("type not found.", o);
        return 1;
    }

    if (!_checkDeclRequiredInitialVal(&o, _semOpt.dataDeclaration_initialValue)) {
        return 1;
    }

    if (o.getValue() != nullptr) {
        Type *tiv = hif::semantics::getSemanticType(o.getValue(), _sem);
        if (tiv == nullptr) {
            _printError("unable to type signal default value.", o);
            return 1;
        }

        messageDebugAssert(
            !o.isStandard(),
            "Found standard Signal. This is the first use of this feature. Remove this check if it is not an error.",
            &o, _sem);

        if (!_checkDeclInitialValAssign(o))
            return 1;
    }

    return ret;
}
int CheckHifDescription::visitValueTP(ValueTP &o)
{
    int ret = GuideVisitor::visitValueTP(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (o.getType() == nullptr) {
        _printError("type not found.", o);
        return 1;
    }

    if (o.isCompileTimeConstant() && !_sem->isTemplateAllowedType(o.getType())) {
        _printError("type not allowed as template.", o);
        return 1;
    }

    if (o.getValue() != nullptr) {
        Type *tiv = hif::semantics::getSemanticType(o.getValue(), _sem);
        if (tiv == nullptr) {
            _printError("unable to type valueTP default value.", o);
            return 1;
        }

        if (!_checkDeclInitialValAssign(o))
            return 1;
    }

    View *v = dynamic_cast<View *>(o.getParent());
    if (v == nullptr && !o.isCompileTimeConstant()) {
        _printError("Non-compile time constant is allowed only for Views.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitParameter(Parameter &o)
{
    int ret = GuideVisitor::visitParameter(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (o.getType() == nullptr) {
        _printError("type not found.", o);
        return 1;
    }

    if (o.getValue() != nullptr) {
        Type *tiv = hif::semantics::getSemanticType(o.getValue(), _sem);
        if (tiv == nullptr) {
            _printError("unable to type default value.", o);
            return 1;
        }

        if (!_checkDeclInitialValAssign(o))
            return 1;
    }

    return ret;
}
int CheckHifDescription::visitField(Field &o)
{
    int ret = GuideVisitor::visitField(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (o.getType() == nullptr) {
        _printError("type not found.", o);
        return 1;
    }

    Record *e = dynamic_cast<Record *>(o.getParent());
    if (e == nullptr) {
        _printError("parent record not found.", o);
        return 1;
    }

    if (_semOpt.scopedType_insideTypedef) {
        TypeDef *td = dynamic_cast<TypeDef *>(e->getParent());
        if (td == nullptr) {
            _printError("record must be inside a typedef.", *o.getParent());
            return 1;
        }

        TypeReference *tr = dynamic_cast<TypeReference *>(o.getType());
        if (tr != nullptr && tr->getName() == td->getName()) {
            _printError("type cannot be a typeref to the typedef of parent record.", o);
            return 1;
        }
    }

    if (!_checkDeclRequiredInitialVal(&o, _semOpt.dataDeclaration_initialValue)) {
        return 1;
    }

    if (o.getValue() != nullptr) {
        Type *tiv = hif::semantics::getSemanticType(o.getValue(), _sem);
        if (tiv == nullptr) {
            _printError("unable to type field default value.", o);
            return 1;
        }

        if (!_checkDeclInitialValAssign(o))
            return 1;
    }

    return ret;
}
int CheckHifDescription::visitInstance(Instance &o)
{
    int ret = GuideVisitor::visitInstance(o);

    if (o.getReferencedType() == nullptr) {
        _printError("referenced type not found.", o);
        return 1;
    }

    if (!_checkDeclaration(&o))
        return 1;

    BaseContents *bp = dynamic_cast<BaseContents *>(o.getParent());
    Value *initVal   = o.getValue();
    if (bp != nullptr && o.isInBList() && o.getBList() == reinterpret_cast<hif::BList<hif::Object> *>(&bp->instances)) {
        // If instance is a sub-module instance...
        ViewReference *vr = dynamic_cast<ViewReference *>(o.getReferencedType());
        if (vr == nullptr) {
            _printError("Referenced type of sub-module instance must be a ViewReference.", o);
            return 1;
        }
        if (vr->getDesignUnit() == o.getName()) {
            _printError("Instance name cannot match DU name.", o);
            return 1;
        }

        if (initVal != nullptr) {
            FunctionCall *fc = dynamic_cast<FunctionCall *>(initVal);

            if (fc == nullptr || fc->getName() != NameTable::getInstance()->hifConstructor()) {
                _printError("Initial value of instance must be a constructor function call", o);
                return 1;
            }
        }
    } else {
        if (initVal != nullptr) {
            _printError("Initial value of instance can be used only with sub-module", o);
            return 1;
        }
    }

    return ret;
}
int CheckHifDescription::visitInt(Int &o)
{
    int ret = GuideVisitor::visitInt(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    if (o.getSpan() == nullptr) {
        _printError("span not set.", o);
        return 1;
    }

    // TODO: this flag means the TRUE NATIVE (int vs sc_int).
    if (_semOpt.int_bitfields && o.getTypeVariant() == Type::SYSTEMC_INT_BITFIELD) {
        Field *parentField = dynamic_cast<Field *>(o.getParent());
        Const *parentConst = dynamic_cast<Const *>(o.getParent());
        if (parentField == nullptr && parentConst == nullptr) {
            Value *spanSize     = hif::semantics::typeGetSpanSize(&o, _sem);
            IntValue *iSpanSize = dynamic_cast<IntValue *>(spanSize);
            if (iSpanSize != nullptr) {
                switch (iSpanSize->getValue()) {
                case 8:
                case 16:
                case 32:
                case 64:
                    break;
                default:
                    delete iSpanSize;
                    _printError("bit fields can appear only inside field type.", o);
                    return 1;
                }
            }
            delete spanSize;
        }
    }

    return ret;
}
int CheckHifDescription::visitIntValue(IntValue &o)
{
    int ret = GuideVisitor::visitIntValue(o);

    if (!_checkSemanticsType(&o))
        return 1;
    if (!_checkSyntacticType(o))
        return 1;

    return ret;
}

int CheckHifDescription::visitBit(Bit &o)
{
    int ret = GuideVisitor::visitBit(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    return ret;
}

int CheckHifDescription::visitBitValue(BitValue &o)
{
    int ret = GuideVisitor::visitBitValue(o);

    if (!_checkSemanticsType(&o))
        return 1;
    if (!_checkSyntacticType(o))
        return 1;

    const bool isLogic = o.getType() != nullptr || hif::typeIsLogic(o.getType(), _sem);
    if (!_checkLogicValues(&o, o.toString(), isLogic))
        return 1;

    if (o.getValue() == bit_dontcare)
        ret = _checkDontCares(o);

    return ret;
}

int CheckHifDescription::visitBitvectorValue(BitvectorValue &o)
{
    int ret = GuideVisitor::visitBitvectorValue(o);

    if (!_checkSemanticsType(&o))
        return 1;
    if (!_checkSyntacticType(o))
        return 1;

    const bool isLogic = o.getType() != nullptr || hif::typeIsLogic(o.getType(), _sem);
    if (!_checkLogicValues(&o, o.getValue(), isLogic))
        return 1;

    // Managing of bitvectors with dontcare bit(s).
    if (o.getValue().find('-') != std::string::npos)
        ret = _checkDontCares(o);

    return ret;
}
int CheckHifDescription::visitBool(Bool &o)
{
    int ret = GuideVisitor::visitBool(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitBoolValue(BoolValue &o)
{
    int ret = GuideVisitor::visitBoolValue(o);

    if (!_checkSemanticsType(&o))
        return 1;
    if (!_checkSyntacticType(o))
        return 1;

    return ret;
}

int CheckHifDescription::visitBreak(Break &o)
{
    int ret = GuideVisitor::visitBreak(o);

    Object *scope = hif::getMatchingScope(&o);

    if (scope == nullptr) {
        _printError("Not found enclosing suitable scope", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitCharValue(CharValue &o)
{
    int ret = GuideVisitor::visitCharValue(o);

    if (!_checkSemanticsType(&o))
        return 1;
    if (!_checkSyntacticType(o))
        return 1;

    return ret;
}
int CheckHifDescription::visitRealValue(RealValue &o)
{
    int ret = GuideVisitor::visitRealValue(o);

    if (!_checkSemanticsType(&o))
        return 1;
    if (!_checkSyntacticType(o))
        return 1;

    return ret;
}
int CheckHifDescription::visitRecord(Record &o)
{
    int ret = GuideVisitor::visitRecord(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    if (o.isUnion() && o.isPacked()) {
        _printError("unions cannot be packed.", o);
        return 1;
    }

    if (_semOpt.scopedType_insideTypedef && dynamic_cast<TypeDef *>(o.getParent()) == nullptr) {
        _printError("parent of record must be a typedef", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitRecordValue(RecordValue &o)
{
    int ret = GuideVisitor::visitRecordValue(o);

    if (!_checkSemanticsType(&o))
        return 1;

    if (o.alts.empty()) {
        DataDeclaration *p = dynamic_cast<DataDeclaration *>(o.getParent());
        if (p == nullptr) {
            _printError("empty alts list. (1)", o);
            return 1;
        }

        Type *bt                  = hif::semantics::getBaseType(p->getType(), false, _sem);
        Record *r                 = dynamic_cast<Record *>(bt);
        const bool initValAllowed = (r != nullptr && r->fields.empty());

        if (!initValAllowed) {
            _printError("empty alts list. (2)", o);
            return 1;
        }
    }

    return ret;
}
int CheckHifDescription::visitRecordValueAlt(RecordValueAlt &o)
{
    int ret = GuideVisitor::visitRecordValueAlt(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("The name is not set.", o);
        return 1;
    }

    if (o.getValue() == nullptr) {
        _printError("Value not set.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitReal(Real &o)
{
    int ret = GuideVisitor::visitReal(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    if (o.getSpan() == nullptr) {
        _printError("span not set.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitStringValue(StringValue &o)
{
    int ret = GuideVisitor::visitStringValue(o);

    if (!_checkSemanticsType(&o))
        return 1;
    if (!_checkSyntacticType(o))
        return 1;

    return ret;
}
int CheckHifDescription::visitFunction(Function &o)
{
    int ret = GuideVisitor::visitFunction(o);
    if (!_sem->isTypeAllowedAsReturn(o.getType()) && !hif::declarationIsPartOfStandard(&o)) {
        _printError("function return type is not allowed by semantics.", o);
        return 1;
    }
    return ret;
}
int CheckHifDescription::visitFunctionCall(FunctionCall &o)
{
    int ret = GuideVisitor::visitFunctionCall(o);

    Function *sig = hif::manipulation::instantiate(&o, _sem);

    if (sig == nullptr) {
        _printError("declaration not found for function call", o);
        return 1;
    }

    // check return type
    if (!_checkSemanticsType(&o))
        return 1;

    // check parameter assign
    if (sig->parameters.size() < o.parameterAssigns.size()) {
        _printError("formal and actual parameters sizes are different.", o);
        return 1;
    }

    if (!_checkInstantiate(&o))
        return 1;
    if (!_checkDeclaration(&o))
        return 1;

    return ret;
}
int CheckHifDescription::visitGlobalAction(GlobalAction &o)
{
    int ret = GuideVisitor::visitGlobalAction(o);

    if (_semOpt.globact_isNoAllowed && !o.actions.empty()) {
        _printError("object now allowed.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitParameterAssign(ParameterAssign &o)
{
    int ret = GuideVisitor::visitParameterAssign(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (dynamic_cast<FunctionCall *>(o.getParent()) == nullptr &&
        dynamic_cast<ProcedureCall *>(o.getParent()) == nullptr) {
        _printError("parent function call or procedure call not found.", o);
        return 1;
    }

    if (!_checkDeclaration(&o))
        return 1;
    if (!_checkSemanticsType(&o))
        return 1;
    Type *t = o.getSemanticType();

    if (o.getValue() == nullptr) {
        _printError("value not found.", o);
        return 1;
    }

    Type *vt = hif::semantics::getSemanticType(o.getValue(), _sem);
    if (vt == nullptr) {
        _printError("value type not found.", o);
        return 1;
    }

    hif::semantics::ILanguageSemantics::ExpressionTypeInfo res = _sem->getExprType(t, vt, op_conv, &o);
    if (res.returnedType == nullptr) {
        _printError("type mismatch between formal and actual parameter.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitPointer(Pointer &o)
{
    int ret = GuideVisitor::visitPointer(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitValueTPAssign(ValueTPAssign &o)
{
    int ret = GuideVisitor::visitValueTPAssign(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (dynamic_cast<FunctionCall *>(o.getParent()) == nullptr &&
        dynamic_cast<ProcedureCall *>(o.getParent()) == nullptr &&
        dynamic_cast<ViewReference *>(o.getParent()) == nullptr &&
        dynamic_cast<TypeReference *>(o.getParent()) == nullptr) {
        _printError("unexpected parent.", o);
        return 1;
    }

    if (!_checkDeclaration(&o))
        return 1;
    if (!_checkSemanticsType(&o))
        return 1;
    Type *t = o.getSemanticType();

    if (o.getValue() == nullptr) {
        _printError("value not found.", o);
        return 1;
    }

    Type *vt = hif::semantics::getSemanticType(o.getValue(), _sem);
    if (vt == nullptr) {
        _printError("value type not found.", o);
        return 1;
    }

    ValueTPAssign::DeclarationType *decl = getDeclaration(&o, _sem);
    if (decl != nullptr) {
        hif::semantics::ILanguageSemantics::ExpressionTypeInfo res = _sem->getExprType(t, vt, op_conv, &o);
        if (res.returnedType == nullptr) {
            _printError("type mismatch between formal and actual parameter.", o);
            return 1;
        }
    }

    return ret;
}
int CheckHifDescription::visitProcedure(Procedure &o)
{
    int ret = GuideVisitor::visitProcedure(o);

    messageDebugAssert(
        !o.isStandard(),
        "Found standard Procedure. This is the first use of this feature. Remove this check if it is not an error.", &o,
        _sem);

    return ret;
}
int CheckHifDescription::visitProcedureCall(ProcedureCall &o)
{
    int ret = GuideVisitor::visitProcedureCall(o);

    SubProgram *sig = hif::manipulation::instantiate(&o, _sem);

    if (sig == nullptr) {
        _printError("declaration not found for procedure call", o);
        return 1;
    }

    // check parameter assign
    if (sig->parameters.size() < o.parameterAssigns.size()) {
        _printError("formal and actual parameters sizes are different.", o);
        return 1;
    }

    if (!_checkInstantiate(&o))
        return 1;
    if (!_checkDeclaration(&o))
        return 1;

    return ret;
}
int CheckHifDescription::visitExpression(Expression &o)
{
    int ret = GuideVisitor::visitExpression(o);

    const bool unExpr = o.getValue2() == nullptr;
    const bool binOp  = hif::operatorIsBinary(o.getOperator());
    const bool unOp   = hif::operatorIsUnary(o.getOperator());
    if ((unExpr && !unOp) || (!unExpr && !binOp)) {
        _printError("Mismatch between operator arity and operands number.", o);
        return 1;
    }

    hif::semantics::getSemanticType(o.getValue1(), _sem);
    if (o.getValue1()->getSemanticType() == nullptr) {
        _printError("unable to type op1.", o);
        return 1;
    }

    if (o.getValue2()) {
        hif::semantics::getSemanticType(o.getValue2(), _sem);
        if (o.getValue2()->getSemanticType() == nullptr) {
            _printError("unable to type op2.", o);
            return 1;
        }
    }

    if (!_checkSemanticsType(&o))
        return 1;

    return ret;
}
int CheckHifDescription::visitAssign(Assign &o)
{
    int ret = GuideVisitor::visitAssign(o);

    if (o.getRightHandSide() == nullptr) {
        _printError("right hand side not found.", o);
        return 1;
    }

    if (o.getLeftHandSide() == nullptr) {
        _printError("left hand side not found.", o);
        return 1;
    }

    Type *tLeft  = hif::semantics::getSemanticType(o.getLeftHandSide(), _sem);
    Type *tRight = hif::semantics::getSemanticType(o.getRightHandSide(), _sem);

    if (tLeft == nullptr) {
        _printError("unable to type left hand side.", o);
        return 1;
    }

    if (tRight == nullptr) {
        _printError("unable to type right hand side.", o);
        return 1;
    }

    hif::semantics::ILanguageSemantics::ExpressionTypeInfo res = _sem->getExprType(tLeft, tRight, op_assign, &o);
    if (res.returnedType == nullptr) {
        _printError("left hand side and right hand side have incompatible types.", o);
        return 1;
    }

    if (o.getDelay()) {
        if (_semOpt.after_isNoAllowed) {
            _printError("Delay is not allowed w.r.t. current semantics.", o);
            return 1;
        }

        Type *tc = hif::semantics::getSemanticType(o.getDelay(), _sem);
        if (tc == nullptr) {
            _printError("Unable to type time value", o);
            return 1;
        }

        if (dynamic_cast<Time *>(tc) == nullptr) {
            _printError("Time value type is not time.", o);
            return 1;
        }
    }

    return ret;
}
int CheckHifDescription::visitIfAlt(IfAlt &o)
{
    int ret = GuideVisitor::visitIfAlt(o);

    if (o.getCondition() == nullptr) {
        _printError("condition not found.", o);
        return 1;
    }

    Type *tc = hif::semantics::getSemanticType(o.getCondition(), _sem);
    if (tc == nullptr) {
        _printError("unable to type condition", o);
        return 1;
    }

    if (!_sem->checkCondition(tc, &o)) {
        messageDebug("Related object", &o, _sem);
        _printError("condition's type is not allowed.", *tc);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitWhenAlt(WhenAlt &o)
{
    int ret = GuideVisitor::visitWhenAlt(o);

    if (o.getCondition() == nullptr) {
        _printError("condition not found.", o);
        return 1;
    }

    Type *tc = hif::semantics::getSemanticType(o.getCondition(), _sem);
    if (tc == nullptr) {
        _printError("unable to type condition", o);
        return 1;
    }

    if (!_sem->checkCondition(tc, &o)) {
        messageDebug("Related object", &o, _sem);
        _printError("condition's type is not allowed.", *tc);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitWhen(When &o)
{
    int ret = GuideVisitor::visitWhen(o);

    if (!_checkSemanticsType(&o))
        return 1;

    Type *refType = o.getSemanticType();
    Type *defType = hif::semantics::getSemanticType(o.getDefault(), _sem);

    hif::semantics::ILanguageSemantics::ExpressionTypeInfo res = _sem->getExprType(refType, defType, op_eq, &o);
    if (defType && res.returnedType == nullptr) {
        _printError("type not compatible for default value.", o);
        return 1;
    }

    for (BList<WhenAlt>::iterator it = o.alts.begin(); it != o.alts.end(); ++it) {
        Type *curr = hif::semantics::getSemanticType((*it)->getValue(), _sem);
        res        = _sem->getExprType(refType, curr, op_eq, *it);
        if (res.returnedType == nullptr) {
            _printError("type not compatible for at least one alt value.", o);
            return 1;
        }
    }

    return ret;
}
int CheckHifDescription::visitWith(With &o)
{
    int ret = GuideVisitor::visitWith(o);

    if (_semOpt.with_isNoAllowed) {
        _printError("object now allowed.", o);
        return 1;
    }

    if (o.getCaseSemantics() != hif::CASE_LITERAL && _sem->getSemanticsOptions().case_isOnlyLiteral) {
        _printError("Not allowed case semantics", o);
        return 1;
    }

    if (!_checkSemanticsType(&o))
        return 1;

    Type *t = hif::semantics::analyzePrecisionType(&o, _sem);
    if (t == nullptr) {
        _printError("unable to type with conditions.", o);
        return 1;
    }

    if (!_sem->isTypeAllowedAsCase(t)) {
        _printError("Case type not allowed.", o);
        delete t;
        return 1;
    }

    if (o.getDefault() == nullptr) {
        _printError("Default value not found.", o);
        delete t;
        return 1;
    }

    delete t;
    return ret;
}
int CheckHifDescription::visitReference(Reference &o)
{
    int ret = GuideVisitor::visitReference(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitReturn(Return &o)
{
    int ret = GuideVisitor::visitReturn(o);

    Procedure *proc = hif::getNearestParent<Procedure>(&o);
    Function *func  = hif::getNearestParent<Function>(&o);
    StateTable *st  = hif::getNearestParent<StateTable>(&o);

    if (func == nullptr && proc == nullptr && (st == nullptr || st->getFlavour() != hif::pf_thread)) {
        _printError("parent function or procedure or SC_THREAD not found.", o);
        return 1;
    }

    if (func == nullptr) {
        // ensure that no value is returned
        if (o.getValue() != nullptr) {
            _printError("Found a return value inside procedure or SC_THREAD", o);
            return 1;
        }
        return ret;
    }

    Type *tr                                                   = hif::semantics::getSemanticType(o.getValue(), _sem);
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo res = _sem->getExprType(func->getType(), tr, op_conv, &o);
    if (res.returnedType == nullptr) {
        _printError(
            "Type mismatch between returned value type "
            "and parent function return type.",
            o);
        messageDebug("Returned value type:", tr, _sem);
        messageDebug("Parent function return type:", func->getType(), _sem);

        return 1;
    }

    return ret;
}
int CheckHifDescription::visitIdentifier(Identifier &o)
{
    int ret = GuideVisitor::visitIdentifier(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (!_checkDeclaration(&o))
        return 1;
    if (!_checkSemanticsType(&o))
        return 1;

    return ret;
}
int CheckHifDescription::visitSystem(System &o)
{
    int ret = GuideVisitor::visitSystem(o);

#ifdef PRINT_SYSTEM_OBJECT
    hif::writeFile_unique_xml_file(o.designUnits.front()->getName(), &o);
#endif

    if (!_sem->isLanguageIdAllowed(o.getLanguageID())) {
        _printError("Not allowed language id.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitAlias(Alias &o)
{
    int ret = GuideVisitor::visitAlias(o);

    messageDebugAssert(
        !o.isStandard(),
        "Found standard Alias. This is the first use of this feature. "
        "Remove this check if it is not an error.",
        &o, _sem);

    return ret;
}
int CheckHifDescription::visitAggregate(Aggregate &o)
{
    int ret = GuideVisitor::visitAggregate(o);

    if (o.alts.empty() && o.getOthers() == nullptr) {
        _printError("empty alts list and others not set.", o);
        return 1;
    }

    if (!_checkSemanticsType(&o))
        return 1;
    Type *t    = o.getSemanticType();
    Type *base = hif::semantics::getBaseType(t, false, _sem);
    if (base == nullptr) {
        _printError("unable to get base type of aggregate object.", o);
        return 1;
    }

    String *stringType = dynamic_cast<String *>(base);
    Array *arrType     = dynamic_cast<Array *>(base);
    if (stringType == nullptr && arrType == nullptr && !hif::semantics::isVectorType(base, _sem)) {
        _printError("invalid base type of aggregate.", *base);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitAggregateAlt(AggregateAlt &o)
{
    int ret = GuideVisitor::visitAggregateAlt(o);

    for (BList<Value>::iterator it(o.indices.begin()); it != o.indices.end(); ++it) {
        if (dynamic_cast<Range *>(*it) != nullptr)
            continue;

        Type *valType = hif::semantics::getSemanticType(*it, _sem);
        if (valType == nullptr) {
            _printError("Type not found for aggregate alt index.", o);
            return 1;
        }

        Type *allowed = _sem->isTypeAllowedAsBound(valType);
        if (allowed != nullptr) {
            delete allowed;
            _printError("Type of aggregate alt index is not allowed by semantics.", o);
            return 1;
        }
    }

    return ret;
}
int CheckHifDescription::visitCast(Cast &o)
{
    int ret = GuideVisitor::visitCast(o);

    if (o.getValue() == nullptr) {
        _printError("Op not found.", o);
        return 1;
    }

    if (o.getType() == nullptr) {
        _printError("Type not found.", o);
        return 1;
    }

    if (!_checkSemanticsType(&o))
        return 1;

    Type *vt = hif::semantics::getSemanticType(o.getValue(), _sem);
    if (!_sem->isCastAllowed(vt, o.getType())) {
        _printError("Casts not allowed.", o);
        return 1;
    }

    if (hif::manipulation::isInLeftHandSide(&o)) {
        _printError("Casts cannot be used as target of assignments.", o);
        return 1;
    }

    if (hif::objectIsInSensitivityList(&o) && hif::getNearestParent<ParameterAssign>(&o) == nullptr) {
        _printError("Casts cannot be inside sensitivity lists.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitChar(Char &o)
{
    int ret = GuideVisitor::visitChar(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitSlice(Slice &o)
{
    int ret = GuideVisitor::visitSlice(o);

    if (o.getPrefix() == nullptr) {
        _printError("Prefix not foud.", o);
        return 1;
    }

    if (!_checkSemanticsType(&o))
        return 1;

    Type *prefixType     = hif::semantics::getSemanticType(o.getPrefix(), _sem);
    Type *prefixBaseType = hif::semantics::getBaseType(prefixType, false, _sem);
    if (dynamic_cast<Bit *>(prefixBaseType) == nullptr && dynamic_cast<Bool *>(prefixBaseType) == nullptr &&
        !hif::semantics::isVectorType(prefixBaseType, _sem) && dynamic_cast<Array *>(prefixBaseType) == nullptr &&
        dynamic_cast<Int *>(prefixBaseType) == nullptr && dynamic_cast<String *>(prefixBaseType) == nullptr) {
        _printError("Prefix type of slice not valid.", *o.getPrefix());
        return 1;
    }

    if (dynamic_cast<Bool *>(prefixBaseType) != nullptr) {
        std::uint64_t bw = hif::semantics::spanGetBitwidth(o.getSpan(), _sem);
        if (bw != 1 && bw != 0) {
            _printError("Invalid slice on single bit type", o);
            return 1;
        }
    }

    return ret;
}
int CheckHifDescription::visitStateTable(StateTable &o)
{
    int ret = GuideVisitor::visitStateTable(o);

    messageDebugAssert(
        !o.isStandard(),
        "Found standard StateTable. "
        "This is the first use of this feature. "
        "Remove this check if it is not an error.",
        &o, _sem);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("The name is not found.", o);
        return 1;
    }

    if (!_opt.allowMultipleStates) {
        if (o.states.size() > 1) {
            _printError("Multiple states inside a StateTable are not allowed.", o);
            return 1;
        }
        if (o.edges.size() > 1) {
            _printError("Transition objects are not allowed.", o);
            return 1;
        }
    }

    return ret;
}
int CheckHifDescription::visitString(String &o)
{
    int ret = GuideVisitor::visitString(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitRange(Range &o)
{
    int ret = GuideVisitor::visitRange(o);

    if (!_sem->isRangeDirectionAllowed(o.getDirection())) {
        _printError("Not valid direction.", o);
        return 1;
    }

    const bool isString = dynamic_cast<String *>(o.getParent()) != nullptr;
    if (isString) {
        if (o.getLeftBound() == nullptr && o.getRightBound() != nullptr) {
            if (o.getDirection() != dir_downto) {
                _printError("In mixed case, only max bound can be null.", o);
                return 1;
            }
            return ret;
        } else if (o.getLeftBound() != nullptr && o.getRightBound() == nullptr) {
            if (o.getDirection() != dir_upto) {
                _printError("In mixed case, only max bound can be null.", o);
                return 1;
            }
            return ret;
        } else if (o.getLeftBound() == nullptr && o.getRightBound() == nullptr) {
            return ret;
        }
    } else {
        if (o.getLeftBound() == nullptr) {
            _printError("Not found LBound.", o);
            return 1;
        }

        if (o.getRightBound() == nullptr) {
            _printError("Not found RBound.", o);
            return 1;
        }
    }

    if (hif::rangeIsValue(&o))
        return ret;

    if (o.getLeftBound() != nullptr) {
        Type *lType = hif::semantics::getSemanticType(o.getLeftBound(), _sem);
        if (lType == nullptr) {
            _printError("Cannot type LBound.", o);
            return 1;
        }

        Type *resL = _sem->isTypeAllowedAsBound(lType);
        if (resL != nullptr) {
            delete resL;
            _printError("Type of LBound is not allowed by semantics.", o);
            return 1;
        }
    }

    if (o.getRightBound() != nullptr) {
        Type *rType = hif::semantics::getSemanticType(o.getRightBound(), _sem);
        if (rType == nullptr) {
            _printError("Cannot type RBound.", o);
            return 1;
        }

        Type *resR = _sem->isTypeAllowedAsBound(rType);
        if (resR != nullptr) {
            delete resR;
            _printError("Type of RBound is not allowed by semantics.", o);
            return 1;
        }
    }

    return ret;
}
int CheckHifDescription::visitSwitch(Switch &o)
{
    int ret = GuideVisitor::visitSwitch(o);

    if (o.getCaseSemantics() != hif::CASE_LITERAL && _sem->getSemanticsOptions().case_isOnlyLiteral) {
        _printError("Not allowed case semantics", o);
        return 1;
    }

    Type *t = hif::semantics::analyzePrecisionType(&o, _sem);
    if (t == nullptr) {
        _printError("Can not type switch conditions", o);
        return 1;
    }

    if (!_sem->isTypeAllowedAsCase(t)) {
        _printError("Case type not allowed", o);
        delete t;
        return 1;
    }

    delete t;
    return ret;
}
int CheckHifDescription::visitDesignUnit(DesignUnit &o)
{
    currentDUName = o.getName();
    int ret       = GuideVisitor::visitDesignUnit(o);
    currentDUName = "";

    if (o.views.empty()) {
        _printError("Not found any view.", o);
        return 1;
    }

    if ((_opt.forceSingleView || _semOpt.designUnit_uniqueView) && o.views.size() != 1) {
        _printError("Not supported more then one view.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitLibrary(Library &o)
{
    int ret = GuideVisitor::visitLibrary(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    if (!_checkDeclaration(&o))
        return 1;

    if (o.getInstance() != nullptr && dynamic_cast<Library *>(o.getInstance()) == nullptr) {
        _printError("Library prefix must be a Library.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitLibraryDef(LibraryDef &o)
{
    if (!_opt.checkStandardLibraryDefs && o.isStandard())
        return 0;

    int ret = GuideVisitor::visitLibraryDef(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("The name is not found.", o);
        return 1;
    }

    if (!_sem->isLanguageIdAllowed(o.getLanguageID())) {
        _printError("Not allowed language id.", o);
        return 1;
    }

    if (o.hasCLinkage() && o.getLanguageID() != hif::cpp) {
        _printError("Not allowed C linkage with non-C++ language.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitMember(Member &o)
{
    int ret = GuideVisitor::visitMember(o);

    if (o.getIndex() == nullptr) {
        _printError("Found member without index.", o);
        return 1;
    }

    if (o.getPrefix() == nullptr) {
        _printError("Prefix not foud.", o);
        return 1;
    }

    if (!_checkSemanticsType(&o))
        return 1;

    Type *valType = hif::semantics::getSemanticType(o.getIndex(), _sem);
    if (valType == nullptr) {
        _printError("Type not found for member index.", o);
        return 1;
    }

    Type *allowed = _sem->isTypeAllowedAsBound(valType);
    if (allowed != nullptr) {
        delete allowed;
        _printError("Type of member index is not allowed by semantics.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitTypeReference(TypeReference &o)
{
    int ret = GuideVisitor::visitTypeReference(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("The name is not set.", o);
        return 1;
    }

    if (!_checkDeclaration(&o))
        return 1;

    if (o.getInstance() != nullptr && dynamic_cast<Library *>(o.getInstance()) == nullptr &&
        dynamic_cast<ViewReference *>(o.getInstance()) == nullptr) {
        _printError("Typereference prefix must be a Library or ViewReference.", o);
        return 1;
    }

    if (!_checkInstantiate(&o))
        return 1;

    return ret;
}
int CheckHifDescription::visitArray(Array &o)
{
    int ret = GuideVisitor::visitArray(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    if (o.getSpan() == nullptr) {
        _printError("Span not found.", o);
        return 1;
    }

    if (o.getType() == nullptr) {
        _printError("Type not found.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitBitvector(Bitvector &o)
{
    int ret = GuideVisitor::visitBitvector(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    if (o.getSpan() == nullptr) {
        _printError("Span not found.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitForGenerate(ForGenerate &o)
{
    int ret = GuideVisitor::visitForGenerate(o);

    if (_semOpt.generates_isNoAllowed) {
        _printError("object now allowed.", o);
        return 1;
    }

    if (!o.initDeclarations.empty() && !o.initValues.empty()) {
        _printError("init.declarations and init.values lists not mutually exclusive.", o);
        return 1;
    }

    if (_semOpt.for_implictIndex && (o.initDeclarations.size() != 1 || !o.initValues.empty())) {
        _printError("Index must be one.", o);
        return 1;
    }

    if (o.getCondition() == nullptr) {
        _printError("condition not found.", o);
        return 1;
    }

    if (dynamic_cast<Range *>(o.getCondition()) != nullptr) {
        if (_semOpt.for_conditionType == hif::semantics::ILanguageSemantics::SemanticOptions::EXPRESSION) {
            _printError("condition must be an expression.", o);
            return 1;
        }
    } else {
        if (_semOpt.for_conditionType == hif::semantics::ILanguageSemantics::SemanticOptions::RANGE) {
            _printError("condition must be an range.", o);
            return 1;
        }

        Type *tc = hif::semantics::getSemanticType(o.getCondition(), _sem);
        if (tc == nullptr && dynamic_cast<Range *>(o.getCondition()) == nullptr) {
            _printError("unable to type condition", o);
            return 1;
        }

        if (!_sem->checkCondition(tc, &o)) {
            _printError("condition type is not valid.", o);
            return 1;
        }
    }

    return ret;
}
int CheckHifDescription::visitIfGenerate(IfGenerate &o)
{
    int ret = GuideVisitor::visitIfGenerate(o);

    if (_semOpt.generates_isNoAllowed) {
        _printError("object now allowed.", o);
        return 1;
    }

    if (o.getCondition() == nullptr) {
        _printError("condition not found.", o);
        return 1;
    }

    Type *tc = hif::semantics::getSemanticType(o.getCondition(), _sem);
    if (tc == nullptr) {
        _printError("unable to type condition", o);
        return 1;
    }

    if (!_sem->checkCondition(tc, &o)) {
        _printError("condition type is not valid.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitFor(For &o)
{
    int ret = GuideVisitor::visitFor(o);

    if (!o.initDeclarations.empty() && !o.initValues.empty()) {
        _printError("Init.declarations and init.values lists not mutually exclusive.", o);
        return 1;
    }

    if (o.getCondition() == nullptr) {
        _printError("condition not found.", o);
        return 1;
    }

    if (dynamic_cast<Range *>(o.getCondition()) != nullptr) {
        if (_semOpt.for_conditionType == hif::semantics::ILanguageSemantics::SemanticOptions::EXPRESSION) {
            _printError("condition must be an expression.", o);
            return 1;
        }
    } else {
        if (_semOpt.for_conditionType == hif::semantics::ILanguageSemantics::SemanticOptions::RANGE) {
            _printError("condition must be an range.", o);
            return 1;
        }

        Type *tc = hif::semantics::getSemanticType(o.getCondition(), _sem);
        if (tc == nullptr && dynamic_cast<Range *>(o.getCondition()) == nullptr) {
            _printError("unable to type condition", o);
            return 1;
        }

        if (!_sem->checkCondition(tc, &o)) {
            _printError("condition type is not valid.", o);
            return 1;
        }
    }

    return ret;
}
int CheckHifDescription::visitWhile(While &o)
{
    int ret = GuideVisitor::visitWhile(o);

    if (o.getCondition() == nullptr) {
        _printError("condition not found.", o);
        return 1;
    }

    Type *tc = hif::semantics::getSemanticType(o.getCondition(), _sem);
    if (tc == nullptr) {
        _printError("unable to type condition", o);
        return 1;
    }

    if (!_sem->checkCondition(tc, &o)) {
        _printError("condition type is not valid.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitView(View &o)
{
    if (!_opt.checkStandardLibraryDefs && o.isStandard())
        return 0;

    int ret = GuideVisitor::visitView(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (o.getEntity() == nullptr) {
        _printError("entity not found.", o);
        return 1;
    }

    if (!_sem->isLanguageIdAllowed(o.getLanguageID())) {
        _printError("Not allowed language id.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitViewReference(ViewReference &o)
{
    int ret = GuideVisitor::visitViewReference(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (o.getDesignUnit().empty() || o.getDesignUnit() == hif::NameTable::getInstance()->none()) {
        _printError("unit name not found.", o);
        return 1;
    }

    if (!_checkDeclaration(&o))
        return 1;

    ViewReference::DeclarationType *deco = hif::semantics::getDeclaration(&o, _sem);
    if (deco != nullptr) {
        DesignUnit *du = dynamic_cast<DesignUnit *>(deco->getParent());
        if (du == nullptr) {
            _printError("wrong declaration parent.", o);
            return 1;
        }

        if (deco->getName() != o.getName() || du->getName() != o.getDesignUnit()) {
            messageDebug("Found declaration is ", deco, _sem);
            _printError("wrong declaration.", o);
            return 1;
        }
    }

    if (o.getInstance() != nullptr && dynamic_cast<Library *>(o.getInstance()) == nullptr &&
        dynamic_cast<ViewReference *>(o.getInstance()) == nullptr) {
        _printError("ViewReference prefix must be a Library or ViewReference.", o);
        return 1;
    }

    if (!_checkInstantiate(&o))
        return 1;

    return ret;
}
int CheckHifDescription::visitWait(Wait &o)
{
    int ret = GuideVisitor::visitWait(o);

    if (o.getTime() != nullptr) {
        Type *tc = hif::semantics::getSemanticType(o.getTime(), _sem);
        if (tc == nullptr) {
            _printError("Unable to type time", o);
            return 1;
        }

        if (dynamic_cast<Time *>(tc) == nullptr) {
            _printError("Time value's type is not Time.", o);
            return 1;
        }
    }

    if (o.getCondition() != nullptr) {
        Type *tc = hif::semantics::getSemanticType(o.getCondition(), _sem);
        if (tc == nullptr) {
            _printError("Unable to type condition", o);
            return 1;
        }

        if (!_sem->checkCondition(tc, &o)) {
            _printError("condition type is not valid.", o);
            return 1;
        }
    }

    if (o.getRepetitions() != nullptr) {
        Type *tc = hif::semantics::getSemanticType(o.getRepetitions(), _sem);
        if (tc == nullptr) {
            _printError("Unable to type repetitions", o);
            return 1;
        }

        Int ii;
        Type *ti = _sem->getMapForType(&ii);
        hif::EqualsOptions opt;
        opt.checkOnlyTypes     = true;
        const bool eqClassName = hif::equals(ti, tc, opt);
        delete ti;
        if (!eqClassName) {
            _printError("Repetition type is not valid.", o);
            return 1;
        }
    }

    if (!o.actions.empty()) {
        if (!_semOpt.waitWithActions) {
            _printError("Actions inside wait are not valid.", o);
            return 1;
        }

        ProcessFlavour flavour;
        const auto found = hif::objectGetProcessFlavour(&o, flavour);
        const bool isRtl = !found || flavour != pf_analog;
        if (isRtl) {
            _printError("Actions inside wait are not valid for RTL processes.", o);
            return 1;
        }
    }

    return ret;
}
int CheckHifDescription::visitEnum(Enum &o)
{
    int ret = GuideVisitor::visitEnum(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    if (_semOpt.scopedType_insideTypedef && dynamic_cast<TypeDef *>(o.getParent()) == nullptr) {
        _printError("parent of enum must be a typedef", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitEnumValue(EnumValue &o)
{
    int ret = GuideVisitor::visitEnumValue(o);

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("name not found.", o);
        return 1;
    }

    if (o.getType() == nullptr) {
        _printError("type not found.", o);
        return 1;
    }

    Enum *e = dynamic_cast<Enum *>(o.getParent());
    if (e == nullptr) {
        _printError("parent enum not found.", o);
        return 1;
    }

    if (_semOpt.scopedType_insideTypedef) {
        TypeDef *td = dynamic_cast<TypeDef *>(e->getParent());
        if (td == nullptr) {
            _printError("enum must be inside a typedef.", o);
            return 1;
        }

        TypeReference *tr = dynamic_cast<TypeReference *>(o.getType());
        if (tr == nullptr || tr->getName() != td->getName()) {
            _printError("type cannot be a typeref to the typedef of parent enum.", o);
            return 1;
        }
    }

    if (o.getValue() != nullptr) {
        Type *tiv = hif::semantics::getSemanticType(o.getValue(), _sem);
        if (tiv == nullptr) {
            _printError("unable to type enum value initial value.", o);
            return 1;
        }

        if (!_checkDeclInitialValAssign(o))
            return 1;
    }

    return ret;
}

int CheckHifDescription::visitEvent(Event &o)
{
    int ret = GuideVisitor::visitEvent(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitTime(Time &o)
{
    int ret = GuideVisitor::visitTime(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("Type now allowed.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitTimeValue(TimeValue &o)
{
    int ret = GuideVisitor::visitTimeValue(o);

    if (!_checkSemanticsType(&o))
        return 1;
    if (!_checkSyntacticType(o))
        return 1;

    return ret;
}

int CheckHifDescription::visitTransition(Transition &o)
{
    int ret = GuideVisitor::visitTransition(o);

    auto st = o.getParent<StateTable>();
    if (st == nullptr) {
        _printError("Parent fo Transition object must be a StateTable.", o);
        return 1;
    }
    if (!st->edges.contains(&o)) {
        _printError("Transition objects must be inside the edges list of a StateTable.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitTypeDef(TypeDef &o)
{
    int ret = GuideVisitor::visitTypeDef(o);

    Enum *e   = dynamic_cast<Enum *>(o.getType());
    Record *r = dynamic_cast<Record *>(o.getType());

    if ((e != nullptr || r != nullptr)) {
        if (!o.isOpaque()) {
            _printError("Typedef of enum and records must be opaque.", o);
            return 1;
        }

        if (o.getRange() != nullptr) {
            _printError("typedef of enum and records can not have ranges.", o);
            return 1;
        }
    }

    if (o.isExternal()) {
        if (r == nullptr || !r->fields.empty()) {
            _printError("External typedef must be an empty record.", o);
            return 1;
        }
    }

    messageDebugAssert(
        !o.isStandard(),
        "Found standard TypeDef. This is the first use of this feature. Remove this check if it is not an error.", &o,
        _sem);

    return ret;
}
int CheckHifDescription::visitFieldReference(FieldReference &o)
{
    int ret = GuideVisitor::visitFieldReference(o);

    if (o.getPrefix() == nullptr) {
        _printError("prefix not found.", o);
        return 1;
    }

    if (o.getName().empty() || o.getName() == hif::NameTable::getInstance()->none()) {
        _printError("field name not set.", o);
        return 1;
    }

    if (!_checkSemanticsType(&o))
        return 1;
    if (!_checkDeclaration(&o))
        return 1;

    return ret;
}

int CheckHifDescription::visitFile(File &o)
{
    GuideVisitor::visitFile(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    return 0;
}

int CheckHifDescription::visitUnsigned(Unsigned &o)
{
    int ret = GuideVisitor::visitUnsigned(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    if (o.getSpan() == nullptr) {
        _printError("Span not found.", o);
        return 1;
    }

    return ret;
}

int CheckHifDescription::visitValueStatement(ValueStatement &o)
{
    int ret = GuideVisitor::visitValueStatement(o);

    if (_semOpt.valueStatement_isNoAllowed) {
        _printError("object now allowed.", o);
        return 1;
    }

    if (o.getValue() == nullptr) {
        _printError("Value not found.", o);
        return 1;
    }

    return ret;
}

int CheckHifDescription::visitSigned(Signed &o)
{
    int ret = GuideVisitor::visitSigned(o);

    if (!_sem->isTypeAllowed(&o)) {
        _printError("type now allowed.", o);
        return 1;
    }

    if (o.getSpan() == nullptr) {
        _printError("Span not found.", o);
        return 1;
    }

    return ret;
}
int CheckHifDescription::visitTypeTPAssign(TypeTPAssign &o)
{
    int ret = GuideVisitor::visitTypeTPAssign(o);

    if (!_checkDeclaration(&o))
        return 1;

    if (o.getType() == nullptr) {
        _printError("Type not set.", o);
        return 1;
    }

    return ret;
}
bool CheckHifDescription::_checkDeclInitialValAssign(DataDeclaration &o)
{
    Type *ts = hif::semantics::getSemanticType(o.getValue(), _sem);
    Type *tt = o.getType();
    if (ts == nullptr) {
        _printError("unable to type init value.", o);
        return false;
    }

    if (tt == nullptr) {
        _printError("Not found data decl type.", o);
        return false;
    }

    Type *bts_op = hif::semantics::getBaseType(ts, true, _sem);
    Type *btt_op = hif::semantics::getBaseType(tt, true, _sem);

    if (hif::equals(bts_op, btt_op))
        return true;

    Type *bts = hif::semantics::getBaseType(ts, false, _sem);
    Type *btt = hif::semantics::getBaseType(tt, false, _sem);

    hif::semantics::ILanguageSemantics::ExpressionTypeInfo res = _sem->getExprType(btt, bts, op_assign, &o);

    if (res.returnedType == nullptr) {
        _printError("decl and initial value have incompatible type.", o);
        return false;
    }

    return true;
}
bool CheckHifDescription::_checkSyntacticType(ConstValue &o)
{
    const bool needed = hif::manipulation::needSyntacticType(&o);
    if (needed && o.getType() == nullptr) {
        _printError("Syntactic type not present.", o);
        return false;
    } else if (!needed && o.getType() != nullptr) {
        _printError("Unexpected syntactic type.", o);
        return false;
    }

    if (o.getType() != nullptr) {
        if (!_sem->isTypeAllowedForConstValue(&o, o.getType())) {
            _printError("Syntactic type not allowed for const value.", o);
            return false;
        }

        if (!hif::typeIsConstexpr(o.getType(), _sem)) {
            _printError("Syntactic type of const value is not constexpr", o);
            return false;
        }
    }

    return true;
}

bool CheckHifDescription::_checkLogicValues(Object *o, const std::string &s, const bool isLogic)
{
    for (std::string::const_iterator i = s.begin(); i != s.end(); ++i) {
        switch (*i) {
        case '0':
        case '1':
            break;

        case '-':
            if (_semOpt.lang_hasDontCare)
                break;
            _printError("Invalid logic value.", *o);
            return false;

        case 'X':
        case 'Z':
            if (isLogic)
                break;
            _printError("Invalid logic value.", *o);
            return false;

        case 'L':
        case 'H':
        case 'W':
        case 'U':
            if (isLogic && _semOpt.lang_has9logic)
                break;
            _printError("Invalid logic value.", *o);
            return false;

        default:
            _printError("Invalid logic value.", *o);
            return false;
        }
    }

    return true;
}

bool CheckHifDescription::_checkSemanticsType(TypedObject *o)
{
    Type *t = hif::semantics::getSemanticType(o, _sem);
    if (t == nullptr) {
        _printError("semantics type not found.", *o);
        return false;
    }

    if (!_opt.checkSemanticTypeSymbols)
        return true;

    ObjectList symbolList;
    if (!_checkSemTypeVisitor.checkSemanticsType(t, symbolList)) {
        _printError(
            "in object semantics type found at least one symbol which declarations "
            "is not in tree.",
            *o, "Involved symbols", symbolList);
        return false;
    }

    return true;
}

template <typename T> bool CheckHifDescription::_checkDeclaration(T *o)
{
    typename T::DeclarationType *decl = getDeclaration(o, _sem);
    if (decl == nullptr) {
        Instance *inst = dynamic_cast<Instance *>(o);
        if (inst != nullptr && dynamic_cast<Library *>(inst->getReferencedType()) != nullptr) {
            return true;
        }

        _printError("Declaration not found.", *o);
        return false;
    }

    if (!hif::isInTree(decl)) {
        _printError("Declaration not in tree.", *o);
        return false;
    }

    const bool unsupported = decl->checkProperty(PROPERTY_UNSUPPORTED);
    if (unsupported) {
        _printError("Declaration not supported.", *o);
        return false;
    }

    return true;
}

template <typename T> bool CheckHifDescription::_checkInstantiate(T *o)
{
    if (!_opt.checkInstantiate)
        return true;

    typename T::DeclarationType *ret = hif::manipulation::instantiate(o, _sem);

    if (ret != nullptr)
        return true;

    _printError("Symbol instantiation failed.", *o);
    return false;
}

void CheckHifDescription::_printError(const std::string &message, Object &o)
{
    if (_opt.exitOnErrors) {
        messageError(message, &o, _sem);
    } else {
        const std::string &msg("CHECK HIF - ERROR! " + message);
        messageDebug(msg, &o, _sem);
    }
}

void CheckHifDescription::_printError(
    const std::string &message,
    Object &o,
    const std::string &listMessage,
    const ObjectList &list)
{
    if (listMessage.empty())
        _printError(message, o);

    std::string msg(message);
    if (!_opt.exitOnErrors) {
        msg = "CHECK HIF - ERROR! " + message;
    }

    messageWarning(msg, &o, _sem);
    std::int64_t index = 1;
    for (ObjectList::const_iterator i = list.begin(); i != list.end(); ++i, ++index) {
        Object *involved = *i;
        std::stringstream ss;
        ss << index;
        std::string listMessageIndex(listMessage + " #" + ss.str());
        messageWarning(listMessageIndex, involved, _sem);
    }

    if (_opt.exitOnErrors) {
        assert(false);
        exit(1);
    }
}

bool CheckHifDescription::_checkDeclRequiredInitialVal(DataDeclaration *o, const bool checkFlag)
{
    // Initial value is required unless type is viewref.
    // When introducing constructors, remember to enable this check.
    Type *t         = getBaseType(o->getType(), false, _sem, true);
    Type *innerType = hif::typeGetNestedType(t, _sem);
    const bool checkInitVal =
        (dynamic_cast<ViewReference *>(innerType) == nullptr) && (dynamic_cast<Event *>(innerType) == nullptr);

    if (checkInitVal && checkFlag && o->getValue() == nullptr) {
        _printError("default value not found.", *o);
        return false;
    }

    return true;
}

int CheckHifDescription::_checkDontCares(Value &o)
{
    // May be in expression or in blist.
    Object *parent = hif::getParentSkippingCasts(&o);

    Expression *e = dynamic_cast<Expression *>(parent);
    if (e != nullptr && (e->getOperator() == op_eq || e->getOperator() == op_case_eq || e->getOperator() == op_neq ||
                         e->getOperator() == op_case_neq))
        return 0; // OK

    Assign *assign = dynamic_cast<Assign *>(parent);
    if (assign != nullptr)
        return 0; // OK

    Return *returnStm = dynamic_cast<Return *>(parent);
    if (returnStm != nullptr)
        return 0; // OK

    ParameterAssign *par = dynamic_cast<ParameterAssign *>(parent);
    if (par != nullptr) {
        Parameter *parDecl = getDeclaration(par, _sem);
        if (parDecl == nullptr) {
            _printError("Cannot find parameter assign (with dontcare value) declaration", o);
            return 1;
        }

        if (hif::declarationIsPartOfStandard(parDecl))
            return 0; // OK
    }

    SwitchAlt *sa   = dynamic_cast<SwitchAlt *>(parent);
    WithAlt *wa     = dynamic_cast<WithAlt *>(parent);
    const bool inSw = sa != nullptr && hif::isSubNode(&o, sa->conditions);
    const bool inWa = wa != nullptr && hif::isSubNode(&o, wa->conditions);
    if (inSw || inWa) {
        return 0; // OK!
    }

    _printError("Bitval dontcare is not in an expression or in a valid blist", o);
    return 1;
}
// ////////////////////////////////////////////////////////////////////////////
// CheckAliases
// ////////////////////////////////////////////////////////////////////////////
class CheckAliasesVisitor : public hif::GuideVisitor
{
public:
    CheckAliasesVisitor(hif::semantics::ILanguageSemantics *sem);
    virtual ~CheckAliasesVisitor();

    virtual bool BeforeVisit(Object &o);
    std::list<Object *> &getAliasedObjects();

private:
    CheckAliasesVisitor(const CheckAliasesVisitor &);
    CheckAliasesVisitor &operator=(const CheckAliasesVisitor &);

    std::set<Object *> _collectedObjects;
    std::list<Object *> _aliasedObjects;
    hif::semantics::ILanguageSemantics *_sem;
};

CheckAliasesVisitor::CheckAliasesVisitor(hif::semantics::ILanguageSemantics *sem)
    : GuideVisitor()
    , _collectedObjects()
    , _aliasedObjects()
    , _sem(sem)
{
}

CheckAliasesVisitor::~CheckAliasesVisitor() {}

bool CheckAliasesVisitor::BeforeVisit(Object &o)
{
    std::set<Object *>::iterator it(_collectedObjects.find(&o));
    if (it == _collectedObjects.end()) {
        _collectedObjects.insert(&o);
        return false;
    }

    _aliasedObjects.push_back(&o);
    messageWarning("Critical: aliased pointer found!", &o, _sem);
    return true;
}

std::list<Object *> &CheckAliasesVisitor::getAliasedObjects() { return _aliasedObjects; }

int _checkAliases(Object &o, hif::semantics::ILanguageSemantics *sem)
{
    CheckAliasesVisitor vis(sem);
    o.acceptVisitor(vis);

    return 0;
}

} // end anonymous namespace
CheckOptions::CheckOptions()
    : checkAliases(false)
    , checkOnCopy(false)
    , checkSimplifiedTree(false)
    , checkMatchOfSimplifiedTree(false)
    , checkFlushingCaches(false)
    , checkStandardLibraryDefs(false)
    , checkInstantiate(false)
    , checkSemanticTypeSymbols(true)
    , // TODO: set false
    exitOnErrors(false)
    , forceSingleView(true)
    , allowMultipleStates(false)
{
    // Nothing to do.
}

CheckOptions::~CheckOptions()
{
    // Nothing to do.
}

CheckOptions::CheckOptions(const CheckOptions &o)
    : checkAliases(o.checkAliases)
    , checkOnCopy(o.checkOnCopy)
    , checkSimplifiedTree(o.checkSimplifiedTree)
    , checkMatchOfSimplifiedTree(o.checkMatchOfSimplifiedTree)
    , checkFlushingCaches(o.checkFlushingCaches)
    , checkStandardLibraryDefs(o.checkStandardLibraryDefs)
    , checkInstantiate(o.checkInstantiate)
    , checkSemanticTypeSymbols(o.checkSemanticTypeSymbols)
    , exitOnErrors(o.exitOnErrors)
    , forceSingleView(o.forceSingleView)
    , allowMultipleStates(o.allowMultipleStates)
{
    // Nothing to do.
}
CheckOptions &CheckOptions::operator=(const CheckOptions &o)
{
    if (this == &o)
        return *this;

    checkAliases               = o.checkAliases;
    checkOnCopy                = o.checkOnCopy;
    checkSimplifiedTree        = o.checkSimplifiedTree;
    checkMatchOfSimplifiedTree = o.checkMatchOfSimplifiedTree;
    checkFlushingCaches        = o.checkFlushingCaches;
    checkStandardLibraryDefs   = o.checkStandardLibraryDefs;
    checkInstantiate           = o.checkInstantiate;
    checkSemanticTypeSymbols   = o.checkSemanticTypeSymbols;
    exitOnErrors               = o.exitOnErrors;
    forceSingleView            = o.forceSingleView;
    allowMultipleStates        = o.allowMultipleStates;

    return *this;
}
int checkHif(Object *o, ILanguageSemantics *sem, const CheckOptions &opt)
{
    Object *tree          = o;
    const bool canReplace = (o->getParent() != nullptr);
    if (opt.checkOnCopy) {
        hif::CopyOptions copyOpt;
        copyOpt.copySemanticsTypes = true;
        tree                       = hif::copy(o, copyOpt);
        if (canReplace)
            o->replace(tree);
    }

    if (opt.checkFlushingCaches) {
        hif::manipulation::flushInstanceCache();
        hif::semantics::flushTypeCacheEntries();
    }

    resetTypes(tree, true);
    resetDeclarations(tree);

    int ret = 0;

    if (opt.checkSimplifiedTree) {
        Object *original = nullptr;
        if (opt.checkMatchOfSimplifiedTree) {
            original = hif::copy(tree);
        }

        hif::manipulation::simplify(tree, sem);

        if (opt.checkMatchOfSimplifiedTree) {
            typedef std::map<Object *, Object *> MatchMap;
            MatchMap matched;
            MatchMap unmatched;
            hif::manipulation::matchTrees(tree, original, matched, unmatched);
            for (MatchMap::iterator it = unmatched.begin(); it != unmatched.end(); ++it) {
                std::clog << "\n\n===============================================================\n\n";
                messageWarning("Simplified unmatched object.", it->first, sem);
                messageWarning("\n\nOriginal unmatched object.", it->second, sem);
            }

            delete original;
            ret |= (!matched.empty() || !unmatched.empty());
        }
    }

    if (opt.checkAliases) {
        ret |= _checkAliases(*o, sem);
    }

    CheckHifDescription v(sem, opt);
    ret |= tree->acceptVisitor(v);

    if (opt.checkOnCopy && canReplace) {
        tree->replace(o);
        delete tree;
    }

    return ret;
}

int checkNativeHif(Object *o, ILanguageSemantics *sem, const CheckOptions &opt)
{
    CheckOptions options(opt);
    options.checkStandardLibraryDefs = false;
    const bool restore               = sem->useNativeSemantics();
    sem->setUseNativeSemantics(true);
    const int ret = checkHif(o, sem, options);
    sem->setUseNativeSemantics(restore);
    return ret;
}

} // namespace semantics
} // namespace hif
