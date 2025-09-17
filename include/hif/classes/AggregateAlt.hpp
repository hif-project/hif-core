/// @file AggregateAlt.hpp
/// @brief Alternative aggregate structure for HIF data representation.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Alt.hpp"
#include "hif/classes/BList.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Alternative for an Aggregate.
/// @details
/// This class represents an alternative for an Aggregate.
/// It consists of a list of indices and a corresponding value.
/// @see Aggregate
class AggregateAlt : public Alt
{
public:
    /// @brief The list of indices for which the alternative defines the value.
    BList<Value> indices;

    /// @brief Constructor.
    AggregateAlt();

    /// @brief Destructor.
    virtual ~AggregateAlt();

    AggregateAlt(const AggregateAlt &)                     = delete;
    auto operator=(const AggregateAlt &) -> AggregateAlt & = delete;

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the value of the aggregate alternative.
    /// @return The value of the aggregate alternative.
    Value *getValue() const;

    /// @brief Sets the value of the aggregate alternative.
    /// @param v The value to be set for the aggregate alternative.
    /// @return old The old value of the aggregate alternative if it is
    /// different from the new one, nullptr otherwise.
    Value *setValue(Value *v);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of the given child relative to this object.
    /// @param child Pointer to the child object.
    /// @return The child's name as a string.
    virtual std::string _getFieldName(const Object *child) const;

    /// @brief Returns the name of the given BList relative to this object.
    /// @param list Reference to the BList of Object instances.
    /// @return The BList's name as a string.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief The value of the aggregate alternative.
    Value *_value;
};

} // namespace hif
