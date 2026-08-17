/// @file hif_simplified_symbol_iterated_concat_test.cpp
/// @brief
/// Copyright (c) 2026, Electronic Systems Design (ESD) Group,
/// University of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.
///
/// Producer-level regression for hif-frontend#15.
///
/// A getSimplifiedSymbol() implementation must return a subtree that is closed
/// under its destination semantics: every standard symbol it hands back has to
/// be spelled and resolvable in the destination domain, not just the outermost
/// one.
///
/// `_getSimplifiedSymbol_iteratedConcat` used to violate that. It copies the
/// whole source call (so it can keep a symbolic replication count), renames the
/// outer call to `hif_verilog_iterated_concat` and forces the outer referenced
/// Library to `hif_verilog_standard`, but left every *nested* standard call in
/// the copy spelled in source form. For `{$clog2(DEPTH){1'b1}}` the copy came
/// back holding a `_system_clog2` call whose referenced Library was already
/// `hif_verilog_standard` -- a mixed-domain subtree. `hif_verilog_standard`
/// declares `hif_verilog__system_clog2`, never `_system_clog2`, so nothing
/// under that nested call could resolve and mapStandardSymbols asserted
/// "Expected declaration" on the first such node it reached.
///
/// This test drives the public seam, HIFSemantics::getSimplifiedSymbol(), on a
/// minimal HIF tree shaped exactly like the one mapStandardSymbols hands to the
/// producer at that point: call names still in source form, referenced
/// libraries already renamed to destination form by the library-fix step.

#include <hif/hif.hpp>
#include <iostream>
#include <string>

using namespace hif;
using namespace hif::semantics;

namespace
{

const char *const kSourceFile  = "repc.v";
const unsigned int kConcatLine = 2;
const unsigned int kConcatCol  = 14;
const unsigned int kClog2Line  = 2;
const unsigned int kClog2Col   = 15;

bool failed = false;

void check(bool condition, const std::string &what)
{
    if (!condition) {
        std::cerr << "FAIL: " << what << "\n";
        failed = true;
    }
}

/// @brief Collects every FunctionCall and Library in a subtree.
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

/// @brief Counts subtree calls/libraries/identifiers matching a name.
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

/// @brief Builds an `Instance` prefix pointing at the destination standard
/// library, exactly as mapStandardSymbols leaves it after its library-fix step:
/// the Instance keeps its source spelling `standard`, the referenced Library has
/// already been renamed to `hif_verilog_standard`.
Instance *makeStandardPrefix(unsigned int line, unsigned int column)
{
    Library *lib = new Library();
    lib->setName("hif_verilog_standard");
    lib->setSystem(true);
    lib->setStandard(false);
    setCodeInfo(lib, line, column);

    Instance *inst = new Instance();
    inst->setName("standard");
    inst->setReferencedType(lib);
    setCodeInfo(inst, line, column);

    return inst;
}

/// @brief `$clog2(DEPTH)` after the frontend's `$` -> `_system` rename.
FunctionCall *makeClog2Call()
{
    Identifier *depth = new Identifier("DEPTH");
    setCodeInfo(depth, kClog2Line, kClog2Col + 7);

    ParameterAssign *pa = new ParameterAssign();
    pa->setName("value");
    pa->setValue(depth);
    setCodeInfo(pa, kClog2Line, kClog2Col + 7);

    FunctionCall *fc = new FunctionCall();
    fc->setName("_system_clog2");
    fc->setInstance(makeStandardPrefix(kClog2Line, kClog2Col));
    fc->parameterAssigns.push_back(pa);
    setCodeInfo(fc, kClog2Line, kClog2Col);

    return fc;
}

/// @brief `{$clog2(DEPTH){1'b1}}` as verilog2hif's parser builds it.
FunctionCall *makeIteratedConcatCall()
{
    ValueTPAssign *times = new ValueTPAssign();
    times->setName("times");
    times->setValue(makeClog2Call());
    setCodeInfo(times, kConcatLine, kConcatCol);

    // verilog2hif emits `1'b1` here as a one-bit constexpr logic Bitvector,
    // not a Bit - see the FCALL/expression node of any translated replication.
    Bitvector *oneType = new Bitvector();
    oneType->setSpan(new Range(0, 0));
    oneType->setLogic(true);
    oneType->setResolved(true);
    oneType->setConstexpr(true);

    BitvectorValue *one = new BitvectorValue("1");
    one->setType(oneType);

    ParameterAssign *expression = new ParameterAssign();
    expression->setName("expression");
    expression->setValue(one);
    setCodeInfo(expression, kConcatLine, kConcatCol + 15);

    FunctionCall *fc = new FunctionCall();
    fc->setName("iterated_concat");
    fc->setInstance(makeStandardPrefix(kConcatLine, kConcatCol));
    fc->templateParameterAssigns.push_back(times);
    fc->parameterAssigns.push_back(expression);
    setCodeInfo(fc, kConcatLine, kConcatCol);

    return fc;
}

/// @brief Minimal `module repc #(parameter DEPTH = 32) (output [7:0] o);`
/// carrying the replication as its single global assignment, plus the real
/// destination standard library so declarations can actually resolve.
System *buildTree(FunctionCall *concat)
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
    assign->setRightHandSide(concat);

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
    // Same library object mapStandardSymbols inserts, copied so the test owns it.
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
    FunctionCall *concat = makeIteratedConcatCall();
    System *system       = buildTree(concat);

