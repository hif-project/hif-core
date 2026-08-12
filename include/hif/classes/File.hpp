/// @file File.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/CompositeType.hpp"

namespace hif
{

///	@brief File type.
///
/// @details
/// This class represents a file type.
class File : public CompositeType
{
public:
    /// @brief Constructor.
    File();

    /// @brief Destructor.
    virtual ~File();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

private:
    // K: disabled.
    File(const File &);
    File &operator=(const File &);
};

} // namespace hif
