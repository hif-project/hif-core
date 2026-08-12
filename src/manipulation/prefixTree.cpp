/// @file prefixTree.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/prefixTree.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

// ///////////////////////////////////////////////////////////////////
// Utility methods
// ///////////////////////////////////////////////////////////////////
Object *_getParent(Object *o, unsigned int times, bool checkError)
{
    Object *parent = o;
    unsigned int t = 0;
    for (; t < times; ++t) {
        if (parent == nullptr)
            break;
        parent = parent->getParent();
    }

    if (parent == nullptr && t != times && checkError) {
        messageError("cannot find required parent", o, nullptr);
    }

    return parent;
}

// ///////////////////////////////////////////////////////////////////
// PrefixVisitor
// ///////////////////////////////////////////////////////////////////
class PrefixVisitor : public GuideVisitor
{
public:
    PrefixVisitor(hif::semantics::ILanguageSemantics *sem, const PrefixTreeOptions &opt);
    virtual ~PrefixVisitor();

    int visitFunctionCall(FunctionCall &o);
    int visitFieldReference(FieldReference &o);
    int visitIdentifier(Identifier &o);
    int visitLibrary(Library &o);
    int visitProcedureCall(ProcedureCall &o);
    int visitTypeReference(TypeReference &o);
    int visitViewReference(ViewReference &o);

private:
    PrefixVisitor(const PrefixVisitor &);
    PrefixVisitor operator=(const PrefixVisitor &);

    hif::semantics::ILanguageSemantics *_sem;
    const PrefixTreeOptions &_opt;

    /// @brief Check if the symbol is not contained in the same scope of its
    /// definition, and add a fully-qualifying prefix to it.
    void _prefixSymbol(Object *symbol);

    bool _setPrefix(Object *symbol, ReferencedType *prefix);

    ViewReference *_getParentInstantiatedView(Object *decl);
};

PrefixVisitor::PrefixVisitor(hif::semantics::ILanguageSemantics *sem, const PrefixTreeOptions &opt)
    : GuideVisitor()
    , _sem(sem)
    , _opt(opt)
{
    // ntd
}

PrefixVisitor::~PrefixVisitor()
{
    // ntd
}

void PrefixVisitor::_prefixSymbol(Object *symbol)
{
    Declaration *decl = hif::semantics::getDeclaration(symbol, _sem);

    if (decl == nullptr && !_opt.strictChecks)
        return;
    messageAssert((decl != nullptr), "Expected source declaration not found", symbol, _sem);

    Const *cst = dynamic_cast<Const *>(decl);
    if (cst != nullptr && cst->isDefine())
        return;

    // Ref. design: vhdl/openCores/multiplier3x3 + at2
    ValueTP *vtp = dynamic_cast<ValueTP *>(decl);
    TypeTP *ttp  = dynamic_cast<TypeTP *>(decl);
    if (vtp != nullptr || ttp != nullptr)
        return;

    ViewReference *prefixVr = _getParentInstantiatedView(decl);
    if (prefixVr != nullptr) {
        if (_setPrefix(symbol, prefixVr)) {
            _prefixSymbol(prefixVr);
        }
        return;
    }

    EnumValue *en         = dynamic_cast<EnumValue *>(decl);
    LibraryDef *declScope = nullptr;
    View *vv              = dynamic_cast<View *>(decl);
    if (en != nullptr) {
        declScope = dynamic_cast<LibraryDef *>(_getParent(decl, 3, _opt.strictChecks));
    } else if (vv != nullptr) {
        declScope = dynamic_cast<LibraryDef *>(_getParent(decl, 2, _opt.strictChecks));
    } else {
        // skip in case of not LibraryDef or library inside LibraryDef.
        declScope = dynamic_cast<LibraryDef *>(_getParent(decl, 1, _opt.strictChecks));
    }

    if (declScope == nullptr)
        return;

    LibraryDef *symScope = hif::getNearestParent<LibraryDef>(symbol);
    while (symScope != nullptr && symScope != declScope) {
        symScope = hif::getNearestParent<LibraryDef>(symScope);
    }

    // Special mamagement for pcall and fcall: always prefix calls,
    // when inside a view and calling parent library methods.
    bool isSpecialCase = false;
    if ((dynamic_cast<FunctionCall *>(symbol) != nullptr || dynamic_cast<ProcedureCall *>(symbol) != nullptr) &&
        getNearestParent<View>(symbol) != nullptr && declScope != nullptr) {
        isSpecialCase = true;
    }

    bool sameScope = (symScope == declScope && _opt.skipPrefixingIfSameScope);
    if (sameScope && !isSpecialCase)
        return;

    // if cannot find a common scope, prefix the symbol and visit the prefix recursively
    Library *prefix = new Library();
    prefix->setName(declScope->getName());
    prefix->setSystem(declScope->isStandard());
    hif::semantics::setDeclaration(prefix, declScope);

    if (_setPrefix(symbol, prefix)) {
        _prefixSymbol(prefix);
    }
}

