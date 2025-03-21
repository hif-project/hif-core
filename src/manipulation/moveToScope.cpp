/// @file moveToScope.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/moveToScope.hpp"

#include "hif/BiVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-member-function"
#    pragma clang diagnostic ignored "-Wmissing-noreturn"
#elif defined __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

// Removing from old list (without deleting) and push back in new list.
template <typename T> void _actualMove(T *obj, BList<T> &destList)
{
    messageAssert(obj->isInBList(), "Unexpected object not in BList", obj, nullptr);
    typename BList<T>::iterator it(obj);
    it.remove();
    destList.push_back(obj);
}

// Move an object contained in a list of a scope to the corresponding
// list of another scope.
// In case of declarations, update the references in the old scope with
// the new name of the object.
template <typename S1, typename S2, typename T>
void _moveToScope(
    S1 *oldScope,
    S2 *newScope,
    BList<T> & /*sourceList*/,
    BList<T> &destList,
    hif::semantics::ILanguageSemantics *sem,
    const std::string &suffixToAppend,
    T *obj)
{
    messageAssert(obj != nullptr, "Unxpected nullptr object", nullptr, sem);

    // If the involved object/list does not concern a declaration,
    // or an object with a name, this is a simple move.
    Declaration *decl = dynamic_cast<Declaration *>(obj);
    Instance *inst    = dynamic_cast<Instance *>(obj);
    if (decl == nullptr && inst == nullptr) {
        _actualMove(obj, destList);
        return;
    }

    // If it is an instance, just assure a unique name.
    if (inst != nullptr) {
        std::string newName = inst->getName();
        if (suffixToAppend != "") {
            std::string n(inst->getName());
            newName = n + suffixToAppend;
        }
        if (checkConflictingName(newName, newScope)) {
            newName = NameTable::getInstance()->getFreshName(newName);
        }
        inst->setName(newName);
        _actualMove(obj, destList);
        return;
    }

    // Case of declarations.

    // If a suffix has been passed, always set it.
    std::string newName = decl->getName();
    if (suffixToAppend != "") {
        std::string n(decl->getName());
        newName = n + suffixToAppend;
    }

    const bool isConflicting = checkConflictingName(newName, newScope);
    // If it does not conflict, this is a simple move.
    if (!isConflicting && suffixToAppend == "") {
        _actualMove(obj, destList);
        return;
    }

    // If conflicts, renaming (with fresh name).
    if (isConflicting) {
        newName = NameTable::getInstance()->getFreshName(decl->getName());
    }

    // Before move, get declaration references and update them.

    // Update its references.
    hif::semantics::ReferencesSet referencesList;
    hif::semantics::getReferences(decl, referencesList, sem, oldScope);

    for (hif::semantics::ReferencesSet::iterator i = referencesList.begin(); i != referencesList.end(); ++i) {
        objectSetName(*i, newName);
    }

    // Update the declaration.
    decl->setName(newName);
    _actualMove(obj, destList);
}
/// @brief Warning: move semantics.
/// Does not have ownership.
struct MoveData {
    hif::semantics::ILanguageSemantics *sem;
    std::string suffixToAppend;
    Object *objToMove;

    MoveData();
    MoveData(const MoveData &d);
    ~MoveData();
    MoveData &operator=(const MoveData &d);
};

MoveData::MoveData()
    : sem(nullptr)
    , suffixToAppend()
    , objToMove(nullptr)
{
    // Nothing to do.
}

MoveData::MoveData(const MoveData &d)
    : sem(d.sem)
    , suffixToAppend(d.suffixToAppend)
    , objToMove(d.objToMove)
{
    // Nothing to do.
}

MoveData::~MoveData()
{
    // Nothing to do.
}

MoveData &MoveData::operator=(const MoveData &d)
{
    if (this == &d)
        return *this;
    sem            = d.sem;
    suffixToAppend = d.suffixToAppend;
    objToMove      = d.objToMove;
    return *this;
}

