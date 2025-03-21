/// @file declarationUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <iostream>
#include <list>
#include <queue>

#include "hif/BiVisitor.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/application_utils/application_utils.hpp"
#include "hif/hifIOUtils.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/declarationUtils.hpp"
#include "hif/semantics/semantics.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#    if __GNUC__ >= 5
#        pragma GCC diagnostic ignored "-Wduplicated-cond"
#    endif
#endif

namespace hif
{
namespace semantics
{

namespace
{ // Unnamed

// ///////////////////////////////////////////////////////////////////
// CandidateInfo utility struct
// ///////////////////////////////////////////////////////////////////

///@brief Structure used from the algorithm for get the best candidates.
///
struct CandidateInfo {
    unsigned int score;
    Declaration *decl;

    CandidateInfo();
    ~CandidateInfo();
    CandidateInfo(const CandidateInfo &c);
    CandidateInfo &operator=(const CandidateInfo &c);

    bool operator<(const CandidateInfo &c) const;
};

CandidateInfo::CandidateInfo()
    : score(0)
    , decl(nullptr)
{
    // ntd
}

CandidateInfo::~CandidateInfo()
{
    // ntd
}

CandidateInfo::CandidateInfo(const CandidateInfo &c)
    : score(c.score)
    , decl(c.decl)
{
    // ntd
}

CandidateInfo &CandidateInfo::operator=(const CandidateInfo &c)
{
    if (this == &c)
        return *this;
    score = c.score;
    decl  = c.decl;
    return *this;
}

bool CandidateInfo::operator<(const CandidateInfo &c) const
{
    // <= is necessary to keep found declarations order.
    return score <= c.score;
}

// /////////////////////////////////////////////////////////////////////////
// typedefs and structures. global variables and forward declaration
// /////////////////////////////////////////////////////////////////////////

typedef std::set<Declaration *> DeclarationSet;
typedef std::list<Declaration *> Declarations;
typedef std::priority_queue<CandidateInfo> CandidateInfoQueue;

// /////////////////////////////////////////////////////////////////////////
// Global variables and forward declaration
// /////////////////////////////////////////////////////////////////////////

DeclarationSet foundDeclarations;

// /////////////////////////////////////////////////////////////////////////
// Utility functions
// /////////////////////////////////////////////////////////////////////////

/// @brief Remove a declaration from global list.
/// @param d The declaration to remove.
///
bool removeDeclaration(Declaration *d)
{
    if (foundDeclarations.find(d) == foundDeclarations.end()) {
        foundDeclarations.insert(d);
        return false;
    } else {
        //messageDebugAssert(false, "Collected declaration more than one time", d, nullptr);
    }
    return true;
}

// /////////////////////////////////////////////////////////////////////////
// Declaration Map visitor
// /////////////////////////////////////////////////////////////////////////

/// @brief This map implement the searching of declaration going up into
/// the scopes.
class InternalDeclarationVisitor : public MonoVisitor<InternalDeclarationVisitor>
{
public:
    InternalDeclarationVisitor();
    virtual ~InternalDeclarationVisitor();

    struct InternalData {
        /// @brief The name to search
        std::string index;

        /// @brief The location from which the declaration must be visible
        Object *location;

        /// @brief The reference semantics, this is used for searching of
        /// native signature
        ILanguageSemantics *sem;

        /// @brief The declaration found
        Declarations resultDeclarations;

        /// @brief The previous visited object
        Object *previous;

        /// @brief The previous visited object
        Object *startingObject;

        /// @brief Check if at least one candidate declaration must be returned.
        bool isMandatory;

        /// @brief Set to true when declaration can be safely not found.
        /// For example, declaration of Instance with Library as referenced type cannot
        /// have declaration.
        /// @note This field is used in check of error.
        bool allowNotFound;

        /// @brief True if is a recursive call.
        bool isRecursive;

        /// @brief The declaration options.
        GetCandidatesOptions dopt;

        /// @brief Store if starting object is overloadable
        bool _isOverloadable;

        /// @brief Store if object need a search in all tree
        bool _searchAll;

        /// @brief If true the declarations are searched only in classes.
        bool _checkOnlyClasses;

        /// @brief True while searching into used libraries.
        bool _isInLibrary;

        /// @brief True if the search in started from search into inheritance.
        bool _isInInheritance;

        /// @brief True if the search must be performed only into standard
        /// library defs.
        bool _checkOnlyStandardLibraryDefs;

        /// @brief Default constructor.
        InternalData();

        /// @brief Destructor.
        ~InternalData();

        /// @brief Copy constructor.
        InternalData(const InternalData &other);

        /// @brief Override of operator =
        InternalData &operator=(const InternalData &other);
    };

    Declarations findDeclarations(
        const std::string &index,
        Object *location,
        Object *startingObject,
        bool &allowNotFound,
        ILanguageSemantics *refSem,
        const GetCandidatesOptions &dopt,
        const bool isRecursive = false);
    /// @brief Base Map: call the map recursively on the parent object.
    void map(Object *obj);

    /// @name Standard cases
    /// @brief in each map is implemented a search on declaration lists of
    /// map object by calling of opportune functions. At end of each map
    /// there is a recursive call to the parent object checked by a guard.
    ///
    /// @{

    void map(Contents *obj);
    void map(DesignUnit *obj);
    void map(Entity *obj);
    void map(For *obj);
    void map(ForGenerate *obj);
    void map(IfGenerate *obj);
    void map(LibraryDef *obj);
    void map(Record *obj);
    void map(StateTable *obj);
    void map(SubProgram *obj);
    void map(System *obj);
    void map(TypeDef *obj);
    void map(View *obj);

    /// @}
    /// @name Particular cases
    /// @brief maps with special management of searching declaration.
    /// @{

    /// @brief Manage search of type reference set as syntactic type of
    /// enum value.
    void map(EnumValue *obj);

    /// @}
    /// @name Maps with special management for starting object.
    /// @note If it is not this case but we reached this map by another call
    /// of the map, it is only called map on the parent object.
    /// @{

    void map(FieldReference *obj);
    void map(FunctionCall *obj);
    void map(Instance *obj);
    void map(Library *obj);
    void map(ParameterAssign *obj);
    void map(PortAssign *obj);
    void map(ProcedureCall *obj);
    void map(TPAssign *obj);
    void map(TypeReference *obj);
    void map(ViewReference *obj);

    /// @}

private:
    /// @brief contains all data
    InternalData _data;

    /// @brief Returns true if symbols required a special management.
    bool _isPrefixedSymbol(Object *o);

    /// @brief Perform the correct calling of the map.
    void _searchInParent(Object *obj);

    /// @name Starting object management methods.
    /// @{

    /// @brief Manage search of field refs. Instance fields must be searhced
    /// only into classes (view) referred by the type of the prefix
    /// parameter. Otherwise the field declaration is searched normally.
    /// @param obj The field reference obj.
    void _manageFieldReference(FieldReference *obj);

    /// @brief Manage search of methods. Instance methods must be searched
    /// only into classes (view) referred by the type of the instance
    /// parameter. Otherwise the method declaration is searched normally.
    /// @param obj The function call.
    ///
    void _manageFunctionCall(FunctionCall *obj);

    /// @brief Manage search of entities. The declaration of relative
    /// viewref is searched and then its entity is returned.
    /// @param obj The instance.
    ///
    void _manageInstance(Instance *obj);

    /// @brief Manage search of library definition referenced by
    /// Library <tt>obj</tt> passed as parameter. If Library has instance
    /// field set, it call the search recursively inside instanced object.
    ///
    void _manageLibrary(Library *obj);

    /// @brief Manage search of the parameter declaration of a Parameter assign.
    /// First of all get the declaration of parent function call or procedure
    /// call and than call the map on returned subprogram declaration.
    /// @param obj The ParameterAssign.
    ///
    void _manageParameterAssign(ParameterAssign *obj);

    /// @brief Manage search of the port declaration of a PortAssign. It get
    /// the corresponding instance and search the port inside view indicated
    /// by the viewref.
    /// @param obj The PortAssign.
    ///
    void _managePortAssign(PortAssign *obj);

    /// @brief Manage search of methods. Instance methods must be searched
    /// only into classes (view) referred by the type of the instance
    /// parameter. Otherwise the method declaration is searched normally.
    /// @param obj The procedure call.
    ///
    void _manageProcedureCall(ProcedureCall *obj);

    /// @brief Manage search of the template parameter declaration of a
    /// TPAssign of a viewref. In this case it get the corresponding
    /// view and search template parameter inside it list.
    /// @param obj The TPAssign.
    ///
    void _manageTPAssign(TPAssign *obj);

    /// @brief Manage search of the type reference declaration inside classes.
    /// E.g. vector< int >::size_type.
    /// @param obj The TypeReference.
    ///
    void _manageTypeReference(TypeReference *obj);

    /// @brief Manage search of view referenced by ViewReference <tt>obj</tt>
    /// passed as parameter. If ViewReference has instance field set, it call
    /// the search recursively inside instanced object.
    /// In standard search it call get declaration of its unit name, and
    /// after that search into the lists of found view.
    ///
    void _manageViewReferece(ViewReference *obj);

    /// @}
    /// @name Utility methods
    /// @{

    /// @brief This function search the index declaration inside the list
    /// passed as parameter. Inside function is checked if we are already
    /// arriving from the list and, in this case, it search declaration only
    /// on precedent elements.
    /// @param list The list of declaration to check.
    ///
    template <typename T> void _getDeclarationInList(BList<T> &list);

    /// @brief This function search the index declaration inside all the
    /// library definitions of list of library passed as parameter.
    /// First of all it get the declarations of each library and after that
    /// it is done a search inside it.
    /// @param list The list of library on which search declaration.
    ///
    void _getDeclarationInLibraries(BList<Library> &list);

    /// @brief This function search the index declaration inside port list
    /// of the given entity.
    /// @param entity The entity on which search declaration.
    ///
    void _getDeclarationInEntity(Entity *entity);

    /// @brief This function set an internal flag used to know if the starting
    /// object is overloadable. Object is overloadable if it is a function
    /// or procedure call.
    ///
    void _setOverloadable();

