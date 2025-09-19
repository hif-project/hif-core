/// @file removeStandardMethods.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cmath>

#include "hif/GuideVisitor.hpp"
#include "hif/HifFactory.hpp"
#include "hif/manipulation/removeStandardMethods.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

namespace hif
{
namespace manipulation
{

namespace /* anon */
{
typedef std::map<std::string, std::string> ViewClocksSupportSignals;
typedef std::pair<std::string, Object *> UnsupportedObject;
typedef std::list<UnsupportedObject> Unsupported;
class RemoveStandardMethodsVisitor : public GuideVisitor
{
public:
    RemoveStandardMethodsVisitor(hif::semantics::ILanguageSemantics *sem, const RemoveStandardMethodsOptions &opt);

    virtual ~RemoveStandardMethodsVisitor();

    virtual int visitSystem(System &o);
    virtual int visitLibraryDef(LibraryDef &o);
    virtual int visitView(View &o);
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitProcedureCall(ProcedureCall &o);
    virtual int visitTypeReference(TypeReference &o);
    virtual int visitIdentifier(Identifier &o);
    virtual int visitFieldReference(FieldReference &o);

    /// @brief Return the class field _isFixed value.
    bool isFixed();
    /// @brief Return the class field _isFixed value.
    Unsupported &getUnsupportedObjects();
    /// @brief Prints unsupported objects.
    void printUnsupported();

protected:
    /// @brief HifFactory object useful for some static methods.
    HifFactory _factory;

    /// @brief The system root.
    hif::System *_system;

    /// @brief The reference semantics.
    hif::semantics::ILanguageSemantics *_sem;

    const RemoveStandardMethodsOptions &_opt;

    DeclarationIsPartOfStandardOptions _declCheckStd;
    DeclarationIsPartOfStandardOptions _declCheckSystemC;
    DeclarationIsPartOfStandardOptions _declCheckSystemCAMS;
    DeclarationIsPartOfStandardOptions _declCheckSupport;

    /// @brief Current Contents.
    hif::Scope *_currentScope;

    /// @brief Map of <clock name, support signal name> of the current view.
    ViewClocksSupportSignals _vcss;

    /// @brief The name of the Signal needed to simulate the risign_edge function.
    std::string _clkSupportSignal;

    /// @brief If true at least a fix has been computed.
    bool _isFixed;
    /// @brief List of unsupported objects.
    Unsupported _unsupportedObjects;

    /// @brief Create the clock support signal.
    void _createClkSupportSignal(DataDeclaration *clockDecl);

    /// @brief Create the process that updates the clock support signal.
    void _createClkSupportSignalUpdProc(Value *clk);

    bool _checkUnsupported(bool flag, const std::string &msg, Object *o);

    hif::Trash _trash;

private:
    bool _fixEventCall(FunctionCall *fc);
    bool _fixConvFunctionCall(FunctionCall *fc);
    bool _fixCastRealToInt(FunctionCall *fc);
    bool _fixLastValue(FunctionCall *fc);
    bool _fixConvUnsigned(FunctionCall *fc);
    bool _fixSignExtend(FunctionCall *fc);
    bool _fixShr(FunctionCall *fc);
    bool _fixSystemTime(FunctionCall *fc);
    bool _fixIteratedConcat(FunctionCall *fc);
    bool _fixSystemFopen(FunctionCall *fc);
    bool _fixSystemRandom(FunctionCall *fc);
    bool _fixSystemFEof(FunctionCall *fc);
    bool _fixNew(FunctionCall *fc);
    bool _fixEdgeEvent(FunctionCall *fc);
    bool _fixIsX(FunctionCall *fc);
    bool _fixVhdlMatch(FunctionCall *fc);
    bool _fixCStr(FunctionCall *fc);
    bool _fixFOpen(FunctionCall *fc);
    bool _fixExt(FunctionCall *fc);
    bool _fixConvStdLogicVector(FunctionCall *fc);
    bool _fixVhdlEndfile(FunctionCall *fc);
    bool _fixSystemRealTime(FunctionCall *fc);
    bool _fixVhdlTrunc(FunctionCall *fc);

    bool _allowSCFunctions(FunctionCall *fc);
    bool _allowVerilogAmsFunctions(FunctionCall *fc);

    bool _fixVhdlAssert(ProcedureCall *pc);
    bool _fixSystemDisplay(ProcedureCall *pc);
    bool _fixSystemFDisplay(ProcedureCall *pc);
    bool _fixSystemFScanF(ProcedureCall *pc);
    bool _fixSystemReadMemH(ProcedureCall *pc);
    bool _fixSystemFinish(ProcedureCall *pc);
    bool _fixSystemFwrite(ProcedureCall *pc);
    bool _fixSystemFclose(ProcedureCall *pc);
    bool _fixSystemFflush(ProcedureCall *pc);
    bool _fixDeallocate(ProcedureCall *pc);
    bool _fixHifLastValue(ProcedureCall *pc);
    bool _fixVhdlWriteline(ProcedureCall *pc);
    bool _fixReadLine(ProcedureCall *pc);
    bool _fixRead(ProcedureCall *pc);
    bool _fixFileClose(ProcedureCall *pc);
    bool _fixVhdlUniform(ProcedureCall *fc);
    bool _fixVhdlHread(ProcedureCall *pc);

    bool _allowSCProcedures(ProcedureCall *pc);
    bool _allowSCAmsProcedures(ProcedureCall *pc);

    bool _fixVhdlAssertSeverity(FieldReference *fr);
    bool _fixVhdlFileOpenKind(FieldReference *fr);
    bool _fixVhdlFileOpenStatus(FieldReference *fr);
    bool _fixVhdlStdIoStreams(FieldReference *fr);
    bool _fixCStdIoStreams(FieldReference *fr);

    bool _fixVhdlTypeReference(TypeReference *o);

    void _checkStandardDecl(Object *obj);
    bool _addHifLibrary(const char *c, bool standard = false);
    bool _sortPrintMethodParameters(ProcedureCall *o, bool isDisplay);
    char _getMod(char n, ParameterAssign *pa, bool &needStringLib, bool &needScCoreLib);
    void _addCast(Value *v, Type *t, bool &needStringLib, bool &needScCoreLib);
    bool _manageFileWriteIOCalls(ProcedureCall *o, bool isDisplay, bool isScan);
    bool _fixPrintMethodParamNames(ProcedureCall *o);

    bool _manageVhdlExtensions(FunctionCall *fc, bool isSigned);

    /// @brief Disabled copy constructor.
    RemoveStandardMethodsVisitor(const RemoveStandardMethodsVisitor &s);

