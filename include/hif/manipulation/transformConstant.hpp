/// @file transformConstant.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/classes.hpp"

namespace hif
{
namespace manipulation
{

/// @brief Transforms a constant value into a desired type.
///
/// @param cvo The current constant.
/// @param to The result constant value.
/// @param sem The reference semantics (default is the HIF one).
/// @param allowTruncation <tt>true</tt> if the transformation is allowed to truncate (potentially causing
///                        loss of information), <tt>false</tt> otherwise.
/// @return A fresh new value, equivalent to @p cvo, but of type @p to.
///

ConstValue *transformConstant(
    ConstValue *cvo,
    Type *to,
    hif::semantics::ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance(),
    bool allowTruncation              = true);

/// @brief Transforms a value into a desired type.
/// For example, an aggregate can be transformed into a bitvector value.
///
/// @param vo The current value.
/// @param to The result value.
/// @param sem The reference semantics (default is the HIF one).
/// @param allowTruncation <tt>true</tt> if the transformation is allowed to truncate (potentially causing
///                        loss of information), <tt>false</tt> otherwise.
/// @return A fresh new value, equivalent to @p vo, but of type @p to.
///

Value *transformValue(
    Value *vo,
    Type *to,
    hif::semantics::ILanguageSemantics *sem = hif::semantics::HIFSemantics::getInstance(),
    bool allowTruncation              = true);
} // namespace manipulation
} // namespace hif
