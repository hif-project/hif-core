/// @file hif_simplified_symbol_verilog_integers_test.cpp
/// @brief
/// Copyright (c) 2026, Electronic Systems Design (ESD) Group,
/// University of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.
///
/// Producer-level regression for the second half of hif-frontend#15.
///
/// `_getSimplifiedSymbol_withVerilogIntegers` renamed the call it produced but
/// never touched the Library its Instance refers to. That was invisible for as
/// long as every input came straight out of the tree, because mapStandardSymbols
/// renames all standard-library references before it starts mapping symbols - so
/// the helper was handed a library that was already `hif_verilog_standard` and
/// leaving it alone happened to be right.
///
/// It is not right in general. mapStandardSymbols also maps symbols it finds in
/// a *copied semantic type*: for `{$clog2(DEPTH){1'b1}}` the replication's
/// return type embeds the `times` actual, and that copy predates the library
/// rename, so it is still entirely source-form. Feeding that to the helper
/// produced `hif_verilog__system_clog2` sitting under a Library still named
/// `standard` - a mixed-domain call that no destination library declares, and
/// mapStandardSymbols asserted "Expected declaration" on its parameter.
///
/// This drives the public seam, HIFSemantics::getSimplifiedSymbol(), with a
/// fully source-form `$clog2` call, which is exactly what that path delivers.

#include <hif/hif.hpp>
#include <iostream>
#include <string>

using namespace hif;
using namespace hif::semantics;

namespace
{

const char *const kSourceFile = "repc.v";
const unsigned int kClog2Line = 2;
const unsigned int kClog2Col  = 15;

bool failed = false;

void check(bool condition, const std::string &what)
{
    if (!condition) {
        std::cerr << "FAIL: " << what << "\n";
        failed = true;
    }
}

class Collector : public GuideVisitor
{
public:
    std::list<FunctionCall *> calls;
    std::list<Library *> libraries;
    std::list<Identifier *> identifiers;

    int visitFunctionCall(FunctionCall &o) override
    {
        calls.push_back(&o);
        return GuideVisitor::visitFunctionCall(o);
    }

    int visitLibrary(Library &o) override
    {
        libraries.push_back(&o);
        return GuideVisitor::visitLibrary(o);
    }

    int visitIdentifier(Identifier &o) override
    {
        identifiers.push_back(&o);
        return GuideVisitor::visitIdentifier(o);
    }
};

template <typename T> unsigned int countNamed(const std::list<T *> &l, const std::string &name)
{
    unsigned int n = 0;
    for (typename std::list<T *>::const_iterator i(l.begin()); i != l.end(); ++i) {
        if ((*i)->getName() == name)
            ++n;
    }
    return n;
}

void setCodeInfo(Object *o, unsigned int line, unsigned int column)
{
    o->setSourceFileName(kSourceFile);
    o->setSourceLineNumber(line);
    o->setSourceColumnNumber(column);
}

/// @brief `$clog2(DEPTH)` entirely in source form: the call name is the
/// post-`$`-rename `_system_clog2` and the referenced Library is still
/// `standard`, as it is inside a semantic type copied before the library fix.
FunctionCall *makeSourceFormClog2Call()
{
    Library *lib = new Library();
    lib->setName("standard");
    lib->setSystem(true);
    lib->setStandard(false);
    setCodeInfo(lib, kClog2Line, kClog2Col);

    Instance *inst = new Instance();
    inst->setName("standard");
    inst->setReferencedType(lib);
    setCodeInfo(inst, kClog2Line, kClog2Col);

    Identifier *depth = new Identifier("DEPTH");
    setCodeInfo(depth, kClog2Line, kClog2Col + 7);

    ParameterAssign *pa = new ParameterAssign();
    pa->setName("value");
    pa->setValue(depth);
    setCodeInfo(pa, kClog2Line, kClog2Col + 7);

    FunctionCall *fc = new FunctionCall();
    fc->setName("_system_clog2");
    fc->setInstance(inst);
    fc->parameterAssigns.push_back(pa);
    setCodeInfo(fc, kClog2Line, kClog2Col);

    return fc;
}

/// @brief Minimal `module repc #(parameter DEPTH = 32) (output [7:0] o);`
/// holding the call, with the destination standard library available so the
/// mapped call can actually resolve.
System *buildTree(FunctionCall *call)
{
    ValueTP *depth = new ValueTP();
    depth->setName("DEPTH");
    depth->setType(new Int());
    depth->setValue(new IntValue(32));

    Bitvector *oType = new Bitvector();
    oType->setSpan(new Range(7, 0));
    oType->setLogic(true);
    oType->setResolved(true);

    Port *o = new Port();
    o->setName("o");
    o->setDirection(dir_out);
    o->setType(oType);

    Entity *entity = new Entity();
    entity->ports.push_back(o);

    Assign *assign = new Assign();
    assign->setLeftHandSide(new Identifier("o"));
    assign->setRightHandSide(call);

    GlobalAction *ga = new GlobalAction();
    ga->actions.push_back(assign);

    Contents *contents = new Contents();
    contents->setName("Architecture");
    contents->setGlobalAction(ga);

    View *view = new View();
    view->setName("behav");
    view->setLanguageID(rtl);
    view->setStandard(false);
    view->templateParameters.push_back(depth);
    view->setEntity(entity);
    view->setContents(contents);

    DesignUnit *du = new DesignUnit();
    du->setName("repc");
    du->views.push_back(view);

    System *system = new System();
    system->setName("system");
    system->setLanguageID(rtl);
    system->libraryDefs.push_back(hif::copy(HIFSemantics::getInstance()->getStandardLibrary("hif_verilog_standard")));
    system->designUnits.push_back(du);

    return system;
}

bool resolves(Object *o)
{
    DeclarationOptions opt;
    opt.error = false;
    return hif::semantics::getDeclaration(o, HIFSemantics::getInstance(), opt) != nullptr;
}

} // namespace

