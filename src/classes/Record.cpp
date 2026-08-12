/// @file Record.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Field.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/Record.hpp"

namespace hif
{

Record::Record()
    : fields()
    , _packed(false)
    , _union(false)
    , _baseType(nullptr)
    , _baseOpaqueType(nullptr)
{
    _setBListParent(fields);
}

Record::~Record()
{
    // ntd
}

ClassId Record::getClassId() const { return CLASSID_RECORD; }

int Record::acceptVisitor(HifVisitor &vis) { return vis.visitRecord(*this); }

Type *Record::getBaseType(bool considerOpacity) const { return considerOpacity ? _baseOpaqueType : _baseType; }

Type *Record::setBaseType(Type *x, bool considerOpacity)
{
    if (considerOpacity) {
        return setChild(_baseOpaqueType, x);
    } else {
        return setChild(_baseType, x);
    }
}

void Record::_calculateFields()
{
    ScopedType::_calculateFields();

    _addBList(fields);
}

std::string Record::_getBListName(const BList<Object> &list) const
{
    if (&list == &fields.toOtherBList<Object>())
        return "fields";
    return ScopedType::_getBListName(list);
}

bool Record::isPacked() const { return _packed; }

void Record::setPacked(bool packed) { _packed = packed; }

bool Record::isUnion() const { return _union; }

void Record::setUnion(bool u) { _union = u; }

} // namespace hif