    /// @brief This function set an internal flag used to know if the search
    /// must be done always on an entire list.
    ///
    void _setSearchAll();

    /// @brief This function is used to compare declaration name with the
    /// index name. This is necessary to manage TypeDef of enumerations;
    /// In this case the declaration is searched also on each enum value.
    /// @param decl The declaration to check.
    ///
    void _checkDeclaration(Declaration *decl);

    /// @brief Given the instance, gets the declaration of a instance method.
    /// @param instance The instance.
    /// @return true if declaration must be searched into standard library defs.
    ///
    bool _getMethodDeclaration(Value *instance);

    /// @brief Search the declaration in the list of parent classes of given
    /// view.
    /// @param view The current view.
    ///
    void _getDeclarationInInheritance(View *view);

    /// @brief Given a TypeReference returns the Record to which the TypeReference
    /// refers to.
    /// @param prefixType The type to check.
    /// @return The record.
    ///
    Record *_getRecordOfTyperef(Type *prefixType);

    /// @}

    void _fillDopt(GetCandidatesOptions &to, const GetCandidatesOptions &from);
};

// ///////////////////////////////////////////////////////////////////
// InternalDeclarationVisitor InternalData
// ///////////////////////////////////////////////////////////////////
InternalDeclarationVisitor::InternalData::InternalData()
    : index()
    , location(nullptr)
    , sem(nullptr)
    , resultDeclarations()
    , previous(nullptr)
    , startingObject(nullptr)
    , isMandatory(false)
    , allowNotFound(false)
    , isRecursive(false)
    , dopt()
    , _isOverloadable(false)
    , _searchAll(false)
    , _checkOnlyClasses(false)
    , _isInLibrary(false)
    , _isInInheritance(false)
    , _checkOnlyStandardLibraryDefs(false)
{
    //ntd
}

InternalDeclarationVisitor::InternalData::~InternalData()
{
    //ntd
}

InternalDeclarationVisitor::InternalData::InternalData(const InternalData &other)
    : index(other.index)
    , location(other.location)
    , sem(other.sem)
    , resultDeclarations(other.resultDeclarations)
    , previous(other.previous)
    , startingObject(other.startingObject)
    , isMandatory(other.isMandatory)
    , allowNotFound(other.allowNotFound)
    , isRecursive(other.isRecursive)
    , dopt(other.dopt)
    , _isOverloadable(other._isOverloadable)
    , _searchAll(other._searchAll)
    , _checkOnlyClasses(other._checkOnlyClasses)
    , _isInLibrary(other._isInLibrary)
    , _isInInheritance(other._isInInheritance)
    , _checkOnlyStandardLibraryDefs(other._checkOnlyStandardLibraryDefs)
{
    //ntd
}

InternalDeclarationVisitor::InternalData &InternalDeclarationVisitor::InternalData::operator=(const InternalData &other)
{
    if (this == &other)
        return *this;
    index                         = other.index;
    location                      = other.location;
    sem                           = other.sem;
    resultDeclarations            = other.resultDeclarations;
    previous                      = other.previous;
    startingObject                = other.startingObject;
    isMandatory                   = other.isMandatory;
    allowNotFound                 = other.allowNotFound;
    isRecursive                   = other.isRecursive;
    dopt                          = other.dopt;
    _isOverloadable               = other._isOverloadable;
    _searchAll                    = other._searchAll;
    _checkOnlyClasses             = other._checkOnlyClasses;
    _isInLibrary                  = other._isInLibrary;
    _isInInheritance              = other._isInInheritance;
    _checkOnlyStandardLibraryDefs = other._checkOnlyStandardLibraryDefs;
    return *this;
}

// ///////////////////////////////////////////////////////////////////
// InternalDeclarationVisitor
// ///////////////////////////////////////////////////////////////////

InternalDeclarationVisitor::InternalDeclarationVisitor()
    : _data()
{
    // ntd
}

InternalDeclarationVisitor::~InternalDeclarationVisitor()
{
    // ntd
}

Declarations InternalDeclarationVisitor::findDeclarations(
    const std::string &index,
    Object *location,
    Object *startingObject,
    bool &allowNotFound,
    ILanguageSemantics *refSem,
    const GetCandidatesOptions &dopt,
    const bool isRecursive)
{
    if (location == nullptr)
        location = startingObject;
    messageAssert(location != nullptr, "Expected location", nullptr, refSem);

    Declarations result;
    _data.location       = location;
    _data.index          = index;
    _data.startingObject = startingObject;
    _data.previous       = startingObject;
    _data.sem            = refSem;
    _data.dopt           = dopt;
    _data.isRecursive    = isRecursive;
    _setOverloadable();
    _setSearchAll();

    // calling the map.
    // Special symbols management could be not correct in case of startingObject
    // is subnode of location. TODO: check
    if (_isPrefixedSymbol(startingObject))
    //        && !hif::isSubNode(startingObject, location))
    {
        callMap(startingObject);
    } else {
        callMap(location);
    }

    // merging results
    result.splice(result.end(), _data.resultDeclarations);
    allowNotFound = _data.allowNotFound;

    return result;
}

bool InternalDeclarationVisitor::_isPrefixedSymbol(Object *o)
{
    if (dynamic_cast<hif::features::ISymbol *>(o) == nullptr)
        return false;

    return dynamic_cast<FieldReference *>(o) != nullptr || dynamic_cast<FunctionCall *>(o) != nullptr ||
           dynamic_cast<Instance *>(o) != nullptr || dynamic_cast<Library *>(o) != nullptr ||
           dynamic_cast<ParameterAssign *>(o) != nullptr || dynamic_cast<PortAssign *>(o) != nullptr ||
           dynamic_cast<ProcedureCall *>(o) != nullptr || dynamic_cast<TPAssign *>(o) != nullptr ||
           dynamic_cast<TypeReference *>(o) != nullptr || dynamic_cast<ViewReference *>(o) != nullptr;
}

void InternalDeclarationVisitor::_searchInParent(Object *obj)
{
    if (obj == _data.location && obj == _data.startingObject) {
        // Searching in parent
        map(_data.location);
    } else if (hif::isSubNode(_data.location, obj)) {
        // Skip intermediate nodes, since they cannot contain the searched declaration,
        // and could create a loop.
        map(obj);
    } else {
        // Dynamic dispatch: since symbol was prefixed, and we have not found
        // its decl by using its prefix (it was nullptr),
        // searching symbol the usual way: starting from location!
        _data.previous = obj;
        callMap(_data.location);
    }
}

/// ---------------------utility methods--------------------------

void InternalDeclarationVisitor::_setOverloadable()
{
    _data._isOverloadable = dynamic_cast<FunctionCall *>(_data.startingObject) != nullptr ||
                            dynamic_cast<ProcedureCall *>(_data.startingObject) != nullptr;
}

void InternalDeclarationVisitor::_setSearchAll()
{
    _data._searchAll = dynamic_cast<FunctionCall *>(_data.startingObject) != nullptr ||
                       dynamic_cast<ProcedureCall *>(_data.startingObject) != nullptr ||
                       dynamic_cast<ViewReference *>(_data.startingObject) != nullptr ||
                       dynamic_cast<Library *>(_data.startingObject) != nullptr;
}

template <typename T> void InternalDeclarationVisitor::_getDeclarationInList(BList<T> &list)
{
    if (_data._checkOnlyStandardLibraryDefs) {
        LibraryDef *ld = dynamic_cast<LibraryDef *>(list.getParent());
        if (ld == nullptr || !ld->isStandard())
            return;
    }

    if (!_data._isOverloadable && !_data.resultDeclarations.empty())
        return;

    if (!_data._searchAll && _data.previous->isInBList() &&
        _data.previous->getBList() == reinterpret_cast<BList<Object> *>(&list)) {
        typename BList<T>::iterator i = typename BList<T>::iterator(static_cast<T *>(_data.previous));
        // skipping current declaration
        --i;

        // searching only up to data.previous declarations
        for (; i != list.rend(); --i) {
            _checkDeclaration(*i);
        }
    } else {
        // searching in all the list
        for (typename BList<T>::iterator i = list.begin(); i != list.end(); ++i) {
            _checkDeclaration(*i);
        }
    }
}

void InternalDeclarationVisitor::_getDeclarationInLibraries(BList<Library> &list)
{
    if (!_data._isOverloadable && !_data.resultDeclarations.empty())
        return;
    if (_data._checkOnlyClasses)
        return;

    bool restore       = _data._isInLibrary;
    _data._isInLibrary = true;

    for (BList<Library>::iterator i = list.begin(); i != list.end(); ++i) {
        // get declaration of library

        // Avoid to repeat the search if not found.
        if ((*i)->getName() == _data.index)
            continue;

        // Note: start to search from system node to avoid infinite loop.
        Object *parent      = (*i)->getParent();
        Object *grandParent = parent->getParent();
        Object *location    = grandParent != nullptr ? grandParent : parent;

        GetCandidatesOptions dopt;
        _fillDopt(dopt, _data.dopt);
        dopt.location   = location;
        LibraryDef *lib = getDeclaration(*i, _data.sem, dopt);
        if (lib == nullptr)
            continue;

        // Case: LibDef: { foo(), Du }, Du: { call(foo), Lib:{LibDef} }
        // --> One time, find foo() from inclusion, and one time into the parent.
        // Then, no need to perform double search!Just skip libs which are parents!
        if (hif::isSubNode(*i, lib))
            continue;

        // if found, search in library
        map(lib);
    }

    _data._isInLibrary = restore;
}

void InternalDeclarationVisitor::_getDeclarationInEntity(Entity *entity)
{
    if (entity == nullptr)
        return;
    if (!_data._isOverloadable && !_data.resultDeclarations.empty())
        return;

    // if we are coming form entity or entity is null,
    // we have not to search again on its list.
    if (dynamic_cast<Entity *>(_data.previous) != nullptr)
        return;

    for (BList<Port>::iterator i = entity->ports.begin(); i != entity->ports.end(); ++i) {
        _checkDeclaration(*i);
    }
}

void InternalDeclarationVisitor::_checkDeclaration(Declaration *decl)
{
    // If declaration is a typedef of an enum, we may searching a enum
    // value that is child of that enum. So we must check if we are in
    // this case; otherwise do the normal check on the name of declaration.
    TypeDef *td = dynamic_cast<TypeDef *>(decl);
    if (td != nullptr) {
        Enum *e = dynamic_cast<Enum *>(td->getType());
        if (e != nullptr) {
            for (BList<EnumValue>::iterator i = e->values.begin(); i != e->values.end(); ++i) {
                if ((*i)->getName() == _data.index) {
                    // Found:
                    _data.resultDeclarations.push_back(*i);
                    return;
                }
            }
        }
    }

    // TODO check record

    // If declaration is a design unit, and we are searching the view
    // referred by a view reference, perform checks on design unit and view
    // names to understand if view declaration is contained in design unit.
    ViewReference *vref = dynamic_cast<ViewReference *>(_data.startingObject);
    DesignUnit *du      = dynamic_cast<DesignUnit *>(decl);
    if (vref != nullptr && du != nullptr && vref->getDesignUnit() == du->getName()) {
        if (du->views.size() == 1 && vref->getName() == hif::NameTable::getInstance()->none()) {
            _data.resultDeclarations.push_back(du->views.front());
            return;
        }

        for (BList<View>::iterator i = du->views.begin(); i != du->views.end(); ++i) {
            if ((*i)->getName() == vref->getName()) {
                // Found:
                _data.resultDeclarations.push_back(*i);
                return;
            }
        }
    }

    if (vref != nullptr && dynamic_cast<View *>(decl) != nullptr) {
        du = dynamic_cast<DesignUnit *>(decl->getParent());
        if (du == nullptr)
            return;
        if (du->getName() != vref->getDesignUnit())
            return;

        if (du->views.size() == 1 && vref->getName() == hif::NameTable::getInstance()->none()) {
            _data.resultDeclarations.push_back(du->views.front());
            return;
        }
    }

    // TODO move special case in a function _checkDeclarationViewRef and perform
    // the check about starting object type inside all checkDeclaration callers

    // standard check
    hif::features::ISymbol *symb = dynamic_cast<hif::features::ISymbol *>(_data.startingObject);
    messageAssert((symb != nullptr), "Expected symbol", _data.startingObject, _data.sem);
    if (_data.index == decl->getName() && _data.startingObject && symb->matchDeclarationType(decl)) {
        _data.resultDeclarations.push_back(decl);
    }
}

bool InternalDeclarationVisitor::_getMethodDeclaration(Value *instance)
{
    ReferencedType *thisType = nullptr;
    Type *t                  = getBaseType(getSemanticType(instance, _data.sem), false, _data.sem);

    if (dynamic_cast<ViewReference *>(t) != nullptr) {
        thisType = static_cast<ViewReference *>(t);
    } else if (dynamic_cast<Library *>(t) != nullptr) {
        thisType = static_cast<Library *>(t);
    } else if (dynamic_cast<Pointer *>(t) != nullptr) {
        Pointer *ptrType = static_cast<Pointer *>(t);
        if (dynamic_cast<ViewReference *>(ptrType->getType())) {
            thisType = static_cast<ViewReference *>(ptrType->getType());
        }
    } else if (dynamic_cast<Reference *>(t) != nullptr) {
        Reference *refType = static_cast<Reference *>(t);
        if (dynamic_cast<ViewReference *>(refType->getType())) {
            thisType = static_cast<ViewReference *>(refType->getType());
        }
    }

    // Some semantics native methods can be invoked on non viewref types.
    // e.g. sc_bv< 5 >a; a.to_int();
    if (thisType == nullptr) {
        // Functions must be inside standard library defs.
        _data._checkOnlyStandardLibraryDefs = true;
        return true;
    }

    GetCandidatesOptions dopt;
    _fillDopt(dopt, _data.dopt);
    dopt.location     = _data.location;
    Declaration *decl = getDeclaration(thisType, _data.sem, dopt);

    if (dynamic_cast<View *>(decl) != nullptr) {
        View *view = static_cast<View *>(decl);
        messageAssert(view->getContents() != nullptr, "Unexpected view without contents", view, _data.sem);

        bool restore            = _data._checkOnlyClasses;
        _data._checkOnlyClasses = true;
        map(view->getContents());
        _data._checkOnlyClasses = restore;

        // in case declaration is not found inside View, search outside it.
        // ref.design: vams_std_bedm/resistor. E.g.:
        // view=electrical
        // port=p<electrical>
        // p.read() <- hif_systemc_read defined inside hif_systemc_sc_core
        if (_data.resultDeclarations.empty()) {
            _data._checkOnlyStandardLibraryDefs = true;
            return true;
        }
    } else if (dynamic_cast<LibraryDef *>(decl) != nullptr) {
        LibraryDef *ldef = static_cast<LibraryDef *>(decl);

        bool restore       = _data._isInLibrary;
        _data._isInLibrary = true;
        map(ldef);
        _data._isInLibrary = restore;
    }

    return false;
}

void InternalDeclarationVisitor::_getDeclarationInInheritance(View *view)
{
    if (!_data._isOverloadable && !_data.resultDeclarations.empty())
        return;

    // Avoid infinite loop: viewref decl cannot be in sister viewref!
    if (view->inheritances.toOtherBList<Object>().contains(_data.previous))
        return;
    for (BList<ViewReference>::iterator i = view->inheritances.begin(); i != view->inheritances.end(); ++i) {
        GetCandidatesOptions dopt;
        _fillDopt(dopt, _data.dopt);
        dopt.location = nullptr;
        View *v       = getDeclaration(*i, _data.sem, dopt);
        if (v == nullptr)
            continue;

        const bool restore     = _data._isInInheritance;
        _data._isInInheritance = true;
        map(v->getContents());
        _data._isInInheritance = restore;

        if (!_data._isOverloadable && !_data.resultDeclarations.empty()) {
            break;
        }
    }
}

Record *InternalDeclarationVisitor::_getRecordOfTyperef(Type *prefixType)
{
    Record *ret = dynamic_cast<Record *>(prefixType);
    messageAssert(
        ret == nullptr, "_getRecordOfTyperef() called on Record", prefixType->getParent()->getParent(), nullptr);

    Reference *ref = dynamic_cast<Reference *>(prefixType);
    if (ref != nullptr) {
        prefixType = ref->getType();
    }

    Pointer *ptr = dynamic_cast<Pointer *>(prefixType);
    while (ptr != nullptr) {
        prefixType = ptr->getType();
        ptr        = dynamic_cast<Pointer *>(ptr->getType());
    }

    TypeReference *tr = dynamic_cast<TypeReference *>(prefixType);
    GetCandidatesOptions dopt;
    _fillDopt(dopt, _data.dopt);
    dopt.location = _data.location;
    while (tr != nullptr) {
        TypeReference::DeclarationType *decl = getDeclaration(tr, _data.sem, dopt);
        messageDebugAssert(decl != nullptr, "Declaration not found", tr, _data.sem);
        TypeDef *td = dynamic_cast<TypeDef *>(decl);
        messageAssert(td != nullptr, "Expected TypeDef declaration", decl, _data.sem);

        ret = dynamic_cast<Record *>(td->getType());
        tr  = dynamic_cast<TypeReference *>(td->getType());
    }

    return ret;
}

void InternalDeclarationVisitor::_fillDopt(GetCandidatesOptions &to, const GetCandidatesOptions &from)
{
    to              = from;
    to.forceRefresh = false;
}

void InternalDeclarationVisitor::map(Object *obj)
{
    if (obj->getParent() == nullptr)
        return;

    _data.previous = obj;
    callMap(obj->getParent());
}

void InternalDeclarationVisitor::map(Contents *obj)
{
    _getDeclarationInList(obj->declarations);
    _getDeclarationInList(obj->stateTables);

    // while searching in parent classes, we must not search global scopes
    // and support libraries.
    if (!_data._isInInheritance) {
        // must be called at last
        _getDeclarationInLibraries(obj->libraries);
    }

    if (_data._isOverloadable || _data.resultDeclarations.empty()) {
        map(static_cast<Object *>(obj));
    }
}

void InternalDeclarationVisitor::map(DesignUnit *obj)
{
    _getDeclarationInList(obj->views);

    if (_data._isOverloadable || _data.resultDeclarations.empty()) {
        map(static_cast<Object *>(obj));
    }
}

void InternalDeclarationVisitor::map(Entity *obj)
{
    _getDeclarationInList(obj->ports);

    if (_data._isOverloadable || _data.resultDeclarations.empty()) {
        map(static_cast<Object *>(obj));
    }
}

void InternalDeclarationVisitor::map(For *obj)
{
    _getDeclarationInList(obj->initDeclarations);

    if (_data._isOverloadable || _data.resultDeclarations.empty()) {
        map(static_cast<Object *>(obj));
    }
}

void InternalDeclarationVisitor::map(ForGenerate *obj)
{
    _getDeclarationInList(obj->declarations);
    _getDeclarationInList(obj->stateTables);
    _getDeclarationInList(obj->initDeclarations);

    if (_data._isOverloadable || _data.resultDeclarations.empty()) {
        map(static_cast<Object *>(obj));
    }
}

void InternalDeclarationVisitor::map(IfGenerate *obj)
{
    _getDeclarationInList(obj->declarations);
    _getDeclarationInList(obj->stateTables);

    if (_data._isOverloadable || _data.resultDeclarations.empty()) {
        map(static_cast<Object *>(obj));
    }
}

void InternalDeclarationVisitor::map(LibraryDef *obj)
{
    _getDeclarationInList(obj->declarations);

    if ((_data._isOverloadable || _data.resultDeclarations.empty()) && !_data._isInLibrary) {
        _getDeclarationInLibraries(obj->libraries);
    }

    if ((_data._isOverloadable || _data.resultDeclarations.empty()) && !_data._isInLibrary) {
        map(static_cast<Object *>(obj));
    }
}

void InternalDeclarationVisitor::map(Record *obj)
{
    _getDeclarationInList(obj->fields);

    if (_data._isOverloadable || _data.resultDeclarations.empty()) {
        map(static_cast<Object *>(obj));
    }
}

void InternalDeclarationVisitor::map(StateTable *obj)
{
    _getDeclarationInList(obj->declarations);
    //_getDeclarationInList(obj->edges);
    //_getDeclarationInList(obj->states);

    if (_data._isOverloadable || _data.resultDeclarations.empty()) {
        map(static_cast<Object *>(obj));
    }
}

void InternalDeclarationVisitor::map(SubProgram *obj)
{
    _getDeclarationInList(obj->parameters);
    _getDeclarationInList(obj->templateParameters);

    _checkDeclaration(obj);

    if (_data._isOverloadable || _data.resultDeclarations.empty()) {
        map(static_cast<Object *>(obj));
    }
}

void InternalDeclarationVisitor::map(System *obj)
{
    _getDeclarationInList(obj->designUnits);
    _getDeclarationInList(obj->libraryDefs);
    _getDeclarationInList(obj->declarations);

    // If Library is not found inside LibraryDef list, it does not exist.
    if (dynamic_cast<Library *>(_data.startingObject) == nullptr) {
        _getDeclarationInLibraries(obj->libraries);
    }
}

void InternalDeclarationVisitor::map(TypeDef *obj)
{
    //if (data.previous->getParent() == obj)
    _getDeclarationInList(obj->templateParameters);

    if (_data._isOverloadable || _data.resultDeclarations.empty()) {
        map(static_cast<Object *>(obj));
    }
}

void InternalDeclarationVisitor::map(View *obj)
{
    _getDeclarationInList(obj->declarations);
    _getDeclarationInEntity(obj->getEntity());
    _getDeclarationInList(obj->templateParameters);

    _getDeclarationInInheritance(obj);

    // while searching in parent classes, we must not search global scopes
    // and support libraries.
    if (_data._isInInheritance || _data._checkOnlyClasses)
        return;

    // must be called at last
    _getDeclarationInLibraries(obj->libraries);

    if (_data._isOverloadable || _data.resultDeclarations.empty()) {
        map(static_cast<Object *>(obj));
    }
}

// ------------- particular cases ---------------

void InternalDeclarationVisitor::map(EnumValue *obj)
{
    if (obj->getParent() == nullptr)
        return;

    Enum *e = dynamic_cast<Enum *>(obj->getParent());
    if (e == nullptr) {
        messageError("Wrong enum value parent (1).", obj, _data.sem);
    }

    TypeDef *td = dynamic_cast<TypeDef *>(e->getParent());
    if (td == nullptr || !td->isOpaque()) {
        messageError("Wrong enum value parent (2).", obj, _data.sem);
    }

    if (td->getName() == _data.index) {
        _data.resultDeclarations.push_back(td);
    }

    if (_data._isOverloadable || _data.resultDeclarations.empty()) {
        map(static_cast<Object *>(obj));
    }
}

// ------------- special management cases ---------------

void InternalDeclarationVisitor::map(FieldReference *obj)
{
    if (_data.startingObject == obj) {
        _manageFieldReference(obj);
        return;
    }

    map(static_cast<Object *>(obj));
}

void InternalDeclarationVisitor::map(FunctionCall *obj)
{
    if (_data.startingObject == obj) {
        _manageFunctionCall(obj);
        return;
    }

    map(static_cast<Object *>(obj));
}
void InternalDeclarationVisitor::map(Instance *obj)
{
    if (_data.startingObject == obj) {
        _manageInstance(obj);
        return;
    }

    map(static_cast<Object *>(obj));
}
void InternalDeclarationVisitor::map(Library *obj)
{
    if (_data.startingObject == obj) {
        _manageLibrary(obj);
        return;
    }

    map(static_cast<Object *>(obj));
}
void InternalDeclarationVisitor::map(ParameterAssign *obj)
{
    if (_data.startingObject == obj) {
        _manageParameterAssign(obj);
        return;
    }

    map(static_cast<Object *>(obj));
}
void InternalDeclarationVisitor::map(PortAssign *obj)
{
    if (_data.startingObject == obj) {
        _managePortAssign(obj);
        return;
    }

    map(static_cast<Object *>(obj));
}
void InternalDeclarationVisitor::map(ProcedureCall *obj)
{
    if (_data.startingObject == obj) {
        _manageProcedureCall(obj);
        return;
    }

    map(static_cast<Object *>(obj));
}
void InternalDeclarationVisitor::map(TPAssign *obj)
{
    if (_data.startingObject == obj) {
        _manageTPAssign(obj);
        return;
    }

    map(static_cast<Object *>(obj));
}
void InternalDeclarationVisitor::map(TypeReference *obj)
{
    if (_data.startingObject == obj) {
        _manageTypeReference(obj);
        return;
    }

    map(static_cast<Object *>(obj));
}

void InternalDeclarationVisitor::map(ViewReference *obj)
{
    if (_data.startingObject == obj) {
        _manageViewReferece(obj);
        return;
    }

    map(static_cast<Object *>(obj));
}

// -----------Starting object management methods--------------------

void InternalDeclarationVisitor::_manageFieldReference(FieldReference *obj)
{
    Type *thisType   = nullptr;
    Type *prefixType = getSemanticType(obj->getPrefix(), _data.sem);
    Type *t          = getBaseType(prefixType, false, _data.sem, false);
    while (t != nullptr) {
        thisType = dynamic_cast<ViewReference *>(t);
        if (thisType != nullptr)
            break;
        thisType = dynamic_cast<Record *>(t);
        if (thisType != nullptr)
            break;
        thisType = dynamic_cast<Library *>(t);
        if (thisType != nullptr)
            break;

        if (dynamic_cast<Pointer *>(t) != nullptr) {
            Pointer *ptrType = static_cast<Pointer *>(t);
            t                = getBaseType(ptrType->getType(), false, _data.sem);
            // Cannot automatically dereference pointers of pointers (or references of references)
            if (dynamic_cast<Pointer *>(t) != nullptr)
                return;
            if (dynamic_cast<Reference *>(t) != nullptr)
                return;
        } else if (dynamic_cast<Reference *>(t) != nullptr) {
            Reference *refType = static_cast<Reference *>(t);
            t                  = getBaseType(refType->getType(), false, _data.sem);
            // Cannot automatically dereference pointers of pointers (or references of references)
            if (dynamic_cast<Reference *>(t) != nullptr)
                return;
            if (dynamic_cast<Pointer *>(t) != nullptr)
                return;
        } else {
            // Some semantics can have native records or native classes.
            _data.allowNotFound = true;
            return;
        }
    }

    // Some semantics can have native records or native classes.
    if (t == nullptr)
        return;
    messageAssert((thisType != nullptr), "Unexpected type", t, _data.sem);

    // If type is record, check in its internal fields.
    Record *record = dynamic_cast<Record *>(thisType);
    if (record != nullptr) {
        RecordValue *rv = dynamic_cast<RecordValue *>(obj->getPrefix());
        if (rv != nullptr) {
            // TODO: in this case it was like a constant which has been
            // expanded therefore we should search for a record declaration
            // with matching field names and types.
            //
            _data.allowNotFound = true;
            return;
        }

        Record *rec = _getRecordOfTyperef(prefixType);
        messageAssert((rec != nullptr), "Unable to get Record from TypeReference", prefixType, _data.sem);

        for (BList<Field>::iterator i = rec->fields.begin(); i != rec->fields.end(); ++i) {
            if (obj->getName() != (*i)->getName())
                continue;
            // Found
            _data.resultDeclarations.push_back((*i));
        }

        return;
    }

    // viewref case:
    ViewReference *viewType = dynamic_cast<ViewReference *>(thisType);
    Library *libType        = dynamic_cast<Library *>(thisType);
    GetCandidatesOptions dopt;
    _fillDopt(dopt, _data.dopt);
    dopt.location = _data.location;
    if (viewType != nullptr) {
        View *view = getDeclaration(viewType, _data.sem, dopt);
        if (view == nullptr) {
            messageError("Declaration of Viewreference not found (1).", viewType, _data.sem);
        }

        messageDebugAssert(view->getContents() != nullptr, "Unexpected view without contents", view, _data.sem);

        bool restore            = _data._checkOnlyClasses;
        _data._checkOnlyClasses = true;
        map(view->getContents());
        _data._checkOnlyClasses = restore;
    } else if (libType != nullptr) {
        LibraryDef *lib = getDeclaration(libType, _data.sem, dopt);
        if (lib == nullptr) {
            messageError("Declaration of Library not found (1).", libType, _data.sem);
        }

        bool restore       = _data._isInLibrary;
        _data._isInLibrary = true;
        map(lib);
        _data._isInLibrary = restore;
    } else {
        messageDebugAssert(false, "Unexpected thisType", thisType, _data.sem);
    }
}

void InternalDeclarationVisitor::_manageFunctionCall(FunctionCall *obj)
{
    if (obj->getInstance() != nullptr) {
        const bool search = _getMethodDeclaration(obj->getInstance());
        if (search)
            _searchInParent(obj);
        return;
    } else if (
        dynamic_cast<Instance *>(obj->getParent()) != nullptr &&
        obj->getName() == hif::NameTable::getInstance()->hifConstructor()) {
        Instance *ii = static_cast<Instance *>(obj->getParent());
        messageDebugAssert(ii->getValue() == obj, "Unexpected function", obj, _data.sem);

        ViewReference *vr = dynamic_cast<ViewReference *>(ii->getReferencedType());
        messageAssert(vr != nullptr, "Expected viewreference as referenced type", ii, _data.sem);

        GetCandidatesOptions dopt;
        _fillDopt(dopt, _data.dopt);
        dopt.location                        = _data.location;
        ViewReference::DeclarationType *view = getDeclaration(vr, _data.sem, dopt);
        messageAssert(view != nullptr, "Cannot find declaration of viewref", vr, _data.sem);
        messageAssert(view->getContents() != nullptr, "Unexpected view without contents", view, _data.sem);

        bool restore            = _data._checkOnlyClasses;
        _data._checkOnlyClasses = true;
        map(view->getContents());
        _data._checkOnlyClasses = restore;

        return;
    }

    _searchInParent(obj);
}

void InternalDeclarationVisitor::_manageInstance(Instance *obj)
{
    messageDebugAssert(obj->getReferencedType() != nullptr, "Expected refrenced type", obj, _data.sem);

    TypeReference *tref = dynamic_cast<TypeReference *>(obj->getReferencedType());
    ViewReference *vref = dynamic_cast<ViewReference *>(obj->getReferencedType());
    Library *lref       = dynamic_cast<Library *>(obj->getReferencedType());

    if (tref != nullptr) {
        // e.g. for SystemC AMS, typedef of sca_electrical (viewref) to sca_node (typeref)

        Type *bt = hif::semantics::getBaseType(tref, false, _data.sem);
        messageAssert(bt != nullptr, "Expected base type", tref, _data.sem);
        vref = dynamic_cast<ViewReference *>(bt);

        // Declaration of Instance is Entity therefore we
        // cannot return a type declaration.
        if (vref == nullptr) {
            _data.allowNotFound = true;
            return;
        }

        // Otherwise fall into vref case...
    }

    GetCandidatesOptions dopt;
    _fillDopt(dopt, _data.dopt);
    dopt.location = _data.location;
    if (vref != nullptr) {
        View *view = getDeclaration(vref, _data.sem, dopt);
        if (view == nullptr)
            return;

        _data.resultDeclarations.push_back(view->getEntity());
    } else if (lref != nullptr) {
        // Declaration of Instance is Entity therefore we
        // cannot return a library def.
        _data.allowNotFound = true;
        return;
    } else {
        messageError("Unexpected referneced type", obj->getReferencedType(), _data.sem);
    }
}

void InternalDeclarationVisitor::_manageLibrary(Library *obj)
{
    if (obj->getInstance() == nullptr) {
        _searchInParent(obj);
        return;
    }

    Library *inst = dynamic_cast<Library *>(obj->getInstance());
    messageAssert(inst != nullptr, "Unexpected library instance", obj, _data.sem);
    GetCandidatesOptions dopt;
    _fillDopt(dopt, _data.dopt);
    dopt.location       = _data.location;
    LibraryDef *instDec = getDeclaration(inst, _data.sem, dopt);
    if (instDec == nullptr) {
        //messageDebugAssert(false, "not found library def declaration", obj, data.sem);
        return;
    }
    map(instDec);
}

void InternalDeclarationVisitor::_manageParameterAssign(ParameterAssign *obj)
{
    messageAssert(
        dynamic_cast<FunctionCall *>(obj->getParent()) != nullptr ||
            dynamic_cast<ProcedureCall *>(obj->getParent()) != nullptr,
        "Unexpected parent", obj->getParent(), _data.sem);

    GetCandidatesOptions dopt;
    _fillDopt(dopt, _data.dopt);
    dopt.location = _data.location;

    SubProgram *sub = dynamic_cast<SubProgram *>(getDeclaration(obj->getParent(), _data.sem, dopt));

    if (sub == nullptr)
        return;

    if (_data.index == hif::NameTable::getInstance()->none()) {
        // The name may be not set just after parsing. Thus this could help
        // even if it could be unsafe.
        BList<ParameterAssign>::size_t pos = obj->getBList()->getPosition(obj);
        messageAssert(pos != obj->getBList()->size(), "Unexpected position", obj, _data.sem);
        _data.resultDeclarations.push_back(sub->parameters.at(pos));
    } else {
        map(sub);
    }

    return;
}

void InternalDeclarationVisitor::_managePortAssign(PortAssign *obj)
{
    Instance *instance = dynamic_cast<Instance *>(obj->getParent());
    messageAssert(instance != nullptr, "Expected instance parent", obj->getParent(), _data.sem);
    messageAssert(instance->getReferencedType() != nullptr, "Expected referenced type", instance, _data.sem);

    ViewReference *vref = dynamic_cast<ViewReference *>(instance->getReferencedType());
    messageAssert(vref != nullptr, "Expected ViewRef refrenced type", instance->getReferencedType(), _data.sem);
    GetCandidatesOptions dopt;
    _fillDopt(dopt, _data.dopt);
    dopt.location = _data.location;
    View *view    = getDeclaration(vref, _data.sem, dopt);

    messageAssert(view != nullptr, "Declaration not found.", vref, _data.sem);

    Entity *entity = view->getEntity();
    messageAssert(entity != nullptr, "Unexpected view without interface", view, _data.sem);

    map(entity);
}

void InternalDeclarationVisitor::_manageProcedureCall(ProcedureCall *obj)
{
    if (obj->getInstance() == nullptr) {
        _searchInParent(obj);
        return;
    }

    const bool search = _getMethodDeclaration(obj->getInstance());
    if (search)
        _searchInParent(obj);
}

void InternalDeclarationVisitor::_manageTPAssign(TPAssign *obj)
{
    GetCandidatesOptions dopt;
    _fillDopt(dopt, _data.dopt);
    dopt.location     = _data.location;
    Declaration *decl = getDeclaration(obj->getParent(), _data.sem, dopt);
    if (decl == nullptr)
        return;
    callMap(decl);
}

void InternalDeclarationVisitor::_manageTypeReference(TypeReference *obj)
{
    if (obj->getInstance() == nullptr) {
        _searchInParent(obj);
        return;
    }

    // viewref case:
    Type *base              = getBaseType(obj->getInstance(), false, _data.sem);
    ViewReference *viewType = dynamic_cast<ViewReference *>(base);
    Library *libType        = dynamic_cast<Library *>(base);

    GetCandidatesOptions dopt;
    _fillDopt(dopt, _data.dopt);
    dopt.location = _data.location;
    if (viewType != nullptr) {
        View *view = getDeclaration(viewType, _data.sem, dopt);
        if (view == nullptr) {
            messageError("Declaration of ViewReference not found (2).", viewType, _data.sem);
        }

        messageAssert(view->getContents() != nullptr, "Unexpected view without contents", view, _data.sem);
        bool restore            = _data._checkOnlyClasses;
        _data._checkOnlyClasses = true;
        map(view->getContents());
        _data._checkOnlyClasses = restore;
    } else if (libType != nullptr) {
        LibraryDef *ldef = getDeclaration(libType, _data.sem, dopt);
        if (ldef == nullptr) {
            messageError("Declaration of Library not found (2).", libType, _data.sem);
        }

        bool restore            = _data._checkOnlyClasses;
        _data._checkOnlyClasses = true;
        map(ldef);
        _data._checkOnlyClasses = restore;
    } else {
        messageDebugAssert(false, "Unexpected case", base, _data.sem);
    }
}

void InternalDeclarationVisitor::_manageViewReferece(ViewReference *obj)
{
    if (obj->getInstance() != nullptr) {
        GetCandidatesOptions dopt;
        _fillDopt(dopt, _data.dopt);
        dopt.location     = _data.location;
        Declaration *decl = getDeclaration(obj->getInstance(), _data.sem, dopt);
        if (decl == nullptr) {
            messageError("Declaration of ViewReference instance not found.", obj->getInstance(), _data.sem);
        }

        View *view      = dynamic_cast<View *>(decl);
        LibraryDef *lib = dynamic_cast<LibraryDef *>(decl);
        if (view != nullptr) {
            messageAssert(view->getContents() != nullptr, "Unexpected view without contents", view, _data.sem);
            // candidate must be a design unit
            // Search a design unit with name = unit name inside view contents
            // of another design unit.
            // e.g. class_a::class_b
            // search the design unit class_b inside view contents of design
            // unit class_a
            //
            // Set flag _checkOnlyClasses to avoid loop.
            bool restore            = _data._checkOnlyClasses;
            _data._checkOnlyClasses = true;
            map(view->getContents());
            _data._checkOnlyClasses = restore;
        } else if (lib != nullptr) {
            bool restore       = _data._isInLibrary;
            _data._isInLibrary = true;
            map(lib);
            _data._isInLibrary = restore;
        } else {
            messageError("Unexpected Viewreference instance declaration type.", obj, _data.sem);
        }
        return;
    }

    _searchInParent(obj);
}

// /////////////////////////////////////////////////////////////////////////
// Get declaration visitor
// /////////////////////////////////////////////////////////////////////////
class GetDeclarationVisitor : public hif::HifVisitor
{
public:
    typedef bool (*CheckMethod)(Declaration *);

