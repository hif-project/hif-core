/// @file Declaration.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>

#include "hif/classes/Declaration.hpp"

namespace hif
{

Declaration::Declaration()
    : _additionalKeywords(nullptr)
{
    // ntd
}

Declaration::~Declaration() { delete _additionalKeywords; }

void Declaration::_calculateFields() { Object::_calculateFields(); }

Object *Declaration::toObject() { return this; }

void Declaration::addAdditionalKeyword(const std::string &kw)
{
    if (_additionalKeywords == nullptr)
        _additionalKeywords = new KeywordList();
    KeywordList::iterator it = std::find(_additionalKeywords->begin(), _additionalKeywords->end(), kw);
    if (it != _additionalKeywords->end())
        return;
    _additionalKeywords->push_back(kw);
}

void Declaration::removeAdditionalKeyword(const std::string &kw)
{
    if (_additionalKeywords == nullptr)
        return;
    KeywordList::iterator it = std::find(_additionalKeywords->begin(), _additionalKeywords->end(), kw);
    if (it == _additionalKeywords->end())
        return;
    _additionalKeywords->erase(it);
}

bool Declaration::hasAdditionalKeywords() { return (_additionalKeywords != nullptr && !_additionalKeywords->empty()); }

bool Declaration::checkAdditionalKeyword(const std::string &kw)
{
    if (_additionalKeywords == nullptr)
        return false;
    KeywordList::iterator it = std::find(_additionalKeywords->begin(), _additionalKeywords->end(), kw);
    return it != _additionalKeywords->end();
}

void Declaration::clearAdditionalKeywords()
{
    if (_additionalKeywords != nullptr) {
        _additionalKeywords->clear();
    }
}

Declaration::KeywordList::iterator Declaration::getAdditionalKeywordsBeginIterator()
{
    if (_additionalKeywords == nullptr)
        _additionalKeywords = new KeywordList();
    return _additionalKeywords->begin();
}

Declaration::KeywordList::iterator Declaration::getAdditionalKeywordsEndIterator()
{
    if (_additionalKeywords == nullptr)
        _additionalKeywords = new KeywordList();
    return _additionalKeywords->end();
}

} // namespace hif
