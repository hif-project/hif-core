/// @file Type.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Type.hpp"
#include "hif/application_utils/Log.hpp"

namespace hif
{

Type::Type()
    : Object()
    , _typeVariant(NATIVE_TYPE)
{
    // ntd
}

Type::~Type()
{
    // ntd
}

void Type::_calculateFields() { Object::_calculateFields(); }

Type::TypeVariant Type::getTypeVariant() const { return _typeVariant; }

void Type::setTypeVariant(const TypeVariant tv) { _typeVariant = tv; }

std::string Type::typeVariantToString(const TypeVariant t)
{
    switch (t) {
    case NATIVE_TYPE:
        return "NATIVE_TYPE";
    case VHDL_BITVECTOR_NUMERIC_STD:
        return "VHDL_BITVECTOR_NUMERIC_STD";
    case SYSTEMC_INT_SC_INT:
        return "SYSTEMC_INT_SC_INT";
    case SYSTEMC_INT_SC_BIGINT:
        return "SYSTEMC_INT_SC_BIGINT";
    case SYSTEMC_INT_BITFIELD:
        return "SYSTEMC_INT_BITFIELD";
    case SYSTEMC_BITVECTOR_PROXY:
        return "SYSTEMC_BITVECTOR_PROXY";
    case SYSTEMC_BITVECTOR_BASE:
        return "SYSTEMC_BITVECTOR_BASE";
    case SYSTEMC_BIT_BITREF:
        return "SYSTEMC_BIT_BITREF";

    default:;
    }

    messageError("Unexpected type variant", nullptr, nullptr);
}

Type::TypeVariant Type::typeVariantFromString(const std::string &s)
{
    if (s == "NATIVE_TYPE")
        return NATIVE_TYPE;
    else if (s == "VHDL_BITVECTOR_NUMERIC_STD")
        return VHDL_BITVECTOR_NUMERIC_STD;
    else if (s == "SYSTEMC_INT_SC_INT")
        return SYSTEMC_INT_SC_INT;
    else if (s == "SYSTEMC_INT_SC_BIGINT")
        return SYSTEMC_INT_SC_BIGINT;
    else if (s == "SYSTEMC_INT_BITFIELD")
        return SYSTEMC_INT_BITFIELD;
    else if (s == "SYSTEMC_BITVECTOR_PROXY")
        return SYSTEMC_BITVECTOR_PROXY;
    else if (s == "SYSTEMC_BITVECTOR_BASE")
        return SYSTEMC_BITVECTOR_BASE;
    else if (s == "SYSTEMC_BIT_BITREF")
        return SYSTEMC_BIT_BITREF;

    messageError(std::string("Unexpected type variant: ") + s, nullptr, nullptr);
}

} // namespace hif
