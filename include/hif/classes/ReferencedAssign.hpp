/// @file ReferencedAssign.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/TypedObject.hpp"
#include "hif/features/INamedObject.hpp"

namespace hif
{

/// @brief Base class for name-based assignments of value to parameters.
///
/// @details
/// This class is a base class for name-based assignments of value to
/// parameters (i.e., assignments of values to parameters referenced by name).
///
/// @see PPAssign, TPAssign
class ReferencedAssign : public TypedObject, public features::INamedObject
{
public:
    /// @brief Constructor.
    ReferencedAssign();

    /// @brief Destructor.
    virtual ~ReferencedAssign();

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    // K: disabled.
    ReferencedAssign(const ReferencedAssign &);
    ReferencedAssign &operator=(const ReferencedAssign &);
};

} // namespace hif
