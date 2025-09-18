/// @file RecordValueAlt.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Alt.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/INamedObject.hpp"

namespace hif
{

/// @brief Alternative for a RecordValue.
///
/// @details
/// This class represents an alternative for a RecordValue.
/// It consists of the name of the field to which it refers to
/// and the corresponding value.
///
/// @see RecordValue
class RecordValueAlt : public Alt, public features::INamedObject
{
public:
    /// @brief Constructor.
    RecordValueAlt();

    /// @brief Destructor.
    virtual ~RecordValueAlt();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the value of the alternative.
    /// @return The value of the alternative.
    Value *getValue() const;

    /// @brief Sets the value of the alternative.
    /// @param v The value of the alternative to be set.
    /// @return The old value of the alternative.
    Value *setValue(Value *v);

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
    /// @brief The value of the alternative.
    Value *_value;

    // K: disabled.
    RecordValueAlt(const RecordValueAlt &);
    RecordValueAlt &operator=(const RecordValueAlt &);
};

} // namespace hif
