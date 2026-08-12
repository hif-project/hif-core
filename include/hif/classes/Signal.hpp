/// @file Signal.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/DataDeclaration.hpp"

namespace hif
{

/// @brief Signal declaration.
///
/// @details
/// This class represents the declaration of a RTL signal.
/// It consists of the name of the signal, its type and its initial value.
class Signal : public DataDeclaration
{
public:
    /// @brief Constructor.
    Signal();

    /// @brief Destructor.
    virtual ~Signal();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns whether this is a standard declaration.
    /// @return <tt>true</tt> if this is a standard declaration, <tt>false</tt> otherwise.
    bool isStandard() const;

    /// @brief Sets whether this is a standard declaration.
    /// @param standard <tt>true</tt> if this is a standard declaration, <tt>false</tt> otherwise.
    void setStandard(bool standard);

    /// @brief Returns whether this is a wrapper for an object with
    /// similar features, which does not have a matching data model.
    /// @return <tt>true</tt> if this is a wrapper, <tt>false</tt> otherwise.
    bool isWrapper() const;

    /// @brief Sets whether this is actually a wrapper for an object with
    /// similar features, which does not have a matching data model.
    /// @param wrapper <tt>true</tt> if this is a wrapper, <tt>false</tt> otherwise.
    void setWrapper(bool wrapper);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    /// @brief Distinguishes between a normal declaration (i.e., part of design)
    /// and a standard one (i.e., part of the language).
    bool _isStandard;

    /// @brief Tells whether this is actually a wrapper for an object with
    /// similar features, which does not have a matching data model (e.g.,
    /// Verilog-AMS natures or disciplines).
    bool _isWrapper;

    // K: disabled
    Signal(const Signal &);
    Signal &operator=(const Signal &);
};

} // namespace hif
