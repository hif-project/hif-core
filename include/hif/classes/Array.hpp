/// @file Array.hpp
/// @brief Array type definition for HIF composite data structures.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/CompositeType.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/ITypeSpan.hpp"

namespace hif
{

///	@brief Array type.
/// @details
/// This class represents an array type.
/// It specifies the type of the array elements and the span of the array. The
/// span indicates the range of values to be used as indices to access the array
/// elements.
/// The <tt>signed</tt> attribute is used to determine whether the array has to
/// be interpreted with signed arithmetic (e.g., in VHDL).
class Array : public CompositeType, public hif::features::ITypeSpan
{
public:
    /// @brief Constructor.
    Array();

    /// @brief Destructor.
    virtual ~Array();

    Array(const Array &)                     = delete;
    auto operator=(const Array &) -> Array & = delete;

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Sets the span of the array.
    /// @param t The span of the array to be set.
    /// @return The old span of the array if it is different from the new one,
    /// nullptr otherwise.
    Range *setSpan(Range *t);

    /// @brief Sets the <tt>signed</tt> attribute.
    /// @param sign The <tt>signed</tt> attribute.
    void setSigned(bool sign);

    /// @brief Returns the <tt>signed</tt> attribute.
    /// @return The <tt>signed</tt> attribute.
    bool isSigned() const;

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

    /// @brief Returns the name of the given child relative to this object.
    /// @param child Pointer to the child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    /// @brief Flag to store the <tt>signed</tt> attribute.
    bool _isSigned;
};

} // namespace hif
