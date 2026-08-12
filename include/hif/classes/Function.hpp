/// @file Function.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/SubProgram.hpp"

namespace hif
{

///	@brief Function declaration.
///
/// @details
/// This class represents the declaration of a function.
class Function : public SubProgram
{
public:
    /// @brief Constructor.
    /// The return type of the function is set to nullptr.
    Function();

    /// @brief Destructor.
    virtual ~Function();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the return type of the function.
    /// @return The return type of the function.
    Type *getType() const;

    /// @brief Sets the return type of the function.
    /// @param t The return type of the function to be set.
    /// @return The old return type of the function if it is different from
    /// the new one, nullptr otherwise.
    Type *setType(Type *t);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    /// @brief The return type of the function.
    Type *_type;

    // K: disabled
    Function(const Function &);
    Function &operator=(const Function &);
};

} // namespace hif
