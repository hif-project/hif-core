/// @file mapStandardSymbols.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif.hpp"
#include "hif/semantics/standardization.hpp"

namespace hif
{
namespace semantics
{

namespace /* anon */
{

// getAllReferences
typedef std::set<Object *> References;
typedef std::map<Declaration *, References> ReferenceMap;
ReferenceMap referenceMap;

// fix structures
typedef std::set<Declaration *> DeclarationToMap;
typedef std::list<Object *> SymbolsToCheck;
typedef std::set<Object *> SymbolsAlreadyChecked;
typedef std::map<Object *, std::string> OldNames;
DeclarationToMap declarationToMap;
SymbolsToCheck symbolsToCheck;
SymbolsAlreadyChecked symbolsAlreadyChecked;
OldNames oldNames;

// trash structures
hif::Trash trash;

ILanguageSemantics *_srcSem  = nullptr;
ILanguageSemantics *_destSem = nullptr;
/// Insert the declaration/object inside trash. Before that, mark all inner
/// symbols as already fixed.
void _trash(Object *o)
{
    std::list<Object *> symbols;
    hif::semantics::collectSymbols(symbols, o);
    for (std::list<Object *>::iterator i(symbols.begin()); i != symbols.end(); ++i) {
        symbolsAlreadyChecked.insert(*i);
    }
    trash.insert(o);
}
// /////////////////////////////////////////////////////////////////////////////
// step2
// /////////////////////////////////////////////////////////////////////////////

void _doStandardLibraryFix(LibraryDef *decl, System *root)
{
    ILanguageSemantics::KeySymbol key = std::make_pair(decl->getName(), decl->getName());
    ILanguageSemantics::ValueSymbol value;

    const ILanguageSemantics::MapCases mapped = _destSem->mapStandardSymbol(decl, key, value, _srcSem);
    switch (mapped) {
    case ILanguageSemantics::UNKNOWN:
        // Note: this is a different behavior. Libraries MUST be mapped.
        // Declarations could be mapped or not...
        messageError("Found a standard library without a known mapping", decl, _destSem);
    case ILanguageSemantics::UNSUPPORTED:
        messageError("Found an unsupported standard library", decl, _destSem);
    case ILanguageSemantics::SIMPLIFIED:
        // Library implementation is no longer needed.
        //            __trash(decl); // maybe nested
        return;
    case ILanguageSemantics::MAP_KEEP:
    case ILanguageSemantics::MAP_DELETE:
        // Proceed ...
        break;
    default:
        messageError("Unexpected case", nullptr, nullptr);
    }

    // insert new implementations
    bool keepOldImplementation = false;
    for (ILanguageSemantics::LibraryList::iterator it(value.libraries.begin()); it != value.libraries.end(); ++it) {
        // A mapped library (one or more, actually) is searched for the LibraryDef declaration.
        // If all library implementations are found in the destination semantics,
        // the old implementation is removed from the tree since all its constructs
        // are mapped into the new ones.
        // Otherwise, the old implementation is kept in the tree as is, since some
        // constructs may be not known by the destination semantics.
        //
        // e.g. mapping from VHDL to SystemC
        // library: hif_standard, constructs: hif_event, hif_severity_level
        //
        // {hif_standard} |-> {hif_standard, sc_core}
        //   {hif_event} |-> {sc_core::event}
        //   {hif_severity_level} |-> {hif_standard::hif_severity_level}
        //
        // hif_standard::hif_event is known to be mapped to sc_core::event, thus
        // its declaration is modified and the implementation of sc_core is given
        // by SystemCSemantics and added to the tree
        // hif_standard::hif_severity_level is not known to SystemCSemantics, thus
        // it is kept as is and the old implementation is maintained

        // In case of recursive call to mapStandardSymbols(), this avoids errors.
        // In fact a standard lib could be yet a standard lib in dst sem!
        // So we have to manage only src std libs.

        //if (!_srcSem->isNativeLibrary(*it)) continue;
        LibraryDef *dstDecl = _destSem->getStandardLibrary(*it);

        if (dstDecl != nullptr) {
            hif::manipulation::AddUniqueObjectOptions addOpt;
            addOpt.equalsOptions.checkOnlyNames = true;
            addOpt.position                     = 0;
            hif::manipulation::addUniqueObject(dstDecl, root->libraryDefs, addOpt);
        } else {
            keepOldImplementation = true;
        }
    }

    messageAssert(
        ((keepOldImplementation && mapped == ILanguageSemantics::MAP_KEEP) ||
         (!keepOldImplementation && mapped == ILanguageSemantics::MAP_DELETE)),
        "Unconsistent implementation", decl, _destSem);

    // store old name
    oldNames[decl] = decl->getName();

    if (mapped == ILanguageSemantics::MAP_KEEP) {
        decl->setName(value.mappedSymbol);
    } else {
        // will be deleted later...
        _trash(decl);
    }

    for (References::iterator it(referenceMap[decl].begin()); it != referenceMap[decl].end(); ++it) {
        // always rename, eventually delete if the reference is used as include
        objectSetName(*it, value.mappedSymbol);

        if (!(*it)->isInBList())
            continue;
        if (reinterpret_cast<BList<Library> *>((*it)->getBList()) != objectGetLibraryList((*it)->getParent())) {
            continue;
        }

        if (dynamic_cast<LibraryDef *>((*it)->getParent()) != nullptr &&
            static_cast<LibraryDef *>((*it)->getParent())->isStandard())
            continue;

        // these two conditions identify a peculiar inclusion which must be preserved
        Library *lib = dynamic_cast<Library *>(*it);
        if (lib->isStandard() || lib->getFilename() != "")
            continue;

        _trash(*it);
        BList<Library>::iterator l(lib);
        l.remove();

        // will be fixed by further visits
    }
}

void _collectInnerDeclarations(LibraryDef *container, System *root)
{
    //    // collect library references
    //    SymbolsToCheck tmpL(referenceMap[container].begin(), referenceMap[container].end());
    //    symbolsToCheck.splice(symbolsToCheck.end(), tmpL, tmpL.begin(), tmpL.end());

    // collect inner declaration references
    for (BList<Declaration>::iterator i(container->declarations.begin()); i != container->declarations.end(); ++i) {
        if (dynamic_cast<LibraryDef *>(*i) != nullptr) {
            messageError("Unsupported", *i, nullptr);
        } else if (dynamic_cast<TypeDef *>(*i) != nullptr) {
            TypeDef *td = static_cast<TypeDef *>(*i);
            Enum *en    = dynamic_cast<Enum *>(td->getType());
            if (en != nullptr) {
                for (BList<EnumValue>::iterator ev(en->values.begin()); ev != en->values.end(); ++ev) {
                    declarationToMap.insert(*ev);
                    SymbolsToCheck tmp(referenceMap[*ev].begin(), referenceMap[*ev].end());
                    symbolsToCheck.splice(symbolsToCheck.end(), tmp, tmp.begin(), tmp.end());
                }
            }
        } else if (dynamic_cast<DesignUnit *>(*i) != nullptr) {
            DesignUnit *du = static_cast<DesignUnit *>(*i);
            messageAssert(du->views.size() == 1, "Unexpected number of views", du, _destSem);

            View *v = du->views.front();
            declarationToMap.insert(v);
            SymbolsToCheck tmp(referenceMap[v].begin(), referenceMap[v].end());
            symbolsToCheck.splice(symbolsToCheck.end(), tmp, tmp.begin(), tmp.end());
        }

        declarationToMap.insert(*i);
        SymbolsToCheck tmp(referenceMap[*i].begin(), referenceMap[*i].end());
        symbolsToCheck.splice(symbolsToCheck.end(), tmp, tmp.begin(), tmp.end());
    }

    // map library
    _doStandardLibraryFix(container, root);
}
void _fixStandardLibraries(System *root)
{
    for (BList<LibraryDef>::iterator it(root->libraryDefs.begin()); it != root->libraryDefs.end(); ++it) {
        if (!(*it)->isStandard())
            continue;
        if (_srcSem->getStandardLibrary((*it)->getName()) == nullptr)
            continue;

        _collectInnerDeclarations(*it, root);
    }
}
// /////////////////////////////////////////////////////////////////////////////
// step3
// /////////////////////////////////////////////////////////////////////////////

void _mapPrefix(Object *sym, ILanguageSemantics::LibraryList &libraries)
{
    std::string n = libraries.front();

    Object *prefix = objectGetInstance(sym);
    if (prefix == nullptr)
        return;

    if (dynamic_cast<Library *>(prefix) != nullptr) {
        Library *l = static_cast<Library *>(prefix);
        l->setStandard(_destSem->isStandardInclusion(n, false));
        l->setName(n);
    } else if (dynamic_cast<Instance *>(prefix) != nullptr) {
        Instance *i = static_cast<Instance *>(prefix);
        Library *l  = dynamic_cast<Library *>(i->getReferencedType());
        //ViewReference * vr = dynamic_cast< ViewReference* >( i->getReferencedType() );
        if (l != nullptr) {
            l->setName(n);
            l->setStandard(_destSem->isStandardInclusion(n, false));
        }
    }
}

std::string _getOldStandardName(Declaration *decl)
{
    LibraryDef *ld = hif::getNearestParent<LibraryDef>(decl);
    messageAssert(ld != nullptr, "Expected library def container", decl, _destSem);

    OldNames::iterator i = oldNames.find(ld);
    messageAssert(i != oldNames.end(), "Unable to find mapping of name", ld, _destSem);

    return i->second;
}

void _mapEnumValue(Object *sym, EnumValue *decl)
{
    std::string stdName = _getOldStandardName(decl);

    std::string declName = decl->getName();
    OldNames::iterator declIt(oldNames.find(decl));
    if (declIt != oldNames.end())
        declName = declIt->second;

    ILanguageSemantics::KeySymbol key = std::make_pair(stdName, declName);
    ILanguageSemantics::ValueSymbol value;

    const ILanguageSemantics::MapCases mapped = _destSem->mapStandardSymbol(decl, key, value, _srcSem);
    switch (mapped) {
    case ILanguageSemantics::UNKNOWN:
        // Keep it as is ...
        return;
    case ILanguageSemantics::UNSUPPORTED:
        messageError("Found an unsupported declaration", decl, _destSem);
    case ILanguageSemantics::SIMPLIFIED:
        // This implementation is no longer needed.
        //            __trash(decl);
        return;
    case ILanguageSemantics::MAP_KEEP:
    case ILanguageSemantics::MAP_DELETE:
        // Proceed ... (trash is decided later)
        break;
    default:
        messageError("Unexpected case", nullptr, nullptr);
    }

    // mapping names of references
    objectSetName(sym, value.mappedSymbol);

    // referenceMap eventual scope AA::BB::*it
    _mapPrefix(sym, value.libraries);

    if (mapped == ILanguageSemantics::MAP_KEEP) {
        if (declIt == oldNames.end())
            oldNames[decl] = decl->getName();
        decl->setName(value.mappedSymbol);
    } else {
        // will be deleted later...
        _trash(decl);
    }
}

void _mapDesignUnit(Object *sym, DesignUnit *decl)
{
    std::string stdName = _getOldStandardName(decl);

    std::string declName = decl->getName();
    OldNames::iterator declIt(oldNames.find(decl));
    if (declIt != oldNames.end())
        declName = declIt->second;

    ILanguageSemantics::KeySymbol key = std::make_pair(stdName, declName);
    ILanguageSemantics::ValueSymbol value;

    const ILanguageSemantics::MapCases mapped = _destSem->mapStandardSymbol(decl, key, value, _srcSem);
    bool simplified                           = false;
    switch (mapped) {
    case ILanguageSemantics::UNKNOWN:
        // Keep it as is ...
        value.libraries.push_back(stdName);
        value.mapAction    = ILanguageSemantics::UNKNOWN;
        value.mappedSymbol = declName;
        break;
    case ILanguageSemantics::UNSUPPORTED:
        messageError("Found an unsupported declaration", decl, _destSem);
    case ILanguageSemantics::SIMPLIFIED:
        // This implementation is no longer needed.
        //simplified = true;
        //            __trash(du);
        messageError("WTF", decl, _destSem);
    case ILanguageSemantics::MAP_KEEP:
    case ILanguageSemantics::MAP_DELETE:
        // Proceed ... (trash is decided later)
        break;
    default:
        messageError("Unexpected case", nullptr, nullptr);
    }

    // mapping names of references
    if (simplified) {
        Object *o = _destSem->getSimplifiedSymbol(key, sym);
        sym->replace(o);
        _trash(sym);
    } else {
        // standard case
        if (dynamic_cast<ViewReference *>(sym) != nullptr) {
            static_cast<ViewReference *>(sym)->setDesignUnit(value.mappedSymbol);
        } else {
            objectSetName(sym, value.mappedSymbol);
        }

        // referenceMap eventual scope AA::BB::*it
        _mapPrefix(sym, value.libraries);

        // adding includes (avoid contents)
        Scope *scope = hif::getNearestScope(sym, false, true, false);
        if (dynamic_cast<Contents *>(scope) != nullptr)
            scope = dynamic_cast<Scope *>(scope->getParent());
        for (ILanguageSemantics::LibraryList::iterator jt(value.libraries.begin()); jt != value.libraries.end(); ++jt) {
            Library *inc = new Library();
            inc->setName(*jt);
            inc->setSystem(true);
            inc->setFilename(_destSem->mapStandardFilename(*jt));
            inc->setStandard(_destSem->isStandardInclusion(*jt, true));

            hif::manipulation::AddUniqueObjectOptions addOpt;
            addOpt.deleteIfNotAdded = true;
            hif::manipulation::addUniqueObject(inc, scope, addOpt);
        }
    }

    if (mapped == ILanguageSemantics::MAP_KEEP) {
        if (declIt == oldNames.end())
            oldNames[decl] = decl->getName();
        decl->setName(value.mappedSymbol);
    } else if (mapped == ILanguageSemantics::MAP_DELETE) {
        // will be deleted later...
        _trash(decl);
    }
}

void _mapDeclaration(Object *sym, Declaration *decl)
{
    std::string stdName = _getOldStandardName(decl);

    std::string declName = decl->getName();
    OldNames::iterator declIt(oldNames.find(decl));
    if (declIt != oldNames.end())
        declName = declIt->second;

    ILanguageSemantics::KeySymbol key = std::make_pair(stdName, declName);
    ILanguageSemantics::ValueSymbol value;

    const ILanguageSemantics::MapCases mapped = _destSem->mapStandardSymbol(decl, key, value, _srcSem);
    bool simplified                           = false;
    switch (mapped) {
    case ILanguageSemantics::UNKNOWN:
        // Keep it as is ...
        value.libraries.push_back(stdName);
        value.mapAction    = ILanguageSemantics::UNKNOWN;
        value.mappedSymbol = declName;
        break;
    case ILanguageSemantics::UNSUPPORTED:
        messageError("Found an unsupported declaration", decl, _destSem);
    case ILanguageSemantics::SIMPLIFIED:
        // This implementation is no longer needed.
        simplified = true;
        //            __trash(decl);
        break;
    case ILanguageSemantics::MAP_KEEP:
    case ILanguageSemantics::MAP_DELETE:
        // Proceed ... (trash is decided later)
        break;
    default:
        messageError("Unexpected case", nullptr, nullptr);
    }

    Scope *scope = hif::getNearestScope(sym, false, true, false);
    if (dynamic_cast<Contents *>(scope) != nullptr)
        scope = dynamic_cast<Scope *>(scope->getParent());
    messageDebugAssert(scope != nullptr, "Nearest scope not found", sym, _destSem);

    // mapping names of references
    if (simplified) {
        Object *o = _destSem->getSimplifiedSymbol(key, sym);
        if (dynamic_cast<Value *>(o) != nullptr) {
            Value *v = static_cast<Value *>(o);
            Type *t  = hif::semantics::getSemanticType(static_cast<Value *>(sym));
            messageAssert(t != nullptr, "Cannot type original value", o, _destSem);

            Cast *c = new Cast();
            c->setType(hif::copy(t));
            c->setValue(v);
            sym->replace(c);

            hif::semantics::collectSymbols(symbolsToCheck, c);
        } else {
            sym->replace(o);
            hif::semantics::collectSymbols(symbolsToCheck, o);
        }
        _trash(sym);
    } else {
        // standard case
        objectSetName(sym, value.mappedSymbol);
        // referenceMap eventual scope AA::BB::*it
        _mapPrefix(sym, value.libraries);
    }

    // adding includes (avoid contents)
    for (ILanguageSemantics::LibraryList::iterator jt(value.libraries.begin()); jt != value.libraries.end(); ++jt) {
        Library *inc = new Library();
        inc->setName(*jt);
        inc->setSystem(true);
        inc->setFilename(_destSem->mapStandardFilename(*jt));
        inc->setStandard(_destSem->isStandardInclusion(*jt, true));

        hif::manipulation::AddUniqueObjectOptions addOpt;
        addOpt.deleteIfNotAdded = true;
        hif::manipulation::addUniqueObject(inc, scope, addOpt);
    }

    if (mapped == ILanguageSemantics::MAP_KEEP) {
        if (declIt == oldNames.end())
            oldNames[decl] = decl->getName();
        decl->setName(value.mappedSymbol);
    } else if (mapped == ILanguageSemantics::MAP_DELETE) {
        // will be deleted later...
        _trash(decl);
    }
}

void _mapSymbol(Object *sym, Declaration *decl)
{
    if (dynamic_cast<EnumValue *>(decl) != nullptr) {
        _mapEnumValue(sym, static_cast<EnumValue *>(decl));
    } else if (dynamic_cast<View *>(decl) != nullptr) {
        _mapDesignUnit(sym, dynamic_cast<DesignUnit *>(decl->getParent()));
    } else {
        _mapDeclaration(sym, decl);
    }
}

void _fixSymbols()
{
    for (SymbolsToCheck::iterator it(symbolsToCheck.begin()); it != symbolsToCheck.end(); ++it) {
        Object *sym = *it;

        // We are interested in instance inner symbols only.
        if (dynamic_cast<Instance *>(sym) != nullptr)
            continue;

        if (symbolsAlreadyChecked.find(sym) != symbolsAlreadyChecked.end())
            continue;

        Declaration *decl = hif::semantics::getDeclaration(sym, _destSem);
        messageAssert(decl != nullptr, "Expected declaration", sym, _destSem);

        if (declarationToMap.find(decl) == declarationToMap.end())
            continue;

        _mapSymbol(sym, decl);
        symbolsAlreadyChecked.insert(sym);
    }
}
} // namespace
// /////////////////////////////////////////////////////////////////////////////
// mapStandardSymbols
// /////////////////////////////////////////////////////////////////////////////

void mapStandardSymbols(Object *o, ILanguageSemantics *srcSem, ILanguageSemantics *destSem)
{
    hif::application_utils::initializeLogHeader("HIF", "mapStandardSymbols");

    System *root = dynamic_cast<System *>(o);
    if (root == nullptr)
        root = getNearestParent<System>(o);
    messageAssert((root != nullptr), "Unable to find tree root", o, destSem);

    _srcSem  = srcSem;
    _destSem = destSem;

    // step 1
    hif::semantics::GetReferencesOptions opt;
    opt.include_unreferenced = true;
    opt.error               = true;
    getAllReferences(referenceMap, destSem, root, opt);

    // step 2
    _fixStandardLibraries(root);

    // step 3
    _fixSymbols();

    trash.clear();

    resetDeclarations(root);
    resetTypes(root, true);
    hif::manipulation::flushInstanceCache();
    hif::semantics::flushTypeCacheEntries();

    hif::application_utils::restoreLogHeader();
}

} // namespace semantics
} // namespace hif
