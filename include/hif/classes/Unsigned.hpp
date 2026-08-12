/// @file Unsigned.hpp
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

/// @brief Unsigned numeric type.
///
/// @details
/// This class represents an unsigned numeric type.
class Unsigned : public SimpleType, public hif::features::ITypeSpan
{
public:
    /// @brief Constructor.
    Unsigned();

    /// @brief Destructor.
    virtual ~Unsigned();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Sets the span of the unsigned type.
    /// @param r The new span of the unsigned type.
    /// @return The old span of the unsigned type if it is different
    /// from the new one, nullptr otherwise.
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
    Unsigned(const Unsigned &);
    Unsigned &operator=(const Unsigned &);
};

} // namespace hif
