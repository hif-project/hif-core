/// @file TPAssign.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/ReferencedAssign.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Template parameter assignment.
///
/// @details
/// This class represents a template parameter assignment (i.e., a template
/// argument/actual parameter).
///
/// @see ReferencedAssign, TypeTP, ValueTP
class TPAssign : public ReferencedAssign
{
public:
    /// @brief Constructor.
    TPAssign();

    /// @brief Destructor.
    virtual ~TPAssign();

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    TPAssign(const TPAssign &);
    TPAssign &operator=(const TPAssign &);
};

} // namespace hif
