/// @file univerCM.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/univerCM/univerCM.hpp"

namespace hif
{
namespace univerCM
{
LibraryDef *getUniverCMPackage()
{
    LibraryDef *ret = new LibraryDef();
    ret->setName("univerCM");

    // @TODO Add here the implementation
    // REMINDER: add univerCM dot() method to remove dot operator.

    return ret;
}

} // namespace univerCM
} // namespace hif
