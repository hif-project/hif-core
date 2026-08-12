/// @file application.hpp
/// @brief Defines the Application class for managing HIF applications.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"

namespace hif
{

namespace application_utils
{

/// @brief Class for managing HIF applications.
/// @details Provides a singleton instance to manage application-specific settings
/// such as the application name.
class Application
{
public:
    /// @brief Retrieves the singleton instance of the Application class.
    /// @return Reference to the single Application instance.
    static auto getInstance() -> Application &;

    /// @brief Sets the name of the application.
    /// @param name The name to be assigned to the application.
    void setName(const std::string &name);

    /// @brief Retrieves the name of the application.
    /// @return The current name of the application.
    auto getName() const -> std::string;

    Application(const Application &other)                      = delete;
    Application(Application &&other)                           = delete;
    auto operator=(const Application &other) -> Application  & = delete;
    auto operator=(const Application &&other) -> Application & = delete;

private:
    /// @brief Constructs the Application object.
    /// @details Private constructor to enforce singleton pattern.
    Application();

    /// @brief Destructs the Application object.
    ~Application() = default;

    std::string _name; ///< Holds the name of the application.
};

} // namespace application_utils
} // namespace hif
