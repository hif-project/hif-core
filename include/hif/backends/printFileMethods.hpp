/// @file printFileMethods.hpp
/// @brief File-related methods.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <fstream>
#include <string>

#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace backends
{

/// @name File-related methods.
/// @{

/// @brief Splits a file name into its base name and extension.
/// @param f The full file name to be split.
/// @param base Output parameter for the base name (without extension).
/// @param ext Output parameter for the file extension (including the dot).

void splitFileName(const std::string &f, std::string &base, std::string &ext);

/// @brief Opens a file and associates it with an output stream.
/// @param name The name of the file to open.
/// @param stream Pointer to the output file stream to associate with the file.
/// @return 0 on success, or an error code if the file cannot be opened.

int openFileStream(const std::string &name, std::ofstream *stream);

/// @brief Closes an open output file stream.
/// @param stream Pointer to the output file stream to be closed.

void closeFileStream(std::ofstream *stream);

/// @}

} // namespace backends
} // namespace hif
