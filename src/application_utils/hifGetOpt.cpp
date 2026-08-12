/// @file hifGetOpt.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <getopt.h>

#include "hif/application_utils/hifGetOpt.hpp"

namespace hif
{
namespace application_utils
{

int hif_opterr   = 1;       /* if error message should be printed */
int hif_optind   = 1;       /* index into parent argv vector */
int hif_optopt   = '?';     /* character checked for validity */
int hif_optreset = 0;       /* reset getopt */
char *hif_optarg = nullptr; /* argument associated with option */

auto hif_getopt(int argc, char *const argv[], const char *optstring) -> int
{
    opterr     = hif_opterr;
    optind     = hif_optind;
    optopt     = hif_optopt;
    optarg     = hif_optarg;
    int ret    = getopt(argc, argv, optstring);
    hif_opterr = opterr;
    hif_optind = optind;
    hif_optopt = optopt;
    hif_optarg = optarg;
    return ret;
}

auto hif_getopt_long(int argc, char **argv, const char *optstring, const struct option *longopts, int *longindex) -> int
{
    opterr     = hif_opterr;
    optind     = hif_optind;
    optopt     = hif_optopt;
    optarg     = hif_optarg;
    int ret    = getopt_long(argc, argv, optstring, longopts, longindex);
    hif_opterr = opterr;
    hif_optind = optind;
    hif_optopt = optopt;
    hif_optarg = optarg;
    return ret;
}

} // namespace application_utils
} // namespace hif
