/// @file resolveHierarchicalSymbol.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/resolveHierarchicalSymbol.hpp"

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

std::string _unwrapSymbol(const std::string &symbol, DefinitionStyle::Type &style)
{
    if (symbol.substr(0, 5) == "work.") {
        style = DefinitionStyle::VHDL;
        std::string useStmt(symbol.substr(5));
        if (useStmt.substr(useStmt.size() - 4) == ".all") {
            useStmt = useStmt.substr(0, useStmt.size() - 4);
        }
        return symbol.substr(5);
    }

    style = DefinitionStyle::HIF;
    return symbol;
}
class ResolveHierarchyVisitor : public hif::GuideVisitor
{
public:
    /// @name Traits
    /// @{
    typedef std::vector<std::string> TokenList;
    /// @}

    ResolveHierarchyVisitor(
        TokenList &tokenList,
        hif::semantics::ILanguageSemantics *sem,
        std::list<hif::Instance *> *inst,
        const std::string &symbol);
    virtual ~ResolveHierarchyVisitor();

    Object *getResolvedSymbol();

private:
    virtual int visitAlias(Alias &o);
    virtual int visitConst(Const &o);
    virtual int visitDesignUnit(DesignUnit &o);
    virtual int visitEnumValue(EnumValue &o);
    virtual int visitField(Field &o);
    virtual int visitFunction(Function &o);
    virtual int visitInstance(Instance &o);
    virtual int visitLibraryDef(LibraryDef &o);
    virtual int visitParameter(Parameter &o);
    virtual int visitPort(Port &o);
    virtual int visitProcedure(Procedure &o);
    virtual int visitSignal(Signal &o);
    virtual int visitStateTable(StateTable &o);
    virtual int visitTypeDef(TypeDef &o);
    virtual int visitTypeTP(TypeTP &o);
    virtual int visitVariable(Variable &o);
    virtual int visitValueTP(ValueTP &o);

    TokenList &_tokenList;
    hif::semantics::ILanguageSemantics *_sem;
    Object *_resolvedObj;
    unsigned int _currentToken;
    std::list<hif::Instance *> *_instancePath;
    const std::string _symbol;

    void _checkLeaf(Object *o);
    void _setResolvedObj(Object *o);

    ResolveHierarchyVisitor(const ResolveHierarchyVisitor &);
    ResolveHierarchyVisitor operator=(const ResolveHierarchyVisitor &);
};

ResolveHierarchyVisitor::ResolveHierarchyVisitor(
    TokenList &tokenList,
    hif::semantics::ILanguageSemantics *sem,
    std::list<Instance *> *inst,
    const std::string &symbol)
    : hif::GuideVisitor()
    , _tokenList(tokenList)
    , _sem(sem)
    , _resolvedObj(nullptr)
    , _currentToken(0)
    , _instancePath(inst)
    , _symbol(symbol)
{
    // ntd
}

ResolveHierarchyVisitor::~ResolveHierarchyVisitor()
{
    // ntd
}

Object *ResolveHierarchyVisitor::getResolvedSymbol() { return _resolvedObj; }

void ResolveHierarchyVisitor::_checkLeaf(Object *o)
{
    if (std::string(hif::objectGetName(o)) == _tokenList[_currentToken]) {
        messageAssert(_currentToken == _tokenList.size() - 1, "Expected as last element", o, _sem);
        _setResolvedObj(o);
    }
}

void ResolveHierarchyVisitor::_setResolvedObj(Object *o)
{
    messageAssert(_resolvedObj == nullptr, "Found more than one match for symbol: " + _symbol, o, _sem);
    _resolvedObj = o;
}

int ResolveHierarchyVisitor::visitAlias(Alias &o)
{
    _checkLeaf(&o);
    return 0;
}

