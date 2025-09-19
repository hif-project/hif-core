/// @file CommandLineParser.hpp
/// @brief Defines the CommandLineParser class for managing command line
/// arguments.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <map>
#include <string>
#include <vector>

#include "hif/application_utils/hifGetOpt.hpp"
#include "hif/application_utils/portability.hpp"
#include "hif/semantics/ILanguageSemantics.hpp"

namespace hif
{
namespace application_utils
{

/// @brief Base class for parsing a tool command line.
///
/// @details This class provides a base implementation for parsing command-line
/// arguments for a tool. It can be derived to customize behavior such as
/// handling additional options or commands.
class CommandLineParser
{
public:
    /// @name Traits.
    /// @{

    /// @brief Struct to hold parser configuration options.
    struct ParserOptions {
        bool printCommandLine = true; ///< Prints the command line before parsing if true.
        bool sortInputFiles   = true; ///< Sorts the list of input files if true.
    };

    /// @brief Struct to hold a command-line option.
    struct Option {
        bool hasArgument = false; ///< Indicates whether the option requires an argument.
        bool isActive    = false; ///< Indicates whether the option is active.
        std::string value;        ///< The value associated with the option.
        std::string description;  ///< A brief description of the option.
    };

    /// @brief Comparator for dictionary operations.
    struct DictionaryComparator {
        /// @brief Comparator function.
        /// @param x The first argument.
        /// @param y The second argument.
        /// @return True if `x` is less than `y`.
        constexpr inline bool operator()(const char &x, const char &y) const noexcept
        {
            const char xl = static_cast<char>(std::tolower(static_cast<int>(x)));
            const char yl = static_cast<char>(std::tolower(static_cast<int>(y)));
            if (xl < yl)
                return true;
            if (xl > yl)
                return false;
            return x < y;
        }
    };

    /// @brief Type alias for a list of string chunks.
    using Chunks          = std::vector<std::string>;

    /// @brief Type alias for a list of options.
    using OptionsList     = std::vector<option>;

    /// @brief Type alias for a list of input files.
    using Files           = std::vector<std::string>;

    /// @brief Maps short options to their corresponding long options.
    using Options         = std::map<char, Option, DictionaryComparator>;

    /// @brief Maps long option names to their corresponding short options.
    using Long2ShortNames = std::map<std::string, char>;

    /// @}

    CommandLineParser();

    CommandLineParser(const CommandLineParser &) = delete;

    CommandLineParser *operator=(const CommandLineParser &) = delete;

    virtual ~CommandLineParser() = default;

    /// @name Main methods.
    /// @{

    /// @brief Adds a command-line option.
    ///
    /// @param shortName The option's short name.
    /// @param longName The option's long name.
    /// @param hasArgument Whether the option requires an argument.
    /// @param isActive Whether the option is active.
    /// @param description A brief description of the option.
    /// @param defaultValue The default value for the option (optional).
    void addOption(
        const char shortName,
        const std::string &longName,
        bool hasArgument,
        bool isActive,
        const std::string &description,
        const std::string &defaultValue = "");

    /// @brief Sets the parsing options.
    /// @param opt the options.
    void setParserOptions(const ParserOptions &opt);

    /// @brief Returns the current parsing options.
    /// @return The options.
    const ParserOptions &getParserOptions() const;

    /// @brief Actual command line parsing method.
    ////
    /// @param argc The main() argc parameter.
    /// @param argv The main() argv parameter.
    ///
    void parse(int argc, char *argv[]);

    /// @brief Add tool infos for banner and help.
    ///
    /// @param toolName The tool name.
    /// @param copyright The copiright notice.
    /// @param toolDescription A brief tool description.
    /// @param synopsys The tool synopsys.
    /// @param notes Optional footnotes.
    void addToolInfos(
        const std::string &toolName,
        const std::string &copyright,
        const std::string &toolDescription,
        const std::string &synopsys,
        const std::string &notes);

    /// @brief Gets an option value.
    ///
    /// @param c The option.
    /// @return The associated value.
    const std::string &getOption(const char c) const;

    /// @brief Gets an option value.
    ///
    /// @param s The option.
    /// @return The associated value.
    const std::string &getOption(const std::string &s) const;

    /// @brief Gets an option as boolean value.
    ///
    /// @param c The option.
    /// @return True if passed by command line.
    bool isOptionFlagSet(const char c) const;

    /// @brief Gets an option as boolean value.
    ///
    /// @param s The option.
    /// @return True if passed by command line.
    bool isOptionFlagSet(const std::string &s) const;

    /// @brief Checks if given option is active in current version.
    ///
    /// @param c The option.
    /// @return True if active.
    bool isActiveOption(const char c) const;

    /// @brief Checks if given option is active in current version.
    ///
    /// @param s The option.
    /// @return True if active.
    bool isActiveOption(const std::string &s) const;

    /// @}

    /// @name Standard options.
    /// @{

    /// @brief Adds the help option to the command-line parser.
    void addHelp();

    /// @brief Adds the verbose option to the command-line parser.
    void addVerbose();

    /// @brief Adds the version option to the command-line parser.
    void addVersion();

    /// @brief Adds the output file option to the command-line parser.
    void addOutputFile();

    /// @brief Adds the output directory option to the command-line parser.
    void addOutputDirectory();

    /// @brief Adds the autostep option to the command-line parser.
    void addAutostep();

    /// @brief Adds the print-only option to the command-line parser.
    void addPrintOnly();

