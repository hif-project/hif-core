/// @file ConfigurationManager.hpp
/// @brief Header file defining the ConfigurationManager class for parsing,
/// managing, and writing structured configuration files.
/// @details The ConfigurationManager class provides functionality to handle
/// configuration files with sections, directives, variables, and comments. It
/// supports reading and writing configuration files, modifying their contents
/// programmatically, and performing validation checks.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>

#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace application_utils
{
/// @brief Parser for configuration files.
///
/// Configuration files follow this format:
/// @code
/// # Comment spanning till the end of the line.
/// [section of variables]
/// ;directive = value1 ... valueN
/// variable = value1 ... valueN
/// @endcode
///
/// This class can read from and write to such files, supporting sections,
/// directives, variables, and comments.
class ConfigurationManager
{
public:
    /// @brief Stores data for a single directive or variable.
    struct KeyValues {
        std::vector<std::string> values;   ///< List of associated values.
        std::vector<std::string> comments; ///< List of associated comments.
    };

    /// @brief Map for storing directives within a section.
    using Directives = std::map<std::string, KeyValues>;
    /// @brief Map for storing variables within a section.
    using Variables  = std::map<std::string, KeyValues>;

    /// @brief Stores data for a single section.
    struct SectionData {
        Directives directives;             ///< Map of directives in this section.
        Variables variables;               ///< Map of variables in this section.
        std::vector<std::string> comments; ///< List of comments associated with this section.
    };

    /// @brief Map for storing sections in the configuration file.
    using Sections        = std::map<std::string, SectionData>;
    /// @brief List for maintaining the order of sections as they appear in the file.
    using OrderedSections = std::list<std::string>;

    ConfigurationManager() = default; ///< Default constructor.
    virtual ~ConfigurationManager();  ///< Destructor.

    /// @name Accessors
    /// @{

    /// @brief Parses a configuration file.
    /// @param file The name of the file to parse.
    /// @return False on error.
    auto parse(const std::string &file) -> bool;

    /// @brief Adds an empty section to the configuration.
    /// @param section The name of the section.
    void addSection(const std::string &section);

    /// @brief Adds a value to a section under a specific identifier.
    /// @param section The section name.
    /// @param id The identifier for the value.
    /// @param value The value to add.
    /// @param isDirective True if the identifier is a directive, false otherwise.
    void
    addValue(const std::string &section, const std::string &id, const std::string &value, bool isDirective = false);

    /// @brief Adds multiple values to a section under a specific identifier.
    /// @param section The section name.
    /// @param id The identifier for the values.
    /// @param values A list of values to add.
    /// @param isDirective True if the identifier is a directive, false otherwise.
    void addValues(
        const std::string &section,
        const std::string &id,
        const std::vector<std::string> &values,
        bool isDirective = false);

    /// @brief Sets a value in a section under a specific identifier.
    /// @param section The section name.
    /// @param id The identifier for the value.
    /// @param value The value to set.
    /// @param isDirective True if the identifier is a directive, false otherwise.
    void
    setValue(const std::string &section, const std::string &id, const std::string &value, bool isDirective = false);

    /// @brief Sets multiple values in a section under a specific identifier.
    /// @param section The section name.
    /// @param id The identifier for the values.
    /// @param values A list of values to set.
    /// @param isDirective True if the identifier is a directive, false otherwise.
    void setValues(
        const std::string &section,
        const std::string &id,
        const std::vector<std::string> &values,
        bool isDirective = false);

    /// @brief Retrieves the last value associated with an identifier in a section.
    /// @param section The section name.
    /// @param id The identifier.
    /// @param isDirective True if the identifier is a directive, false otherwise.
    /// @return The last value associated with the identifier.
    /// @throw std::out_of_range If the section or identifier does not exist.
    auto getValue(const std::string &section, const std::string &id, bool isDirective = false) -> std::string &;

    /// @brief Retrieves the last value associated with an identifier in a section.
    /// @param section The section name.
    /// @param id The identifier.
    /// @param isDirective True if the identifier is a directive, false otherwise.
    /// @return The last value associated with the identifier.
    /// @throw std::out_of_range If the section or identifier does not exist.
    auto
    getValue(const std::string &section, const std::string &id, bool isDirective = false) const -> const std::string &;

    /// @brief Retrieves all values associated with an identifier in a section.
    /// @param section The section name.
    /// @param id The identifier.
    /// @param isDirective True if the identifier is a directive, false otherwise.
    /// @return A list of values associated with the identifier.
    /// @throw std::out_of_range If the section or identifier does not exist.
    auto getValues(const std::string &section, const std::string &id, bool isDirective = false)
        -> std::vector<std::string> &;

    /// @brief Retrieves all values associated with an identifier in a section.
    /// @param section The section name.
    /// @param id The identifier.
    /// @param isDirective True if the identifier is a directive, false otherwise.
    /// @return A list of values associated with the identifier.
    /// @throw std::out_of_range If the section or identifier does not exist.
    auto getValues(const std::string &section, const std::string &id, bool isDirective = false) const
        -> const std::vector<std::string> &;

    /// @brief Checks if a section exists.
    /// @param section The section name.
    /// @return True if the section exists, false otherwise.
    auto hasSection(const std::string &section) const -> bool;

    /// @brief Checks if a directive exists in a section.
    /// @param section The section name.
    /// @param dir The directive name.
    /// @return True if the directive exists, false otherwise.
    auto hasDirective(const std::string &section, const std::string &dir) const -> bool;

    /// @brief Checks if a variable exists in a section.
    /// @param section The section name.
    /// @param var The variable name.
    /// @return True if the variable exists, false otherwise.
    auto hasVariable(const std::string &section, const std::string &var) const -> bool;

    /// @brief Deletes a section and all its contents.
    /// @param section The section name.
    void eraseSection(const std::string &section);

    /// @brief Deletes a directive from a section.
    /// @param section The section name.
    /// @param dir The directive name.
    void eraseDirective(const std::string &section, const std::string &dir);

    /// @brief Deletes a variable from a section.
    /// @param section The section name.
    /// @param var The variable name.
    void eraseVariable(const std::string &section, const std::string &var);

    /// @brief Writes the current configuration to a file.
    /// @param file The output file name.
    /// @return False on error.
    auto writeFile(const std::string &file) const -> bool;

    /// @brief Adds a comment at the beginning of the file.
    /// @param comment The comment text.
    void addComment(const std::string &comment);

    /// @brief Adds a comment to a section.
    /// @param section The section name.
    /// @param comment The comment text.
    void addComment(const std::string &section, const std::string &comment);

    /// @brief Adds a comment to a variable or directive.
    /// @param section The section name.
    /// @param identifier The identifier (variable or directive).
    /// @param comment The comment text.
    /// @param isDirective True if the identifier is a directive, false otherwise.
    void addComment(
        const std::string &section,
        const std::string &identifier,
        const std::string &comment,
        bool isDirective = false);
    /// @}

protected:
    /// @name Validity Checks
    /// @{

    /// @brief Validates the configuration after parsing.
    /// @return False if validation fails.
    virtual auto _validateInput() -> bool;

    /// @brief Validates the configuration before writing.
    /// @return False if validation fails.
    virtual auto _validateOutput() const -> bool;
    /// @}

    /// @name Parsing Support
    /// @{

    /// @brief Parses a single line of the configuration file.
    /// @param line The line to parse.
    /// @return False on error.
    auto _parseLine(const std::string &line) -> bool;

    /// @brief Parses a comment line.
    /// @param s The comment line.
    /// @return False on error.
    static auto _parseComment(const std::string &s) -> bool;

    /// @brief Parses a section header.
    /// @param s The section header.
    /// @return False on error.
    auto _parseSection(const std::string &s) -> bool;

    /// @brief Parses a directive line.
    /// @param s The directive line.
    /// @return False on error.
    auto _parseDirective(const std::string &s) -> bool;

    /// @brief Parses a variable line.
    /// @param s The variable line.
    /// @return False on error.
    auto _parseVariable(const std::string &s) -> bool;

    /// @brief Parses values associated with a key.
    /// @param key The key to associate the values with.
    /// @param s The string containing the values.
    /// @param isDirective True if the key is a directive, false otherwise.
    void _parseValues(const std::string &key, const std::string &s, bool isDirective);

    /// @}

    Sections _sections;               ///< Map of sections in the configuration.
    std::string _currentSection;      ///< Current section being processed.
    OrderedSections _orderedSections; ///< List for maintaining section order.
};

} // namespace application_utils
} // namespace hif
