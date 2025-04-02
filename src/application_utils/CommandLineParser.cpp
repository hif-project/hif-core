/// @file CommandLineParser.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>
#include <cstdlib>
#include <cstring>

#include "hif/application_utils/CommandLineParser.hpp"
#include "hif/application_utils/Log.hpp"
#include "hif/application_utils/dumpVersion.hpp"
#include "hif/manipulation/manipulation.hpp"

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

#ifdef NDEBUG
#    define HIF_IS_ACTIVE false
#else
#    define HIF_IS_ACTIVE true
#endif

// /////////////////////////////////////////////////////////////////////////////
// ParserOptions
// /////////////////////////////////////////////////////////////////////////////

namespace hif
{

namespace application_utils
{

CommandLineParser::CommandLineParser()
    : _optionsList()
    , _options()
    , _long2short()
    , _format()
    , _files()
    , _maxLineSize(80)
    , _indentation("      ")
    , _toolName()
    , _toolDescription()
    , _copyright()
    , _synopsys()
    , _notes()
    , _topLevelDu(nullptr)
    , _parserOptions()
{
    // ntd
}

// ////////////////////////////////////////////////////////////
// Main methods.
// ////////////////////////////////////////////////////////////
void CommandLineParser::addOption(
    const char shortName,
    const std::string &longName,
    bool hasArgument,
    bool isActive,
    const std::string &description,
    const std::string &defaultValue)
{
    // Sanity check in debug config only:
    messageDebugAssert(
        _options.find(shortName) == _options.end(), std::string("Option '") + shortName + "' already in use.", nullptr,
        nullptr);

    // Insert the longName and shortName into the map
    auto [it, inserted] = _long2short.insert(std::make_pair(longName, shortName));

    if (!inserted) {
        messageError("Failed to add option.\n", nullptr, nullptr);
        std::exit(1);
    }

    // Adding the new option for parsing:
    struct option opt = {
        it->first.c_str(),
        hasArgument ? hif_required_argument : hif_no_argument,
        nullptr,
        shortName,
    };
    _optionsList.push_back(opt);

    // Adding the new option format for parsing:
    _format += (shortName + (hasArgument ? std::string(":") : std::string()));

    // Saving option infos:
    Option &o     = _options[shortName];
    o.hasArgument = hasArgument;
    o.isActive    = isActive;
    o.value       = defaultValue;

    // Adding option to the help:
    o.description = _formatLine(shortName, longName, description);
}

void CommandLineParser::setParserOptions(const CommandLineParser::ParserOptions &opt) { _parserOptions = opt; }

auto CommandLineParser::getParserOptions() const -> const CommandLineParser::ParserOptions & { return _parserOptions; }
void CommandLineParser::parse(int argc, char *argv[])
{
    if (_parserOptions.printCommandLine) {
        std::string s(argv[0]);
        for (int i = 1; i < argc; ++i) {
            s += std::string(" ") + argv[i];
        }
        messageInfo("Running command:\n    " + s + "\n");
    }

    if (argc == 1) {
        printHelp();
    }
    struct option terminatorOption = {
        nullptr,
        0,
        nullptr,
        0,
    };
    _optionsList.push_back(terminatorOption);

    int optionIndex = 0;
    char c          = 0;
    while ((c = static_cast<char>(hif_getopt_long(argc, argv, _format.c_str(), _optionsList.data(), &optionIndex))) !=
           -1) {
        auto it(_options.find(c));
        if (it == _options.end() || !it->second.isActive) {
            messageError(
                "Not allowed usage or not available option.\n \
         		Try to run with '--help' for more information.",
                nullptr, nullptr);
        }

        if (!it->second.hasArgument) {
            it->second.value = "true";
            continue;
        }

        if (hif_optarg == nullptr) {
            messageError((std::string("Option '-") + c + "' requires an argument."), nullptr, nullptr);
        }

        it->second.value = hif_optarg;
    }

    // Retrieve optional files list
    for (int index = hif_optind; index < argc; ++index) {
        _files.emplace_back(argv[index]);
    }

    if (_parserOptions.sortInputFiles) {
        std::sort(_files.begin(), _files.end());
    }
}
void CommandLineParser::addToolInfos(
    const std::string &toolName,
    const std::string &copyright,
    const std::string &toolDescription,
    const std::string &synopsys,
    const std::string &notes)
{
    _toolName        = toolName;
    _copyright       = copyright;
    _toolDescription = toolDescription;
    _synopsys        = synopsys;
    _notes           = notes;
}
auto CommandLineParser::getOption(const char c) const -> const std::string &
{
    messageDebugAssert(_options.find(c) != _options.end(), std::string("Unregistered option: ") + c, nullptr, nullptr);
    // By using [] we assure eventual creation of "value" (which is a string).
    // But no [] const is provided, so we must use a const cast:
    auto &opt = const_cast<Options &>(_options);
    return opt[c].value;
}

auto CommandLineParser::getOption(const std::string &s) const -> const std::string &
{
    auto it = _long2short.find(s);

    // Here in case of not found, we cannot return a const ref to an empty
    // string since we do not have such a string...
    // So we must go into error!
    messageAssert(it != _long2short.end(), std::string("Unregistered option: ") + s, nullptr, nullptr);

    const char c = it->second;
    return getOption(c);
}

auto CommandLineParser::isOptionFlagSet(const char c) const -> bool
{
    messageDebugAssert(_options.find(c) != _options.end(), std::string("Unregistered option: ") + c, nullptr, nullptr);
    if (!isActiveOption(c)) {
        return false;
    }

    return !getOption(c).empty();
}

auto CommandLineParser::isOptionFlagSet(const std::string &s) const -> bool
{
    messageDebugAssert(
        _long2short.find(s) != _long2short.end(), std::string("Unregistered option: ") + s, nullptr, nullptr);
    const char c = _long2short.find(s)->second;
    return isOptionFlagSet(c);
}

auto CommandLineParser::isActiveOption(const char c) const -> bool
{
    auto it = _options.find(c);
    messageDebugAssert(it != _options.end(), std::string("Unregistered option: ") + c, nullptr, nullptr);
    return it->second.isActive;
}

auto CommandLineParser::isActiveOption(const std::string &s) const -> bool
{
    messageDebugAssert(
        _long2short.find(s) != _long2short.end(), std::string("Unregistered option: ") + s, nullptr, nullptr);
    const char c = _long2short.find(s)->second;
    return isActiveOption(c);
}
// ////////////////////////////////////////////////////////////
// Standard options.
// ////////////////////////////////////////////////////////////
void CommandLineParser::addHelp() { addOption('h', "help", false, true, "Prints this help.", ""); }

void CommandLineParser::addVerbose() { addOption('V', "verbose", false, true, "Enables verbose output.", ""); }

void CommandLineParser::addVersion() { addOption('v', "version", false, true, "Prints tool version.", ""); }

void CommandLineParser::addOutputFile() { addOption('o', "output", true, true, "Specifies the output file name.", ""); }

void CommandLineParser::addOutputDirectory()
{
    addOption('D', "directory", true, true, "Specifies the output directory.", "");
}

void CommandLineParser::addAutostep()
{
    addOption(
        'a', "autostep", false, HIF_IS_ACTIVE, "Starts the tool from the phase matching the input file name.", "");
}

void CommandLineParser::addPrintOnly()
{
    addOption('p', "printonly", false, HIF_IS_ACTIVE, "Prints the HIF tree without any manipulation.", "");
}

void CommandLineParser::addWriteParsing()
{
    addOption('w', "writeparsing", false, HIF_IS_ACTIVE, "Prints infos related to parsing steps.", "");
}

void CommandLineParser::addParseOnly()
{
    addOption('P', "parseonly", false, HIF_IS_ACTIVE, "Only parses input files without building the HIF tree.", "");
}

void CommandLineParser::addConfigFile(bool generateStub)
{
    addOption('C', "config", true, true, "Specifies input configuration file.", "");
    if (generateStub) {
        addOption('G', "generateConfig", true, true, "Generates a stub config file with given file name.", "");
    }
}

void CommandLineParser::addOptimization()
{
    addOption('O', "optimization", false, true, "Generates optimized code at the expense of readability.", "");
}

void CommandLineParser::addTopLevel()
{
    addOption(
        'T', "toplevel", true, true,
        "Sets the name of the top-level design unit.\n"
        "By default tries to automatically detect it from the description.");
}

void CommandLineParser::addClock() { addOption('c', "clock", true, true, "Sets the name of the top-level clock.\n"); }

void CommandLineParser::addReset() { addOption('r', "reset", true, true, "Sets the name of the top-level reset.\n"); }

// ////////////////////////////////////////////////////////////
// Support methods.
// ////////////////////////////////////////////////////////////

auto CommandLineParser::getFiles() -> CommandLineParser::Files & { return _files; }

void CommandLineParser::printHelp() const
{
    std::string help;
    std::string delimiter(_maxLineSize + 4, '*');

    std::string toolName(_toolName);
#ifndef NDEBUG
    toolName += " [DEBUG]";
#endif

    help = "\n\n" + delimiter + "\n" + _makeLine() + _makeLine(toolName) + _formatLine('a', "", _copyright) +
           _makeLine() + _formatLine('a', "", _toolDescription) + _makeLine() + _makeLine("SYNOPSYS") +
           _formatLine('a', "", _synopsys) + _makeLine() + _makeLine(std::string("DESCRIPTION"));

    for (const auto &_option : _options) {
        if (_option.second.isActive) {
            help += _option.second.description;
        }
    }

    if (!_notes.empty()) {
        help += _makeLine() + _makeLine("NOTES") + _formatLine('a', "", _notes, 2);
    }

    help += _makeLine() + delimiter + "\n\n";

    messageInfo(help);
    exit(EXIT_SUCCESS);
}

void CommandLineParser::printVersion() const
{
    dumpVersion(_toolName);
    exit(EXIT_SUCCESS);
}

auto CommandLineParser::isVerbose() const -> bool
{
#ifdef NDEBUG
    return true;
#endif
    return isOptionFlagSet('V');
}

auto CommandLineParser::getOutputFile() const -> const std::string & { return getOption('o'); }

auto CommandLineParser::getOutputDirectory() const -> const std::string & { return getOption('D'); }

auto CommandLineParser::isAutostep() const -> bool { return isOptionFlagSet('a'); }

auto CommandLineParser::isPrintOnly() const -> bool { return isOptionFlagSet('p'); }

auto CommandLineParser::isWriteParsing() const -> bool { return isOptionFlagSet('w'); }

auto CommandLineParser::isParseOnly() const -> bool { return isOptionFlagSet('P'); }

auto CommandLineParser::getConfigFile() const -> const std::string & { return getOption('C'); }

auto CommandLineParser::isOptimized() const -> bool { return isOptionFlagSet('O'); }

auto CommandLineParser::getTopLevel() const -> const std::string & { return getOption('T'); }

auto CommandLineParser::getTopLevelDesignUnit(hif::System *system, hif::semantics::ILanguageSemantics *sem)
    -> hif::DesignUnit *
{
    if (_topLevelDu != nullptr) {
        return _topLevelDu;
    }

    hif::manipulation::FindTopOptions topt;
    topt.topLevelName   = getTopLevel();
    topt.checkAtMostOne = true;
    topt.useHeuristics  = true;
    View *topView       = hif::manipulation::findTopLevelModule(system, sem, topt);
    messageAssert(topView != nullptr, "Top View not found.", nullptr, nullptr);
    _topLevelDu = dynamic_cast<DesignUnit *>(topView->getParent());
    messageAssert(_topLevelDu != nullptr, "Top Module not found.", nullptr, nullptr);
    messageAssert(
        _topLevelDu->views.size() == 1U,
        "Top level Module has more than one architecture."
        "This is not supported yet.",
        nullptr, nullptr);

    return _topLevelDu;
}

auto CommandLineParser::getGenerateConfigFile() const -> const std::string & { return getOption('G'); }

auto CommandLineParser::getClock() const -> const std::string & { return getOption('c'); }

auto CommandLineParser::getReset() const -> const std::string & { return getOption('r'); }

// ////////////////////////////////////////////////////////////
// Internal methods.
// ////////////////////////////////////////////////////////////

auto CommandLineParser::_makeLine(const std::string &s) const -> std::string
{
    if (s.empty()) {
        return "* " + _padString(s, _maxLineSize) + " *\n";
    }
    std::string ret;
    Chunks chunks;
    _makeChunks(chunks, s, _maxLineSize);
    for (auto &chunk : chunks) {
        ret += "* " + _padString(chunk, _maxLineSize) + " *\n";
    }

    return ret;
}

auto CommandLineParser::_formatLine(
    const char shortName,
    const std::string &longName,
    const std::string &description,
    const std::size_t margin) const -> std::string
{
    std::string ret;
    if (!longName.empty()) {
        ret = _makeLine(std::string("-") + shortName + ", --" + longName);
    }

    Chunks chunks;
    _makeChunks(chunks, description, _maxLineSize - 5, margin);
    for (auto &chunk : chunks) {
        ret += "* " + _padString(_indentation + chunk, _maxLineSize) + " *\n";
    }

    return ret;
}

void CommandLineParser::_makeChunks(
    Chunks &chunks,
    std::string s,
    const std::size_t maxSize,
    const std::size_t margin)

{
    std::string line;

    while (!s.empty()) {
        std::string::size_type pos(s.find_first_of(" \n"));
        bool isNewline = (pos != std::string::npos && s[pos] == '\n');
        std::string c  = s.substr(0, pos);
        if (line.size() + c.size() + 1 < maxSize) {
            if (!line.empty()) {
                line += " ";
            }
            line += c;
        } else {
            chunks.push_back(line);
            line = std::string(margin, ' ');
            line += c;
        }
        if (isNewline) {
            chunks.push_back(line);
            line = "";
        }
        if (pos != std::string::npos) {
            s = s.substr(pos + 1);
        } else {
            s = "";
        }
    }

    if (!line.empty()) {
        chunks.push_back(line);
    }
    if (!s.empty() && s != line) {
        chunks.push_back(s);
    }
}
auto CommandLineParser::_padString(const std::string &s, const std::size_t maxLineSize) -> std::string
{
    std::string padding;
    std::string::size_type size = maxLineSize - s.size();
    if (maxLineSize < s.size()) {
        size = 0;
    }
    padding.assign(size, ' ');
    return s + padding;
}

} // namespace application_utils

} // namespace hif
