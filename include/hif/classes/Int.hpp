/// @file Int.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/SimpleType.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/ITypeSpan.hpp"

namespace hif
{

///	@brief Integer type.
///
/// @details
/// This class represents the integer type.
/// It specifies the bitwidth in terms of span and the signedness.
class Int : public SimpleType, public hif::features::ITypeSpan
{
public:
    /// @brief Constructor.
    Int();

    /// @brief Destructor.
    virtual ~Int();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Sets the bitwidth of the integer type.
    /// @param r The bitwidth of the integer type to be set.
    /// @return The old bitwidth of the integer type if it is different
    /// from the new one, nullptr otherwise.
    Range *setSpan(Range *r);

    /// @brief Returns the <tt>signed</tt> attribute.
    /// @return The <tt>signed</tt> attribute.
    bool isSigned() const;

    /// @brief Sets the <tt>signed</tt> attribute.
    /// @param sign The <tt>signed</tt> attribute.
    void setSigned(bool sign);

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

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    /// @brief Flag to store the <tt>signed</tt> attribute.
    bool _isSigned;

    // disabled
    Int(const Int &);
    Int &operator=(const Int &);
};

} // namespace hif
