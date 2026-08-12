/// @file Properties.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "hif/application_utils/FileStructure.hpp"
#include "hif/backends/Properties.hpp"

namespace hif
{
namespace backends
{

void Properties::load(const std::string &config_file_name, bool silent, bool evaluate)
{
    name = eval(config_file_name);
    std::ifstream stream(name);

    if (!stream) {
        if (!silent) {
            std::cerr << "Cannot load config file: " << name << std::endl;
        }
        return;
    }

    std::string line;
    while (std::getline(stream, line)) {
        // Trim leading whitespace.
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos)
            continue; // Skip empty lines.

        // Ignore comments.
        if (line[start] == '#')
            continue;

        // Find position of the delimiter '='
        size_t equal_pos = line.find('=');
        if (equal_pos == std::string::npos)
            continue;

        // Extract key (trimmed)
        std::string key = line.substr(start, equal_pos - start);
        key.erase(key.find_last_not_of(" \t") + 1);

        // Extract value (trimmed)
        size_t value_start = line.find_first_not_of(" \t", equal_pos + 1);
        if (value_start == std::string::npos)
            continue; // Skip if no value.

        std::string value = line.substr(value_start);

        // Store the property
        if (evaluate)
            setProperty(key, value);
        else
            setProperty(key, value, 0);
    }
}

void Properties::dump(std::ostream &out) const
{
    for (const auto &property : properties) {
        out << property.first << " = " << property.second << std::endl;
    }
}

std::string Properties::getProperty(const std::string &property_name) const
{
    auto itr = properties.find(property_name);
    if (itr == properties.end()) {
        return "";
    }
    return itr->second;
}

std::string Properties::operator[](const std::string &property_name) const { return this->getProperty(property_name); }

void Properties::setProperty(const std::string &property_name, const std::string &property, bool evaluate)
{
    if (evaluate) {
        properties[property_name] = eval(property);
    } else {
        properties[property_name] = property;
    }
}

void Properties::appendProperty(const std::string &property_name, const std::string &property, bool evaluate)
{
    setProperty(property_name, property, evaluate);
    Properties iprops;
    iprops.load(name, 1, evaluate);
    iprops.setProperty(property_name, property, evaluate);
    std::ofstream oFile;
    hif::application_utils::FileStructure fFile(name);
    if (!(fFile.getParentFile().exists())) {
        fFile.getParentFile().make_dirs();
    }
    oFile.open(name.c_str());
    if (oFile.is_open()) {
        iprops.dump(oFile);
    }
}

std::string Properties::eval(const std::string &s_in) const
{
    int l;
    std::string s = hif::application_utils::FileStructure::eval(s_in);

    l = static_cast<int>(s.find("$("));
    if (l == -1) {
        return s;
    } else {
        std::string r(s, 0, static_cast<unsigned long>(l));
        std::string s1(s, static_cast<unsigned long>(l + 2));
        std::string evald_s1(eval(s1));
        int vl;

        vl = static_cast<int>(evald_s1.find(")"));
        if (vl != -1) {
            std::string varname(evald_s1, 0, static_cast<unsigned long>(vl));
            std::string r2(evald_s1, static_cast<unsigned long>(vl + 1));

            if (properties.find(varname) != properties.end()) {
                Properties *This = const_cast<Properties *>(this);
                // mutable not supported by SUN 5.0
                r += This->properties[varname];
            } else {
                char *env_val = getenv(varname.c_str());

                if (env_val != nullptr) {
                    r += env_val;
                } else {
                    r += "$(" + varname + ")";
                }
            }
            r += r2;
        } else {
            r += "$(" + evald_s1;
        }

        return r;
    }
}

} // namespace backends
} // namespace hif
