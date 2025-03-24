/// @file fixTemplateParameters.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>
#include <cstdlib>
#include <iostream>

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hifIOUtils.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/fixTemplateParameters.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"
#include "hif/trash.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#    pragma clang diagnostic ignored "-Wmissing-noreturn"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#    if __GNUC__ >= 5
#        pragma GCC diagnostic ignored "-Wduplicated-cond"
#    endif
#endif

//#define DEBUG_INSERTIONS

namespace hif
{
namespace manipulation
{

namespace
{

// ///////////////////////////////////////////////////////////////////
// PreserveCTCVisitor
// ///////////////////////////////////////////////////////////////////
class PreserveCTCVisitor : public GuideVisitor
{
public:
    PreserveCTCVisitor();
    ~PreserveCTCVisitor();

    int visitLibraryDef(LibraryDef &o);
    int visitValueTP(ValueTP &o);
    int visitView(View &o);
};

PreserveCTCVisitor::PreserveCTCVisitor()
    : GuideVisitor()
{
    // ntd
}

PreserveCTCVisitor::~PreserveCTCVisitor()
{
    // ntd
}

int PreserveCTCVisitor::visitLibraryDef(LibraryDef &o)
{
    if (o.isStandard())
        return 0;
    GuideVisitor::visitLibraryDef(o);
    return 0;
}

int PreserveCTCVisitor::visitValueTP(ValueTP &o)
{
    GuideVisitor::visitValueTP(o);

    if (dynamic_cast<View *>(o.getParent()) != nullptr) {
        o.setCompileTimeConstant(false);
    }

    return 0;
}

int PreserveCTCVisitor::visitView(View &o)
{
    if (o.isStandard())
        return 0;
    GuideVisitor::visitView(o);
    return 0;
}

typedef std::set<Declaration *> Declarations;
typedef std::set<Object *> ReferencesSet;
typedef std::map<Declaration *, ReferencesSet> ReferencesMap;
typedef std::map<Declaration *, Type *> GoodTypeMap;
typedef std::map<DataDeclaration *, std::string> DefinesMap;

const char *PROPERTY_NEW_NAME = "NEW_NAME";

/// @brief Map that store the good type for each declaration that has a bad type.
/// Stored type will be set by aliasing in declarations.
GoodTypeMap goodTypeMap;

/// @brief Map that store the new names assign to Const that should be moved to
/// define. Needed for temporal dependency between visitors.
DefinesMap definesMap;
/// @brief If not already set, add a property to field concatenating record name
/// with "_" and fieldName.
/// @param f The field.
/// @param decl The declaration.
/// @return The fixed name.
///
std::string _setNewFieldName(Field *f, Declaration *decl)
{
    TypedObject *propValue = f->getProperty(PROPERTY_NEW_NAME);
    if (propValue != nullptr)
        return static_cast<Identifier *>(propValue)->getName();

    std::string n =
        hif::NameTable::getInstance()->getFreshName((std::string(decl->getName()) + "_" + f->getName()).c_str());
    f->addProperty(PROPERTY_NEW_NAME, new Identifier(n));
    return n;
}
/// @brief If not present creates the global string enum used to replace
/// original template parameter of string type. After that it return
/// a pointer to it.
/// @param o An object related to the tree.
/// @param sem The semantics.
/// @return A pointer to global string enum.
///
Enum *_addAndGetStringEnum(Object *o, hif::semantics::ILanguageSemantics *sem)
{
    System *sys = hif::getNearestParent<System>(o);
    messageAssert(sys != nullptr, "Unexpected case", o, sem);

    // Adding enum if not already exists:
    TypeDef *td = new TypeDef();
    td->setName(NameTable::getInstance()->hifStringNames());
    td->setType(new Enum());
    td->setOpaque(true);
    hif::manipulation::AddUniqueObjectOptions addOpt;
    addOpt.equalsOptions.checkOnlyNames = true;
    bool ins                            = hif::manipulation::addUniqueObject(td, sys->declarations, addOpt);

    // If not already present return the created enum:
    if (ins)
        return static_cast<Enum *>(td->getType());

    // Otherwise get and return the enum:
    delete td;
    TypeReference *tr = new TypeReference();
    tr->setName(NameTable::getInstance()->hifStringNames());
    hif::semantics::DeclarationOptions opt;
    opt.location = o;
    TypeDef *tde = dynamic_cast<TypeDef *>(hif::semantics::getDeclaration(tr, sem, opt));
    messageAssert(tde != nullptr, "Unexpected case", tr, sem);
    Enum *e = dynamic_cast<Enum *>(tde->getType());
    messageAssert(e != nullptr, "Unexpected case", tde->getType(), sem);
    return e;
}

/// @brief Adds a value to global string enum with a enum value that has
/// the given name.
/// @param o An object related to the tree.
/// @param n The name of enum value to add.
/// @param sem The semantics.
///
std::string _addStringEnumEntry(Object *o, std::string n, hif::semantics::ILanguageSemantics *sem)
{
    // Getting the global string enum:
    Enum *e = _addAndGetStringEnum(o, sem);

    // Removing undesired chars.
    std::replace(n.begin(), n.end(), '.', '_');
    for (std::string::size_type i = 0; i < n.size(); ++i) {
        if (n[i] != '*')
            continue;
        n = n.substr(0, i) + "hifStar" + n.substr(i + 1);
    }
    std::string nnn = nullptr;
    if (n == "")
        nnn = NameTable::getInstance()->hifEmptyStringName();
    else
        nnn = n.c_str();

    // Creating the enum value:
    EnumValue *ev = new EnumValue();
    ev->setName(nnn);
    TypeReference *tr = new TypeReference();
    tr->setName(NameTable::getInstance()->hifStringNames());
    ev->setType(tr);

    // Adding new value to enum:
    hif::manipulation::AddUniqueObjectOptions addOpt;
    addOpt.equalsOptions.checkOnlyNames = true;
    addOpt.deleteIfNotAdded             = true;
    hif::manipulation::addUniqueObject(ev, e->values, addOpt);

    return nnn;
}

/// @brief Check if the given declaration is already a template that is when
/// is a ValueTP or is a Const with the property move to define set.
/// @param decl The declaration.
/// @return True if is already a template.
///
bool _checkIsAlreadyTemplate(Declaration *decl, const bool fixCompileTimeConstant)
{
    if (dynamic_cast<ValueTP *>(decl) != nullptr) {
        ValueTP *vtp = static_cast<ValueTP *>(decl);
        if (fixCompileTimeConstant)
            vtp->setCompileTimeConstant(true);
        return true;
    }

    if (dynamic_cast<Const *>(decl) != nullptr) {
        Const *c = static_cast<Const *>(decl);
        if (definesMap.find(c) != definesMap.end())
            return true;
    }

    if (dynamic_cast<Parameter *>(decl) != nullptr && goodTypeMap.find(decl) != goodTypeMap.end()) {
        return true;
    }

    if (dynamic_cast<Function *>(decl) != nullptr && goodTypeMap.find(decl) != goodTypeMap.end()) {
        return true;
    }

    return false;
}

// ///////////////////////////////////////////////////////////////////
// TypeConverterVisitor
// ///////////////////////////////////////////////////////////////////
class TypeConverterVisitor : public hif::GuideVisitor
{
public:
    TypeConverterVisitor(hif::semantics::ILanguageSemantics *sem);
    ~TypeConverterVisitor();

    int visitValueTP(ValueTP &o);

    int visitIdentifier(Identifier &);
    int visitValueTPAssign(ValueTPAssign &);

private:
    typedef std::map<ValueTP *, std::set<TypedObject *>> TemplateMap;

    void _collectSymbols(TypedObject *o);
    void _collectValueTP(ValueTP *vtp, TypedObject *o = nullptr);
    Value *_transformTimeValueToRecordValue(Value *o);
    std::string _getTimeUnitString(TimeValue::TimeUnit u);

    /// @brief The semantics on witch works the visitor.
    hif::semantics::ILanguageSemantics *_sem;

    /// @brief The map from templates to be refined to corresponding symbols.
    TemplateMap _templateMap;

    hif::HifFactory _factory;

    // disabled
    TypeConverterVisitor(const TypeConverterVisitor &);
    TypeConverterVisitor &operator=(const TypeConverterVisitor &);
};

TypeConverterVisitor::TypeConverterVisitor(hif::semantics::ILanguageSemantics *sem)
    : _sem(sem)
    , _templateMap()
    , _factory(sem)
{
    // ntd
}

TypeConverterVisitor::~TypeConverterVisitor()
{
    // ntd
}

Value *TypeConverterVisitor::_transformTimeValueToRecordValue(Value * /*o*/)
{
    messageError("Transform not managed yet!", nullptr, nullptr);
#if 0
    if (o == nullptr) return nullptr;
    TimeValue * tv = dynamic_cast <TimeValue *>(o);
    messageAssert(tv != nullptr, "Unsupported value of time vtp", o, _sem);

    RecordValue * rv = new RecordValue();
    RecordValueAlt * rvaReal = new RecordValueAlt();
    rvaReal->setName("value");
    rvaReal->setValue(_factory.realval(tv->getValue()));
    rv->alts.push_back(rvaReal);
    RecordValueAlt * rvaUnit = new RecordValueAlt();
    rvaUnit->setName("unit");
    rvaUnit->setValue(_factory.fieldRef(
                          _factory.libraryInstance(hif::NameTable::getInstance()->hifUtility(), false, true),
                          _getTimeUnitString(tv->getUnit())));
    rv->alts.push_back(rvaUnit);

    if (tv->getValue() > static_cast<long long>(tv->getValue()))
    {
        // TODO: multiply the value to avoid integer truncation
        messageWarning( "Time value will be truncated since is "
                        "associated with a template", o, _sem );
    }

    return rv;
#endif
}

void TypeConverterVisitor::_collectSymbols(TypedObject *o)
{
    ValueTP *vtp = dynamic_cast<ValueTP *>(hif::semantics::getDeclaration(o, _sem));
    if (vtp == nullptr)
        return;

    _collectValueTP(vtp, o);
}

void TypeConverterVisitor::_collectValueTP(ValueTP *vtp, TypedObject *o)
{
    if (dynamic_cast<Time *>(hif::semantics::getBaseType(vtp->getType(), false, _sem)) == nullptr) {
        return;
    }

    if (o != nullptr) {
        _templateMap[vtp].insert(o);
    } else {
        _templateMap[vtp];
    }
}

std::string TypeConverterVisitor::_getTimeUnitString(TimeValue::TimeUnit u)
{
    switch (u) {
    case TimeValue::time_fs:
        return "HIF_TIME_FS";
    case TimeValue::time_ps:
        return "HIF_TIME_PS";
    case TimeValue::time_ns:
        return "HIF_TIME_NS";
    case TimeValue::time_us:
        return "HIF_TIME_US";
    case TimeValue::time_ms:
        return "HIF_TIME_MS";
    case TimeValue::time_sec:
        return "HIF_TIME_SEC";
    case TimeValue::time_min:
        return "HIF_TIME_MIN";
    case TimeValue::time_hr:
        return "HIF_TIME_HR";
    default:;
    }

    messageError("Unxpected case", nullptr, nullptr);
}

int TypeConverterVisitor::visitValueTP(ValueTP &o)
{
    GuideVisitor::visitValueTP(o);
    _collectValueTP(&o);
    return 0;
}

int TypeConverterVisitor::visitIdentifier(Identifier &o)
{
    GuideVisitor::visitIdentifier(o);
    _collectSymbols(&o);
    return 0;
}

int TypeConverterVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    GuideVisitor::visitValueTPAssign(o);
    _collectSymbols(&o);
    return 0;
}

// ///////////////////////////////////////////////////////////////////
// MoveVisitor
// ///////////////////////////////////////////////////////////////////
class MoveVisitor : public hif::GuideVisitor
{
public:
    /// @brief Default constructor.
    MoveVisitor(hif::semantics::ILanguageSemantics *sem, const FixTemplateOptions &opt);

