/// @file collectSymbols.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/semantics/collectSymbols.hpp"

#include "hif/GuideVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-private-field"
#elif __GNUC__
#    pragma GCC diagnostic ignored "-Weffc++"
#endif
namespace hif
{
namespace semantics
{

namespace /*anon*/
{

/****************************************************************************/
/*  Collect symbols visitor                                                 */
/****************************************************************************/
class CollectSymbolsVisitor : public GuideVisitor
{
public:
    CollectSymbolsVisitor(std::list<Object *> &list, ILanguageSemantics *sem, const bool skip_standard_declarations);
    ~CollectSymbolsVisitor();

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
    std::list<Object *> &_list;
    ILanguageSemantics *_sem;
    const bool _skipStandardDeclarations;

    template <typename T>

    // K: disabled
    CollectSymbolsVisitor(const CollectSymbolsVisitor &)            = delete;
    CollectSymbolsVisitor &operator=(const CollectSymbolsVisitor &) = delete;
};

CollectSymbolsVisitor::CollectSymbolsVisitor(
    std::list<Object *> &list,
    ILanguageSemantics *sem,
    const bool skip_standard_declarations)
    : GuideVisitor()
    , _list(list)
    , _sem(sem)
    , _skipStandardDeclarations(skip_standard_declarations)
{
    // ntd
}

CollectSymbolsVisitor::~CollectSymbolsVisitor()
{
    // ntd
}

int CollectSymbolsVisitor::visitIdentifier(Identifier &o)
{
    GuideVisitor::visitIdentifier(o);
    _list.push_back(&o);
    return 0;
}

int CollectSymbolsVisitor::visitParameterAssign(ParameterAssign &o)
{
    GuideVisitor::visitParameterAssign(o);
    _list.push_back(&o);
    return 0;
}

int CollectSymbolsVisitor::visitPortAssign(PortAssign &o)
{
    GuideVisitor::visitPortAssign(o);
    _list.push_back(&o);
    return 0;
}

int CollectSymbolsVisitor::visitFieldReference(FieldReference &o)
{
    GuideVisitor::visitFieldReference(o);
    _list.push_back(&o);
    return 0;
}

int CollectSymbolsVisitor::visitInstance(Instance &o)
{
    GuideVisitor::visitInstance(o);
    _list.push_back(&o);
    return 0;
}

int CollectSymbolsVisitor::visitLibrary(Library &o)
{
    GuideVisitor::visitLibrary(o);
    _list.push_back(&o);
    return 0;
}

int CollectSymbolsVisitor::visitFunctionCall(FunctionCall &o)
{
    GuideVisitor::visitFunctionCall(o);
    _list.push_back(&o);
    return 0;
}

int CollectSymbolsVisitor::visitProcedureCall(ProcedureCall &o)
{
    GuideVisitor::visitProcedureCall(o);
    _list.push_back(&o);
    return 0;
}

int CollectSymbolsVisitor::visitTypeTPAssign(TypeTPAssign &o)
{
    GuideVisitor::visitTypeTPAssign(o);
    _list.push_back(&o);
    return 0;
}

int CollectSymbolsVisitor::visitValueTPAssign(ValueTPAssign &o)
{
    GuideVisitor::visitValueTPAssign(o);
    _list.push_back(&o);
    return 0;
}

int CollectSymbolsVisitor::visitTypeReference(TypeReference &o)
{
    GuideVisitor::visitTypeReference(o);
    _list.push_back(&o);
    return 0;
}

int CollectSymbolsVisitor::visitViewReference(ViewReference &o)
{
    GuideVisitor::visitViewReference(o);
    _list.push_back(&o);
    return 0;
}
} // namespace

void collectSymbols(SymbolList &list, Object *root, ILanguageSemantics *sem, const bool skip_standard_declarations)
{
    hif::application_utils::initializeLogHeader("HIF", "collectSymbols");

    if (root == nullptr)
        return;

    CollectSymbolsVisitor csv(list, sem, skip_standard_declarations);
    root->acceptVisitor(csv);

    hif::application_utils::restoreLogHeader();
}

} // namespace semantics
} // namespace hif
