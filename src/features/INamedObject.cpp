/// @file INamedObject.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/features/INamedObject.hpp"
#include "hif/NameTable.hpp"
#include "hif/application_utils/Log.hpp"

namespace hif
{
namespace features
{

void INamedObject::setName(const std::string &name)
{
    _name = name;
}

std::string INamedObject::getName() const { return _name; }

INamedObject::INamedObject()
    : _name(hif::NameTable::getInstance()->none())
{
    // Nothing to do.
}

INamedObject::INamedObject(const hif::features::INamedObject &other)
    : IFeature(other)
    , _name(other._name)
{
    // Nothing to do.
}

hif::features::INamedObject &INamedObject::operator=(const hif::features::INamedObject &other)
{
    if (this == &other)
        return *this;

    IFeature::operator=(other);
    _name = other._name;

    return *this;
}

bool INamedObject::matchName(const std::string &nameToMatch) const { return (_name == nameToMatch); }

} // namespace features
} // namespace hif
