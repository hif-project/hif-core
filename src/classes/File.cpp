/// @file File.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/File.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

File::File()
{
    // ntd
}

File::~File()
{
    // ntd
}

ClassId File::getClassId() const { return CLASSID_FILE; }

int File::acceptVisitor(HifVisitor &vis) { return vis.visitFile(*this); }

void File::_calculateFields() { CompositeType::_calculateFields(); }

} // namespace hif
