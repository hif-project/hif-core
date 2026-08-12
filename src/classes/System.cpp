/// @file System.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include "hif/classes/Action.hpp"
#include "hif/classes/DesignUnit.hpp"
#include "hif/classes/Library.hpp"
#include "hif/classes/LibraryDef.hpp"

#include "hif/HifVisitor.hpp"
#include "hif/application_utils/application_utils.hpp"
#include "hif/classes/System.hpp"

/// @brief The major version of the XML format.
#define HIF_XML_FORMAT_VERSION_INFO_MAJOR 5

/// @brief The minor version of the XML format.
#define HIF_XML_FORMAT_VERSION_INFO_MINOR 0

namespace hif
{

System::System()
    : libraryDefs()
    , designUnits()
    , declarations()
    , libraries()
    , actions()
    , _version()
    , _languageID(hif::rtl)
{
    _setBListParent(libraryDefs);
    _setBListParent(designUnits);
    _setBListParent(declarations);
    _setBListParent(libraries);
    _setBListParent(actions);
}

System::~System()
{
    // ntd
}

void System::setVersionInfo(const VersionInfo &info) { _version = info; }

System::VersionInfo System::getVersionInfo() const { return _version; }

System::VersionInfo::VersionInfo()
    : release(hif::application_utils::getHIFVersion())
    , tool("")
    , generationDate("")
    , formatVersionMajor(HIF_XML_FORMAT_VERSION_INFO_MAJOR)
    , formatVersionMinor(HIF_XML_FORMAT_VERSION_INFO_MINOR)
{
    // ntd
}

System::VersionInfo::~VersionInfo()
{
    // ntd
}

System::VersionInfo::VersionInfo(const VersionInfo &other)
    : release(other.release)
    , tool(other.tool)
    , generationDate(other.generationDate)
    , formatVersionMajor(other.formatVersionMajor)
    , formatVersionMinor(other.formatVersionMinor)
{
    // ntd
}

System::VersionInfo &System::VersionInfo::operator=(VersionInfo other)
{
    swap(other);
    return *this;
}

void System::VersionInfo::swap(VersionInfo &other)
{
    release.swap(other.release);
    tool.swap(other.tool);
    generationDate.swap(other.generationDate);
    std::swap(formatVersionMajor, other.formatVersionMajor);
    std::swap(formatVersionMinor, other.formatVersionMinor);
}

ClassId System::getClassId() const { return CLASSID_SYSTEM; }

int System::acceptVisitor(HifVisitor &vis) { return vis.visitSystem(*this); }

void System::_calculateFields()
{
    Scope::_calculateFields();

    _addBList(libraryDefs);
    _addBList(designUnits);
    _addBList(declarations);
    _addBList(libraries);
    _addBList(actions);
}

std::string System::_getBListName(const BList<Object> &list) const
{
    if (&list == &libraryDefs.toOtherBList<Object>())
        return "libraryDefs";
    if (&list == &designUnits.toOtherBList<Object>())
        return "designUnits";
    if (&list == &declarations.toOtherBList<Object>())
        return "declarations";
    if (&list == &libraries.toOtherBList<Object>())
        return "libraries";
    if (&list == &actions.toOtherBList<Object>())
        return "actions";
    return Scope::_getBListName(list);
}

hif::LanguageID System::getLanguageID() const { return _languageID; }

hif::LanguageID System::setLanguageID(hif::LanguageID languageID)
{
    LanguageID prev = _languageID;
    _languageID     = languageID;
    return prev;
}

} // namespace hif
