/// @file referencesUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/referencesUtils.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace semantics
{

namespace /*anon*/
{

/****************************************************************************/
/*	Get references visitor													*/
/****************************************************************************/
class GetReferencesVisitor : public GuideVisitor
{
public:
    GetReferencesVisitor(
        Declaration *declaration,
        hif::semantics::ReferencesSet &list,
        ILanguageSemantics *refSem,
        const GetReferencesOptions &opt);

    GetReferencesVisitor(
        hif::semantics::ReferencesMap &refMap,
        ILanguageSemantics *refSem,
        const GetReferencesOptions &opt);

    ~GetReferencesVisitor();

    virtual bool BeforeVisit(Object &o);

    virtual int visitIdentifier(Identifier &);
    virtual int visitParameterAssign(ParameterAssign &);
    virtual int visitPortAssign(PortAssign &);

    virtual int visitFieldReference(FieldReference &o);
    virtual int visitFunctionCall(FunctionCall &);
    virtual int visitInstance(Instance &o);
    virtual int visitLibrary(Library &);
    virtual int visitProcedureCall(ProcedureCall &);
    virtual int visitTypeReference(TypeReference &);
    virtual int visitTypeTPAssign(TypeTPAssign &);
    virtual int visitValueTPAssign(ValueTPAssign &);
    virtual int visitViewReference(ViewReference &);

    virtual int visitAlias(Alias &);
    virtual int visitConst(Const &);
    virtual int visitEntity(Entity &);
    virtual int visitEnumValue(EnumValue &);
    virtual int visitField(Field &);
    virtual int visitFunction(Function &);
    virtual int visitLibraryDef(LibraryDef &);
    virtual int visitParameter(Parameter &);
    virtual int visitPort(Port &);
    virtual int visitProcedure(Procedure &);
    virtual int visitTypeDef(TypeDef &);
    virtual int visitTypeTP(TypeTP &);
    virtual int visitSignal(Signal &);
    virtual int visitValueTP(ValueTP &);
    virtual int visitVariable(Variable &);
    virtual int visitView(View &);

private:
    Declaration *_declaration;
    hif::semantics::ReferencesSet *_list;
    hif::semantics::ReferencesMap *_map;

    ILanguageSemantics *_refSem;

    const GetReferencesOptions &_opt;

    template <typename T> void _addReference(T *o);

    void _addDeclaration(Declaration *decl);

    // K: disabled
    GetReferencesVisitor(const GetReferencesVisitor &);
    GetReferencesVisitor &operator=(const GetReferencesVisitor &);
};

GetReferencesVisitor::GetReferencesVisitor(
    ReferencesMap &refMap,
    ILanguageSemantics *refSem,
    const GetReferencesOptions &opt)
    : GuideVisitor()
    , _declaration(nullptr)
    , _list(nullptr)
    , _map(&refMap)
    , _refSem(refSem)
    , _opt(opt)
{
    // ntd
}

GetReferencesVisitor::GetReferencesVisitor(
    Declaration *declaration,
    ReferencesSet &list,
    ILanguageSemantics *refSem,
    const GetReferencesOptions &opt)
    : GuideVisitor()
    , _declaration(declaration)
    , _list(&list)
    , _map(nullptr)
    , _refSem(refSem)
    , _opt(opt)
{
    // ntd
}

GetReferencesVisitor::~GetReferencesVisitor()
{
    // ntd
}

bool GetReferencesVisitor::BeforeVisit(Object & /*o*/)
{
    if (!_opt.only_first)
        return false;

    if (_declaration != nullptr) {
        return !_list->empty();
    }

    return false;
}

template <typename T> void GetReferencesVisitor::_addReference(T *o)
{
    if (_declaration != nullptr) {
        if (_declaration->getName() != o->getName())
            return;
        Declaration *decl = getDeclaration(o, _refSem);
        if (decl != _declaration)
            return;
        if (_opt.only_first && !_list->empty())
            return;
        if (_opt.check_object_method != nullptr && !_opt.check_object_method(o, _refSem, _opt))
            return;
        _list->insert(o);
    } else {
        Declaration *decl = getDeclaration(o, _refSem);

        if (_opt.skip_standard_declarations && decl != nullptr && declarationIsPartOfStandard(decl))
            return;

        if (dynamic_cast<Instance *>(o) != nullptr) {
            // Special case: if instance is used as scope operator, i.e., its
            // declaration should be a LibraryDef or a TypeDef, it can be null.
            Instance *inst = static_cast<Instance *>(static_cast<Object *>(o));
            if (dynamic_cast<ViewReference *>(inst->getReferencedType()) == nullptr)
                return;
        }

        if (decl == nullptr) {
            messageAssert(!_opt.error, "Declaration not found.", o, _refSem);
            return;
        }

        if (_opt.only_first && _map->find(decl) != _map->end() && !(*_map)[decl].empty())
            return;

        if (_opt.check_object_method != nullptr && !_opt.check_object_method(o, _refSem, _opt))
            return;

        (*_map)[decl].insert(o);
    }
}

void GetReferencesVisitor::_addDeclaration(Declaration *decl)
{
    if ((_declaration != nullptr) || !_opt.include_unreferenced)
        return;

    if (_opt.check_object_method != nullptr && !_opt.check_object_method(decl, _refSem, _opt))
        return;

    (*_map)[decl];
}

int GetReferencesVisitor::visitIdentifier(Identifier &o)
{
    GuideVisitor::visitIdentifier(o);
    _addReference(&o);
    return 0;
}

int GetReferencesVisitor::visitParameterAssign(ParameterAssign &o)
{
    GuideVisitor::visitParameterAssign(o);
    _addReference(&o);
    return 0;
}

int GetReferencesVisitor::visitPortAssign(PortAssign &o)
{
    GuideVisitor::visitPortAssign(o);
    _addReference(&o);
    return 0;
}

int GetReferencesVisitor::visitFieldReference(FieldReference &o)
{
    GuideVisitor::visitFieldReference(o);
    // TODO change as other methods when GetName will be GetName
    if (_declaration != nullptr) {
        if (_declaration->getName() != o.getName())
            return 0;
        Declaration *decl = getDeclaration(&o, _refSem);
        if (decl != _declaration)
            return 0;
        if (_opt.only_first && !_list->empty())
            return 0;
        if (_opt.check_object_method != nullptr && !_opt.check_object_method(&o, _refSem, _opt))
            return 0;
        _list->insert(&o);
    } else {
        Declaration *decl = getDeclaration(&o, _refSem);
        if (decl == nullptr) {
#ifndef NDEBUG
            // In case of constant record prefix, getDeclaration() returns fails.
            if (dynamic_cast<RecordValue *>(o.getPrefix()) == nullptr) {
                messageAssert(false, "Unexpected non-record value", &o, _refSem);
            }
#endif
            return 0;
        }

        if (_opt.only_first && _map->find(decl) != _map->end() && !(*_map)[decl].empty())
            return 0;

        if (_opt.check_object_method != nullptr && !_opt.check_object_method(&o, _refSem, _opt))
            return 0;

        (*_map)[decl].insert(&o);
    }
    return 0;
}

int GetReferencesVisitor::visitInstance(Instance &o)
{
    GuideVisitor::visitInstance(o);
    _addReference(&o);
    return 0;
}

int GetReferencesVisitor::visitLibrary(Library &o)
{
    GuideVisitor::visitLibrary(o);
    _addReference(&o);
    return 0;
}

int GetReferencesVisitor::visitFunctionCall(FunctionCall &o)
{
    GuideVisitor::visitFunctionCall(o);
    _addReference(&o);
    return 0;
}

int GetReferencesVisitor::visitProcedureCall(ProcedureCall &o)
{
    GuideVisitor::visitProcedureCall(o);
    _addReference(&o);
    return 0;
}

int GetReferencesVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    GuideVisitor::visitTypeTPAssign(o);
    _addReference(&o);
    return 0;
}

int GetReferencesVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    GuideVisitor::visitValueTPAssign(o);
    _addReference(&o);
    return 0;
}

