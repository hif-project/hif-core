/// @file PortAssign.hpp
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

///	@brief Port assignment.
///
/// @details
/// This class represents a port assignment (i.e., a port binding).
///
/// @see PPAssign
class PortAssign : public PPAssign, public features::TemplateSymbolIf<Port>
{
public:
    /// @brief Constructor.
    PortAssign();

    /// @brief Destructor.
    virtual ~PortAssign();

    /// @brief Returns the type of the bound port.
    /// NOTE: this field can be used for partial designs where the
    /// declaration of the bound port can not be found.
    /// Otherwise use the type of the port declaration.
    /// @return The type of the bound port.
    Type *getType() const;

    /// @brief Sets the type of the bound port.
    /// NOTE: this field can be used for partial designs where the
    /// declaration of the bound port can not be found.
    /// Otherwise use the type of the port declaration.
    /// @param t The type of the bound port to be set.
    /// @return The old type of the bound port if it is different
    /// from the new one, nullptr otherwise.
    Type *setType(Type *t);

    /// @brief Returns the partial binding of the bound port.
    /// @return The partial binding of the bound port.
    Value *getPartialBind() const;

    /// @brief Sets the partial binding of the bound port.
    /// @param pB The new partial binding.
    /// @return The old partial binding of the bound port if it is different
    /// from the new one, nullptr otherwise.
    Value *setPartialBind(Value *pB);

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
    /// @brief The type of the bound port.
    /// NOTE: this field can be used for partial designs where the
    /// declaration of the bound port can not be found.
    /// Otherwise use the type of port declaration.
    Type *_type;

    /// @brief If the bound port is not "associated in whole" but "associated
    /// individually" (see VHDL standard for more details) this field is used to
    /// keep trace of partial binding until refinements.
    Value *_partialBind;

    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    // K: disabled.
    PortAssign(const PortAssign &);
    PortAssign &operator=(const PortAssign &);
};

} // namespace hif