auto main() -> int
{
    FunctionCall *call = makeSourceFormClog2Call();
    System *system     = buildTree(call);

    Object *sourceSnapshot = hif::copy(call);

    ILanguageSemantics::KeySymbol key(std::string("standard"), std::string("_system_clog2"));
    Object *produced = HIFSemantics::getInstance()->getSimplifiedSymbol(key, call);

    check(produced != nullptr, "getSimplifiedSymbol returned nullptr");
    if (produced == nullptr) {
        delete system;
        delete sourceSnapshot;
        return 1;
    }

    check(hif::equals(call, sourceSnapshot), "the source input subtree was mutated by the producer");

    FunctionCall *mapped = dynamic_cast<FunctionCall *>(produced);
    check(mapped != nullptr, "produced object is not a FunctionCall");
    if (mapped == nullptr) {
        delete system;
        delete sourceSnapshot;
        delete produced;
        return 1;
    }

    check(mapped->getName() == "hif_verilog__system_clog2", "the mapped call is not 'hif_verilog__system_clog2'");

    Collector c;
    produced->acceptVisitor(c);

    // The referenced library must be destination-form, exactly once.
    check(countNamed(c.libraries, "standard") == 0, "the referenced Library is still the source-form 'standard'");
    check(
        countNamed(c.libraries, "hif_verilog_standard") == 1,
        "expected exactly one 'hif_verilog_standard' referenced Library");
    for (std::list<Library *>::iterator i(c.libraries.begin()); i != c.libraries.end(); ++i) {
        check(
            (*i)->getName().find("hif_verilog_hif_verilog_") == std::string::npos,
            "a Library name carries a doubled 'hif_verilog_' prefix");
    }
    check(
        mapped->getName().find("hif_verilog_hif_verilog_") == std::string::npos,
        "the call name carries a doubled 'hif_verilog_' prefix");

    // The symbolic argument survives: no folding to a constant.
    check(countNamed(c.identifiers, "DEPTH") == 1, "the symbolic 'DEPTH' argument was not preserved");

    // User CodeInfo is preserved.
    check(mapped->getSourceFileName() == kSourceFile, "the mapped call lost its source file name");
    check(mapped->getSourceLineNumber() == kClog2Line, "the mapped call lost its source line");
    check(mapped->getSourceColumnNumber() == kClog2Col, "the mapped call lost its source column");

    // And it resolves under destination semantics once placed in the tree.
    call->replace(produced);
    delete call;

    check(resolves(mapped), "the mapped $clog2 call does not resolve under HIF semantics");
    check(
        !mapped->parameterAssigns.empty() && resolves(mapped->parameterAssigns.front()),
        "the mapped $clog2 parameter assign does not resolve under HIF semantics");

    delete system;
    delete sourceSnapshot;

    if (failed)
        return 1;

    std::cout << "PASS\n";
    return 0;
}
