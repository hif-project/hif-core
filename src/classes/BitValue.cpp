/// @file BitValue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/BitValue.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/application_utils/Log.hpp"

namespace hif
{

BitValue::BitValue()
    : _value(bit_u)
{
    // ntd
}

BitValue::BitValue(const BitConstant b)
    : _value(b)
{
    // ntd
}

BitValue::~BitValue()
{
    // ntd
}

ClassId BitValue::getClassId() const { return CLASSID_BITVALUE; }

int BitValue::acceptVisitor(HifVisitor &vis) { return vis.visitBitValue(*this); }

void BitValue::_calculateFields() { ConstValue::_calculateFields(); }

BitConstant BitValue::getValue() const { return _value; }

void BitValue::setValue(const BitConstant x) { _value = x; }

void BitValue::setValue(const char x)
{
    switch (x) {
    case 'x':
    case 'X':
        _value = bit_x;
        break;
    case 'z':
    case 'Z':
        _value = bit_z;
        break;
    case '0':
        _value = bit_zero;
        break;
    case '1':
        _value = bit_one;
        break;
    case 'l':
    case 'L':
        _value = bit_l;
        break;
    case 'h':
    case 'H':
        _value = bit_h;
        break;
    case 'w':
    case 'W':
        _value = bit_w;
        break;
    case '-':
        _value = bit_dontcare;
        break;
    case 'u':
    case 'U':
        _value = bit_u;
        break;
    default:
        break;
    }
}

std::string BitValue::toString() const { return bitConstantToString(_value); }

bool BitValue::is01() const
{
    if (_value != bit_zero && _value != bit_one)
        return false;
    return true;
}

} // namespace hif
