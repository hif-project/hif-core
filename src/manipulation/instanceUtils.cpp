/// @file instanceUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/instanceUtils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/search.hpp"
#include "hif/semantics/semantics.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

//#define HIF_DEBUG_CACHE
//#define SKIP_NO_TEMPLATE_DECLS

namespace hif
{
namespace manipulation
{

namespace
{ // anon namespace

// ///////////////////////////////////////////////////////////////////
// Signature struct
// ///////////////////////////////////////////////////////////////////
struct Signature {
    SubProgram *sub;

    Signature();
    ~Signature();
    Signature(const Signature &s);
    Signature &operator=(const Signature &s);

    bool operator<(const Signature &s) const;
};

Signature::Signature()
    : sub(nullptr)
{
    // ntd
}

Signature::~Signature() { delete sub; }

Signature::Signature(const Signature &s)
    : sub(hif::copy(s.sub))
{
    // ntd
}

Signature &Signature::operator=(const Signature &s)
{
    if (this == &s)
        return *this;
    delete sub;
    sub = hif::copy(s.sub);
    return *this;
}

bool Signature::operator<(const Signature &s) const
{
    std::string n1(sub->getName());
    std::string n2(s.sub->getName());
    if (n1 != n2)
        return n1 < n2;
    std::string s1 = hif::objectGetKey(sub);
    std::string s2 = hif::objectGetKey(s.sub);
    return s1 < s2;
}
// ///////////////////////////////////////////////////////////////////
// Instantiation
// ///////////////////////////////////////////////////////////////////
struct Instantiation {
    Instantiation();
    ~Instantiation();

    BList<TPAssign> templates;
    Declaration *instance;
    Declaration *originalDeclaration;
    bool isSignature;
    hif::semantics::ILanguageSemantics *sem;

