/// @file NameTable.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdint>
#include <cstring>
#include <sstream>

#include <chrono>

#include "hif/application_utils/FileStructure.hpp"
#include "hif/hif.hpp"

namespace hif
{

namespace
{

const char *const name_none                  = "(no name)";
const char *const name_any                   = "(any name)";
const char *const name_hif_string_names      = "hif_string_names";
const char *const name_hif_globals           = "hif_globals";
const char *const name_hif_empty_string_name = "HIF_EMPTY_STRING";
const char *const name_hif_constructor       = "__hif_constructor";
const char *const name_hif_destructor        = "__hif_destructor";

} // namespace

NameTable::NameTable()
    : m_name_map()
    , fobbidden_name_list()
    , rng(static_cast<unsigned int>(std::time(NULL)))
    , dist(0.0F, 1.0F)
{
    m_name_map.insert(name_none);
    m_name_map.insert(name_any);
    m_name_map.insert(name_hif_string_names);
    m_name_map.insert(name_hif_empty_string_name);
}

NameTable *NameTable::getInstance()
{
    static NameTable instance;
    return &instance;
}

bool NameTable::setForbiddenListFromFile(std::string file_name, bool append)
{
    if (!append) {
        fobbidden_name_list.clear();
    }

    FILE *fpointer;
    char buffer[100];
    fpointer = fopen(file_name.c_str(), "r");
    if (fpointer == nullptr)
        return false;

    while (!feof(fpointer)) {
        char *str;
        str = fgets(buffer, 100, fpointer);
        if (str == nullptr)
            break;
        std::string tmpstr(str);
        // trim last character if is EOL
        if (tmpstr.at(tmpstr.length() - 1) == '\n')
            tmpstr = tmpstr.substr(0, tmpstr.length() - 1);
        fobbidden_name_list.insert(tmpstr);
    }
    // Closing the file:
    fclose(fpointer);
    return true;
}

void NameTable::printNameTable()
{
    printf("NameTable:\n");
    for (const auto &entry : m_name_map) {
        printf("    %s\n", entry.c_str());
    }
}

bool NameTable::nameExists(const std::string &name) { return m_name_map.count(name); }

std::string NameTable::getFreshName(const std::string &prefix)
{
    std::string _prefix = ((prefix.empty()) ? "hif" : prefix) + "_";
    std::uint64_t id    = 0;
    std::string name;
    do {
        name = _prefix + std::to_string(id++);
    } while (this->nameExists(name));
    registerName(name);
    return name;
}

std::string NameTable::getFreshName(const std::string &name, const std::string &suffix)
{
    return this->getFreshName(name + suffix);
}

std::string NameTable::getFreshName(const std::string &name, std::uint64_t suffix)
{
    return this->getFreshName(name + "_" + std::to_string(suffix));
}

std::string NameTable::registerName(const std::string &name)
{
    m_name_map.insert(name);
    return name;
}

std::string NameTable::registerName(const std::string &name, const int index)
{
    return this->registerName(name + "_" + std::to_string(index));
}

std::string NameTable::none() { return name_none; }

std::string NameTable::any() { return name_any; }

std::string NameTable::hifStringNames() { return name_hif_string_names; }

std::string NameTable::hifEmptyStringName() { return name_hif_empty_string_name; }

std::string NameTable::hifGlobals() { return name_hif_globals; }

std::string NameTable::hifConstructor() { return name_hif_constructor; }

std::string NameTable::hifDestructor() { return name_hif_destructor; }

} // namespace hif
