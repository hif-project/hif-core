/// @file Identifier.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Value.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/INamedObject.hpp"
#include "hif/features/ISymbol.hpp"

namespace hif
{

///	@brief Identifier.
///
/// @details
/// This class represents the occurrence of an identifier.
/// An identifier is characterized by the corresponding name in the NameTable.
///
/// @see NameTable
class Identifier : public Value, public features::TemplateSymbolIf<DataDeclaration>, public features::INamedObject
{
public:
    /// @brief Default constructor.
    Identifier();

    /// @brief Destructor.
    virtual ~Identifier();

    /// @brief Constructor.
    /// @param name The name to be assigned.
    Identifier(const std::string &name);

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

    /// @brief Returns this object as hif::Object.
    /// @return This object as hif::Object.
    virtual Object *toObject();

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    // K: disabled
    Identifier(const Identifier &);
    Identifier &operator=(const Identifier &);
};

} // namespace hif
