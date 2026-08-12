/// @file RecordValue.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/RecordValueAlt.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/RecordValue.hpp"

namespace hif
{

RecordValue::RecordValue()
    : alts()
{
    _setBListParent(alts);
}

RecordValue::~RecordValue()
{
    // ntd
}

ClassId RecordValue::getClassId() const { return CLASSID_RECORDVALUE; }

int RecordValue::acceptVisitor(HifVisitor &vis) { return vis.visitRecordValue(*this); }

void RecordValue::_calculateFields()
{
    Value::_calculateFields();

    _addBList(alts);
}

std::string RecordValue::_getBListName(const BList<Object> &list) const
{
    if (&list == &alts.toOtherBList<Object>())
        return "alts";
    return Value::_getBListName(list);
}

} // namespace hif
