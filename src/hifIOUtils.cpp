/// @file hifIOUtils.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <time.h>

#if (defined __GNUC__)
#    include <sys/time.h>
#else
#    include <windows.h>
#endif

#include "hif/application_utils/Log.hpp"
#include "hif/hif.hpp"
#include "hif/hifPrinter.hpp"

#ifndef __GNUC__

namespace hif
{
namespace
{

/// @brief Define suseconds_t as an alias for time_t for compatibility.
using suseconds_t = time_t;

/// @brief Struct to represent timezone information.
struct timezone {
    int tz_minuteswest; ///< Minutes west of Greenwich.
    int tz_dsttime;     ///< Type of daylight savings time correction to apply.
};

/// @brief Difference between Windows epoch and Unix epoch in microseconds.
#    define DELTA_EPOCH_IN_MICROSECS 11644473600000000ULL

/// @brief Replacement for `gettimeofday` on non-GNU platforms (e.g., Windows).
/// @details Retrieves the current time and timezone information.
///
/// @param tv Pointer to a timeval struct to store the current time.
/// @param tz Pointer to a timezone struct to store timezone information (optional).
/// @return Returns 0 on success.
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME ft;
    unsigned __int64 tmpres   = 0;
    static int tz_initialized = 0;

    // Check if the timeval pointer is not null.
    if (tv != nullptr) {
        GetSystemTimeAsFileTime(&ft);

        // Convert FILETIME (100-ns intervals since Windows epoch) to microseconds.
        tmpres |= static_cast<unsigned __int64>(ft.dwHighDateTime) << 32;
        tmpres |= ft.dwLowDateTime;

        // Convert to Unix epoch time.
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tmpres /= 10; // Convert to microseconds.

        // Set seconds and microseconds in the timeval struct.
        tv->tv_sec  = static_cast<long>(tmpres / 1000000UL);
        tv->tv_usec = static_cast<long>(tmpres % 1000000UL);
    }

    // Check if the timezone pointer is not null.
    if (tz != nullptr) {
        if (!tz_initialized) {
            _tzset(); // Initialize timezone information.
            tz_initialized = 1;
        }
        tz->tz_minuteswest = _timezone / 60; // Set minutes west of Greenwich.
        tz->tz_dsttime     = _daylight;      // Set daylight savings time flag.
    }

    return 0;
}
} // unnamed namespace
} // namespace hif

#endif

namespace hif
{

// Implemented in xml_parser.cpp
Object *parse_xml(std::istream &in, hif::semantics::ILanguageSemantics *sem);

namespace
{ // anon namespace

/// Hack to quickly convert an object of type \p T to a string.
template <typename T> std::string toStr(T const &t)
{
    std::ostringstream i;
    i << t;
    return i.str();
}

/// Returns a string with the current format: hh:mm:ss:usec
std::string getCurrTimeStr()
{
    struct timeval tv1;
    gettimeofday(&tv1, nullptr);
    struct tm *current;
    time_t now;
    time(&now);
    current             = localtime(&now);
    std::string timestr = toStr(current->tm_hour);
    timestr.append(":");
    timestr.append(toStr(current->tm_min));
    timestr.append(":");
    timestr.append(toStr(current->tm_sec));
    timestr.append(":");
    timestr.append(toStr(tv1.tv_usec));
    return timestr;
}

Object *_readFile(std::istream &instream, const ReadHifOptions &opt) { return parse_xml(instream, opt.sem); }

} // namespace
} // namespace hif