    GetDeclarationVisitor(ILanguageSemantics *sem, Declarations *list, const GetCandidatesOptions &opt);
    virtual ~GetDeclarationVisitor();

    GetDeclarationVisitor(const GetDeclarationVisitor &)                     = delete;
    auto operator=(const GetDeclarationVisitor &) -> GetDeclarationVisitor & = delete;

    int visitFieldReference(FieldReference &o);
    int visitFunctionCall(FunctionCall &o);
    int visitIdentifier(Identifier &o);
    int visitInstance(Instance &o);
    int visitLibrary(Library &o);
    int visitParameterAssign(ParameterAssign &o);
    int visitPortAssign(PortAssign &o);
    int visitProcedureCall(ProcedureCall &o);
    int visitTypeReference(TypeReference &o);
    int visitTypeTPAssign(TypeTPAssign &o);
    int visitValueTPAssign(ValueTPAssign &o);
    int visitViewReference(ViewReference &o);

    Declaration *getResult() const;

    /// @brief Use an heuristic to determinate what is the best candidate of
    /// the list of possible candidates.
    /// @param candidates The list of candidates.
    /// @param startingObject The object from which is called get declaration.
    /// @param isMandatory Always return at least one declaration, if any.
    ///
    template <typename T>
    Declaration *getBestCandidate(Declarations &candidates, T *startingObject, const bool isMandatory);

private:
    /// @brief Scores enums used to find best candidate.
    enum Scores : unsigned int {
        EQUALS             = 512,
        SAME_SPAN          = 256,
        SAME_TYPE          = 128,
        SAME_BASE          = 64,
        ASSIGNABLE         = 32,
        FLAGS              = 4,
        SCALAR             = 4,
        SAME_INSTANCE_TYPE = 2,
        SORTED_PARAMETERS  = 1
    };

