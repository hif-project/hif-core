/// @file INamedObject.hpp
/// @brief Interface for objects with a name property in HIF.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/NameTable.hpp"
#include "hif/application_utils/portability.hpp"
#include "hif/features/IFeature.hpp"

namespace hif
{
namespace features
{

/// @brief Objects with this feature have a name.
class INamedObject : public IFeature
{
public:
    /// @name Accessors.
    /// @{

    /// @brief Sets the name.
    ///
    /// @param n the name to be set.
    ///
    void setName(const std::string &name);

    /// @brief Gets the name.
    ///
    /// @return The name.
    ///
    std::string getName() const;

    /// @brief Checks whether given name is equals to the object name.
    /// @param nameToMatch The name to check.
    /// @return <tt>True</tt> if names are equals.
    bool matchName(const std::string &nameToMatch) const;

    /// @}

protected:
    INamedObject();
    virtual ~INamedObject() = default;
    INamedObject(const INamedObject &other);
    INamedObject &operator=(const INamedObject &other);

private:
    /// @brief The stored name.
    std::string _name;
};

} // namespace features
} // namespace hif
