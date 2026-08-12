/// @file buildHierarchicalSymbol.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/buildHierarchicalSymbol.hpp"

#include "hif/BiVisitor.hpp"
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
class BuildHierarchyVisitor : public MonoVisitor<BuildHierarchyVisitor>
{
public:
    BuildHierarchyVisitor(
        Object *startingObject,
        hif::semantics::ILanguageSemantics *sem,
        const DefinitionStyle::Type style);
    virtual ~BuildHierarchyVisitor();

    void map(Object *o);

    void map(Alias *o);
    void map(Const *o);
    void map(DesignUnit *o);
    void map(EnumValue *o);
    void map(Field *o);
    void map(Function *o);
    void map(Instance *o);
    void map(LibraryDef *o);
    void map(Parameter *o);
    void map(Port *o);
    void map(Procedure *o);
    void map(Signal *o);
    void map(StateTable *o);
    void map(System *o);
    void map(TypeDef *o);
    void map(TypeTP *o);
    void map(Variable *o);
    void map(View *o);
    void map(ValueTP *o);

    std::string getResult() const;

private:
    std::string _symbol;
    Object *_startingObject;
    hif::semantics::ILanguageSemantics *_sem;
    DefinitionStyle::Type _style;

    void _dotConcatenation(Object *o, const std::string &objName);
    void _startConcatenation(Object *o, const std::string &objName);

    // disabled
    BuildHierarchyVisitor(const BuildHierarchyVisitor &o);
    BuildHierarchyVisitor &operator=(const BuildHierarchyVisitor &o);
};

BuildHierarchyVisitor::BuildHierarchyVisitor(
    Object *startingObject,
    hif::semantics::ILanguageSemantics *sem,
    const DefinitionStyle::Type style)
    : _symbol("")
    , _startingObject(startingObject)
    , _sem(sem)
    , _style(style)
{
    // ntd
}

BuildHierarchyVisitor::~BuildHierarchyVisitor()
{
    // ntd
}

void BuildHierarchyVisitor::map(Object *o) { callMap(o->getParent()); }

void BuildHierarchyVisitor::map(Alias *o) { _startConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(Const *o) { _startConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(DesignUnit *o)
{
    _symbol = o->getName() + _symbol; // see view

    map(static_cast<Object *>(o));
}

void BuildHierarchyVisitor::map(EnumValue *o) { _startConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(Field *o) { _startConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(Function *o) { _dotConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(Instance *o) { _dotConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(LibraryDef *o) { _dotConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(Parameter *o) { _startConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(Port *o) { _startConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(Procedure *o) { _dotConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(Signal *o) { _startConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(StateTable *o)
{
    if (dynamic_cast<BaseContents *>(o->getParent()) != nullptr) {
        _dotConcatenation(o, o->getName());
        return;
    }
    map(static_cast<Object *>(o));
}

void BuildHierarchyVisitor::map(System *)
{
    return; // end
}

void BuildHierarchyVisitor::map(TypeDef *o)
{
    // starting object may be EnumValue or Record Field
    _dotConcatenation(o, o->getName());
}

void BuildHierarchyVisitor::map(TypeTP *o) { _startConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(Variable *o) { _startConcatenation(o, o->getName()); }

void BuildHierarchyVisitor::map(View *o)
{
    std::string previous = (_symbol.empty()) ? "" : "." + _symbol;

    if (_style == DefinitionStyle::HIF || _style == DefinitionStyle::VHDL) {
        _symbol = std::string("(") + o->getName() + ")" + previous;
    } else {
        _symbol = previous;
    }

    map(static_cast<Object *>(o));
}

void BuildHierarchyVisitor::map(ValueTP *o) { _startConcatenation(o, o->getName()); }

std::string BuildHierarchyVisitor::getResult() const { return _symbol; }

void BuildHierarchyVisitor::_dotConcatenation(Object *o, const std::string &objName)
{
    std::string previous = (_symbol.empty()) ? "" : "." + _symbol;
    _symbol              = objName + previous;

    map(o);
}

void BuildHierarchyVisitor::_startConcatenation(Object *o, const std::string &objName)
{
    messageAssert(o == _startingObject, "Expected as starting object", o, _sem);
    _symbol = objName;

    map(o);
}

std::string _wrapSymbol(const std::string &symbol, DefinitionStyle::Type style, Object *startingObject)
{
    switch (style) {
    case DefinitionStyle::HIF:
        return symbol;
    case DefinitionStyle::VHDL:
        if (dynamic_cast<LibraryDef *>(startingObject) != nullptr)
            return "work." + symbol + ".all";
        return "work." + symbol;
    case DefinitionStyle::VERILOG:
        return symbol;
    case DefinitionStyle::SYSTEMC:
    default:;
    }

    messageError("Unsupported definition style", nullptr, nullptr);
}

} // namespace

std::string buildHierarchicalSymbol(Object *obj, hif::semantics::ILanguageSemantics *sem, DefinitionStyle::Type style)
{
    messageAssert(obj != nullptr, "Expected object", obj, sem);
    messageAssert(hif::NameTable::getInstance()->none() != hif::objectGetName(obj), "Expected name", obj, sem);

    BuildHierarchyVisitor bhv(obj, sem, style);
    bhv.callMap(obj);
    std::string symbol = bhv.getResult();
    return _wrapSymbol(symbol, style, obj);
}
} // namespace manipulation
} // namespace hif
