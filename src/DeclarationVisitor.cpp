/// @file DeclarationVisitor.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/DeclarationVisitor.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/semantics/HIFSemantics.hpp"
#include "hif/semantics/declarationUtils.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

// ///////////////////////////////////////////////////////////////////
// DeclarationVisitorOptions
// ///////////////////////////////////////////////////////////////////

DeclarationVisitorOptions::DeclarationVisitorOptions()
    : visitDeclarationsOnce(false)
    , visitSymbolsOnce(false)
    , visitReferencesAfterDeclaration(false)
    , refsMap(nullptr)
    , sem(hif::semantics::HIFSemantics::getInstance())
{
    // ntd
}

DeclarationVisitorOptions::~DeclarationVisitorOptions()
{
    // ntd
}

DeclarationVisitorOptions::DeclarationVisitorOptions(const DeclarationVisitorOptions &o)
    : visitDeclarationsOnce(o.visitDeclarationsOnce)
    , visitSymbolsOnce(o.visitSymbolsOnce)
    , visitReferencesAfterDeclaration(o.visitReferencesAfterDeclaration)
    , refsMap(o.refsMap)
    , sem(o.sem)
{
    // ntd
}

DeclarationVisitorOptions &DeclarationVisitorOptions::operator=(DeclarationVisitorOptions o)
{
    swap(o);
    return *this;
}

void DeclarationVisitorOptions::swap(DeclarationVisitorOptions &o)
{
    std::swap(visitDeclarationsOnce, o.visitDeclarationsOnce);
    std::swap(visitSymbolsOnce, o.visitSymbolsOnce);
    std::swap(visitReferencesAfterDeclaration, o.visitReferencesAfterDeclaration);
    std::swap(refsMap, o.refsMap);
    std::swap(sem, o.sem);
}

// ///////////////////////////////////////////////////////////////////
// DeclarationVisitor
// ///////////////////////////////////////////////////////////////////

DeclarationVisitor::DeclarationVisitor(const DeclarationVisitorOptions &opt, System *root)
    : GuideVisitor()
    , _opt(opt)
    , _visitedDeclarations()
    , _visitedSymbols()
{
    if (_opt.visitReferencesAfterDeclaration && _opt.refsMap == nullptr) {
        messageAssert(root != nullptr, "Required root object to calculate references", nullptr, _opt.sem);
        _opt.refsMap = new hif::semantics::ReferencesMap();
        hif::semantics::getAllReferences(*_opt.refsMap, _opt.sem, root);
    }
}

DeclarationVisitor::~DeclarationVisitor()
{
    // ntd
}

// //////////////////////////
// Common virtual methods
// //////////////////////////

int DeclarationVisitor::visitDeclaration(Declaration & /*o*/) { return 0; }

int DeclarationVisitor::visitDataDeclaration(DataDeclaration &o) { return visitDeclaration(o); }

int DeclarationVisitor::visitSymbol(features::ISymbol &o)
{
    Object *symbolObject = o.toObject();
    Declaration *d       = _getDeclaration(symbolObject);
    int rv               = 0;
    if (d != nullptr) {
        rv = d->acceptVisitor(*this);
    }

    return rv;
}

int DeclarationVisitor::visitReferences(Declaration &o)
{
    int rv = 0;
    if (_opt.visitReferencesAfterDeclaration) {
        if (_opt.refsMap->find(&o) == _opt.refsMap->end())
            return rv;

        for (hif::semantics::ReferencesSet::iterator i = (*_opt.refsMap)[&o].begin(); i != (*_opt.refsMap)[&o].end();
             ++i) {
            Object *ref = *i;
            rv |= ref->acceptVisitor(*this);
        }
    }

    return rv;
}

// //////////////////////////
// Symbols visits
// //////////////////////////

int DeclarationVisitor::visitFieldReference(FieldReference &o)
{
    int rv = 0;
    if (!_checkSymbol(o))
        return rv;
    rv |= GuideVisitor::visitFieldReference(o);
    rv |= visitSymbol(o);
    return rv;
}

int DeclarationVisitor::visitFunctionCall(FunctionCall &o)
{
    int rv = 0;
    if (!_checkSymbol(o))
        return rv;
    rv |= GuideVisitor::visitFunctionCall(o);
    rv |= visitSymbol(o);
    return rv;
}

int DeclarationVisitor::visitIdentifier(Identifier &o)
{
    int rv = 0;
    if (!_checkSymbol(o))
        return rv;
    rv |= GuideVisitor::visitIdentifier(o);
    rv |= visitSymbol(o);
    return rv;
}

int DeclarationVisitor::visitInstance(Instance &o)
{
    int rv = 0;
    if (!_checkSymbol(o))
        return rv;
    rv |= GuideVisitor::visitInstance(o);
    rv |= visitSymbol(o);
    return rv;
}

