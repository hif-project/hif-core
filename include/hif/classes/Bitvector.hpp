/// @file Bitvector.hpp
/// @brief Bitvector type for representing arrays of bits in HIF.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/SimpleType.hpp"
#include "hif/classes/forwards.hpp"
#include "hif/features/ITypeSpan.hpp"

namespace hif
{

///	@brief Bit vector type.
/// @details
/// This class represents a bit vector type. It includes the span of the vector.
/// The <tt>logic</tt> attribute is used to distinguish between logic vector
/// type (4-value logic) and bit vector type (2-value logic).
/// The <tt>resolved</tt> attribute is used to distinguish between resolved and
/// unresolved bit vector types.
/// The <tt>signed</tt> attribute indicates whether the bit vector has to be
/// interpreted with signed or unsigned arithmetic.
class Bitvector : public SimpleType, public hif::features::ITypeSpan
{
public:
#if (defined _MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#if (defined _MSC_VER)
#pragma warning(pop)
#endif

    /// @brief Constructor.
    Bitvector();

    /// @brief Destructor.
    virtual ~Bitvector();

    Bitvector(const Bitvector &)                     = delete;
    auto operator=(const Bitvector &) -> Bitvector & = delete;

    /// @brief Returns a string representing the class name.
    /// @return The string representing the class name.
    ClassId getClassId() const;

    /// @brief Returns the <tt>logic</tt> attribute.
    /// @return The <tt>logic</tt> attribute.
    bool isLogic() const;

    /// @brief Sets the <tt>logic</tt> attribute.
    /// @param logic The <tt>logic</tt> attribute to be set.
    void setLogic(const bool logic);

    /// @brief Sets the <tt>resolved</tt> attribute.
    /// @return The <tt>resolved</tt> attribute.
    bool isResolved() const;

    /// @brief Set the <tt>resolved</tt> attribute.
    /// @param resolved The <tt>resolved</tt> attribute to be set.
    void setResolved(const bool resolved);

    /// @brief Sets the span of the bit vector.
    /// @param t The new span of the bit vector.
    /// @return The old span of the bit vector if it is different
    /// from the new one, nullptr otherwise.
    Range *setSpan(Range *t);

    /// @brief Sets the <tt>signed</tt> attribute.
    /// @param sign The <tt>signed</tt> attribute.
    void setSigned(const bool sign);

    /// @brief Returns the <tt>signed</tt> attribute.
    /// @return The <tt>signed</tt> attribute.
    bool isSigned() const;

    /// @brief Accepts a visitor to visit the current object.
    /// @param vis The visitor.
    /// @return Integer representing the result of the visit. Default value is 0.
    virtual int acceptVisitor(HifVisitor &vis);

    /// @brief Returns this object as hif::Object.
    /// @return This object as hif::Object.
    virtual Object *toObject();

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Returns the name of the given child relative to this object.
    /// @param child Pointer to the child object.
    /// @return The name of the child.
    virtual std::string _getFieldName(const Object *child) const;

private:
    /// @brief Flag to store the <tt>logic</tt> attribute.
    bool _isLogic;
    /// @brief Flag to store the <tt>resolved</tt> attribute.
    bool _isResolved;
    /// @brief Flag to store the <tt>signed</tt> attribute.
    bool _isSigned;
};

} // namespace hif