    /// @brief Destructor.
    virtual ~MoveVisitor();

    /// @name Visitor of objects that have to be fixed
    /// @{

    virtual int visitRange(hif::Range &o);
    virtual int visitValueTP(hif::ValueTP &o);
    virtual int visitConst(hif::Const &o);

    /// @}

    /// @name Visitor of objects that can be references
    /// @{

    virtual int visitIdentifier(Identifier &);
    virtual int visitParameterAssign(ParameterAssign &);
    virtual int visitFieldReference(FieldReference &o);
    virtual int visitFunctionCall(FunctionCall &);
    virtual int visitInstance(Instance &o);
    virtual int visitProcedureCall(ProcedureCall &);
    virtual int visitValueTPAssign(ValueTPAssign &);

    /// @}

    /// @name Visitor of declarations creating new scopes
    /// @{

    virtual int visitFunction(Function &o);
    virtual int visitProcedure(Procedure &o);
    virtual int visitView(View &o);
    virtual int visitLibraryDef(LibraryDef &o);
    virtual int visitSystem(System &o);

    /// @}

    /// @name Visitor of special cases to be simplified before GuideVisitor
    /// @{

    virtual int visitFor(For &o);

    /// @}

protected:
    /// @brief Check if given data declaration is alredy a template.
    /// Internally it call the global method _checkIsAlreadyTemplate
    /// adding eventual check on presence of declaration in deleted set.
    /// @param decl The declaration to check.
    /// @return True if declaration is already a template parameter.
    ///
    bool _isAlredyTemplate(Declaration *decl);

    /// @brief Moves the given declaration to "template". This is done or
    /// translating the declaration to valueTP and adding it to correct
    /// template declaratio list, or adding the property "MOVE_TO_DEFINE"
    /// to the declaration directly inside a library def.
    /// @param decl The declaration to check.
    /// @return The eventual new DataDeclaration if declaration type is moved.
    ///
    DataDeclaration *_moveToTemplate(DataDeclaration *decl);

    /// @brief Starting from given declaration this method creates a
    /// corresponding valueTP. It also add the original declaration inside
    /// _deleteSet.
    /// @param c The declaration to transform.
    /// @param declarations Reference to the list where put new valueTP.
    /// @return The new ValueTP.
    ///
    ValueTP *_makeValueTP(DataDeclaration *c, BList<Declaration> &declarations);

    /// @brief Starting from given constants this method adds the property
    /// PROPERTY_MOVE_TO_DEFINE necessary to printer for print it as define.
    /// @param c The constant to fix.
    /// @param parentScope The parent scope of declaration to move (used
    /// only for warnings).
    ///
    void _makeDefine(DataDeclaration *c, Scope *parentScope);

    /// @brief If necessary move parameter assign as template parameter assign.
    /// Inside this method creates the corresponding valueTPAssign and push it
    /// inside parent template parameter assign list.
    /// The pass is not immediately removed but only inserted into _deleteSet.
    /// @param pass The parameter assign.
    /// @return The created value template parameter assign.
    ///
    ValueTPAssign *_moveParameterToTemplate(ParameterAssign *pass);

    /// @brief If given reference is not already a template, it call
    /// _moveToTemplate function and call current visitor on new declaration
    /// (if returned) setting the new _recursionPosition pointer.
    /// @param o The object to manage.
    ///
    template <typename T> void _manageReference(T *o);

    /// @brief Print eventual warning when try to move as constant (template
    /// parameter o define a variable.
    /// @param c The declaration to check.
    /// @param parent The parent scope where declaration will be moved.
    /// @param where The string rapresenting where the declaration will be
    /// moved (i.e. template / define )
    ///
    void _printEventualMoveVariableWarning(DataDeclaration *c, Object *parent, std::string where);
    void _manageTemplateDeclaration(Object *o);

    /// @brief Checks whether a range makes a bad scope.
    bool _rangeIsInBadScope(hif::Range &o);

    /// @brief Checks whether the given librarydef is standard.
    bool _isSystemCStandardLibrary(LibraryDef *o);

private:
    /// @brief The semantics on witch works the visitor.
    hif::semantics::ILanguageSemantics *_sem;

    /// @brief Flag that identify if current scope is a bad scope that is
    /// when eventual symbols must be constants.
    bool _isInBadScope;

    /// @brief Pointer that identify where push the eventual new declaration.
    /// If nullptr declaration in push backed, otherwise is inserted before object
    /// pointed by this variable.
    DataDeclaration *_recursionPosition;

    /// @brief Pointer that identify the last valueTP of functions and procedures.
    /// This is used to push the valueTP created starting from a parameter.
    /// That always must be declared before other value template parameter.
    Declaration *_paramPosition;

    /// @brief The set of moved objects. In visitor destructor the objects in list
    /// are removed from tree and deleted.
    hif::Trash _trash;

    /// @brief Set used to break recursive function calls
    std::set<hif::SubProgram *> _currentSubs;

    typedef std::set<For *> ForSet;
    ForSet _forSet;

    typedef std::set<Object *> VisitedSet;
    VisitedSet _visitedSet;

    /// @brief Stores standard declarations which create a bad scope,
    /// but which are not moved to templates w.r.t. fixStandardDeclarations option.
    /// E.g. a parameter of standard method, requires assigned values to be constant.
    Declarations _standardConstDecls;

    const FixTemplateOptions &_opt;

