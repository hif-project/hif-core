/// @file Slice.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/PrefixedReference.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Slice of a vector.
///
/// @details
/// This class represents a slice of a vector (i.e., a contiguous portion
/// of a given vector identified by a corresponding span).
class Slice : public PrefixedReference
{
public:
    /// @brief Constructor.
    Slice();

    /// @brief Destructor.
    virtual ~Slice();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Return the span of the slice.
    /// @return The span of the slice.
    Range *getSpan() const;

    /// @brief Sets the span of the slice.
    /// @param r The span of the slice to be set.
    /// @return The old span of the slice, or nullptr if none.
    Range *setSpan(Range *r);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object to get the name for.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    /// @brief The span of the slice.
    Range *_span;

    // K: disabled
    Slice(const Slice &);
    Slice &operator=(const Slice &);
};

} // namespace hif
