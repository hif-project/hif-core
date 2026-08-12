/// @file TimeValue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/TimeValue.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/classes/RealValue.hpp"
#include "hif/hif_utils/hif_utils.hpp"

namespace hif
{

TimeValue::TimeValue()
    : _unit(TimeValue::time_ns)
    , _value(0.0)
{
    // ntd
}

TimeValue::~TimeValue()
{
    // ntd
}

bool TimeValue::operator<(const TimeValue &other) const
{
    TimeUnit minUnit = (_unit < other._unit) ? _unit : other._unit;

    TimeValue *_this  = hif::copy(this);
    TimeValue *_other = hif::copy(&other);

    _this->changeUnit(minUnit);
    _other->changeUnit(minUnit);

    bool ret = _this->getValue() < _other->getValue();

    delete _this;
    delete _other;

    return ret;
}

ClassId TimeValue::getClassId() const { return CLASSID_TIMEVALUE; }

int TimeValue::acceptVisitor(HifVisitor &vis) { return vis.visitTimeValue(*this); }

std::string TimeValue::timeUnitToString(const TimeUnit t)
{
    switch (t) {
    case time_fs:
        return "FS";
    case time_ps:
        return "PS";
    case time_ns:
        return "NS";
    case time_us:
        return "US";
    case time_ms:
        return "MS";
    case time_sec:
        return "SEC";
    case time_min:
        return "MIN";
    case time_hr:
        return "HR";

    default:;
    }

    messageError("Unexpected time unit", nullptr, nullptr);
}

TimeValue::TimeUnit TimeValue::timeUnitFromString(const std::string &s)
{
    std::string sUp;
    for (std::string::const_iterator i = s.begin(); i != s.end(); ++i) {
        sUp.push_back(static_cast<char>(toupper(*i)));
    }
    if (sUp == "FS")
        return time_fs;
    else if (sUp == "PS")
        return time_ps;
    else if (sUp == "NS")
        return time_ns;
    else if (sUp == "US")
        return time_us;
    else if (sUp == "MS")
        return time_ms;
    else if (sUp == "SEC")
        return time_sec;
    else if (sUp == "MIN")
        return time_min;
    else if (sUp == "HR")
        return time_hr;

    messageError(std::string("Unexpected time unit: ") + sUp, nullptr, nullptr);
}

void TimeValue::_calculateFields() { ConstValue::_calculateFields(); }

double TimeValue::getValue() const { return _value; }

double TimeValue::setValue(const double x)
{
    double old = _value;
    _value     = x;
    return old;
}

void TimeValue::changeUnit(const TimeUnit u)
{
    double scale[] = {1.0, 1000.0, 1000.0, 1000.0, 1000.0, 1000.0, 60.0, 60.0};

    unsigned int myU = static_cast<unsigned int>(_unit);
    unsigned int otU = static_cast<unsigned int>(u);

    while (myU < otU) {
        ++myU;
        _value *= scale[_unit];
    }

    while (myU > otU) {
        _value /= scale[_unit];
        --myU;
    }

    _unit = static_cast<TimeUnit>(myU);
}

TimeValue::TimeUnit TimeValue::getUnit() const { return _unit; }

void TimeValue::setUnit(TimeValue::TimeUnit u) { _unit = u; }

} // namespace hif
