/// @file dumpVersion.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <iostream>

#include "hif/application_utils/dumpVersion.hpp"
#include "hif/hifVersion.hpp"

namespace hif
{
namespace application_utils
{

void dumpVersion(std::string const &tool_name)
{
    std::string toolName(tool_name);
#ifndef NDEBUG
    toolName += " [DEBUG]";
#endif
    std::cout << toolName << " -- version " << HIF_VERSION << "\n";
#ifndef NDEBUG
    std::string date = hif_getCurrentDateAsString();
    if (date.substr(5, 2) == "12") {
        std::cout << "    *             ,                          \n"
                  << "                _/^\\_                        \n"
                  << "               <     >                       \n"
                  << "*               /.-.\\           *            \n"
                  << "       *        `/&\\`                   *    \n"
                  << "               ,@.*;@,                       \n"
                  << "              /_o.I %_\\    *                 \n"
                  << " *           (`'--:o(_@;                     \n"
                  << "            /`;--.,__ `')             *      \n"
                  << "           ;@`o % O,*`'`&\\                   \n"
                  << "     *    (`'--)_@ ;o %'()\\      *           \n"
                  << "          /`;--._`''--._O'@;                 \n"
                  << "         /&*,()~o`;-.,_ `\"\"`)                \n"
                  << "*          /`,@ ;+& () o*`;-';\\              \n"
                  << "        (`\"\"--.,_0 +% @' &()\\                \n"
                  << "        /-.,_    ``''--....-'`)  *           \n"
                  << "   *    /@%;o`:;'--,.__   __.'\\              \n"
                  << "       ;*,&(); @ % &^;~`\"`o;@();         *   \n"
                  << "       /(); o^~; & ().o@*&`;&%O\\             \n"
                  << "       `\"=\"==\"\"==,,,.,=\"==\"===\"`             \n"
                  << "    __.----.(\\-''#####---...___...-----._    \n"
                  << "  '`         \\)_`\"\"\"\"\"`                      \n"
                  << "          .--' ')                            \n"
                  << "        o(  )_-\\                             \n"
                  << "          `\"\"\"` `                            \n"
                  << "\n\n";
    }
#endif
}

auto getHIFVersion() -> std::string { return HIF_VERSION; }

} // namespace application_utils
} // namespace hif
