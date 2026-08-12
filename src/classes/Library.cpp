/// @file Library.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Library.hpp"
#include "hif/HifVisitor.hpp"

namespace hif
{

Library::Library()
    : ReferencedType()
    , _filename()
    , _isStandard(false)
    , _isSystem(false)
{
    // ntd
}

Library::~Library()
{
    // ntd
}

ClassId Library::getClassId() const { return CLASSID_LIBRARY; }

int Library::acceptVisitor(HifVisitor &vis) { return vis.visitLibrary(*this); }

void Library::_calculateFields() { ReferencedType::_calculateFields(); }

const std::string &Library::getFilename() const { return _filename; }

void Library::setFilename(const std::string &f) { _filename = f; }

bool Library::isStandard() const { return _isStandard; }

void Library::setStandard(bool standard) { _isStandard = standard; }

bool Library::isSystem() const { return _isSystem; }

void Library::setSystem(bool system) { _isSystem = system; }

Object *Library::toObject() { return this; }

} // namespace hif