// Map used by function moveToScope, access point to function _moveToScope.
class MoveToScopeVisitor : public BiVisitor<MoveToScopeVisitor>
{
public:
    MoveToScopeVisitor(MoveData &data);
    virtual ~MoveToScopeVisitor();

    void map(BaseContents *from, BaseContents *to);
    void map(BaseContents *from, View *to);
    void map(Contents *from, Contents *to);
    void map(Contents *from, View *to);
    void map(Entity *from, View *to);
    void map(For *from, Scope *to);
    void map(Object *from, Object *to);
    void map(StateTable *from, LibraryDef *to);
    void map(StateTable *from, View *to);
    void map(StateTable *from, BaseContents *to);
    void map(StateTable *from, SubProgram *to);
    void map(StateTable *from, StateTable *to);
    void map(View *from, View *to);
    void map(Entity *from, Entity *to);

private:
    MoveData _data;

    // disabled.
    MoveToScopeVisitor(const MoveToScopeVisitor &);
    MoveToScopeVisitor &operator=(const MoveToScopeVisitor &);
};
// Main call
MoveToScopeVisitor::MoveToScopeVisitor(MoveData &data)
    : _data(data)
{
    // ntd
}

MoveToScopeVisitor::~MoveToScopeVisitor()
{
    // ntd
}

// Map calls
void MoveToScopeVisitor::map(Object * /*from*/, Object * /*to*/)
{
    messageError("Unexpected objects reached by MoveToScope_t", nullptr, nullptr);
}

void MoveToScopeVisitor::map(Contents *from, Contents *to)
{
    // If obj is null, all contents has to be moved.
    if (_data.objToMove == nullptr) {
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->libraries);
        // Call the superclass (BaseContents) map.
        map(static_cast<BaseContents *>(from), static_cast<BaseContents *>(to));
        return;
    }

    // If only the obj must be moved, and it is contained in libraries.
    if (&from->libraries == reinterpret_cast<BList<Library> *>(_data.objToMove->getBList())) {
        Library *casted = static_cast<Library *>(_data.objToMove);
        messageAssert(casted != nullptr, "Unexpected obj to move", _data.objToMove, _data.sem);
        _moveToScope(from, to, from->libraries, to->libraries, _data.sem, _data.suffixToAppend, casted);
        return;
    }

    // If only the obj must be moved, but it is contained in one of the
    // superclass (BaseContents) lists.
    map(static_cast<BaseContents *>(from), static_cast<BaseContents *>(to));
}

