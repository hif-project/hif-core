/// @file Range.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/IntValue.hpp"
#include "hif/classes/Value.hpp"
#include "hif/hifEnums.hpp"

namespace hif
{

/// @brief Range (i.e., a directed discrete interval).
///
/// @details
/// This class represents a range, which is a directed discrete interval.
class Range : public Value
{
public:
    /// @brief Default constructor.
    Range();

    /// @brief Default constructor.
    Range(Value *lbound, Value *rbound, const RangeDirection dir);

    /// @brief Constructor.
    /// @param lbound The left integer bound of the range.
    /// @param rbound The right integer bound of the range.
    template <
        typename T1,
        typename T2,
        typename std::enable_if<std::is_integral<T1>::value && !std::is_same<T1, bool>::value, int>::type = 0,
        typename std::enable_if<std::is_integral<T2>::value && !std::is_same<T2, bool>::value, int>::type = 0>
    Range(T1 lbound, T2 rbound)
        : Range(
              new IntValue(lbound),
              new IntValue(rbound),
              static_cast<std::int64_t>(lbound) >= static_cast<std::int64_t>(rbound) ? dir_downto : dir_upto)
    {
        // Nothing to do here.
    }

    /// @brief Destructor.
    virtual ~Range();

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the direction of the range.
    /// @return The direction of the range.
    RangeDirection getDirection() const;

    /// @brief Sets the direction of the range.
    /// @param d The direction of the range to be set.
    void setDirection(const RangeDirection d);

    /// @brief Returns the left bound of the range.
    /// @return The left bound of the range.
    Value *getLeftBound() const;

    /// @brief Sets the left bound of the range.
    /// @param b The left bound of the range to be set.
    /// @return The old left bound of the range, or nullptr if none.
    Value *setLeftBound(Value *b);

    /// @brief Returns the right bound of the range.
    /// @return The right bound of the range.
    Value *getRightBound() const;

    /// @brief Sets the right bound of the range.
    /// @param b The right bound of the range to be set.
    /// @return The old right bound of the range, or nullptr if none.
    Value *setRightBound(Value *b);

    /// @brief Returns the type of the range.
    /// @return The type of the range.
    Type *getType() const;

    /// @brief Sets the type of the range.
    /// @param t The type of the range to be set.
    /// @return The old type of the range, or nullptr if none.
    Type *setType(Type *t);

    /// @brief Returns the semantic type of the range.
    /// @warning Should not be invoked.
    /// @return The semantic type of the range.
    virtual Type *getSemanticType() const;

    /// @brief Sets the semantic type of the range.
    /// @warning Raises an error when invoked.
    /// @param t The semantic type of the range to be set.
    /// @return The old semantic type of the range, or nullptr if none.
    virtual Type *setSemanticType(Type *t);

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

    /// @brief Swap the bounds and the direction.
    void swapBounds();

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of given child w.r.t. this.
    virtual std::string _getFieldName(const Object *child) const;

private:
    /// @brief The direction of the range.
    RangeDirection _direction;

    /// @brief The left bound of the range.
    Value *_leftBound;

    /// @brief The right bound of the range.
    Value *_rightBound;

    /// @brief The type of range (used for typed ranges).
    Type *_type;

    // K: disabled
    Range(const Range &);
    Range &operator=(const Range &);
};

} // namespace hif
