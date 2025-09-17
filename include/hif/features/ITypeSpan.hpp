/// @file ITypeSpan.hpp
/// @brief Interface for type span objects in HIF.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <string>

#include "hif/application_utils/portability.hpp"
#include "hif/features/IFeature.hpp"

namespace hif
{
class Object;
class Range;

namespace features
{

/// @brief Non-template base class.
/// Interface for symbols, including their declaration type and other
/// common features.
class ITypeSpan : public IFeature
{
public:
    virtual ~ITypeSpan() = 0;

    /// @brief Returns the span of the type.
    /// @return The span of the type.
    Range *getSpan() const;

    /// @brief Sets the span of the type.
    /// @param r The span of the type to be set.
    /// @return The old span of the type if it is different
    /// from the new one, nullptr otherwise.
    virtual Range *setSpan(Range *r) = 0;

protected:
    ITypeSpan();

    ITypeSpan(const ITypeSpan &other);

    ITypeSpan &operator=(const ITypeSpan &other);

    /// The span of the type.
    Range *_span;
};

} // namespace features
} // namespace hif
