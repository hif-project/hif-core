/// @file Generate.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/BaseContents.hpp"

namespace hif
{

/// @brief Base class for generators.
///
/// @details
/// This class is the base class for conditional generators (IfGenerate) and
/// iterative generators (ForGenerate).
///
/// @see IfGenerate, ForGenerate
class Generate : public BaseContents
{
public:
    /// @brief Constructor.
    Generate();

    /// @brief Destructor.
    virtual ~Generate();

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();
};

} // namespace hif