bool PrefixVisitor::_setPrefix(Object *symbol, ReferencedType *prefix)
{
    if (dynamic_cast<Library *>(symbol)) {
        Library *v = static_cast<Library *>(symbol);
        if (v->getInstance() != nullptr) {
            delete prefix;
            return false; // trust the existing one
        }
        v->setInstance(prefix);
    } else if (dynamic_cast<ViewReference *>(symbol)) {
        ViewReference *v = static_cast<ViewReference *>(symbol);
        if (v->getInstance() != nullptr) {
            delete prefix;
            return false; // trust the existing one
        }
        v->setInstance(prefix);
    } else if (dynamic_cast<TypeReference *>(symbol)) {
        TypeReference *v = static_cast<TypeReference *>(symbol);
        if (v->getInstance() != nullptr) {
            delete prefix;
            return false; // trust the existing one
        }
        v->setInstance(prefix);
    } else if (dynamic_cast<FunctionCall *>(symbol)) {
        FunctionCall *v = static_cast<FunctionCall *>(symbol);
        if (v->getInstance() != nullptr) {
            // ntd. fixed by inner visit of ReferencedType
            delete prefix;
            return false;
        }
        Instance *inst = new Instance();
        inst->setName(prefix->getName());
        inst->setReferencedType(prefix);
        v->setInstance(inst);
    } else if (dynamic_cast<ProcedureCall *>(symbol)) {
        ProcedureCall *v = static_cast<ProcedureCall *>(symbol);
        if (v->getInstance() != nullptr) {
            // ntd. fixed by inner visit of ReferencedType
            delete prefix;
            return false;
        }
        Instance *inst = new Instance();
        inst->setName(prefix->getName());
        inst->setReferencedType(prefix);
        v->setInstance(inst);
    } else if (dynamic_cast<FieldReference *>(symbol)) {
        // ntd. fixed by inner visit of its prefix
        delete prefix;
        return false;
    } else if (dynamic_cast<Identifier *>(symbol)) {
        Identifier *v = static_cast<Identifier *>(symbol);

        Instance *inst = new Instance();
        inst->setName(prefix->getName());
        inst->setReferencedType(prefix);

        FieldReference *fr = new FieldReference();
        fr->setName(v->getName());
        fr->setPrefix(inst);
        v->replace(fr);

        delete v;
    } else {
        messageError("Unexpected symbol", symbol, _sem);
    }

    return true;
}

