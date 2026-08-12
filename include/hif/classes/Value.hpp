/// @file Value.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/TypedObject.hpp"

namespace hif
{

/// @brief Abstract class for values.
///
/// @details
/// This class is an abstract class for values.
class Value : public TypedObject
{
public:
    /// @brief Constructor.
    Value();

    /// @brief Destructor.
    virtual ~Value();

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    // K: disabled.
    Value(const Value &);
    Value &operator=(const Value &);
};

} // namespace hif
