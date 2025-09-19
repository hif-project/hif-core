/// @file Type.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/application_utils/portability.hpp"
#include "hif/classes/Object.hpp"

namespace hif
{

/// @brief Abstract class for types.
///
/// @details
/// This class is an abstract class for types.
class Type : public Object
{
public:
    /// @brief The possibile variants of types in all semantics.
    enum TypeVariant : unsigned char {
        NATIVE_TYPE,                /// <@brief Default: no variant is in use.
        VHDL_BITVECTOR_NUMERIC_STD, /// <@brief Signed type defined in std_logic_signed
        SYSTEMC_INT_BITFIELD,       /// <@brief c++ bitfield
        SYSTEMC_INT_SC_INT,         /// <@brief sc_int, sc_uint
        SYSTEMC_INT_SC_BIGINT,      /// <@brief sc_bigint, sc_bigunit
        SYSTEMC_BITVECTOR_PROXY,    /// <@brief sc_proxy_lv, sc_proxy_bv
        SYSTEMC_BITVECTOR_BASE,     /// <@brief sc_lv_base, sc_bv_base
        SYSTEMC_BIT_BITREF          /// <@brief sc_bitref_l, sc_bitref_b
    };

    /// @brief Constructor.
    Type();

    /// @brief Destructor.
    virtual ~Type();

    /// @brief Returns the <tt>typeVariant</tt> attribute.
    /// @return The <tt>typeVariant</tt> attribute.
    ///
    TypeVariant getTypeVariant() const;

    /// @brief Sets the <tt>typeVariant</tt> attribute.
    /// @param tv The <tt>typeVariant</tt> attribute to be set.
    void setTypeVariant(const TypeVariant tv);

    /// @brief Return string representation of given type variant.
    /// @param t The type variant.
    /// @return The string representation of given type variant.
    static std::string typeVariantToString(const TypeVariant t);

    /// @brief Return the type variant for given string.
    /// @param s The string.
    /// @return The type variant for given string.
    static TypeVariant typeVariantFromString(const std::string &s);

protected:
    /// @brief Fills the internal fields and blists lists.
    virtual void _calculateFields();

    /// @brief Stores the <tt>typeVariant</tt> attribute.
    /// Default value is NATIVE_TYPE.
    TypeVariant _typeVariant;
};

} // namespace hif
