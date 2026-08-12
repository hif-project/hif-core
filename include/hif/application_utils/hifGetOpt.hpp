/// @file hifGetOpt.hpp
/// @brief Provides functionality for parsing command-line options.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"

#include <getopt.h>

namespace hif
{
namespace application_utils
{

/// @brief Enumeration for argument requirements in command-line options.
enum GetOptValues : unsigned char {
    hif_no_argument       = 0, ///< Option does not take an argument.
    hif_required_argument = 1, ///< Option requires an argument.
    hif_optional_argument = 2  ///< Option can optionally take an argument.
};

/// @brief Parses short-form command-line options.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param optstring String defining valid options.
/// @return The option character, or `-1` when no more options are found.
auto hif_getopt(int argc, char *const argv[], const char *optstring) -> int;

/// @brief Parses long-form command-line options.
/// @param argc Argument count.
/// @param argv Argument vector.
/// @param optstring String defining valid options.
/// @param longopts Array of `option` structures for long options.
/// @param longindex Pointer to store index of the matched long option.
/// @return The option character, or `-1` when no more options are found.
auto hif_getopt_long(int argc, char **argv, const char *optstring, const struct option *longopts, int *longindex)
    -> int;

/// @brief Global variables used by `hif_getopt` and `hif_getopt_long`.
extern int hif_opterr;   ///< Whether to print error messages.
extern int hif_optind;   ///< Index into the parent `argv` vector.
extern int hif_optopt;   ///< Option character checked for validity.
extern int hif_optreset; ///< Flag to reset option parsing state.
extern char *hif_optarg; ///< Argument associated with an option.

} // namespace application_utils
} // namespace hif