    // disabled
    MoveVisitor(const MoveVisitor &);
    MoveVisitor &operator=(const MoveVisitor &);
};

MoveVisitor::MoveVisitor(hif::semantics::ILanguageSemantics *sem, const FixTemplateOptions &opt)
    : _sem(sem)
    , _isInBadScope(false)
    , _recursionPosition(nullptr)
    , _paramPosition(nullptr)
    , _trash()
    , _currentSubs()
    , _forSet()
    , _visitedSet()
    , _standardConstDecls()
    , _opt(opt)
{
    // ntd
}

MoveVisitor::~MoveVisitor() { _trash.clear(); }

// ///////////////////////////////////////////////////////////////////
//  Support methods
// ///////////////////////////////////////////////////////////////////

ValueTP *MoveVisitor::_makeValueTP(DataDeclaration *c, BList<Declaration> &declarations)
{
    if (!_opt.fixStandardDeclarations && hif::declarationIsPartOfStandard(c)) {
        // Ref. design: vhdl/openCores/fpmul + DDT
        Parameter *pa = dynamic_cast<Parameter *>(c);
        if (pa != nullptr) {
            _standardConstDecls.insert(pa);
        }
        return nullptr;
    }

    Scope *oldScope = hif::getNearestParent<Scope>(c);
    For *forScope   = hif::getNearestParent<For>(c);
    messageAssert(oldScope != nullptr || forScope != nullptr, "Unexpected case", oldScope, _sem);
    Scope *newScope = dynamic_cast<Scope *>(declarations.getParent());
    messageAssert(newScope != nullptr, "Unexpected case", oldScope, _sem);
    if (forScope != nullptr)
        hif::manipulation::moveToScope(forScope, newScope, _sem, "", c);
    else
        hif::manipulation::moveToScope(oldScope, newScope, _sem, "", c);

    _printEventualMoveVariableWarning(c, declarations.getParent(), "template");

    // Building new vtp
    ValueTP *vtp = new ValueTP();
    vtp->setName(c->getName());
    vtp->setType(c->getType());
    vtp->setValue(c->getValue());

#ifdef DEBUG_INSERTIONS
    clog << "PARENT NAME: " << hif::objectGetName(c->getParent()) << endl;
    clog << "LISTA ATTUALE: " << endl;
    for (BList<Declaration>::iterator i = declarations.begin(); i != declarations.end(); ++i) {
        clog << hif::objectGetName(*i) << endl;
    }
#endif

    if (_recursionPosition == nullptr) {
        if (dynamic_cast<Parameter *>(c) != nullptr) {
            // Since declarations of parameters appear before any other
            // subprogram declaration, such a relative order must be preserved
            // to allow compiling.
            if (_paramPosition == nullptr) {
#ifdef DEBUG_INSERTIONS
                clog << "PUSH BACK: " << hif::objectGetName(vtp) << endl;
#endif
                declarations.push_back(vtp);
            } else {
                messageAssert(
                    _paramPosition->getBList() == reinterpret_cast<BList<Object> *>(&declarations),
                    "Unexpected position", _paramPosition, _sem);
                BList<Declaration>::iterator it(_paramPosition);
                it.insert_after(vtp);
                _paramPosition = vtp;
#ifdef DEBUG_INSERTIONS
                clog << "INSERT: " << hif::objectGetName(vtp) << " AFTER: " << hif::objectGetName(*it) << endl;
#endif
            }
        } else {
#ifdef DEBUG_INSERTIONS
            clog << "PUSH BACK: " << hif::objectGetName(vtp) << endl;
#endif
            declarations.push_back(vtp);
        }
    } else {
        messageAssert(
            _recursionPosition->getBList() == reinterpret_cast<BList<Object> *>(&declarations), "Unexpected position",
            _paramPosition, _sem);
        BList<DataDeclaration>::iterator it(_recursionPosition);
        it.insert_before(vtp);

#ifdef DEBUG_INSERTIONS
        clog << "INSERT: " << hif::objectGetName(vtp) << " BEFORE: " << hif::objectGetName(*it) << endl;
#endif
    }

#ifdef DEBUG_INSERTIONS
    clog << "-------------------------------------------------------\n";
#endif

    // Do not remove declaration field since can be referenced by semantic types.
    c->setType(hif::copy(vtp->getType()));
    c->setValue(hif::copy(vtp->getValue()));

    // Removing c from tree
    _trash.insert(c);

    return vtp;
}

void MoveVisitor::_makeDefine(DataDeclaration *decl, Scope *parentScope)
{
    if (definesMap.find(decl) != definesMap.end())
        return;
    _printEventualMoveVariableWarning(decl, parentScope, "define");
    Const *c    = dynamic_cast<Const *>(decl);
    Variable *v = dynamic_cast<Variable *>(decl);
    messageAssert(c != nullptr || v != nullptr, "Unexpected declaration", decl, _sem);
    // Renaming in <name>_define:
    std::string n = NameTable::getInstance()->getFreshName(decl->getName(), "_define");

    // Adding property define:
    definesMap.insert(std::make_pair(decl, n));
    if (c != nullptr)
        c->setDefine(true);
}

bool MoveVisitor::_isAlredyTemplate(Declaration *decl)
{
    if (dynamic_cast<Parameter *>(decl) != nullptr && goodTypeMap.find(decl) != goodTypeMap.end()) {
        return false;
    }

    if (_checkIsAlreadyTemplate(decl, true)) {
        return true;
    }

    const bool isInTrash = _trash.contains(decl);
    return isInTrash;
}

DataDeclaration *MoveVisitor::_moveToTemplate(DataDeclaration *decl)
{
    System *s = hif::getNearestParent<System>(decl);
    if (s == nullptr) {
        // May be nullptr in case of standard function.
        return nullptr;
    }

    For *f = dynamic_cast<For *>(decl->getParent());
    if (f != nullptr) {
        // will be fixed after guide visitor
        _forSet.insert(f);
        return nullptr;
    }

    // Looking for nearest compatible parent object that is object where
    // can appear the bad declaration.
    SubProgram *sub = hif::getNearestParent<SubProgram>(decl);
    View *v         = hif::getNearestParent<View>(decl);
    LibraryDef *ld  = hif::getNearestParent<LibraryDef>(decl);

    DataDeclaration *ret = nullptr;
    if (sub != nullptr) {
        // Case of bad declaration is inside a subprogram.

        // Creating valueTP starting from declaration to move:
        ValueTP *vtp = _makeValueTP(decl, sub->templateParameters);
        ret          = vtp;
    } else if (v != nullptr) {
        // Case of bad declaration is inside a view.

        // Creating valueTP starting from declaration to move:
        ValueTP *vtp = _makeValueTP(decl, v->templateParameters);
        ret          = vtp;
    } else if (ld != nullptr) {
        // Adding property define to constant and rename declaration adding
        // subscript _define:
        _makeDefine(decl, ld);
        ret = decl;
    } else if (s != nullptr) {
        // Adding property define to constant and rename declaration adding
        // subscript _define:
        _makeDefine(decl, s);
        ret = decl;
    } else {
        messageError("Unexpected case", decl, _sem);
    }

    return ret;
}

template <typename T> void MoveVisitor::_manageReference(T *o)
{
    if (!_isInBadScope)
        return;

    typename T::DeclarationType *decl = hif::semantics::getDeclaration(o, _sem);
    if (decl == nullptr) {
        messageDebugAssert(false, "Declaration not found", o, _sem);
        return;
    }

    if (_isAlredyTemplate(decl))
        return;

    DataDeclaration *ddecl = dynamic_cast<DataDeclaration *>(decl);
    messageAssert(ddecl != nullptr, "Expected DataDeclaration", decl, _sem);

    // Enum values are fine.
    if (dynamic_cast<EnumValue *>(ddecl) != nullptr)
        return;

    DataDeclaration *newDecl = _moveToTemplate(ddecl);
    if (newDecl == nullptr)
        return;

    DataDeclaration *r = _recursionPosition;
    _recursionPosition = newDecl;
    newDecl->acceptVisitor(*this);
    _recursionPosition = r;
}
void MoveVisitor::_printEventualMoveVariableWarning(DataDeclaration *c, Object *parent, std::string where)
{
    if (dynamic_cast<Variable *>(c) == nullptr)
        return;

    Declaration *scope       = dynamic_cast<Declaration *>(parent);
    std::string parentString = (parent != nullptr) ? (std::string(" (of ") + scope->getName() + ")") : "";

    std::string info;
    if (c->getSourceLineNumber() != 0) {
        std::stringstream ss;
        ss << c->getSourceFileName() << ": " << c->getSourceLineNumber();
        info = ss.str();
    }

    messageWarning(
        std::string("Moving variable ") + c->getName() + parentString + " to " + where + "." +
            " Variable should be a constant in original design: " + info,
        nullptr, nullptr);
}

void MoveVisitor::_manageTemplateDeclaration(Object *o)
{
    if (!_isInBadScope)
        return;
    Declaration *decl = hif::semantics::getDeclaration(o, _sem);
    messageAssert(decl != nullptr, "Declaration not found", o, _sem);
    ValueTP *vtp = dynamic_cast<ValueTP *>(decl);
    if (vtp == nullptr || vtp->isCompileTimeConstant())
        return;
    vtp->setCompileTimeConstant(true);
    vtp->acceptVisitor(*this);
}
ValueTPAssign *MoveVisitor::_moveParameterToTemplate(ParameterAssign *pass)
{
    Parameter *decl = hif::semantics::getDeclaration(pass, _sem);
    messageAssert(decl != nullptr, "Declaration not found", pass, _sem);

    // Moving paramass to vtpa only when its declaration has been moved.
    if (!_isAlredyTemplate(decl))
        return nullptr;

    // Already fixed.
    if (_trash.contains(pass))
        return nullptr;

    // Getting parent function or procedure calls:
    Object *call         = pass->getParent();
    FunctionCall *fcall  = dynamic_cast<FunctionCall *>(call);
    ProcedureCall *pcall = dynamic_cast<ProcedureCall *>(call);
    messageAssert(fcall != nullptr || pcall != nullptr, "Unexpected call", call, _sem);

    // Creating new value TP assign staring from parameter assigns:
    ValueTPAssign *vtpa = new ValueTPAssign();
    vtpa->setName(pass->getName());
    vtpa->setValue(pass->getValue());
    pass->setValue(nullptr);

    // Replacing in tree and removes parameter assigns from references
    // list:
    _trash.insert(pass);

    // Adding in correct list of TP assing:
    if (fcall != nullptr) {
        fcall->templateParameterAssigns.push_back(vtpa);
    } else // if (pcall != nullptr)
    {
        pcall->templateParameterAssigns.push_back(vtpa);
    }

    return vtpa;
}
bool MoveVisitor::_rangeIsInBadScope(hif::Range &o)
{
    Object *parent = o.getParent();
    messageAssert(parent != nullptr, "Unexpected unrelated object", &o, _sem);
    Slice *slice = dynamic_cast<Slice *>(parent);
    if (slice == nullptr && dynamic_cast<For *>(parent) == nullptr && dynamic_cast<ForGenerate *>(parent) == nullptr) {
        return true;
    }

    if (slice == nullptr)
        return false;

    // TODO remove this shit!
    // In case of: arr.range(a,b) = aggregate{...}
    // the aggregate will be mapped to fcall make_aggregate(),
    // which has a int template parameter, which is the size of the
    // target of assignment. Therefore, its range must be constant!
    Type *tt =                                        //hif::semantics::getBaseType(
        hif::semantics::getSemanticType(slice, _sem); //,
    //   false, _sem );
    messageAssert(tt != nullptr, "Cannot find slice type", slice, _sem);

    Array *typeA       = dynamic_cast<Array *>(tt);
    Bitvector *typeB   = dynamic_cast<Bitvector *>(tt);
    Signed *typeS      = dynamic_cast<Signed *>(tt);
    Unsigned *typeU    = dynamic_cast<Unsigned *>(tt);
    String *typeString = dynamic_cast<String *>(tt);
    // Support of ddt manipulations:
    // Ref design: vhdl/intecs/micro_interface + ddt + hif2vp
    Int *typeInt       = dynamic_cast<Int *>(tt);

    if (typeB == nullptr && typeS == nullptr && typeU == nullptr && typeA == nullptr && typeString == nullptr &&
        typeInt == nullptr) {
        messageError("Unexpected type of slice.", slice, _sem);
    }

    if (typeString != nullptr) {
        // nothing to do since in SystemC Strings have no bounds.
        return false;
    }

    if (_opt.useHdtLib && !hif::manipulation::isInLeftHandSide(slice))
        return true;

    // @warning With the code decommented, vhdl/ips/mep_sp_ram design
    // wrongly unrolls some for loops.
    // On the other hand, some designs could introduce casts,
    // and therefore the spans must be constants.
    // For this case, we have no actual broken design,
    // since all introduced casts are removed by simplify.
    // E.g. vhdl/custom/crc and vhdl/custom/test_span_direction.

    //if (typeA != nullptr)
    {
        Assign *aaa = dynamic_cast<Assign *>(parent->getParent());
        if (aaa == nullptr)
            return false;
        Aggregate *aggr = dynamic_cast<Aggregate *>(aaa->getRightHandSide());
        if (aggr == nullptr)
            return false;

        //return true;
    }

    return true;
}

bool MoveVisitor::_isSystemCStandardLibrary(LibraryDef *o)
{
    if (o == nullptr)
        return false;
    if (!o->isStandard())
        return false;
    std::string s(o->getName());
    std::string prefix("hif_systemc_");
    if (s.substr(0, prefix.size()) != prefix)
        return false;
    s = s.substr(prefix.size());
    return _opt.checkSem->isNativeLibrary(s);
}
// ///////////////////////////////////////////////////////////////////
//  Visitor of objects that have to be fixed
// ///////////////////////////////////////////////////////////////////
int MoveVisitor::visitRange(hif::Range &o)
{
    bool r        = _isInBadScope;
    _isInBadScope = _rangeIsInBadScope(o);
    GuideVisitor::visitRange(o);
    _isInBadScope = r;
    return 0;
}

int MoveVisitor::visitValueTP(hif::ValueTP &o)
{
    if (_visitedSet.find(&o) != _visitedSet.end())
        return 0;
    if (o.isCompileTimeConstant())
        _visitedSet.insert(&o);
    bool r              = _isInBadScope;
    _isInBadScope       = o.isCompileTimeConstant();
    DataDeclaration *rp = _recursionPosition;
    _recursionPosition  = &o;
    GuideVisitor::visitValueTP(o);
    _recursionPosition = rp;
    _isInBadScope      = r;
    return 0;
}

int MoveVisitor::visitConst(hif::Const &o)
{
    // TODO: check if it is required that all const create a bad scope.
    // e.g. const = var.
    bool r              = _isInBadScope;
    DataDeclaration *rp = _recursionPosition;
    if (definesMap.find(&o) != definesMap.end()) {
        _isInBadScope      = true;
        _recursionPosition = &o;
    } else {
        _isInBadScope = false;
    }
    GuideVisitor::visitConst(o);
    _recursionPosition = rp;
    _isInBadScope      = r;
    return 0;
}
// ///////////////////////////////////////////////////////////////////
// objects that can be references inside a bad scope
// ///////////////////////////////////////////////////////////////////

int MoveVisitor::visitIdentifier(Identifier &o)
{
    GuideVisitor::visitIdentifier(o);
    _manageTemplateDeclaration(&o);
    _manageReference(&o);
    return 0;
}

int MoveVisitor::visitParameterAssign(ParameterAssign &o)
{
    // Function/Procedure calls have been already visited and therefore
    // their parameter declarations have been already moved to templates.
    // Move from parameter assign to template parameter assign
    ValueTPAssign *vtpa = _moveParameterToTemplate(&o);

    if (vtpa == nullptr) {
        Parameter *param = hif::semantics::getDeclaration(&o, _sem);
        messageAssert(param != nullptr, "Declaration not found", &o, _sem);

        if (_standardConstDecls.find(param) != _standardConstDecls.end()) {
            const bool restore = _isInBadScope;
            _isInBadScope      = true;
            GuideVisitor::visitParameterAssign(o);
            _isInBadScope = restore;
            return 0;
        }

        if (_isInBadScope) {
            if (!_opt.checkSem->isTemplateAllowedType(param->getType()) && !_isAlredyTemplate(param) &&
                goodTypeMap.find(param) == goodTypeMap.end()) {
                Type *goodType     = _opt.checkSem->getTemplateAllowedType(param->getType());
                goodTypeMap[param] = goodType;
            }
        }
        GuideVisitor::visitParameterAssign(o);
        return 0;
    }

    vtpa->acceptVisitor(*this);

    return 0;
}

int MoveVisitor::visitFieldReference(FieldReference &o)
{
    // We have to move prefix in bad scope. Therefore calling guide visitor
    // should be enough.
    GuideVisitor::visitFieldReference(o);

    // managing prefixed identifiers
    FieldReference::DeclarationType *decl = hif::semantics::getDeclaration(&o, _sem);
    if (dynamic_cast<Const *>(decl) != nullptr) {
        _manageReference(&o);
    }

    return 0;
}

int MoveVisitor::visitFunctionCall(FunctionCall &o)
{
    FunctionCall::DeclarationType *decl = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(decl != nullptr, "Declaration not found", &o, _sem);

    if (_isInBadScope) {
        if (!_isAlredyTemplate(decl)) {
            if (!decl->checkProperty(PROPERTY_CONSTEXPR)) {
                if (_opt.setConstExpr)
                    decl->addProperty(PROPERTY_CONSTEXPR);
            }

            if (!_opt.checkSem->isTemplateAllowedType(decl->getType())) {
                Type *goodType    = _opt.checkSem->getTemplateAllowedType(decl->getType());
                goodTypeMap[decl] = goodType;
            } else {
                goodTypeMap[decl] = nullptr;
            }
        }
    }

    decl->acceptVisitor(*this);

    GuideVisitor::visitFunctionCall(o);
    return 0;
}

int MoveVisitor::visitInstance(Instance &o)
{
    GuideVisitor::visitInstance(o);
    if (_isInBadScope && dynamic_cast<BaseContents *>(o.getParent()) != nullptr) {
        messageDebugAssert(false, "Unexpected object in bad scope", &o, _sem);
    }
    return 0;
}

int MoveVisitor::visitProcedureCall(ProcedureCall &o)
{
    ProcedureCall::DeclarationType *decl = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(decl != nullptr, "Declaration not found", &o, _sem);

    decl->acceptVisitor(*this);

    GuideVisitor::visitProcedureCall(o);
    return 0;
}

int MoveVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    ValueTP *vtp = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(vtp != nullptr, "Declaration not found", &o, _sem);
    View *pview = dynamic_cast<View *>(vtp->getParent());
    if (pview != nullptr) {
        pview->acceptVisitor(*this);
    }

