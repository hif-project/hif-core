/// @file NameTable.hpp
/// @brief Name table management for symbol lookup and storage in HIF.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <ctime>
#include <list>
#include <random>
#include <set>
#include <string>

#include <unordered_map>

#include "hif/application_utils/portability.hpp"

namespace hif
{

///	@brief This class is a table containing all names.
///	@details
/// This class contains all names (identifiers) used in Hif system descriptions.
/// As names are unique in the table, they can be compared using == and != (and
/// comparison costs a pointer comparison.
class NameTable
{
public:
    using NameMap        = std::set<std::string>;
    using ForbiddenNames = std::set<std::string>;

    /// Singleton stuff.
    /// @brief Function thats return an instance to NameTable class.
    static NameTable *getInstance();

    /// @brief Sets the List of reserved names, that should not be converted in uppercase or lowercase
    /// @param file_name the filename (full_path) that contains reserved names. It should be formatted as follow:
    ///		one name for lines (whitout spaces)
    /// @param append boolean that indicates if append.
    /// @returns a boolean indicating if the file was correctly loaded into nametable.
    bool setForbiddenListFromFile(std::string file_name, bool append = false);

    /// @brief Print the NameTable content
    void printNameTable();

    /// @brief Return the name associated to a given string.
    /// @param name the string whose name is sought.
    bool nameExists(const std::string &name);

    /// @brief Return a fresh name.
    /// A name is fresh if it does not occur in the file.
    /// This function returns a name containing exactly the prefix if
    /// - a prefix is given
    /// - prefix is not already present
    /// .
    /// Otherwise it generates a fresh name using the prefix string as prefix.
    /// @param prefix [optional] the string prefix to be used to generate a fresh name
    /// @return the fresh name.
    std::string getFreshName(const std::string &prefix = std::string());

    /// @brief Return a fresh name.
    /// A name is fresh if it does not occur in the file.
    /// @param n The old name.
    /// @param suffix [optional] the string prefix to be used to generate a fresh name
    /// @return The fresh name.
    std::string getFreshName(const std::string &name, const std::string &suffix);

    /// @brief Return a fresh name.
    /// A name is fresh if it does not occur in the file.
    /// @param n The old name.
    /// @param suffix the string suffix to be used to generate a fresh name
    /// @return The fresh name.
    std::string getFreshName(const std::string &name, unsigned long long suffix);

    /// @brief Return the name associated to a given string.
    /// This one creates the name if it is not in the table yet.
    /// @param s the string whose name is sought.
    /// @return The name.
    std::string registerName(const std::string &name);

    /// @brief Return the name associated to a given string.
    /// This one creates the name if it is not in the table yet.
    /// @param s the string whose name is sought.
    /// @param index The index to concat at the end of @p s.
    /// @return The name.
    std::string registerName(const std::string &name, const int index);

    /// @brief Return the special "none" name.
    /// @return return m_none value.
    static std::string none();

    /// @brief Return the special "any" name.
    /// @return return m_any value.
    static std::string any();

    /// @brief Return the special "hif_string_names" name.
    /// @return return hif_string_names.
    static std::string hifStringNames();

    /// @brief Returns the special name to signal a empty string.
    /// @return The string.
    static std::string hifEmptyStringName();

    /// @brief Returns the special name to global HIF library.
    /// @return The string.
    static std::string hifGlobals();

    /// @brief Returns the special name "__hif_constructor" name.
    /// @return The string.
    static std::string hifConstructor();

    /// @brief Returns the special name "__hif_destructor" name.
    /// @return The string.
    static std::string hifDestructor();

    static bool isDefaultValue(const std::string &name) { return name == none(); }

private:
    NameMap m_name_map;
    ForbiddenNames fobbidden_name_list;

    /// Mersenne Twister random engine.
    std::mt19937 rng;
    /// Uniform distribution in [0, 1).
    std::uniform_real_distribution<double> dist;

    /// @brief Default Constructor
    NameTable();

    /// @brief Default Destructor
    ~NameTable() = default;

    /// @brief Copy constructor
    NameTable(NameTable &) = delete;

    /// @brief Assignment operator
    NameTable &operator=(NameTable &) = delete;
};

/// Macro to retrieve the "none" name from the NameTable singleton.
#define NAME_NONE NameTable::getInstance()->none()

/// Macro to retrieve the "any" name from the NameTable singleton.
#define NAME_ANY NameTable::getInstance()->any()

} // namespace hif
