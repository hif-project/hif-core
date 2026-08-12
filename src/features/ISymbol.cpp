/// @file ISymbol.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/features/ISymbol.hpp"

#include "hif/application_utils/application_utils.hpp"
#include "hif/classes/classes.hpp"

namespace hif
{
namespace features
{

// /////////////////////////////////////////////////////////////////////////////
// Non-template base class.
// /////////////////////////////////////////////////////////////////////////////

ISymbol::ISymbol()
{
    // ntd
}

ISymbol::~ISymbol()
{
    // ntd
}

ISymbol::ISymbol(const ISymbol &other)
    : IFeature(other)
{
    // ntd
}

ISymbol &ISymbol::operator=(const ISymbol &other)
{
    IFeature::operator=(other);
    return *this;
}
// /////////////////////////////////////////////////////////////////////////////
// Template class
// /////////////////////////////////////////////////////////////////////////////

template <class T> bool TemplateSymbolIf<T>::matchDeclarationType(Object *o)
{
    return (dynamic_cast<DeclarationType *>(o) != nullptr);
}

template <class T>
TemplateSymbolIf<T>::TemplateSymbolIf()
    : ISymbol()
    , _declaration(nullptr)
{
    // ntd
}

template <class T> TemplateSymbolIf<T>::~TemplateSymbolIf()
{
    // ntd
}

template <class T>
TemplateSymbolIf<T>::TemplateSymbolIf(const TemplateSymbolIf<T> &other)
    : ISymbol(other)
    , _declaration(other._declaration)
{
    // ntd
}

template <class T> TemplateSymbolIf<T> &TemplateSymbolIf<T>::operator=(const TemplateSymbolIf<T> & /*other*/)
{
    return *this;
}

template <class T> void TemplateSymbolIf<T>::setDeclaration(Object *d)
{
    DeclarationType *decl = dynamic_cast<DeclarationType *>(d);
    messageAssert(d == nullptr || decl != nullptr, "Wrong declaration type", d, nullptr);
    _declaration = decl;
}

template <class T> typename TemplateSymbolIf<T>::DeclarationType *TemplateSymbolIf<T>::GetDeclaration()
{
    return _declaration;
}
// /////////////////////////////////////////////////////////////////////////////
// Instantiations
// /////////////////////////////////////////////////////////////////////////////

/// @relates TemplateSymbolIf
/// @brief Explicit instantiation for the `TemplateSymbolIf` class template with `DataDeclaration`.
template class TemplateSymbolIf<DataDeclaration>;

/// @relates TemplateSymbolIf
/// @brief Explicit instantiation for the `TemplateSymbolIf` class template with `Declaration`.
template class TemplateSymbolIf<Declaration>;

/// @relates TemplateSymbolIf
/// @brief Explicit instantiation for the `TemplateSymbolIf` class template with `Function`.
template class TemplateSymbolIf<Function>;

/// @relates TemplateSymbolIf
/// @brief Explicit instantiation for the `TemplateSymbolIf` class template with `Entity`.
template class TemplateSymbolIf<Entity>;

/// @relates TemplateSymbolIf
/// @brief Explicit instantiation for the `TemplateSymbolIf` class template with `LibraryDef`.
template class TemplateSymbolIf<LibraryDef>;

/// @relates TemplateSymbolIf
/// @brief Explicit instantiation for the `TemplateSymbolIf` class template with `Parameter`.
template class TemplateSymbolIf<Parameter>;

/// @relates TemplateSymbolIf
/// @brief Explicit instantiation for the `TemplateSymbolIf` class template with `Port`.
template class TemplateSymbolIf<Port>;

/// @relates TemplateSymbolIf
/// @brief Explicit instantiation for the `TemplateSymbolIf` class template with `SubProgram`.
template class TemplateSymbolIf<SubProgram>;

/// @relates TemplateSymbolIf
/// @brief Explicit instantiation for the `TemplateSymbolIf` class template with `TypeDeclaration`.
template class TemplateSymbolIf<TypeDeclaration>;

/// @relates TemplateSymbolIf
/// @brief Explicit instantiation for the `TemplateSymbolIf` class template with `TypeTP`.
template class TemplateSymbolIf<TypeTP>;

/// @relates TemplateSymbolIf
/// @brief Explicit instantiation for the `TemplateSymbolIf` class template with `ValueTP`.
template class TemplateSymbolIf<ValueTP>;

/// @relates TemplateSymbolIf
/// @brief Explicit instantiation for the `TemplateSymbolIf` class template with `View`.
template class TemplateSymbolIf<View>;

} // namespace features
} // namespace hif