void MoveToScopeVisitor::map(BaseContents *from, BaseContents *to)
{
    // If obj is null, all contents has to be moved.
    // The order of moves has effect on the update of references.
    if (_data.objToMove == nullptr) {
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->declarations);
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->generates);
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->stateTables);
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->instances);
        if (to->getGlobalAction() == nullptr)
            to->setGlobalAction(new GlobalAction());
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->getGlobalAction()->actions);
        return;
    }

    // Move all the global actions.
    if (_data.objToMove == from->getGlobalAction()) {
        if (to->getGlobalAction() == nullptr)
            to->setGlobalAction(new GlobalAction());
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->getGlobalAction()->actions);
        return;
    }

    // Else, the object is contained in one of the BLists.
    BList<Object> *objList = _data.objToMove->getBList();
    messageAssert(objList != nullptr, "Unexpected object not in Blist", _data.objToMove, _data.sem);

    if (&from->declarations == reinterpret_cast<BList<Declaration> *>(objList)) {
        Declaration *casted = static_cast<Declaration *>(_data.objToMove);
        _moveToScope(from, to, from->declarations, to->declarations, _data.sem, _data.suffixToAppend, casted);
    } else if (&from->generates == reinterpret_cast<BList<Generate> *>(objList)) {
        Generate *casted = static_cast<Generate *>(_data.objToMove);
        _moveToScope(from, to, from->generates, to->generates, _data.sem, _data.suffixToAppend, casted);
    } else if (&from->stateTables == reinterpret_cast<BList<StateTable> *>(objList)) {
        StateTable *casted = static_cast<StateTable *>(_data.objToMove);
        _moveToScope(from, to, from->stateTables, to->stateTables, _data.sem, _data.suffixToAppend, casted);
    } else if (&from->instances == reinterpret_cast<BList<Instance> *>(objList)) {
        Instance *casted = static_cast<Instance *>(_data.objToMove);
        _moveToScope(from, to, from->instances, to->instances, _data.sem, _data.suffixToAppend, casted);
    } else if (
        from->getGlobalAction() != nullptr &&
        &from->getGlobalAction()->actions == reinterpret_cast<BList<Action> *>(objList)) {
        if (to->getGlobalAction() == nullptr) {
            to->setGlobalAction(new GlobalAction());
        }
        Action *casted = static_cast<Action *>(_data.objToMove);
        _moveToScope(
            from, to, from->getGlobalAction()->actions, to->getGlobalAction()->actions, _data.sem, _data.suffixToAppend,
            casted);
    } else {
        messageDebugAssert(false, "Unexpected case", nullptr, _data.sem); // Sanity check.
    }
}
void MoveToScopeVisitor::map(StateTable *from, LibraryDef *to)
{
    // If obj is null, all contents has to be moved.
    // The order of moves has effect on the update of references.
    if (_data.objToMove == nullptr) {
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->declarations);
        return;
    }

    // Else, the object is contained in one of the BLists.
    BList<Object> *objList = _data.objToMove->getBList();
    messageAssert(objList != nullptr, "Unexpected object not in blist", _data.objToMove, _data.sem);

    if (&from->declarations == reinterpret_cast<BList<Declaration> *>(objList)) {
        Declaration *casted = static_cast<Declaration *>(_data.objToMove);
        _moveToScope(from, to, from->declarations, to->declarations, _data.sem, _data.suffixToAppend, casted);
    } else {
        // Sanity check.
        messageDebugAssert(false, "Unexpected case", nullptr, nullptr);
    }
}

void MoveToScopeVisitor::map(StateTable *from, View *to)
{
    messageAssert(to->getContents() != nullptr, "Unexpected case", to, _data.sem);
    map(from, to->getContents());
}

