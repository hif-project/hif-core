/// @file ISymbol.hpp
/// @brief
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
    virtual ~ISymbol() = 0;

    /// @brief Given an object, check whether it matches the DeclarationType.
    virtual bool matchDeclarationType(Object *o) = 0;

protected:
    ISymbol();

    ISymbol(const ISymbol &other);

    ISymbol &operator=(const ISymbol &other);

    /// @brief Set the declaration of the corresponding symbol.
    /// @param d pointer to a DeclarationType that contains the
    /// corresponding symbol declaration.
    virtual void setDeclaration(Object *d) = 0;

    friend void hif::semantics::setDeclaration(Object *o, Object *decl);
};
/// @brief Interface for symbols, including their declaration type and other
/// common features.
template <class T> class TemplateSymbolIf : public ISymbol
{
public:
    typedef T DeclarationType;

    virtual ~TemplateSymbolIf() = 0;

    virtual bool matchDeclarationType(Object *o);

    /// @brief Get the declaration of the corresponding symbol.
    /// @return Pointer to a DeclarationType that contains the
    /// corresponding symbol declaration.
    DeclarationType *GetDeclaration();

protected:
    TemplateSymbolIf();

    TemplateSymbolIf(const TemplateSymbolIf<T> &other);

    TemplateSymbolIf<T> &operator=(const TemplateSymbolIf<T> &other);

    virtual void setDeclaration(Object *d);

    /// @brief The declaration.
    DeclarationType *_declaration;
};

} // namespace features
} // namespace hif
