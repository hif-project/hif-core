/// @file hifVersion.hpp
/// @brief Defines version macros for the HIF library.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

/// Major version HIF.
#define HIF_MAJOR_VERSION 1
/// Minor version HIF.
#define HIF_MINOR_VERSION 0
/// Micro version HIF.
#define HIF_MICRO_VERSION 0
/// Helper to transform the given argument into a string.
#define HIF_STR_HELPER(x) #x
/// Helper to transform the given argument into a string.
#define HIF_STR(x)        HIF_STR_HELPER(x)
/// Complete version HIF.
#define HIF_VERSION       HIF_STR(HIF_MAJOR_VERSION) "." HIF_STR(HIF_MINOR_VERSION) "." HIF_STR(HIF_MICRO_VERSION)