    bool r        = _isInBadScope;
    _isInBadScope = vtp->isCompileTimeConstant();

    // _recursionPosition must be preserved in case of recursive calls.
    // E.g. vtp v = foo(a) with a which must be moved before v.

    GuideVisitor::visitValueTPAssign(o);

    _isInBadScope = r;
    return 0;
}

// ///////////////////////////////////////////////////////////////////
// Visitor of declarations creating new scopes
// ///////////////////////////////////////////////////////////////////

int MoveVisitor::visitFunction(Function &o)
{
    // Break recursive calls
    if (_currentSubs.find(&o) != _currentSubs.end())
        return 0;
    _currentSubs.insert(&o);

    DataDeclaration *rr = _recursionPosition;
    _recursionPosition  = nullptr;
    Declaration *pr     = _paramPosition;
    _paramPosition      = o.templateParameters.empty() ? nullptr : o.templateParameters.back();
    bool r              = _isInBadScope;
    _isInBadScope       = o.checkProperty(PROPERTY_CONSTEXPR);
    GuideVisitor::visitFunction(o);
    _isInBadScope      = r;
    _paramPosition     = pr;
    _recursionPosition = rr;

    _currentSubs.erase(&o);
    return 0;
}

int MoveVisitor::visitProcedure(Procedure &o)
{
    // Break recursive calls
    if (_currentSubs.find(&o) != _currentSubs.end())
        return 0;
    _currentSubs.insert(&o);

    DataDeclaration *rr = _recursionPosition;
    _recursionPosition  = nullptr;
    Declaration *pr     = _paramPosition;
    _paramPosition      = o.templateParameters.empty() ? nullptr : o.templateParameters.back();
    bool r              = _isInBadScope;
    _isInBadScope       = o.checkProperty(PROPERTY_CONSTEXPR);
    GuideVisitor::visitProcedure(o);
    _isInBadScope      = r;
    _paramPosition     = pr;
    _recursionPosition = rr;

    _currentSubs.erase(&o);
    return 0;
}

int MoveVisitor::visitView(View &o)
{
    if (_visitedSet.find(&o) != _visitedSet.end())
        return 0;
    _visitedSet.insert(&o);
    LibraryDef *pLibDef = hif::getNearestParent<LibraryDef>(&o);
    if (_isSystemCStandardLibrary(pLibDef))
        return 0;

    DataDeclaration *rr = _recursionPosition;
    _recursionPosition  = nullptr;
    bool r              = _isInBadScope;
    _isInBadScope       = false;
    GuideVisitor::visitView(o);
    _isInBadScope      = r;
    _recursionPosition = rr;
    return 0;
}

int MoveVisitor::visitLibraryDef(LibraryDef &o)
{
    if (_isSystemCStandardLibrary(&o))
        return 0;
    DataDeclaration *rr = _recursionPosition;
    _recursionPosition  = nullptr;
    bool r              = _isInBadScope;
    _isInBadScope       = false;
    GuideVisitor::visitLibraryDef(o);
    _isInBadScope      = r;
    _recursionPosition = rr;
    return 0;
}

int MoveVisitor::visitSystem(System &o)
{
    DataDeclaration *rr = _recursionPosition;
    _recursionPosition  = nullptr;
    bool r              = _isInBadScope;
    _isInBadScope       = false;
    GuideVisitor::visitSystem(o);
    _isInBadScope      = r;
    _recursionPosition = rr;
    return 0;
}

int MoveVisitor::visitFor(For &o)
{
    GuideVisitor::visitFor(o);

    if (_forSet.find(&o) == _forSet.end())
        return 0;

    hif::manipulation::SimplifyOptions opt;
    opt.simplify_constants           = true;
    opt.simplify_template_parameters = true;
    opt.simplify_statements          = true;
    hif::manipulation::simplify(o.getCondition(), _sem, opt);
    hif::manipulation::simplify(o.initDeclarations, _sem, opt);

    hif::manipulation::SimplifyOptions opt2;
    opt2.simplify_statements = true;
    hif::manipulation::simplify(&o, _sem, opt2);

    return 0;
}
// ///////////////////////////////////////////////////////////////////
// FixReferencesVisitor
// ///////////////////////////////////////////////////////////////////
class FixReferencesVisitor : public hif::GuideVisitor
{
public:
    /// @brief Default constructor.
    FixReferencesVisitor(hif::semantics::ILanguageSemantics *sem, const FixTemplateOptions &opt);

    /// @brief Destructor.
    virtual ~FixReferencesVisitor();

    /// @name Visitor of objects that can be references
    /// @{

    virtual int visitIdentifier(Identifier &o);
    virtual int visitFieldReference(FieldReference &o);
    virtual int visitValueTPAssign(ValueTPAssign &o);
    virtual int visitParameterAssign(ParameterAssign &o);
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitReturn(Return &o);

    /// @}
    /// @name Visitors used to collect declarations.
    /// @{

    virtual int visitConst(Const &);
    virtual int visitValueTP(ValueTP &);

    /// @}

protected:
    Type *_getGoodType(Declaration *decl, Type *badType);

    void _renameEventualDefine(Identifier *ref);

    template <typename T> void _fixReference(T *ref, typename T::DeclarationType *decl);

    void _fixBadGeneralRef(ValueTPAssign *ref, Type *badType, Type *goodType);
    void _fixBadGeneralRef(Identifier *ref, Type *badType, Type *goodType);
    void _fixBadGeneralRef(FieldReference *ref, Type *badType, Type *goodType);
    void _fixBadGeneralRef(ParameterAssign *ref, Type *badType, Type *goodType);
    void _fixBadGeneralRef(FunctionCall *ref, Type *badType, Type *goodType);
    void _fixBadGeneralRef(Return *ref, Type *badType, Type *goodType);

    void _fixBadStringRef(ValueTPAssign *ref, Type *badType, Type *goodType);
    void _fixBadStringRef(Identifier *ref, Type *badType, Type *goodType);
    void _fixBadStringRef(FieldReference *ref, Type *badType, Type *goodType);
    void _fixBadStringRef(ParameterAssign *ref, Type *badType, Type *goodType);
    void _fixBadStringRef(FunctionCall *ref, Type *badType, Type *goodType);
    void _fixBadStringRef(Return *ref, Type *badType, Type *goodType);

    void _fixBadRecordRef(ValueTPAssign *ref, Record *badType, Record *goodType);
    void _fixBadRecordRef(Identifier *ref, Record *badType, Record *goodType);
    void _fixBadRecordRef(FieldReference *ref, Record *badType, Record *goodType);
    void _fixBadRecordRef(ParameterAssign *ref, Record *badType, Record *goodType);
    void _fixBadRecordRef(FunctionCall *ref, Record *badType, Record *goodType);
    void _fixBadRecordRef(Return *ref, Record *badType, Record *goodType);

    void _fixBadTimeRef(ValueTPAssign *ref, Time *badType, Record *goodType);
    void _fixBadTimeRef(Identifier *ref, Time *badType, Record *goodType);
    void _fixBadTimeRef(FieldReference *ref, Time *badType, Record *goodType);
    void _fixBadTimeRef(ParameterAssign *ref, Time *badType, Record *goodType);
    void _fixBadTimeRef(FunctionCall *ref, Time *badType, Record *goodType);
    void _fixBadTimeRef(Return *ref, Time *badType, Record *goodType);

private:
    hif::semantics::ILanguageSemantics *_sem;
    ReferencesSet _deleteSet;

    const FixTemplateOptions &_opt;