    // The producer must not mutate the caller-owned source subtree.
    Object *sourceSnapshot = hif::copy(concat);

    ILanguageSemantics::KeySymbol key(std::string("standard"), std::string("iterated_concat"));
    Object *produced = HIFSemantics::getInstance()->getSimplifiedSymbol(key, concat);

    check(produced != nullptr, "getSimplifiedSymbol returned nullptr");
    if (produced == nullptr) {
        delete system;
        delete sourceSnapshot;
        return 1;
    }

    check(hif::equals(concat, sourceSnapshot), "the source input subtree was mutated by the producer");

    FunctionCall *outer = dynamic_cast<FunctionCall *>(produced);
    check(outer != nullptr, "produced object is not a FunctionCall");
    if (outer == nullptr) {
        delete system;
        delete sourceSnapshot;
        delete produced;
        return 1;
    }

    // 1. Outer call is in canonical destination form, exactly once.
    check(outer->getName() == "hif_verilog_iterated_concat", "outer call is not 'hif_verilog_iterated_concat'");

    Collector c;
    produced->acceptVisitor(c);

    // 2. No source-form standard call survives; the nested $clog2 is canonical.
    check(countNamed(c.calls, "_system_clog2") == 0, "a source-form '_system_clog2' call survives in the subtree");
    check(
        countNamed(c.calls, "hif_verilog__system_clog2") == 1,
        "expected exactly one canonical 'hif_verilog__system_clog2' call");

    // 3. Referenced libraries are destination-form, exactly once each.
    check(countNamed(c.libraries, "standard") == 0, "a source-form 'standard' Library survives in the subtree");
    check(
        countNamed(c.libraries, "hif_verilog_standard") == c.libraries.size(),
        "not every referenced Library is 'hif_verilog_standard'");
    for (std::list<Library *>::iterator i(c.libraries.begin()); i != c.libraries.end(); ++i) {
        check(
            (*i)->getName().find("hif_verilog_hif_verilog_") == std::string::npos,
            "a Library name carries a doubled 'hif_verilog_' prefix");
    }
    for (std::list<FunctionCall *>::iterator i(c.calls.begin()); i != c.calls.end(); ++i) {
        check(
            (*i)->getName().find("hif_verilog_hif_verilog_") == std::string::npos,
            "a call name carries a doubled 'hif_verilog_' prefix");
    }

    // 4. The symbolic replication count survives: no folding to a constant.
    check(countNamed(c.identifiers, "DEPTH") == 1, "the symbolic 'DEPTH' replication count was not preserved");

    // 5. User CodeInfo is preserved on the mapped nodes.
    check(outer->getSourceFileName() == kSourceFile, "outer call lost its source file name");
    check(outer->getSourceLineNumber() == kConcatLine, "outer call lost its source line");
    check(outer->getSourceColumnNumber() == kConcatCol, "outer call lost its source column");

    FunctionCall *clog2 = nullptr;
    for (std::list<FunctionCall *>::iterator i(c.calls.begin()); i != c.calls.end(); ++i) {
        if ((*i)->getName() == "hif_verilog__system_clog2")
            clog2 = *i;
    }
    check(clog2 != nullptr, "no canonical $clog2 call found in the produced subtree");
    if (clog2 != nullptr) {
        check(clog2->getSourceFileName() == kSourceFile, "$clog2 call lost its source file name");
        check(clog2->getSourceLineNumber() == kClog2Line, "$clog2 call lost its source line");
        check(clog2->getSourceColumnNumber() == kClog2Col, "$clog2 call lost its source column");
    }

    // 6. The whole subtree resolves under destination semantics once in tree,
    //    which is what mapStandardSymbols does immediately after this call.
    concat->replace(produced);
    delete concat;

    check(resolves(outer), "the outer iterated-concat call does not resolve under HIF semantics");
    if (clog2 != nullptr) {
        check(resolves(clog2), "the nested $clog2 call does not resolve under HIF semantics");
        check(
            !clog2->parameterAssigns.empty() && resolves(clog2->parameterAssigns.front()),
            "the nested $clog2 parameter assign does not resolve under HIF semantics");
    }

    delete system;
    delete sourceSnapshot;

    if (failed)
        return 1;

    std::cout << "PASS\n";
    return 0;
}
