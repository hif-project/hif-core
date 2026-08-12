/// @file Procedure.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Procedure.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Procedure::Procedure()
{
    // ntd
}

Procedure::~Procedure()
{
    // ntd
}

ClassId Procedure::getClassId() const { return CLASSID_PROCEDURE; }

int Procedure::acceptVisitor(HifVisitor &vis) { return vis.visitProcedure(*this); }

void Procedure::_calculateFields() { SubProgram::_calculateFields(); }

} // namespace hif
