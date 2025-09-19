/// @file ISymbol.hpp
/// @brief Interface for symbol objects in the HIF framework.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <string>

#include "hif/application_utils/portability.hpp"
#include "hif/features/IFeature.hpp"

namespace hif
{
class Object;

namespace semantics
{

void setDeclaration(Object *o, Object *decl);
} // namespace semantics

namespace features
{

/// @brief Non-template base class.
/// Interface for symbols, including their declaration type and other
/// common features.
class ISymbol : public IFeature
{
public:
    /// @brief Virtual destructor.
    virtual ~ISymbol() = 0;

    /// @brief Given an object, check whether it matches the DeclarationType.
    /// @param o The object to check.
    /// @return True if the object matches the declaration type.
    virtual bool matchDeclarationType(Object *o) = 0;

protected:
    /// @brief Default constructor.
    ISymbol();

    /// @brief Copy constructor.
    /// @param other The ISymbol to copy.
    ISymbol(const ISymbol &other);

    /// @brief Assignment operator.
    /// @param other The ISymbol to assign.
    /// @return Reference to this.
    ISymbol &operator=(const ISymbol &other);

    /// @brief Set the declaration of the corresponding symbol.
    /// @param d pointer to a DeclarationType that contains the
    /// corresponding symbol declaration.
    virtual void setDeclaration(Object *d) = 0;

    /// @brief Friend function to set declaration.
    /// @param o The object.
    /// @param decl The declaration.
    friend void hif::semantics::setDeclaration(Object *o, Object *decl);
};
/// @brief Interface for symbols, including their declaration type and other
/// common features.
template <class T> class TemplateSymbolIf : public ISymbol
{
public:
    /// @brief Type alias for the declaration type.
    typedef T DeclarationType;

    /// @brief Virtual destructor.
    virtual ~TemplateSymbolIf() = 0;

    /// @brief Check if the object matches the declaration type.
    /// @param o The object to check.
    /// @return True if the object matches the declaration type.
    virtual bool matchDeclarationType(Object *o);

    /// @brief Get the declaration of the corresponding symbol.
    /// @return Pointer to a DeclarationType that contains the
    /// corresponding symbol declaration.
    DeclarationType *GetDeclaration();

protected:
    /// @brief Default constructor.
    TemplateSymbolIf();

    /// @brief Copy constructor.
    /// @param other The TemplateSymbolIf to copy.
    TemplateSymbolIf(const TemplateSymbolIf<T> &other);

    /// @brief Assignment operator.
    /// @param other The TemplateSymbolIf to assign.
    /// @return Reference to this.
    TemplateSymbolIf<T> &operator=(const TemplateSymbolIf<T> &other);

    /// @brief Set the declaration of the corresponding symbol.
    /// @param d Pointer to a DeclarationType that contains the
    /// corresponding symbol declaration.
    virtual void setDeclaration(Object *d);

    /// @brief The declaration.
    DeclarationType *_declaration;
};

} // namespace features
} // namespace hif
