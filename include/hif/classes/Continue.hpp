/// @file Continue.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Action.hpp"
#include "hif/features/INamedObject.hpp"

namespace hif
{

/// @brief Continue statement.
/// @details
/// This class represents a continue statement (i.e., move to the next
/// iteration in a loop).
/// The name of the enclosing loop can be specified.
class Continue : public Action, public features::INamedObject
{
public:
    /// @brief Constructor.
    /// The default loop name is <tt>NAME_NONE</tt>.
    Continue();

    /// @brief Destructor.
    virtual ~Continue();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

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

private:
    // K: disabled
    Continue(const Continue &);
    Continue &operator=(const Continue &);
};

} // namespace hif
