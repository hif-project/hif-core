/// @file TypeDef.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/TypeDef.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/Range.hpp"

namespace hif
{

TypeDef::TypeDef()
    : templateParameters()
    , _range(nullptr)
    , _isOpaque(false)
    , _isStandard(false)
    , _isExternal(false)
{
    _setBListParent(templateParameters);
}

TypeDef::~TypeDef() { delete (_range); }

bool TypeDef::isOpaque() const { return _isOpaque; }

void TypeDef::setOpaque(bool is_opaque) { _isOpaque = is_opaque; }

Range *TypeDef::getRange() const { return _range; }

Range *TypeDef::setRange(Range *x) { return setChild(_range, x); }

ClassId TypeDef::getClassId() const { return CLASSID_TYPEDEF; }

int TypeDef::acceptVisitor(HifVisitor &vis) { return vis.visitTypeDef(*this); }

void TypeDef::_calculateFields()
{
    TypeDeclaration::_calculateFields();

    _addField(_range);
    _addBList(templateParameters);
}

std::string TypeDef::_getFieldName(const Object *child) const
{
    if (child == _range)
        return "range";
    return TypeDeclaration::_getFieldName(child);
}

std::string TypeDef::_getBListName(const BList<Object> &list) const
{
    if (&list == &templateParameters.toOtherBList<Object>())
        return "templateParameters";
    return TypeDeclaration::_getBListName(list);
}

bool TypeDef::isStandard() const { return _isStandard; }

void TypeDef::setStandard(bool standard) { _isStandard = standard; }

bool TypeDef::isExternal() const { return _isExternal; }

void TypeDef::setExternal(bool external) { _isExternal = external; }

} // namespace hif