    FixReferencesVisitor(const FixReferencesVisitor &);
    FixReferencesVisitor &operator=(const FixReferencesVisitor &);
};
FixReferencesVisitor::FixReferencesVisitor(hif::semantics::ILanguageSemantics *sem, const FixTemplateOptions &opt)
    : _sem(sem)
    , _deleteSet()
    , _opt(opt)
{
}

FixReferencesVisitor::~FixReferencesVisitor()
{
    for (ReferencesSet::iterator i = _deleteSet.begin(); i != _deleteSet.end(); ++i) {
        (*i)->replace(nullptr);
        delete *i;
    }
}

// ///////////////////////////////////////////////////////////////////
// Support functions
// ///////////////////////////////////////////////////////////////////

Type *FixReferencesVisitor::_getGoodType(Declaration *decl, Type *badType)
{
    GoodTypeMap::iterator it = goodTypeMap.find(decl);
    if (it != goodTypeMap.end())
        return hif::copy(it->second);

    Type *goodType = _opt.checkSem->getTemplateAllowedType(badType);
    if (dynamic_cast<Record *>(goodType) != nullptr) {
        Record *recGood = static_cast<Record *>(goodType);
        for (BList<Field>::iterator i = recGood->fields.begin(); i != recGood->fields.end(); ++i) {
            _setNewFieldName(*i, decl);
        }
    }

    goodTypeMap[decl] = goodType;
    return hif::copy(goodType);
}

void FixReferencesVisitor::_renameEventualDefine(Identifier *ref)
{
    Identifier::DeclarationType *decl = hif::semantics::getDeclaration(ref, _sem);
    messageAssert(decl != nullptr, "Expected declaration", ref, _sem);

    Const *c = dynamic_cast<Const *>(decl);
    if (c == nullptr)
        return;
    messageAssert(definesMap.find(c) != definesMap.end(), "Expected in define map", c, _sem);

    ref->setName(definesMap[c]);
}

template <typename T> void FixReferencesVisitor::_fixReference(T *ref, typename T::DeclarationType *decl)
{
    if (!_checkIsAlreadyTemplate(decl, false))
        return;

    DataDeclaration *ddecl = dynamic_cast<DataDeclaration *>(decl);
    Function *fdecl        = dynamic_cast<Function *>(decl);
    messageAssert((ddecl != nullptr || fdecl != nullptr), "Unsupported case", decl, _sem);

    // Storing syntactical bad type of declaration:
    Type *badType = nullptr;
    if (ddecl != nullptr) {
        badType = ddecl->getType();
    } else if (fdecl != nullptr) {
        TypedObject *tobj = dynamic_cast<TypedObject *>(ref);
        if (tobj != nullptr)
            badType = hif::semantics::getSemanticType(tobj, _sem); // need instantiation
        else
            badType = fdecl->getType();
    }

    Const *constDecl         = dynamic_cast<Const *>(ddecl);
    const bool isConstDefine = (constDecl != nullptr && constDecl->isDefine());

    // If declaration type is good there is nothing to do.
    if (_opt.checkSem->isTemplateAllowedType(badType) && !isConstDefine) {
        // NOTE: we must go on with define constant since the fix (moving and
        // renaming) is done in following methods.
        // Ref. design: vhdl/custom/crc, vhdl/custom/test_span_direction
        return;
    }

    // Getting the base bad type of declaration:
    Type *baseBadType = hif::semantics::getBaseType(badType, false, _sem);

    // Getting the good declaration type.
    Type *goodType = _getGoodType(decl, baseBadType);

    // Calling right fix checking badType.
    if (dynamic_cast<String *>(baseBadType) != nullptr) {
        _fixBadStringRef(ref, badType, goodType);
    } else if (dynamic_cast<Record *>(baseBadType) != nullptr) {
        _fixBadRecordRef(ref, static_cast<Record *>(baseBadType), static_cast<Record *>(goodType));
    } else if (dynamic_cast<Time *>(baseBadType) != nullptr) {
        _fixBadTimeRef(ref, static_cast<Time *>(baseBadType), static_cast<Record *>(goodType));
    } else if (goodType != nullptr) {
        _fixBadGeneralRef(ref, badType, goodType);
    }
}

void FixReferencesVisitor::_fixBadGeneralRef(ValueTPAssign *ref, Type * /*badType*/, Type *goodType)
{
    // Adding cast of value to goodType.
    Cast *c = new Cast();
    c->setType(goodType);
    c->setValue(ref->getValue());
    ref->setValue(c);
}

void FixReferencesVisitor::_fixBadGeneralRef(Identifier *ref, Type *badType, Type *goodType)
{
    delete goodType;
    if (hif::objectIsInSensitivityList(ref)) {
        messageError("unexpected ref in sensitivity.", ref, _sem);
    }

    _renameEventualDefine(ref);

    // Case reference is a general Value:
    // Adding cast of value to original badType.
    Cast *c = new Cast();
    c->setType(hif::copy(badType));
    ref->replace(c);
    c->setValue(ref);
    c->getType()->acceptVisitor(*this);
}

void FixReferencesVisitor::_fixBadGeneralRef(FieldReference *ref, Type *badType, Type *goodType)
{
    FieldReference::DeclarationType *dec = hif::semantics::getDeclaration(ref, _sem);
    messageAssert(dec != nullptr, "Expected declaration", ref, _sem);

    Const *cDec = dynamic_cast<Const *>(dec);
    if (cDec != nullptr) {
        LibraryDef *lib = dynamic_cast<LibraryDef *>(cDec->getParent());
        messageAssert(lib != nullptr, "Expected constant inside library def", cDec, _sem);

        Scope *s = hif::getNearestScope(ref, false, true, false);
        if (dynamic_cast<Contents *>(s) != nullptr)
            s = static_cast<Scope *>(s->getParent());

        Library *libRef = new Library();
        libRef->setName(lib->getName());
        libRef->setSystem(lib->isStandard());

        hif::manipulation::AddUniqueObjectOptions addOpt;
        addOpt.equalsOptions.checkOnlyNames = true;
        addOpt.deleteIfNotAdded             = true;
        hif::manipulation::addUniqueObject(libRef, s, addOpt);
    }

    // Was part of Record or Const moved as define
    Identifier *f = new Identifier(ref->getName());
    ref->replace(f);
    delete ref;
    _fixBadGeneralRef(f, badType, goodType);
}

void FixReferencesVisitor::_fixBadGeneralRef(ParameterAssign *ref, Type * /*badType*/, Type *goodType)
{
    // Adding cast of value to goodType.
    Cast *c = new Cast();
    c->setType(goodType);
    c->setValue(ref->getValue());
    ref->setValue(c);
}

void FixReferencesVisitor::_fixBadGeneralRef(FunctionCall *ref, Type *badType, Type *goodType)
{
    delete goodType;

    Cast *c = new Cast();
    c->setType(hif::copy(badType));
    ref->replace(c);
    c->setValue(ref);
    c->getType()->acceptVisitor(*this);
}

void FixReferencesVisitor::_fixBadGeneralRef(Return *ref, Type * /*badType*/, Type *goodType)
{
    // Adding cast of value to goodType.
    Cast *c = new Cast();
    c->setType(goodType);
    c->setValue(ref->getValue());
    ref->setValue(c);
}

void FixReferencesVisitor::_fixBadStringRef(ValueTPAssign *ref, Type * /*badType*/, Type *goodType)
{
    delete goodType;
    // Case reference was a TPAssign:
    StringValue *text = dynamic_cast<StringValue *>(ref->getValue());
    if (text == nullptr) {
        Identifier *arg = dynamic_cast<Identifier *>(ref->getValue());
        if (arg != nullptr) {
            Identifier::DeclarationType *decl = hif::semantics::getDeclaration(arg, _sem);
            messageDebugAssert(decl != nullptr, "Unable to get declaration.", arg, _sem);
            ValueTP *vtp = dynamic_cast<ValueTP *>(decl);
            // If the template argument is an identifier naming a template parameter,
            // no further actions need to be taken
            messageAssert(vtp != nullptr, "Expected a value template parameter as declaration.", decl, _sem);
            return;
        }
        messageError("Expected a text object as value of template argument.", ref, _sem);
    }

    // add a new enum value entry into global enums.
    std::string n = _addStringEnumEntry(ref, text->getValue(), _sem);

    // Set valueTPAssign value with an identifier to created enum value.
    Identifier *enumRef = new Identifier(n);
    delete ref->setValue(enumRef);
}

void FixReferencesVisitor::_fixBadStringRef(Identifier *ref, Type * /*badType*/, Type *goodType)
{
    delete goodType;

    _renameEventualDefine(ref);

    // Case reference was an other general Value (e.g. identifier):
    Expression *exp = dynamic_cast<Expression *>(ref->getParent());
    // Could be the right hand side of an assignment.
    if (exp == nullptr)
        return;

    // Getting the other operand StringValue:
    StringValue *text = nullptr;
    if (exp->getValue1() == ref) {
        text = dynamic_cast<StringValue *>(exp->getValue2());
    } else {
        text = dynamic_cast<StringValue *>(exp->getValue1());
    }

    if (text == nullptr) {
        // Could be an expr as: constString == constString
        return;
    }

    // Creating and adding the new enum value:
    std::string n = _addStringEnumEntry(ref, text->getValue(), _sem);

    // Replacing text with an identifier with name of text:
    Identifier *id = new Identifier(n);
    text->replace(id);
    delete text;
}

void FixReferencesVisitor::_fixBadStringRef(FieldReference *ref, Type *badType, Type *goodType)
{
    Identifier *f = new Identifier(ref->getName());
    ref->replace(f);
    delete ref;

    _fixBadStringRef(f, badType, goodType);
}

void FixReferencesVisitor::_fixBadStringRef(ParameterAssign *ref, Type * /*badType*/, Type *goodType)
{
    delete goodType;
    // Case reference was a TPAssign:
    StringValue *text = dynamic_cast<StringValue *>(ref->getValue());
    messageAssert(text != nullptr, "Unexpected ref value", ref->getValue(), _sem);

    // add a new enum value entry into global enums.
    std::string n = _addStringEnumEntry(ref, text->getValue(), _sem);

    // Set valueTPAssign value with an identifier to created enum value.
    Identifier *enumRef = new Identifier(n);
    delete ref->setValue(enumRef);
}

void FixReferencesVisitor::_fixBadStringRef(FunctionCall *ref, Type *badType, Type *goodType)
{
    delete goodType;

    Cast *c = new Cast();
    c->setType(hif::copy(badType));
    ref->replace(c);
    c->setValue(ref);
    c->getType()->acceptVisitor(*this);
}

void FixReferencesVisitor::_fixBadStringRef(Return *ref, Type * /*badType*/, Type *goodType)
{
    delete goodType;
    // Case reference was a TPAssign:
    StringValue *text = dynamic_cast<StringValue *>(ref->getValue());
    messageAssert(text != nullptr, "Unexpected ref value", ref->getValue(), _sem);

    // add a new enum value entry into global enums.
    std::string nref = _addStringEnumEntry(ref, text->getValue(), _sem);

    // Set valueTPAssign value with an identifier to created enum value.
    Identifier *enumRef = new Identifier(nref);
    delete ref->setValue(enumRef);
}

void FixReferencesVisitor::_fixBadRecordRef(ValueTPAssign *ref, Record * /*badType*/, Record *goodType)
{
    messageAssert(ref->isInBList(), "Unexpected case", ref, _sem);

    ValueTP *decl = hif::semantics::getDeclaration(ref, _sem);
    messageAssert(decl != nullptr, "Declaration not found", ref, _sem);

    for (BList<Field>::iterator i = goodType->fields.begin(); i != goodType->fields.end(); ++i) {
        ValueTPAssign *vtpa = new ValueTPAssign();
        std::string n       = _setNewFieldName(*i, decl);
        vtpa->setName(n);
        FieldReference *fr = new FieldReference();
        fr->setName((*i)->getName());
        fr->setPrefix(hif::copy(ref->getValue()));
        Cast *c = new Cast();
        c->setType(hif::copy((*i)->getType()));
        c->setValue(fr);
        vtpa->setValue(c);
        // Adding new assign:
        BList<TPAssign>::iterator iit(ref);
        iit.insert_before(vtpa);
    }

    delete goodType;
    _deleteSet.insert(ref);
}

void FixReferencesVisitor::_fixBadRecordRef(Identifier *ref, Record *badType, Record *goodType)
{
    if (hif::objectIsInSensitivityList(ref)) {
        messageError("Unexpected ref in sensitivity.", ref, _sem);
    }
    DataDeclaration *decl = hif::semantics::getDeclaration(ref, _sem);
    messageAssert(decl != nullptr, "Declaration not found", ref, _sem);

    // Case reference is a general Value:
    RecordValue *rv          = new RecordValue();
    BList<Field>::iterator j = badType->fields.begin();
    for (BList<Field>::iterator i = goodType->fields.begin(); i != goodType->fields.end(); ++i, ++j) {
        RecordValueAlt *rva = new RecordValueAlt();
        rva->setName((*j)->getName());
        Cast *c = new Cast();
        c->setType(hif::copy((*j)->getType()));
        std::string n = _setNewFieldName(*i, decl);
        c->setValue(new Identifier(n));
        rva->setValue(c);
        rv->alts.push_back(rva);
    }
    ref->replace(rv);
    delete ref;
    delete goodType;
}

void FixReferencesVisitor::_fixBadRecordRef(FieldReference *ref, Record *badType, Record *goodType)
{
    Identifier *f = new Identifier(ref->getName());
    ref->replace(f);
    delete ref;

    _fixBadRecordRef(f, badType, goodType);
}

void FixReferencesVisitor::_fixBadRecordRef(ParameterAssign *ref, Record * /*badType*/, Record *goodType)
{
    messageAssert(ref->isInBList(), "Unexpected case", ref, _sem);
    Parameter *decl = hif::semantics::getDeclaration(ref, _sem);
    messageAssert(decl != nullptr, "Declaration not found", ref, _sem);

    for (BList<Field>::iterator i = goodType->fields.begin(); i != goodType->fields.end(); ++i) {
        ParameterAssign *pa = new ParameterAssign();
        std::string n       = _setNewFieldName(*i, decl);
        pa->setName(n);
        FieldReference *fr = new FieldReference();
        fr->setName((*i)->getName());
        fr->setPrefix(hif::copy(ref->getValue()));
        Cast *c = new Cast();
        c->setType(hif::copy((*i)->getType()));
        c->setValue(fr);
        pa->setValue(c);
        // Adding new assign:
        BList<ParameterAssign>::iterator iit(ref);
        iit.insert_before(pa);
    }

    delete goodType;
    _deleteSet.insert(ref);
}

void FixReferencesVisitor::_fixBadRecordRef(FunctionCall * /*ref*/, Record * /*badType*/, Record *goodType)
{
    delete goodType;
    messageError("Const function call with return type record are not supported yet.", nullptr, nullptr);
}

void FixReferencesVisitor::_fixBadRecordRef(Return * /*ref*/, Record * /*badType*/, Record *goodType)
{
    delete goodType;

    messageError("Return of const function of type record are not supported yet.", nullptr, nullptr);
}
void FixReferencesVisitor::_fixBadTimeRef(ValueTPAssign *ref, Time * /*badType*/, Record *goodType)
{
    messageAssert(ref->isInBList(), "Unexpected case", ref, _sem);
    ValueTP *decl = hif::semantics::getDeclaration(ref, _sem);
    messageAssert(decl != nullptr, "Declaration not found", ref, _sem);

    for (BList<Field>::iterator i = goodType->fields.begin(); i != goodType->fields.end(); ++i) {
        ValueTPAssign *vtpa = new ValueTPAssign();
        std::string n       = _setNewFieldName(*i, decl);
        vtpa->setName(n);
        FieldReference *fr = new FieldReference();
        fr->setName((*i)->getName());
        fr->setPrefix(hif::copy(ref->getValue()));
        Cast *c = new Cast();
        c->setType(hif::copy((*i)->getType()));
        c->setValue(fr);
        vtpa->setValue(c);
        // Adding new assign:
        BList<TPAssign>::iterator iit(ref);
        iit.insert_before(vtpa);
    }

    delete goodType;
    _deleteSet.insert(ref);
}

void FixReferencesVisitor::_fixBadTimeRef(Identifier *ref, Time *badType, Record *goodType)
{
    if (hif::objectIsInSensitivityList(ref)) {
        messageError("Unexpected ref in sensitivity.", ref, _sem);
    }

    DataDeclaration *decl = hif::semantics::getDeclaration(ref, _sem);
    messageAssert(decl != nullptr, "Declaration not found", ref, _sem);

    // Case reference is a general Value:
    // creating a time value
    RecordValue *rv = new RecordValue();

    RecordValueAlt *rva1 = new RecordValueAlt();
    rva1->setName("value");
    std::string n = _setNewFieldName(goodType->fields.front(), decl);
    rva1->setValue(new Identifier(n));
    rv->alts.push_back(rva1);

    RecordValueAlt *rva2 = new RecordValueAlt();
    rva2->setName("unit");
    std::string nn = _setNewFieldName(goodType->fields.back(), decl);
    rva2->setValue(new Identifier(nn));
    rv->alts.push_back(rva2);

    Cast *c = new Cast();
    c->setType(hif::copy(badType));
    c->setValue(rv);

    ref->replace(c);
    c->getType()->acceptVisitor(*this);
    delete ref;
    delete goodType;
}

void FixReferencesVisitor::_fixBadTimeRef(FieldReference * /*ref*/, Time * /*badType*/, Record *goodType)
{
    delete goodType;
    messageError("Field references of time type are not supported yet.", nullptr, nullptr);
}

void FixReferencesVisitor::_fixBadTimeRef(ParameterAssign * /*ref*/, Time * /*badType*/, Record *goodType)
{
    delete goodType;
    messageError("Parameter assigns of time type are not supported yet.", nullptr, nullptr);
}

void FixReferencesVisitor::_fixBadTimeRef(FunctionCall * /*ref*/, Time * /*badType*/, Record *goodType)
{
    delete goodType;
    messageError("Const function call with return type time are not supported yet.", nullptr, nullptr);
}

void FixReferencesVisitor::_fixBadTimeRef(Return * /*ref*/, Time * /*badType*/, Record *goodType)
{
    delete goodType;
    messageError("Return of const function of type time are not supported yet.", nullptr, nullptr);
}

// ///////////////////////////////////////////////////////////////////
// Visitor of objects that can be references
// ///////////////////////////////////////////////////////////////////
int FixReferencesVisitor::visitIdentifier(Identifier &o)
{
    GuideVisitor::visitIdentifier(o);

    Identifier::DeclarationType *decl = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(decl != nullptr, "Declaration not found", &o, _sem);

    ValueTP *vtp = dynamic_cast<ValueTP *>(decl);
    if (vtp != nullptr && !vtp->isCompileTimeConstant())
        return 0;

    _fixReference(&o, decl);
    return 0;
}

int FixReferencesVisitor::visitFieldReference(FieldReference &o)
{
    GuideVisitor::visitFieldReference(o);

    FieldReference::DeclarationType *decl = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(decl != nullptr, "Declaration not found", &o, _sem);

    ValueTP *vtp = dynamic_cast<ValueTP *>(decl);
    if (vtp != nullptr && !vtp->isCompileTimeConstant())
        return 0;

    _fixReference(&o, decl);
    return 0;
}

int FixReferencesVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    GuideVisitor::visitValueTPAssign(o);

