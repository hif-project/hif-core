/// @file resetDeclarations.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/resetDeclarations.hpp"

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
/*	Reset declaration visitor												*/
/****************************************************************************/
class ResetDeclarationVisitor : public GuideVisitor
{
public:
    ResetDeclarationVisitor(const ResetDeclarationsOptions &opt);

    /// @brief Destructor
    virtual ~ResetDeclarationVisitor();

    virtual int visitIdentifier(Identifier &o);
    virtual int visitParameterAssign(ParameterAssign &o);
    virtual int visitPortAssign(PortAssign &o);

    virtual int visitFieldReference(FieldReference &o);
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitInstance(Instance &o);
    virtual int visitLibrary(Library &o);
    virtual int visitProcedureCall(ProcedureCall &o);
    virtual int visitTypeReference(TypeReference &o);
    virtual int visitTypeTPAssign(TypeTPAssign &o);
    virtual int visitValueTPAssign(ValueTPAssign &o);
    virtual int visitViewReference(ViewReference &o);

private:
    const ResetDeclarationsOptions &_opt;

    template <typename T> void _resetDeclaration(T *obj);

    ResetDeclarationVisitor(const ResetDeclarationVisitor &);
    ResetDeclarationVisitor &operator=(const ResetDeclarationVisitor &);
};
// Constructor
ResetDeclarationVisitor::ResetDeclarationVisitor(const ResetDeclarationsOptions &opt)
    : GuideVisitor(0)
    , _opt(opt)
{
    // ntd
}

// Destructor
ResetDeclarationVisitor::~ResetDeclarationVisitor()
{
    // ntd
}

template <typename T> void ResetDeclarationVisitor::_resetDeclaration(T *obj)
{
    typename T::DeclarationType *oldDecl = nullptr;
    if (_opt.onlyVisible) {
        DeclarationOptions opt;
        opt.dontSearch = true;
        oldDecl        = getDeclaration(obj, _opt.sem);
    }

    setDeclaration(obj, nullptr);

    if (_opt.onlyVisible) {
        // search declaration in symbol scope
        typename T::DeclarationType *newDecl = getDeclaration(obj, _opt.sem);
        if (newDecl != nullptr) {
            // can be found => reset since visible
            setDeclaration(obj, nullptr);
        } else {
            // cannot be found => restore previous (may was nullptr)
            setDeclaration(obj, oldDecl);
        }
    }
}

int ResetDeclarationVisitor::visitIdentifier(Identifier &o)
{
    if (!_opt.onlyCurrent)
        GuideVisitor::visitIdentifier(o);
    _resetDeclaration(&o);
    return 0;
}

int ResetDeclarationVisitor::visitParameterAssign(ParameterAssign &o)
{
    Value *valueRestore = nullptr;
    if (_opt.onlySignatures) {
        valueRestore = o.setValue(nullptr);
    }

    if (!_opt.onlyCurrent)
        GuideVisitor::visitParameterAssign(o);
    _resetDeclaration(&o);

    if (_opt.onlySignatures) {
        o.setValue(valueRestore);
    }
    return 0;
}

int ResetDeclarationVisitor::visitPortAssign(PortAssign &o)
{
    Value *valueRestore = nullptr;
    if (_opt.onlySignatures) {
        valueRestore = o.setValue(nullptr);
    }

    if (!_opt.onlyCurrent)
        GuideVisitor::visitPortAssign(o);
    _resetDeclaration(&o);

    if (_opt.onlySignatures) {
        o.setValue(valueRestore);
    }
    return 0;
}

int ResetDeclarationVisitor::visitLibrary(Library &o)
{
    ReferencedType *refRestore = nullptr;
    if (_opt.onlySignatures) {
        refRestore = o.setInstance(nullptr);
    }

    if (!_opt.onlyCurrent)
        GuideVisitor::visitLibrary(o);
    _resetDeclaration(&o);

    if (_opt.onlySignatures) {
        o.setInstance(refRestore);
    }
    return 0;
}

int ResetDeclarationVisitor::visitFieldReference(FieldReference &o)
{
    if (!_opt.onlyCurrent)
        GuideVisitor::visitFieldReference(o);
    // In case of constant value (record value) the declaration does
    // not exists.
    // It is like asking for the declaration of a integer constant.
    if (dynamic_cast<RecordValue *>(o.getPrefix()) == nullptr) {
        _resetDeclaration(&o);
    }
    return 0;
}

int ResetDeclarationVisitor::visitFunctionCall(FunctionCall &o)
{
    //    Value * instRestore = nullptr;
    //    if (_opt.onlySignatures)
    //    {
    //        instRestore = o.setInstance(nullptr);
    //    }

    if (!_opt.onlyCurrent)
        GuideVisitor::visitFunctionCall(o);
    _resetDeclaration(&o);

    //    if (_opt.onlySignatures)
    //    {
    //        o.setInstance(instRestore);
    //    }

    return 0;
}

