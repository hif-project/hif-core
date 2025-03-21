/// @file updateDeclarations.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/updateDeclarations.hpp"

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
/*	Update declaration visitor												*/
/****************************************************************************/
class UpdateDeclarationsVisitor : public GuideVisitor
{
public:
    UpdateDeclarationsVisitor(ILanguageSemantics *sem, const UpdateDeclarationOptions &opt);
    ~UpdateDeclarationsVisitor();

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

private:
    ILanguageSemantics *_sem;
    const UpdateDeclarationOptions &_opt;

    void checkFound(Declaration *decl, Object &obj, const bool mustBe = true);

    template <typename T> typename T::DeclarationType *_getDeclaration(T *obj);

    // K: disabled
    UpdateDeclarationsVisitor(const UpdateDeclarationsVisitor &);
    UpdateDeclarationsVisitor &operator=(const UpdateDeclarationsVisitor &);
};

UpdateDeclarationsVisitor::UpdateDeclarationsVisitor(ILanguageSemantics *sem, const UpdateDeclarationOptions &opt)
    : GuideVisitor()
    , _sem(sem)
    , _opt(opt)
{
    // ntd
}

UpdateDeclarationsVisitor::~UpdateDeclarationsVisitor()
{
    // ntd
}

void UpdateDeclarationsVisitor::checkFound(Declaration *decl, Object &obj, const bool mustBe)
{
    if (!_opt.error)
        return;
    if (decl != nullptr && mustBe)
        return;
    if (decl == nullptr && !mustBe)
        return;

    // rise error
    if (mustBe) {
        messageError("Not found expected declaration of object.", &obj, _sem);
    }
}

template <typename T> typename T::DeclarationType *UpdateDeclarationsVisitor::_getDeclaration(T *obj)
{
    typename T::DeclarationType *oldDecl = nullptr;
    if (_opt.onlyVisible) {
        DeclarationOptions opt(_opt);
        opt.dontSearch = true;
        oldDecl        = getDeclaration(obj, _sem, opt);
        //if (oldDecl == nullptr) return nullptr;
        if (oldDecl != nullptr && _opt.root != nullptr && !hif::isSubNode(oldDecl, _opt.root)) {
            // Old decl not child of root:
            // preserve oldDecl
            setDeclaration(obj, oldDecl);
            return oldDecl;
        }
        setDeclaration(obj, nullptr);
    }

    DeclarationOptions opt(_opt);
    typename T::DeclarationType *newDecl = getDeclaration(obj, _sem, opt);

    if (_opt.onlyVisible) {
        if (newDecl == nullptr) {
            // not found, restore old declaration.
            setDeclaration(obj, oldDecl);
            return oldDecl;
        } else {
            // keep new declaration.
            return newDecl;
        }
    }

    return newDecl;
}

int UpdateDeclarationsVisitor::visitIdentifier(Identifier &o)
{
    GuideVisitor::visitIdentifier(o);
    checkFound(_getDeclaration(&o), o);
    return 0;
}

int UpdateDeclarationsVisitor::visitParameterAssign(ParameterAssign &o)
{
    GuideVisitor::visitParameterAssign(o);
    checkFound(_getDeclaration(&o), o);
    return 0;
}

int UpdateDeclarationsVisitor::visitPortAssign(PortAssign &o)
{
    GuideVisitor::visitPortAssign(o);
    checkFound(_getDeclaration(&o), o);
    return 0;
}

int UpdateDeclarationsVisitor::visitFieldReference(FieldReference &o)
{
    GuideVisitor::visitFieldReference(o);

    // In case of constant value (record value) the declaration does
    // not exists.
    // It is like asking for the declaration of a integer constant.
    if (dynamic_cast<RecordValue *>(o.getPrefix()) == nullptr) {
        checkFound(_getDeclaration(&o), o);
    }
    return 0;
}

int UpdateDeclarationsVisitor::visitInstance(Instance &o)
{
    Instance::DeclarationType *s = o.GetDeclaration();
    if (_opt.onlyVisible)
        hif::semantics::setDeclaration(&o, nullptr);
    GuideVisitor::visitInstance(o);
    if (_opt.onlyVisible && o.GetDeclaration() == nullptr) {
        hif::semantics::setDeclaration(&o, s);
    }
    const bool mustBe = dynamic_cast<Library *>(o.getReferencedType()) == nullptr;
    checkFound(_getDeclaration(&o), o, mustBe);
    return 0;
}