    /// @brief The options
    GetCandidatesOptions _opt;

    /// @brief The declaration found
    Declaration *_resultDeclaration;

    /// @brief If not nullptr, returns all candidates.
    Declarations *_list;

    /// @brief The semantics
    ILanguageSemantics *_sem;

    /// @brief In debug build print removes of candidate reason.
    bool _internalDebugPrintRemoveReason;

    template <typename T> bool _getMemberDeclaration(T *symbol);

    template <typename T> void _getGenericDeclaration(T *symbol, CheckMethod checkMethod = nullptr);

    template <typename T> void _getCallDeclaration(T *symbol, CheckMethod checkMethod = nullptr);

    /// @brief Filter out declarations w.r.t. template type T.
    template <typename T> void _setCandidates(Declarations &list, Declarations &result);

    /// @brief Check that it is found only one candidate, otherwise it rise
    /// a warning messages and return the first of candidates.
    /// @param candidates The list of candidates.
    /// @param startingObject the starting object.
    ///
    template <typename T> T *_checkCandidates(Declarations &candidates, Object *startingObject);

    /// @brief This function is used form getBestCandidate function to calculate
    /// the scores of candidates. It assign a score using an heuristic on types
    /// of formal and actual parameters.
    /// @param formal The type of formal parameter.
    /// @param actual The type of actual parameter.
    ///
    unsigned int _calculateScore(Type *formal, Type *actual, Object *startingObject);

