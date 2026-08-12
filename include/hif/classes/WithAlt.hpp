/// @file WithAlt.hpp
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

/// @brief Selected signal assignment alternative.
///
/// @details
/// This class represents an alternative for a selected signal assignment (With).
/// It consists of a list of conditions (in <tt>or</tt> between them)
/// and an associated value.
///
/// @see With
class WithAlt : public Alt
{
public:
    /// @brief List of conditions (in or between them).
    BList<Value> conditions;

    /// @brief Constructor.
    WithAlt();

    /// @brief Destructor.
    virtual ~WithAlt();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

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

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief The value associated to the alternative.
    Value *_value;

    // K: disabled
    WithAlt(const WithAlt &);
    WithAlt &operator=(const WithAlt &);
};

} // namespace hif
