/// @file Object.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>
#include <sstream>

#include "hif/application_utils/Log.hpp"
#include "hif/classes/Object.hpp"
#include "hif/classes/TypedObject.hpp"
#include "hif/hif_utils/hif_utils.hpp"

namespace hif
{

// /////////////////////////////////////////////////////////////////////////////
// CodeInfo
// /////////////////////////////////////////////////////////////////////////////

Object::CodeInfo::CodeInfo()
    : filename()
    , lineNumber(0)
    , columnNumber(0)
{
    // ntd
}

Object::CodeInfo::CodeInfo(const std::string &f, unsigned int l, unsigned int c)
    : filename(f)
    , lineNumber(l)
    , columnNumber(c)
{
    // ntd
}

Object::CodeInfo::~CodeInfo()
{
    // ntd
}

Object::CodeInfo::CodeInfo(const Object::CodeInfo &other)
    : filename(other.filename)
    , lineNumber(other.lineNumber)
    , columnNumber(other.columnNumber)
{
    // ntd
}

Object::CodeInfo &Object::CodeInfo::operator=(const Object::CodeInfo &other)
{
    CodeInfo tmp(other);
    swap(tmp);
    return *this;
}

void Object::CodeInfo::swap(Object::CodeInfo &other)
{
    filename.swap(other.filename);
    std::swap(lineNumber, other.lineNumber);
    std::swap(columnNumber, other.columnNumber);
}

bool Object::CodeInfo::operator<(const Object::CodeInfo &o) const
{
    if (filename < o.filename)
        return true;
    if (o.filename < filename)
        return false;
    if (lineNumber < o.lineNumber)
        return true;
    if (o.lineNumber < lineNumber)
        return false;
    if (columnNumber < o.columnNumber)
        return true;
    if (o.columnNumber < columnNumber)
        return false;
    return false;
}

std::string Object::CodeInfo::getSourceInfoString() const
{
    if (lineNumber == 0)
        return "";
    std::stringstream ss;
    ss << filename << ":" << lineNumber;
    if (columnNumber != 0)
        ss << ":" << columnNumber;
    return ss.str();
}
// /////////////////////////////////////////////////////////////////////////////
// Object
// /////////////////////////////////////////////////////////////////////////////
void Object::_setBListParent(BList<Object> &p) { p.setParent(this); }

void Object::_setParentLink(void *p) { _parentlink = p; }

bool Object::isInBList() const { return _parentlink != nullptr; }

BList<Object> *Object::getBList() const
{
    if (_parentlink == nullptr)
        return nullptr;
    return reinterpret_cast<BList<Object> *>(static_cast<BListHost::BLink *>(_parentlink)->parentlist);
}

Object *Object::getParent() const
{
    if (_parentlink != nullptr)
        return static_cast<BListHost::BLink *>(_parentlink)->parentlist->getParent();
    return _parent;
}

template <typename T> T *Object::getParent() const { return dynamic_cast<T *>(getParent()); }

Object::Object()
    : _comments(nullptr)
    , _parentlink(nullptr)
    , _parent(nullptr)
    , _codeInfo(nullptr)
    , _properties(nullptr)
    , _field(nullptr)
    , _fields(nullptr)
    , _blists(nullptr)
{
}

Object::~Object()
{
    if (_properties != nullptr) {
        for (PropertyMap::const_iterator it(_properties->begin()); it != _properties->end(); ++it) {
            delete it->second;
        }

        delete _properties;
    }

    delete _comments;
    delete _codeInfo;
    delete _fields;
    delete _blists;
}
void Object::_setParent(Object *p) { _parent = p; }

TypedObject *Object::addProperty(const std::string &n, TypedObject *v)
{
    if (_properties == nullptr)
        _properties = new PropertyMap();
    PropertyMapIterator it = _properties->find(n);
    TypedObject *oldValue  = nullptr;
    if (it != _properties->end()) {
        oldValue = it->second;
    }
    _properties->insert(std::make_pair(n, v));
    return oldValue;
}

TypedObject *Object::addProperty(const PropertyId n, TypedObject *v) { return addProperty(getPropertyName(n), v); }

void Object::removeProperty(const std::string &n)
{
    if (_properties == nullptr)
        return;
    for (PropertyMap::iterator i = _properties->find(n); i != _properties->end(); i = _properties->find(n)) {
        delete i->second;
        _properties->erase(i);
    }
}

void Object::removeProperty(const PropertyId n) { removeProperty(getPropertyName(n)); }

bool Object::checkProperty(const std::string &n) const
{
    if (_properties == nullptr)
        return false;
    PropertyMap::const_iterator i = _properties->find(n);
    return i != _properties->end();
}

bool Object::checkProperty(const PropertyId n) const { return checkProperty(getPropertyName(n)); }

void Object::clearProperties()
{
    if (_properties == nullptr)
        return;
    for (PropertyMapIterator it = _properties->begin(); it != _properties->end(); ++it) {
        _properties->erase(it);
    }
}

Object::PropertyMapIterator Object::getPropertyBeginIterator()
{
    if (_properties == nullptr)
        _properties = new PropertyMap();
    return _properties->begin();
}

Object::PropertyMapIterator Object::getPropertyEndIterator()
{
    if (_properties == nullptr)
        _properties = new PropertyMap();
    return _properties->end();
}

bool Object::hasProperties() const { return (_properties != nullptr && !_properties->empty()); }

void Object::setSourceLineNumber(unsigned int i)
{
    if (_codeInfo == nullptr)
        _codeInfo = new CodeInfo();
    _codeInfo->lineNumber = i;
}

void Object::setSourceColumnNumber(unsigned int i)
{
    if (_codeInfo == nullptr)
        _codeInfo = new CodeInfo();
    _codeInfo->columnNumber = i;
}

unsigned int Object::getSourceLineNumber() const
{
    if (_codeInfo == nullptr)
        return 0;
    return _codeInfo->lineNumber;
}

unsigned int Object::getSourceColumnNumber() const
{
    if (_codeInfo == nullptr)
        return 0;
    return _codeInfo->columnNumber;
}

void Object::setSourceFileName(const std::string &f)
{
    if (_codeInfo == nullptr)
        _codeInfo = new CodeInfo();
    _codeInfo->filename = f;
}

std::string Object::getSourceFileName() const
{
    if (_codeInfo == nullptr)
        return std::string();
    return _codeInfo->filename;
}

const Object::CodeInfo &Object::getCodeInfo() const
{
    if (_codeInfo == nullptr) {
        CodeInfo **tmp = const_cast<CodeInfo **>(&_codeInfo);
        *tmp           = new CodeInfo();
    }

    return *_codeInfo;
}

void Object::setCodeInfo(const Object::CodeInfo &ci)
{
    if (_codeInfo == nullptr)
        _codeInfo = new CodeInfo();
    *_codeInfo = ci;
}

std::string Object::getSourceInfoString() const
{
    if (_codeInfo == nullptr)
        return "";
    return _codeInfo->getSourceInfoString();
}

bool Object::hasComments() const { return (_comments != nullptr && !_comments->empty()); }

void Object::addComment(const std::string &c)
{
    if (_comments == nullptr)
        _comments = new StringList();
    _comments->push_back(c);
}

void Object::addComments(const Object::StringList &cc)
{
    if (_comments == nullptr)
        _comments = new StringList();
    _comments->insert(_comments->end(), cc.begin(), cc.end());
}

void Object::removeComment(const std::string &c)
{
    if (_comments == nullptr)
        return;
    StringList::iterator it = std::find(_comments->begin(), _comments->end(), c);
    if (it == _comments->end())
        return;
    _comments->erase(it);
}

void Object::clearComment()
{
    if (_comments == nullptr)
        return;
    _comments->clear();
}

Object::StringList &Object::getComments()
{
    if (_comments == nullptr)
        _comments = new StringList();
    return *_comments;
}

bool Object::replace(Object *other)
{
    if (this->getParent() == nullptr)
        return false;

    if (this->isInBList()) {
        // Inside a BList
        BList<Object>::iterator i(this);
        if (other == nullptr) {
            i.remove();
        } else {
            if (other->_field != nullptr)
                *other->_field = nullptr;
            i = other;
        }
        return true;
    }

    if (this->_field == nullptr) {
        messageError("Hif replace: object field not set.", this, nullptr);
    }

    // Updating internal pointers to parent and field.
    *this->_field = other;
    if (other != nullptr) {
        other->_setParent(this->getParent());
        if (other->_field != nullptr)
            *other->_field = nullptr;
        other->_field = this->_field;
    }
    this->_field = nullptr;
    this->_setParent(nullptr);

    return true;
}

bool Object::replaceWithList(BList<Object> &list)
{
    if (!isInBList()) {
        messageError("Expected object in BList", this, nullptr);
    }

    if (list.empty()) {
        // Replace with empty list => replace with nothing.
        replace(nullptr);
        return true;
    }

    Object *where = list.front();
    BList<Object>::iterator rm(list.front());
    rm.remove();
    this->replace(where);

    BList<Object>::iterator start(where);
    while (!list.empty()) {
        Object *obj = list.back();
        BList<Object>::iterator rm2(list.back());
        rm2.remove();
        start.insert_after(obj);
    }

    return true;
}

const Object::Fields &Object::getFields()
{
    if (_fields == nullptr) {
        _fields = new Fields();
        _blists = new BLists();
        _calculateFields();
    }
    return *_fields;
}

const Object::BLists &Object::getBLists()
{
    if (_blists == nullptr) {
        _fields = new Fields();
        _blists = new BLists();
        _calculateFields();
    }
    return *_blists;
}

TypedObject *Object::getProperty(const std::string &n) const
{
    if (!checkProperty(n))
        return nullptr;
    return _properties->find(n)->second;
}

TypedObject *Object::getProperty(const PropertyId n) const { return getProperty(getPropertyName(n)); }

void *Object::_getParentLink() { return _parentlink; }

std::string Object::_getFieldName(const Object * /*child*/) const { return ""; }

std::string Object::_getBListName(const BList<Object> & /*list*/) const { return ""; }

void Object::_calculateFields()
{
    // ntd
}

Object *Object::_setChild(Object **field, Object *newObj)
{
    Object *&tmpField = *field;
    if (tmpField == newObj)
        return tmpField;

    if (newObj != nullptr) {
        // If newObj field is set, then it is inside another object,
        // therefore we have to remove it from old parent.
        // Otherwise if it is inside a BList we have to remove it.
        if (newObj->_field != nullptr)
            *newObj->_field = nullptr;
        if (newObj->isInBList()) {
            BList<Object>::iterator i(newObj);
            i.remove();
        }
        newObj->_field = &tmpField;
        newObj->_setParent(this);
    }

    Object *tmp = tmpField;
    tmpField    = newObj;
    if (tmp != nullptr && tmp->getParent() == this) {
        // Cleaning up pointer to parent and parent field.
        tmp->_field = nullptr;
        tmp->_setParent(nullptr);
    }

    return tmp;
}

std::string Object::getFieldName() const
{
    if (getParent() == nullptr)
        return "";
    if (isInBList())
        return "";
    return getParent()->_getFieldName(this);
}

std::string Object::getBListName(const BList<Object> &list) const { return _getBListName(list); }

template <typename T> std::string Object::getBListName(const BList<T> &list) const
{
    return getBListName(list.template toOtherBList<Object>());
}

// /////////////////////////////////////
// Explicit instantations
// /////////////////////////////////////

template <typename T> void Object::_setBListParent(BList<T> &p) { _setBListParent(p.template toOtherBList<Object>()); }

template <typename T> T *Object::setChild(T *&field, T *newObj)
{
    Object **tmp = reinterpret_cast<Object **>(&field);
    return static_cast<T *>(_setChild(tmp, static_cast<Object *>(newObj)));
}

template <typename T> void Object::_addField(T *&f) { _fields->push_back(reinterpret_cast<Object **>(&f)); }

template <typename T> void Object::_addBList(BList<T> &l) { _blists->push_back(reinterpret_cast<BList<Object> *>(&l)); }

/// @brief Defines a template method for setting the parent of a BList field.
#define HIF_TEMPLATE_METHOD(T) void Object::_setBListParent<T>(BList<T> & p)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

/// @brief Defines a template method for setting a child object in a specific field.
#define HIF_TEMPLATE_METHOD(T) T *Object::setChild<T>(T * &field, T * newObj)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

/// @brief Defines a template method for adding a field to the current object.
#define HIF_TEMPLATE_METHOD(T) void Object::_addField<T>(T * &f)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

/// @brief Defines a template method for adding a BList to the current object.
#define HIF_TEMPLATE_METHOD(T) void Object::_addBList<T>(BList<T> & l)
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

/// @brief Defines a template method for retrieving the name of a BList field.
#define HIF_TEMPLATE_METHOD(T) std::string Object::getBListName<T>(const BList<T> &l) const
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

/// @brief Defines a template method for retrieving the parent of the current object.
#define HIF_TEMPLATE_METHOD(T) T *Object::getParent<T>() const
HIF_INSTANTIATE_METHOD()
#undef HIF_TEMPLATE_METHOD

} // namespace hif
