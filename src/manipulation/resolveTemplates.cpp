/// @file resolveTemplates.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/resolveTemplates.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"
#include "hif/trash.hpp"

//#define ENABLE_EXPERIMENTAL_FIX

typedef std::set<hif::Object *> References;
typedef std::set<hif::Declaration *> Declarations;
typedef std::map<hif::Declaration *, References> DeclarationsReferences;
typedef std::map<hif::Declaration *, hif::Declaration *> InstanceToCopyMap;
typedef std::list<DeclarationsReferences *> ReferencesMapList;

namespace hif
{
namespace manipulation
{

namespace /* anon */
{
// /////////////////////////////////////////////////////////////////////////////
// General support methods
// /////////////////////////////////////////////////////////////////////////////

bool _fixIndex(Value *index, hif::semantics::ILanguageSemantics *sem)
{
    if (index == nullptr)
        return false;
    Type *indexType = hif::semantics::getSemanticType(index, sem);
    if (indexType == nullptr)
        return false;
    Type *allowed = sem->isTypeAllowedAsBound(indexType);
    if (allowed == nullptr)
        return false;

    Cast *c = new Cast();
    c->setType(allowed);
    index->replace(c);
    c->setValue(index);
    hif::manipulation::assureSyntacticType(index, sem);

    return true;
}

// /////////////////////////////////////////////////////////////////////////////
// IndexVisitor
// /////////////////////////////////////////////////////////////////////////////
class IndexVisitor : public GuideVisitor
{
public:
    IndexVisitor(hif::semantics::ILanguageSemantics *sem);
    ~IndexVisitor();

    bool isFixed();

    int visitAggregateAlt(AggregateAlt &o);
    int visitAssign(Assign &o);
    int visitConst(Const &o);
    int visitRange(Range &o);
    int visitMember(Member &o);
    int visitPortAssign(PortAssign &o);
    int visitVariable(Variable &o);

private:
    hif::semantics::ILanguageSemantics *_sem;
    bool _isFixed;
    HifFactory _factory;

    IndexVisitor(const IndexVisitor &);
    IndexVisitor &operator=(const IndexVisitor &);
};

IndexVisitor::IndexVisitor(semantics::ILanguageSemantics *sem)
    : _sem(sem)
    , _isFixed(false)
    , _factory(sem)
{
    // ntd
}

IndexVisitor::~IndexVisitor()
{
    // ntd
}

bool IndexVisitor::isFixed() { return _isFixed; }

int IndexVisitor::visitAggregateAlt(AggregateAlt &o)
{
    GuideVisitor::visitAggregateAlt(o);

    for (BList<Value>::iterator it(o.indices.begin()); it != o.indices.end(); ++it) {
        if (dynamic_cast<Range *>(*it) != nullptr)
            continue;
        _isFixed |= _fixIndex(*it, _sem);
    }

    return 0;
}

int IndexVisitor::visitAssign(Assign &o)
{
    GuideVisitor::visitAssign(o);
    Type *left  = hif::semantics::getSemanticType(o.getLeftHandSide(), _sem);
    Type *right = hif::semantics::getSemanticType(o.getRightHandSide(), _sem);
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo info = _sem->getExprType(left, right, hif::op_assign, &o);
    if (info.returnedType != nullptr)
        return 0;
    o.setRightHandSide(_factory.cast(hif::copy(left), o.getRightHandSide()));
    _isFixed = true;
    return 0;
}

int IndexVisitor::visitConst(Const &o)
{
    // ref design: can_oc + ddt
    GuideVisitor::visitConst(o);
    Type *left                                                  = o.getType();
    Type *right                                                 = hif::semantics::getSemanticType(o.getValue(), _sem);
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo info = _sem->getExprType(left, right, hif::op_assign, &o);
    if (info.returnedType != nullptr)
        return 0;
    o.setValue(_factory.cast(hif::copy(left), o.getValue()));
    _isFixed = true;
    return 0;
}

int IndexVisitor::visitRange(Range &o)
{
    GuideVisitor::visitRange(o);
    _isFixed |= _fixIndex(o.getLeftBound(), _sem);
    _isFixed |= _fixIndex(o.getRightBound(), _sem);
    return 0;
}

int IndexVisitor::visitMember(Member &o)
{
    GuideVisitor::visitMember(o);
    _isFixed |= _fixIndex(o.getIndex(), _sem);
    return 0;
}

int IndexVisitor::visitPortAssign(PortAssign &o)
{
    GuideVisitor::visitPortAssign(o);
    Type *paType    = hif::semantics::getSemanticType(&o);
    Type *valueType = hif::semantics::getSemanticType(o.getValue());
    if (paType == nullptr || valueType == nullptr)
        return 0;
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo info =
        _sem->getExprType(paType, valueType, hif::op_bind, &o);
    if (info.returnedType != nullptr)
        return 0;
    o.setValue(_factory.cast(hif::copy(paType), o.getValue()));
    return 0;
}

int IndexVisitor::visitVariable(Variable &o)
{
    GuideVisitor::visitVariable(o);
    Type *left                                                  = o.getType();
    Type *right                                                 = hif::semantics::getSemanticType(o.getValue(), _sem);
    hif::semantics::ILanguageSemantics::ExpressionTypeInfo info = _sem->getExprType(left, right, hif::op_assign, &o);
    if (info.returnedType != nullptr)
        return 0;
    o.setValue(_factory.cast(hif::copy(left), o.getValue()));
    _isFixed = true;
    return 0;
}

// /////////////////////////////////////////////////////////////////////////////
// ResolveTemplates
// /////////////////////////////////////////////////////////////////////////////
class ResolveTemplates : public HifVisitor
{
public:
    typedef std::set<DesignUnit *> DesignUnits;
    /// @brief Constructor.
    ResolveTemplates(
        hif::System *sysObj,
        hif::semantics::ILanguageSemantics *sem,
        hif::View *top,
        const ResolveTempalteOptions &opt);