void MoveToScopeVisitor::map(StateTable *from, BaseContents *to)
{
    // If obj is null, all contents has to be moved.
    // The order of moves has effect on the update of references.
    if (_data.objToMove == nullptr) {
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->declarations);
        return;
    }

    // Else, the object is contained in one of the BLists.
    BList<Object> *objList = _data.objToMove->getBList();
    messageAssert(objList != nullptr, "Unexpected object not in blist", _data.objToMove, _data.sem);

    if (&from->declarations == reinterpret_cast<BList<Declaration> *>(objList)) {
        Declaration *casted = static_cast<Declaration *>(_data.objToMove);
        _moveToScope(from, to, from->declarations, to->declarations, _data.sem, _data.suffixToAppend, casted);
    } else {
        // Sanity check.
        messageDebugAssert(false, "Unexpected case", nullptr, nullptr);
    }
}
void MoveToScopeVisitor::map(StateTable *from, SubProgram *to)
{
    // Already in the proper scope.
    if (from == to->getStateTable())
        return;

    map(from, to->getStateTable());
}
void MoveToScopeVisitor::map(StateTable *from, StateTable *to)
{
    // If obj is null, all contents has to be moved.
    // The order of moves has effect on the update of references.
    if (_data.objToMove == nullptr) {
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->declarations);
        return;
    }

    // Else, the object is contained in one of the BLists.
    BList<Object> *objList = _data.objToMove->getBList();
    messageAssert(objList != nullptr, "Unexpected object not in blist", _data.objToMove, _data.sem);

    if (&from->declarations == reinterpret_cast<BList<Declaration> *>(objList)) {
        Declaration *casted = static_cast<Declaration *>(_data.objToMove);
        _moveToScope(from, to, from->declarations, to->declarations, _data.sem, _data.suffixToAppend, casted);
    } else {
        // Sanity check.
        messageDebugAssert(false, "Unexpected case", nullptr, nullptr);
    }
}
void MoveToScopeVisitor::map(For *from, Scope *to)
{
    Contents fake;

    if (_data.objToMove == nullptr) {
        fake.declarations.merge(from->initDeclarations.toOtherBList<Declaration>());
    } else {
        if (_data.objToMove->isInBList()) {
            BList<Object>::iterator it(_data.objToMove);
            it.remove();
        }
        fake.declarations.push_back(dynamic_cast<Declaration *>(_data.objToMove));
    }

    hif::manipulation::moveToScope(&fake, to, _data.sem, _data.suffixToAppend, fake.declarations);
}
void MoveToScopeVisitor::map(Contents *from, View *to)
{
    // If obj is null, all contents has to be moved.
    if (_data.objToMove == nullptr) {
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->libraries);
        // Call the superclass (BaseContents) map.
        map(static_cast<BaseContents *>(from), to);
        return;
    }

    // If only the obj must be moved, and it is contained in libraries.
    if (&from->libraries == reinterpret_cast<BList<Library> *>(_data.objToMove->getBList())) {
        Library *casted = static_cast<Library *>(_data.objToMove);
        messageAssert(casted != nullptr, "Unexpected obj to move", _data.objToMove, _data.sem);
        _moveToScope(from, to, from->libraries, to->libraries, _data.sem, _data.suffixToAppend, casted);
        return;
    }

    // If only the obj must be moved, but it is contained in one of the
    // superclass (BaseContents) lists.
    map(static_cast<BaseContents *>(from), to);
}
void MoveToScopeVisitor::map(Entity *from, View *to)
{
    messageAssert(
        from->getParent() == to, "At the moment, move to scope is not supported from Entity to View.", to, _data.sem);

    // Nothing to do, since in the same scope...
}
void MoveToScopeVisitor::map(BaseContents *from, View *to)
{
    // If obj is null, all contents has to be moved.
    // The order of moves has effect on the update of references.
    if (_data.objToMove == nullptr) {
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->declarations);

        // Generates cannot be moved.
        if (!from->generates.empty()) {
            map(static_cast<Object *>(from), static_cast<Object *>(to));
        }

        // Statetable cannot be moved.
        if (!from->stateTables.empty()) {
            map(static_cast<Object *>(from), static_cast<Object *>(to));
        }

        // Instances cannot be moved.
        if (!from->instances.empty()) {
            map(static_cast<Object *>(from), static_cast<Object *>(to));
        }

        // GlobalActions cannot be moved.
        if (from->getGlobalAction() != nullptr && !from->getGlobalAction()->actions.empty()) {
            map(static_cast<Object *>(from), static_cast<Object *>(to));
        }
        return;
    }

    // Move all the global actions.
    if (_data.objToMove == from->getGlobalAction()) {
        // GlobalActions cannot be moved.
        map(static_cast<Object *>(from), static_cast<Object *>(to));
    }

    // Else, the object is contained in one of the BLists.
    BList<Object> *objList = _data.objToMove->getBList();
    messageAssert(objList != nullptr, "Unexpected object not in blist", _data.objToMove, _data.sem);

    if (&from->declarations == reinterpret_cast<BList<Declaration> *>(objList)) {
        Declaration *casted = static_cast<Declaration *>(_data.objToMove);
        _moveToScope(from, to, from->declarations, to->declarations, _data.sem, _data.suffixToAppend, casted);
    } else if (&from->generates == reinterpret_cast<BList<Generate> *>(objList)) {
        // Generates cannot be moved.
        map(static_cast<Object *>(from), static_cast<Object *>(to));
    } else if (&from->stateTables == reinterpret_cast<BList<StateTable> *>(objList)) {
        // Statetable cannot be moved.
        map(static_cast<Object *>(from), static_cast<Object *>(to));
    } else if (&from->instances == reinterpret_cast<BList<Instance> *>(objList)) {
        // Instances cannot be moved.
        map(static_cast<Object *>(from), static_cast<Object *>(to));
    } else if (
        from->getGlobalAction() != nullptr &&
        &from->getGlobalAction()->actions == reinterpret_cast<BList<Action> *>(objList)) {
        // GlobalActions cannot be moved.
        map(static_cast<Object *>(from), static_cast<Object *>(to));
    } else {
        messageDebugAssert(false, "Unexpected case", nullptr, nullptr);
        // Sanity check.
    }
}

