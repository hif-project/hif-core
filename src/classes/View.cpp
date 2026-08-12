/// @file View.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Library.hpp"
#include "hif/classes/ViewReference.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/classes/Contents.hpp"
#include "hif/classes/Entity.hpp"
#include "hif/classes/View.hpp"

namespace hif
{

View::View()
    : libraries()
    , templateParameters()
    , declarations()
    , inheritances()
    , _contents(nullptr)
    , _entity(nullptr)
    , _languageID(rtl)
    , _isStandard(false)
    , _filename()
{
    _setBListParent(libraries);
    _setBListParent(templateParameters);
    _setBListParent(declarations);
    _setBListParent(inheritances);
}

View::~View()
{
    delete (_entity);
    delete (_contents);
}

Entity *View::getEntity() const { return _entity; }

Entity *View::setEntity(Entity *x) { return setChild(_entity, x); }

Contents *View::getContents() const { return _contents; }

Contents *View::setContents(Contents *x) { return setChild(_contents, x); }

const std::string &View::getFilename() const { return _filename; }

void View::setFilename(const std::string &v) { _filename = v; }

ClassId View::getClassId() const { return CLASSID_VIEW; }

int View::acceptVisitor(HifVisitor &vis) { return vis.visitView(*this); }

void View::_calculateFields()
{
    Scope::_calculateFields();

    _addField(_entity);
    _addField(_contents);
    _addBList(libraries);
    _addBList(templateParameters);
    _addBList(declarations);
    _addBList(inheritances);
}

std::string View::_getFieldName(const Object *child) const
{
    if (child == _entity)
        return "entity";
    if (child == _contents)
        return "contents";
    return Scope::_getFieldName(child);
}

std::string View::_getBListName(const BList<Object> &list) const
{
    if (&list == &libraries.toOtherBList<Object>())
        return "libraries";
    if (&list == &templateParameters.toOtherBList<Object>())
        return "templateParameters";
    if (&list == &declarations.toOtherBList<Object>())
        return "declarations";
    if (&list == &inheritances.toOtherBList<Object>())
        return "inheritances";
    return Scope::_getBListName(list);
}

hif::LanguageID View::getLanguageID() const { return _languageID; }

hif::LanguageID View::setLanguageID(const hif::LanguageID languageID)
{
    hif::LanguageID prev = _languageID;
    _languageID          = languageID;
    return prev;
}

bool View::isStandard() const { return _isStandard; }

void View::setStandard(bool standard) { _isStandard = standard; }

} // namespace hif
