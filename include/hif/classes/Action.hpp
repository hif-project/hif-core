/// @file Action.hpp
/// @brief Abstract class definition for action statements.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Object.hpp"

namespace hif
{

///	@brief Abstract class for action statements.
/// @details
/// This class is an abstract class for action statements, which typically
/// describe code in processes or subroutines.
class Action : public Object
{
public:
    /// @brief Constructor.
    Action();

    /// @brief Destructor.
    virtual ~Action();

    Action(const Action &)                     = delete;
    auto operator=(const Action &) -> Action & = delete;

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();
};

} // namespace hif