    /// @brief Given the priority queue of candidates, removes the lowest-score ones.
    void _getGreatestCandidates(CandidateInfoQueue &bestCandidates, Declarations &normalCandidates);

    /// @brief Print many infos about given removed candidate reason.
    void _printRemovedCandidateReason(Declaration *candidate, const int index, const std::string &removeReason);
};

GetDeclarationVisitor::GetDeclarationVisitor(
    ILanguageSemantics *sem,
    Declarations *list,
    const GetCandidatesOptions &opt)
    : _opt(opt)
    , _resultDeclaration(nullptr)
    , _list(list)
    , _sem(sem)
    , _internalDebugPrintRemoveReason(false)
{
    // ntd
}

GetDeclarationVisitor::~GetDeclarationVisitor()
{
    // ntd
}

template <typename T> bool GetDeclarationVisitor::_getMemberDeclaration(T *symbol)
{
    if ((symbol->GetDeclaration() == nullptr || _opt.forceRefresh) && !_opt.dontSearch)
        return false;

    if (_list == nullptr)
        _resultDeclaration = symbol->GetDeclaration();
    else
        _list->push_back(symbol->GetDeclaration());

    return true;
}

template <typename T> void GetDeclarationVisitor::_getGenericDeclaration(T *symbol, CheckMethod checkMethod)
{
    if (_getMemberDeclaration(symbol))
        return;

    bool allowNotFound = false;
    InternalDeclarationVisitor idv;
    Declarations result = idv.findDeclarations(symbol->getName(), _opt.location, symbol, allowNotFound, _sem, _opt);

    if (checkMethod != nullptr)
        result.remove_if(checkMethod);

    if (_list == nullptr) {
        _resultDeclaration = _checkCandidates<typename T::DeclarationType>(result, symbol);
        if (_resultDeclaration == nullptr && _opt.error && !allowNotFound) {
            messageError("Declaration not found", symbol, _sem);
        }
    } else {
        _setCandidates<typename T::DeclarationType>(*_list, result);
        if (_list->empty() && _opt.error) {
            messageError("Declaration not found", symbol, _sem);
        }
    }
}

template <typename T> void GetDeclarationVisitor::_getCallDeclaration(T *symbol, CheckMethod checkMethod)
{
    if (_getMemberDeclaration(symbol))
        return;

    bool allowNotFound = false;
    InternalDeclarationVisitor idv;
    Declarations result = idv.findDeclarations(symbol->getName(), _opt.location, symbol, allowNotFound, _sem, _opt);

    foundDeclarations.clear();
    if (checkMethod != nullptr)
        result.remove_if(checkMethod);

    if (_list == nullptr) {
        _resultDeclaration = getBestCandidate(result, symbol, _opt.atLeastOne);
        if (_resultDeclaration == nullptr && _opt.error && !allowNotFound) {
            messageError("Declaration not found", symbol, _sem);
        }
    } else if (_opt.atLeastOne) {
        _resultDeclaration = getBestCandidate(result, symbol, _opt.atLeastOne);
        if (_resultDeclaration == nullptr && _opt.error && !allowNotFound) {
            messageError("Declaration not found", symbol, _sem);
        }

        if (_resultDeclaration != nullptr && _list != nullptr) {
            _list->push_back(_resultDeclaration);
            _resultDeclaration = nullptr;
        }
    } else {
        _setCandidates<typename T::DeclarationType>(*_list, result);
        if (_list->empty() && _opt.error && !allowNotFound) {
            messageError("Declaration not found", symbol, _sem);
        }
    }
}

int GetDeclarationVisitor::visitFieldReference(FieldReference &o)
{
    _getGenericDeclaration(&o, FieldReference::isNotAllowedDeclaration);
    return 0;
}

int GetDeclarationVisitor::visitFunctionCall(FunctionCall &o)
{
    _getCallDeclaration(&o, removeDeclaration);
    return 0;
}

int GetDeclarationVisitor::visitIdentifier(Identifier &o)
{
    _getGenericDeclaration(&o);
    return 0;
}

int GetDeclarationVisitor::visitInstance(Instance &o)
{
    _getGenericDeclaration(&o);
    return 0;
}

int GetDeclarationVisitor::visitLibrary(Library &o)
{
    _getGenericDeclaration(&o);
    return 0;
}

int GetDeclarationVisitor::visitParameterAssign(ParameterAssign &o)
{
    _getGenericDeclaration(&o);
    return 0;
}

int GetDeclarationVisitor::visitPortAssign(PortAssign &o)
{
    _getGenericDeclaration(&o);
    return 0;
}

int GetDeclarationVisitor::visitProcedureCall(ProcedureCall &o)
{
    _getCallDeclaration(&o, removeDeclaration);
    return 0;
}

int GetDeclarationVisitor::visitTypeReference(TypeReference &o)
{
    _getGenericDeclaration(&o);
    return 0;
}

int GetDeclarationVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    _getGenericDeclaration(&o);
    return 0;
}

int GetDeclarationVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    _getGenericDeclaration(&o);
    return 0;
}

int GetDeclarationVisitor::visitViewReference(ViewReference &o)
{
    _getGenericDeclaration(&o);
    return 0;
}

Declaration *GetDeclarationVisitor::getResult() const { return _resultDeclaration; }

template <typename T> void GetDeclarationVisitor::_setCandidates(Declarations &list, Declarations &result)
{
    for (Declarations::iterator i = result.begin(); i != result.end(); ++i) {
        if (_opt.getAll) {
            list.push_back(*i);
        } else if (dynamic_cast<T *>(*i) == nullptr) {
            continue;
        } else {
            list.push_back(*i);
        }
    }
}

template <typename T>
Declaration *
GetDeclarationVisitor::getBestCandidate(Declarations &candidates, T *startingObject, const bool isMandatory)
{
    if (candidates.empty())
        return nullptr;

    // NOTE: Viewref, typeref, etc are not handled yet.
    FunctionCall *fco  = dynamic_cast<FunctionCall *>(startingObject);
    ProcedureCall *pco = dynamic_cast<ProcedureCall *>(startingObject);
    if (fco == nullptr && pco == nullptr) {
        return _checkCandidates<typename T::DeclarationType>(candidates, startingObject);
    }

    // Fill originalCandidates infos.
    Declarations originalCandidates(candidates);

    // Removes duplicate candidates.
    foundDeclarations.clear();
    candidates.remove_if(removeDeclaration);

    // Optimization: if mandatory is set and only one candidate is remaining,
    // return the candidate.
    if (isMandatory && candidates.size() == 1) {
        return _checkCandidates<typename T::DeclarationType>(candidates, startingObject);
    }

    // Setup variables to search for best candidate
    BList<ParameterAssign> ppAssignRestore;
    BList<ParameterAssign> &ppAssign = fco != nullptr ? fco->parameterAssigns : pco->parameterAssigns;
    BList<TPAssign> &tpAssign        = fco != nullptr ? fco->templateParameterAssigns : pco->templateParameterAssigns;
    ppAssignRestore.merge(ppAssign);
    CandidateInfoQueue bestCandidates;
    CandidateInfoQueue worstCandidates;

    // Check candidates
    int candidateIndex = 1;
    for (Declarations::iterator i = candidates.begin(); i != candidates.end(); ++candidateIndex) {
        ppAssign.clear();
        hif::CopyOptions opt;
        opt.copySemanticsTypes = true;
        hif::copy(ppAssignRestore, ppAssign, opt);
        ppAssign.addProperty(PROPERTY_TEMPORARY_OBJECT);

        SubProgram *sub = dynamic_cast<SubProgram *>(*i);

        // sanity check
        if (sub == nullptr) {
            if (_internalDebugPrintRemoveReason) {
                _printRemovedCandidateReason(sub, candidateIndex, "is not a subprogram");
            }

            i = candidates.erase(i);
            continue;
        }

        // //////////////////////////////////////////////////////////////
        // step #1: check number of parameters
        // (TODO: ellipsis)

        if (ppAssign.size() > sub->parameters.size()) {
            if (_internalDebugPrintRemoveReason) {
                _printRemovedCandidateReason(
                    sub, candidateIndex, "actual parameter assing number is greather than formal");
            }

            i = candidates.erase(i);
            continue;
        }

        if (tpAssign.size() > sub->templateParameters.size()) {
            if (_internalDebugPrintRemoveReason) {
                _printRemovedCandidateReason(
                    sub, candidateIndex, "actual template parameter assing number is greather than formal");
            }

            i = candidates.erase(i);
            continue;
        }

        // //////////////////////////////////////////////////////////////
        // step #2: check default values of not assigned parameters

        if (!hif::manipulation::sortParameters(
                ppAssign, sub->parameters, true, hif::manipulation::SortMissingKind::ALL, _sem, true)) {
            // If sortParameter fails to add parameters (if trying to match a
            // formal-parameter-default value), this is sure not the right candidate.

            if (_internalDebugPrintRemoveReason) {
                _printRemovedCandidateReason(sub, candidateIndex, "sort parameters fails");
            }

            i = candidates.erase(i);
            continue;
        }

        bool skip = false;
        for (BList<Parameter>::iterator jf = sub->parameters.begin(); jf != sub->parameters.end(); ++jf) {
            if ((*jf)->getValue() != nullptr)
                continue;
            skip = true;

            for (BList<ParameterAssign>::iterator ja = ppAssign.begin(); ja != ppAssign.end(); ++ja) {
                if ((*ja)->getName() != (*jf)->getName())
                    continue;
                skip = false;
                break;
            }
            if (skip)
                break;
        }

        if (skip) {
            if (_internalDebugPrintRemoveReason) {
                _printRemovedCandidateReason(
                    sub, candidateIndex, "not found at least one formal parameter matching actual parmeters name");
            }

            i = candidates.erase(i);
            continue;
        }

        // //////////////////////////////////////////////////////////////
        // step #3: checking if parameters assignable in refSem semantics.
        // step #4: assign scores to candidate declaration.

        unsigned int score = 0;

        // Getting the instantiated signature.
        SubProgram *instantiatedSub = nullptr;
        hif::manipulation::InstantiateOptions instOpt;
        instOpt.onlySignature = true;
        instOpt.candidate     = static_cast<SubProgram *>(*i);

        if (fco != nullptr) {
            instantiatedSub = hif::manipulation::instantiate(fco, _sem, instOpt);
        } else {
            instantiatedSub = hif::manipulation::instantiate(pco, _sem, instOpt);
        }

        if (instantiatedSub == nullptr) {
            if (_internalDebugPrintRemoveReason) {
                _printRemovedCandidateReason(sub, candidateIndex, "method instantiation fails");
            }

            i = candidates.erase(i);
            continue;
        }

        // Try to assign formal and actual params adding scores.
        if (_internalDebugPrintRemoveReason) {
            _printRemovedCandidateReason(
                sub, candidateIndex, "found at least one non-assignable actual parameter value (see details below)");
        }

        BList<Parameter>::iterator formalIt       = instantiatedSub->parameters.begin();
        BList<ParameterAssign>::iterator actualIt = ppAssign.begin();
        int paramIndex                            = 1;
        for (; actualIt != ppAssign.end(); ++formalIt, ++actualIt, ++paramIndex) {
            ParameterAssign *actual = *actualIt;
            Parameter *formal       = *formalIt;
            if (formal == nullptr) {
                skip = true;
                break;
            }

            // No need to copy params, since there is the restore BList.
            Value *actualValue = actual->getValue();
            Type *actualType   = getSemanticType(actualValue, _sem);
            Type *formalType   = formal->getType();

            messageAssert(
                _opt.looseTypeChecks || formalType != nullptr, "Expected formalType: formal is", formal, _sem);
            messageAssert(
                _opt.looseTypeChecks || actualType != nullptr, "Expected actualType: actual is", actualValue, _sem);
            if (formalType == nullptr || actualType == nullptr)
                continue;

            // Ref design: vhdl/gaisler/can_oc
            ILanguageSemantics::ExpressionTypeInfo info = _sem->getExprType(formalType, actualType, op_conv, *actualIt);
            if (info.returnedType == nullptr) {
#if 0
                // ACTIVATE TO DEBUG
                messageDebug("FormalType", formalType, _sem);
                messageDebug("ActualType", actualType, _sem);
#endif

                if (_internalDebugPrintRemoveReason) {
                    std::stringstream sss;
                    sss << paramIndex;
                    std::string message;
                    message += "For parameter #";
                    message += sss.str();
                    message += " \"";
                    message += formal->getName();
                    message += "\" not known conversion from (actual type):";
                    messageDebug(message, actualType, _sem);
                    messageDebug("to (formal type)", formalType, _sem);
                }

                skip = true;
            }

            // Set the score
            score += _calculateScore(formalType, actualType, startingObject);
        }

        // Checking call instance of standard methods to potential increase score
        // in case of scalar caller or same instance type.
        Value *inst = fco ? fco->getInstance() : pco->getInstance();
        if (inst != nullptr && dynamic_cast<Function *>(instantiatedSub) != nullptr &&
            dynamic_cast<LibraryDef *>(sub->getParent()) != nullptr &&
            static_cast<LibraryDef *>(sub->getParent())->isStandard()) {
            Function *f = static_cast<Function *>(instantiatedSub);
            Type *t     = hif::semantics::getSemanticType(inst, _sem);
            messageAssert(_opt.looseTypeChecks || t != nullptr, "Cannot type instance", inst, _sem);

            const bool callerIsScalar =
                (dynamic_cast<Array *>(t) == nullptr && dynamic_cast<Bitvector *>(t) == nullptr &&
                 dynamic_cast<Signed *>(t) == nullptr && dynamic_cast<Unsigned *>(t) == nullptr);

            const bool functionIsScalar =
                (dynamic_cast<Array *>(f->getType()) == nullptr && dynamic_cast<Bitvector *>(f->getType()) == nullptr &&
                 dynamic_cast<Signed *>(f->getType()) == nullptr && dynamic_cast<Unsigned *>(f->getType()) == nullptr);

            if (callerIsScalar != functionIsScalar)
                score += SCALAR;

            if (dynamic_cast<TypeReference *>(t) != nullptr &&
                dynamic_cast<TypeReference *>(getBaseType(t, false, _sem)) != nullptr) {
                // Is a type ref to type TP: the same by construction.
                score += SAME_INSTANCE_TYPE;
            } else {
                if (typeIsLogic(f->getType(), _sem) == typeIsLogic(t, _sem))
                    score += SAME_INSTANCE_TYPE;
            }
        }

        // Checking original candidate adding score if parameters was already sorted.
        // This should fix cases like: foo(int a, char b) and foo(char b, int a).
        // Ref design: vhdl/gaisler/can_oc
        hif::EqualsOptions eopt;
        eopt.checkOnlyNames = true;
        if (hif::equals(ppAssignRestore, ppAssign, eopt))
            score += SORTED_PARAMETERS;

        // Skip is set if at least one actual parameter value is not assignable
        // to the corresponding formal one.
        if (!skip) {
            // Current declaration is a valid candidate
            CandidateInfo c;
            c.score = score;
            c.decl  = *i;
            bestCandidates.push(c);
        } else {
            // Add to worstCandidates list.
            CandidateInfo c;
            c.score = score;
            c.decl  = *i;
            worstCandidates.push(c);
        }

        ++i;
    } // candidates for end.

    // Restoring actual parameters:
    ppAssign.clear();
    ppAssign.merge(ppAssignRestore);

    Declarations normalCandidates;
    _getGreatestCandidates(bestCandidates, normalCandidates);

    if (_opt.getAllAssignables) {
        // we are interested in assignability only, thus return the complete
        // list of possible candidates.
        candidates = normalCandidates;
        return nullptr;
    }

    if (!normalCandidates.empty()) {
        return _checkCandidates<typename T::DeclarationType>(normalCandidates, startingObject);
    }

    // Normal candiadtes is empty.
    // Check if search is mandatory and has worst candidates.
    Declarations worstNormalCandidates;
    _getGreatestCandidates(worstCandidates, worstNormalCandidates);

    if (!worstNormalCandidates.empty() && isMandatory) {
        typename T::DeclarationType *worstCandidate =
            _checkCandidates<typename T::DeclarationType>(worstNormalCandidates, startingObject);
        return worstCandidate;
    }

#if 0
    // ACTIVATE ONE OF THIS TO DEBUG!!
    _internalDebugPrintRemoveReason = true;
    getBestCandidate(originalCandidates, startingObject, isMandatory);
#endif

#ifndef NDEBUG
    if (_internalDebugPrintRemoveReason)
        return nullptr;

    messageDebug(
        "For following call, getBestCandidate return nullptr because all candidates have been removed.", startingObject,
        _sem);

    // //////////////////////////////////////////////////////////////
    // Debug code!
    // All candidates are removed but initial list was not empty!
    // Rise a debug messages with all infos.
    _internalDebugPrintRemoveReason = true;
    getBestCandidate(originalCandidates, startingObject, isMandatory);
    assert(false);
#endif

    return nullptr;
}

template <typename T> T *GetDeclarationVisitor::_checkCandidates(Declarations &candidates, Object *startingObject)
{
    for (Declarations::iterator i = candidates.begin(); i != candidates.end();) {
        Declaration *candidate = *i;
        if (dynamic_cast<T *>(candidate) == nullptr) {
            i = candidates.erase(i);
        } else {
            ++i;
        }
    }

    if (candidates.empty())
        return nullptr;

    Declaration *decl = candidates.front();

    if (candidates.size() == 1) {
        return dynamic_cast<T *>(decl);
    }

    bool isConflicting = false;
    for (Declarations::iterator i = candidates.begin(); i != candidates.end(); ++i) {
        Declaration *dec_i       = *i;
        Scope *scope_i           = hif::getNearestParent<Scope>(dec_i);
        Declarations::iterator j = i;
        ++j;
        for (; j != candidates.end(); ++j) {
            Declaration *dec_j = *j;
            Scope *scope_j     = hif::getNearestParent<Scope>(dec_j);
            if (scope_i != scope_j)
                continue;
            isConflicting = true;
            break;
        }
    }

    if (isConflicting) {
        messageWarning("For " + std::string(decl->getName()) + ", more than one meaning exist.", startingObject, _sem);
#ifndef NDEBUG
        BList<ParameterAssign> *ppAssign = nullptr;
        if (dynamic_cast<FunctionCall *>(startingObject) != nullptr) {
            FunctionCall *fc = static_cast<FunctionCall *>(startingObject);
            ppAssign         = &fc->parameterAssigns;
        } else if (dynamic_cast<ProcedureCall *>(startingObject) != nullptr) {
            ProcedureCall *pc = static_cast<ProcedureCall *>(startingObject);
            ppAssign          = &pc->parameterAssigns;
        }

        if (ppAssign != nullptr) {
            int pcount = 1;
            for (BList<ParameterAssign>::iterator i = ppAssign->begin(); i != ppAssign->end(); ++i, ++pcount) {
                ParameterAssign *pa = *i;
                std::clog << "------------- Param" << pcount << " type:" << std::endl;
                messageDebug("", getSemanticType(pa->getValue(), _sem), _sem);
                std::clog << std::endl;
            }
        }

        std::clog << "################################################" << std::endl;
        for (Declarations::iterator i = candidates.begin(); i != candidates.end(); ++i) {
            std::clog << "PTR: " << *i << std::endl;
            messageDebug("candiate: ", *i, _sem);
            std::clog << std::endl;
        }
        std::clog << "################################################" << std::endl;
#endif
        assert(false);
    }

    return dynamic_cast<T *>(decl);
}

unsigned int GetDeclarationVisitor::_calculateScore(Type *formal, Type *actual, Object *startingObject)
{
    if (hif::equals(actual, formal))
        return EQUALS;
    EqualsOptions opt;
    opt.checkOnlyTypes = true;

    Type *formalBase = getBaseType(formal, false, _sem, false);
    Type *actualBase = getBaseType(actual, false, _sem, false);

    unsigned int ret = 0;
    if (hif::equals(actual, formal, opt)) {
        Value *actualSize = typeGetSpanSize(actual, _sem);
        Value *formalSize = typeGetSpanSize(formal, _sem);
        CompareResult res = compareValues(formalSize, actualSize, _sem, true);

        delete actualSize;
        delete formalSize;

        // we are not loosing precision
        if (res == CompareResult::EQUAL || res == CompareResult::GREATER)
            ret = SAME_SPAN;
        if (res == CompareResult::UNKNOWN)
            ret = SAME_TYPE;
    } else if (hif::equals(actualBase, formalBase, opt)) {
        ret = SAME_BASE;
    }

    // Since they are assignable, it's a standard conversion.
    // Use of implicit user-defined conversion methods (score = 2)
    ILanguageSemantics::ExpressionTypeInfo info = _sem->getExprType(formal, actual, op_conv, startingObject);
    {
        unsigned int flags = ret;
        if (flags == 0)
            flags = ASSIGNABLE;

        // In this case, we are trying to understand the correct signature for
        // overloaded function from the context. Therefore, we cannot consider
        // type flags (e.g., signed/unsigned formal parameter must have the same
        // score to be resolved in case of logic vector actual parameter).
        if (_opt.getAllAssignables)
            return flags;

        // If referring to a template type, skip further checks.
        // Reference design: checker_generator, run() method.
        Type *at = getBaseType(actual, false, _sem, false);
        Type *ft = getBaseType(formal, false, _sem, false);
        if (dynamic_cast<TypeReference *>(at) != nullptr)
            return flags;
        if (dynamic_cast<TypeReference *>(ft) != nullptr)
            return flags;

        if (typeIsSigned(ft, _sem) == typeIsSigned(at, _sem))
            flags += FLAGS;
        if (typeIsLogic(ft, _sem) == typeIsLogic(at, _sem))
            flags += FLAGS;
        if (typeIsResolved(ft, _sem) == typeIsResolved(at, _sem))
            flags += FLAGS;
        if (typeIsConstexpr(ft, _sem) == typeIsConstexpr(at, _sem))
            flags += FLAGS;
        return flags;
    }

    // TODO: ellipsis (score = 1)
    //return 0; // ret still zero
}

void GetDeclarationVisitor::_getGreatestCandidates(CandidateInfoQueue &bestCandidates, Declarations &normalCandidates)
{
    // taking candidates with highest score
    Declarations templateCandidates;
    normalCandidates.clear();
    unsigned int score = 0;
    while (!bestCandidates.empty()) {
        CandidateInfo c = bestCandidates.top();
        bestCandidates.pop();
        if (c.score == score) {
            SubProgram *sub = dynamic_cast<SubProgram *>(c.decl);
            if (sub->templateParameters.empty()) {
                normalCandidates.push_back(c.decl);
            } else {
                templateCandidates.push_back(c.decl);
            }
        } else if (c.score > score) {
            score = c.score;
            normalCandidates.clear();
            templateCandidates.clear();
            SubProgram *sub = dynamic_cast<SubProgram *>(c.decl);
            if (sub->templateParameters.empty()) {
                normalCandidates.push_back(c.decl);
            } else {
                templateCandidates.push_back(c.decl);
            }
        }
    }

#if 0
    std::clog << "Best score is " << score << std::endl;
#endif

    // in case of both template and not template, taking not template.
    if (!normalCandidates.empty())
        templateCandidates.clear();

    // merging the results
    normalCandidates.splice(normalCandidates.end(), templateCandidates);
}

#ifdef NDEBUG
void GetDeclarationVisitor::_printRemovedCandidateReason(Declaration *, const int, const std::string &)
{
    // never called
}
#else
void GetDeclarationVisitor::_printRemovedCandidateReason(
    Declaration *candidate,
    const int index,
    const std::string &removeReason)
{
    std::stringstream ss;
    ss << index;
    std::string message("\n\n=============================================================\n");
    message += "Removed candidate #" + ss.str() + ":";
    message += "\nReason: ";
    message += removeReason;
    messageDebug(message, candidate, _sem);
}
#endif

} // end unnamed namespace