    /// @brief Disabled assignment operator.
    RemoveStandardMethodsVisitor &operator=(const RemoveStandardMethodsVisitor &);
};

RemoveStandardMethodsVisitor::RemoveStandardMethodsVisitor(
    semantics::ILanguageSemantics *sem,
    const RemoveStandardMethodsOptions &opt)
    : _factory(sem)
    , _system()
    , _sem(sem)
    , _opt(opt)
    , _declCheckStd()
    , _declCheckSystemC()
    , _declCheckSystemCAMS()
    , _declCheckSupport()
    , _currentScope(nullptr)
    , _vcss()
    , _clkSupportSignal("")
    , _isFixed(false)
    , _unsupportedObjects()
    , _trash()
{
    _declCheckStd.reset();
    _declCheckSystemC.reset();
    _declCheckSystemCAMS.reset();
    _declCheckSupport.reset();

    _declCheckStd.enableCppFamily(_opt.allowStdLib);
    _declCheckStd.allowCppExtraLibs = false;

    _declCheckSystemC.enableSystemCFamily(_opt.allowSystemC);

    _declCheckSystemCAMS.enableSystemCAMSFamily(_opt.allowSystemCAMS);
    _declCheckSystemCAMS.allowVerilogAMS = _opt.allowSystemCAMS;

    _declCheckSupport.allowVerilog = _opt.allowSupportLibrary;
    _declCheckSupport.enableVhdlFamily(_opt.allowSupportLibrary);
    _declCheckStd.allowCppExtraLibs = _opt.allowSupportLibrary;
}

RemoveStandardMethodsVisitor::~RemoveStandardMethodsVisitor() { _trash.clear(); }

int RemoveStandardMethodsVisitor::visitSystem(System &o)
{
    _system = &o;
    hif::GuideVisitor::visitSystem(o);
    return 0;
}

int RemoveStandardMethodsVisitor::visitLibraryDef(LibraryDef &o)
{
    if (hif::declarationIsPartOfStandard(&o))
        return 0;
    Scope *restore = _currentScope;
    _currentScope  = &o;
    GuideVisitor::visitLibraryDef(o);
    _currentScope = restore;
    return 0;
}

int RemoveStandardMethodsVisitor::visitView(hif::View &o)
{
    if (hif::declarationIsPartOfStandard(&o))
        return 0;
    Scope *restore = _currentScope;
    _currentScope  = &o;
    GuideVisitor::visitView(o);
    _currentScope = restore;
    _vcss.clear();

    return 0;
}

int RemoveStandardMethodsVisitor::visitFunctionCall(hif::FunctionCall &o)
{
    GuideVisitor::visitFunctionCall(o);
    if (_fixEventCall(&o))
        return 0;
    if (_fixConvFunctionCall(&o))
        return 0;
    if (_fixCastRealToInt(&o))
        return 0;
    if (_fixLastValue(&o))
        return 0;
    if (_fixConvUnsigned(&o))
        return 0;
    if (_fixSignExtend(&o))
        return 0;
    if (_fixShr(&o))
        return 0;
    if (_fixSystemTime(&o))
        return 0;
    if (_fixIteratedConcat(&o))
        return 0;
    if (_fixSystemFopen(&o))
        return 0;
    if (_fixSystemRandom(&o))
        return 0;
    if (_fixSystemFEof(&o))
        return 0;
    if (_fixNew(&o))
        return 0;
    if (_fixEdgeEvent(&o))
        return 0;
    if (_fixIsX(&o))
        return 0;
    if (_fixVhdlMatch(&o))
        return 0;
    if (_fixCStr(&o))
        return 0;
    if (_fixFOpen(&o))
        return 0;
    if (_fixExt(&o))
        return 0;
    if (_fixConvStdLogicVector(&o))
        return 0;
    if (_fixVhdlEndfile(&o))
        return 0;
    if (_fixSystemRealTime(&o))
        return 0;
    if (_fixVhdlTrunc(&o))
        return 0;

    if (_allowSCFunctions(&o))
        return 0;
    if (_allowVerilogAmsFunctions(&o))
        return 0;

    _checkStandardDecl(&o);
    return 0;
}

int RemoveStandardMethodsVisitor::visitProcedureCall(ProcedureCall &o)
{
    GuideVisitor::visitProcedureCall(o);
    if (_fixVhdlAssert(&o))
        return 0;
    if (_fixSystemDisplay(&o))
        return 0;
    if (_fixSystemFDisplay(&o))
        return 0;
    if (_fixSystemFScanF(&o))
        return 0;
    if (_fixSystemReadMemH(&o))
        return 0;
    if (_fixSystemFinish(&o))
        return 0;
    if (_fixSystemFwrite(&o))
        return 0;
    if (_fixSystemFclose(&o))
        return 0;
    if (_fixSystemFflush(&o))
        return 0;
    if (_fixDeallocate(&o))
        return 0;
    if (_fixHifLastValue(&o))
        return 0;
    if (_fixVhdlWriteline(&o))
        return 0;
    if (_fixReadLine(&o))
        return 0;
    if (_fixRead(&o))
        return 0;
    if (_fixFileClose(&o))
        return 0;
    if (_fixVhdlUniform(&o))
        return 0;
    if (_fixVhdlHread(&o))
        return 0;

    if (_allowSCProcedures(&o))
        return 0;
    if (_allowSCAmsProcedures(&o))
        return 0;

    _checkStandardDecl(&o);
    return 0;
}

int RemoveStandardMethodsVisitor::visitTypeReference(TypeReference &o)
{
    hif::GuideVisitor::visitTypeReference(o);
    if (_fixVhdlTypeReference(&o))
        return 0;

    if (_opt.allowSupportLibrary)
        return 0;
    _checkStandardDecl(&o);
    return 0;
}

int RemoveStandardMethodsVisitor::visitIdentifier(Identifier &o)
{
    hif::GuideVisitor::visitIdentifier(o);
    if (_opt.allowSupportLibrary)
        return 0;
    _checkStandardDecl(&o);
    return 0;
}

int RemoveStandardMethodsVisitor::visitFieldReference(FieldReference &o)
{
    hif::GuideVisitor::visitFieldReference(o);
    if (_fixVhdlAssertSeverity(&o))
        return 0;
    if (_fixVhdlFileOpenKind(&o))
        return 0;
    if (_fixVhdlFileOpenStatus(&o))
        return 0;
    if (_fixVhdlStdIoStreams(&o))
        return 0;
    if (_fixCStdIoStreams(&o))
        return 0;

    if (_opt.allowSupportLibrary)
        return 0;
    _checkStandardDecl(&o);
    return 0;
}

bool RemoveStandardMethodsVisitor::isFixed() { return _isFixed; }

Unsupported &RemoveStandardMethodsVisitor::getUnsupportedObjects() { return _unsupportedObjects; }

void RemoveStandardMethodsVisitor::printUnsupported()
{
    if (_unsupportedObjects.empty())
        return;

    messageWarning("One or more unsupported mappings have been met.", nullptr, nullptr);
    for (Unsupported::const_iterator it(_unsupportedObjects.begin()); it != _unsupportedObjects.end(); ++it) {
        const std::string &msg = it->first;
        Object *obj            = it->second;
        messageWarning(msg, obj, _sem);
    }
    messageError("Found at least one unsupported object during mapping.", nullptr, nullptr);
}

void RemoveStandardMethodsVisitor::_createClkSupportSignal(DataDeclaration *clockDecl)
{
    _clkSupportSignal = "clkSup";
    _clkSupportSignal += NameTable::getInstance()->getFreshName("");

    // Create the support signal
    Signal *suppSig = new Signal();
    suppSig->setName(_clkSupportSignal);
    suppSig->setType(hif::copy(clockDecl->getType()));
    suppSig->setRange(hif::copy(clockDecl->getRange()));
    if (clockDecl->getValue() != nullptr) {
        suppSig->setValue(hif::copy(clockDecl->getValue()));
    } else {
        Type *tp    = hif::semantics::getBaseType(suppSig->getType(), false, _sem);
        bool native = typeIsConstexpr(tp, _sem);
        if (dynamic_cast<Bit *>(tp) != nullptr) {
            bool resolved = typeIsResolved(tp, _sem);
            if (typeIsLogic(tp, _sem)) {
                suppSig->setValue(_factory.bitval(bit_z, _factory.bit(true, resolved, native)));
            } else {
                suppSig->setValue(_factory.bitval(bit_zero, _factory.bit(false, resolved, native)));
            }
        } else if (dynamic_cast<Bool *>(tp) != nullptr) {
            suppSig->setValue(_factory.boolval(false, _factory.boolean(native)));
        } else {
            messageError("Unable to provide initial value for clock support signal", suppSig, _sem);
        }
    }

    // Get the list of declarations that contains clk
    BList<Declaration> *decls = nullptr;
    Entity *en                = dynamic_cast<Entity *>(clockDecl->getParent());
    if (en != nullptr) {
        View *currView = dynamic_cast<View *>(en->getParent());
        messageAssert(currView != nullptr, "Expected View parent", en, _sem);

        decls = &(currView->getContents()->declarations);
    }
    if (decls == nullptr) {
        decls = reinterpret_cast<BList<Declaration> *>(clockDecl->getBList());
    }
    if (decls == nullptr)
        messageError("Unable to find list of declarations", clockDecl, nullptr);

    // Insert the signal into the declarations list
    decls->push_back(suppSig);
}

void RemoveStandardMethodsVisitor::_createClkSupportSignalUpdProc(Value *clk)
{
    std::string sName("Upd");
    sName += _clkSupportSignal;
    StateTable *stb = new StateTable();
    stb->setName(sName);
    stb->setFlavour(_opt.preferScMethodFlavour ? pf_method : pf_hdl);
    State *st = new State();
    st->setName(sName);
    stb->states.push_back(st);
    stb->setEntryStateName(sName);

    // Set the sensitivity list
    stb->sensitivity.push_back(hif::copy(clk));

    // Set the action
    st->actions.push_back(_factory.assignment(_factory.identifier(_clkSupportSignal.c_str()), hif::copy(clk)));

    // Insert the statetable
    View *view = dynamic_cast<View *>(_currentScope);
    messageAssert(view != nullptr, "nullptr view", nullptr, nullptr);
    Contents *contents = view->getContents();
    messageAssert(contents != nullptr, "nullptr content", nullptr, nullptr);

    contents->stateTables.push_back(stb);
}

bool RemoveStandardMethodsVisitor::_checkUnsupported(bool flag, const std::string &msg, Object *o)
{
    if (flag)
        return false;
    _unsupportedObjects.push_back(std::make_pair(msg, o));
    return true;
}

bool RemoveStandardMethodsVisitor::_fixEventCall(FunctionCall *fc)
{
    /// The function hif_vhdl_rising_edge and hif_vhdl_event
    /// work on (logic) bit signals/ports. Since the resulting design will not
    /// have (logic) bit types, the function calls have to be replaced with
    /// a corresponding implementation.
    /// Solution: Create a signal updated with the clock value in a asyncronous
    /// process (obviusly with the clock in the sensitivity list).
    /// Replace the function call with the expression (clock & (clock != signal)) for
    /// hif_vhdl_rising_edge and simply (clock != signal) for hif_vhdl_event.
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_rising_edge" && fcallName != "hif_vhdl_event" && fcallName != "hif_vhdl_falling_edge")
        return false;

    if (_opt.allowSystemC && _opt.useSystemCEvent)
        return true;

    _isFixed = true;

    Value *clk    = nullptr;
    bool isRising = true;
    if (fcallName.compare("hif_vhdl_rising_edge") == 0) {
        isRising = true;
        clk      = getChildSkippingCasts(fc->parameterAssigns.front()->getValue());
    } else if (fcallName.compare("hif_vhdl_falling_edge") == 0) {
        isRising = false;
        clk      = getChildSkippingCasts(fc->parameterAssigns.front()->getValue());
    } else if (fcallName.compare("hif_vhdl_event") == 0) {
        clk = fc->getInstance();
    } else {
        messageError("Unexpected function call", fc, _sem);
    }
    messageAssert(clk != nullptr, "Unable to find clock identifier", fc, _sem);

    // Supporting members. Ref design: vhdl/unott/dig_proc
    Value *clkPrefix           = hif::getTerminalPrefix(clk);
    // Get the clock declaration
    DataDeclaration *clockDecl = dynamic_cast<DataDeclaration *>(hif::semantics::getDeclaration(clkPrefix, _sem));
    if (clockDecl == nullptr)
        messageError("Declaration not found", clk, nullptr);

    std::string clockName(clockDecl->getName());
    ViewClocksSupportSignals::iterator it = _vcss.find(clockName.c_str());

    if (it != _vcss.end()) {
        _clkSupportSignal = (*it).second;
    } else {
        _createClkSupportSignal(clockDecl);
        _createClkSupportSignalUpdProc(clkPrefix);
        _vcss[clockName] = _clkSupportSignal;
    }

    // Create the substitution value
    Function *fun    = hif::manipulation::instantiate(fc, _sem);
    Type *retType    = fun->getType();
    Value *sig       = hif::copy(clk);
    Identifier *term = dynamic_cast<Identifier *>(hif::getTerminalPrefix(sig));
    term->setName(_clkSupportSignal);
    Expression *expr1 = _factory.expression(hif::copy(clk), op_case_neq, sig);
    if (fcallName.compare("hif_vhdl_event") == 0) {
        fc->replace(_factory.cast(hif::copy(retType), expr1));
    } else if (isRising) {
        Bool b;
        Type *mappedBool = _sem->getMapForType(&b);
        Value *expr2 =
            _factory.cast(mappedBool, _factory.expression(hif::copy(clk), op_case_eq, _factory.bitval(hif::bit_one)));

        Expression *exprAnd = _factory.expression(_factory.cast(hif::copy(mappedBool), expr1), hif::op_and, expr2);

        if (!_opt.useBinaryLogic) {
            Value *expr3 = _factory.cast(
                hif::copy(mappedBool),
                _factory.expression(hif::copy(sig), hif::op_case_eq, _factory.bitval(hif::bit_zero)));
            exprAnd = _factory.expression(exprAnd, hif::op_and, expr3);
        }

        fc->replace(_factory.cast(hif::copy(retType), exprAnd));
    } else {
        Bool b;
        Type *mappedBool = _sem->getMapForType(&b);
        Value *expr2 =
            _factory.cast(mappedBool, _factory.expression(hif::copy(clk), op_case_eq, _factory.bitval(hif::bit_zero)));

        Expression *exprAnd = _factory.expression(_factory.cast(hif::copy(mappedBool), expr1), hif::op_and, expr2);

        if (!_opt.useBinaryLogic) {
            Value *expr3 = _factory.cast(
                hif::copy(mappedBool),
                _factory.expression(hif::copy(sig), hif::op_case_eq, _factory.bitval(hif::bit_one)));
            exprAnd = _factory.expression(exprAnd, hif::op_and, expr3);
        }

        fc->replace(_factory.cast(hif::copy(retType), exprAnd));
    }

    delete fc;
    return true;
}

bool RemoveStandardMethodsVisitor::_fixConvFunctionCall(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_to_x01" && fcallName != "hif_vhdl_to_integer" && fcallName != "hif_vhdl_conv_integer")
        return false;

    if (_opt.useBinaryLogic) {
        _isFixed      = true;
        Value *param  = fc->parameterAssigns.front()->getValue();
        Type *semType = hif::semantics::getSemanticType(fc, _sem);
        Cast *c       = _factory.cast(hif::copy(semType), param);
        fc->replace(c);
        delete fc;
        return true;
    }

    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixVhdlAssert(ProcedureCall *pc)
{
    std::string pcallName(pc->getName());
    if (pcallName != "hif_vhdl_assert")
        return false;

    if (_opt.allowStdLib) {
        messageAssert(!pc->parameterAssigns.empty(), "Unexpected call to assert without parameters", pc, _sem);
        SubProgram *decl = hif::semantics::getDeclaration(pc, _sem);
        messageAssert(decl != nullptr, "Cannot find declaration for procedure call", pc, _sem);
        hif::manipulation::sortParameters(
            pc->parameterAssigns, decl->parameters, true, hif::manipulation::SortMissingKind::ALL, _sem);

        Value *condition =
            _factory.expression(hif::op_not, _factory.cast(_factory.boolean(), pc->parameterAssigns.at(0)->getValue()));
        Value *report = pc->parameterAssigns.at(1)->getValue();
        if (dynamic_cast<StringValue *>(report) != nullptr && static_cast<StringValue *>(report)->getValue() == "") {
            report = nullptr;
        }
        bool error      = false;
        Value *severity = pc->parameterAssigns.at(2)->getValue();
        if ((dynamic_cast<Identifier *>(severity) != nullptr &&
             static_cast<Identifier *>(severity)->matchName("hif_vhdl_failure")) ||
            (dynamic_cast<FieldReference *>(severity) != nullptr &&
             static_cast<FieldReference *>(severity)->matchName("hif_vhdl_failure"))) {
            error = true;
        }

        BList<Action> actions;
        if (report != nullptr) {
            ProcedureCall *printf = _factory.procedureCall(
                "hif_systemc_printf", nullptr, _factory.noTemplateArguments(),
                _factory.parameterArgument("param1", report));
            actions.push_back(printf);
            _addHifLibrary("hif_systemc_cstdio", false);
        }
        if (error) {
            ProcedureCall *errorCall = nullptr;
            if (_opt.allowStdLib && !_opt.allowSystemC) {
                _addHifLibrary("hif_systemc_cstdlib", false);
                errorCall = _factory.procedureCall(
                    "hif_systemc_exit", nullptr, _factory.noTemplateArguments(),
                    _factory.parameterArgument("param1", _factory.identifier("EXIT_FAILURE")));
            } else if (_opt.allowSystemC) {
                _addHifLibrary("hif_systemc_sc_core", false);
                errorCall = _factory.procedureCall(
                    "hif_systemc_sc_stop", _factory.libraryInstance("hif_systemc_sc_core", false, true),
                    _factory.noTemplateArguments(), _factory.noParameterArguments());
            } else {
                if (_checkUnsupported(false, "Mapping requires allowSystemC or allowStdLib", pc))
                    return true;
            }
            actions.push_back(errorCall);
        }

        If *ifStm = _factory.ifStmt(_factory.noActions(), _factory.ifAlt(condition, actions));

        pc->replace(ifStm);
        _trash.insert(pc);
        _isFixed = true;
        return true;
    }
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", pc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixCastRealToInt(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_castRealToInt")
        return false;

    Value *param1 = fc->parameterAssigns.at(0)->getValue();
    Value *param2 = fc->parameterAssigns.at(1)->getValue();
    Value *param3 = fc->parameterAssigns.at(2)->getValue();

    RealValue *param = dynamic_cast<RealValue *>(param1);
    IntValue *size   = dynamic_cast<IntValue *>(param2);
    BoolValue *sign  = dynamic_cast<BoolValue *>(param3);
    messageAssert(sign != nullptr, "Unxepected non constant sign", fc, _sem);
    bool signVal = sign->getValue();

    if (param != nullptr && size != nullptr) {
        double d       = round(param->getValue());
        double sizeVal = static_cast<double>(size->getValue());
        double min     = 0.0;
        double max     = 0.0;
        if (!signVal) {
            min = 0.0;
            max = pow(2.0, sizeVal) - 1;
        } else {
            min = -pow(2.0, sizeVal - 1);
            max = pow(2.0, sizeVal - 1) - 1;
        }

        if (d < min)
            d = min;
        if (d > max)
            d = max;
        std::int64_t dd = static_cast<std::int64_t>(d);

        _isFixed = true;
        Value *v = _factory.intval(dd);
        fc->replace(v);
        delete fc;
        return true;
    }

    Value *min = nullptr;
    Value *max = nullptr;
    if (!signVal) {
        min = _factory.realval(0.0);
        max = _factory.expression(
            _factory.expression(_factory.realval(2.0), hif::op_pow, _factory.cast(_factory.real(), param2)),
            hif::op_minus, _factory.realval(1.0));
    } else {
        min = _factory.expression(
            hif::op_minus,
            _factory.expression(
                _factory.realval(2.0), hif::op_pow,
                _factory.expression(_factory.cast(_factory.real(), param2), hif::op_minus, _factory.realval(1.0))));
        max = _factory.expression(
            _factory.expression(
                _factory.realval(2.0), hif::op_pow,
                _factory.expression(
                    _factory.cast(_factory.real(), hif::copy(param2)), hif::op_minus, _factory.realval(1.0))),
            hif::op_minus, _factory.realval(1.0));
    }
    // Act like c++ Round function
    // if (v > max) : max
    // else if (v < min) : min
    // if v >= 0
    // {
    //  if (v >= (int)v + 0.5) : (int)v + 1
    //  else (int)v
    // }
    // if v < 0
    // {
    //  if (v > (int)v - 0.5) : (int)v
    //  else (int)v - 1
    // }

    When *when   = new When();
    WhenAlt *wa1 = _factory.whenalt(
        _factory.expression(param1, hif::op_ge, max), _factory.cast(_factory.integer(), hif::copy(max)));
    when->alts.push_back(wa1);
    WhenAlt *wa2 = _factory.whenalt(
        _factory.expression(hif::copy(param1), hif::op_le, min), _factory.cast(_factory.integer(), hif::copy(min)));
    when->alts.push_back(wa2);

    Value *trunc      = _factory.cast(_factory.integer(), hif::copy(param1));
    Expression *cond1 = _factory.expression(
        _factory.expression(hif::copy(param1), hif::op_ge, _factory.realval(0.0)), hif::op_and,
        _factory.expression(
            hif::copy(param1), hif::op_ge,
            _factory.expression(_factory.cast(_factory.real(), trunc), hif::op_plus, _factory.realval(0.5))));
    WhenAlt *wa3 = _factory.whenalt(cond1, _factory.expression(hif::copy(trunc), hif::op_plus, _factory.intval(1)));
    when->alts.push_back(wa3);
    Expression *cond1Else = _factory.expression(hif::copy(param1), hif::op_ge, _factory.realval(0.0));
    WhenAlt *wa4          = _factory.whenalt(cond1Else, hif::copy(trunc));
    when->alts.push_back(wa4);
    Expression *cond2 = _factory.expression(
        hif::copy(param1), hif::op_gt,
        _factory.expression(_factory.cast(_factory.real(), hif::copy(trunc)), hif::op_minus, _factory.realval(0.5)));
    WhenAlt *wa5 = _factory.whenalt(cond2, hif::copy(trunc));
    when->alts.push_back(wa5);

    when->setDefault(_factory.expression(hif::copy(trunc), hif::op_minus, _factory.intval(1)));

    fc->replace(when);
    delete fc;
    return true;
}

bool RemoveStandardMethodsVisitor::_fixLastValue(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_last_value" && fcallName != "hif_systemc_hif_lastValue")
        return false;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixConvUnsigned(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_conv_unsigned")
        return false;

    Value *param1 = fc->parameterAssigns.at(0)->getValue();
    Value *param2;
    if (fc->parameterAssigns.size() == 2) {
        param2 = fc->parameterAssigns.at(1)->getValue();
    } else {
        ValueTPAssign *vtpa = dynamic_cast<ValueTPAssign *>(fc->templateParameterAssigns.front());
        param2              = vtpa->getValue();
    }
    if (param1 == nullptr || param2 == nullptr)
        return false;

    Type *paramType = hif::semantics::getBaseType(param1, false, _sem);
    messageAssert(paramType != nullptr, "Cannot type argument", param1, _sem);
    IntValue *size = dynamic_cast<IntValue *>(param2);
    if (size == nullptr) {
        if (_opt.allowSupportLibrary)
            return true;
        messageError("Unsupported function with non-constant size argument", fc, _sem);
    }
    Range *span = _factory.range(size->getValue() - 1, 0);

    bool isLogic = hif::typeIsLogic(paramType, _sem);

    if (isLogic && !_opt.useBinaryLogic) {
        _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary or useBinaryLogic", fc);
        return true;
    }

    if (_checkUnsupported(!isLogic || _opt.useBinaryLogic, "Mapping requires useBinaryLogic", fc))
        return true;

    if (dynamic_cast<Bitvector *>(paramType) != nullptr || dynamic_cast<Int *>(paramType) != nullptr ||
        dynamic_cast<Signed *>(paramType) != nullptr) {
        _isFixed = true;
        hif::typeSetSigned(paramType, false, _sem);
        hif::typeSetSpan(paramType, span, _sem);
        return true;
    } else if (dynamic_cast<Unsigned *>(paramType) != nullptr) {
        _isFixed = true;
        hif::typeSetSpan(paramType, span, _sem);
        return true;
    }

    messageError("Unexpected parameter type", paramType, _sem);
}

bool RemoveStandardMethodsVisitor::_fixSignExtend(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_sxt")
        return false;
    _manageVhdlExtensions(fc, true);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixShr(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_shr")
        return false;

    Value *param1 = fc->parameterAssigns.at(0)->getValue();
    Value *param2 = fc->parameterAssigns.at(1)->getValue();
    if (param1 == nullptr || param2 == nullptr)
        return false;
    BitvectorValue *bvv = dynamic_cast<BitvectorValue *>(param2);
    if (bvv == nullptr) {
        if (_opt.allowSupportLibrary)
            return true;
        messageError("Method mapping supported only in case of constant second param", fc, _sem);
    }

    if (!bvv->is01() && _opt.allowSupportLibrary)
        return true;
    if (_checkUnsupported(bvv->is01() || _opt.useBinaryLogic, "Mapping requires useBinaryLogic", bvv))
        return true;

    bvv->to01();

    Type *argType   = hif::semantics::getBaseType(param1, false, _sem);
    std::int64_t argBw = static_cast<std::int64_t>(hif::semantics::typeGetSpanBitwidth(argType, _sem));
    std::stringstream ss(bvv->getValue());
    std::int64_t count = 0;
    ss >> count;

    Value *sign = nullptr;
    if (hif::typeIsSigned(argType, _sem)) {
        sign = _factory.member(hif::copy(param1), new IntValue(argBw - 1));
    } else {
        sign = _factory.bitval(hif::bit_zero);
    }

    std::int64_t delta = argBw - count;
    if (delta > 0) {
        _isFixed            = true;
        Expression *shifted = _factory.expression(param1, hif::op_srl, _factory.intval(count, _factory.integer()));
        Range *span         = _factory.range(delta - 1, 0);
        Value *ret          = _factory.slice(shifted, span);
        for (std::int64_t i = delta; i < argBw; ++i)
            ret = _factory.expression(hif::copy(sign), hif::op_concat, ret);
        fc->replace(ret);
        delete sign;
        delete fc;
        return true;
    }

    _isFixed   = true;
    Value *ret = sign;
    for (std::int64_t i = 1; i < argBw; ++i)
        ret = _factory.expression(hif::copy(sign), hif::op_concat, ret);
    fc->replace(ret);
    delete fc;
    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemTime(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_verilog__system_time")
        return false;

    _isFixed = true;
    if (_checkUnsupported(_opt.allowSystemC, "Mapping requires allowSystemC", fc))
        return true;

    _addHifLibrary("hif_systemc_sc_core", false);
    FunctionCall *tsFun = _factory.functionCall(
        "hif_systemc_sc_time_stamp", _factory.libraryInstance("hif_systemc_sc_core", false, true),
        _factory.noTemplateArguments(), _factory.noParameterArguments());
    Value *toDouble = _factory.cast(_factory.real(), tsFun);
    Type *semType   = hif::semantics::getSemanticType(fc, _sem);
    messageAssert(semType != nullptr, "Unable to get semantic type", fc, _sem);
    Cast *c = _factory.cast(hif::copy(semType), toDouble);
    fc->replace(c);
    _trash.insert(fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixIteratedConcat(FunctionCall *fc)
{
    std::string fcName(fc->getName());
    if (fcName != "hif_verilog_iterated_concat")
        return false;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemFopen(FunctionCall *fc)
{
    std::string fcName(fc->getName());
    if (fcName != "hif_verilog__system_fopen")
        return false;
    if (_checkUnsupported(_opt.allowStdLib, "Mapping requires allowStdLib", fc))
        return true;

    Type *type = hif::semantics::getSemanticType(fc, _sem);
    if (type == nullptr)
        return false;
    FunctionCall *filenoCall = _factory.functionCall(
        "hif_systemc_fileno", _factory.libraryInstance("hif_systemc_cstdio", false, true),
        _factory.noTemplateArguments(), _factory.parameterArgument("param1", nullptr));
    fc->replace(filenoCall);
    filenoCall->parameterAssigns.front()->setValue(fc);
    fc->setName("hif_systemc_fopen");
    delete fc->setInstance(_factory.libraryInstance("hif_systemc_cstdio", false, true));

    if (fc->parameterAssigns.size() == 1UL) {
        fc->parameterAssigns.push_back(_factory.parameterArgument("param2", _factory.stringval("w+")));
    }

    // Eventual include necessary libraries.
    _addHifLibrary("hif_systemc_cstdio", false);

    hif::semantics::resetDeclarations(fc);

    // Eventual fix fdopen parameter assignability.
    hif::backends::makeParametersAssignable(filenoCall, _sem, true);
    hif::backends::addEventualCast(filenoCall, hif::semantics::getSemanticType(filenoCall, _sem), type);

    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemRandom(FunctionCall *fc)
{
    std::string fcName(fc->getName());
    if (fcName != "hif_verilog__system_random")
        return false;
    if (_opt.allowSupportLibrary)
        return true;

    if (_checkUnsupported(_opt.allowStdLib, "Mapping requires allowStdLib", fc))
        return true;

    _addHifLibrary("hif_systemc_cstdlib", false);
    if (!fc->parameterAssigns.empty()) {
        messageWarning("Call to verilog random with a seed is mapped to a plain C rand", fc, _sem);
    }

    FunctionCall *rand = _factory.functionCall(
        "hif_systemc_rand", nullptr, _factory.noTemplateArguments(), _factory.noParameterArguments());

    fc->replace(rand);
    _trash.insert(fc);
    _isFixed = true;
    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemFEof(FunctionCall *fc)
{
    std::string fcName(fc->getName());
    if (fcName != "hif_verilog__system_feof")
        return false;
    if (_checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc))
        return true;

    messageAssert(fc->parameterAssigns.size() == 1, "Unexpected fcall parameters", fc, _sem);
    // translate as feof(int(fd))
    ParameterAssign *pa = fc->parameterAssigns.front();
    pa->setValue(_factory.cast(_factory.integer(), pa->setValue(nullptr)));

    return true;
}

bool RemoveStandardMethodsVisitor::_fixNew(FunctionCall *fc)
{
    if (fc->getName() != "hif_vhdl_new")
        return false;

    fc->setName("hif_systemc_new");

    Instance *inst = dynamic_cast<Instance *>(fc->getInstance());
    if (inst != nullptr) {
        Library *lib = dynamic_cast<Library *>(inst->getReferencedType());
        if (lib != nullptr) {
            messageAssert(lib->getName() == "hif_vhdl_standard", "Unexpected library instance", lib, _sem);
            lib->setName("hif_systemc_standard");
            hif::semantics::resetDeclarations(inst);
            hif::semantics::resetTypes(inst);
        }
    }

    _addHifLibrary("hif_systemc_standard", true);

    hif::semantics::resetDeclarations(fc);
    hif::backends::makeParametersAssignable(fc, _sem, true);

    return true;
}

bool RemoveStandardMethodsVisitor::_fixEdgeEvent(FunctionCall *fc)
{
    std::string fcName(fc->getName());
    if (fcName != "hif_systemc_posedge_event" && fcName != "hif_systemc_negedge_event")
        return false;
    _checkUnsupported(_opt.allowSystemC, "Mapping requires allowSystemC", fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixIsX(FunctionCall *fc)
{
    std::string fcName(fc->getName());
    if (fcName != "hif_vhdl_is_x")
        return false;

    Value *param    = fc->parameterAssigns.front()->getValue();
    Type *paramType = hif::semantics::getSemanticType(param, _sem);
    // If type is not logic or useBinaryLogic flag is true
    // replace function call with bool value false
    if ((paramType != nullptr && !hif::typeIsLogic(paramType, _sem)) || _opt.useBinaryLogic) {
        BoolValue *f = _factory.boolval(false, _factory.boolean());
        fc->replace(f);
        _trash.insert(fc);
        _isFixed = true;
        return true;
    }

    if (_opt.allowSystemC) {
        _addHifLibrary("hif_systemc_sc_dt", false);
        FunctionCall *scFun = _factory.functionCall(
            "hif_systemc_is_01", hif::copy(param), _factory.noTemplateArguments(), _factory.noParameterArguments());
        Expression *expr = _factory.expression(hif::op_not, scFun);

        fc->replace(expr);
        _trash.insert(fc);
        _isFixed = true;
        return true;
    }

    if (_opt.allowSupportLibrary)
        return true;
    messageError("Unable to map function call", fc, _sem);
}

bool RemoveStandardMethodsVisitor::_fixVhdlMatch(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_std_match")
        return false;
    if (!_opt.useBinaryLogic) {
        _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc);
        return true;
    }

    auto type = hif::semantics::getSemanticType(fc, _sem);
    messageAssert(type != nullptr, "Cannot type function call", fc, _sem);

    Value *expr = _factory.cast(
        hif::copy(type), _factory.expression(
                             hif::copy(fc->parameterAssigns.front()->getValue()), hif::op_case_eq,
                             hif::copy(fc->parameterAssigns.back()->getValue())));
    fc->replace(expr);
    _trash.insert(fc);

    return true;
}

bool RemoveStandardMethodsVisitor::_fixCStr(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_systemc_c_str")
        return false;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixFOpen(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_file_open")
        return false;
// TODO enable and map file type
#if 0
    if (_opt.allowStdLib)
    {
        Value * param1 = fc->parameterAssigns.at(0)->getValue();
        Value * fname = _factory.functionCall(
                    "hif_systemc_c_str",
                    param1,
                    _factory.noTemplateArguments(),
                    _factory.noParameterArguments()
                    );
        FieldReference * fref = dynamic_cast<FieldReference *>(
                    fc->parameterAssigns.at(1)->getValue());
        messageAssert(fref != nullptr, "Expected field reference", fc, _sem);
        Value * type = nullptr;

        if (fref->matchName("hif_vhdl_read_mode"))
            type = _factory.stringval("r");
        else if (fref->matchName("hif_vhdl_write_mode"))
            type = _factory.stringval("w");
        else if (fref->matchName("hif_vhdl_append_mode"))
            type = _factory.stringval("a");
        else messageError("Unexpected case for file open", fc, _sem);

        FunctionCall * fopen = _factory.functionCall(
                    "hif_systemc_fopen",
                    nullptr,
                    _factory.noTemplateArguments(),
                    (_factory.parameterArgument("param1", fname),
                     _factory.parameterArgument("param2", type))
                    );
        _addHifLibrary("hif_systemc_string");
        _addHifLibrary("hif_systemc_cstdio");
        fc->replace(fopen);
        delete fc;
        _isFixed = true;
        return true;
    }
#endif
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixExt(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_ext")
        return false;
    _manageVhdlExtensions(fc, false);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixConvStdLogicVector(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_conv_std_logic_vector")
        return false;

    if (_opt.useBinaryLogic && fc->parameterAssigns.size() == 2) {
        Value *param   = fc->parameterAssigns.at(1)->getValue();
        IntValue *size = dynamic_cast<IntValue *>(param);
        if (size != nullptr) {
            Value *v     = fc->parameterAssigns.at(0)->getValue();
            Range *span  = _factory.range(size->getValue() - 1, 0);
            Bitvector *t = _factory.bitvector(span, true, true);
            Cast *c      = _factory.cast(t, v);
            fc->replace(c);
            delete fc;
            _isFixed = true;
            return true;
        }
    }
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixVhdlEndfile(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_endfile")
        return false;
// TODO enable and map file type
#if 0
    if (_opt.allowStdLib)
    {
        FunctionCall * feof = _factory.functionCall(
                    "hif_systemc_feof",
                    nullptr,
                    _factory.noTemplateArguments(),
                    _factory.parameterArgument(
                        "param1",
                        fc->parameterAssigns.front()->getValue()
                        )
                    );
        Expression * expr = _factory.expression(
                    feof,
                    hif::op_neq,
                    new IntValue(0)
                    );
        _addHifLibrary("hif_systemc_cstdio");
        fc->replace(expr);
        delete fc;
        _isFixed = true;
        return true;
    }
#endif
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemRealTime(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_verilog__system_realtime")
        return false;
    _checkUnsupported(_opt.allowSystemC, "Mapping requires allowSystemC", fc);

    _isFixed = true;
    _addHifLibrary("hif_systemc_sc_core", false);
    FunctionCall *tsFun = _factory.functionCall(
        "hif_systemc_sc_time_stamp", _factory.libraryInstance("hif_systemc_sc_core", false, true),
        _factory.noTemplateArguments(), _factory.noParameterArguments());
    Value *toDouble = _factory.cast(_factory.real(), tsFun);

    fc->replace(toDouble);
    _trash.insert(fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixVhdlTrunc(FunctionCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_trunc")
        return false;

    if (_opt.allowStdLib) {
        _addHifLibrary("hif_systemc_cmath", false);
        fc->setName("hif_systemc_trunc");
        delete fc->setInstance(nullptr);
        hif::semantics::resetDeclarations(fc);
        _isFixed = true;
        return true;
    }

    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixVhdlUniform(ProcedureCall *fc)
{
    std::string fcallName(fc->getName());
    if (fcallName != "hif_vhdl_uniform")
        return false;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixVhdlHread(ProcedureCall *pc)
{
    std::string pcallName(pc->getName());
    if (pcallName != "hif_vhdl_hread")
        return false;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", pc);
    return true;
}

bool RemoveStandardMethodsVisitor::_allowSCFunctions(FunctionCall *fc)
{
    if (!hif::declarationIsPartOfStandard(fc, _sem, _declCheckSystemC))
        return false;
    _checkUnsupported(_opt.allowSystemC, "Mapping requires allowSystemC", fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_allowVerilogAmsFunctions(FunctionCall *fc)
{
    if (!hif::declarationIsPartOfStandard(fc, _sem, _declCheckSystemCAMS))
        return false;
    _checkUnsupported(_opt.allowSystemCAMS, "Mapping requires allowSystemC", fc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemDisplay(ProcedureCall *pc)
{
    std::string pcallName(pc->getName());
    if (pcallName != "hif_verilog__system_display")
        return false;
    if (_checkUnsupported(_opt.allowStdLib, "Mapping requires allowStdLib", pc))
        return true;

    _sortPrintMethodParameters(pc, true);
    if (!_fixPrintMethodParamNames(pc))
        return false;

    pc->setName("hif_systemc_printf");
    delete pc->setInstance(nullptr);
    _addHifLibrary("hif_systemc_cstdio", false);

    _isFixed = true;
    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemFDisplay(ProcedureCall *pc)
{
    std::string pcallName(pc->getName());
    if (pcallName != "hif_verilog__system_fdisplay")
        return false;
    if (_checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", pc))
        return true;

    if (pc->parameterAssigns.size() == 1) {
        // Adding empty second parameter
        pc->parameterAssigns.push_back(_factory.parameterArgument("param2", _factory.stringval("")));
    }

    if (!_manageFileWriteIOCalls(pc, true, false))
        return false;
    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemFScanF(ProcedureCall *pc)
{
    std::string pcallName(pc->getName());
    if (pcallName != "hif_verilog__system_fscanf")
        return false;
    if (_checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", pc))
        return true;

    if (pc->parameterAssigns.size() == 1) {
        // Adding empty second parameter
        pc->parameterAssigns.push_back(_factory.parameterArgument("param2", _factory.stringval("")));
    }

    if (!_manageFileWriteIOCalls(pc, false, true))
        return false;

    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemReadMemH(ProcedureCall *pc)
{
    std::string pcallName(pc->getName());
    if (pcallName != "hif_verilog__system_readmemh")
        return false;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", pc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemFinish(ProcedureCall *pc)
{
    std::string pcallName(pc->getName());
    if (pcallName != "hif_verilog__system_finish" && pcallName != "hif_verilog__system_stop")
        return false;

    if (_opt.allowSystemC) {
        _isFixed = true;
        _addHifLibrary("hif_systemc_sc_core", false);
        ProcedureCall *stopPc = _factory.procedureCall(
            "hif_systemc_sc_stop", _factory.libraryInstance("hif_systemc_sc_core", false, true),
            _factory.noTemplateArguments(), _factory.noParameterArguments());
        pc->replace(stopPc);
        _trash.insert(pc);
        return true;
    } else if (_opt.allowSupportLibrary) {
        return true;
    }

    if (_checkUnsupported(_opt.allowStdLib, "Mapping requires allowStdLib", pc))
        return true;
    _isFixed = true;
    _addHifLibrary("hif_systemc_cstdlib", false);
    ProcedureCall *exitFc = _factory.procedureCall(
        "hif_systemc_exit", nullptr, _factory.noTemplateArguments(),
        _factory.parameterArgument("param1", _factory.identifier("EXIT_SUCCESS")));
    pc->replace(exitFc);
    _trash.insert(pc);

    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemFwrite(ProcedureCall *pc)
{
    std::string pcName(pc->getName());
    if (pcName != "hif_verilog__system_fwrite")
        return false;
    if (_checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", pc))
        return true;

    if (!_manageFileWriteIOCalls(pc, false, false))
        return false;

    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemFclose(ProcedureCall *pc)
{
    std::string pcName(pc->getName());
    if (pcName != "hif_verilog__system_fclose")
        return false;
    if (_checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", pc))
        return true;

    messageAssert(pc->parameterAssigns.size() == 1, "Unexpected pcall parameters", pc, _sem);
    // translate aas fclose(int(fd))
    ParameterAssign *pa = pc->parameterAssigns.front();
    pa->setValue(_factory.cast(_factory.integer(), pa->setValue(nullptr)));

    return true;
}

bool RemoveStandardMethodsVisitor::_fixSystemFflush(ProcedureCall *pc)
{
    std::string pcName(pc->getName());
    if (pcName != "hif_verilog__system_fflush")
        return false;
    if (_checkUnsupported(_opt.allowStdLib, "Mapping requires allowStdLib", pc))
        return true;

    if (!pc->parameterAssigns.empty()) {
        // translate as fflush(int(fd))
        ParameterAssign *pa = pc->parameterAssigns.front();
        pa->setValue(_factory.cast(_factory.integer(), pa->setValue(nullptr)));
    } else {
        // translate as fflush(cast<File*>(0))
        pc->parameterAssigns.push_back(_factory.parameterArgument(
            "param1", _factory.cast(_factory.pointer(_factory.file(_factory.string())), _factory.intval(0))));

        pc->setName("hif_systemc_fflush");
        delete pc->setInstance(_factory.libraryInstance("hif_systemc_cstdio", false, true));

        hif::semantics::resetDeclarations(pc);
        // Eventual include necessary libraries.
        _addHifLibrary("hif_systemc_cstdio", false);
    }

    return true;
}

bool RemoveStandardMethodsVisitor::_fixDeallocate(ProcedureCall *pc)
{
    if (pc->getName() != "hif_vhdl_deallocate")
        return false;

    pc->setName("hif_systemc_delete");

    Instance *inst = dynamic_cast<Instance *>(pc->getInstance());
    if (inst != nullptr) {
        Library *lib = dynamic_cast<Library *>(inst->getReferencedType());
        if (lib != nullptr) {
            messageAssert(lib->getName() == "hif_vhdl_standard", "Unexpected library instance", lib, _sem);
            lib->setName("hif_systemc_standard");
            hif::semantics::resetDeclarations(inst);
            hif::semantics::resetTypes(inst);
        }
    }

    _addHifLibrary("hif_systemc_standard", true);

    hif::semantics::resetDeclarations(pc);
    hif::backends::makeParametersAssignable(pc, _sem, true);

    return true;
}

bool RemoveStandardMethodsVisitor::_fixHifLastValue(ProcedureCall *pc)
{
    std::string pcallName(pc->getName());
    if (pcallName != "hif_systemc_hif_lastValue")
        return false;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", pc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixVhdlWriteline(ProcedureCall *pc)
{
    std::string pcallName(pc->getName());
    if (pcallName != "hif_vhdl_writeline")
        return false;

    auto id = dynamic_cast<Identifier *>(pc->parameterAssigns.back()->getValue());
    if (id == nullptr) {
        _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", pc);
        return true;
    }

    _checkUnsupported(_opt.allowStdLib, "Mapping requires allowStdLib", pc);

    // Mapping to:
    // fprintf(f, "%s", l);
    // *l = "";
    ProcedureCall *fpf = _factory.procedureCall(
        "hif_systemc_fprintf", _factory.libraryInstance("hif_systemc_cstdio", false, true),
        _factory.noTemplateArguments(),
        (_factory.parameterArgument("param1", hif::copy(pc->parameterAssigns.front()->getValue())),
         _factory.parameterArgument("param2", _factory.stringval("%s")),
         _factory.parameterArgument("param3", hif::copy(pc->parameterAssigns.back()->getValue()))));
    Assign *ass = _factory.assignment(_factory.expression(hif::op_deref, hif::copy(id)), _factory.stringval(""));
    BList<Action>::iterator it(pc);
    it.insert_after(ass);
    it.insert_after(fpf);
    _addHifLibrary("hif_systemc_cstdio", false);
    _trash.insert(pc);

    return true;
}

bool RemoveStandardMethodsVisitor::_fixReadLine(ProcedureCall *pc)
{
    std::string pcallName(pc->getName());
    if (pcallName != "hif_vhdl_readline")
        return false;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", pc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixRead(ProcedureCall *pc)
{
    std::string pcallName(pc->getName());
    if (pcallName != "hif_vhdl_read")
        return false;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", pc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixFileClose(ProcedureCall *pc)
{
    std::string pcallName(pc->getName());
    if (pcallName != "hif_vhdl_file_close")
        return false;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", pc);
    return true;
}

bool RemoveStandardMethodsVisitor::_allowSCProcedures(ProcedureCall *pc)
{
    if (!hif::declarationIsPartOfStandard(pc, _sem, _declCheckSystemC))
        return false;
    _checkUnsupported(_opt.allowSystemC, "Mapping requires allowSystemC", pc);
    return true;
}

bool RemoveStandardMethodsVisitor::_allowSCAmsProcedures(ProcedureCall *pc)
{
    if (!hif::declarationIsPartOfStandard(pc, _sem, _declCheckSystemCAMS))
        return false;
    _checkUnsupported(_opt.allowSystemCAMS, "Mapping requires allowSystemC", pc);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixVhdlAssertSeverity(FieldReference *fr)
{
    std::string frName(fr->getName());
    if (frName != "hif_vhdl_note" && frName != "hif_vhdl_warning" && frName != "hif_vhdl_error" &&
        frName != "hif_vhdl_failure")
        return false;

    ProcedureCall *pc = dynamic_cast<ProcedureCall *>(fr->getParent()->getParent());
    if (pc != nullptr && pc->matchName("hif_vhdl_assert"))
        return true;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fr);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixVhdlFileOpenKind(FieldReference *fr)
{
    std::string frName(fr->getName());
    if (frName != "hif_vhdl_read_mode" && frName != "hif_vhdl_write_mode" && frName != "hif_vhdl_append_mode")
        return false;

    ProcedureCall *pc = dynamic_cast<ProcedureCall *>(fr->getParent()->getParent());
    if (pc != nullptr && pc->matchName("hif_vhdl_file_open"))
        return true;
    FunctionCall *fc = dynamic_cast<FunctionCall *>(fr->getParent()->getParent());
    if (fc != nullptr && fc->matchName("hif_vhdl_file_open"))
        return true;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fr);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixVhdlFileOpenStatus(FieldReference *fr)
{
    std::string frName(fr->getName());
    if (frName != "hif_vhdl_open_ok" && frName != "hif_vhdl_status_error" && frName != "hif_vhdl_name_error" &&
        frName != "hif_vhdl_mode_error")
        return false;

    ProcedureCall *pc = dynamic_cast<ProcedureCall *>(fr->getParent()->getParent());
    if (pc != nullptr && pc->matchName("hif_vhdl_file_open"))
        return true;
    FunctionCall *fc = dynamic_cast<FunctionCall *>(fr->getParent()->getParent());
    if (fc != nullptr && fc->matchName("hif_vhdl_file_open"))
        return true;
    _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fr);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixVhdlStdIoStreams(FieldReference *fr)
{
    std::string frName(fr->getName());
    if (frName != "hif_vhdl_output" && frName != "hif_vhdl_input")
        return false;
    const auto isUnSupported = _checkUnsupported(_opt.allowStdLib, "Mapping requires allowStdLib", fr);
    if (isUnSupported)
        return true;

    // Mapping to C equivalents:
    _addHifLibrary("hif_systemc_cstdio", false);
    std::string name;
    if (frName == "hif_vhdl_output")
        name = "hif_systemc_stdout";
    else
        name = "hif_systemc_stdin";

    auto newFr = _factory.fieldRef(_factory.libraryInstance("hif_systemc_cstdio", false, true), name.c_str());
    fr->replace(newFr);
    _trash.insert(fr);

    return true;
}

bool RemoveStandardMethodsVisitor::_fixCStdIoStreams(FieldReference *fr)
{
    std::string frName(fr->getName());
    if (frName != "hif_systemc_stdout" && frName != "hif_systemc_stderr" && frName != "hif_systemc_stdin")
        return false;

    _checkUnsupported(_opt.allowStdLib, "Mapping requires allowStdLib", fr);
    return true;
}

bool RemoveStandardMethodsVisitor::_fixVhdlTypeReference(TypeReference *o)
{
    if (!o->matchName("hif_vhdl_line") && !o->matchName("hif_vhdl_text"))
        return false;
    if (_opt.allowSupportLibrary)
        return 0;

    TypeDef *td = dynamic_cast<TypeDef *>(hif::manipulation::instantiate(o, _sem));
    o->replace(hif::copy(td->getType()));
    delete o;

    _isFixed = true;
    return true;
}

void RemoveStandardMethodsVisitor::_checkStandardDecl(Object *obj)
{
    if (!hif::declarationIsPartOfStandard(obj, _sem))
        return;
#ifdef NDEBUG
    // In release, we are just more permissive:
    if (hif::declarationIsPartOfStandard(obj, _sem, _declCheckStd))
        return;
    if (hif::declarationIsPartOfStandard(obj, _sem, _declCheckSupport))
        return;
#endif
    // If decl is a call to a standard view, skip it.
    auto decl = hif::semantics::getDeclaration(obj, _sem);
    if (hif::getNearestParent<View>(decl) != nullptr)
        return;
    messageError("Unmanaged object", obj, _sem);
}

bool RemoveStandardMethodsVisitor::_addHifLibrary(const char *c, bool standard)
{
    bool ret = hif::backends::addHifLibrary(c, _currentScope, _system, _sem, standard);
    messageAssert(ret, "Unable to add library", nullptr, nullptr);
    return ret;
}

bool RemoveStandardMethodsVisitor::_sortPrintMethodParameters(ProcedureCall *o, bool isDisplay)
{
    hif::semantics::resetTypes(o);
    hif::semantics::UpdateDeclarationOptions dopt;
    dopt.forceRefresh = true;
    hif::semantics::updateDeclarations(o, _sem, dopt);
    hif::semantics::typeTree(o, _sem);

    ParameterAssign *passText = nullptr;
    BList<ParameterAssign> passOther;

    // Reordering parameters and adding required %s
    for (BList<ParameterAssign>::iterator i = o->parameterAssigns.begin(); i != o->parameterAssigns.end();) {
        ParameterAssign *pa  = *i;
        unsigned int counter = 0;

        StringValue *tpa = dynamic_cast<StringValue *>(pa->getValue());

        if (tpa != nullptr) {
            std::string val = tpa->getValue();
            counter         = 0;

            // Counting % modifiers
            for (std::string::size_type j = 0; j < val.size(); ++j) {
                char c = val[j];
                if (c != '%')
                    continue;
                if (val[j + 1] == '%') {
                    ++j;
                    continue;
                }
                ++counter;
            }

            // Removing format string and marging it with potential other
            // format string.
            i = i.remove();
            if (passText == nullptr) {
                // First string parameter
                passText = pa;
            } else {
                StringValue *text  = static_cast<StringValue *>(passText->getValue());
                StringValue *text2 = static_cast<StringValue *>(pa->getValue());
                text->setValue(text->getValue() + text2->getValue());
                delete pa;
            }

            // Skipping counted parameter by format string
            for (BList<ParameterAssign>::iterator j = o->parameterAssigns.begin();
                 j != o->parameterAssigns.end() && counter > 0; --counter) {
                ParameterAssign *tmp = *j;
                j                    = j.remove();
                passOther.push_back(tmp);
            }

            i = o->parameterAssigns.begin();

            continue;
        }

        // tpa == nullptr
        messageAssert(counter == 0, "Unexpected counter different from zero.", o, _sem);
        if (passText == nullptr) {
            // No format string matched, creating new format string
            StringValue *t = new StringValue("%s"); // fixed later
            t->setType(_factory.string(true));
            ParameterAssign *papa = new ParameterAssign();
            papa->setName("param1");
            papa->setValue(t);
            passText = papa;

            i = i.remove();
            passOther.push_back(pa);
            continue;
        }

        // passText != nullptr: parameter not counted by format string.
        StringValue *text = static_cast<StringValue *>(passText->getValue());
        text->setValue(text->getValue() + "%s"); // fixed later

        i = i.remove();
        passOther.push_back(pa);
    }

    messageAssert(passText != nullptr, "Unexpected case", o, _sem);

    // Fixing %mod
    bool needStringLib   = false;
    bool needScCoreLib   = false;
    unsigned int counter = 0;
    std::string val      = static_cast<StringValue *>(passText->getValue())->getValue();
    for (std::string::size_type i = 0; i < val.size(); ++i) {
        char c = val[i];
        if (c != '%')
            continue;

        ++i;
        char n = val[i];
        // Skipping %%
        if (n == '%')
            continue;
        if (n == 'm') {
            // Special management for module printing case:
            // there is not corresponding argument create a new one!
            StateTable *st = hif::getNearestParent<StateTable>(o);
            messageAssert(st != nullptr, "Cannot find parent state table", o, _sem);
            Scope *dd = dynamic_cast<SubProgram *>(st->getParent());
            if (dd == nullptr)
                dd = st;
            std::string hierName =
                hif::manipulation::buildHierarchicalSymbol(dd, _sem, hif::manipulation::DefinitionStyle::VERILOG);

            ParameterAssign *moduleParameter =
                _factory.parameterArgument("paramX", _factory.stringval(hierName.c_str(), false));
            passOther.insert(moduleParameter, counter, true);
        }

        ParameterAssign *p_at = passOther.at(counter);
        messageAssert(p_at != nullptr, "Required parameter not found.", o, _sem);
        char map = _getMod(n, p_at, needStringLib, needScCoreLib);
        val[i]   = map;
        ++counter;
    }

    if (isDisplay) {
        static_cast<StringValue *>(passText->getValue())->setValue(val + "\\n");
    } else {
        static_cast<StringValue *>(passText->getValue())->setValue(val);
    }

    if (_checkUnsupported(!needStringLib || _opt.allowStdLib, "Mapping requires allowStdLib", o))
        return true;
    if (_checkUnsupported(!needScCoreLib || _opt.allowSystemC, "Mapping requires allowSystemC", o))
        return true;

    if (needStringLib)
        _addHifLibrary("hif_systemc_string", false);
    if (needScCoreLib)
        _addHifLibrary("hif_systemc_sc_core", false);

    // Re-composing parameters
    o->parameterAssigns.push_back(passText);
    o->parameterAssigns.merge(passOther);

    return true;
}

char RemoveStandardMethodsVisitor::_getMod(char n, ParameterAssign *pa, bool &needStringLib, bool &needScCoreLib)
{
    Type *t = hif::semantics::getBaseType(hif::semantics::getSemanticType(pa->getValue(), _sem), false, _sem);
    messageAssert(t != nullptr, "Cannot type description", pa->getValue(), _sem);

    Bitvector *bv = dynamic_cast<Bitvector *>(t);
    Bit *bit      = dynamic_cast<Bit *>(t);
    String *s     = dynamic_cast<String *>(t);
    Char *c       = dynamic_cast<Char *>(t);
    Real *r       = dynamic_cast<Real *>(t);
    Time *time    = dynamic_cast<Time *>(t);
    Int *intType  = dynamic_cast<Int *>(t);

    messageAssert(
        bv != nullptr || bit != nullptr || s != nullptr || c != nullptr || r != nullptr || time != nullptr ||
            intType != nullptr,
        "Unexpected type. (1)", t, _sem);

    char ret             = 's';
    const char originalN = n;
    n                    = static_cast<char>(tolower(n));
    bool supported       = true;
    switch (n) {
    case 'h':
    case 'x': {
        if (originalN == n)
            ret = 'x';
        else
            ret = 'X';

        if (bv == nullptr && intType == nullptr) {
            supported = false;
            break;
        }
        _addCast(pa->getValue(), _factory.integer(), needStringLib, needScCoreLib);
    } break;
    case 'o': {
        ret = 'o';
        if (bv == nullptr && intType == nullptr) {
            supported = false;
            break;
        }
        _addCast(pa->getValue(), _factory.integer(), needStringLib, needScCoreLib);
    } break;
    case 'd': {
        ret = 'd';
        if (bv == nullptr && bit == nullptr && r == nullptr && time == nullptr && intType == nullptr) {
            supported = false;
            break;
        }

        if (time != nullptr) {
            _addCast(pa->getValue(), _factory.real(), needStringLib, needScCoreLib);
        } else {
            _addCast(pa->getValue(), _factory.integer(), needStringLib, needScCoreLib);
        }
    } break;
    case 'b':
    case 'z': {
        if (bv == nullptr && bit == nullptr) {
            supported = false;
            break;
        }

        if (bit != nullptr) {
            _addCast(pa->getValue(), _factory.character(), needStringLib, needScCoreLib);
            ret = 'c';
        } else if (bv != nullptr) {
            _addCast(pa->getValue(), _factory.string(), needStringLib, needScCoreLib);
            ret = 's';
        }
    } break;
    case 'c': {
        ret = 'c';
        if (bit == nullptr && c == nullptr && intType == nullptr) {
            supported = false;
            break;
        }
        _addCast(pa->getValue(), _factory.character(), needStringLib, needScCoreLib);
    } break;
    case 's': {
        if (bv == nullptr && bit == nullptr && s == nullptr && c == nullptr && time == nullptr && intType == nullptr) {
            supported = false;
            break;
        }

        if ((bv != nullptr && !_opt.printBitvectorsAsIntegers) || bit != nullptr || s != nullptr) {
            _addCast(pa->getValue(), _factory.string(), needStringLib, needScCoreLib);
            ret = 's';
        } else if (c != nullptr) {
            ret = 'c';
        } else if (time != nullptr) {
            _addCast(pa->getValue(), _factory.real(), needStringLib, needScCoreLib);
            ret = 'e';
        } else if (intType != nullptr || (bv != nullptr && _opt.printBitvectorsAsIntegers)) {
            _addCast(pa->getValue(), _factory.integer(), needStringLib, needScCoreLib);
            ret = 'd';
        }
    } break;
    case 'e': {
        ret = 'e';
        if (bv == nullptr && r == nullptr && intType == nullptr) {
            supported = false;
            break;
        }
        _addCast(pa->getValue(), _factory.real(), needStringLib, needScCoreLib);
    } break;
    case 'f': {
        ret = 'f';
        if (bv == nullptr && r == nullptr && intType == nullptr) {
            supported = false;
            break;
        }
        _addCast(pa->getValue(), _factory.real(), needStringLib, needScCoreLib);
    } break;
    case 'g': {
        ret = 'g';
        if (bv == nullptr && r == nullptr && intType == nullptr) {
            supported = false;
            break;
        }
        _addCast(pa->getValue(), _factory.real(), needStringLib, needScCoreLib);
    } break;
    case 't': {
        if (bv == nullptr && r == nullptr && time == nullptr && intType == nullptr) {
            supported = false;
            break;
        }

        if (bv != nullptr && !_opt.printBitvectorsAsIntegers) {
            _addCast(pa->getValue(), _factory.string(), needStringLib, needScCoreLib);
            ret = 's';
        } else if (r != nullptr || (bv != nullptr && _opt.printBitvectorsAsIntegers)) {
            _addCast(pa->getValue(), _factory.real(), needStringLib, needScCoreLib);
            ret = 'e';
        } else if (time != nullptr) {
            _addCast(pa->getValue(), _factory.real(), needStringLib, needScCoreLib);
            ret = 'e';
        } else if (intType != nullptr || (bv != nullptr && _opt.printBitvectorsAsIntegers)) {
            _addCast(pa->getValue(), _factory.integer(), needStringLib, needScCoreLib);
            ret = 'd';
        }

    } break;
    case 'm': {
        if (s == nullptr) {
            supported = false;
            break;
        }

        ret           = 's';
        needStringLib = true;
    } break;
    case 'l':
    case 'v':
    case 'u':
    default:
        supported = false;
        break;
    }

    messageAssert(supported, "Unexpected type for print modifier: " + std::string(1, originalN), t, _sem);

    return ret;
}

void RemoveStandardMethodsVisitor::_addCast(Value *v, Type *t, bool &needStringLib, bool &needScCoreLib)
{
    // Fixing types:
    Type *vt = hif::semantics::getBaseType(hif::semantics::getSemanticType(v, _sem), false, _sem);
    messageAssert(vt != nullptr, "Cannot type value", v, _sem);

    hif::EqualsOptions opt;
    opt.checkOnlyTypes = true;
    if (hif::equals(vt, t, opt)) {
        delete t;
        if (dynamic_cast<String *>(vt) != nullptr) {
            FunctionCall *fc = _factory.functionCall(
                "hif_systemc_c_str", nullptr, _factory.noTemplateArguments(), _factory.noParameterArguments());
            v->replace(fc);
            fc->setInstance(v);
            needStringLib = true;
        }
        return;
    }

    if (dynamic_cast<Time *>(t) != nullptr) {
        delete t;
        FunctionCall *fc = _factory.functionCall(
            "hif_systemc_to_default_time_units", nullptr, _factory.noTemplateArguments(),
            _factory.noParameterArguments());
        v->replace(fc);
        fc->setInstance(v);
        needScCoreLib = true;
        return;
    }

    Cast *c = new Cast();
    c->setType(t);
    v->replace(c);
    c->setValue(v);

    if (dynamic_cast<String *>(t) != nullptr) {
        FunctionCall *fc = _factory.functionCall(
            "hif_systemc_c_str", nullptr, _factory.noTemplateArguments(), _factory.noParameterArguments());
        c->replace(fc);
        fc->setInstance(c);
        needStringLib = true;
    }
}

bool RemoveStandardMethodsVisitor::_manageFileWriteIOCalls(ProcedureCall *o, bool isDisplay, bool isScan)
{
    // Temporary remove first parameter assign (file descriptor) to call common method.
    ParameterAssign *pa = o->parameterAssigns.front();
    o->parameterAssigns.begin().remove();

    // Renaming parameters
    _fixPrintMethodParamNames(o);

    // Fix method parameters
    _sortPrintMethodParameters(o, isDisplay);

    // Restore first parameter assign
    o->parameterAssigns.push_front(pa);

    // Forcing first parameter as integer
    pa->setValue(_factory.cast(_factory.integer(), pa->setValue(nullptr)));

    // Align actual parameter name w.r.t. hif definition.
    if (!_fixPrintMethodParamNames(o))
        return false;

    hif::semantics::resetTypes(o);
    hif::semantics::UpdateDeclarationOptions dopt;
    dopt.forceRefresh = true;
    hif::semantics::updateDeclarations(o, _sem, dopt);
    hif::semantics::typeTree(o, _sem);

    if (isScan) {
        int count = 0;
        for (BList<ParameterAssign>::iterator i = o->parameterAssigns.begin(); i != o->parameterAssigns.end();
             ++i, ++count) {
            ParameterAssign *pp = *i;
            if (count < 2)
                continue;

            Value *pv = pp->setValue(nullptr);
            pp->setValue(_factory.expression(op_ref, pv));
        }
    }

    return true;
}

bool RemoveStandardMethodsVisitor::_fixPrintMethodParamNames(ProcedureCall *o)
{
    // Fixing param names
    unsigned int position = 1;
    for (BList<ParameterAssign>::iterator i = o->parameterAssigns.begin(); i != o->parameterAssigns.end();
         ++i, ++position) {
        std::stringstream ss;
        ss << "param" << position;
        (*i)->setName(ss.str());
    }

    return true;
}

bool RemoveStandardMethodsVisitor::_manageVhdlExtensions(FunctionCall *fc, bool isSigned)
{
    Value *param1 = fc->parameterAssigns.at(0)->getValue();
    Value *param2;
    if (fc->parameterAssigns.size() == 2) {
        param2 = fc->parameterAssigns.at(1)->getValue();
    } else {
        ValueTPAssign *vtpa = dynamic_cast<ValueTPAssign *>(fc->templateParameterAssigns.at(0));
        param2              = vtpa->getValue();
    }
    if (param1 == nullptr || param2 == nullptr)
        return false;

    Type *argType  = hif::semantics::getBaseType(param1, false, _sem);
    Value *argSize = hif::semantics::typeGetSpanSize(argType, _sem);
    Value *newSize = param2;

    Type *type      = hif::semantics::getSemanticType(fc, _sem);
    Range *typeSpan = hif::typeGetSpan(type, _sem);
    messageAssert(typeSpan != nullptr, "Span not found", type, _sem);
    bool isDownto = typeSpan->getDirection() == hif::dir_downto;
    bool isLogic  = hif::typeIsLogic(argType, _sem);

    bool sameSize = hif::equals(argSize, newSize);
    Value *extension =
        _factory.cast(_factory.boolean(), _factory.expression(hif::copy(argSize), hif::op_le, hif::copy(newSize)));
    Value *check           = hif::manipulation::simplify(extension, _sem);
    BoolValue *boolValue   = dynamic_cast<BoolValue *>(check);
    bool isExtension = boolValue != nullptr && boolValue->getValue();
    bool isTrunc     = boolValue != nullptr && !boolValue->getValue();
    delete boolValue;

    // same span case:
    Value *sameValue = hif::copy(param1);
    // trunc case:
    Range *span      = _factory.range(
        _factory.expression(hif::copy(newSize), hif::op_minus, _factory.intval(1)), hif::dir_downto, new IntValue(0));
    if (!isDownto)
        span->swapBounds();
    Value *truncValue = _factory.slice(hif::copy(param1), span);
    // extension case:
    Type *inner       = hif::copy(argType);
    hif::typeSetSigned(inner, isSigned, _sem);
    Type *outer     = hif::copy(type);
    Value *extValue = _factory.cast(outer, _factory.cast(inner, hif::copy(param1)));

    // Doing replacement:
    if (sameSize) {
        _isFixed = true;
        fc->replace(sameValue);
        delete truncValue;
        delete extValue;
        delete fc;
        return true;
    } else if (isTrunc) {
        _isFixed = true;
        delete sameValue;
        fc->replace(truncValue);
        delete extValue;
        delete fc;
        return true;
    } else if (isExtension) {
        if (isLogic && !_opt.useBinaryLogic) {
            _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc);

            delete sameValue;
            delete truncValue;
            delete extValue;
            return true;
        }

        _isFixed = true;
        delete sameValue;
        delete truncValue;
        fc->replace(extValue);
        delete fc;
        return true;
    }

    // Unknown width comparison: make a When...
    if (isLogic && !_opt.useBinaryLogic) {
        _checkUnsupported(_opt.allowSupportLibrary, "Mapping requires allowSupportLibrary", fc);
        return true;
    }

    When *when = _factory.when(
        (_factory.whenalt(
             _factory.cast(
                 _factory.boolean(), _factory.expression(hif::copy(argSize), hif::op_case_eq, hif::copy(newSize))),
             _factory.cast(hif::copy(outer), sameValue)),
         _factory.whenalt(
             _factory.cast(_factory.boolean(), _factory.expression(hif::copy(argSize), hif::op_lt, hif::copy(newSize))),
             _factory.cast(hif::copy(outer), truncValue))),
        extValue);

    _isFixed = true;
    fc->replace(when);
    delete fc;
    return true;
}

} // namespace
RemoveStandardMethodsOptions::RemoveStandardMethodsOptions()
    : allowStdLib(true)
    , allowSystemC(true)
    , allowSupportLibrary(true)
    , useBinaryLogic(false)
    , useSystemCEvent(true)
    , preferScMethodFlavour(false)
    , allowSystemCAMS(false)
    , printBitvectorsAsIntegers(false)
{
    // ntd
}

RemoveStandardMethodsOptions::~RemoveStandardMethodsOptions()
{
    //ntd
}

RemoveStandardMethodsOptions::RemoveStandardMethodsOptions(const RemoveStandardMethodsOptions &other)
    : allowStdLib(other.allowStdLib)
    , allowSystemC(other.allowSystemC)
    , allowSupportLibrary(other.allowSupportLibrary)
    , useBinaryLogic(other.useBinaryLogic)
    , useSystemCEvent(other.useSystemCEvent)
    , preferScMethodFlavour(other.preferScMethodFlavour)
    , allowSystemCAMS(other.allowSystemCAMS)
    , printBitvectorsAsIntegers(other.printBitvectorsAsIntegers)
{
    //ntd
}

RemoveStandardMethodsOptions &RemoveStandardMethodsOptions::operator=(RemoveStandardMethodsOptions other)
{
    this->swap(other);
    return *this;
}

void RemoveStandardMethodsOptions::swap(RemoveStandardMethodsOptions &other)
{
    std::swap(allowStdLib, other.allowStdLib);
    std::swap(allowSystemC, other.allowSystemC);
    std::swap(allowSupportLibrary, other.allowSupportLibrary);
    std::swap(useBinaryLogic, other.useBinaryLogic);
    std::swap(useSystemCEvent, other.useSystemCEvent);
    std::swap(preferScMethodFlavour, other.preferScMethodFlavour);
    std::swap(allowSystemCAMS, other.allowSystemCAMS);
    std::swap(printBitvectorsAsIntegers, other.printBitvectorsAsIntegers);
}

bool removeStandardMethods(
    Object *root,
    hif::semantics::ILanguageSemantics *sem,
    const RemoveStandardMethodsOptions &opt)
{
    if (!opt.allowSupportLibrary) {
        LastValueOptions optLast;
        optLast.replaceRisingFallingEdge = false;
        optLast.inlineLastValue          = true;
        mapLastValueToSystemC(root, optLast);
    }
    RemoveStandardMethodsVisitor vis(sem, opt);
    root->acceptVisitor(vis);
    vis.printUnsupported();
    return vis.isFixed();
}

} // namespace manipulation
} // namespace hif