int ResolveHierarchyVisitor::visitConst(Const &o)
{
    _checkLeaf(&o);
    return 0;
}
int ResolveHierarchyVisitor::visitDesignUnit(DesignUnit &o)
{
    std::string sym(_tokenList[_currentToken]);
    std::string duName;
    std::string viewName;
    {
        const std::string::size_type open   = sym.find('(');
        const std::string::size_type closed = sym.find(')');

        duName = sym.substr(0, open);
        if (open == std::string::npos)
            viewName = "";
        else
            viewName = sym.substr(open + 1, closed - open - 1);
    }

    if (std::string(o.getName()) != duName)
        return 0;

    View *view = nullptr;
    if (viewName == "") {
        messageAssert(o.views.size() == 1, "More than one View found, but no View name has been specidfied.", &o, _sem);
        view = o.views.front();
    } else {
        for (BList<View>::iterator i = o.views.begin(); i != o.views.end(); ++i) {
            if ((*i)->getName() != viewName)
                continue;
            view = *i;
            break;
        }
    }

    if (_currentToken == _tokenList.size() - 1) {
        _setResolvedObj(view);
        return 0;
    }

    if (view == nullptr)
        return 0;

    ++_currentToken;
    GuideVisitor::visitView(*view);

    return 0;
}
int ResolveHierarchyVisitor::visitEnumValue(EnumValue &o)
{
    _checkLeaf(&o);
    return 0;
}
int ResolveHierarchyVisitor::visitField(Field &o)
{
    _checkLeaf(&o);
    return 0;
}
int ResolveHierarchyVisitor::visitFunction(Function &o)
{
    if (std::string(o.getName()) != _tokenList[_currentToken])
        return 0;

    if (_currentToken == _tokenList.size() - 1) {
        _setResolvedObj(&o);
        return 0;
    }

    ++_currentToken;
    GuideVisitor::visitFunction(o);

    return 0;
}
int ResolveHierarchyVisitor::visitInstance(Instance &o)
{
    Contents *c = dynamic_cast<Contents *>(o.getParent());
    if (c == nullptr || !o.isInBList() || o.getBList() != reinterpret_cast<BList<Object> *>(&c->instances))
        return 0;

    if (std::string(o.getName()) != _tokenList[_currentToken])
        return 0;

    if (_instancePath != nullptr)
        _instancePath->push_back(&o);

    if (_currentToken == _tokenList.size() - 1) {
        _setResolvedObj(&o);
        return 0;
    }

    ViewReference *vr = dynamic_cast<ViewReference *>(o.getReferencedType());
    if (vr == nullptr)
        return 0;

    ViewReference::DeclarationType *view = hif::semantics::getDeclaration(vr, _sem);
    messageAssert(view != nullptr, "Declaration not found", vr, _sem);

    ++_currentToken;
    view->acceptVisitor(*this);

    return 0;
}
int ResolveHierarchyVisitor::visitLibraryDef(LibraryDef &o)
{
    if (std::string(o.getName()) != _tokenList[_currentToken])
        return 0;

    if (_currentToken == _tokenList.size() - 1) {
        _setResolvedObj(&o);
        return 0;
    }

    ++_currentToken;
    GuideVisitor::visitLibraryDef(o);

    return 0;
}
int ResolveHierarchyVisitor::visitParameter(Parameter &o)
{
    _checkLeaf(&o);
    return 0;
}
int ResolveHierarchyVisitor::visitPort(Port &o)
{
    _checkLeaf(&o);
    return 0;
}
int ResolveHierarchyVisitor::visitProcedure(Procedure &o)
{
    if (std::string(o.getName()) != _tokenList[_currentToken])
        return 0;

    if (_currentToken == _tokenList.size() - 1) {
        _setResolvedObj(&o);
        return 0;
    }

    ++_currentToken;
    GuideVisitor::visitProcedure(o);

    return 0;
}
int ResolveHierarchyVisitor::visitSignal(Signal &o)
{
    _checkLeaf(&o);
    return 0;
}
int ResolveHierarchyVisitor::visitStateTable(StateTable &o)
{
    BaseContents *bc = dynamic_cast<BaseContents *>(o.getParent());
    if (bc == nullptr || !o.isInBList() || o.getBList() != reinterpret_cast<BList<Object> *>(&bc->stateTables)) {
        GuideVisitor::visitStateTable(o);
        return 0;
    }

    // process
    if (std::string(o.getName()) != _tokenList[_currentToken])
        return 0;

    if (_currentToken == _tokenList.size() - 1) {
        _setResolvedObj(&o);
        return 0;
    }

    ++_currentToken;
    GuideVisitor::visitStateTable(o);

    return 0;
}
int ResolveHierarchyVisitor::visitTypeDef(TypeDef &o)
{
    if (std::string(o.getName()) != _tokenList[_currentToken])
        return 0;

    if (_currentToken == _tokenList.size() - 1) {
        _setResolvedObj(&o);
        return 0;
    }

    // typedef of enum and records
    ++_currentToken;
    GuideVisitor::visitTypeDef(o);

    return 0;
}
int ResolveHierarchyVisitor::visitTypeTP(TypeTP &o)
{
    _checkLeaf(&o);
    return 0;
}
int ResolveHierarchyVisitor::visitVariable(Variable &o)
{
    _checkLeaf(&o);
    return 0;
}
int ResolveHierarchyVisitor::visitValueTP(ValueTP &o)
{
    _checkLeaf(&o);
    return 0;
}
} // namespace

Object *resolveHierarchicalSymbol(
    const std::string &symbol,
    hif::System *system,
    hif::semantics::ILanguageSemantics *sem,
    std::list<hif::Instance *> *instancePath)
{
    ResolveHierarchyVisitor::TokenList tokens;
    DefinitionStyle::Type recognizedStyle;
    std::string sym = _unwrapSymbol(symbol, recognizedStyle);
    while (!sym.empty()) {
        const std::string::size_type dot = sym.find('.');
        tokens.push_back(sym.substr(0, dot));

        if (dot == std::string::npos)
            sym = "";
        else
            sym = sym.substr(dot + 1);
    }

    ResolveHierarchyVisitor v(tokens, sem, instancePath, symbol);
    system->acceptVisitor(v);
    return v.getResolvedSymbol();
}

} // namespace manipulation
} // namespace hif