    Instantiation(const Instantiation &o);
    Instantiation &operator=(const Instantiation &o);
    bool operator<(const Instantiation &other) const;
};

Instantiation::Instantiation()
    : templates()
    , instance(nullptr)
    , originalDeclaration(nullptr)
    , isSignature(false)
    , sem(nullptr)
{
    // ntd
}

Instantiation::~Instantiation()
{
    templates.clear();
    delete instance;
}

Instantiation::Instantiation(const Instantiation &o)
    : templates()
    , instance(nullptr)
    , originalDeclaration(nullptr)
    , isSignature(o.isSignature)
    , sem(nullptr)
{
    // warning move semantics
    Instantiation *i = const_cast<Instantiation *>(&o);
    templates.merge(i->templates);
    instance               = i->instance;
    i->instance            = nullptr;
    originalDeclaration    = i->originalDeclaration;
    i->originalDeclaration = nullptr;
    sem                    = i->sem;
    i->sem                 = nullptr;
}

Instantiation &Instantiation::operator=(const Instantiation &o)
{
    // warning move semantics
    if (this == &o)
        return *this;
    this->~Instantiation();
    new (this) Instantiation(o);
    return *this;
}

bool Instantiation::operator<(const Instantiation &other) const { return instance < other.instance; }
// ///////////////////////////////////////////////////////////////////
// Typedefs
// ///////////////////////////////////////////////////////////////////
typedef std::set<SubProgram *> SubPrograms;
typedef std::map<Signature, SubPrograms> RecursionMap;
typedef std::map<Declaration *, Object *> NameMap;
typedef std::set<Parameter *> Parameters;
typedef std::map<Declaration *, std::list<Instantiation>> Cache;
typedef std::set<Declaration *> Instantiations;
// ///////////////////////////////////////////////////////////////////
// Global variables
// ///////////////////////////////////////////////////////////////////
RecursionMap recursionMap;
Cache viewCache;
Cache subCache;
Cache typeDefCache;
hif::Trash trashCache;
Instantiations allInstantions;
// ///////////////////////////////////////////////////////////////////
// Utility methods
// ///////////////////////////////////////////////////////////////////

// Is only a wrapper to add options if needed
template <typename T> T *_copy(T *o) { return hif::copy(o); }

/// @brief This method check whether the signature depends on
/// one or more actual values assigned to formal parameters.
/// This is possible in VHDL, for example:
/// @code
/// foo: ( int p 0) -> ( bitvector< p > )
/// @endcode
/// In case this case is matched, returns true after having set initial
/// values of formal parameters to actual parameter values.
void _replaceFormalParameters(BList<ParameterAssign> &assigns, SubProgram *decl, BList<Parameter> &paramList)
{
    messageAssert(
        assigns.size() == paramList.size(), "Unexpected diffrent size between formal and actual parameters list",
        nullptr, nullptr);
    Parameters dependingParams;
    if (!hif::signatureDependsOnActualParameters(decl, dependingParams))
        return;

    // found
    BList<ParameterAssign>::iterator i = assigns.begin();
    BList<Parameter>::iterator j       = paramList.begin();
    for (; j != paramList.end(); ++i, ++j) {
        if (dependingParams.find(*j) != dependingParams.end()) {
            // Putting a cast on the initial value, to ensure that all operations
            // involving the current formal parameter are still valid.
            Cast *c = new Cast();
            c->setValue(_copy((*i)->getValue()));
            c->setType(_copy((*j)->getType()));

            delete (*j)->setValue(c);
        } else {
            delete (*j)->setValue(nullptr);
        }
    }
}

void _simplifyTemplates(
    Declaration *decl,
    hif::semantics::ILanguageSemantics *ref_sem,
    Object * /*context*/,
    const bool simplifyParameters = false)
{
    hif::manipulation::SimplifyOptions opt;
    opt.simplify_parameters          = simplifyParameters;
    opt.simplify_template_parameters = true;
    opt.simplify_statements          = true;
    opt.simplify_generates           = false;
    opt.simplify_semantics_types     = true;
    opt.simplify_declarations        = false;
    opt.replace_result               = true;
    opt.simplify_constants           = false;
    //opt.context = context;
    opt.root                         = decl;
    hif::manipulation::simplify(decl, ref_sem, opt);
}

void _replaceTemplates(BList<TPAssign> &assigns, BList<Declaration> &tpList)
{
    messageAssert(
        assigns.size() == tpList.size(), "Unexpected diffrent size between assigns and template parameters list",
        nullptr, nullptr);
    BList<TPAssign>::iterator i    = assigns.begin();
    BList<Declaration>::iterator j = tpList.begin();

    for (; i != assigns.end(); ++i, ++j) {
        ValueTPAssign *vtpa = dynamic_cast<ValueTPAssign *>(*i);
        TypeTPAssign *ttpa  = dynamic_cast<TypeTPAssign *>(*i);

        if (vtpa != nullptr) {
            ValueTP *vtp = dynamic_cast<ValueTP *>(*j);
            messageAssert(vtp != nullptr, "Unexpected case (1)", *j, nullptr);

            // Putting a cast on the initial value, to ensure that all operations
            // involving the current template parameter are still valid.
            Cast *c = new Cast();
            c->setValue(_copy(vtpa->getValue()));
            c->setType(_copy(vtp->getType()));

            delete vtp->setValue(c);
        } else // ttpa
        {
            TypeTP *ttp = dynamic_cast<TypeTP *>(*j);
            messageAssert(ttp != nullptr, "Unexpected case (2)", *j, nullptr);
            delete ttp->setType(nullptr);
            ttp->setType(_copy(ttpa->getType()));
        }
    }
}

// ///////////////////////////////////////////////////////////////////
// Cache utility methods
// ///////////////////////////////////////////////////////////////////
void _addCacheEntry(
    BList<TPAssign> &templates,
    Declaration *newInstance,
    Declaration *origDecl,
    hif::semantics::ILanguageSemantics *sem,
    Cache &cache,
    const bool sigDependsOnActualParams = false,
    const bool onlySignature            = false)
{
    if (sigDependsOnActualParams) {
        trashCache.insert(newInstance);
        allInstantions.insert(newInstance);
        return;
    }

    //messageDebugAssert(!templates.empty(), "Unexpected empty templates list", nullptr, sem);

#ifdef HIF_DEBUG_CACHE
    Object *parent = templates.front()->getParent();
    clog << "Adding cache entry for: ";
    if (dynamic_cast<ViewReference *>(parent) != nullptr) {
        ViewReference *vr = static_cast<ViewReference *>(parent);
        clog << vr->getUnit() << "[" << vr->getName() << "]";
    } else {
        clog << objectGetName(parent);
    }

    clog << "(decl: "
         << hif::semantics::getDeclaration(parent, hif::semantics::HIFSemantics::getInstance(), nullptr, false, true)
         << ", " << newInstance << ", " << origDecl << ")";

    clog << " < ";
    for (BList<TPAssign>::iterator i = templates.begin(); i != templates.end(); ++i) {
        clog << objectGetName(*i) << " ";
    }
    clog << ">" << endl;
#endif

    Instantiation inst;
    inst.templates.merge(templates);
    inst.instance            = newInstance;
    inst.originalDeclaration = origDecl;
    inst.isSignature         = onlySignature;
    inst.sem                 = sem;

    cache[origDecl].push_back(inst);
    allInstantions.insert(newInstance);
}
Declaration *_searchCacheEntry(
    BList<TPAssign> &templates,
    Declaration *origDecl,
    hif::semantics::ILanguageSemantics *sem,
    Cache &cache,
    const bool isSignature)
{
    Cache::iterator it = cache.find(origDecl);
    if (it == cache.end())
        return nullptr;

    hif::EqualsOptions opt;
    opt.assureSameSymbolDeclarations = true;

    for (Cache::mapped_type::iterator i = it->second.begin(); i != it->second.end(); ++i) {
        if (sem != (*i).sem)
            continue;
        if (isSignature != (*i).isSignature)
            continue;

        BList<TPAssign> &tpAss = (*i).templates;
        if (!hif::equals(templates, tpAss, opt))
            continue;

        return (*i).instance;
    }

    return nullptr;
}
template <typename T>
T *_searchCacheEntry(
    BList<TPAssign> &templates,
    T *origDecl,
    hif::semantics::ILanguageSemantics *sem,
    Cache &cache,
    const bool isSignature = false)
{
    return static_cast<T *>(
        _searchCacheEntry(templates, static_cast<Declaration *>(origDecl), sem, cache, isSignature));
}

// ///////////////////////////////////////////////////////////////////
// Instantiate utility methods
// ///////////////////////////////////////////////////////////////////
View *_instantiate(
    ViewReference *symbol,
    hif::semantics::ILanguageSemantics *sem,
    const InstantiateOptions &opt = InstantiateOptions())
{
    typedef ViewReference SymbolType;
    typedef SymbolType::DeclarationType DeclarationType;

    DeclarationType *originalDecl = hif::semantics::getDeclaration(symbol, sem);
    if (originalDecl == nullptr)
        return nullptr;
#ifdef SKIP_NO_TEMPLATE_DECLS
    if (originalDecl->templateParameters.empty())
        return originalDecl;
#endif

    hif::semantics::updateDeclarations(symbol, sem);
    SymbolType *symbolCopy = hif::copy(symbol);

    const bool canReplaceSymbol = symbol->getParent() != nullptr;
    if (canReplaceSymbol)
        symbol->replace(symbolCopy);

    hif::manipulation::sortParameters(
        symbolCopy->templateParameterAssigns, originalDecl->templateParameters, true,
        hif::manipulation::SortMissingKind::ALL, sem);
    if (originalDecl->templateParameters.size() != symbolCopy->templateParameterAssigns.size()) {
        messageDebug("originalDecl", originalDecl, sem);
        messageDebug("symbolCopy", symbolCopy, sem);
        messageError("Template parameters of symbol and declaration do not match", nullptr, nullptr);
    }

    if (canReplaceSymbol)
        symbolCopy->replace(symbol);

    // Search in cache if current template configuration was already
    // calculated for corresponding symbol.
    DeclarationType *cacheEntry =
        _searchCacheEntry(symbolCopy->templateParameterAssigns, originalDecl, sem, viewCache, opt.onlySignature);

    // If found, return entry in cache.
    if (cacheEntry != nullptr) {
        delete symbolCopy;
        return cacheEntry;
    }

    Contents *cc = nullptr;
    if (opt.onlySignature)
        cc = originalDecl->setContents(nullptr);

    hif::semantics::updateDeclarations(originalDecl, sem);
    DeclarationType *declarationCopy = hif::copy(originalDecl);

    if (opt.onlySignature)
        originalDecl->setContents(cc);

    // Now replace template parameters with their instantiations.
    _replaceTemplates(symbolCopy->templateParameterAssigns, declarationCopy->templateParameters);
    hif::semantics::mapDeclarationsInTree(declarationCopy, originalDecl, declarationCopy, sem);

    // Temporary fix: replace should be not necessary.
    // Reference design: Built_In_FIR into step: bindOpenPortAssign.
    const bool canReplace = (originalDecl->getParent() != nullptr);
    if (canReplace)
        originalDecl->replace(declarationCopy);

    _simplifyTemplates(declarationCopy, sem, symbol);

    if (canReplace)
        declarationCopy->replace(originalDecl);
    // Add current configuration to the cache.
    _addCacheEntry(
        symbolCopy->templateParameterAssigns, declarationCopy, originalDecl, sem, viewCache, opt.onlySignature);

    delete symbolCopy;
    return declarationCopy;
}

template <typename T>
ViewReference *
_isInstanceMethodSubProgram(T *symbol, hif::semantics::ILanguageSemantics *sem, const InstantiateOptions &opt)
{
    typedef T SymbolType;
    typedef typename SymbolType::DeclarationType DeclarationType;

    DeclarationType *candidate = dynamic_cast<DeclarationType *>(opt.candidate);
    const bool hasCandidate    = (candidate != nullptr);

    Value *symbolInstance = symbol->getInstance();
    if (symbolInstance == nullptr)
        return nullptr;

    // Intresting cases for instance:
    // - hif::Instance of ViewReference
    // - ViewReference semantics type (instance method) but not special cases
    //   as IMPLICIT_INSTANCE methods.

    ViewReference *retVr = nullptr;
    if (dynamic_cast<Instance *>(symbolInstance) != nullptr) {
        Instance *i        = static_cast<Instance *>(symbolInstance);
        ReferencedType *rt = i->getReferencedType();
        if (dynamic_cast<ViewReference *>(rt) == nullptr)
            return nullptr;

        retVr = static_cast<ViewReference *>(rt);
    } else {
        // check the type of instance
        Type *instType     = hif::semantics::getSemanticType(symbolInstance, sem);
        Type *instBaseType = hif::semantics::getBaseType(instType, false, sem, true);

        retVr = dynamic_cast<ViewReference *>(instBaseType);
        if (retVr == nullptr)
            return nullptr;

        // Ensuring correct management fo special standard methods.
        DeclarationType *declToCheck = nullptr;
        if (hasCandidate) {
            declToCheck = candidate;
        } else {
            // remove call instance to avoid infinite loop
            Value *tmpInst = symbol->setInstance(nullptr);

            hif::semantics::GetCandidatesOptions copt;
            copt.atLeastOne = true;
            std::list<DeclarationType *> candidates;
            hif::semantics::getCandidates(candidates, symbol, sem, copt);

            // restore call instance
            symbol->setInstance(tmpInst);

            // at the moment assuming unique candidate (may be overloaded?)
            if (candidates.size() != 1)
                return retVr;

            declToCheck = candidates.front();
        }
        // if is implicit instance do nothing
        if (declToCheck->getKind() == SubProgram::IMPLICIT_INSTANCE)
            return nullptr;

        retVr = static_cast<ViewReference *>(instBaseType);
    }

    return retVr;
}

template <typename T>
typename T::DeclarationType *_instantiateInstanceMethodSubProgram(
    T *symbol,
    hif::semantics::ILanguageSemantics *sem,
    const InstantiateOptions &opt,
    ViewReference *instanceVr,
    View *viewDecl)
{
    typedef T SymbolType;
    typedef typename SymbolType::DeclarationType DeclarationType;

    DeclarationType *candidate = dynamic_cast<DeclarationType *>(opt.candidate);
    const bool hasCandidate    = (candidate != nullptr);

    // Searching subprogram inside instantiated method class.
    View *vv = hif::manipulation::instantiate(instanceVr, sem);
    if (vv == nullptr) {
        messageDebugAssert(false, "Unable to instantiate method instance (viewref).", symbol, nullptr);
        return nullptr;
    }

    const bool canReplaceInstView = (viewDecl->getParent() != nullptr);
    if (canReplaceInstView)
        viewDecl->replace(vv);

    hif::semantics::DeclarationsStack stack(symbol);
    stack.push();

    hif::semantics::ResetDeclarationsOptions ropt;
    ropt.onlySignatures = true;
    hif::semantics::resetDeclarations(symbol, ropt);

    DeclarationType *dd = nullptr;
    if (!hasCandidate) {
        // Temporarily remove instance to prevent infinite loop
        Value *inst = symbol->setInstance(nullptr);

        hif::semantics::DeclarationOptions dopt;
        dopt.location = vv->getContents();
        dd            = hif::semantics::getDeclaration(symbol, sem, dopt);

        // Restore instance
        symbol->setInstance(inst);
    } else {
        dd = dynamic_cast<DeclarationType *>(hif::manipulation::matchObject(candidate, viewDecl, vv, sem));
    }

    if (dd == nullptr) {
        // not found. Restore original view and return nullptr
        if (canReplaceInstView)
            vv->replace(viewDecl);
        stack.pop();

        return nullptr;
    }

    // Temporarily remove instance to prevent infinite loop
    Value *inst = symbol->setInstance(nullptr);

    InstantiateOptions opt2;
    opt2.candidate       = dd;
    DeclarationType *ret = instantiate(symbol, sem, opt2);

    // Restore instance
    symbol->setInstance(inst);

    // Restore original view
    if (canReplaceInstView)
        vv->replace(viewDecl);
    stack.pop();

    return ret;
}

template <typename T>
typename T::DeclarationType *
_instantiateSubprogram(T *symbol, hif::semantics::ILanguageSemantics *sem, const InstantiateOptions &opt)
{
    typedef T SymbolType;
    typedef typename SymbolType::DeclarationType DeclarationType;

    DeclarationType *candidate = dynamic_cast<DeclarationType *>(opt.candidate);
    const bool hasCandidate    = (candidate != nullptr);

    ViewReference *instVr = _isInstanceMethodSubProgram(symbol, sem, opt);
    if (instVr != nullptr) {
        View *viewDecl = hif::semantics::getDeclaration(instVr, sem);
        if (!hasCandidate || hif::isSubNode(candidate, viewDecl)) {
            // special management for instance methods.
            return _instantiateInstanceMethodSubProgram(symbol, sem, opt, instVr, viewDecl);
        }
    }

    // Standard management
    DeclarationType *originalDecl = hasCandidate ? candidate : hif::semantics::getDeclaration(symbol, sem);
    if (originalDecl == nullptr)
        return nullptr;

    Parameters dependingParams;
    const bool dependsOnActualParameters = hif::signatureDependsOnActualParameters(originalDecl, dependingParams);

    // This check should be just an optimization, since the return type of instantiated method
    // will be simplifyed by getSemanticType().
#ifdef SKIP_NO_TEMPLATE_DECLS
    if (originalDecl->templateParameters.empty() && !dependsOnActualParameters) {
        // nothing to instantiate, return the original declaration
        return originalDecl;
    }
#endif

    SymbolType *symbolCopy = nullptr;
    if (hasCandidate) {
        hif::semantics::DeclarationsStack stack(symbol);
        stack.push();

        hif::semantics::setDeclaration(symbol, originalDecl);
        hif::semantics::updateDeclarations(symbol, sem);
        symbolCopy = hif::copy(symbol);

        stack.pop();
    } else {
        hif::semantics::updateDeclarations(symbol, sem);
        symbolCopy = hif::copy(symbol);
        symbolCopy->addProperty(PROPERTY_TEMPORARY_OBJECT);
    }
    const bool canReplaceSymbol = symbol->getParent() != nullptr;
    if (canReplaceSymbol)
        symbol->replace(symbolCopy);

    // Sort parameters of the call copy watching the declaration
    // setting eventual bad names and adding the missing parameter.
    hif::manipulation::sortParameters(
        symbolCopy->parameterAssigns, originalDecl->parameters, true, hif::manipulation::SortMissingKind::ALL, sem,
        hasCandidate);
    if (originalDecl->parameters.size() != symbolCopy->parameterAssigns.size()) {
        if (canReplaceSymbol)
            symbolCopy->replace(symbol);
        delete symbolCopy;
        messageDebugAssert(hasCandidate, "Expected candidate (1)", nullptr, sem);
        return nullptr;
    }

    // Sort template parameters of the call copy watching the declaration
    // setting eventual bad names and adding the missing parameter.
    hif::manipulation::sortParameters(
        symbolCopy->templateParameterAssigns, originalDecl->templateParameters, true,
        hif::manipulation::SortMissingKind::ALL, sem, hasCandidate);
    if (originalDecl->templateParameters.size() != symbolCopy->templateParameterAssigns.size()) {
        if (canReplaceSymbol)
            symbolCopy->replace(symbol);
        delete symbolCopy;
        messageDebugAssert(hasCandidate, "Expected candidate (2)", nullptr, sem);
        return nullptr;
    }

    if (canReplaceSymbol)
        symbolCopy->replace(symbol);

    // Search in cache if current template configuration was already
    // calculated for corresponding symbol.
    DeclarationType *cacheEntry = nullptr;
    if (!dependsOnActualParameters) {
        cacheEntry =
            _searchCacheEntry(symbolCopy->templateParameterAssigns, originalDecl, sem, subCache, opt.onlySignature);
        if (cacheEntry != nullptr) {
            delete symbolCopy;
            return cacheEntry;
        }
    }

    StateTable *st = nullptr;
    if (opt.onlySignature) {
        // if only signature option, temporary remove the subprogram state stable.
        st = originalDecl->setStateTable(nullptr);
    }

    hif::semantics::updateDeclarations(originalDecl, sem);
    DeclarationType *declarationCopy = hif::copy(originalDecl);

    if (opt.onlySignature) {
        // Restoring the state table if removed.
        originalDecl->setStateTable(st);
    }

    // If depends on actual parameter
    // replace parameters with their instantiations.
    if (dependsOnActualParameters) {
        _replaceFormalParameters(symbolCopy->parameterAssigns, declarationCopy, declarationCopy->parameters);
    }

    // Now replace template parameters with their instantiations.
    _replaceTemplates(symbolCopy->templateParameterAssigns, declarationCopy->templateParameters);
    // Update internal declarations
    hif::semantics::mapDeclarationsInTree(declarationCopy, originalDecl, declarationCopy, sem);

    // Temporary fix: replace should be not necessary.
    // Reference design: Built_In_FIR into step: bindOpenPortAssign.
    const bool canReplace = (originalDecl->getParent() != nullptr);
    if (canReplace)
        originalDecl->replace(declarationCopy);

    _simplifyTemplates(declarationCopy, sem, symbol, dependsOnActualParameters);

    if (canReplace)
        declarationCopy->replace(originalDecl);

    // Add current configuration to the cache.
    _addCacheEntry(
        symbolCopy->templateParameterAssigns, declarationCopy, originalDecl, sem, subCache, dependsOnActualParameters,
        opt.onlySignature);

    delete symbolCopy;
    return declarationCopy;
}

TypeDef *_instantiate(TypeReference *symbol, hif::semantics::ILanguageSemantics *sem)
{
    typedef TypeReference SymbolType;

    TypeDef *originalDecl = dynamic_cast<TypeDef *>(hif::semantics::getDeclaration(symbol, sem));
    if (originalDecl == nullptr)
        return nullptr;
        // In case of no tempaltes, returning something in the tree could be unsafe for typing,
        // since it could be not-simplifyed.
        // Ref. design: vhdl/openCores/avs_aes + composite_recurse = true
        // in ILanguageSemantics::getSliceSemanticType() for typerefs.
#ifdef SKIP_NO_TEMPLATE_DECLS
    if (originalDecl->templateParameters.empty())
        return originalDecl;
#endif

    hif::semantics::updateDeclarations(symbol, sem);
    SymbolType *symbolCopy = hif::copy(symbol);

    const bool canReplaceSymbol = symbol->getParent() != nullptr;
    if (canReplaceSymbol)
        symbol->replace(symbolCopy);

    hif::manipulation::sortParameters(
        symbolCopy->templateParameterAssigns, originalDecl->templateParameters, true,
        hif::manipulation::SortMissingKind::ALL, sem);
    messageAssert(
        originalDecl->templateParameters.size() == symbolCopy->templateParameterAssigns.size(), "Sort parameter failed",
        symbol, sem);

    if (canReplaceSymbol)
        symbolCopy->replace(symbol);

    // Search in cache if current template configuration was already
    // calculated for corresponding symbol.
    TypeDef *cacheEntry = _searchCacheEntry(symbolCopy->templateParameterAssigns, originalDecl, sem, typeDefCache);

    // If found, return entry in cache.
    if (cacheEntry != nullptr) {
        delete symbolCopy;
        return cacheEntry;
    }

    hif::semantics::updateDeclarations(originalDecl, sem);
    TypeDef *declarationCopy = hif::copy(originalDecl);

    // Now replace template parameters with their instantiations.
    _replaceTemplates(symbolCopy->templateParameterAssigns, declarationCopy->templateParameters);
    hif::semantics::mapDeclarationsInTree(declarationCopy, originalDecl, declarationCopy, sem);

    // Temporary fix: replace should be not necessary.
    // Reference design: Built_In_FIR into step: bindOpenPortAssign.
    const bool canReplace = (originalDecl->getParent() != nullptr);
    if (canReplace)
        originalDecl->replace(declarationCopy);

    _simplifyTemplates(declarationCopy, sem, symbol);

    if (canReplace)
        declarationCopy->replace(originalDecl);

    // Add current configuration to the cache.
    _addCacheEntry(symbolCopy->templateParameterAssigns, declarationCopy, originalDecl, sem, typeDefCache);

    delete symbolCopy;
    return declarationCopy;
}
Function *_instantiate(
    FunctionCall *originalCall,
    hif::semantics::ILanguageSemantics *sem,
    const InstantiateOptions &opt = InstantiateOptions())
{
    return _instantiateSubprogram(originalCall, sem, opt);
}
SubProgram *_instantiate(
    ProcedureCall *originalCall,
    hif::semantics::ILanguageSemantics *sem,
    const InstantiateOptions &opt = InstantiateOptions())
{
    return _instantiateSubprogram(originalCall, sem, opt);
}

} // namespace

// ///////////////////////////////////////////////////////////////////
// Public utility Methods
// ///////////////////////////////////////////////////////////////////

void flushInstanceCache()
{
#ifdef HIF_DEBUG_CACHE
    clog << ">>>>>>>>>>>>>>>>>>>>>>>Flushing cache" << endl;
#endif
    viewCache.clear();
    subCache.clear();
    typeDefCache.clear();
    allInstantions.clear();

    trashCache.clear();
}

bool isInCache(Object *obj)
{
    //messageAssert((obj != nullptr), "Given nullptr as starting object", nullptr, nullptr);
    if (obj == nullptr)
        return false;
    if (allInstantions.find(static_cast<Declaration *>(obj)) != allInstantions.end())
        return true;

    Object *parent = obj;
    Object *tmp    = obj->getParent();
    while (tmp != nullptr) {
        parent = tmp;
        tmp    = tmp->getParent();
    }

    if (dynamic_cast<System *>(parent) != nullptr)
        return false; // is in tree

    Declaration *decl = dynamic_cast<Declaration *>(parent);
    if (decl == nullptr)
        return false; // cannot be in cache

    return (allInstantions.find(decl) != allInstantions.end());
}

void addInCache(Declaration *o) { trashCache.insert(o); }
// ///////////////////////////////////////////////////////////////////
// InstantiateOptions
// ///////////////////////////////////////////////////////////////////
InstantiateOptions::InstantiateOptions()
    : replace(false)
    , onlySignature(false)
    , candidate(nullptr)
{
    // ntd
}

InstantiateOptions::~InstantiateOptions()
{
    // ntd
}

InstantiateOptions::InstantiateOptions(const InstantiateOptions &o)
    : replace(o.replace)
    , onlySignature(o.onlySignature)
    , candidate(o.candidate)
{
    // ntd
}

InstantiateOptions &InstantiateOptions::operator=(const InstantiateOptions &o)
{
    if (&o == this)
        return *this;

    replace       = o.replace;
    onlySignature = o.onlySignature;
    candidate     = o.candidate;

    return *this;
}
// ///////////////////////////////////////////////////////////////////
// Instantiate Methods
// ///////////////////////////////////////////////////////////////////

View *instantiate(ViewReference *instance, hif::semantics::ILanguageSemantics *ref_sem, const InstantiateOptions &opt)
{
    View *ret = _instantiate(instance, ref_sem, opt);
    if (opt.replace)
        hif::semantics::setDeclaration(instance, ret);
    return ret;
}
SubProgram *
instantiate(ProcedureCall *originalCall, hif::semantics::ILanguageSemantics *ref_sem, const InstantiateOptions &opt)
{
    SubProgram *ret = _instantiate(originalCall, ref_sem, opt);
    if (opt.replace)
        hif::semantics::setDeclaration(originalCall, ret);
    return ret;
}
Function *
instantiate(FunctionCall *originalCall, hif::semantics::ILanguageSemantics *ref_sem, const InstantiateOptions &opt)
{
    Function *ret = _instantiate(originalCall, ref_sem, opt);
    if (opt.replace)
        hif::semantics::setDeclaration(originalCall, ret);
    return ret;
}

TypeReference::DeclarationType *
instantiate(TypeReference *instance, hif::semantics::ILanguageSemantics *ref_sem, const InstantiateOptions &opt)
{
    TypeTP *decl = dynamic_cast<TypeTP *>(hif::semantics::getDeclaration(instance, ref_sem));
    if (decl != nullptr)
        return decl;

    TypeDef *ret = nullptr;
    if (instance->getInstance() != nullptr) {
        Scope *instScope = instantiate(instance->getInstance(), ref_sem);
        TypeDef *currTR  = dynamic_cast<TypeDef *>(instScope);
        while (currTR != nullptr) {
            ReferencedType *t = dynamic_cast<ReferencedType *>(currTR->getType());
            messageAssert(t != nullptr, "Unexpected type (1)", currTR->getType(), ref_sem);
            instScope = instantiate(t, ref_sem);
            messageAssert(instScope != nullptr, "Cannot instantiate referneced type", t, ref_sem);
            currTR = dynamic_cast<TypeDef *>(instScope);
        }

        if (dynamic_cast<View *>(instScope) != nullptr) {
            View *v             = static_cast<View *>(instScope);
            TypeReference *copy = hif::copy(instance);
            delete copy->setInstance(nullptr);
            // First call to set declaration to something in the instantiated view.
            hif::semantics::DeclarationOptions dopt;
            dopt.location     = v->getContents();
            dopt.forceRefresh = true;
            hif::semantics::getDeclaration(copy, ref_sem, dopt);
            ret = _instantiate(copy, ref_sem);
            delete copy;
        } else if (dynamic_cast<LibraryDef *>(instScope) != nullptr) {
            ret = _instantiate(instance, ref_sem);
        } else {
            ret = _instantiate(instance, ref_sem);
        }
    } else {
        ret = _instantiate(instance, ref_sem);
    }

    if (opt.replace)
        hif::semantics::setDeclaration(instance, ret);
    return ret;
}
Scope *instantiate(ReferencedType *instance, hif::semantics::ILanguageSemantics *ref_sem, const InstantiateOptions &opt)
{
    if (dynamic_cast<TypeReference *>(instance) != nullptr) {
        return instantiate(static_cast<TypeReference *>(instance), ref_sem, opt);
    } else if (dynamic_cast<ViewReference *>(instance) != nullptr) {
        return instantiate(static_cast<ViewReference *>(instance), ref_sem, opt);
    } else if (dynamic_cast<Library *>(instance) != nullptr) {
        return hif::semantics::getDeclaration(static_cast<Library *>(instance), ref_sem);
    } else {
        messageError("Unexpected referenced type", instance, nullptr);
    }
}
Port *instantiate(PortAssign *pa, hif::semantics::ILanguageSemantics *ref_sem, const InstantiateOptions &opt)
{
    Instance *inst = dynamic_cast<Instance *>(pa->getParent());
    if (inst == nullptr)
        return nullptr;

    ViewReference *vr = dynamic_cast<ViewReference *>(inst->getReferencedType());
    if (vr == nullptr)
        return nullptr;

    View *v = _instantiate(vr, ref_sem, opt);
    if (v == nullptr)
        return nullptr;

    Entity *en = v->getEntity();

    Port *found = nullptr;
    for (BList<Port>::iterator i = en->ports.begin(); i != en->ports.end(); ++i) {
        if ((*i)->getName() != pa->getName())
            continue;
        found = *i;
        break;
    }

    Port *ret = found;
    if (opt.replace)
        hif::semantics::setDeclaration(pa, ret);

    return ret;
}
Instance::DeclarationType *
instantiate(Instance *instance, hif::semantics::ILanguageSemantics *ref_sem, const InstantiateOptions &opt)
{
    ViewReference *vr = dynamic_cast<ViewReference *>(instance->getReferencedType());
    if (vr == nullptr)
        return nullptr;

    View *v = _instantiate(vr, ref_sem, opt);
    if (v == nullptr)
        return nullptr;

    Entity *ret = v->getEntity();
    if (opt.replace)
        hif::semantics::setDeclaration(instance, ret);

    return ret;
}
Parameter *
instantiate(ParameterAssign *instance, hif::semantics::ILanguageSemantics *ref_sem, const InstantiateOptions &opt)
{
    FunctionCall *fcall  = dynamic_cast<FunctionCall *>(instance->getParent());
    ProcedureCall *pcall = dynamic_cast<ProcedureCall *>(instance->getParent());
    if (fcall == nullptr && pcall == nullptr)
        return nullptr;

    SubProgram *sub = nullptr;

    if (fcall != nullptr) {
        sub = _instantiate(fcall, ref_sem, opt);
    } else // ( pcall != nullptr )
    {
        sub = _instantiate(pcall, ref_sem, opt);
    }

    if (sub == nullptr)
        return nullptr;

    Parameter *found = nullptr;
    for (BList<Parameter>::iterator i = sub->parameters.begin(); i != sub->parameters.end(); ++i) {
        if ((*i)->getName() != instance->getName())
            continue;
        found = *i;
        break;
    }

    Parameter *ret = found;
    if (opt.replace)
        hif::semantics::setDeclaration(instance, ret);

    return ret;
}
FieldReference::DeclarationType *
instantiate(FieldReference *instance, hif::semantics::ILanguageSemantics *ref_sem, const InstantiateOptions &opt)
{
    FieldReference::DeclarationType *originalDecl = hif::semantics::getDeclaration(instance, ref_sem);
    if (originalDecl == nullptr)
        return nullptr;

    // in case of record fields, instantiate the prefix of the prefix (if any)
    // and then the prefix itself
    Field *ff = dynamic_cast<Field *>(originalDecl);
    if (ff != nullptr) {
        FieldReference *prefix = dynamic_cast<FieldReference *>(instance->getPrefix());
        if (prefix == nullptr)
            return nullptr;

        Declaration *prefixDecl = hif::semantics::getDeclaration(prefix, ref_sem);
        messageAssert(prefixDecl != nullptr, "Not found prefix declaration", prefix, ref_sem);

        DataDeclaration *instantiatedPrefixDecl = dynamic_cast<DataDeclaration *>(instantiate(prefix, ref_sem));
        if (instantiatedPrefixDecl == nullptr)
            return nullptr;

        View *prefixView = hif::getNearestParent<View>(instantiatedPrefixDecl);
        if (prefixView == nullptr)
            return nullptr;

        Type *prefixTypeDecl = hif::semantics::getBaseType(instantiatedPrefixDecl->getType(), false, ref_sem);
        Record *r            = dynamic_cast<Record *>(prefixTypeDecl);
        if (r == nullptr)
            return nullptr;

        // TODO: manage instantiation of template parameters of typedefs
        View *originalPrefixView = hif::getNearestParent<View>(prefixDecl);
        if (originalPrefixView == nullptr)
            return nullptr;

        if (hif::isSubNode(r, originalPrefixView)) {
            Object *rdecl = hif::manipulation::matchObject(r, originalPrefixView, prefixView, ref_sem);
            r             = dynamic_cast<Record *>(rdecl);
            if (r == nullptr)
                return nullptr;
        }

        for (BList<Field>::iterator iter = r->fields.begin(); iter != r->fields.end(); ++iter) {
            if ((*iter)->getName() == ff->getName())
                return (*iter);
        }
        return nullptr;
    }

    // in case of viewrefs, instantiate the view
    View *originalView = hif::getNearestParent<View>(originalDecl);
    if (originalView == nullptr)
        return nullptr;

    ViewReference *vr = dynamic_cast<ViewReference *>(hif::semantics::getSemanticType(instance->getPrefix(), ref_sem));
    if (vr == nullptr)
        return nullptr;

    View *v = _instantiate(vr, ref_sem);
    if (v == nullptr)
        return nullptr;

    Object *decl = hif::manipulation::matchObject(originalDecl, originalView, v, ref_sem);

    FieldReference::DeclarationType *ret = dynamic_cast<FieldReference::DeclarationType *>(decl);
    hif::semantics::mapDeclarationsInTree(ret, v, originalView, ref_sem);

    // TODO: manage instantiation of template parameters of typedefs
    //       (when the prefix is a typeref to a record)

    if (opt.replace)
        hif::semantics::setDeclaration(instance, ret);
    return ret;
}
ValueTPAssign::DeclarationType *
instantiate(ValueTPAssign *instance, hif::semantics::ILanguageSemantics *ref_sem, const InstantiateOptions &opt)
{
    // checks possible parents
    FunctionCall *fcall  = dynamic_cast<FunctionCall *>(instance->getParent());
    ProcedureCall *pcall = dynamic_cast<ProcedureCall *>(instance->getParent());
    TypeReference *tr    = dynamic_cast<TypeReference *>(instance->getParent());
    ViewReference *vr    = dynamic_cast<ViewReference *>(instance->getParent());

    if (fcall == nullptr && pcall == nullptr && tr == nullptr && vr == nullptr)
        return nullptr;

    BList<Declaration> *declList = nullptr;
    SubProgram *sub              = nullptr;
    if (fcall != nullptr) {
        sub = _instantiate(fcall, ref_sem, opt);
        if (sub == nullptr)
            return nullptr;

        declList = &sub->templateParameters;
    } else if (pcall != nullptr) {
        sub = _instantiate(pcall, ref_sem, opt);
        if (sub == nullptr)
            return nullptr;

        declList = &sub->templateParameters;
    } else if (tr != nullptr) {
        TypeDef *d = _instantiate(tr, ref_sem);
        if (d == nullptr)
            return nullptr;

        declList = &d->templateParameters;
    } else // if ( vr != nullptr )
    {
        ViewReference::DeclarationType *d = _instantiate(vr, ref_sem, opt);
        if (d == nullptr)
            return nullptr;

        declList = &d->templateParameters;
    }

    Declaration *found = nullptr;
    for (BList<Declaration>::iterator i = declList->begin(); i != declList->end(); ++i) {
        if ((*i)->getName() != instance->getName())
            continue;
        found = *i;
        break;
    }

    ValueTPAssign::DeclarationType *ret = dynamic_cast<ValueTPAssign::DeclarationType *>(found);
    if (opt.replace)
        hif::semantics::setDeclaration(instance, ret);

    return ret;
}
TypeTPAssign::DeclarationType *
instantiate(TypeTPAssign *instance, hif::semantics::ILanguageSemantics *ref_sem, const InstantiateOptions &opt)
{
    // checks possible parents
    FunctionCall *fcall  = dynamic_cast<FunctionCall *>(instance->getParent());
    ProcedureCall *pcall = dynamic_cast<ProcedureCall *>(instance->getParent());
    TypeReference *tr    = dynamic_cast<TypeReference *>(instance->getParent());
    ViewReference *vr    = dynamic_cast<ViewReference *>(instance->getParent());

    if (fcall == nullptr && pcall == nullptr && tr == nullptr && vr == nullptr)
        return nullptr;

    BList<Declaration> *declList = nullptr;
    if (fcall != nullptr) {
        FunctionCall::DeclarationType *d = _instantiate(fcall, ref_sem, opt);
        if (d == nullptr)
            return nullptr;

        declList = &d->templateParameters;
    } else if (pcall != nullptr) {
        ProcedureCall::DeclarationType *d = _instantiate(pcall, ref_sem, opt);
        if (d == nullptr)
            return nullptr;

        declList = &d->templateParameters;
    } else if (tr != nullptr) {
        TypeDef *d = _instantiate(tr, ref_sem);
        if (d == nullptr)
            return nullptr;

        declList = &d->templateParameters;
    } else // if ( vr != nullptr )
    {
        ViewReference::DeclarationType *d = _instantiate(vr, ref_sem, opt);
        if (d == nullptr)
            return nullptr;

        declList = &d->templateParameters;
    }
    Declaration *found = nullptr;
    for (BList<Declaration>::iterator i = declList->begin(); i != declList->end(); ++i) {
        if ((*i)->getName() != instance->getName())
            continue;
        found = *i;
        break;
    }

    TypeTPAssign::DeclarationType *ret = dynamic_cast<TypeTPAssign::DeclarationType *>(found);
    if (opt.replace)
        hif::semantics::setDeclaration(instance, ret);

    return ret;
}

} // namespace manipulation
} // namespace hif
