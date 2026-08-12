/// @file StringValue.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/ConstValue.hpp"

namespace hif
{

/// @brief String literal value (text object).
///
/// @details
/// This class represents a string literal value (i.e., a text object).
/// The <tt>plain</tt> attribute is used to represent something "opaque"
/// with respect to Hif.
/// Therefore, a text object could be used to embed code.
class StringValue : public ConstValue
{
public:
    /// @brief Constructor.
    StringValue();

    /// @brief Constructor.
    /// @param t The string value to be assigned. Default is nullptr.
    StringValue(const std::string &t);

    /// @brief Destructor.
    virtual ~StringValue();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the string literal value.
    /// @returns The string literal value.
    std::string getValue() const;

    /// @brief Sets the string literal value.
    /// @param t The string literal value to be set.
    void setValue(const std::string &t);

    /// @brief Sets the <tt>plain</tt> attribute.
    /// @param plain The <tt>plain</tt> attribute.
    void setPlain(bool plain);

    /// @brief Returns the <tt>plain</tt> attribute.
    /// @return The <tt>plain</tt> attribute.
    bool isPlain() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    // K: disabled
    StringValue(const StringValue &);
    StringValue &operator=(const StringValue &);

    /// @brief The actual string value.
    std::string _text;

    /// @brief The <tt>plain</tt> attribute.
    bool _isPlain;
};

} // namespace hif
