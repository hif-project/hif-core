/// @file matchedGet.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/manipulation/matchedGet.hpp"

#include "hif/hif.hpp"

namespace hif
{
namespace manipulation
{

namespace /*anon*/
{

typedef unsigned char Byte;

} // namespace

Object *matchedGet(Object *newParent, Object *oldObj, Object *oldParent)
{
    if (oldParent == nullptr)
        oldParent = oldObj->getParent();
    if (oldParent == nullptr) {
        messageDebugAssert(oldParent != nullptr, "Old parent is nullptr.", nullptr, nullptr);
        return nullptr;
    }
    messageAssert(newParent != nullptr, "New parent is nullptr.", nullptr, nullptr);

    EqualsOptions opt;
    opt.skipChilden = true;
    if (!hif::equals(newParent, oldParent, opt))
        return nullptr;

    Object *ret = nullptr;
    if (oldObj->isInBList()) {
        BList<Object> *oldList     = oldObj->getBList();
        const std::ptrdiff_t shift = reinterpret_cast<Byte *>(oldList) - reinterpret_cast<Byte *>(oldParent);

        BList<Object> *newList = reinterpret_cast<BList<Object> *>(reinterpret_cast<Byte *>(newParent) + shift);

        std::size_t pos = oldList->getPosition(oldObj);
        ret                             = newList->at(pos);
    } else {
        const std::ptrdiff_t shift = reinterpret_cast<Byte *>(oldObj->_field) - reinterpret_cast<Byte *>(oldParent);
        Object **newObjField       = reinterpret_cast<Object **>(reinterpret_cast<Byte *>(newParent) + shift);

        ret = *newObjField;
    }

    return ret;
}

} // namespace manipulation
} // namespace hif
