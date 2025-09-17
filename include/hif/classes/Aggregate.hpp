/// @file Aggregate.hpp
/// @brief Composite data structure representing an aggregate value.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/AggregateAlt.hpp"
#include "hif/classes/Value.hpp"

namespace hif
{

///	@brief Composite type constant value.
/// @details
/// Aggregates are used to define constant values for array or record types.
/// They contain a list of AggregateAlt to describe the specified values of the
/// elements or fields, and a default value for all non-specified elements or
/// fields.
/// @see AggregateAlt
class Aggregate : public Value
{
public:
    /// @brief List of AggregateAlt to describe the specified values of the
    /// elements or fields of the constant.
    BList<AggregateAlt> alts;

    /// @brief Constructor.
    Aggregate();

    /// @brief Destructor.
    virtual ~Aggregate();

    Aggregate(const Aggregate &)                     = delete;
    auto operator=(const Aggregate &) -> Aggregate & = delete;

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the default value of the aggregate.
    /// @return Pointer to the aggregate default value.
    Value *getOthers() const;

    /// @brief Sets the default value of the aggregate (for all non-specified elements or fields).
    /// @param v The default value to be set.
    /// @return The old default value if it is different from
    /// the new one, nullptr otherwise.
    Value *setOthers(Value *v);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of the given child with respect to this object.
    /// @param child Pointer to the child object.
    /// @return The name of the child object as a string.
    virtual std::string _getFieldName(const Object *child) const;

    /// @brief Returns the name of the given BList with respect to this object.
    /// @param list Reference to the BList of Object instances.
    /// @return The name of the BList as a string.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief Default value for non-specified elements or fields.
    Value *_others;
};

} // namespace hif