int UpdateDeclarationsVisitor::visitLibrary(Library &o)
{
    GuideVisitor::visitLibrary(o);
    Library::DeclarationType *decl = _getDeclaration(&o);

    // A system library could represent a standard header without declaration.
    if (decl == nullptr && o.isSystem())
        return 0;

    checkFound(decl, o);
    return 0;
}

int UpdateDeclarationsVisitor::visitFunctionCall(FunctionCall &o)
{
    FunctionCall::DeclarationType *s = o.GetDeclaration();
    if (_opt.onlyVisible)
        hif::semantics::setDeclaration(&o, nullptr);
    GuideVisitor::visitFunctionCall(o);
    if (_opt.onlyVisible && o.GetDeclaration() == nullptr) {
        hif::semantics::setDeclaration(&o, s);
    }
    checkFound(_getDeclaration(&o), o);
    return 0;
}

int UpdateDeclarationsVisitor::visitProcedureCall(ProcedureCall &o)
{
    ProcedureCall::DeclarationType *s = o.GetDeclaration();
    if (_opt.onlyVisible)
        hif::semantics::setDeclaration(&o, nullptr);
    GuideVisitor::visitProcedureCall(o);
    if (_opt.onlyVisible && o.GetDeclaration() == nullptr) {
        hif::semantics::setDeclaration(&o, s);
    }
    checkFound(_getDeclaration(&o), o);
    return 0;
}

int UpdateDeclarationsVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    GuideVisitor::visitTypeTPAssign(o);
    checkFound(_getDeclaration(&o), o);
    return 0;
}

int UpdateDeclarationsVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    GuideVisitor::visitValueTPAssign(o);
    checkFound(_getDeclaration(&o), o);
    return 0;
}

int UpdateDeclarationsVisitor::visitTypeReference(TypeReference &o)
{
    TypeReference::DeclarationType *s = o.GetDeclaration();
    if (_opt.onlyVisible)
        hif::semantics::setDeclaration(&o, nullptr);
    GuideVisitor::visitTypeReference(o);
    if (_opt.onlyVisible && o.GetDeclaration() == nullptr) {
        hif::semantics::setDeclaration(&o, s);
    }
    checkFound(_getDeclaration(&o), o);
    return 0;
}

int UpdateDeclarationsVisitor::visitViewReference(ViewReference &o)
{
    ViewReference::DeclarationType *s = o.GetDeclaration();
    if (_opt.onlyVisible)
        hif::semantics::setDeclaration(&o, nullptr);
    GuideVisitor::visitViewReference(o);
    if (_opt.onlyVisible && o.GetDeclaration() == nullptr) {
        hif::semantics::setDeclaration(&o, s);
    }
    checkFound(_getDeclaration(&o), o);
    return 0;
}

} // namespace

// ///////////////////////////////////////////////////////////////////
// UpdateDeclarationOptions
// ///////////////////////////////////////////////////////////////////

UpdateDeclarationOptions::UpdateDeclarationOptions()
    : DeclarationOptions()
    , onlyVisible(false)
    , root(nullptr)
{
    // ntd
}

UpdateDeclarationOptions::~UpdateDeclarationOptions()
{
    // ntd
}

UpdateDeclarationOptions::UpdateDeclarationOptions(const UpdateDeclarationOptions &other)
    : DeclarationOptions(other)
    , onlyVisible(other.onlyVisible)
    , root(other.root)
{
    // ntd
}

UpdateDeclarationOptions &UpdateDeclarationOptions::operator=(DeclarationOptions other)
{
    swap(other);
    return *this;
}

UpdateDeclarationOptions &UpdateDeclarationOptions::operator=(UpdateDeclarationOptions other)
{
    swap(other);
    return *this;
}

void UpdateDeclarationOptions::swap(DeclarationOptions &other) { DeclarationOptions::swap(other); }

void UpdateDeclarationOptions::swap(UpdateDeclarationOptions &other)
{
    DeclarationOptions::swap(other);
    std::swap(onlyVisible, other.onlyVisible);
    std::swap(root, other.root);
}

// ///////////////////////////////////////////////////////////////////
// updateDeclarations
// ///////////////////////////////////////////////////////////////////

void updateDeclarations(Object *root, ILanguageSemantics *sem, const UpdateDeclarationOptions &opt)
{
    if (root == nullptr)
        return;
    hif::application_utils::initializeLogHeader("HIF", "updateDeclarations");

    UpdateDeclarationsVisitor udv(sem, opt);
    if (opt.forceRefresh)
        resetDeclarations(root);
    root->acceptVisitor(udv);

    hif::application_utils::restoreLogHeader();
}

} // namespace semantics
} // namespace hif