    ValueTPAssign::DeclarationType *decl = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(decl != nullptr, "Declaration not found.", &o, _sem);

    if (!decl->isCompileTimeConstant())
        return 0;

    _fixReference(&o, decl);

    hif::manipulation::mapToNative(o.getValue(), _sem, _opt.checkSem);
    return 0;
}

int FixReferencesVisitor::visitParameterAssign(ParameterAssign &o)
{
    GuideVisitor::visitParameterAssign(o);

    ParameterAssign::DeclarationType *decl = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(decl != nullptr, "Declaration not found", &o, _sem);

    _fixReference(&o, decl);

    if (!_checkIsAlreadyTemplate(decl, false))
        return 0;
    hif::manipulation::mapToNative(o.getValue(), _sem, _opt.checkSem);
    return 0;
}

int FixReferencesVisitor::visitFunctionCall(FunctionCall &o)
{
    GuideVisitor::visitFunctionCall(o);

    FunctionCall::DeclarationType *decl = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(decl != nullptr, "Declaration not found", &o, _sem);

    _fixReference(&o, decl);
    return 0;
}

int FixReferencesVisitor::visitReturn(Return &o)
{
    GuideVisitor::visitReturn(o);

    if (o.getValue() == nullptr)
        return 0;

    Function *f = hif::getNearestParent<Function>(&o);
    messageAssert(f != nullptr, "Cannot found parent funzion", &o, _sem);

    _fixReference(&o, f);
    return 0;
}

// ////////////////////////
// Visits used to collect
// declarations.

int FixReferencesVisitor::visitConst(Const &o)
{
    GuideVisitor::visitConst(o);
    // Yet into map.
    return 0;
}

int FixReferencesVisitor::visitValueTP(ValueTP &o)
{
    GuideVisitor::visitValueTP(o);

    // If declaration type is good there is nothing to do.
    if (!o.isCompileTimeConstant() || _opt.checkSem->isTemplateAllowedType(o.getType()))
        return 0;

    // Storing syntactical bad type of declaration:
    Type *badType = o.getType();

    // Getting the base bad type of declaration:
    Type *baseBadType = hif::semantics::getBaseType(badType, false, _sem);

    // Getting the good declaration type.
    Type *goodType = _getGoodType(&o, baseBadType);
    delete goodType;

    return 0;
}

// ///////////////////////////////////////////////////////////////////
// FixDeclarationType method
// ///////////////////////////////////////////////////////////////////

void _renameEventualDefine(DataDeclaration *decl)
{
    Const *c = dynamic_cast<Const *>(decl);
    if (c == nullptr)
        return;
    messageAssert(definesMap.find(c) != definesMap.end(), "Not found decl in map", decl, nullptr);

    decl->setName(definesMap[c]);
}

void _checkConstexprFunction(Function *originalDecl, hif::semantics::ILanguageSemantics *sem)
{
    messageAssert(
        originalDecl->checkProperty(PROPERTY_CONSTEXPR), "function should have PROPERTY_CONSTEXPR", originalDecl, sem);

    // Sanity checks
    StateTable *stOriginal = originalDecl->getStateTable();
    if (stOriginal == nullptr) {
        LibraryDef *ld = dynamic_cast<LibraryDef *>(originalDecl->getParent());
        if (ld != nullptr && ld->isStandard())
            return;
        messageError("function without state table", originalDecl, sem);
    }

    messageAssert(stOriginal->states.size() == 1, "state table with more than one state", originalDecl, sem);
    for (BList<Declaration>::iterator i = stOriginal->declarations.begin(); i != stOriginal->declarations.end(); ++i) {
        if (dynamic_cast<Const *>(*i) != nullptr)
            continue;
        messageError("Found function which must be constexpr, but currently is not supported. (1)", originalDecl, sem);
    }

    hif::manipulation::SimplifyOptions opt;
    opt.simplify_constants  = true;
    opt.simplify_statements = true;
    hif::manipulation::simplify(originalDecl, sem, opt);

    const std::size_t ss = stOriginal->states.front()->actions.size();
    if (ss != 1) {
        messageError(
            "Found function which must be a constexpr, but currently is not supported. (2)", originalDecl, sem);
    }

    Action *a = stOriginal->states.front()->actions.back();
    if (dynamic_cast<Return *>(a) != nullptr) {
        // Ok supported.
    } else if (dynamic_cast<If *>(a) != nullptr) {
        If *ret = static_cast<If *>(a);
        When *w = hif::manipulation::transformIfToWhen(ret, sem);
        if (w == nullptr) {
            messageError(
                "Found function which must be a constexpr, but currently is not supported. (3)", originalDecl, sem);
        }
        Return *r = new Return();
        r->setValue(w);
        ret->replace(r);
        delete ret;
    } else {
        messageError(
            "Found function which must be a constexpr, but currently is not supported. (4)", originalDecl, sem);
    }
}

