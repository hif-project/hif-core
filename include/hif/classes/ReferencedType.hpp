/// @file ReferencedType.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Type.hpp"
#include "hif/features/INamedObject.hpp"

namespace hif
{

/// @brief Referenced type.
///
/// @details
/// This class represents a referenced type (i.e., a type which actually
/// is a reference to another entity).
///
/// @see Library, TypeReference, ViewReference
class ReferencedType : public Type, public features::INamedObject
{
public:
    /// @brief Constructor.
    ReferencedType();

    /// @brief Destructor.
    virtual ~ReferencedType();

    /// @brief Sets the reference to the definition of the type here referred.
    /// @param type The reference to the definition of the type here referred.
    /// @return The old reference to the definition of the type here referred.
    ReferencedType *setInstance(ReferencedType *type);

    /// @brief Returns the reference to the definition of the type here referred.
    /// @return The reference to the definition of the type here referred.
    ReferencedType *getInstance() const;

protected:
    /// @brief The reference to the definition of the type here referred.
    ReferencedType *_instance;

    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    virtual std::string _getFieldName(const Object *child) const;

private:
    // K: disabled
    ReferencedType(const ReferencedType &);
    ReferencedType &operator=(const ReferencedType &);
};

} // namespace hif
