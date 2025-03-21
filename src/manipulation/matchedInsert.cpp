/// @file matchedInsert.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <iostream>

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/manipulation/matchedInsert.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{
namespace manipulation
{

namespace /* anon */
{

bool _checkExisting(Object *_existing, Object *_newParent, Object *_oldObj, const MatchedInsertType::type _type)
{
    if (_existing == nullptr)
        return true;

    switch (_type) {
    case MatchedInsertType::TYPE_DELETE:
        delete _existing;
        break;
    case MatchedInsertType::TYPE_ERROR:
    case MatchedInsertType::TYPE_EXPAND:
        hif::manipulation::matchedInsert(
            _existing, _newParent, _oldObj, _oldObj->getParent(), MatchedInsertType::TYPE_ONLY_REPLACE);
        return false;
    case MatchedInsertType::TYPE_ONLY_REPLACE:
        break;
    default:
        messageError("Unexpected case", nullptr, nullptr);
    }

    return true;
}
template <typename T> bool _checked_cast(Object *o, T *&no)
{
    no = dynamic_cast<T *>(o);
    return (o == nullptr || no != nullptr);
}

typedef unsigned char Byte;
template <typename T>
bool _matchBList(
    Object *&_existing,
    BList<T> &oldList,
    BList<T> &newList,
    Object *_newObj,
    Object *_oldObj,
    const MatchedInsertType::type _type)
{
    typename BList<T>::size_t position = oldList.getPosition(static_cast<T *>(_oldObj));
    if (position == oldList.size())
        return false;

    typename BList<T>::size_t listSize = newList.size();
    if (position <= listSize && _type != MatchedInsertType::TYPE_EXPAND) {
        _existing = newList.at(position);
        // Position is already correct
    } else if (position <= listSize /*&& _type == MatchedInsertType::TYPE_EXPAND*/) {
        _existing = nullptr;
        // Position is already correct
    } else if (/*position > listSize && */ _type != MatchedInsertType::TYPE_EXPAND) {
        position  = listSize;
        // Never error since the matching element does not exist!
        _existing = nullptr; // newList.at( position );
    } else                   // if( position > listSize && _type == MatchedInsertType::EXPAND )
    {
        position  = listSize;
        _existing = nullptr;
    }

    T *no;
    if (!_checked_cast(_newObj, no))
        return false;
    newList.insert(no, position, _type == MatchedInsertType::TYPE_EXPAND);

    return true;
}
} // namespace

bool matchedInsert(
    Object *newObj,
    Object *newParent,
    Object *oldObj,
    Object *oldParent,
    const MatchedInsertType::type type)
{
    if (oldParent == nullptr)
        oldParent = oldObj->getParent();
    if (oldParent == nullptr) {
        messageDebugAssert(oldParent != nullptr, "Old parent is nullptr.", nullptr, nullptr);
        return false;
    }
    messageAssert(newParent != nullptr, "New parent is nullptr.", nullptr, nullptr);

    Object *_existing;
    if (oldObj->isInBList()) {
        BList<Object> *oldList = oldObj->getBList();
        const ptrdiff_t shift  = reinterpret_cast<Byte *>(oldList) - reinterpret_cast<Byte *>(oldParent);

        BList<Object> *newList = reinterpret_cast<BList<Object> *>(reinterpret_cast<Byte *>(newParent) + shift);

        if (!_matchBList(_existing, *oldList, *newList, newObj, oldObj, type)) {
            return false;
        }
    } else {
        const ptrdiff_t shift = reinterpret_cast<Byte *>(oldObj->_field) - reinterpret_cast<Byte *>(oldParent);
        newObj->replace(nullptr);
        newObj->_field = reinterpret_cast<Object **>(reinterpret_cast<Byte *>(newParent) + shift);
        _existing      = *newObj->_field;
        if (_existing != nullptr)
            _existing->replace(nullptr);
        *newObj->_field = newObj;
        newObj->_setParent(newParent);
    }

    return _checkExisting(_existing, newParent, oldObj, type);
}

} // namespace manipulation
} // namespace hif
