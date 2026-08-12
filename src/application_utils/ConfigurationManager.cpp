/// @file ConfigurationManager.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "hif/application_utils/ConfigurationManager.hpp"
#include "hif/application_utils/Log.hpp"

namespace hif
{
namespace application_utils
{

namespace /* anon */
{

#ifdef DEBUG_CONFIG_MANAGER
void _printSectionsMap(ConfigurationManager::Sections &sections)
{
    typedef ConfigurationManager::Sections Sections;
    typedef ConfigurationManager::Directives Directives;
    typedef ConfigurationManager::Variables Variables;

    for (Sections::iterator i = sections.begin(); i != sections.end(); ++i) {
        std::clog << "-----------------------------------------------------\n";
        std::clog << "Section: " << i->first << std::endl;

        std::clog << "    # Directives:\n";
        for (Directives::iterator j = i->second.directives.begin(); j != i->second.directives.end(); ++j) {
            std::clog << "    ;" << j->first << " = ";
            for (std::vector<std::string>::iterator k = j->second.begin(); k != j->second.end(); ++k) {
                std::clog << *k << " ";
            }
            std::clog << std::endl;
        }

        std::clog << std::endl;

        std::clog << "    # Variables:\n";
        for (Variables::iterator j = i->second.variables.begin(); j != i->second.variables.end(); ++j) {
            std::clog << "    " << j->first << " = ";
            for (std::vector<std::string>::iterator k = j->second.begin(); k != j->second.end(); ++k) {
                std::clog << *k << " ";
            }
            std::clog << std::endl;
        }

        std::clog << "-----------------------------------------------------\n";
    }
    exit(1);
}
#endif

auto ltrim(std::string &s) -> std::string &
{
    std::string::size_type pos = s.find_first_not_of(" \n\r\t\f\a");
    if (pos == std::string::npos) {
        s = "";
        return s;
    }
    s = s.substr(pos);
    return s;
}

auto rtrim(std::string &s) -> std::string &
{
    if (s.empty()) {
        return s;
    }
    s.erase(s.find_last_not_of(" \n\r\t\f\a") + 1);
    return s;
}

auto trim(std::string s) -> std::string { return ltrim(rtrim(s)); }

void _printComments(std::ofstream &out, const std::vector<std::string> &comments)
{
    for (const auto &comment : comments) {
        out << "# " << comment << '\n';
    }
}

void _printDirective(std::ofstream &out, const std::string &dir, const ConfigurationManager::KeyValues &keyValues)
{
    if (keyValues.values.empty()) {
        return;
    }

    _printComments(out, keyValues.comments);

    out << ";" << dir << " =";

    for (const auto &value : keyValues.values) {
        out << " " << value;
    }

    out << "\n\n";
}

void _printVariable(std::ofstream &out, const std::string &var, const ConfigurationManager::KeyValues &keyValues)
{
    if (keyValues.values.empty()) {
        return;
    }

    _printComments(out, keyValues.comments);

    out << var << " =";

    for (const auto &value : keyValues.values) {
        out << " " << value;
    }

    out << "\n\n";
}

void _printSection(std::ofstream &out, const std::string &section, const ConfigurationManager::SectionData &data)
{
    _printComments(out, data.comments);

    if (!section.empty()) {
        out << "[" << section << "]\n\n";
    }

    for (const auto &directive : data.directives) {
        _printDirective(out, directive.first, directive.second);
    }

    out << "\n";

    for (const auto &variable : data.variables) {
        _printVariable(out, variable.first, variable.second);
    }

    out << "\n";
}

} // namespace

auto ConfigurationManager::parse(const std::string &file) -> bool
{
    _currentSection = "";
    std::ifstream in(file.c_str());
    std::string line;
    while (in.good()) {
        std::getline(in, line);
        if (!_parseLine(line)) {
            return false;
        }
    }

#ifdef DEBUG_CONFIG_MANAGER
    _printSectionsMap(_sections);
#endif

    return _validateInput();
}

void ConfigurationManager::addSection(const std::string &section)
{
    if (_sections.find(section) != _sections.end()) {
        return;
    }
    if (section.empty()) {
        _orderedSections.push_front(section);
    } else {
        _orderedSections.push_back(section);
    }
    _sections[section];
}

void ConfigurationManager::addValue(
    const std::string &section,
    const std::string &id,
    const std::string &value,
    bool isDirective)
{
    addSection(section);
    if (isDirective) {
        _sections[section].directives[id].values.push_back(value);
    } else {
        _sections[section].variables[id].values.push_back(value);
    }
}

void ConfigurationManager::addValues(
    const std::string &section,
    const std::string &id,
    const std::vector<std::string> &values,
    bool isDirective)
{
    addSection(section);
    if (isDirective) {
        std::vector<std::string> &v = _sections[section].directives[id].values;
        v.insert(v.end(), values.begin(), values.end());
    } else {
        std::vector<std::string> &v = _sections[section].variables[id].values;
        v.insert(v.end(), values.begin(), values.end());
    }
}

void ConfigurationManager::setValue(
    const std::string &section,
    const std::string &id,
    const std::string &value,
    bool isDirective)
{
    addSection(section);
    if (isDirective) {
        _sections[section].directives[id].values.clear();
        _sections[section].directives[id].values.push_back(value);
    } else {
        _sections[section].variables[id].values.clear();
        _sections[section].variables[id].values.push_back(value);
    }
}

void ConfigurationManager::setValues(
    const std::string &section,
    const std::string &id,
    const std::vector<std::string> &values,
    bool isDirective)
{
    addSection(section);
    if (isDirective) {
        std::vector<std::string> &v = _sections[section].directives[id].values;
        v                           = values;
    } else {
        std::vector<std::string> &v = _sections[section].variables[id].values;
        v                           = values;
    }
}

auto ConfigurationManager::getValue(const std::string &section, const std::string &id, bool isDirective)
    -> std::string &
{
    auto it = _sections.find(section);
    messageAssert(it != _sections.end(), "No section found", nullptr, nullptr);
    if (isDirective) {
        auto jt = it->second.directives.find(id);
        messageAssert(jt != it->second.directives.end(), "No variable found", nullptr, nullptr);
        messageAssert(!jt->second.values.empty(), "No value found", nullptr, nullptr);
        return jt->second.values.back();
    }

    auto jt = it->second.variables.find(id);
    messageAssert(jt != it->second.variables.end(), "No variable found", nullptr, nullptr);
    messageAssert(!jt->second.values.empty(), "No value found", nullptr, nullptr);
    return jt->second.values.back();
}

auto ConfigurationManager::getValues(const std::string &section, const std::string &id, bool isDirective)
    -> std::vector<std::string> &
{
    auto it = _sections.find(section);
    messageAssert(it != _sections.end(), "No section found", nullptr, nullptr);
    if (isDirective) {
        auto jt = it->second.directives.find(id);
        messageAssert(jt != it->second.directives.end(), "No variable found", nullptr, nullptr);
        return jt->second.values;
    }
    auto jt = it->second.variables.find(id);
    messageAssert(jt != it->second.variables.end(), "No variable found", nullptr, nullptr);
    return jt->second.values;
}

auto ConfigurationManager::getValue(const std::string &section, const std::string &id, bool isDirective) const
    -> const std::string &
{
    auto it = _sections.find(section);
    messageAssert(it != _sections.end(), "No section found", nullptr, nullptr);
    if (isDirective) {
        auto jt = it->second.directives.find(id);
        messageAssert(jt != it->second.directives.end(), "No variable found", nullptr, nullptr);
        messageAssert(!jt->second.values.empty(), "No value found", nullptr, nullptr);
        return jt->second.values.back();
    }

    auto jt = it->second.variables.find(id);
    messageAssert(jt != it->second.variables.end(), "No variable found", nullptr, nullptr);
    messageAssert(!jt->second.values.empty(), "No value found", nullptr, nullptr);
    return jt->second.values.back();
}

auto ConfigurationManager::getValues(const std::string &section, const std::string &id, bool isDirective) const
    -> const std::vector<std::string> &
{
    auto it = _sections.find(section);
    messageAssert(it != _sections.end(), "No section found", nullptr, nullptr);
    if (isDirective) {
        auto jt = it->second.directives.find(id);
        messageAssert(jt != it->second.directives.end(), "No variable found", nullptr, nullptr);
        return jt->second.values;
    }
    auto jt = it->second.variables.find(id);
    messageAssert(jt != it->second.variables.end(), "No variable found", nullptr, nullptr);
    return jt->second.values;
}

auto ConfigurationManager::hasSection(const std::string &section) const -> bool
{
    auto it = _sections.find(section);
    return it != _sections.end();
}

auto ConfigurationManager::hasDirective(const std::string &section, const std::string &dir) const -> bool
{
    auto it = _sections.find(section);
    if (it == _sections.end()) {
        return false;
    }
    const Directives &dirs = it->second.directives;
    auto jt                = dirs.find(dir);
    return jt != dirs.end();
}

auto ConfigurationManager::hasVariable(const std::string &section, const std::string &var) const -> bool
{
    auto it = _sections.find(section);
    if (it == _sections.end()) {
        return false;
    }
    const Variables &vars = it->second.variables;
    auto jt               = vars.find(var);
    return jt != vars.end();
}

void ConfigurationManager::eraseSection(const std::string &section)
{
    _orderedSections.erase(std::find(_orderedSections.begin(), _orderedSections.end(), section));
    _sections.erase(section);
}

void ConfigurationManager::eraseDirective(const std::string &section, const std::string &dir)
{
    auto it = _sections.find(section);
    if (it == _sections.end()) {
        return;
    }
    it->second.directives.erase(dir);
}

void ConfigurationManager::eraseVariable(const std::string &section, const std::string &var)
{
    auto it = _sections.find(section);
    if (it == _sections.end()) {
        return;
    }
    it->second.variables.erase(var);
}

auto ConfigurationManager::writeFile(const std::string &file) const -> bool
{
    if (!_validateOutput()) {
        return false;
    }
    std::ofstream out(file.c_str());

    for (const auto &_orderedSection : _orderedSections) {
        auto j = _sections.find(_orderedSection);
        _printSection(out, j->first, j->second);
    }

    out << "\n# EOF\n" << std::flush;

    return true;
}

void ConfigurationManager::addComment(const std::string &comment) { addComment("", comment); }

void ConfigurationManager::addComment(const std::string &section, const std::string &comment)
{
    if (section.empty()) {
        addSection(section);
    }
    auto it = _sections.find(section);
    if (it == _sections.end()) {
        return;
    }
    it->second.comments.push_back(comment);
}

void ConfigurationManager::addComment(
    const std::string &section,
    const std::string &identifier,
    const std::string &comment,
    bool isDirective)
{
    auto it = _sections.find(section);
    if (it == _sections.end()) {
        return;
    }

    if (isDirective) {
        Directives &directives = it->second.variables;
        auto jt                = directives.find(identifier);
        if (jt == directives.end()) {
            return;
        }
        jt->second.comments.push_back(comment);
    } else {
        Variables &variables = it->second.variables;
        auto jt              = variables.find(identifier);
        if (jt == variables.end()) {
            return;
        }
        jt->second.comments.push_back(comment);
    }
}

auto ConfigurationManager::_validateInput() -> bool
{
    // For this basic implementation, input is always fine.
    return true;
}

auto ConfigurationManager::_validateOutput() const -> bool
{
    // For this basic implementation, output is always fine.
    return true;
}

auto ConfigurationManager::_parseLine(const std::string &line) -> bool
{
    std::string l = trim(line);
    if (l.empty()) {
        return true;
    }

    switch (l[0]) {
    case '#':
        return _parseComment(l);
    case '[':
        return _parseSection(l);
    case ';':
        return _parseDirective(l);
    default:
        return _parseVariable(l);
    }
}

auto ConfigurationManager::_parseComment(const std::string & /*s*/) -> bool
{
    // @TODO
    //_sections[_currentSection].comments.push_back(l.substr(1));
    return true;
}

auto ConfigurationManager::_parseSection(const std::string &s) -> bool
{
    if (s[s.size() - 1] != ']') {
        return false;
    }
    std::string res = trim(s.substr(1, s.size() - 2));
    if (res.empty()) {
        return false;
    }
    _currentSection = res;
    addSection(res);
    return true;
}

auto ConfigurationManager::_parseDirective(const std::string &s) -> bool
{
    std::string::size_type pos = s.find('=');
    std::string key;
    std::string val;
    if (pos == std::string::npos) {
        key = trim(s.substr(1));
        val = "";
    } else {
        key = trim(s.substr(1, pos - 1));
        val = trim(s.substr(pos + 1));
    }
    if (key.empty()) {
        return false;
    }
    _parseValues(key, val, true);
    return true;
}

auto ConfigurationManager::_parseVariable(const std::string &s) -> bool
{
    std::string::size_type pos = s.find('=');
    std::string key;
    std::string val;
    if (pos == std::string::npos) {
        key = trim(s);
        val = "";
    } else {
        key = trim(s.substr(0, pos));
        val = trim(s.substr(pos + 1));
    }
    if (key.empty()) {
        return false;
    }
    _parseValues(key, val, false);
    return true;
}

void ConfigurationManager::_parseValues(const std::string &key, const std::string &s, bool isDirective)
{
    std::vector<std::string> values;
    std::stringstream ss;
    std::string val;
    ss << s;
    while (ss.good()) {
        ss >> val;
        if (!val.empty()) {
            values.push_back(val);
        }
    }
    addValues(_currentSection, key, values, isDirective);
}

} // namespace application_utils
} // namespace hif