int DeclarationVisitor::visitLibrary(Library &o)
{
    int rv = 0;
    if (!_checkSymbol(o))
        return rv;
    rv |= GuideVisitor::visitLibrary(o);
    rv |= visitSymbol(o);
    return rv;
}

int DeclarationVisitor::visitParameterAssign(ParameterAssign &o)
{
    int rv = 0;
    if (!_checkSymbol(o))
        return rv;
    rv |= GuideVisitor::visitParameterAssign(o);
    rv |= visitSymbol(o);
    return rv;
}

int DeclarationVisitor::visitPortAssign(PortAssign &o)
{
    int rv = 0;
    if (!_checkSymbol(o))
        return rv;
    rv |= GuideVisitor::visitPortAssign(o);
    rv |= visitSymbol(o);
    return rv;
}

int DeclarationVisitor::visitProcedureCall(ProcedureCall &o)
{
    int rv = 0;
    if (!_checkSymbol(o))
        return rv;
    rv |= GuideVisitor::visitProcedureCall(o);
    rv |= visitSymbol(o);
    return rv;
}

int DeclarationVisitor::visitTypeReference(TypeReference &o)
{
    int rv = 0;
    if (!_checkSymbol(o))
        return rv;
    rv |= GuideVisitor::visitTypeReference(o);
    rv |= visitSymbol(o);
    return rv;
}

int DeclarationVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    int rv = 0;
    if (!_checkSymbol(o))
        return rv;
    rv |= GuideVisitor::visitTypeTPAssign(o);
    rv |= visitSymbol(o);
    return rv;
}

int DeclarationVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    int rv = 0;
    if (!_checkSymbol(o))
        return rv;
    rv |= GuideVisitor::visitValueTPAssign(o);
    rv |= visitSymbol(o);
    return rv;
}

int DeclarationVisitor::visitViewReference(ViewReference &o)
{
    int rv = 0;
    if (!_checkSymbol(o))
        return rv;
    rv |= GuideVisitor::visitViewReference(o);
    rv |= visitSymbol(o);
    return rv;
}
// //////////////////////////
// Declarations visits
// //////////////////////////
int DeclarationVisitor::visitContents(Contents &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitContents(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitDesignUnit(DesignUnit &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitDesignUnit(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitFunction(Function &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitFunction(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitEntity(Entity &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitEntity(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitForGenerate(ForGenerate &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitForGenerate(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitIfGenerate(IfGenerate &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitIfGenerate(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitLibraryDef(LibraryDef &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitLibraryDef(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitProcedure(Procedure &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitProcedure(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitState(State &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitState(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitStateTable(StateTable &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitStateTable(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitTypeDef(TypeDef &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitTypeDef(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitTypeTP(TypeTP &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitTypeTP(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitView(View &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitView(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitSystem(System &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitSystem(o);
    rv |= visitDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}
// //////////////////////////
// DataDeclarations visits
// //////////////////////////

int DeclarationVisitor::visitAlias(Alias &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitAlias(o);
    rv |= visitDataDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitConst(Const &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitConst(o);
    rv |= visitDataDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitEnumValue(EnumValue &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitEnumValue(o);
    rv |= visitDataDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitField(Field &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitField(o);
    rv |= visitDataDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitParameter(Parameter &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitParameter(o);
    rv |= visitDataDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitPort(Port &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitPort(o);
    rv |= visitDataDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitSignal(Signal &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitSignal(o);
    rv |= visitDataDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitValueTP(ValueTP &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitValueTP(o);
    rv |= visitDataDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

int DeclarationVisitor::visitVariable(Variable &o)
{
    if (!_checkDeclaration(o))
        return 0;
    int rv = GuideVisitor::visitVariable(o);
    rv |= visitDataDeclaration(o);
    rv |= visitReferences(o);
    return rv;
}

Declaration *DeclarationVisitor::_getDeclaration(Object *o) { return hif::semantics::getDeclaration(o, _opt.sem); }

bool DeclarationVisitor::_checkSymbol(features::ISymbol &o)
{
    if (_opt.visitSymbolsOnce)
        return true;

    Object *symbolObject = o.toObject();
    if (_visitedSymbols.find(symbolObject) != _visitedSymbols.end()) {
        return false;
    }

    _visitedSymbols.insert(symbolObject);
    return true;
}

bool DeclarationVisitor::_checkDeclaration(Declaration &o)
{
    if (!_opt.visitDeclarationsOnce)
        return true;

    if (_visitedDeclarations.find(&o) != _visitedDeclarations.end()) {
        return false;
    }

    _visitedDeclarations.insert(&o);
    return true;
}

} // namespace hif
