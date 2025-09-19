/// @file DataDeclaration.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Declaration.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

///	@brief Abstract class for data declarations.
///
/// @details
/// This class is an abstract class for data declarations.
/// A data declaration consists of a name, a type, an initial value,
/// and optionally a range constraint.
class DataDeclaration : public Declaration
{
public:
    /// @brief Constructor.
    DataDeclaration();

    /// @brief Destructor.
    virtual ~DataDeclaration();

    /// @brief Returns the type of the data declaration.
    /// @return The type of the data declaration.
    Type *getType() const;

    /// @brief Sets the type of the data declaration.
    /// @param t The type of the data declaration to be set.
    /// @return The old type of the data declaration if it is different
    /// from the new one, nullptr otherwise.
    Type *setType(Type *t);

    /// @brief Returns the initial value of the declaration.
    /// @return The initial value of the declaration.
    Value *getValue() const;

    /// @brief Sets the initial value of the data declaration.
    /// @param v The initial value of the data declaration to be set.
    /// @return The old initial value of the data declaration if it is
    /// different from the new one, nullptr otherwise.
    Value *setValue(Value *v);

    /// @brief Returns the range of the data declaration.
    /// @return The range of the data declaration.
    Range *getRange() const;

    /// @brief Sets the range constraint of the data declaration.
    /// @param r The range constraint of the data declaration to be set.
    /// @return The old range constraint of the data declaration if it is
    /// different from the new one, nullptr otherwise.
    Range *setRange(Range *r);

protected:
    /// @brief The type of the data declaration.
    Type *_type;

    /// @brief The initial value of the data declaration.
    Value *_value;

    /// @brief The range constraint of the data declaration.
    Range *_range;

    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    /// @param child The child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    // K: disabled.
    DataDeclaration(const DataDeclaration &);
    DataDeclaration &operator=(const DataDeclaration &);
};

} // namespace hif
