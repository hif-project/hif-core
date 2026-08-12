/// @file Scope.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <list>

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Declaration.hpp"

namespace hif
{

/// @brief Abstract class for scopes.
///
/// @details
/// This class is an abstract class for scopes.
class Scope : public Declaration
{
public:
    /// @brief Constructor.
    Scope();

    /// @brief Destructor.
    virtual ~Scope();

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    // K; disabled
    Scope(const Scope &);
    Scope &operator=(const Scope &);
};

} // namespace hif
