/// @file LibraryDef.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Library.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/LibraryDef.hpp"

namespace hif
{

LibraryDef::LibraryDef()
    : libraries()
    , declarations()
    , _languageID(rtl)
    , _isStandard(false)
    , _hasCLinkage(false)
{
    _setBListParent(libraries);
    _setBListParent(declarations);

    // ntd
}

LibraryDef::~LibraryDef()
{
    // ntd
}

bool LibraryDef::isStandard() const { return _isStandard; }

void LibraryDef::setStandard(bool standard) { _isStandard = standard; }

void LibraryDef::setCLinkage(bool cLinkage) { _hasCLinkage = cLinkage; }

bool LibraryDef::hasCLinkage() const { return _hasCLinkage; }

ClassId LibraryDef::getClassId() const { return CLASSID_LIBRARYDEF; }

int LibraryDef::acceptVisitor(HifVisitor &vis) { return vis.visitLibraryDef(*this); }

void LibraryDef::_calculateFields()
{
    Scope::_calculateFields();

    _addBList(libraries);
    _addBList(declarations);
}

std::string LibraryDef::_getBListName(const BList<Object> &list) const
{
    if (&list == &libraries.toOtherBList<Object>())
        return "libraries";
    if (&list == &declarations.toOtherBList<Object>())
        return "declarations";
    return Scope::_getBListName(list);
}

hif::LanguageID LibraryDef::getLanguageID() const { return _languageID; }

hif::LanguageID LibraryDef::setLanguageID(const hif::LanguageID language_id)
{
    LanguageID prev = _languageID;
    _languageID     = language_id;
    return prev;
}

} // namespace hif