    /// @brief Adds the write-parsing option to the command-line parser.
    void addWriteParsing();

    /// @brief Adds the parse-only option to the command-line parser.
    void addParseOnly();

    /// @brief Adds the configuration file option to the command-line parser.
    /// @param generateStub add option to generate a stub config file with given file name.
    void addConfigFile(bool generateStub);

    /// @brief Adds the optimization option to the command-line parser.
    void addOptimization();

    /// @brief Adds the top-level design unit option to the command-line parser.
    void addTopLevel();

    /// @brief Adds the clock option to the command-line parser.
    void addClock();

    /// @brief Adds the reset option to the command-line parser.
    void addReset();

    /// @}

    /// @name Support methods.
    /// @{

    /// @brief Retrieves the list of input files.
    /// @return A reference to the list of input files.
    Files &getFiles();

    /// @brief Prints the help message and exits the program.
    void printHelp() const;

    /// @brief Prints the tool version and exits the program.
    void printVersion() const;

    /// @brief Checks whether verbose mode is enabled.
    /// @return True if verbose mode is enabled, false otherwise.
    bool isVerbose() const;

    /// @brief Retrieves the output file name.
    /// @return A reference to the output file name string.
    const std::string &getOutputFile() const;

    /// @brief Retrieves the output directory name.
    /// @return A reference to the output directory string.
    const std::string &getOutputDirectory() const;

    /// @brief Checks whether autostep mode is enabled.
    /// @return True if autostep mode is enabled, false otherwise.
    bool isAutostep() const;

    /// @brief Checks whether print-only mode is enabled.
    /// @return True if print-only mode is enabled, false otherwise.
    bool isPrintOnly() const;

    /// @brief Checks whether write-parsing mode is enabled.
    /// @return True if write-parsing mode is enabled, false otherwise.
    bool isWriteParsing() const;

    /// @brief Checks whether parse-only mode is enabled.
    /// @return True if parse-only mode is enabled, false otherwise.
    bool isParseOnly() const;

    /// @brief Retrieves the configuration file name.
    /// @return A reference to the configuration file name string.
    const std::string &getConfigFile() const;

    /// @brief Checks whether optimization is enabled.
    /// @return True if optimization is enabled, false otherwise.
    bool isOptimized() const;

    /// @brief Retrieves the name of the top-level design unit.
    /// @return A reference to the top-level design unit name string.
    const std::string &getTopLevel() const;

    /// @brief Retrieves the top-level design unit from the system.
    ///
    /// @param system The system to search for the top-level design unit.
    /// @param sem The language semantics to use for searching.
    /// @return A pointer to the top-level design unit.
    hif::DesignUnit *getTopLevelDesignUnit(hif::System *system, hif::semantics::ILanguageSemantics *sem);

    /// @brief Retrieves the file name for generating a configuration file.
    /// @return A reference to the configuration file name string.
    const std::string &getGenerateConfigFile() const;

    /// @brief Retrieves the name of the top-level clock signal.
    /// @return A reference to the clock signal name string.
    const std::string &getClock() const;

    /// @brief Retrieves the name of the top-level reset signal.
    /// @return A reference to the reset signal name string.
    const std::string &getReset() const;

    /// @}

protected:
    /// @name Internal methods.
    /// @{

    /// @brief Format a string as a line of the help.
    ///
    /// @param s The string.
    /// @return The formatted string.
    std::string _makeLine(const std::string &s = "") const;

    /// @brief Formats the parameters as a synopsys description of the help.
    ///
    /// @param shortName The command short name.
    /// @param longName The command long name.
    /// @param description The command description.
    /// @param margin Additional margin when wrapping. Default is 0.
    /// @return The formatted entry string.
    std::string _formatLine(
        const char shortName,
        const std::string &longName,
        const std::string &description,
        const std::size_t margin = 0) const;

    /// @brief Splits a string into chunks for the help.
    ///
    /// @param chunks The filled shunk list.
    /// @param s The string to be split.
    /// @param maxSize THe maximum size.
    /// @param margin Additional margin when wrapping. Default is 0.
    static void _makeChunks(Chunks &chunks, std::string s, const std::size_t maxSize, const std::size_t margin = 0);

    /// @brief Pads given string to fix exactely maxLineSize.
    ///
    /// @param s The string.
    /// @param maxLineSize The line size.
    /// @return The padded string.
    static std::string _padString(const std::string &s, const std::size_t maxLineSize);

    /// @}

    OptionsList _optionsList;       ///< List of available command-line options.
    Options _options;               ///< Parsed command-line options.
    Long2ShortNames _long2short;    ///< Mapping from long option names to short option names.
    std::string _format;            ///< Format string for displaying help or usage information.
    Files _files;                   ///< List of input or output files specified in the command line.
    std::size_t _maxLineSize;       ///< Maximum line size for output formatting.
    const std::string _indentation; ///< Indentation string used for formatted output.
    std::string _toolName;          ///< The name of the tool.
    std::string _toolDescription;   ///< Description of the tool's functionality.
    std::string _copyright;         ///< Copyright information for the tool.
    std::string _synopsys;          ///< Synopsis of the tool's usage.
    std::string _notes;             ///< Additional notes or information about the tool.
    DesignUnit *_topLevelDu;        ///< Pointer to the top-level design unit, if applicable.
    ParserOptions _parserOptions;   ///< Options specific to the command-line parser behavior.
};

} // namespace application_utils
} // namespace hif
