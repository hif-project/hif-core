/// @file String.hpp
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

/// @brief String type.
///
/// @details
/// This class represents the string type.
class String : public SimpleType
{
public:
    /// @brief Constructor.
    String();

    /// @brief Destructor.
    virtual ~String();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Sets the span information of the string type.
    /// @param r The span of the string type to be set.
    /// @return The old span of the string type if it is different
    /// from the new one, nullptr otherwise.
    Range *setSpanInformation(Range *r);

    /// @brief Returns the span information of the type.
    /// @return The span of the type.
    Range *getSpanInformation() const;

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
    /// @param child The child object to get the name for.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    /// @brief The span information.
    Range *_spanInformation;

    // K: disabled
    String(const String &);
    String &operator=(const String &);
};

} // namespace hif
