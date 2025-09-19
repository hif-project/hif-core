/// @file Real.hpp
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

/// @brief Real data type.
///
/// @details
/// This class represents the real data type.
/// The real data type can be restricted to a given range. The physical
/// implementation of a real type cannot be specified.
class Real : public SimpleType, public hif::features::ITypeSpan
{
public:
    /// @brief Constructor.
    Real();

    /// @brief Destructor.
    virtual ~Real();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Sets the range constraint on the real data type.
    /// @param r The range constraint to be set on the real data type.
    /// @return The previous range constraint, or nullptr if none.
    Range *setSpan(Range *r);

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
    // K: disabled
    Real(const Real &);
    Real &operator=(const Real &);
};

} // namespace hif
