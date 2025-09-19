/// @file trash.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/trash.hpp"

#include "hif/hif_utils/hif_utils.hpp"
#include "hif/manipulation/manipulation.hpp"
#include "hif/semantics/semantics.hpp"

namespace hif
{

namespace /*anon*/
{

} // namespace

Trash::Trash()
    : _holder()
{
    // ntd
}

Trash::~Trash() { clear(); }
void Trash::clear(Object *where)
{
    TrashHolder tmpTrash;
    for (TrashHolder::iterator i = _holder.begin(); i != _holder.end(); i = _holder.begin()) {
        Object *o = *i;
        _holder.erase(i);
        if (o == nullptr)
            continue;

        bool found = false;

        if (hif::isSubNode(where, o)) {
            tmpTrash.insert(o);
            continue;
        }

        for (TrashHolder::iterator j = _holder.begin(); j != _holder.end();) {
            Object *tmp = *j;
            if (hif::isSubNode(o, tmp)) {
                found = true;
                break;
            } else if (hif::isSubNode(tmp, o)) {
                _holder.erase(j++);
                continue;
            }
            ++j;
        }
        if (!found) {
            o->replace(nullptr);
            delete o;
        }
    }

    _holder.clear();
    _holder = tmpTrash;
}

void Trash::reset() { _holder.clear(); }

void Trash::insert(Object *object)
{
    if (object == nullptr)
        return;
    _holder.insert(object);
}

void Trash::insert(BList<Object> &list)
{
    for (BList<Object>::iterator i = list.begin(); i != list.end();) {
        _holder.insert(*i);
        i = i.remove();
    }
}

bool Trash::contains(Object *o) const
{
    if (o == nullptr)
        return false;
    for (TrashHolder::const_iterator it = _holder.begin(); it != _holder.end(); ++it) {
        if (hif::isSubNode(o, *it))
            return true;
    }
    return false;
}

bool Trash::remove(Object *o)
{
    if (o == nullptr)
        return false;
    TrashHolder::iterator it = _holder.find(o);
    if (it == _holder.end())
        return false;
    _holder.erase(it);
    return true;
}

template <typename T> void Trash::insert(BList<T> &list) { Trash::insert(list.template toOtherBList<Object>()); }
#define HIF_TEMPLATE_METHOD(T) void Trash::insert<T>(BList<T> & list)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

} // namespace hif
