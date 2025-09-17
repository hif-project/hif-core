/// @file Alt.hpp
/// @brief Alternative structure for representing conditional branches.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/BList.hpp"
#include "hif/classes/Object.hpp"

namespace hif
{

/// @brief Abstract class for alternative objects.
/// @details
/// This class is an abstract class for alternative objects, which are used by
/// classes that feature selection among a variety of cases (e.g., Aggregate,
/// If, Switch, When, With).
class Alt : public Object
{
public:
    /// @brief Constructor.
    Alt();

    /// @brief Destructor.
    virtual ~Alt();

    Alt(const Alt &)                     = delete;
    auto operator=(const Alt &) -> Alt & = delete;

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();
};

} // namespace hif
