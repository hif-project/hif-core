/// @file IfGenerate.hpp
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

///	@brief If generate construct (conditional generator).
///
/// @details
/// This class represents the VHDL <tt>if generate</tt> construct.
/// It allows to conditionally generate a number of design elements only if
/// a given static condition is met.
///
/// Note: it refers to IEEE Std. 1076-2003.
///
/// @see Generate
class IfGenerate : public Generate
{
public:
    /// @brief Constructor.
    /// The generating condition is set to nullptr.
    IfGenerate();

    /// @brief Destructor.
    virtual ~IfGenerate();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the generating condition.
    /// @return The generating condition.
    Value *getCondition() const;

    /// @brief Sets the generating condition.
    /// @param c The generating condition to be set.
    /// @return The old generating condition if it is different
    /// from the new one, nullptr otherwise.
    Value *setCondition(Value *c);

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
    /// @brief The generating condition.
    Value *_condition;

    // K: disabled
    IfGenerate(const IfGenerate &);
    IfGenerate &operator=(const IfGenerate &);
};

} // namespace hif