    /// @brief Destructor.
    virtual ~ResolveTemplates();

    virtual int visitFunction(Function &o);
    virtual int visitProcedure(Procedure &o);
    virtual int visitTypeDef(TypeDef &o);
    virtual int visitView(View &o);

    /// @brief Recursively calls this visitor on the declarations contained in
    /// \p o.
    void fixDeclarationsInMap(Declaration *o);

    /// @brief Return the class field _isFixed value.
    bool isFixed();

    const Declarations &getVisitedDeclarations();

private:
    /// @brief Pointer to the Hif System object.
    hif::System *_system;

    /// @brief The reference semantics.
    hif::semantics::ILanguageSemantics *_sem;

    hif::HifFactory _factory;

    /// @brief Pointer to the top level Hif View.
    hif::View *_top;

    /// @brief Options.
    const ResolveTempalteOptions &_opt;

    /// @brief Options
    SimplifyOptions _sOpt;

    /// @brief Map containing the instantiated declaration and its copy.
    InstanceToCopyMap _instanceToCopyMap;

    /// @brief Map containing the current list of declarations with their
    /// references.
    ReferencesMapList _refsMapList;

    Declarations _instantiatedDeclarations;
    DesignUnits _designUnits;
    Declarations _visitedDeclarations;

    /// @brief If true at least a fix has been computed.
    bool _isFixed;

    /// @brief Handle the ProcedureCall references.
    void _handleProcedureCall(ProcedureCall::DeclarationType *decl, hif::ProcedureCall *pcall);

    /// @brief Set the content of the copy of the declaration returned by the
    /// instantiate function call.
    template <typename T> T *_setDeclCopy(T *d, T *decl);

    /// @brief Simplify given tree by setting context option to resolve
    /// symbols no more visible.
    void _simplifyByContext(Object *o);

    /// Checks wether it is a CTC template list.
    bool _isCTC(const BList<Declaration> &templates) const;

    hif::Trash _trash;

#ifdef ENABLE_EXPERIMENTAL_FIX
    /// @brief
    void _fixUndeclaredSymbols(hif::SubProgram *sp, hif::FunctionCall *fcall);
#endif

    /// @brief Disabled copy constructor.
    ResolveTemplates(const ResolveTemplates &s);

