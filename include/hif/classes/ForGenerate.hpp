/// @file ForGenerate.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Generate.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

///	@brief For generate construct (iterative generator).
///
/// @details
/// This class represents the VHDL <tt>for generate</tt> construct.
/// It allows to iteratively generate a number of design elements.
///
/// @see Generate
class ForGenerate : public Generate
{
public:
    /// @brief List of for-loop-indices declarations.
    BList<DataDeclaration> initDeclarations;

    /// @brief List of actions performed in the initialization
    /// phase of the loop.
    BList<Action> initValues;

    /// @brief List of actions performed at the end of each iteration.
    BList<Action> stepActions;

    /// @brief Constructor.
    ForGenerate();

    /// @brief Destructor.
    virtual ~ForGenerate();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the generator condition.
    /// @return The generator condition.
    virtual Value *getCondition() const;

    /// @brief Sets the generator condition.
    /// @param cond The generator condition to be set.
    /// @return The old generator condition.
    virtual Value *setCondition(Value *cond);

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

    /// @brief Returns the name of given BList w.r.t. this.
    /// @param list The BList to get the name for.
    /// @return The name of the BList.
    virtual std::string _getBListName(const BList<Object> &list) const;

private:
    /// @brief The for generator condition.
    Value *_condition;

    // K: disabled
    ForGenerate(const ForGenerate &);
    ForGenerate &operator=(const ForGenerate &);
};

} // namespace hif
