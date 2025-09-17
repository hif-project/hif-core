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
    IFeature();
    IFeature(const IFeature &other);
    IFeature &operator=(const IFeature &other);
};

} // namespace features
} // namespace hif
