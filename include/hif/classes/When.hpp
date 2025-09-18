/// @file When.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Value.hpp"
#include "hif/classes/forwards.hpp"

namespace hif
{

/// @brief Conditional expression.
///
/// @details
/// This class represents a conditional expression (e.g., the
/// <tt>when</tt> construct in VHDL, or the ternary operator in C/C++).
/// The value to which a conditional expression evaluates depends on a list
/// of alternatives (WhenAlt), each one containing a condition and an
/// associated value.
/// When one of the condition is matched, the expression takes on the
/// corresponding value. If no conditions are matched, and a default value
/// is provided, the expression takes on the default value.
///
/// @see WhenAlt
class When : public Value
{
public:
    /// @brief The alt type.
    typedef WhenAlt AltType;

    /// The list of alternatives.
    BList<WhenAlt> alts;

    /// @brief Constructor.
    When();

    /// @brief Destructor.
    virtual ~When();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the default value of the conditional expression.
    /// @return The default value of the conditional expression.
    /// If a default value do not exist 0 is returned.
    Value *getDefault() const;

    /// @brief Sets the default value of the conditional expression.
    /// @param v The default value to be set.
    /// @return The previous default value of the conditional expression.
    Value *setDefault(Value *v);

    /// @brief Returns logic ternary property.
    /// @return logic ternary property.
    bool isLogicTernary() const;

    /// @brief Sets logic ternary property.
    /// @param logicTernary The value to set.
    void setLogicTernary(const bool logicTernary);

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
    /// @brief The default value of the conditional expression.
    Value *_defaultvalue;

    /// @brief Identify whether the When condition must be treated as logic condition
    /// as in Verilog semantics.
    bool _logicTernary;

    // K: disabled
    When(const When &);
    When &operator=(const When &);
};

} // namespace hif
