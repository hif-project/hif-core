/// @file transformGlobalActions.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>
#include <sstream>
#include <utility>

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/manipulation/transformGlobalActions.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace
{ // unnamed
class GetSensitivityVisitor : public GuideVisitor
{
public:
    GetSensitivityVisitor(semantics::ILanguageSemantics *sem, bool addVariablesInSensitivity);

    virtual ~GetSensitivityVisitor();

    virtual int visitIdentifier(Identifier &o);
    virtual int visitMember(Member &o);
    virtual int visitSlice(Slice &o);

    BList<Value> *getList();

protected:
    BList<Value> *sensList;
    semantics::ILanguageSemantics *_sem;
    bool _addVariablesInSensitivity;

    GetSensitivityVisitor(const GetSensitivityVisitor &);
    GetSensitivityVisitor &operator=(const GetSensitivityVisitor &);
};

GetSensitivityVisitor::GetSensitivityVisitor(semantics::ILanguageSemantics *sem, bool addVariablesInSensitivity)
    : sensList(new BList<Value>())
    , _sem(sem)
    , _addVariablesInSensitivity(addVariablesInSensitivity)
{
    // ntd
}
GetSensitivityVisitor::~GetSensitivityVisitor()
{
    // ntd
}
int GetSensitivityVisitor::visitIdentifier(Identifier &o)
{
    GuideVisitor::visitIdentifier(o);

    DataDeclaration *deco = hif::semantics::getDeclaration(&o, _sem);

    Port *p     = dynamic_cast<Port *>(deco);
    Signal *s   = dynamic_cast<Signal *>(deco);
    Variable *v = dynamic_cast<Variable *>(deco);

    if (v != nullptr && !_addVariablesInSensitivity)
        return 0;
    if (s == nullptr && p == nullptr && v == nullptr)
        return 0;

    Identifier *copy = hif::copy(&o);
    hif::manipulation::AddUniqueObjectOptions addOpt;
    addOpt.deleteIfNotAdded = true;
    addUniqueObject(copy, *sensList, addOpt);

    return 0;
}

int GetSensitivityVisitor::visitMember(Member &o)
{
    // Only bits relative to member should be considered!
    // TODO
    return GuideVisitor::visitMember(o);
}

int GetSensitivityVisitor::visitSlice(Slice &o)
{
    // Only bits relative to member should be considered!
    // TODO
    return GuideVisitor::visitSlice(o);
}

BList<Value> *GetSensitivityVisitor::getList() { return sensList; }
class GlobactVisitor : public GuideVisitor
{
public:
    GlobactVisitor(
        semantics::ILanguageSemantics *sem,
        std::set<StateTable *> &list,
        bool addVariablesInSensitivity);

    virtual ~GlobactVisitor();

    virtual int visitGlobalAction(GlobalAction &o);

    virtual int visitAssign(Assign &o);

protected:
    void _transformAssignToST(Assign *o);

    void _transformGlobactToST(GlobalAction *o);

    BList<Value> *_getSensitivityList(Object *root);

    semantics::ILanguageSemantics *_sem;
    std::set<StateTable *> &_list;
    bool _addVariablesInSensitivity;

    GlobactVisitor(const GlobactVisitor &);
    GlobactVisitor &operator=(const GlobactVisitor &);
};

GlobactVisitor::GlobactVisitor(
    semantics::ILanguageSemantics *sem,
    std::set<StateTable *> &list,
    bool addVariablesInSensitivity)
    : GuideVisitor()
    , _sem(sem)
    , _list(list)
    , _addVariablesInSensitivity(addVariablesInSensitivity)
{
}

GlobactVisitor::~GlobactVisitor() {}

int GlobactVisitor::visitGlobalAction(GlobalAction &o)
{
    _transformGlobactToST(&o);
    return 0;
}
int GlobactVisitor::visitAssign(Assign &o)
{
    if (dynamic_cast<GlobalAction *>(o.getParent()) == nullptr)
        return 0;
    _transformAssignToST(&o);
    return 0;
}

BList<Value> *GlobactVisitor::_getSensitivityList(Object *root)
{
    GetSensitivityVisitor v(_sem, _addVariablesInSensitivity);
    root->acceptVisitor(v);

    return v.getList();
}
void GlobactVisitor::_transformAssignToST(Assign *ao)
{
    // create state table for action ao.
    StateTable *st = new StateTable();
    st->setName(NameTable::getInstance()->getFreshName("globact_process"));
    State *state = new State();
    state->setName(NameTable::getInstance()->getFreshName("state"));
    st->states.push_back(state);

    // getting names for sensitivity list
    BList<Value> *sensList = _getSensitivityList(ao->getRightHandSide());
    st->sensitivity.merge(*sensList);
    delete sensList;

    // add state table
    static_cast<Contents *>(ao->getParent()->getParent())->stateTables.push_back(st);
    _list.insert(st);

    BList<Action>::iterator it(ao);
    it.remove();

    state->actions.push_back(ao);
}

void GlobactVisitor::_transformGlobactToST(GlobalAction *o)
{
    for (BList<Action>::iterator i = o->actions.begin(); i != o->actions.end();) {
        // sanity checks.
        Assign *ao = dynamic_cast<Assign *>((*i));
        if (ao == nullptr) {
            if (dynamic_cast<ProcedureCall *>(*i) != nullptr) {
                ProcedureCall::DeclarationType *decl =
                    hif::semantics::getDeclaration(static_cast<ProcedureCall *>(*i), _sem);
                messageAssert(
                    (decl != nullptr && decl->getKind() == SubProgram::MACRO),
                    "Found ProcedureCall as GlobalAction with unexpected declaration (MACRO is expected).", *i, _sem);
                ++i;
                continue;
            }
            messageError("Global actions different from assign are not supported.", o, _sem);
        }

        ++i;
        _transformAssignToST(ao);
    }
}

} // namespace
// //////////////////////////////////////////////////////////////////////////////////////
// transformGlobalActions method.
// //////////////////////////////////////////////////////////////////////////////////////
void transformGlobalActions(
    Object *o,
    std::set<StateTable *> &list,
    semantics::ILanguageSemantics *sem,
    bool addVariablesInSensitivity)
{
    if (o == nullptr)
        return;

    hif::application_utils::initializeLogHeader("HIF", "transform_globact");

    GlobactVisitor v(sem, list, addVariablesInSensitivity);
    o->acceptVisitor(v);

    hif::application_utils::restoreLogHeader();
}
} // namespace manipulation
} // namespace hif
