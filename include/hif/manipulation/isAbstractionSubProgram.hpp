/// @file isAbstractionSubProgram.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/NameTable.hpp"
#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Check if the given name is amethod added during abstraction
/// @param name The given subprogram name
/// @return true when name matches

bool isAbstractionSubProgram(const std::string &name);

} // namespace manipulation
} // namespace hif