ViewReference *PrefixVisitor::_getParentInstantiatedView(Object *decl)
{
    SubProgram *sub = hif::getNearestParent<SubProgram>(decl);
    if (sub != nullptr)
        return nullptr;

    View *view = hif::getNearestParent<View>(decl);
    if (view == nullptr || (!isInCache(view) && !_opt.setContainingView))
        return nullptr;
    DesignUnit *du = dynamic_cast<DesignUnit *>(view->getParent());
    if (du == nullptr)
        return nullptr;

    ViewReference *vr = new ViewReference();
    vr->setName(view->getName());
    vr->setDesignUnit(du->getName());
    hif::semantics::setDeclaration(vr, view);
    for (BList<Declaration>::iterator i = view->templateParameters.begin(); i != view->templateParameters.end(); ++i) {
        Declaration *tp = *i;
        if (dynamic_cast<ValueTP *>(tp) != nullptr) {
            ValueTP *vtp = static_cast<ValueTP *>(tp);

            ValueTPAssign *vtpa = new ValueTPAssign();
            vtpa->setName(vtp->getName());
            vtpa->setValue(hif::copy(vtp->getValue()));

            vr->templateParameterAssigns.push_back(vtpa);
        } else // TypeTP
        {
            TypeTP *ttp = static_cast<TypeTP *>(tp);

            TypeTPAssign *ttpa = new TypeTPAssign();
            ttpa->setName(ttp->getName());
            ttpa->setType(hif::copy(ttp->getType()));
            hif::semantics::setDeclaration(ttpa, ttp);

            vr->templateParameterAssigns.push_back(ttpa);
        }
    }

    return vr;
}

int PrefixVisitor::visitFunctionCall(FunctionCall &o)
{
    if (_opt.recursive)
        GuideVisitor::visitFunctionCall(o);
    _prefixSymbol(&o);
    return 0;
}

int PrefixVisitor::visitFieldReference(FieldReference &o)
{
    if (_opt.recursive)
        GuideVisitor::visitFieldReference(o);
    _prefixSymbol(&o);
    return 0;
}

int PrefixVisitor::visitIdentifier(Identifier &o)
{
    if (_opt.recursive)
        GuideVisitor::visitIdentifier(o);
    _prefixSymbol(&o);
    return 0;
}

int PrefixVisitor::visitLibrary(Library &o)
{
    if (_opt.recursive)
        GuideVisitor::visitLibrary(o);
    _prefixSymbol(&o);
    return 0;
}

int PrefixVisitor::visitProcedureCall(ProcedureCall &o)
{
    if (_opt.recursive)
        GuideVisitor::visitProcedureCall(o);
    _prefixSymbol(&o);
    return 0;
}

int PrefixVisitor::visitTypeReference(TypeReference &o)
{
    if (_opt.recursive)
        GuideVisitor::visitTypeReference(o);
    _prefixSymbol(&o);
    return 0;
}

int PrefixVisitor::visitViewReference(ViewReference &o)
{
    if (_opt.recursive)
        GuideVisitor::visitViewReference(o);
    _prefixSymbol(&o);
    return 0;
}

} // namespace

// ///////////////////////////////////////////////////////////////////
// PrefixTreeOptions
// ///////////////////////////////////////////////////////////////////
PrefixTreeOptions::PrefixTreeOptions()
    : recursive(true)
    , strictChecks(true)
    , skipPrefixingIfSameScope(true)
    , setContainingView(false)
{
    // ntd
}

PrefixTreeOptions::~PrefixTreeOptions()
{
    // ntd
}

PrefixTreeOptions::PrefixTreeOptions(const PrefixTreeOptions &o)
    : recursive(o.recursive)
    , strictChecks(o.strictChecks)
    , skipPrefixingIfSameScope(o.skipPrefixingIfSameScope)
    , setContainingView(o.setContainingView)
{
    // ntd
}

PrefixTreeOptions &PrefixTreeOptions::operator=(PrefixTreeOptions o)
{
    swap(o);
    return *this;
}

void PrefixTreeOptions::swap(PrefixTreeOptions &o)
{
    std::swap(recursive, o.recursive);
    std::swap(strictChecks, o.strictChecks);
    std::swap(skipPrefixingIfSameScope, o.skipPrefixingIfSameScope);
    std::swap(setContainingView, o.setContainingView);
}

// ///////////////////////////////////////////////////////////////////
// main method
// ///////////////////////////////////////////////////////////////////
void prefixTree(Object *root, hif::semantics::ILanguageSemantics *sem, const PrefixTreeOptions &opt)
{
    if (root == nullptr)
        return;
    PrefixVisitor v(sem, opt);
    root->acceptVisitor(v);
}

} // namespace manipulation
} // namespace hif