    /// @brief Disabled assignment operator.
    ResolveTemplates &operator=(const ResolveTemplates &);
};
ResolveTemplates::ResolveTemplates(
    hif::System *sysObj,
    hif::semantics::ILanguageSemantics *sem,
    hif::View *top,
    const ResolveTempalteOptions &opt)
    : _system(sysObj)
    , _sem(sem)
    , _factory(sem)
    , _top(top)
    , _opt(opt)
    , _sOpt()
    , _instanceToCopyMap()
    , _refsMapList()
    , _instantiatedDeclarations()
    , _designUnits()
    , _visitedDeclarations()
    , _isFixed(false)
    , _trash()
{
    _sOpt.simplify_template_parameters = true;
    _sOpt.simplify_statements          = true;
}

ResolveTemplates::~ResolveTemplates()
{
    // 1- Check if declarations can be removed:
    hif::semantics::ReferencesMap allRefs;
    hif::semantics::GetReferencesOptions getOpts;
    getOpts.include_unreferenced      = true;
    getOpts.skip_standard_declarations = true;
    hif::semantics::getAllReferences(allRefs, _sem, _system);
    for (Declarations::iterator it(_instantiatedDeclarations.begin()); it != _instantiatedDeclarations.end(); ++it) {
        Declaration *decl                          = *it;
        hif::semantics::ReferencesMap::iterator jt = allRefs.find(decl);
        if (jt == allRefs.end())
            continue;
        hif::semantics::ReferencesSet &refs = jt->second;
        if (!refs.empty())
            continue;
        _trash.insert(decl);
    }

    // 2- Remove decls that will be deleted
    Declarations tmp(_visitedDeclarations);
    _visitedDeclarations.clear();
    for (Declarations::iterator it(tmp.begin()); it != tmp.end(); ++it) {
        Declaration *decl = *it;
        if (_trash.contains(decl))
            continue;
        _visitedDeclarations.insert(decl);
    }

    // 3- Clear trash
    _trash.clear();

    // 4- Remove empty design units:
    for (DesignUnits::iterator it = _designUnits.begin(); it != _designUnits.end(); ++it) {
        DesignUnit *du = *it;
        if (du->views.empty()) {
            du->replace(nullptr);
            delete du;
        }
    }
}

int ResolveTemplates::visitFunction(Function &o)
{
    if (hif::declarationIsPartOfStandard(&o))
        return 0;
    if (o.templateParameters.empty()) {
        fixDeclarationsInMap(&o);
        return 0;
    }

    _visitedDeclarations.insert(&o);
    _isFixed = true;

    DeclarationsReferences dRef = *_refsMapList.back();
    References &refs            = dRef[&o];

    if (_opt.removeInstantiatedSubPrograms)
        _instantiatedDeclarations.insert(&o);

    // No references
    if (refs.empty()) {
        // Ref design: vhdl/custom/test_compile_time_tempalte + ddt
        if (_opt.removeUnreferenced)
            _trash.insert(&o);
        return 0;
    }

    for (References::iterator it = refs.begin(); it != refs.end(); ++it) {
        FunctionCall *fcall  = dynamic_cast<FunctionCall *>(*it);
        ProcedureCall *pcall = dynamic_cast<ProcedureCall *>(*it);
        if (fcall != nullptr) {
            FunctionCall::DeclarationType *fDecl = instantiate(fcall, _sem);
            messageAssert(fDecl != nullptr, "Instantiate failed", fcall, _sem);

            FunctionCall::DeclarationType *fDeclCopy = _setDeclCopy(&o, fDecl);
            fcall->setName(fDeclCopy->getName());
            _trash.insert(fcall->templateParameterAssigns);
#ifdef ENABLE_EXPERIMENTAL_FIX
            _fixUndeclaredSymbols(fDeclCopy, fcall);
#endif
            hif::semantics::resetDeclarations(fcall);
            hif::manipulation::simplify(fDeclCopy, _sem, _sOpt);

        } else if (pcall != nullptr) {
            _handleProcedureCall(&o, pcall);
        } else {
            messageError("Unexpected case", *it, _sem);
        }
    }
    return 0;
}

int ResolveTemplates::visitProcedure(Procedure &o)
{
    if (hif::declarationIsPartOfStandard(&o))
        return 0;
    if (o.templateParameters.empty()) {
        fixDeclarationsInMap(&o);
        return 0;
    }

    _visitedDeclarations.insert(&o);
    _isFixed = true;

    DeclarationsReferences dRef = *_refsMapList.back();
    References &refs            = dRef[&o];

    if (_opt.removeInstantiatedSubPrograms)
        _instantiatedDeclarations.insert(&o);

    // No references
    if (refs.empty()) {
        if (_opt.removeUnreferenced)
            _trash.insert(&o);
        return 0;
    }

    for (References::iterator it = refs.begin(); it != refs.end(); ++it) {
        ProcedureCall *pcall = dynamic_cast<ProcedureCall *>(*it);
        if (pcall != nullptr) {
            _handleProcedureCall(&o, pcall);
        } else {
            messageError("Unexpected case", *it, _sem);
        }
    }
    return 0;
}

int ResolveTemplates::visitTypeDef(TypeDef &o)
{
    if (hif::declarationIsPartOfStandard(&o))
        return 0;
    if (o.templateParameters.empty()) {
        fixDeclarationsInMap(&o);
        return 0;
    }

    _visitedDeclarations.insert(&o);
    _isFixed = true;

    DeclarationsReferences dRef = *_refsMapList.back();
    References &refs            = dRef[&o];

    if (_opt.removeInstantiatedTypeDefs)
        _instantiatedDeclarations.insert(&o);

    // No references
    if (refs.empty()) {
        if (_opt.removeUnreferenced)
            _trash.insert(&o);
        return 0;
    }

    for (References::iterator it = refs.begin(); it != refs.end(); ++it) {
        TypeReference *typeRef = dynamic_cast<TypeReference *>(*it);
        if (typeRef != nullptr) {
            TypeReference::DeclarationType *tDecl = instantiate(typeRef, _sem);
            messageAssert(tDecl != nullptr, "Instantiate failed", typeRef, _sem);

            TypeReference::DeclarationType *tDeclCopy = _setDeclCopy(&o, static_cast<TypeDef *>(tDecl));

            typeRef->setName(tDeclCopy->getName());
            _trash.insert(typeRef->templateParameterAssigns);
            hif::semantics::resetDeclarations(typeRef);
        } else {
            messageError("Unexpected case", *it, _sem);
        }
    }
    return 0;
}

int ResolveTemplates::visitView(View &o)
{
    if (hif::declarationIsPartOfStandard(&o))
        return 0;

    // Special case for the top level
    if (&o == _top) {
        if (_isCTC(o.templateParameters))
            _isFixed = true;

        simplify(&o, _sem, _sOpt);
        _trash.insert(o.templateParameters);

        fixDeclarationsInMap(&o);
        return 0;
    }

    if (!_isCTC(o.templateParameters)) {
        fixDeclarationsInMap(&o);
        return 0;
    }

    _visitedDeclarations.insert(&o);

    // trash the design unit
    DesignUnit *parentDu = dynamic_cast<DesignUnit *>(o.getParent());
    messageAssert(parentDu != nullptr, "Unexpected case.", &o, _sem);
    _isFixed = true;

    DeclarationsReferences dRef = *_refsMapList.back();
    References &refs            = dRef[&o];

    if (_opt.removeInstantiatedViews) {
        _instantiatedDeclarations.insert(&o);
        _designUnits.insert(parentDu);
    }

    // No references
    if (refs.empty()) {
        if (_opt.removeUnreferenced && _top != &o) {
            _trash.insert(&o);
            _designUnits.insert(parentDu);
        }
        return 0;
    }

    for (References::iterator it = refs.begin(); it != refs.end(); ++it) {
        ViewReference *viewRef = dynamic_cast<ViewReference *>(*it);
        if (viewRef == nullptr)
            messageError("Unexpected case", *it, _sem);

        ViewReference::DeclarationType *vDecl = instantiate(viewRef, _sem);
        messageAssert(vDecl != nullptr, "Instantiate failed", viewRef, _sem);

        ViewReference::DeclarationType *vDeclCopy = _setDeclCopy(&o, vDecl);

        DesignUnit *duCopy = dynamic_cast<DesignUnit *>(vDeclCopy->getParent());
        messageAssert(duCopy != nullptr, "Unexpected case.", vDeclCopy, _sem);
        viewRef->setDesignUnit(duCopy->getName());
        _trash.insert(viewRef->templateParameterAssigns);
        hif::semantics::resetDeclarations(viewRef);
        hif::manipulation::simplify(duCopy, _sem, _sOpt);
    }

    return 0;
}

void ResolveTemplates::fixDeclarationsInMap(Declaration *o)
{
    hif::manipulation::simplify(o, _sem, _sOpt);
    if (_visitedDeclarations.find(o) != _visitedDeclarations.end())
        return;
    _visitedDeclarations.insert(o);
    DeclarationsReferences referencesMapNew;
    hif::semantics::GetReferencesOptions opt;
    opt.include_unreferenced      = true;
    opt.skip_standard_declarations = true;
    hif::semantics::getAllReferences(referencesMapNew, _sem, o, opt);
    referencesMapNew.erase(o);

    _refsMapList.push_back(&referencesMapNew);
    for (DeclarationsReferences::iterator it = referencesMapNew.begin(); it != referencesMapNew.end(); ++it) {
        it->first->acceptVisitor(*this);
    }
    _refsMapList.pop_back();
}

bool ResolveTemplates::isFixed() { return _isFixed; }

const Declarations &ResolveTemplates::getVisitedDeclarations() { return _visitedDeclarations; }
void ResolveTemplates::_handleProcedureCall(ProcedureCall::DeclarationType *decl, ProcedureCall *pcall)
{
    ProcedureCall::DeclarationType *pDecl = instantiate(pcall, _sem);
    messageAssert(pDecl != nullptr, "Instantiate failed", pcall, _sem);

    ProcedureCall::DeclarationType *pDeclCopy = _setDeclCopy(decl, pDecl);

    pcall->setName(pDeclCopy->getName());
    _trash.insert(pcall->templateParameterAssigns);
    hif::semantics::resetDeclarations(pcall);
    hif::manipulation::simplify(pDeclCopy, _sem, _sOpt);
}

void ResolveTemplates::_simplifyByContext(Object *o)
{
    if (o == nullptr)
        return;

    SimplifyOptions opt;
    opt.context                      = o;
    opt.simplify_constants           = true;
    opt.simplify_template_parameters = true;
    opt.simplify_parameters          = true;
    opt.simplify_statements          = true;

    simplify(o, _sem, opt);
}

bool ResolveTemplates::_isCTC(const BList<Declaration> &templates) const
{
    if (templates.empty())
        return false;
    if (_opt.forceNonCTCInstantiation)
        return true;

    for (auto decl : templates) {
        if (dynamic_cast<TypeTP *>(decl) != nullptr) {
            return true;
        } else if (dynamic_cast<ValueTP *>(decl) != nullptr) {
            auto vtp = static_cast<ValueTP *>(decl);
            if (vtp->isCompileTimeConstant())
                return true;
            continue;
        }
        messageError("Unexpected declaration in tempalte list", decl, _sem);
    }

    return false;
}

template <typename T> T *ResolveTemplates::_setDeclCopy(T *d, T *decl)
{
    T *declCopy                     = nullptr;
    InstanceToCopyMap::iterator git = _instanceToCopyMap.find(decl);
    if (git != _instanceToCopyMap.end()) {
        declCopy = dynamic_cast<T *>(git->second);
        return declCopy;
    }

    declCopy = hif::copy(decl);
    _trash.insert(declCopy->templateParameters);

    _instanceToCopyMap[decl]                 = declCopy;
    // Handle the View special case
    ViewReference::DeclarationType *viewDecl = dynamic_cast<ViewReference::DeclarationType *>(declCopy);
    if (viewDecl != nullptr) {
        DesignUnit *parentDu         = dynamic_cast<DesignUnit *>(d->getParent());
        // instantiated View
        DesignUnit *parentDuDeclCopy = new DesignUnit();

        messageAssert(parentDu != nullptr && parentDuDeclCopy != nullptr, "Unexpected case.", viewDecl, _sem);

        std::string n(parentDu->getName() + std::string("_inst"));
        parentDuDeclCopy->setName(NameTable::getInstance()->getFreshName(n.c_str()));

        parentDuDeclCopy->views.push_back(viewDecl);

        messageAssert(parentDu->isInBList(), "Unsupported case", parentDu, _sem);
        BList<Object> *list = parentDu->getBList();
        list->push_back(parentDuDeclCopy);
    } else {
        std::string n(declCopy->getName() + std::string("_inst"));
        declCopy->setName(NameTable::getInstance()->getFreshName(n.c_str()));

        messageAssert(d->isInBList(), "Unsupported case", d, _sem);
        BList<Object> *list = d->getBList();
        list->insert(declCopy, list->getPosition(d), true);
    }

    if (dynamic_cast<SubProgram *>(d) != nullptr) {
        SubProgram *sub              = dynamic_cast<SubProgram *>(d);
        SubProgram *subCopy          = dynamic_cast<SubProgram *>(declCopy);
        BList<Parameter>::iterator i = sub->parameters.begin();
        BList<Parameter>::iterator j = subCopy->parameters.begin();
        for (; i != sub->parameters.end(); ++i, ++j) {
            Parameter *p1 = *i;
            Parameter *p2 = *j;
            if (p2->getValue() != nullptr || p1->getValue() == nullptr)
                continue;

            p2->setValue(hif::copy(p1->getValue()));
        }
    }

    hif::semantics::mapDeclarationsInTree(declCopy, decl, declCopy, _sem);
    _simplifyByContext(declCopy);
    fixDeclarationsInMap(declCopy);

    return declCopy;
}

#ifdef ENABLE_EXPERIMENTAL_FIX
void ResolveTemplates::_fixUndeclaredSymbols(hif::SubProgram *sp, hif::FunctionCall *fcall)
{
    std::list<Object *> list;
    Semantics::collectSymbols(list, sp, _sem);

    for (std::list<Object *>::iterator it = list.begin(); it != list.end(); ++it) {
        hif::Object *symbol = *it;

        Declaration *decl = Semantics::getDeclaration(symbol, _sem, sp->getStateTable()->states.front(), true);
        if (decl != nullptr)
            continue;

        Declaration *origDecl = Semantics::getDeclaration(symbol, _sem, fcall);
        if (origDecl == nullptr && dynamic_cast<Instance *>(symbol) != nullptr)
            continue;
        messageAssert(origDecl != nullptr, "Not found declaration.", symbol, _sem);

        DataDeclaration *dd = dynamic_cast<DataDeclaration *>(origDecl);
        messageAssert(dd != nullptr, "Unsupported case (1).", origDecl, _sem);
        messageAssert(dynamic_cast<Alias *>(origDecl) == nullptr, "Unsupported case.", origDecl, _sem);

        // It will be managed by fixing the symbol inside its prefix.
        if (dynamic_cast<Field *>(origDecl) != nullptr)
            continue;

        // Add the declaration in the SubProgram
        Parameter *par = new Parameter();
        par->SetType(hif::copy(dd->getType()));
        par->setName(dd->getName());
        sp->parameters.push_back(par);

        // Add the parameter assign in the functioncall
        Value *symbolValue = dynamic_cast<Value *>(symbol);
        messageAssert(symbolValue != nullptr, "Unsupported case (2).", symbol, _sem);

        ParameterAssign *pa = new ParameterAssign();
        pa->setName(dd->getName());
        pa->setValue(hif::copy(symbolValue));
        fcall->parameterAssigns.push_back(pa);
    }
}
#endif
class RangeVisitor : public GuideVisitor
{
public:
    RangeVisitor(
        hif::semantics::ILanguageSemantics *sem,
        const hif::manipulation::ResolveTempalteOptions &mainOpts,
        Declarations &instantiatedDeclarations);
    ~RangeVisitor();

