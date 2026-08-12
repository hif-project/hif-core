/// @file BitvectorValue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/BitvectorValue.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/application_utils/Log.hpp"

namespace hif
{

BitvectorValue::BitvectorValue()
    : _value()
{
    // ntd
}

BitvectorValue::BitvectorValue(const std::string &sValue)
    : _value()
{
    if (!_handleValue(sValue)) {
        messageError("Illegal value for bit value constant: \"" + sValue + "\".", nullptr, nullptr);
    }
}

bool BitvectorValue::_handleValue(const std::string &val)
{
    if (val.size() == 0)
        return false;
    for (unsigned int i = 0; i < val.size(); ++i) {
        if (val.at(i) == '0')
            _value.push_back(bit_zero);
        else if (val.at(i) == '1')
            _value.push_back(bit_one);
        else if (val.at(i) == 'u' || val.at(i) == 'U')
            _value.push_back(bit_u);
        else if (val.at(i) == 'x' || val.at(i) == 'X')
            _value.push_back(bit_x);
        else if (val.at(i) == 'z' || val.at(i) == 'Z')
            _value.push_back(bit_z);
        else if (val.at(i) == 'w' || val.at(i) == 'W')
            _value.push_back(bit_w);
        else if (val.at(i) == 'l' || val.at(i) == 'L')
            _value.push_back(bit_l);
        else if (val.at(i) == 'h' || val.at(i) == 'H')
            _value.push_back(bit_h);
        else if (val.at(i) == '-')
            _value.push_back(bit_dontcare);
        else
            return false;
    }
    return true;
}

BitvectorValue::~BitvectorValue()
{
    // ntd
}

ClassId BitvectorValue::getClassId() const { return CLASSID_BITVECTORVALUE; }

int BitvectorValue::acceptVisitor(HifVisitor &vis) { return vis.visitBitvectorValue(*this); }

void BitvectorValue::_calculateFields() { ConstValue::_calculateFields(); }

void BitvectorValue::setValue(const std::string &value)
{
    _value.clear();
    if (!_handleValue(value)) {
        messageError("Illegal value for bit value constant: \"" + value + "\".", nullptr, nullptr);
    }
}

std::string BitvectorValue::getValue() const
{
    std::string ret = "";
    for (unsigned int i = 0; i < _value.size(); ++i) {
        switch (_value.at(i)) {
        case bit_zero:
            ret.append("0");
            break;
        case bit_one:
            ret.append("1");
            break;
        case bit_x:
            ret.append("X");
            break;
        case bit_z:
            ret.append("Z");
            break;
        case bit_u:
            ret.append("U");
            break;
        case bit_w:
            ret.append("W");
            break;
        case bit_h:
            ret.append("H");
            break;
        case bit_l:
            ret.append("L");
            break;
        case bit_dontcare:
            ret.append("-");
            break;
        default:
            break;
        }
    }
    return ret;
}

bool BitvectorValue::is01() const
{
    for (unsigned int i = 0; i < _value.size(); ++i) {
        if (_value.at(i) != bit_zero && _value.at(i) != bit_one)
            return false;
    }
    return true;
}

void BitvectorValue::to01()
{
    for (unsigned int i = 0; i < _value.size(); ++i) {
        if (_value.at(i) == bit_h)
            _value[i] = bit_one;
        else if (_value.at(i) != bit_one)
            _value[i] = bit_zero;
    }
}

bool BitvectorValue::isX() const
{
    for (unsigned int i = 0; i < _value.size(); ++i) {
        if (_value.at(i) != bit_x)
            return false;
    }
    return true;
}

std::uint64_t BitvectorValue::getValueAsUnsigned() const { return static_cast<std::uint64_t>(getValueAsSigned()); }

int64_t BitvectorValue::getValueAsSigned() const
{
    if (!is01())
        return 0;
    if (_value.size() > 64)
        return 0;
    char *ptr;
    std::string tmp = getValue();
    return strtoll(tmp.c_str(), &ptr, 2);
}

} // namespace hif
