/// @file BListHost.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <cstring>
#include <list>
#include <sstream>
#include <string>

#include "hif/application_utils/Log.hpp"
#include "hif/hif_utils/hif_utils.hpp"

namespace hif
{

// //////////////////////////////////////////////////////////////////////////
// BLink
// //////////////////////////////////////////////////////////////////////////
BListHost::BLink::BLink()
    : parentlist(nullptr)
    , next(nullptr)
    , prev(nullptr)
    , element(nullptr)
{
}

BListHost::BLink::~BLink()
{
    delete element;
#ifndef NDEBUG
    parentlist = nullptr;
    next       = nullptr;
    prev       = nullptr;
    element    = nullptr;
#endif
}

void BListHost::BLink::removeFromList()
{
    if (next != nullptr)
        next->prev = prev;
    if (prev != nullptr)
        prev->next = next;
    if (parentlist->_head == this)
        parentlist->_head = next;
    if (parentlist->_tail == this)
        parentlist->_tail = prev;
    next = nullptr;
    prev = nullptr;
}

void BListHost::BLink::swap(BLink *link)
{
    Object *tmp   = element;
    element       = link->element;
    link->element = tmp;
    link->element->_setParentLink(link);
    element->_setParentLink(this);
}

// //////////////////////////////////////////////////////////////////////////
// BListHost
// //////////////////////////////////////////////////////////////////////////
BListHost::BListHost(CheckSuitableMethod checkSuitableMethod)
    : _parent(nullptr)
    , _head(nullptr)
    , _tail(nullptr)
    , _checkSuitableMethod(checkSuitableMethod)
{
    // ntd
}
BListHost::~BListHost()
{
    clear();
#ifndef NDEBUG
    _parent              = nullptr;
    _head                = nullptr;
    _tail                = nullptr;
    _checkSuitableMethod = nullptr;
#endif
}

BListHost::BListHost(const BListHost &other)
    : _parent(nullptr)
    , _head(nullptr)
    , _tail(nullptr)
    , _checkSuitableMethod(other._checkSuitableMethod)
{
    for (BListHost::iterator i = other.begin(); i != other.end(); ++i) {
        this->push_back(hif::copy(*i));
    }
}

void BListHost::swap(BListHost &other)
{
    // no parent swap
    std::swap(_head, other._head);
    std::swap(_tail, other._tail);
    std::swap(_checkSuitableMethod, other._checkSuitableMethod);
}
std::string BListHost::getName() const
{
    if (_parent == nullptr)
        return "";
    return _parent->getBListName(*reinterpret_cast<const BList<Object> *>(this));
}
BListHost::iterator BListHost::begin() const
{
    if (_head == nullptr)
        return iterator(nullptr);
    return iterator(_head->element);
}
BListHost::iterator BListHost::end() const { return iterator(nullptr); }
BListHost::iterator BListHost::rbegin() const
{
    if (_tail == nullptr)
        return iterator(nullptr);
    return iterator(_tail->element);
}

BListHost::iterator BListHost::rend() const { return iterator(nullptr); }
Object *BListHost::front() const
{
    if (_head == nullptr)
        return nullptr;
    return _head->element;
}
Object *BListHost::back() const
{
    if (_tail == nullptr)
        return nullptr;
    return _tail->element;
}
void BListHost::push_front(Object *o)
{
    if (o == nullptr)
        return;
    BLink *l      = new BLink();
    l->element    = o;
    l->parentlist = this;
    o->_setParentLink(l);
    o->_setParent(nullptr);
    o->_field = nullptr;

    if (_head == nullptr) {
        _head = l;
        _tail = l;
        return;
    }

    _head->prev = l;
    l->next     = _head;
    _head       = l;
}
void BListHost::push_back(Object *o)
{
    if (o == nullptr)
        return;
    BLink *l      = new BLink();
    l->element    = o;
    l->parentlist = this;
    o->_setParentLink(l);
    o->_setParent(nullptr);
    o->_field = nullptr;

    if (_tail == nullptr) {
        _head   = l;
        _tail   = l;
        l->next = nullptr;
        l->prev = nullptr;
        return;
    }

    _tail->next = l;
    l->prev     = _tail;
    _tail       = l;
}
void BListHost::erase(Object *o)
{
    for (BLink *l = _head; l != nullptr; l = l->next) {
        if (l->element != o)
            continue;
        l->removeFromList();
        delete l;
        return;
    }
}
void BListHost::remove(Object *o)
{
    for (BLink *l = _head; l != nullptr; l = l->next) {
        if (l->element != o)
            continue;
        o->_setParentLink(nullptr);
        o->_setParent(nullptr);
        l->element = nullptr;
        l->removeFromList();
        delete l;
        return;
    }
}

void BListHost::removeAll()
{
    for (BListHost::iterator i = this->begin(); i != this->end();) {
        i = i.remove();
    }
}

void BListHost::eraseSubTree(Object *a)
{
    for (BListHost::iterator i = this->begin(); i != this->end(); ++i) {
        if (!hif::isSubNode(a, *i))
            continue;
        i.erase();
    }
}

void BListHost::removeSubTree(Object *a)
{
    for (BListHost::iterator i = this->begin(); i != this->end(); ++i) {
        if (!hif::isSubNode(a, *i))
            continue;
        i.remove();
        return;
    }
}
void BListHost::clear()
{
    BLink *next = nullptr;
    for (BLink *l = _head; l != nullptr; l = next) {
        next = l->next;
        delete l;
    }

    _head = nullptr;
    _tail = nullptr;
}

bool BListHost::empty() const { return _head == nullptr; }

auto BListHost::size() const -> std::size_t
{
    std::size_t ret = 0;
    for (BLink *l = _head; l != nullptr; l = l->next) {
        ++ret;
    }
    return ret;
}

void BListHost::merge(BListHost &x)
{
    if (_tail == nullptr) {
        _head = x._head;
        _tail = x._tail;

        for (BLink *l = x._head; l != nullptr; l = l->next) {
            l->parentlist = this;
        }

        x._head = nullptr;
        x._tail = nullptr;

        return;
    }

    _tail->next = x._head;
    if (!x.empty()) {
        x._head->prev = _tail;
        _tail         = x._tail;
    }

    for (BLink *l = x._head; l != nullptr; l = l->next) {
        l->parentlist = this;
    }

    x._head = nullptr;
    x._tail = nullptr;
}
void BListHost::swap(iterator a, iterator b)
{
    _toBLink((*a)->_getParentLink())->swap(_toBLink((*b)->_getParentLink()));
}
void BListHost::remove_dopplegangers(const bool strict)
{
    for (BLink *i = _head; i != nullptr; i = i->next) {
        for (BLink *j = i->next; j != nullptr;) {
            bool rremove = false;
            if (strict)
                rremove = (i->element == j->element);
            else
                rremove = hif::equals(i->element, j->element);
            if (!rremove) {
                j = j->next;
                continue;
            }

            BLink *next = j->next;
            j->removeFromList();
            delete j;
            j = next;
        }
    }
}
Object *BListHost::getParent() { return _parent; }
void BListHost::setParent(Object *p) { _parent = p; }
std::size_t BListHost::getPosition(Object *o) const
{
    size_t count = 0;
    for (BListHost::iterator i = this->begin(); i != this->end(); ++i) {
        if ((*i) == o)
            return count;
        ++count;
    }

    return count;
}
Object *BListHost::insert(Object *o, std::size_t pos, const bool expand)
{
    BListHost::iterator i = this->begin();
    i                     = i + pos;
    if (i == this->end()) {
        this->push_back(o);
        return nullptr;
    } else if (expand) {
        i.insert_before(o);
        return nullptr;
    } else // !expand
    {
        Object *ret = *i;
        i           = o;
        return ret;
    }
}
Object *BListHost::at(std::size_t pos) const
{
    BListHost::iterator i = this->begin();
    i                     = i + pos;
    return *i;
}
bool BListHost::contains(Object *o) const
{
    if (o == nullptr)
        return false;
    if (!o->isInBList())
        return false;

    return reinterpret_cast<const void *>(this) == reinterpret_cast<const void *>(o->getBList());
}
Object *BListHost::findByName(const std::string &n) const
{
    for (BListHost::iterator i = this->begin(); i != this->end(); ++i) {
        if (hif::objectGetName(*i) == n)
            return *i;
    }

    return nullptr;
}
bool BListHost::checkSuitable(Object *o) const { return (*_checkSuitableMethod)(o); }

void BListHost::addProperty(const std::string &n, TypedObject *v)
{
    for (BListHost::iterator i = this->begin(); i != this->end(); ++i) {
        Object *o = *i;
        if (o->checkProperty(n))
            continue;
        o->addProperty(n, v);
    }
}

void BListHost::addProperty(const PropertyId n, TypedObject *v)
{
    for (BListHost::iterator i = this->begin(); i != this->end(); ++i) {
        Object *o = *i;
        if (o->checkProperty(n))
            continue;
        o->addProperty(n, v);
    }
}

void BListHost::removeProperty(const std::string &n)
{
    for (BListHost::iterator i = this->begin(); i != this->end(); ++i) {
        (*i)->removeProperty(n);
    }
}

void BListHost::removeProperty(const PropertyId n)
{
    for (BListHost::iterator i = this->begin(); i != this->end(); ++i) {
        (*i)->removeProperty(n);
    }
}

bool BListHost::checkProperty(const std::string &n, const bool hasAll) const
{
    if (this->empty())
        return false;
    for (BListHost::iterator i = this->begin(); i != this->end(); ++i) {
        const bool has = (*i)->checkProperty(n);
        if (hasAll && !has)
            return false;
        else if (!hasAll && has)
            return true;
    }

    return hasAll;
}

bool BListHost::checkProperty(const PropertyId n, const bool hasAll) const
{
    if (this->empty())
        return false;
    for (BListHost::iterator i = this->begin(); i != this->end(); ++i) {
        const bool has = (*i)->checkProperty(n);
        if (hasAll && !has)
            return false;
        else if (!hasAll && has)
            return true;
    }

    return hasAll;
}

void BListHost::clearProperties()
{
    for (BListHost::iterator i = this->begin(); i != this->end(); ++i) {
        (*i)->clearProperties();
    }
}

bool BListHost::hasProperties(const bool hasAll) const
{
    if (this->empty())
        return false;
    for (BListHost::iterator i = this->begin(); i != this->end(); ++i) {
        const bool has = (*i)->hasProperties();
        if (hasAll && !has)
            return false;
        else if (!hasAll && has)
            return true;
    }

    return hasAll;
}

BListHost::BLink *BListHost::_toBLink(void *l) { return static_cast<BListHost::BLink *>(l); }

// //////////////////////////////////////////////////////////////////////////
// BListHost iterator
// //////////////////////////////////////////////////////////////////////////
BListHost::iterator::iterator(Object *o)
    : _link(nullptr)
{
    if (o != nullptr) {
        _link = _toBLink(o->_getParentLink());
    }
}
BListHost::iterator::iterator(const iterator &i)
    : _link(i._link)
{
}
BListHost::iterator::~iterator()
{
#ifndef NDEBUG
    _link = nullptr;
#endif
}
BListHost::iterator &BListHost::iterator::operator=(const iterator &i)
{
    if (this == &i)
        return *this;
    _link = i._link;
    return *this;
}
BListHost::iterator &BListHost::iterator::operator=(Object *o)
{
    if (_link == nullptr) {
        messageError("accessing invalid iterator (1).", nullptr, nullptr);
    }
    if (o == nullptr) {
        messageError("invalid parameter.", nullptr, nullptr);
    }
    // must be inside a BListHost
    Object *old = _link->element;
    old->_setParentLink(nullptr);
    old->_setParent(nullptr);
    _link->element = o;
    o->_setParentLink(_link);
    o->_setParent(nullptr);
    o->_field = nullptr;

    return *this;
}
BListHost::iterator &BListHost::iterator::erase()
{
    if (_link == nullptr) {
        messageError("accessing invalid iterator (2).", nullptr, nullptr);
    }
    BLink *next = _link->next;
    _link->removeFromList();
    delete _link;
    _link = next;
    return *this;
}
BListHost::iterator &BListHost::iterator::rerase()
{
    if (_link == nullptr) {
        messageError("accessing invalid iterator (3).", nullptr, nullptr);
    }
    BLink *prev = _link->prev;
    _link->removeFromList();
    delete _link;
    _link = prev;
    return *this;
}

BListHost::iterator &BListHost::iterator::remove()
{
    if (_link == nullptr) {
        messageError("accessing invalid iterator (4).", nullptr, nullptr);
    }
    BLink *next    = _link->next;
    Object *e      = _link->element;
    _link->element = nullptr;
    e->_setParentLink(nullptr);
    e->_setParent(nullptr);
    _link->removeFromList();
    delete _link;
    _link = next;
    return *this;
}
BListHost::iterator &BListHost::iterator::rremove()
{
    if (_link == nullptr) {
        messageError("accessing invalid iterator (5).", nullptr, nullptr);
    }
    BLink *prev    = _link->prev;
    Object *e      = _link->element;
    _link->element = nullptr;
    e->_setParentLink(nullptr);
    e->_setParent(nullptr);
    _link->removeFromList();
    delete _link;
    _link = prev;
    return *this;
}

//#if (defined __GNUC__ )
//#pragma GCC diagnostic ignored "-Weffc++"
//#endif

BListHost::iterator BListHost::iterator::insert_after(Object *a)
{
    if (a == nullptr)
        return iterator();
    if (_link == nullptr) {
        messageError("accessing invalid iterator (6).", nullptr, nullptr);
    }
    BLink *l = new BLink();
    a->_setParentLink(l);
    a->_setParent(nullptr);
    a->_field     = nullptr;
    l->element    = a;
    l->parentlist = _link->parentlist;
    l->next       = _link->next;
    l->prev       = _link;
    _link->next   = l;
    if (l->next != nullptr)
        l->next->prev = l;
    messageAssert(_link->parentlist != nullptr, "Unexpected link without parent", nullptr, nullptr);
    if (_link == _link->parentlist->_tail)
        _link->parentlist->_tail = l;

    return iterator(a);
}
BListHost::iterator BListHost::iterator::insert_before(Object *a)
{
    if (a == nullptr)
        return iterator();
    if (_link == nullptr) {
        messageError("accessing invalid iterator (7).", nullptr, nullptr);
    }
    BLink *l   = new BLink();
    l->element = a;
    a->_setParentLink(l);
    l->parentlist = _link->parentlist;
    a->_setParent(nullptr);
    a->_field   = nullptr;
    l->prev     = _link->prev;
    l->next     = _link;
    _link->prev = l;
    if (l->prev != nullptr)
        l->prev->next = l;
    messageAssert(_link->parentlist != nullptr, "Unexpected link without parent", nullptr, nullptr);
    if (_link == _link->parentlist->_head)
        _link->parentlist->_head = l;

    return iterator(a);
}
BListHost::iterator BListHost::iterator::insert_after(hif::BListHost &a)
{
    iterator ret(*this);
    if (!a.empty())
        ret = iterator(a.back());

    // todo optimize with pointers
    while (!a.empty()) {
        Object *obj = a.back();
        a.remove(obj);
        this->insert_after(obj);
    }

    return ret;
}
BListHost::iterator BListHost::iterator::insert_before(hif::BListHost &a)
{
    iterator ret(*this);
    if (!a.empty())
        ret = iterator(a.front());

    // todo optimize with pointers
    for (BListHost::iterator i = a.begin(); i != a.end();) {
        Object *o = *i;
        i         = i.remove();
        this->insert_before(o);
    }

    return ret;
}

bool BListHost::iterator::isEnd() const { return _link == nullptr; }
Object *BListHost::iterator::operator*() const
{
    if (_link == nullptr)
        return nullptr;
    return _link->element;
}
BListHost::iterator BListHost::iterator::operator++(int)
{
    iterator tmp(*this);
    ++(*this);
    return tmp;
}
BListHost::iterator &BListHost::iterator::operator++()
{
    if (_link == nullptr) {
        messageError("accessing invalid iterator (8).", nullptr, nullptr);
    }
    _link = _link->next;
    return *this;
}
BListHost::iterator BListHost::iterator::operator--(int)
{
    iterator tmp(*this);
    --(*this);
    return tmp;
}
BListHost::iterator &BListHost::iterator::operator--()
{
    if (_link == nullptr) {
        messageError("accessing invalid iterator (9).", nullptr, nullptr);
    }
    _link = _link->prev;
    return *this;
}

BListHost::iterator BListHost::iterator::operator+(std::size_t s) const
{
    iterator ret(*this);
    for (size_t i = 0; i < s; ++i) {
        messageDebugAssert(ret._link != nullptr, "Unexpected nullptr link (1)", nullptr, nullptr);

        if (ret._link != nullptr)
            ret._link = ret._link->next;
    }

    return ret;
}

BListHost::iterator BListHost::iterator::operator-(std::size_t s) const
{
    iterator ret(*this);
    for (size_t i = 0; i < s; ++i) {
        messageDebugAssert(ret._link != nullptr, "Unexpected nullptr link (2)", nullptr, nullptr);

        if (ret._link != nullptr)
            ret._link = ret._link->prev;
    }

    return ret;
}
bool BListHost::iterator::operator==(const iterator &i) const { return (_link == i._link); }
bool BListHost::iterator::operator!=(const iterator &i) const { return (_link != i._link); }

} // namespace hif