// ///////////////////////////////////////////////////////////////////
// get declaration
// ///////////////////////////////////////////////////////////////////

Declaration *getDeclaration(Object *o, ILanguageSemantics *sem, const DeclarationOptions &opt)
{
    if (o == nullptr)
        return nullptr;

    hif::application_utils::initializeLogHeader("HIF", "getDeclaration");

    GetCandidatesOptions copt(opt);
    GetDeclarationVisitor gdv(sem, nullptr, copt);
    o->acceptVisitor(gdv);

    Declaration *ret = gdv.getResult();
    if (!opt.dontSearch)
        setDeclaration(o, ret);

    hif::application_utils::restoreLogHeader();
    return ret;
}

// ///////////////////////////////////////////////////////////////////
// get candidates options
// ///////////////////////////////////////////////////////////////////

GetCandidatesOptions::GetCandidatesOptions()
    : DeclarationOptions()
    , getAll(false)
    , atLeastOne(false)
    , getAllAssignables(false)
{
    // ntd
}

GetCandidatesOptions::~GetCandidatesOptions()
{
    // ntd
}

GetCandidatesOptions::GetCandidatesOptions(const DeclarationOptions &other)
    : DeclarationOptions(other)
    , getAll(false)
    , atLeastOne(false)
    , getAllAssignables(false)
{
    // ntd
}

