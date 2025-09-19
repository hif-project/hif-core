/// @file getMatchingScope.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/hif_utils/getMatchingScope.hpp"
#include "hif/hif_utils/getNearestParent.hpp"

namespace hif
{

namespace /*anon*/
{

Object *_getMatchingScope(Object *o, const std::string &name, bool checkSwitch)
{
    if (o == nullptr)
        return nullptr;
    if (dynamic_cast<For *>(o) != nullptr) {
        For *f = static_cast<For *>(o);
        if (f->getName() == name || NameTable::getInstance()->none() == name)
            return o;
    } else if (dynamic_cast<While *>(o) != nullptr) {
        While *f = static_cast<While *>(o);
        if (f->getName() == name || NameTable::getInstance()->none() == name)
            return o;
    } else if (checkSwitch && dynamic_cast<Switch *>(o) != nullptr) {
        if (NameTable::getInstance()->none() == name)
            return o;
    }

    return _getMatchingScope(o->getParent(), name, checkSwitch);
}

} // namespace
Object *getMatchingScope(Break *o) { return _getMatchingScope(o, o->getName(), true); }

Object *getMatchingScope(Continue *o) { return _getMatchingScope(o, o->getName(), false); }

Object *getMatchingScope(Object *o)
{
    if (dynamic_cast<Break *>(o) != nullptr) {
        return getMatchingScope(static_cast<Break *>(o));
    } else if (dynamic_cast<Continue *>(o) != nullptr) {
        return getMatchingScope(static_cast<Continue *>(o));
    }
    return nullptr;
}
} // namespace hif
