/// @file dumpVersion.hpp
/// @brief Utilities for printing and retrieving HIF version information.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <string>

#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace application_utils
{

/// @brief Returns the current HIF tag.
/// @return The current HIF tag.
auto getHIFVersion() -> std::string;

/// @brief prints a banner with current HIF infos.
/// @param tool_name The name of the tool.
void dumpVersion(std::string const &tool_name);

} // namespace application_utils
} // namespace hif