void MoveToScopeVisitor::map(View *from, View *to)
{
    if (_data.objToMove == nullptr) {
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->declarations);

        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->inheritances);

        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->libraries);

        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->templateParameters);

        map(from->getContents(), to->getContents());
        map(from->getEntity(), to->getEntity());

        return;
    }

    BList<Object> *objList = _data.objToMove->getBList();
    messageAssert(objList != nullptr, "Unexpected object not in blist", _data.objToMove, _data.sem);

    if (&from->declarations == reinterpret_cast<BList<Declaration> *>(objList)) {
        Declaration *casted = static_cast<Declaration *>(_data.objToMove);
        _moveToScope(from, to, from->declarations, to->declarations, _data.sem, _data.suffixToAppend, casted);
    } else if (&from->inheritances == reinterpret_cast<BList<ViewReference> *>(objList)) {
        ViewReference *casted = static_cast<ViewReference *>(_data.objToMove);
        _moveToScope(from, to, from->inheritances, to->inheritances, _data.sem, _data.suffixToAppend, casted);
    } else if (&from->libraries == reinterpret_cast<BList<Library> *>(objList)) {
        Library *casted = static_cast<Library *>(_data.objToMove);
        _moveToScope(from, to, from->libraries, to->libraries, _data.sem, _data.suffixToAppend, casted);
    } else if (&from->templateParameters == reinterpret_cast<BList<Declaration> *>(objList)) {
        Declaration *casted = static_cast<Declaration *>(_data.objToMove);
        _moveToScope(
            from, to, from->templateParameters, to->templateParameters, _data.sem, _data.suffixToAppend, casted);
    } else {
        messageDebugAssert(false, "Unexpected case", nullptr, nullptr);
    }
}

void MoveToScopeVisitor::map(Entity *from, Entity *to)
{
    if (_data.objToMove == nullptr) {
        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->parameters);

        hif::manipulation::moveToScope(from, to, _data.sem, _data.suffixToAppend, from->ports);
        return;
    }

    BList<Object> *objList = _data.objToMove->getBList();
    messageAssert(objList != nullptr, "Unexpected object not in blist", _data.objToMove, _data.sem);
    if (&from->ports == reinterpret_cast<BList<Port> *>(objList)) {
        // Ports cannot be moved.
        map(static_cast<Object *>(from), static_cast<Object *>(to));
    } else if (&from->parameters == reinterpret_cast<BList<Parameter> *>(objList)) {
        // Generates cannot be moved.
        map(static_cast<Object *>(from), static_cast<Object *>(to));
    } else {
        messageDebugAssert(false, "Unexpected case", nullptr, nullptr);
    }
}
} // namespace

void moveToScope(
    Scope *oldScope,
    Scope *newScope,
    hif::semantics::ILanguageSemantics *sem,
    const std::string &suffix,
    Object *obj)
{
    if (oldScope == newScope)
        return;

    MoveData data;
    data.sem            = sem;
    data.suffixToAppend = suffix;
    data.objToMove      = obj;
    MoveToScopeVisitor mtsv(data);
    mtsv.callMap(oldScope, newScope);
}
void moveToScope(
    For *oldScope,
    Scope *newScope,
    hif::semantics::ILanguageSemantics *sem,
    const std::string &suffix,
    Object *obj)
{
    MoveData data;
    data.sem            = sem;
    data.suffixToAppend = suffix;
    data.objToMove      = obj;
    MoveToScopeVisitor mtsv(data);
    mtsv.callMap(oldScope, newScope);
}

} // namespace manipulation
} // namespace hif
