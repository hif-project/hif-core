/// @file SubProgram.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Parameter.hpp"

#include "hif/application_utils/Log.hpp"
#include "hif/classes/StateTable.hpp"
#include "hif/classes/SubProgram.hpp"

namespace hif
{

SubProgram::SubProgram()
    : parameters()
    , templateParameters()
    , _statetable(nullptr)
    , _kind(INSTANCE)
    , _isStandard(false)
{
    _setBListParent(parameters);
    _setBListParent(templateParameters);
}

SubProgram::~SubProgram() { delete (_statetable); }

StateTable *SubProgram::getStateTable() const { return _statetable; }

StateTable *SubProgram::setStateTable(StateTable *x) { return setChild(_statetable, x); }

SubProgram::Kind SubProgram::getKind() const { return _kind; }

void SubProgram::setKind(Kind k) { _kind = k; }

bool SubProgram::isStandard() const { return _isStandard; }

void SubProgram::setStandard(bool standard) { _isStandard = standard; }

std::string SubProgram::kindToString(const Kind t)
{
    switch (t) {
    case INSTANCE:
        return "INSTANCE";
    case VIRTUAL:
        return "VIRTUAL";
    case STATIC:
        return "STATIC";
    case MACRO:
        return "MACRO";
    case IMPLICIT_INSTANCE:
        return "IMPLICIT_INSTANCE";
    default:;
    }

    messageError("Unexpected subprogram kind", nullptr, nullptr);
}

SubProgram::Kind SubProgram::kindFromString(const std::string &s)
{
    if (s == "INSTANCE")
        return INSTANCE;
    else if (s == "VIRTUAL")
        return VIRTUAL;
    else if (s == "STATIC")
        return STATIC;
    else if (s == "MACRO")
        return MACRO;
    else if (s == "IMPLICIT_INSTANCE")
        return IMPLICIT_INSTANCE;

    messageError(std::string("Unexpected subprogram kind: ") + s, nullptr, nullptr);
}

void SubProgram::_calculateFields()
{
    Scope::_calculateFields();

    _addField(_statetable);
    _addBList(parameters);
    _addBList(templateParameters);
}

std::string SubProgram::_getFieldName(const Object *child) const
{
    if (child == _statetable)
        return "stateTables";
    return Scope::_getFieldName(child);
}

std::string SubProgram::_getBListName(const BList<Object> &list) const
{
    if (&list == &parameters.toOtherBList<Object>())
        return "parameters";
    if (&list == &templateParameters.toOtherBList<Object>())
        return "templateParameters";
    return Scope::_getBListName(list);
}

} // namespace hif
