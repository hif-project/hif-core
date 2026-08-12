/// @file Identifier.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Identifier.hpp"
#include "hif/HifVisitor.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/classes/Type.hpp"

namespace hif
{
Identifier::Identifier()
{
    // ntd
}

Identifier::Identifier(const std::string &name)
{
    messageAssert(!name.empty(), "given null name", nullptr, nullptr);
    messageDebugAssert(name != NameTable::getInstance()->none(), "given name none", nullptr, nullptr);
    setName(name);
}

Identifier::~Identifier()
{
    // ntd
}

ClassId Identifier::getClassId() const { return CLASSID_IDENTIFIER; }

int Identifier::acceptVisitor(HifVisitor &vis) { return vis.visitIdentifier(*this); }

void Identifier::_calculateFields() { Value::_calculateFields(); }

Object *Identifier::toObject() { return this; }

} // namespace hif
