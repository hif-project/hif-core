/// @file BList.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <list>
#include <sstream>
#include <string>

#include "hif/classes/classes.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wweak-template-vtables"
#endif

namespace hif
{
// //////////////////////////////////////////////////////////////////////////
// BList
// //////////////////////////////////////////////////////////////////////////

template <class T>
BList<T>::BList()
    : BListHost(&BList<T>::_checkSuitable)
{
    // ntd
}
template <class T> BList<T>::~BList()
{
    // ntd
}

template <class T>
BList<T>::BList(const BList<T> &other)
    : BListHost(other)
{
    // ntd
}

template <class T> BList<T> &BList<T>::operator=(BList<T> other)
{
    swap(other);
    return *this;
}

template <class T> void BList<T>::swap(BList<T> &other) { BListHost::swap(other); }

template <class T> std::string BList<T>::getName() const { return BListHost::getName(); }

template <class T> typename BList<T>::iterator BList<T>::begin() const { return BListHost::begin(); }

template <class T> typename BList<T>::iterator BList<T>::end() const { return static_cast<iterator>(BListHost::end()); }

template <class T> typename BList<T>::iterator BList<T>::rbegin() const
{
    return static_cast<iterator>(BListHost::rbegin());
}

template <class T> typename BList<T>::iterator BList<T>::rend() const
{
    return static_cast<iterator>(BListHost::rend());
}

template <class T> T *BList<T>::front() const { return static_cast<T *>(BListHost::front()); }

template <class T> T *BList<T>::back() const { return static_cast<T *>(BListHost::back()); }

template <class T> void BList<T>::push_front(T *a) { BListHost::push_front(a); }

template <class T> void BList<T>::push_back(T *a) { BListHost::push_back(a); }

template <class T> void BList<T>::clear() { BListHost::clear(); }
template <class T> void BList<T>::erase(T *a) { BListHost::erase(a); }

template <class T> void BList<T>::remove(T *a) { BListHost::remove(a); }

template <class T> void BList<T>::removeAll() { BListHost::removeAll(); }

template <class T> void BList<T>::eraseSubTree(T *a) { BListHost::eraseSubTree(a); }

template <class T> void BList<T>::removeSubTree(T *a) { BListHost::removeSubTree(a); }

template <class T> bool BList<T>::empty() const { return BListHost::empty(); }

template <class T> typename BList<T>::size_t BList<T>::size() const { return BListHost::size(); }

template <class T> void BList<T>::merge(BList<T> &x) { BListHost::merge(x); }

template <class T> void BList<T>::swap(iterator a, iterator b) { BListHost::swap(a.toBaseClass(), b.toBaseClass()); }

template <class T> void BList<T>::remove_dopplegangers(bool strict) { BListHost::remove_dopplegangers(strict); }

template <class T> Object *BList<T>::getParent() { return BListHost::getParent(); }

template <class T> void BList<T>::setParent(Object *p) { BListHost::setParent(p); }

template <class T> bool BList<T>::_checkSuitable(Object *o) { return dynamic_cast<T *>(o) != nullptr; }
template <class T> typename BList<T>::size_t BList<T>::getPosition(T *o) const { return BListHost::getPosition(o); }

template <class T> T *BList<T>::insert(T *o, const size_t pos, const bool expand)
{
    return static_cast<T *>(BListHost::insert(o, pos, expand));
}

template <class T> T *BList<T>::at(const size_t pos) const { return static_cast<T *>(BListHost::at(pos)); }

template <class T> bool BList<T>::contains(T *o) const { return BListHost::contains(o); }

template <class T> T *BList<T>::findByName(const std::string &n) const
{
    return static_cast<T *>(BListHost::findByName(n));
}

template <class T> bool BList<T>::checkSuitable(Object *o) const { return BListHost::checkSuitable(o); }

template <class T> void BList<T>::addProperty(const std::string &n, TypedObject *v) { BListHost::addProperty(n, v); }

template <class T> void BList<T>::addProperty(const PropertyId n, TypedObject *v) { BListHost::addProperty(n, v); }

template <class T> void BList<T>::removeProperty(const std::string &n) { BListHost::removeProperty(n); }

template <class T> void BList<T>::removeProperty(const PropertyId n) { BListHost::removeProperty(n); }

template <class T> bool BList<T>::checkProperty(const std::string &n, const bool hasAll) const
{
    return BListHost::checkProperty(n, hasAll);
}

template <class T> bool BList<T>::checkProperty(const PropertyId n, const bool hasAll) const
{
    return BListHost::checkProperty(n, hasAll);
}

template <class T> void BList<T>::clearProperties() { BListHost::clearProperties(); }

template <class T> bool BList<T>::hasProperties(const bool hasAll) const { return BListHost::hasProperties(hasAll); }

// //////////////////////////////////////////////////////////////////////////
// BList iterator
// //////////////////////////////////////////////////////////////////////////

template <class T>
BList<T>::iterator::iterator(T *o)
    : BListHost::iterator(o)
{
}

template <class T>
BList<T>::iterator::iterator(const iterator &i)
    : BListHost::iterator(i)
{
}

template <class T>
BList<T>::iterator::iterator(const BListHost::iterator &i)
    : BListHost::iterator(i)
{
}

template <class T> BList<T>::iterator::~iterator() {}

template <class T> typename BList<T>::iterator &BList<T>::iterator::operator=(const iterator &i)
{
    BListHost::iterator::operator=(i);
    return *this;
}

template <class T> typename BList<T>::iterator &BList<T>::iterator::operator=(T *o)
{
    BListHost::iterator::operator=(o);
    return *this;
}

template <class T> typename BList<T>::iterator &BList<T>::iterator::erase()
{
    return static_cast<iterator &>(BListHost::iterator::erase());
}

template <class T> typename BList<T>::iterator &BList<T>::iterator::rerase()
{
    return static_cast<iterator &>(BListHost::iterator::rerase());
}

template <class T> typename BList<T>::iterator &BList<T>::iterator::remove()
{
    return static_cast<iterator &>(BListHost::iterator::remove());
}

template <class T> typename BList<T>::iterator &BList<T>::iterator::rremove()
{
    return static_cast<iterator &>(BListHost::iterator::rremove());
}

template <class T> typename BList<T>::iterator BList<T>::iterator::insert_after(T *a)
{
    return static_cast<iterator>(BListHost::iterator::insert_after(a));
}

template <class T> typename BList<T>::iterator BList<T>::iterator::insert_before(T *a)
{
    return static_cast<iterator>(BListHost::iterator::insert_before(a));
}

template <class T> typename BList<T>::iterator BList<T>::iterator::insert_after(BList<T> &a)
{
    BListHost *l = reinterpret_cast<BListHost *>(&a);
    return static_cast<iterator>(BListHost::iterator::insert_after(*l));
}

template <class T> typename BList<T>::iterator BList<T>::iterator::insert_before(BList<T> &a)
{
    BListHost *l = reinterpret_cast<BListHost *>(&a);
    return static_cast<iterator>(BListHost::iterator::insert_before(*l));
}

template <class T> bool BList<T>::iterator::isEnd() const { return BListHost::iterator::isEnd(); }

template <class T> T *BList<T>::iterator::operator*() const
{
    return static_cast<T *>(BListHost::iterator::operator*());
}

#if (defined __GNUC__)
#    if HIF_DIAGNOSTIC_PUSH_POP
#        pragma GCC diagnostic push
#    endif
#    pragma GCC diagnostic ignored "-Weffc++"
#endif

template <class T> typename BList<T>::iterator BList<T>::iterator::operator++(int)
{
    return static_cast<iterator>(BListHost::iterator::operator++(0));
}

template <class T> typename BList<T>::iterator &BList<T>::iterator::operator++()
{
    return static_cast<iterator &>(BListHost::iterator::operator++());
}

template <class T> typename BList<T>::iterator BList<T>::iterator::operator--(int)
{
    return static_cast<iterator>(BListHost::iterator::operator--(0));
}

template <class T> typename BList<T>::iterator &BList<T>::iterator::operator--()
{
    return static_cast<iterator &>(BListHost::iterator::operator--());
}

template <class T> typename BList<T>::iterator BList<T>::iterator::operator+(const size_t s) const
{
    return static_cast<iterator>(BListHost::iterator::operator+(s));
}

template <class T> typename BList<T>::iterator BList<T>::iterator::operator-(const size_t s) const
{
    return static_cast<iterator>(BListHost::iterator::operator-(s));
}
#if (defined __GNUC__)
#    if HIF_DIAGNOSTIC_PUSH_POP
#        pragma GCC diagnostic pop
#    else
#        pragma GCC diagnostic ignored "-Weffc++"
#    endif
#endif

template <class T> bool BList<T>::iterator::operator==(const iterator &i) const
{
    return BListHost::iterator::operator==(i);
}

template <class T> bool BList<T>::iterator::operator!=(const iterator &i) const
{
    return BListHost::iterator::operator!=(i);
}
template <class T> BListHost::iterator &BList<T>::iterator::toBaseClass() { return *this; }

HIF_INSTANTIATE_TEMPLATE_FOR_HIF_OBJECTS(BList)

} // namespace hif