GetCandidatesOptions::GetCandidatesOptions(const GetCandidatesOptions &other)
    : DeclarationOptions(other)
    , getAll(other.getAll)
    , atLeastOne(other.atLeastOne)
    , getAllAssignables(other.getAllAssignables)
{
    // ntd
}

GetCandidatesOptions &GetCandidatesOptions::operator=(DeclarationOptions other)
{
    swap(other);
    return *this;
}

GetCandidatesOptions &GetCandidatesOptions::operator=(GetCandidatesOptions other)
{
    swap(other);
    return *this;
}

void GetCandidatesOptions::swap(DeclarationOptions &other) { DeclarationOptions::swap(other); }

void GetCandidatesOptions::swap(GetCandidatesOptions &other)
{
    DeclarationOptions::swap(other);
    std::swap(getAll, other.getAll);
    std::swap(atLeastOne, other.atLeastOne);
    std::swap(getAllAssignables, other.getAllAssignables);
}

// ///////////////////////////////////////////////////////////////////
// get candidates
// ///////////////////////////////////////////////////////////////////
void getCandidates(
    std::list<Declaration *> &list,
    Object *o,
    hif::semantics::ILanguageSemantics *sem,
    const GetCandidatesOptions &opt)
{
    messageAssert(opt.getAll == false || opt.atLeastOne == false, "Invalid options", nullptr, sem);
    if (o == nullptr)
        return;

    hif::application_utils::initializeLogHeader("HIF", "getCandidates");

    GetCandidatesOptions copt(opt);
    copt.forceRefresh = opt.forceRefresh || opt.getAllAssignables;
    GetDeclarationVisitor gdv(sem, &list, copt);
    o->acceptVisitor(gdv);

    if (opt.getAllAssignables) {
        if (dynamic_cast<FunctionCall *>(o) != nullptr) {
            FunctionCall *fc = static_cast<FunctionCall *>(o);
            gdv.getBestCandidate(list, fc, false);
        } else if (dynamic_cast<ProcedureCall *>(o) != nullptr) {
            ProcedureCall *pc = static_cast<ProcedureCall *>(o);
            gdv.getBestCandidate(list, pc, false);
        }
    }

    hif::application_utils::restoreLogHeader();
}

