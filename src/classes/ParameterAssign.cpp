/// @file ParameterAssign.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/ParameterAssign.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

ParameterAssign::ParameterAssign()
{
    // ntd
}

ParameterAssign::~ParameterAssign()
{
    // ntd
}

ClassId ParameterAssign::getClassId() const { return CLASSID_PARAMETERASSIGN; }

int ParameterAssign::acceptVisitor(HifVisitor &vis) { return vis.visitParameterAssign(*this); }

void ParameterAssign::_calculateFields() { PPAssign::_calculateFields(); }

hif::Object *hif::ParameterAssign::toObject() { return this; }

} // namespace hif
