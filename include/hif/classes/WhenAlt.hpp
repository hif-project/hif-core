/// @file WhenAlt.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Alt.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Conditional expression alternative.
///
/// @details
/// This class represents an alternative for a conditional expression (When).
/// It consists of a condition and an associated value.
/// If the condition is matched, the conditional expression will take on the
/// associated value.
class WhenAlt : public Alt
{
public:
    /// @brief Constructor.
    WhenAlt();

    /// @brief Destructor.
    virtual ~WhenAlt();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the condition of the alternative.
    /// @return The condition of the alternative.
    Value *getCondition() const;

    /// @brief Sets the condition of the alternative.
    /// @param v The condition of the alternative to be set.
    /// @return The old condition of the alternative.
    Value *setCondition(Value *v);

    /// @brief Returns the value associated to the alternative.
    /// @return The value associated to the alternative.
    Value *getValue() const;

    /// @brief Sets the value associated to the alternative.
    /// @param v The value to be associated to the alternative.
    /// @return The value previously associated to the alternative.
    Value *setValue(Value *v);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    /// @brief The condition of the alternative.
    Value *_condition;

    /// @brief The value associated to the alternative.
    Value *_value;

    // K: disabled
    WhenAlt(const WhenAlt &);
    WhenAlt &operator=(const WhenAlt &);
};

} // namespace hif
