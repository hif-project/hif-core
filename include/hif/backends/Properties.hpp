/// @file Properties.hpp
/// @brief Property management utilities for HIF backend objects.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <iostream>
#include <map>
#include <string>

#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace backends
{

/// @brief Parameters structure description (key = value).
class Properties
{
public:
    /// @brief Load a configuration file
    /// @param config_file_name Configuration file name
    /// @param silent if no file corresponds => no error message
    /// @param evaluate elaborate values (which depend on others keys)
    void load(const std::string &config_file_name, bool silent = false, bool evaluate = true);

    /// @brief Print on the ostream the properties list.
    /// @param out ostream where to print.
    void dump(std::ostream &out) const;

    /// @brief Get a property value
    /// @param property_name key to identify
    /// @return the corresponding value as a string
    std::string getProperty(const std::string &property_name) const;

    /// @brief Get a property value
    /// @param property_name key to identify
    /// @return the corresponding value as a string
    std::string operator[](const std::string &property_name) const;

    /// @brief Set a property value
    /// @param property_name key to find/set
    /// @param property value to set
    /// @param evaluate elaborate the value.
    void setProperty(const std::string &property_name, const std::string &property, bool evaluate = true);

    /// @brief Append a property
    /// @param property_name key to append
    /// @param property value to set
    /// @param evaluate elaborate the value.
    void appendProperty(const std::string &property_name, const std::string &property, bool evaluate = true);

    /// @brief Elaborate a value and return this evaluation
    std::string eval(const std::string &) const;

private:
    /// @brief Configuration file name.
    std::string name;
    /// @brief Properties map.
    std::map<std::string, std::string, std::less<std::string>> properties;
};

} // namespace backends
} // namespace hif