int ResetDeclarationVisitor::visitInstance(Instance &o)
{
    if (!_opt.onlyCurrent)
        GuideVisitor::visitInstance(o);
    _resetDeclaration(&o);
    return 0;
}

int ResetDeclarationVisitor::visitProcedureCall(ProcedureCall &o)
{
    //    Value * instRestore = nullptr;
    //    if (_opt.onlySignatures)
    //    {
    //        instRestore = o.setInstance(nullptr);
    //    }

    if (!_opt.onlyCurrent)
        GuideVisitor::visitProcedureCall(o);
    _resetDeclaration(&o);

    //    if (_opt.onlySignatures)
    //    {
    //        o.setInstance(instRestore);
    //    }
    return 0;
}

int ResetDeclarationVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    Type *valueRestore = nullptr;
    if (_opt.onlySignatures) {
        valueRestore = o.setType(nullptr);
    }

    if (!_opt.onlyCurrent)
        GuideVisitor::visitTypeTPAssign(o);
    _resetDeclaration(&o);

    if (_opt.onlySignatures) {
        o.setType(valueRestore);
    }
    return 0;
}

int ResetDeclarationVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    Value *valueRestore = nullptr;
    if (_opt.onlySignatures) {
        valueRestore = o.setValue(nullptr);
    }

    if (!_opt.onlyCurrent)
        GuideVisitor::visitValueTPAssign(o);
    _resetDeclaration(&o);

    if (_opt.onlySignatures) {
        o.setValue(valueRestore);
    }
    return 0;
}

int ResetDeclarationVisitor::visitTypeReference(TypeReference &o)
{
    if (!_opt.onlyCurrent)
        GuideVisitor::visitTypeReference(o);
    _resetDeclaration(&o);
    return 0;
}

int ResetDeclarationVisitor::visitViewReference(ViewReference &o)
{
    if (!_opt.onlyCurrent)
        GuideVisitor::visitViewReference(o);
    _resetDeclaration(&o);
    return 0;
}

} // namespace

// ///////////////////////////////////////////////////////////////////
// ResetDeclarations Options
// ///////////////////////////////////////////////////////////////////

ResetDeclarationsOptions::ResetDeclarationsOptions()
    : onlyCurrent(false)
    , onlyVisible(false)
    , onlySignatures(false)
    , sem(HIFSemantics::getInstance())
{
    // ntd
}

ResetDeclarationsOptions::~ResetDeclarationsOptions()
{
    // ntd
}

ResetDeclarationsOptions::ResetDeclarationsOptions(const ResetDeclarationsOptions &other)
    : onlyCurrent(other.onlyCurrent)
    , onlyVisible(other.onlyVisible)
    , onlySignatures(other.onlySignatures)
    , sem(other.sem)
{
    // ntd
}
ResetDeclarationsOptions &ResetDeclarationsOptions::operator=(ResetDeclarationsOptions other)
{
    swap(other);
    return *this;
}

void ResetDeclarationsOptions::swap(ResetDeclarationsOptions &other)
{
    std::swap(onlyCurrent, other.onlyCurrent);
    std::swap(onlyVisible, other.onlyVisible);
    std::swap(onlySignatures, other.onlySignatures);
    std::swap(sem, other.sem);
}

// ///////////////////////////////////////////////////////////////////
// resetDeclarations
// ///////////////////////////////////////////////////////////////////

void resetDeclarations(Object *o, const ResetDeclarationsOptions &opt)
{
    hif::application_utils::initializeLogHeader("HIF", "resetDeclarations");

    // checks
    messageAssert(o != nullptr, "Passed nullptr object", nullptr, nullptr);
#ifndef NDEBUG
    hif::features::ISymbol *s = dynamic_cast<hif::features::ISymbol *>(o);
    messageDebugAssert(s != nullptr || !opt.onlyCurrent, "Passed non symbol and onlyCurrent option", o, opt.sem);
#endif

    ResetDeclarationVisitor rdv(opt);
    o->acceptVisitor(rdv);

    hif::application_utils::restoreLogHeader();
}
void resetDeclarations(BList<Object> &o, const ResetDeclarationsOptions &opt)
{
    for (BList<Object>::iterator i = o.begin(); i != o.end(); ++i) {
        resetDeclarations(*i, opt);
    }
}

template <typename T> void resetDeclarations(BList<T> &o, const ResetDeclarationsOptions &opt)
{
    resetDeclarations(o.template toOtherBList<Object>(), opt);
}

/// @brief Macro for template method instantiation.
#define HIF_TEMPLATE_METHOD(T) void resetDeclarations<T>(BList<T> &, const ResetDeclarationsOptions &)
HIF_INSTANTIATE_METHOD()

#undef HIF_TEMPLATE_METHOD
} // namespace semantics
} // namespace hif