namespace hif
{

PrintHifOptions::PrintHifOptions()
    : printComments(true)
    , printSummary(false)
    , printCodeInfos(false)
    , printProperties(true)
    , printAdditionalKeywords(true)
    , printHifStandardLibraries(false)
    , appendMode(false)
    , sem(hif::semantics::HIFSemantics::getInstance())
{
    // ntd
}

PrintHifOptions::~PrintHifOptions()
{
    // ntd
}

PrintHifOptions::PrintHifOptions(const PrintHifOptions &other)
    : printComments(other.printComments)
    , printSummary(other.printSummary)
    , printCodeInfos(other.printCodeInfos)
    , printProperties(other.printProperties)
    , printAdditionalKeywords(other.printAdditionalKeywords)
    , printHifStandardLibraries(other.printHifStandardLibraries)
    , appendMode(other.appendMode)
    , sem(other.sem)
{
    // ntd
}

PrintHifOptions &PrintHifOptions::operator=(const PrintHifOptions &other)
{
    if (&other == this)
        return *this;

    printComments             = other.printComments;
    printSummary              = other.printSummary;
    printCodeInfos            = other.printCodeInfos;
    printProperties           = other.printProperties;
    printAdditionalKeywords   = other.printAdditionalKeywords;
    printHifStandardLibraries = other.printHifStandardLibraries;
    appendMode                = other.appendMode;
    sem                       = other.sem;

    return *this;
}

void writeFile(const std::string &filename, Object *obj, bool xml_format, const PrintHifOptions &opt)
{
    // Check directory
    hif::application_utils::FileStructure filePath(filename);
    hif::application_utils::FileStructure dirPath = filePath.getParentFile();
    dirPath.make_dirs();
    messageAssert(dirPath.exists() && dirPath.isDirectory(), "Expected directory", nullptr, nullptr);

    // Actual write
    std::string f(filename);
    std::string ext;
    if (xml_format) {
        ext = ".hif.xml";
    } else {
        ext = ".hif";
    }

    if (f.length() < ext.length() || f.substr(f.length() - ext.length()) != ext) {
        f += ext;
    }

    std::ofstream out;
    if (opt.appendMode) {
        out.open(f.c_str(), std::ios_base::out | std::ios_base::app);
    } else
        out.open(f.c_str(), std::ios_base::out);
    writeFile(out, obj, xml_format, opt);
}
void writeFile(std::ostream &outstream, Object *obj, bool xml_format, const PrintHifOptions &opt)
{
    if (dynamic_cast<System *>(obj)) {
        // add info to System
        System *so               = static_cast<System *>(obj);
        System::VersionInfo info = so->getVersionInfo();
        // set specific info
        info.tool                = hif::application_utils::getApplicationName();
        info.generationDate      = getCurrTimeStr();
        // Forcing latest version:
        System::VersionInfo info2;
        info.formatVersionMajor = info2.formatVersionMajor;
        info.formatVersionMinor = info2.formatVersionMinor;
        so->setVersionInfo(info);
    }
    if (xml_format) {
        printXml(*obj, outstream, opt); // always print code info in xml
    } else {
        printHif(*obj, outstream, opt);
    }
}

void writeUniqueFile(
    const std::string &filename,
    Object *obj,
    const std::string &custom_path,
    bool xml_format,
    const PrintHifOptions &opt)
{
    // Check directory
    application_utils::FileStructure filePath(filename);
    application_utils::FileStructure dirPath = filePath.getParentFile();
    dirPath.make_dirs();
    messageAssert(dirPath.exists() && dirPath.isDirectory(), "Expected directory", nullptr, nullptr);

    // Actual write
    static int file_id = 0;

    if (obj == nullptr)
        return;
    if (!filename.find("/") && !filename.find("\\"))
        return;

    struct timeval tv1;
    gettimeofday(&tv1, nullptr);

    struct tm *current;
    time_t now;

    time(&now);
    current = localtime(&now);

    std::string outfile("");
    std::stringstream val;

    if (custom_path == "") {
        application_utils::hif_mkdir(
            "debug_out_files", application_utils::PERMISSION_RWX_USR | application_utils::PERMISSION_RWX_GRP |
                                   application_utils::PERMISSION_R_OTH | application_utils::PERMISSION_X_OTH);
        val << "debug_out_files/" << filename << "_" << getCurrTimeStr() << "_" << file_id++;
        val >> outfile;
    } else {
        val << custom_path << filename << "_" << current->tm_hour << ":" << current->tm_min << ":" << current->tm_sec
            << ":" << tv1.tv_usec << "_" << file_id++;
        val >> outfile;
    }

    writeFile(outfile, obj, xml_format, opt);
    std::cout << "* Written file " << outfile << std::endl;
}

Object *readFile(const std::string &filename, const ReadHifOptions &opt)
{
    std::ifstream in(filename.c_str());
    return _readFile(in, opt);
}

void writeParents(unsigned int depth, std::ostream &outstream, Object *obj, const hif::PrintHifOptions &opt)
{
    if (obj == nullptr)
        return;
    obj = obj->getParent();
    if (obj == nullptr)
        return;

    for (unsigned int i = 0; i < depth || depth == 0; ++i) {
        outstream << std::endl;
        outstream << "------------------------------------------------------------\n";
        outstream << "Parent #" << (i + 1) << ":\n";
        writeFile(outstream, obj, false, opt);
        outstream << std::endl;
        obj = obj->getParent();
        if (obj == nullptr)
            break;
    }
}
ReadHifOptions::ReadHifOptions()
    : loadHifStandardLibrary(true)
    , sem(hif::semantics::HIFSemantics::getInstance())
{
    // ntd
}

ReadHifOptions::~ReadHifOptions()
{
    // ntd
}

ReadHifOptions::ReadHifOptions(const ReadHifOptions &other)
    : loadHifStandardLibrary(other.loadHifStandardLibrary)
    , sem(other.sem)
{
    // ntd
}

ReadHifOptions &ReadHifOptions::operator=(const ReadHifOptions &other)
{
    if (&other == this)
        return *this;
    loadHifStandardLibrary = other.loadHifStandardLibrary;
    sem                    = other.sem;
    return *this;
}
} // namespace hif
