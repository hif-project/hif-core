/// @file IFeature.hpp
/// @brief Interface for feature objects in the HIF framework.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Object.hpp"

namespace hif
{
namespace features
{

/// @brief Objects with this feature have a name.
class IFeature
{
public:
    /// @brief Returns this object as hif::Object.
    /// @return This object as hif::Object.
    virtual Object *toObject() = 0;

    /// @brief The destructor
    virtual ~IFeature() = 0;

protected:
    /// @brief Default constructor.
    IFeature();

    /// @brief Copy constructor.
    /// @param other The IFeature to copy.
    IFeature(const IFeature &other);

    /// @brief Assignment operator.
    /// @param other The IFeature to assign.
    /// @return Reference to this.
    IFeature &operator=(const IFeature &other);
};

} // namespace features
} // namespace hif
