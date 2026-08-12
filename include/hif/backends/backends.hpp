/// @file backends.hpp
/// @brief Main include for HIF backend utilities and interfaces.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

namespace hif
{
/// @brief Wraps utilities to write HIF backends.
namespace backends
{
} // namespace backends
} // namespace hif

#include "hif/backends/CHifDirStruct.hpp"
#include "hif/backends/IndentedStream.hpp"
#include "hif/backends/NodeVisitor.hpp"
#include "hif/backends/Properties.hpp"
#include "hif/backends/Session.hpp"
#include "hif/backends/backendManipulations.hpp"