void _fixBadStringDecl(
    Declaration *decl,
    Type * /*badType*/,
    Type *goodType,
    hif::semantics::ILanguageSemantics *sem,
    hif::semantics::ILanguageSemantics *checkSem)
{
    // 1- Fixing declaration:
    delete hif::objectSetType(decl, goodType);
    DataDeclaration *dataDecl = dynamic_cast<DataDeclaration *>(decl);
    if (dataDecl == nullptr)
        return;

    _renameEventualDefine(dataDecl);

    // 2- Fixing initial value:
    StringValue *initVal = dynamic_cast<StringValue *>(dataDecl->getValue());
    if (initVal != nullptr) {
        // Initial value must be a text. In this case add a new
        // enum value entry into global enums.
        std::string n       = _addStringEnumEntry(dataDecl, initVal->getValue(), sem);
        // Set initial value with an identifier to created enum value.
        Identifier *enumRef = new Identifier(n);
        delete dataDecl->setValue(enumRef);
    }

    if (dataDecl->getValue() != nullptr) {
        hif::manipulation::mapToNative(dataDecl->getValue(), sem, checkSem);
    }
}
void _fixBadRecordDecl(
    Declaration *deco,
    Type * /*badType*/,
    Record *goodType,
    hif::semantics::ILanguageSemantics *sem,
    hif::semantics::ILanguageSemantics *checkSem)
{
    messageAssert(goodType != nullptr, "Unexpected nullptr good type", nullptr, sem);
    DataDeclaration *decl = dynamic_cast<DataDeclaration *>(deco);
    // Functions return records are not supported yet.
    messageAssert(decl != nullptr, "Unexpected declaration", deco, sem);

    // Adding new declaration:
    messageAssert(decl->isInBList(), "Unexpected declaration (2)", deco, sem);

    // 1-2- Fixing declaration type and initial values:
    for (BList<Field>::iterator i = goodType->fields.begin(); i != goodType->fields.end(); ++i) {
        Field *f = (*i);
        // Transforming fields to VTP or Defines according with
        // original decl object kind.
        hif::CopyOptions opt;
        opt.copyChildObjects = false;
        DataDeclaration *nf  = hif::copy(decl, opt);
        nf->setType(hif::copy(f->getType()));
        TypedObject *propValue = f->getProperty(PROPERTY_NEW_NAME);
        messageAssert(propValue != nullptr, "Unexpected empty property", f, sem);
        nf->setName(static_cast<Identifier *>(propValue)->getName());
        BList<Declaration>::iterator it(decl);
        it.insert_before(nf);
        // Fixing initial value:
        if (decl->getValue() != nullptr) {
            FieldReference *fr = new FieldReference();
            fr->setName(f->getName());
            fr->setPrefix(hif::copy(decl->getValue()));
            Cast *c = new Cast();
            c->setType(hif::copy(nf->getType()));
            c->setValue(fr);
            nf->setValue(c);

            hif::manipulation::mapToNative(c, sem, checkSem);
        }
    }

    // Removing old decl:
    BList<Declaration>::iterator it(decl);
    it.erase();
    delete goodType;
}

void _fixBadTimeDecl(
    Declaration *deco,
    Type * /*badType*/,
    Record *goodType,
    hif::semantics::ILanguageSemantics *sem,
    hif::semantics::ILanguageSemantics * /*checkSem*/)
{
    messageAssert(goodType != nullptr, "Unexpected nullptr good type", nullptr, sem);
    DataDeclaration *decl = dynamic_cast<DataDeclaration *>(deco);
    // Functions return records are not supported yet.
    messageAssert(decl != nullptr, "Unexpected declaration", deco, sem);

    // Adding new declaration:
    messageAssert(decl->isInBList(), "Unexpected declaration (2)", deco, sem);

    TimeValue *tv = dynamic_cast<TimeValue *>(decl->getValue());
    messageAssert(
        decl->getValue() == nullptr || tv != nullptr, "Only time values are supported as initial values at the moment.",
        decl, sem);

    // 1-2- Fixing declaration type and initial values:
    {
        Field *f = goodType->fields.front();
        // Transforming fields to VTP or Defines according with
        // original decl object kind.
        hif::CopyOptions opt;
        opt.copyChildObjects = false;
        DataDeclaration *nf  = hif::copy(decl, opt);
        nf->setType(hif::copy(f->getType()));
        TypedObject *propValue = f->getProperty(PROPERTY_NEW_NAME);
        messageAssert(propValue != nullptr, "Unexpected empty property", f, sem);
        nf->setName(static_cast<Identifier *>(propValue)->getName());
        BList<Declaration>::iterator it(decl);
        it.insert_before(nf);
        // Fixing initial value:
        if (tv != nullptr) {
            IntValue *v = new IntValue();
            v->setValue(static_cast<long long>(tv->getValue()));
            v->setType(hif::copy(nf->getType()));
            nf->setValue(v);
        }
    }

    {
        Field *f = goodType->fields.back();
        // Transforming fields to VTP or Defines according with
        // original decl object kind.
        hif::CopyOptions opt;
        opt.copyChildObjects = false;
        DataDeclaration *nf  = hif::copy(decl, opt);
        nf->setType(hif::copy(f->getType()));
        TypedObject *propValue = f->getProperty(PROPERTY_NEW_NAME);
        messageAssert(propValue != nullptr, "Unexpected empty property", f, sem);
        nf->setName(static_cast<Identifier *>(propValue)->getName());
        BList<Declaration>::iterator it(decl);
        it.insert_before(nf);
        // Fixing initial value:
        if (tv != nullptr) {
            hif::HifFactory ff(sem);
            std::string nn;
            switch (tv->getUnit()) {
            case hif::TimeValue::time_fs:
                nn = "hif_systemc_SC_FS";
                break;
            case hif::TimeValue::time_ps:
                nn = "hif_systemc_SC_PS";
                break;
            case hif::TimeValue::time_ns:
                nn = "hif_systemc_SC_NS";
                break;
            case hif::TimeValue::time_us:
                nn = "hif_systemc_SC_US";
                break;
            case hif::TimeValue::time_ms:
                nn = "hif_systemc_SC_MS";
                break;
            case hif::TimeValue::time_sec:
                nn = "hif_systemc_SC_SEC";
                break;
            case hif::TimeValue::time_min:
            case hif::TimeValue::time_hr:
            default: // fall into
                messageError("Unsupported time unit in systemc", tv, sem);
            }
            FieldReference *fr = ff.fieldRef(ff.libraryInstance("hif_systemc_sc_core", false, true), nn.c_str());
            nf->setValue(fr);

            Library *scCore = new Library();
            scCore->setName("hif_systemc_sc_core");
            scCore->setStandard(false);
            scCore->setSystem(true);
            hif::manipulation::AddUniqueObjectOptions addOpt;
            addOpt.equalsOptions.checkOnlyNames = true;
            addOpt.deleteIfNotAdded             = true;
            hif::manipulation::addUniqueObject(scCore, deco, addOpt);
        }
    }

    // Removing old decl:
    BList<Declaration>::iterator it(decl);
    it.erase();
    delete goodType;
}

void _fixBadGeneralDecl(
    Declaration *decl,
    Type * /*badType*/,
    Type *goodType,
    hif::semantics::ILanguageSemantics *sem,
    hif::semantics::ILanguageSemantics *checkSem)
{
    // 1- Fixing declaration:
    delete hif::objectSetType(decl, goodType);
    DataDeclaration *dataDecl = dynamic_cast<DataDeclaration *>(decl);
    if (dataDecl == nullptr)
        return;

    _renameEventualDefine(dataDecl);

    // 2- Fixing initial value:
    if (dataDecl->getValue() != nullptr) {
        // Adding a cast of initial value to goodType.
        Cast *c = new Cast();
        c->setType(hif::copy(goodType));
        c->setValue(dataDecl->getValue());
        dataDecl->setValue(c);
    }

    if (dataDecl->getValue() != nullptr) {
        hif::manipulation::mapToNative(dataDecl->getValue(), sem, checkSem);
    }
}

void fixDeclarationType(hif::semantics::ILanguageSemantics *sem, const FixTemplateOptions &opt)
{
    for (GoodTypeMap::iterator i = goodTypeMap.begin(); i != goodTypeMap.end(); ++i) {
        Declaration *decl = i->first;
        Function *func    = dynamic_cast<Function *>(decl);
        Type *declType    = hif::objectGetType(decl);
        Type *badType     = declType;
        Type *baseBadType = hif::semantics::getBaseType(declType, false, sem);
        Type *goodType    = i->second;

        // Calling right fix checking badType.
        if (dynamic_cast<String *>(baseBadType) != nullptr) {
            _fixBadStringDecl(decl, badType, goodType, sem, opt.checkSem);
        } else if (dynamic_cast<Record *>(baseBadType) != nullptr) {
            _fixBadRecordDecl(
                decl, static_cast<Record *>(baseBadType), static_cast<Record *>(goodType), sem, opt.checkSem);
        } else if (dynamic_cast<Time *>(baseBadType) != nullptr) {
            _fixBadTimeDecl(decl, static_cast<Time *>(baseBadType), static_cast<Record *>(goodType), sem, opt.checkSem);
        } else if (goodType != nullptr) {
            _fixBadGeneralDecl(decl, badType, goodType, sem, opt.checkSem);
        }

        if (func != nullptr)
            _checkConstexprFunction(func, sem);
    }
}
// ///////////////////////////////////////////////////////////////////
// DefaultValueVisitor
// ///////////////////////////////////////////////////////////////////

/// @brief This visitor is used to fix template parameters without default
/// value, due to previous manipulations. The situation is like:
/// @code
///     template< par0, par1, par2=x, par3, par4=y, par5>
/// @endcode
/// thus an error is raised since we are mixing tps with and without default
/// value. We are going to fix it like:
/// @code
///     template< par0, par1, par2=x, par3=default, par4=y, par5=default>
/// @endcode
/// setting the default value of each template parameter that needs it, basing
/// on its type.
class DefaultValueVisitor : public hif::GuideVisitor
{
public:
    /// @brief Default constructor.
    DefaultValueVisitor(hif::semantics::ILanguageSemantics *sem, const FixTemplateOptions &opt);

    /// @brief Destructor.
    virtual ~DefaultValueVisitor();

    /// @name Visitor of objects that own template parameters.
    /// @{

    virtual int visitFunction(hif::Function &o);
    virtual int visitProcedure(hif::Procedure &o);
    virtual int visitTypeDef(hif::TypeDef &o);
    virtual int visitView(hif::View &o);

    /// @}

    virtual int visitCast(hif::Cast &o);

private:
    hif::semantics::ILanguageSemantics *_sem;

    const FixTemplateOptions &_opt;

    DefaultValueVisitor(const DefaultValueVisitor &);
    DefaultValueVisitor &operator=(const DefaultValueVisitor &);

    int _fixDefaultValues(hif::BList<hif::Declaration> &templates);
};
DefaultValueVisitor::DefaultValueVisitor(hif::semantics::ILanguageSemantics *sem, const FixTemplateOptions &opt)
    : _sem(sem)
    , _opt(opt)
{
    // ntd
}
DefaultValueVisitor::~DefaultValueVisitor()
{
    // ntd
}
int DefaultValueVisitor::visitFunction(hif::Function &o)
{
    GuideVisitor::visitFunction(o);
    _fixDefaultValues(o.templateParameters);
    return 0;
}
int DefaultValueVisitor::visitProcedure(hif::Procedure &o)
{
    GuideVisitor::visitProcedure(o);
    _fixDefaultValues(o.templateParameters);
    return 0;
}
int DefaultValueVisitor::visitTypeDef(hif::TypeDef &o)
{
    GuideVisitor::visitTypeDef(o);
    _fixDefaultValues(o.templateParameters);
    return 0;
}
int DefaultValueVisitor::visitView(hif::View &o)
{
    GuideVisitor::visitView(o);
    _fixDefaultValues(o.templateParameters);
    return 0;
}
int DefaultValueVisitor::visitCast(hif::Cast &o)
{
    GuideVisitor::visitCast(o);

#if 0
    Time * tc = dynamic_cast <Time *>(o.getType());
    if (tc == nullptr) return 0;

    RecordValue * rv = dynamic_cast <RecordValue *>(o.getOperator());
    messageAssert(rv != nullptr, "Unexpected cast to time", &o, _sem);

    hif::HifFactory f(_sem);

    FunctionCall * fc = f.functionCall(
                hif::NameTable::getInstance()->hifCastToTime(),
                f.libraryInstance(hif::NameTable::getInstance()->hifUtility(), false , true),
                f.noTemplateArguments(),
                (
                    f.parameterArgument("value", rv->alts.front()->setValue(nullptr)),
                    f.parameterArgument("unit", rv->alts.back()->setValue(nullptr))
                    )
                );

    o.replace(fc);
    delete &o;
#endif

    return 0;
}
int DefaultValueVisitor::_fixDefaultValues(hif::BList<hif::Declaration> &templates)
{
    bool addMissing = false;
    for (hif::BList<hif::Declaration>::iterator it(templates.begin()); it != templates.end(); ++it) {
        ValueTP *vtp = dynamic_cast<ValueTP *>(*it);
        TypeTP *ttp  = dynamic_cast<TypeTP *>(*it);

        if (vtp != nullptr) {
            if (!vtp->isCompileTimeConstant()) {
                if (vtp->getValue() == nullptr)
                    vtp->setValue(_sem->getTypeDefaultValue(vtp->getType(), nullptr));
                continue;
            }

            if (vtp->getValue() == nullptr && !addMissing)
                continue;
            if (vtp->getValue() != nullptr) {
                addMissing = true;
            } else {
                Value *v = _sem->getTypeDefaultValue(vtp->getType(), nullptr);
                vtp->setValue(v);
            }
        } else if (ttp != nullptr) {
            if (ttp->getType() == nullptr && !addMissing)
                continue;
            if (ttp->getType() != nullptr) {
                addMissing = true;
            } else {
                Bool *t = new Bool(); // arbitrary
                ttp->setType(t);
            }
        } else {
            messageError("Unexpected object", *it, _sem);
        }
    }

    return 0;
}

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