template <typename T>
typename T::DeclarationType *
getDeclaration(T *o, hif::semantics::ILanguageSemantics *sem, const DeclarationOptions &opt)
{
    return dynamic_cast<typename T::DeclarationType *>(getDeclaration(static_cast<Object *>(o), sem, opt));
}

template <typename T>
void getCandidates(
    std::list<typename T::DeclarationType *> &list,
    T *o,
    hif::semantics::ILanguageSemantics *refSem,
    const GetCandidatesOptions &opt)
{
    std::list<Declaration *> *l = reinterpret_cast<std::list<Declaration *> *>(&list);
    getCandidates(*l, static_cast<Object *>(o), refSem, opt);
}

/// @brief Define a templated method `getCandidates` for type `T`.
#define HIF_TEMPLATE_METHOD(T)                                                                                         \
    void getCandidates<T>(                                                                                             \
        std::list<T::DeclarationType *> & list, T * o, hif::semantics::ILanguageSemantics * refSem,                    \
        const GetCandidatesOptions &opt)
HIF_INSTANTIATE_SYMBOLS()
#undef HIF_TEMPLATE_METHOD

/// @brief Define a templated method `getDeclaration` for type `T`.
#define HIF_TEMPLATE_METHOD(T)                                                                                         \
    T::DeclarationType *getDeclaration<T>(                                                                             \
        T * o, hif::semantics::ILanguageSemantics * sem, const DeclarationOptions &opt)
HIF_INSTANTIATE_SYMBOLS()
#undef HIF_TEMPLATE_METHOD

} // namespace semantics
} // namespace hif