int GetReferencesVisitor::visitTypeReference(TypeReference &o)
{
    GuideVisitor::visitTypeReference(o);
    _addReference(&o);
    return 0;
}

int GetReferencesVisitor::visitViewReference(ViewReference &o)
{
    GuideVisitor::visitViewReference(o);
    _addReference(&o);
    return 0;
}

int GetReferencesVisitor::visitAlias(Alias &o)
{
    GuideVisitor::visitAlias(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitConst(Const &o)
{
    GuideVisitor::visitConst(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitEntity(Entity &o)
{
    GuideVisitor::visitEntity(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitEnumValue(EnumValue &o)
{
    GuideVisitor::visitEnumValue(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitField(Field &o)
{
    GuideVisitor::visitField(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitFunction(Function &o)
{
    GuideVisitor::visitFunction(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitLibraryDef(LibraryDef &o)
{
    GuideVisitor::visitLibraryDef(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitParameter(Parameter &o)
{
    GuideVisitor::visitParameter(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitPort(Port &o)
{
    GuideVisitor::visitPort(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitProcedure(Procedure &o)
{
    GuideVisitor::visitProcedure(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitTypeDef(TypeDef &o)
{
    GuideVisitor::visitTypeDef(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitTypeTP(TypeTP &o)
{
    GuideVisitor::visitTypeTP(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitSignal(Signal &o)
{
    GuideVisitor::visitSignal(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitValueTP(ValueTP &o)
{
    GuideVisitor::visitValueTP(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitVariable(Variable &o)
{
    GuideVisitor::visitVariable(o);
    _addDeclaration(&o);
    return 0;
}

int GetReferencesVisitor::visitView(View &o)
{
    GuideVisitor::visitView(o);
    _addDeclaration(&o);
    return 0;
}

} // namespace

// ///////////////////////////////////////////////////////////////////
// References methods
// ///////////////////////////////////////////////////////////////////
void getReferences(
    Declaration *decl,
    ReferencesSet &list,
    ILanguageSemantics *refSem,
    Object *root,
    const GetReferencesOptions &opt)
{
    hif::application_utils::initializeLogHeader("HIF", "getReferences");

    if (root == nullptr)
        root = hif::getNearestParent<System>(decl);
    messageAssert(root != nullptr, "Cannot find system object", nullptr, refSem);

    GetReferencesVisitor grv(decl, list, refSem, opt);
    root->acceptVisitor(grv);

    hif::application_utils::restoreLogHeader();
}

void getReferences(
    Declaration *decl,
    ReferencesSet &list,
    ILanguageSemantics *refSem,
    BList<Object> &root,
    const hif::semantics::GetReferencesOptions &opt)
{
    for (BList<Object>::iterator i = root.begin(); i != root.end(); ++i) {
        getReferences(decl, list, refSem, *i, opt);
    }
}

template <typename T>
void getReferences(
    Declaration *decl,
    ReferencesSet &list,
    ILanguageSemantics *refSem,
    BList<T> &root,
    const GetReferencesOptions &opt)
{
    getReferences(decl, list, refSem, root.template toOtherBList<Object>(), opt);
}
void getAllReferences(ReferencesMap &refMap, ILanguageSemantics *refSem, Object *root, const GetReferencesOptions &opt)
{
    hif::application_utils::initializeLogHeader("HIF", "getAllReferences");
    messageAssert(root != nullptr, "Passed nullptr root", nullptr, nullptr);

    GetReferencesVisitor grv(refMap, refSem, opt);
    root->acceptVisitor(grv);

    hif::application_utils::restoreLogHeader();
}

void getAllReferences(
    ReferencesMap &refMap,
    ILanguageSemantics *refSem,
    BList<Object> &root,
    const GetReferencesOptions &opt)
{
    for (BList<Object>::iterator i = root.begin(); i != root.end(); ++i) {
        getAllReferences(refMap, refSem, *i, opt);
    }
}

template <typename T>
void getAllReferences(
    ReferencesMap &refMap,
    ILanguageSemantics *refSem,
    BList<T> &root,
    const GetReferencesOptions &opt)
{
    getAllReferences(refMap, refSem, root.template toOtherBList<Object>(), opt);
}

#define HIF_TEMPLATE_METHOD(T)                                                                                         \
    void getReferences<T>(                                                                                             \
        Declaration *, ReferencesSet &, ILanguageSemantics *, BList<T> &, const GetReferencesOptions &)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

#define HIF_TEMPLATE_METHOD(T)                                                                                         \
    void getAllReferences<T>(ReferencesMap &, ILanguageSemantics *, BList<T> &, const GetReferencesOptions &)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD
} // namespace semantics
} // namespace hif
