/// @file FieldReference.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/FieldReference.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/classes/DataDeclaration.hpp"
#include "hif/classes/Parameter.hpp"
#include "hif/classes/TypeDef.hpp"
#include "hif/classes/ValueTP.hpp"

namespace hif
{

FieldReference::FieldReference()
{
    // ntd
}

FieldReference::~FieldReference()
{
    // ntd
}

ClassId FieldReference::getClassId() const { return CLASSID_FIELDREFERENCE; }

int FieldReference::acceptVisitor(HifVisitor &vis) { return vis.visitFieldReference(*this); }

bool FieldReference::isNotAllowedDeclaration(Declaration *d)
{
    // Allowing TypeDef, and data decls, except Param and ValueTP.
    TypeDef *td = dynamic_cast<TypeDef *>(d);
    if (td != nullptr)
        return false;

    DataDeclaration *dd = dynamic_cast<DataDeclaration *>(d);
    if (dd == nullptr)
        return true;

    Parameter *pp = dynamic_cast<Parameter *>(dd);
    if (pp != nullptr)
        return true;

    ValueTP *vtp = dynamic_cast<ValueTP *>(dd);
    if (vtp != nullptr)
        return true;

    return false;
}

void FieldReference::_calculateFields() { PrefixedReference::_calculateFields(); }

Object *FieldReference::toObject() { return this; }

} // namespace hif
