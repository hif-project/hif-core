/// @file hifPrinter.hpp
/// @brief Printing and formatting utilities for HIF objects.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <cstdio>
#include <fstream>
#include <iostream>

#include "hif/hifIOUtils.hpp"

namespace hif
{

/// @brief Print textual hif format of Hif tree in given output stream.
/// @param obj The root object from which start to print.
/// @param o The output stream.
/// @param opt The printing options.
///
void printHif(Object &obj, std::ostream &o, const PrintHifOptions &opt);

/// @brief Print an XML of Hif tree in given output stream.
/// @param obj The root object from which start to print.
/// @param o The output stream.
/// @param opt The printing options.
///
void printXml(Object &obj, std::ostream &o, const PrintHifOptions &opt);

} // namespace hif
