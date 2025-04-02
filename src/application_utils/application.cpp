/// @file application.cpp
/// @brief Implements the Application class for managing HIF applications.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cassert>
#include <csignal>
#include <cstdlib>
#include <iostream>

#include "hif/application_utils/application.hpp"

static inline void segfaultHandler(int /*signal*/)
{
    std::cerr << "-- FATAL_ERROR (SF): detected unexpected statement inside the design.\n"
              << "   Please contact the HIF support team.\n\n";
    std::exit(EXIT_FAILURE);
}

static inline void fpeHandler(int /*signal*/)
{
    std::cerr << "-- FATAL_ERROR (FP): detected unexpected expression inside the design.\n"
              << "   Please contact the HIF support team.\n\n";
    std::exit(EXIT_FAILURE);
}

static inline void newHandler()
{
    std::cerr << "-- FATAL_ERROR (NH): detected unexpected data inside the design.\n"
              << "   Please contact the HIF support team.\n\n";
    std::exit(EXIT_FAILURE);
}

static inline void registerSignalHandlers()
{
    // Set SIGSEGV handler and check for errors.
    if (signal(SIGSEGV, &segfaultHandler) == SIG_ERR) {
        std::cerr << "Failed to set SIGSEGV handler!\n";
        std::exit(EXIT_FAILURE);
    }

    // Set SIGFPE handler and check for errors.
    if (signal(SIGFPE, &fpeHandler) == SIG_ERR) {
        std::cerr << "Failed to set SIGFPE handler!\n";
        std::exit(EXIT_FAILURE);
    }

    // Set the new handler for memory allocation failures
    std::set_new_handler(newHandler);
}

namespace hif
{
namespace application_utils
{

Application::Application()
    : _name()
{
    registerSignalHandlers();
}

auto Application::getInstance() -> Application &
{
    static Application _app;
    return _app;
}

void Application::setName(const std::string &name) { _name = name; }

auto Application::getName() const -> std::string { return _name; }

} // namespace application_utils
} // namespace hif
