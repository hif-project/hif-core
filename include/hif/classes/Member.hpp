/// @file Member.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/PrefixedReference.hpp"

namespace hif
{

///	@brief Single element selection in an array.
///
/// @details
/// This class represents the access to a single selected element in an array
/// or a vector.
/// It consists of a prefix (i.e., the array or vector on which selection is
/// performed) and a list of indices (to handle multidimensional arrays).
class Member : public PrefixedReference
{
public:
    /// @brief Constructor.
    Member();

    /// @brief Destructor.
    virtual ~Member();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

    /// @brief Return the index of the member.
    /// @return The index of the member.
    Value *getIndex() const;

    /// @brief Sets the index of the member.
    /// @param v The index of the member to be set.
    /// @return The old index of the member, or nullptr if none.
    Value *setIndex(Value *v);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    /// @brief The index of the member.
    Value *_index;

    // K: disabled
    Member(const Member &);
    Member &operator=(const Member &);
};

} // namespace hif
