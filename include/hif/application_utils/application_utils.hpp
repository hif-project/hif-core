/// @file application_utils.hpp
/// @brief Main include for HIF application utility functions and helpers.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

namespace hif
{
/// @brief Wraps utilities to write potable applications.
namespace application_utils
{
} // namespace application_utils
} // namespace hif

#include "hif/application_utils/CommandLineParser.hpp"
#include "hif/application_utils/ConfigurationManager.hpp"
#include "hif/application_utils/FileStructure.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/application_utils/StepFileManager.hpp"
#include "hif/application_utils/application.hpp"
#include "hif/application_utils/dumpVersion.hpp"
#include "hif/application_utils/portability.hpp"
