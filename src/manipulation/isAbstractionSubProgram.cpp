/// @file isAbstractionSubProgram.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/isAbstractionSubProgram.hpp"

namespace hif
{
namespace manipulation
{

bool isAbstractionSubProgram(const std::string &name)
{
    if (name == "update_input_queue" || name == "update_event_queue" || name == "flag_elaboration" ||
        name == "synch_elaboration" || name == "simulate" || name == "synch_negedge_elaboration" ||
        name == "start_of_simulation" || name == "initialize" || name == "finalize")
        return true;
    return false;
}
} // namespace manipulation
} // namespace hif
