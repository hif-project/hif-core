/// @file univerCM.hpp
/// @brief Utilities and definitions for the univerCM backend in HIF.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/LibraryDef.hpp"

// ////////////////////////////////////////////
// FORWARD DECLARATIONS
// ////////////////////////////////////////////

namespace hif
{

} // namespace hif
// ////////////////////////////////////////////
// ACTUAL PACKAGE
// ////////////////////////////////////////////

namespace hif
{
/// @brief Wraps methods realted to univerCM.
namespace univerCM
{

/// @brief Returns the univerCM LibraryDef, according with HIF semantics.
/// @return The LibraryDef.

LibraryDef *getUniverCMPackage();

} // namespace univerCM
} // namespace hif
