/// @file PrefixedReference.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

///	@brief Base class for prefixed references.
///
/// @details
/// This class is a base class for prefixed references (i.e., accesses
/// to elements of arrays or records).
///
/// @see FieldReference, Member, Slice
class PrefixedReference : public Value
{
public:
    /// @brief Constructor.
    PrefixedReference();

    /// @brief Destructor.
    virtual ~PrefixedReference();

    /// @brief Returns the prefix.
    /// @return The prefix.
    Value *getPrefix() const;

    /// @brief Sets the prefix.
    /// @param v The prefix to be set.
    /// @return The old prefix if it is different from the new one,
    /// nullptr otherwise.
    Value *setPrefix(Value *v);

protected:
    /// @brief The prefix.
    Value *_prefix;

    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    // K: disabled.
    PrefixedReference(const PrefixedReference &);
    PrefixedReference &operator=(const PrefixedReference &);
};

} // namespace hif
