/// @file DeclarationVisitor.hpp
/// @brief Visitor for processing and fixing declarations in HIF objects.
/// @details
/// Provides a mechanism to process declarations before their associated symbols
/// and manage references. Includes extensive options for customization during traversal.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/GuideVisitor.hpp"
#include "hif/features/ISymbol.hpp"
#include "hif/semantics/ILanguageSemantics.hpp"
#include "hif/semantics/referencesUtils.hpp"

namespace hif
{

/// @brief Options for customizing declaration visitor behavior.
/// @details
/// Provides flags and references for controlling the traversal of declarations, symbols, and references.
struct DeclarationVisitorOptions {
    /// @brief Visit each declaration only once. Default is false.
    bool visitDeclarationsOnce;

    /// @brief Visit each symbol only once. Default is false.
    bool visitSymbolsOnce;

    /// @brief Visit references after processing declarations. Default is false.
    bool visitReferencesAfterDeclaration;

    /// @brief Pointer to the references map.
    hif::semantics::ReferencesMap *refsMap;

    /// @brief Pointer to the language semantics instance.
    hif::semantics::ILanguageSemantics *sem;

    DeclarationVisitorOptions();
    ~DeclarationVisitorOptions();
    /// @brief Copy constructor.
    /// @param o The object to copy.
    DeclarationVisitorOptions(const DeclarationVisitorOptions &o);
    /// @brief Copy assignment operator.
    /// @param o The object to assign.
    /// @return Reference to this object.
    DeclarationVisitorOptions &operator=(DeclarationVisitorOptions o);
    /// @brief Swaps the contents with another object.
    /// @param o The object to swap with.
    void swap(DeclarationVisitorOptions &o);
};

/// @brief Visitor for fixing declarations before their symbols.
/// @details
/// Supports traversing HIF declarations, symbols, and references while managing visitation flags.
class DeclarationVisitor : public GuideVisitor
{
public:
    /// @brief Set of visited declarations.
    typedef std::set<Declaration *> DeclarationSet;

    /// @brief Set of visited symbols.
    typedef std::set<Object *> SymbolSet;

    /// @brief Constructor.
    /// @param opt Options for controlling the visitor behavior.
    /// @param root Root system to traverse. Default is nullptr.
    DeclarationVisitor(const DeclarationVisitorOptions &opt, System *root = nullptr);

    /// @brief Deleted copy constructor.
    DeclarationVisitor(const DeclarationVisitor &) = delete;

    /// @brief Deleted assignment operator.
    DeclarationVisitor &operator=(const DeclarationVisitor &) = delete;

    /// @brief Destructor.
    virtual ~DeclarationVisitor();

    /// @name Common virtual methods
    /// @{

    /// @brief Visits a declaration.
    /// @param o The declaration to visit.
    /// @return True if visited successfully, false otherwise.
    virtual int visitDeclaration(Declaration &o);

    /// @brief Visits a data declaration.
    /// @param o The data declaration to visit.
    /// @return True if visited successfully, false otherwise.
    virtual int visitDataDeclaration(DataDeclaration &o);

    /// @brief Visits a symbol.
    /// @param o The symbol to visit.
    /// @return Return value of the symbol visit.
    virtual int visitSymbol(hif::features::ISymbol &o);

    /// @brief Visits references associated with a declaration.
    /// @param o The declaration whose references are to be visited.
    /// @return Combined result of visiting all references.
    virtual int visitReferences(Declaration &o);

    /// @}

    /// @name Symbols visits
    /// @{
    virtual int visitFieldReference(FieldReference &o);
    virtual int visitFunctionCall(FunctionCall &o);
    virtual int visitIdentifier(Identifier &o);
    virtual int visitInstance(Instance &o);
    virtual int visitLibrary(Library &o);
    virtual int visitParameterAssign(ParameterAssign &o);
    virtual int visitPortAssign(PortAssign &o);
    virtual int visitProcedureCall(ProcedureCall &o);
    virtual int visitTypeReference(TypeReference &o);
    virtual int visitTypeTPAssign(TypeTPAssign &o);
    virtual int visitValueTPAssign(ValueTPAssign &o);
    virtual int visitViewReference(ViewReference &o);
    /// @}

    /// @name Declarations visits
    /// @{
    virtual int visitContents(Contents &o);
    virtual int visitDesignUnit(DesignUnit &o);
    virtual int visitFunction(Function &o);
    virtual int visitEntity(Entity &o);
    virtual int visitForGenerate(ForGenerate &o);
    virtual int visitIfGenerate(IfGenerate &o);
    virtual int visitLibraryDef(LibraryDef &o);
    virtual int visitProcedure(Procedure &o);
    virtual int visitState(State &o);
    virtual int visitStateTable(StateTable &o);
    virtual int visitTypeDef(TypeDef &o);
    virtual int visitTypeTP(TypeTP &o);
    virtual int visitView(View &o);
    virtual int visitSystem(System &o);
    /// @}

    /// @name DataDeclarations visits
    /// @{
    virtual int visitAlias(Alias &o);
    virtual int visitConst(Const &o);
    virtual int visitEnumValue(EnumValue &o);
    virtual int visitField(Field &o);
    virtual int visitParameter(Parameter &o);
    virtual int visitPort(Port &o);
    virtual int visitSignal(Signal &o);
    virtual int visitValueTP(ValueTP &o);
    virtual int visitVariable(Variable &o);
    /// @}

protected:
    /// @brief Wrapper for getting a declaration.
    /// @param o The object to retrieve the declaration from.
    /// @return The corresponding declaration, or nullptr if not found.
    virtual Declaration *_getDeclaration(Object *o);

    /// @brief Checks if a symbol has already been visited.
    /// @param o The symbol to check.
    /// @return True if the symbol was not visited before, false otherwise.
    virtual bool _checkSymbol(hif::features::ISymbol &o);

    /// @brief Checks if a declaration has already been visited.
    /// @param o The declaration to check.
    /// @return True if the declaration was not visited before, false otherwise.
    virtual bool _checkDeclaration(Declaration &o);

    DeclarationVisitorOptions _opt; ///< Options controlling the visitor behavior.

private:
    DeclarationSet _visitedDeclarations; ///< Set of visited declarations.
    SymbolSet _visitedSymbols;           ///< Set of visited symbols.
};

} // namespace hif
