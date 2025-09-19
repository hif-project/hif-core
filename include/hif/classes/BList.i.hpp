/// @file BList.i.hpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include "hif/classes/BList.hpp"

namespace hif
{

template <class T> template <typename T1> BList<T>::operator BList<T1> &() { return this->toOtherBList<T1>(); }

template <class T> template <typename Comparator> bool BList<T>::sort(Comparator &c) { return BListHost::sort(c); }

template <class T> template <typename T1> BList<T1> &BList<T>::toOtherBList()
{
    // Sanity checks:
    T *t   = nullptr;
    T1 *t1 = nullptr;
    t1     = static_cast<T1 *>(t);
    t      = static_cast<T *>(t1);
    // Conversion:
    union U {
        BList<T> *l1;
        BList<T1> *l2;
    };
    U u;
    u.l1 = this;
    return *(u.l2);
}

template <class T> template <typename T1> const BList<T1> &BList<T>::toOtherBList() const
{
    // Sanity checks:
    T *t   = nullptr;
    T1 *t1 = nullptr;
    t1     = static_cast<T1 *>(t);
    t      = static_cast<T *>(t1);
    // Conversion:
    union U {
        const BList<T> *l1;
        const BList<T1> *l2;
    };
    U u;
    u.l1 = this;
    return *(u.l2);
}

} // namespace hif