    int visitFunctionCall(FunctionCall &o);
    int visitRange(hif::Range &o);
    int visitView(hif::View &o);
    int visitLibraryDef(hif::LibraryDef &o);

    int visitFunction(Function &o);
    int visitProcedure(Procedure &o);
    int visitTypeDef(TypeDef &o);

private:
    bool _checkRange(Range *range, bool noError);
    bool _checkBound(Value *v);
    bool _checkDeclaration(Declaration *o);

    hif::semantics::ILanguageSemantics *_sem;
    hif::manipulation::SimplifyOptions _opts;
    const hif::manipulation::ResolveTempalteOptions &_mainOpts;
    Declarations &_instantiatedDeclarations;
    bool _needConstant;

    RangeVisitor(const RangeVisitor &);
    RangeVisitor &operator=(const RangeVisitor &);
};

RangeVisitor::RangeVisitor(
    hif::semantics::ILanguageSemantics *sem,
    const hif::manipulation::ResolveTempalteOptions &mainOpts,
    Declarations &instantiatedDeclarations)
    : GuideVisitor()
    , _sem(sem)
    , _opts()
    , _mainOpts(mainOpts)
    , _instantiatedDeclarations(instantiatedDeclarations)
    , _needConstant(true)
{
    _opts.simplify_constants                   = true;
    _opts.simplify_defines                     = true;
    _opts.simplify_functioncalls               = true;
    _opts.simplify_statements                  = true;
    _opts.simplify_template_parameters         = true;  // Simplify only templates which are not actually tempaltes,
    _opts.simplify_non_ctc_template_parameters = false; // but constants passed to instance constructor
}

RangeVisitor::~RangeVisitor() {}

int RangeVisitor::visitFunctionCall(FunctionCall &o)
{
    GuideVisitor::visitFunctionCall(o);

    // In case of standard methods, ensure returned types are fixed size!
    // ref design: vhdl/ips/mephisto_core + ddt
    if (!hif::declarationIsPartOfStandard(&o, _sem))
        return 0;
    Type *baseType = hif::semantics::getBaseType(&o, false, _sem);
    if (!hif::semantics::isVectorType(baseType, _sem) && dynamic_cast<Int *>(baseType) == nullptr)
        return 0;

    bool ok = _checkRange(hif::typeGetSpan(baseType, _sem), true);
    if (ok)
        return 0;

    hif::manipulation::SimplifyOptions opts;
    opts.simplify_constants = true;
    hif::manipulation::simplify(&o, _sem, opts);

    // Checking that simplify() has done a good job:
    hif::semantics::resetTypes(&o);
    baseType           = hif::semantics::getBaseType(&o, false, _sem);
    bool checkOk = _checkRange(hif::typeGetSpan(baseType, _sem), true);
    if (!checkOk) {
        messageDebug("Non-constant span type:", baseType, _sem);
        messageDebug("Unable to get a constant span", &o, _sem);
    }

    return 0;
}

int RangeVisitor::visitRange(hif::Range &o)
{
    if (dynamic_cast<DataDeclaration *>(o.getParent()) != nullptr) {
        return hif::GuideVisitor::visitRange(o);
    }

    _checkRange(&o, false);

    return hif::GuideVisitor::visitRange(o);
}

int RangeVisitor::visitView(View &o)
{
    bool restore = _needConstant;
    if (_checkDeclaration(&o))
        return 0;
    hif::GuideVisitor::visitView(o);
    _needConstant = restore;
    return 0;
}

int RangeVisitor::visitLibraryDef(LibraryDef &o)
{
    if (o.isStandard())
        return 0;
    return hif::GuideVisitor::visitLibraryDef(o);
}

int RangeVisitor::visitFunction(Function &o)
{
    bool restore = _needConstant;
    if (_checkDeclaration(&o))
        return 0;
    hif::GuideVisitor::visitFunction(o);
    _needConstant = restore;
    return 0;
}

int RangeVisitor::visitProcedure(Procedure &o)
{
    bool restore = _needConstant;
    if (_checkDeclaration(&o))
        return 0;
    hif::GuideVisitor::visitProcedure(o);
    _needConstant = restore;
    return 0;
}

int RangeVisitor::visitTypeDef(TypeDef &o)
{
    bool restore = _needConstant;
    if (_checkDeclaration(&o))
        return 0;
    hif::GuideVisitor::visitTypeDef(o);
    _needConstant = restore;
    return 0;
}

bool RangeVisitor::_checkRange(Range *range, bool noError)
{
    if (range == nullptr)
        return true;

    auto parent = dynamic_cast<Slice *>(range->getParent());
    if (parent != nullptr) {
        // Bounds of normal slices are not required to be ctc:
        // a[n , m] = expr or b = a[n, m]
        // n and m are not required to be constatns. But here constvalueBounds
        // is specificed by options. So manage other options:
        const auto isLhs = hif::manipulation::isInLeftHandSide(parent);
        if (isLhs && _mainOpts.allowLhsSlices)
            return true;
        if (!isLhs && _mainOpts.allowRhsSlices)
            return true;
    }

    bool isSpanInformation = dynamic_cast<String *>(range->getParent()) != nullptr;

    Value *v               = range->getLeftBound();
    bool isLeftError = !_checkBound(v) && (v != nullptr || !isSpanInformation);

    v                       = range->getRightBound();
    bool isRightError = !_checkBound(v) && (v != nullptr || !isSpanInformation);

    std::uint64_t bw = hif::semantics::spanGetBitwidth(range, _sem);
    if (bw == 0) {
        if (isLeftError) {
            if (noError)
                return false;
            messageDebug("Parent: ", range->getParent(), _sem);
            hif::semantics::updateDeclarations(range->getLeftBound(), _sem);
            messageDebug("Left bound: ", range->getLeftBound(), _sem);
            messageError("Left bound is still not a constvalue", range, _sem);
        } else if (isRightError) {
            if (noError)
                return false;
            messageDebug("Parent: ", range->getParent(), _sem);
            hif::semantics::updateDeclarations(range->getRightBound(), _sem);
            messageDebug("Right bound: ", range->getRightBound(), _sem);
            messageError("Right bound is still not a constvalue", range, _sem);
        }
    }
    return true;
}

bool RangeVisitor::_checkBound(Value *v)
{
    if (v == nullptr)
        return false;
    v = hif::manipulation::simplify(v, _sem, _opts);
    if (dynamic_cast<ConstValue *>(v) != nullptr)
        return true;

    if (!_needConstant)
        return true;

    hif::semantics::ReferencesMap refMap;
    hif::semantics::getAllReferences(refMap, _sem, v);
    for (hif::semantics::ReferencesMap::iterator it = refMap.begin(); it != refMap.end(); ++it) {
        Declaration *decl                     = it->first;
        hif::semantics::ReferencesSet &refSet = it->second;
        ValueTP *valTp                        = dynamic_cast<ValueTP *>(decl);
        TypeDef *td     = valTp != nullptr ? dynamic_cast<TypeDef *>(valTp->getParent()) : nullptr;
        SubProgram *sub = valTp != nullptr ? dynamic_cast<SubProgram *>(valTp->getParent()) : nullptr;
        for (hif::semantics::ReferencesSet::iterator jt = refSet.begin(); jt != refSet.end(); ++jt) {
            Identifier *id        = dynamic_cast<Identifier *>(*jt);
            bool isTemplate = hif::isSubNode(id, td) || hif::isSubNode(id, sub);
            if (!isTemplate)
                return false;
        }
    }

    return true;
}

bool RangeVisitor::_checkDeclaration(Declaration *o)
{
    // Skip standard scopes
    if (hif::declarationIsPartOfStandard(o))
        return true;
    // If has templates, it is allowed to not have constant bounds
    if (!hif::objectGetTemplateParameterList(o)->empty()) {
        _needConstant = false;
        return false;
    }
    // If decl has not been visited, then it can have non-constant bounds
    if (_instantiatedDeclarations.find(o) == _instantiatedDeclarations.end()) {
        _needConstant = false;
        return false;
    }
    // Ok, must have only const bounds
    return false;
}

} // namespace

// /////////////////////////////////////////////////////////////////////////////
// ResolveTempalteOptions
// /////////////////////////////////////////////////////////////////////////////

ResolveTempalteOptions::ResolveTempalteOptions()
    : top(nullptr)
    , removeUnreferenced(false)
    , constvalueBounds(false)
    , allowLhsSlices(false)
    , allowRhsSlices(false)
    , removeInstantiatedViews(false)
    , removeInstantiatedSubPrograms(false)
    , removeInstantiatedTypeDefs(false)
    , forceNonCTCInstantiation(false)
{
    // ntd
}

ResolveTempalteOptions::~ResolveTempalteOptions()
{
    // ntd
}

ResolveTempalteOptions::ResolveTempalteOptions(const ResolveTempalteOptions &other)
    : top(other.top)
    , removeUnreferenced(other.removeUnreferenced)
    , constvalueBounds(other.constvalueBounds)
    , allowLhsSlices(other.allowLhsSlices)
    , allowRhsSlices(other.allowRhsSlices)
    , removeInstantiatedViews(other.removeInstantiatedViews)
    , removeInstantiatedSubPrograms(other.removeInstantiatedSubPrograms)
    , removeInstantiatedTypeDefs(other.removeInstantiatedTypeDefs)
    , forceNonCTCInstantiation(other.forceNonCTCInstantiation)
{
    // ntd
}

ResolveTempalteOptions &ResolveTempalteOptions::operator=(ResolveTempalteOptions other)
{
    this->swap(other);
    return *this;
}

void ResolveTempalteOptions::swap(ResolveTempalteOptions &other)
{
    std::swap(top, other.top);
    std::swap(removeUnreferenced, other.removeUnreferenced);
    std::swap(constvalueBounds, other.constvalueBounds);
    std::swap(allowLhsSlices, other.allowLhsSlices);
    std::swap(allowRhsSlices, other.allowRhsSlices);
    std::swap(removeInstantiatedViews, other.removeInstantiatedViews);
    std::swap(removeInstantiatedSubPrograms, other.removeInstantiatedSubPrograms);
    std::swap(removeInstantiatedTypeDefs, other.removeInstantiatedTypeDefs);
    std::swap(forceNonCTCInstantiation, other.forceNonCTCInstantiation);
}
// /////////////////////////////////////////////////////////////////////////////
// resolveTemplates()
// /////////////////////////////////////////////////////////////////////////////

bool resolveTemplates(hif::System *system, hif::semantics::ILanguageSemantics *sem, const ResolveTempalteOptions &opt)
{
    View *top = opt.top;
    // if nullptr top, compute it
    if (top == nullptr)
        top = hif::manipulation::findTopLevelModule(system, sem);
    if (top == nullptr)
        return false;

    bool ret = false;
    Declarations instantiatedDeclarations;
    {
        ResolveTemplates visitor(system, sem, top, opt);
        top->acceptVisitor(visitor);
        ret                      = visitor.isFixed();
        instantiatedDeclarations = visitor.getVisitedDeclarations();
    }

    hif::semantics::resetDeclarations(system);
    hif::semantics::resetTypes(system);
    hif::manipulation::flushInstanceCache();
    hif::semantics::flushTypeCacheEntries();

    if (opt.constvalueBounds) {
        RangeVisitor rv(sem, opt, instantiatedDeclarations);
        system->acceptVisitor(rv);
    }

    // Since tempaltes have been expanded,
    // some template indexes could have been changed into constants,
    // with invalid span dimension. E.g. in HIF Int with non-standard spans are
    // not allowed as indexes.
    // Possible optimization: call this visitor of !opt.constvalueBounds,
    // and merge the fix inside RangeVisitor.
    // Ref design: mios_dpr8x64_verilog2hif_ddt

    // resetTypes: ref designs:
    // - vhdl/ips/mephisto_core + ddt
    // - vhdl/gaisler/can_oc + ddt
    hif::semantics::resetTypes(system);

    IndexVisitor indexVisitor(sem);
    system->acceptVisitor(indexVisitor);
    ret |= indexVisitor.isFixed();

    return ret;
}
} // namespace manipulation
} // namespace hif