void _fixAssignments(System *o, hif::semantics::ILanguageSemantics *sem)
{
    // Fix the assign statements and the data declaration initial value
    // within their declaration type.
    // This is required since the moving of a constant as define may
    // break typing of aforementioned objects. This can happen
    // for example when the type of an lhs of an assignment contains symbols
    // which declaration is moved as define in global scope. And in this case
    // moved declaration became visible from all scopes and therefore
    // simplify() will not solve constant symbols anymore.
    // This visitor if needed add a cast to the rhs/initial value to preserve typing.
    // Example:
    // namespace NS
    // {
    //    module M1
    //    {
    //        const int c = 5;
    //    }
    //
    //    typedef sc_lv<M1::c> TD;
    // }
    //
    // module M2
    // {
    //    NS::TD var = expr; // c is not visible here, so getSemanticsType will simplify it.
    // }
    //
    // Ref. design: mephisto_core + a2t hif2sc
    hif::manipulation::FixAssignmentOptions fopt;
    fopt.fixAssigns               = true;
    fopt.fixDataDeclarationsValue = true;
    hif::manipulation::fixAssignments(o, sem, fopt);
}
// ///////////////////////////////////////////////////////////////////
// FixVTPInitialValue
// ///////////////////////////////////////////////////////////////////
class FixVTPInitialValue : public GuideVisitor
{
public:
    FixVTPInitialValue(System *sys, hif::semantics::ILanguageSemantics *sem);
    ~FixVTPInitialValue();

private:
    FixVTPInitialValue(const FixVTPInitialValue &);
    FixVTPInitialValue &operator=(const FixVTPInitialValue &);

    hif::semantics::ILanguageSemantics *_sem;
    bool _isInitialValue;
    hif::semantics::ReferencesMap _refMap;

    int visitIdentifier(Identifier &o);
    int visitValueTP(ValueTP &o);
};

FixVTPInitialValue::FixVTPInitialValue(System *sys, hif::semantics::ILanguageSemantics *sem)
    : _sem(sem)
    , _isInitialValue(false)
    , _refMap()
{
    hif::semantics::getAllReferences(_refMap, _sem, sys);
}

FixVTPInitialValue::~FixVTPInitialValue()
{
    // ntd
}

int FixVTPInitialValue::visitIdentifier(Identifier &o)
{
    GuideVisitor::visitIdentifier(o);

    // Checks
    if (!_isInitialValue)
        return 0;
    Identifier::DeclarationType *decl = hif::semantics::getDeclaration(&o, _sem);
    messageAssert(decl != nullptr, "Declaration not found", &o, _sem);
    ValueTP *vtp = dynamic_cast<ValueTP *>(decl);
    if (vtp == nullptr || vtp->isCompileTimeConstant())
        return 0;

    // check of refs:
    for (hif::semantics::ReferencesSet::iterator i = _refMap[vtp].begin(); i != _refMap[vtp].end(); ++i) {
        ValueTPAssign *vtpa = dynamic_cast<ValueTPAssign *>(*i);
        if (vtpa == nullptr)
            continue;
        messageAssert(vtpa->isInBList(), "Unexpected ValueTPAssign", vtpa, _sem);
        Object *ret = vtpa->getBList()->findByName(vtp->getName());
        if (ret != nullptr)
            continue;

        // Actual fix is very complex, since should require to take in account
        // initial values, with multiple refs to different vtpa.
        // So just rise a warning.
        messageWarning(
            "Found an implicitly assigned value template which depends on "
            "other templates, whose initial value will be simplified."
            " This could lead to a non-equivalent design description.",
            vtp, _sem);
        break;
    }
    // Fix of initial value
    messageAssert(vtp->getValue() != nullptr, "Initial value not found.", vtp, _sem);
    Value *v = hif::copy(vtp->getValue());
    Cast *c  = new Cast();
    Type *t  = hif::semantics::getSemanticType(&o, _sem);
    messageAssert(t != nullptr, "Cannot type description", &o, _sem);
    c->setType(hif::copy(t));
    c->setValue(v);
    o.replace(c);
    _refMap[vtp].erase(&o);
    delete &o;
    c->acceptVisitor(*this);

    raiseUniqueWarning("Simplified initial values of value template parameters,"
                       " which depend on other template values.");
    return 0;
}

int FixVTPInitialValue::visitValueTP(ValueTP &o)
{
    bool restore    = _isInitialValue;
    _isInitialValue = !o.isCompileTimeConstant();
    GuideVisitor::visitValueTP(o);
    _isInitialValue = restore;

    return 0;
}
} // namespace

// ///////////////////////////////////////////////////////////////////
// FixTemplateOptions
// ///////////////////////////////////////////////////////////////////
FixTemplateOptions::FixTemplateOptions()
    : useHdtLib(false)
    , checkSem(nullptr)
    , setConstExpr(false)
    , fixStandardDeclarations(false)
{
    // ntd
}

FixTemplateOptions::~FixTemplateOptions()
{
    // ntd
}

FixTemplateOptions::FixTemplateOptions(const FixTemplateOptions &o)
    : useHdtLib(o.useHdtLib)
    , checkSem(o.checkSem)
    , setConstExpr(o.setConstExpr)
    , fixStandardDeclarations(o.fixStandardDeclarations)
{
    // ntd
}

FixTemplateOptions &FixTemplateOptions::operator=(FixTemplateOptions o)
{
    swap(o);
    return *this;
}

void FixTemplateOptions::swap(FixTemplateOptions &o)
{
    std::swap(useHdtLib, o.useHdtLib);
    std::swap(checkSem, o.checkSem);
    std::swap(setConstExpr, o.setConstExpr);
    std::swap(fixStandardDeclarations, o.fixStandardDeclarations);
}

// ////////////////////////////////////////////////////////////////////////
// Entry point.
// ////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
//#define DEBUG_REFINE
#endif

void fixTemplateParameters(System *o, hif::semantics::ILanguageSemantics *sem, const FixTemplateOptions &opt)
{
    hif::application_utils::initializeLogHeader("Manipulation", "fixTemplateParameters");

    FixTemplateOptions options(opt);
    if (options.checkSem == nullptr)
        options.checkSem = sem;

    /*
    //////////////////////////////////////////////
    //                  ____________________    //
    //                 /                    \   //
    //      |\___/|    | Abandon all hope,  |   //
    //     /       \   | ye who enter here. |   //
    //    |     \__/|  \_  _________________/   //
    //    ||\  <.><.>    |/                     //
    //    | _     > )                           //
    //     \   /----                            //
    //      |   -\/                             //
    //     /     \                              //
    //////////////////////////////////////////////
    */

    hif::semantics::typeTree(o, sem);
    hif::semantics::updateDeclarations(o, sem);

#if 0
    // 0 - Check for eventual time template parameters
    {
        TypeConverterVisitor tcv(sem);
        o->acceptVisitor(tcv);
    }
#endif

    // 1 - Preserve compile time constant
    PreserveCTCVisitor pctc;
    o->acceptVisitor(pctc);

    // 2 - Moving declarations to templates
    {
        MoveVisitor mVis(sem, options);
        o->acceptVisitor(mVis);
    }

#ifdef DEBUG_REFINE
    messageInfo("Fix_templates_STEP0");
    hif::writeFile("Fix_templates_STEP0", o, false);
#endif

    hif::semantics::UpdateDeclarationOptions dopt;
    dopt.forceRefresh = true;
    dopt.error        = true;

    // Flushing of instace cache is necessary since moved object may have
    // declarations in cache (in case of typerefs).
    // Ref. design vhdl/gaisler/can_oc.
    hif::semantics::resetTypes(o);
    hif::manipulation::flushInstanceCache();
    hif::semantics::flushTypeCacheEntries();
    hif::semantics::updateDeclarations(o, sem, dopt);

#ifdef DEBUG_REFINE
    messageInfo("Fix_templates_STEP1");
    hif::writeFile("Fix_templates_STEP1", o, false);
#endif

    // 3 - Fixing references type
    {
        FixReferencesVisitor rVis(sem, options);
        o->acceptVisitor(rVis);
    }

#ifdef DEBUG_REFINE
    messageInfo("Fix_templates_STEP2");
    hif::writeFile("Fix_templates_STEP2", o, false);
#endif

    // 4 - Fixing declaration type
    fixDeclarationType(sem, options);

#ifdef DEBUG_REFINE
    messageInfo("Fix_templates_STEP3");
    hif::writeFile("Fix_templates_STEP3", o, false);
#endif

    // Freeing some memory.
    goodTypeMap.clear();

    // Cache is invalid due to new template args, decls are invalid due
    // to moves.
    hif::semantics::UpdateDeclarationOptions upOpt;
    upOpt.forceRefresh = true;
    hif::manipulation::flushInstanceCache();
    hif::semantics::flushTypeCacheEntries();
    hif::semantics::resetTypes(o);
    hif::semantics::updateDeclarations(o, sem, upOpt);

    // 5 - Fixing missing default values.
    // Added also fix of time values
    DefaultValueVisitor dvVis(sem, options);
    o->acceptVisitor(dvVis);

    // 6 - Fixing assignment between declaration and their initial value and
    // assign objects.
    _fixAssignments(o, sem);

    // 7 - Fixing initial values of VTP's which are moved to consts.
    FixVTPInitialValue vtpVis(o, sem);
    o->acceptVisitor(vtpVis);

#ifdef DEBUG_REFINE
    messageInfo("Fix_templates_STEP4");
    hif::writeFile("Fix_templates_STEP4", o, false);
#endif

    // 8 - Simplify tree since introduces cast and mapToNative could make the
    // tree simplifiable.
    hif::manipulation::simplify(o, sem);

#ifdef DEBUG_REFINE
    messageInfo("Fix_templates_STEP5");
    hif::writeFile("Fix_templates_STEP5", o, false);
#endif

    hif::application_utils::restoreLogHeader();
}

} // namespace manipulation
} // namespace hif
