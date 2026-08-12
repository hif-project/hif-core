/// @file ParameterAssign.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/PPAssign.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/ISymbol.hpp"

namespace hif
{

///	@brief Parameter assignment.
///
/// @details
/// This class represents a parameter assignment (i.e., an argument/actual
/// parameter).
///
/// @see PPAssign, Parameter
class ParameterAssign : public PPAssign, public features::TemplateSymbolIf<Parameter>
{
public:
    /// @brief Constructor.
    ParameterAssign();

    /// @brief Destructor.
    virtual ~ParameterAssign();

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
    // K: disabled.
    ParameterAssign(const ParameterAssign &);
    ParameterAssign &operator=(const ParameterAssign &);
};

} // namespace hif
