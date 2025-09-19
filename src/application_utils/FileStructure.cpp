/// @file FileStructure.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cerrno>
#include <iostream>
#if (defined _MSC_VER)
#    include <windows.h>
#endif
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <sys/stat.h>
#include <sys/types.h>

#include "hif/application_utils/FileStructure.hpp"

namespace hif
{
namespace application_utils
{

//
//        Constants
//
/// @brief Maximum buffer size.
#define MAX_BUF_SIZE 4096

// MESSAGGES
/// @brief Error message for write permission denied.
#define DIR_WRITE_PERM_DENIED    "Write permission denied.\n Cannnot write the file \"%s\"."
/// @brief Error message for file already exists.
#define DIR_EXISTS               "The file \"%s\" already exists."
/// @brief Error message for too many links.
#define DIR_PARENT_DIR_TOO_LINKS "Too many links.\nCannot treat the file \"%s\"."
/// @brief Error message for not enough room.
#define DIR_NOT_ENOUGH_ROOM      "Not enough rooms.\nCannot treat the file \"%s\"."
/// @brief Error message for parent directory read-only.
#define DIR_PARENT_DIR_READ_ONLY "Parent directory is in read-only mode.\nCannot treat the file \"%s\"."
//#define DIR_SRC_PATH_NOT_FOUND "Path \"%s\" not found."
/// @brief Error message for file does not exist.
#define DIR_NO_EXIST             "The file named by \"%s\" doesn't exist."
//#define DIR_COPY_FILE_FAILED "Cannot copy the file \"%s\" onto the file \"%s\"."
//#define DIR_CANNOT_OPEN_FILE "Cannot open the file \"%s\".\n%s"
/// @brief Error message for path too long.
#define DIR_SUP_MAX_BUF_SIZE     "The absolute path corresponding to \"%s\" is too long."

//
//	Functions
//
static void msg_error [[noreturn]] (const std::string &string_e)
{
    char *message = nullptr;
    switch (errno) {
    case EACCES:
        message = new char[strlen(DIR_WRITE_PERM_DENIED) + string_e.size() + 1];
        sprintf(message, DIR_WRITE_PERM_DENIED, string_e.c_str());
        break;
    case EEXIST:
        message = new char[strlen(DIR_EXISTS) + string_e.size() + 1];
        sprintf(message, DIR_EXISTS, string_e.c_str());
        break;
    case EMLINK:
        message = new char[strlen(DIR_PARENT_DIR_TOO_LINKS) + string_e.size() + 1];
        sprintf(message, DIR_PARENT_DIR_TOO_LINKS, string_e.c_str());
        break;
    case ENOSPC:
        message = new char[strlen(DIR_NOT_ENOUGH_ROOM) + string_e.size() + 1];
        sprintf(message, DIR_NOT_ENOUGH_ROOM, string_e.c_str());
        break;
    case EROFS:
        message = new char[strlen(DIR_PARENT_DIR_READ_ONLY) + string_e.size() + 1];
        sprintf(message, DIR_PARENT_DIR_READ_ONLY, string_e.c_str());
        break;
    case ENOENT:
        message = new char[strlen(DIR_NO_EXIST) + string_e.size() + 1];
        sprintf(message, DIR_NO_EXIST, string_e.c_str());
        break;
    case ERANGE:
        message = new char[strlen(DIR_SUP_MAX_BUF_SIZE) + string_e.size() + 1];
        sprintf(message, DIR_SUP_MAX_BUF_SIZE, string_e.c_str());
        break;
    default:
        message = new char[strlen("File structure: error number %d undefined.") + string_e.size() + 1];
        sprintf(message, "File structure: error number %d undefined.", errno);
        break;
    }
    messageError(message, nullptr, nullptr);
}

static auto copy_file(const std::string &file_in, const std::string &file_out) -> bool
{
    FILE *fp_in      = nullptr;
    FILE *fp_out     = nullptr;
    size_t n_obj_in  = 0;
    size_t n_obj_fix = 256;
    void *ptr_in     = nullptr;

    fp_in = fopen(file_in.c_str(), "rb");
    if (fp_in == nullptr) {
        msg_error(file_in);
    }
    fp_out = fopen(file_out.c_str(), "wb");
    if (fp_out == nullptr) {
        msg_error(file_out);
    }
    while (feof(fp_in) == 0) {
        ptr_in   = malloc(1 * n_obj_fix);
        n_obj_in = fread(ptr_in, 1, n_obj_fix, fp_in);
        fwrite(ptr_in, 1, n_obj_in, fp_out);
        free(ptr_in);
    }
    fclose(fp_in);
    fclose(fp_out);
    return (true);
}

static auto copy_dir(const FileStructure &source, const FileStructure &dest) -> bool
{
    std::vector<FileStructure> list_files = source.listFiles();

    if (list_files.empty()) {
        return (true);
    } else {
        auto first_e = list_files.begin();
        auto end_e   = list_files.end();
        FileStructure file_t;

        while (first_e != end_e) {
            if (!(file_t.renameFile(dest))) {
                return (false);
            }
            file_t.addChild((*first_e).getChild());
            if ((*first_e).isDirectory()) {
                if (!file_t.make_dirs()) {
                    return (false);
                }
                if (!copy_dir(*first_e, file_t)) {
                    return (false);
                }
            } else {
                if (!copy_file((*first_e).toString(), file_t.toString())) {
                    std::ostringstream os;
                    os << "Cannot copy the file \"" << (*first_e).toString().c_str() << "\" onto the file \""
                       << file_t.toString().c_str() << "\".";
                    messageError(os.str(), nullptr, nullptr);
                }
            }
            first_e++;
        }
    }
    return (true);
}

static void erase_point_element(std::vector<std::string> *file_p)
{
    auto first_e = file_p->begin();
    auto end_e   = file_p->end();

    while (first_e != end_e) {
        if (*first_e == ".") {
            first_e = file_p->erase(first_e);
            end_e   = file_p->end();
        } else {
            first_e++;
        }
    }
}

static void split_string(const std::string &string_s, const std::string &string_f, std::vector<std::string> &string_v)
{
    std::string string_v_e;
    std::string::size_type pos_t   = 0;
    std::string::size_type pos_t_1 = 0;

    pos_t_1 = 0;
    while ((pos_t = string_s.find_first_of(string_f, pos_t_1)) != std::string::npos) {
        string_v_e = string_s.substr(pos_t_1, pos_t - pos_t_1);
        if ((string_v_e + FileStructure::DefaultSeparator) != FileStructure::DefaultPrefix) {
            string_v.push_back(string_v_e);
        }
        pos_t_1 = pos_t + 1;
    }
    if (pos_t_1 < string_s.length()) {
        string_v.push_back(string_s.substr(pos_t_1, string_s.length() - pos_t_1));
    }
}

//inline
static void to_abstract_name(
    const std::string &path_name,
    const std::string &separator_path,
    std::vector<std::string> &abstract_name)
{
    if (path_name.empty()) {
        messageError("Path doesn't exist.", nullptr, nullptr);
    }
    split_string(path_name, separator_path, abstract_name);
}
//
//	Classes methods
//

FileStructure::FileStructure()
    : abstractName()
    , prefix(FileStructure::DefaultPrefix)
    , separator(FileStructure::DefaultSeparator)
    , absolutePath(false)
{
    // Nothing to do
}

FileStructure::FileStructure(const std::string &path_name)
    : abstractName()
    , prefix(FileStructure::DefaultPrefix)
    , separator(FileStructure::DefaultSeparator)
    , absolutePath(false)
{
    std::string string_t;

    if (prefix == path_name.substr(0, prefix.length())) {
        string_t     = path_name.substr(prefix.length(), path_name.length() - prefix.length());
        absolutePath = true;
    }
    to_abstract_name(path_name, FileStructure::SeparatorList, abstractName);
    if (absolutePath) {
        erase_point_element(&abstractName);
    }
}

FileStructure::FileStructure(const FileStructure &parent, const std::string &child)
    : abstractName(parent.abstractName)
    , prefix(parent.prefix)
    , separator(parent.separator)
    , absolutePath(parent.isAbsolute())
{
    std::vector<std::string> child_v;
    std::vector<std::string>::iterator first_c_v;
    std::vector<std::string>::iterator end_c_v;

    to_abstract_name(child, FileStructure::SeparatorList, child_v);
    first_c_v = child_v.begin();
    end_c_v   = child_v.end();
    while (first_c_v != end_c_v) {
        abstractName.push_back(*first_c_v++);
    }
    if (absolutePath) {
        erase_point_element(&abstractName);
    }
}

FileStructure::FileStructure(const std::string &parent, const std::string &child)
    : abstractName()
    , prefix(FileStructure::DefaultPrefix)
    , separator(FileStructure::DefaultSeparator)
    , absolutePath(false)
{
    std::vector<std::string> child_v;
    std::vector<std::string>::iterator first_c_v;
    std::vector<std::string>::iterator end_c_v;
    std::string string_t;

    if (prefix == parent.substr(0, prefix.length())) {
        string_t     = parent.substr(prefix.length(), parent.length() - prefix.length());
        absolutePath = true;
    }

    to_abstract_name(parent, FileStructure::SeparatorList, abstractName);
    to_abstract_name(child, FileStructure::SeparatorList, child_v);
    first_c_v = child_v.begin();
    end_c_v   = child_v.end();
    while (first_c_v != end_c_v) {
        abstractName.push_back(*first_c_v++);
    }
}

FileStructure::FileStructure(const std::vector<std::string> &path_name)
    : abstractName()
    , prefix(FileStructure::DefaultPrefix)
    , separator(FileStructure::DefaultSeparator)
    , absolutePath(false)
{
    if (!path_name.empty()) {
        abstractName = path_name;
    } else {
        abstractName.emplace_back("");
    }
}

FileStructure::~FileStructure()
{
    // nothing to do
}

auto FileStructure::canRead() -> bool
{
    // Not implemented yet.
    return (true);
}

auto FileStructure::canWrite() -> bool
{
    // Not implemented yet.
    return (true);
}

auto FileStructure::compareTo(const FileStructure &path_name) const -> int
{
    FileStructure file_t = path_name.getAbsoluteFile();
    FileStructure file_u = getAbsoluteFile();

    erase_point_element(&(file_t.abstractName));
    erase_point_element(&(file_u.abstractName));
    std::string string_t = file_t.toString();
    std::string string_u = file_u.toString();

    if (string_t.length() > string_u.length()) {
        return (-1);
    }
    if (string_t.length() < string_u.length())
        return (-2);
    if (string_t != string_u) {
        return (0);
    }
    return (1);
}

auto FileStructure::exists() const -> bool
{
    struct stat f_st {
    };
    if (stat(toString().c_str(), &f_st) == 0) {
        return true;
    }
    if (errno != ENOENT) {
        msg_error(toString());
    }
    return false;
}

auto FileStructure::getAbsoluteFile() const -> FileStructure
{
    FileStructure path_name;

    if (!isAbsolute()) {

        std::string string_af;
        std::string string_t;
        auto first_e = abstractName.begin();
        auto end_e   = abstractName.end();

        string_af              = std::filesystem::current_path();
        path_name.prefix       = prefix;
        path_name.separator    = separator;
        path_name.absolutePath = true;
        string_t = string_af.substr(path_name.prefix.length(), string_af.length() - path_name.prefix.length());
        to_abstract_name(string_t, FileStructure::SeparatorList, path_name.abstractName);
        while (first_e != end_e) {
            path_name.abstractName.push_back(*first_e++);
        }
    } else {
        path_name = *this;
    }

    return path_name;
}

auto FileStructure::getAbsolutePath() const -> std::string { return getAbsoluteFile().toString(); }

auto FileStructure::getName() const -> std::string { return toString(); }

auto FileStructure::getParent() const -> std::string
{
    FileStructure path_name;

    path_name = *this;
    path_name.abstractName.pop_back();
    return path_name.toString();
}

auto FileStructure::getChild() const -> std::string { return abstractName.back(); }

void FileStructure::addChild(const std::string &string_e) { abstractName.push_back(string_e); }

auto FileStructure::getParentFile() const -> FileStructure
{
    FileStructure path_name;

    path_name = *this;
    path_name.abstractName.pop_back();
    return path_name;
}

auto FileStructure::getPath() const -> std::string { return getParent(); }

auto FileStructure::getAbstractName() const -> std::vector<std::string> { return abstractName; }

auto FileStructure::isAbsolute() const -> bool
{
    if (!absolutePath) {
        return (false);
    }
    return (true);
}

auto FileStructure::isDirectory() const -> bool { return std::filesystem::is_directory(toString()); }

auto FileStructure::isFile() const -> bool { return std::filesystem::is_regular_file(toString()); }

auto FileStructure::isLink() const -> bool { return std::filesystem::is_symlink(toString()); }

auto FileStructure::isHidden() const -> bool
{
    // UNIX systems
    if (isFile() && (abstractName.back()[0] == '.')) {
        return (true);
    }
    return (false);
}

auto FileStructure::lastModified() const -> time_t
{
    struct stat f_st {
    };

    if (stat(toString().c_str(), &f_st) == 0) {
        return (f_st.st_mtime);
    }
    return (0);
}

auto FileStructure::lastAccess() const -> time_t
{
    struct stat f_st {
    };

    if (stat(toString().c_str(), &f_st) == 0) {
        return (f_st.st_atime);
    }
    return (0);
}

auto FileStructure::lastAttributesChange() const -> time_t
{
    struct stat f_st {
    };

    if (stat(toString().c_str(), &f_st) == 0) {
        return (f_st.st_ctime);
    }
    return (0);
}

auto FileStructure::length() const -> long
{
    FileStructure file_u = getAbsoluteFile();

    erase_point_element(&(file_u.abstractName));
    std::string string_u = file_u.toString();
    return static_cast<long>(string_u.length());
}

auto FileStructure::size() const -> long
{
    const std::filesystem::path path = toString();
    std::error_code ec;
    auto sz = std::filesystem::file_size(path, ec);
    if (ec) {
        return 0;
    }
    return static_cast<long>(sz);
}

auto FileStructure::depth() const -> int { return static_cast<int>(abstractName.size()); }

auto FileStructure::setToExe() const -> int
{
    std::filesystem::permissions(
        toString(),
        std::filesystem::perms::owner_all |                                            // Owner: read, write, execute
            std::filesystem::perms::group_read | std::filesystem::perms::group_exec |  // Group: read, execute
            std::filesystem::perms::others_read | std::filesystem::perms::others_exec, // Others: read, execute
        std::filesystem::perm_options::replace);
    return 0;
}

#if (defined _MSC_VER)

std::vector<std::string> FileStructure::list() const
{
    std::vector<std::string> files_list;
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = nullptr;

    //Specify a file mask. *.* = We want everything!
    std::string sPath = toString() + "\\*.*";

    if ((hFind = FindFirstFile(sPath.c_str(), &fdFile)) == INVALID_HANDLE_VALUE) {
        return files_list;
    }

    do {
        //Find first file will always return "."
        //    and ".." as the first two directories.
        if (strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0) {
            //Build up our file path using the passed in
            //  [sDir] and the file/foldername we just found:
            sPath = toString() + "\\" + fdFile.cFileName;
            files_list.push_back(sPath);
            /*
            //Is the entity a File or Folder?
            if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
            {
            files_list.push_back(sPath);
            ListDirectoryContents(sPath);
            }
            else{
            files_list.push_back(sPath);
            }
            */
        }
    } while (FindNextFile(hFind, &fdFile)); //Find the next file.

    FindClose(hFind); //Always, Always, clean things up!

    return files_list;
}
std::vector<std::string> FileStructure::list(const std::string &filter) const
{
    std::vector<std::string> files_list;
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = nullptr;

    std::string sPath;

    //Specify a file mask. *.* = We want everything!
    sPath = toString() + "\\*.*";

    if ((hFind = FindFirstFile(sPath.c_str(), &fdFile)) == INVALID_HANDLE_VALUE) {
        return files_list;
    }

    do {
        //Find first file will always return "."
        //    and ".." as the first two directories.
        if (strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0) {
            //Build up our file path using the passed in
            //  [sDir] and the file/foldername we just found:
            if (string(fdFile.cFileName).find_last_of(filter) != std::string(fdFile.cFileName).npos) {
                sPath = toString() + "\\" + fdFile.cFileName;
                files_list.push_back(sPath);
            }
        }
    } while (FindNextFile(hFind, &fdFile)); //Find the next file.

    FindClose(hFind); //Always, Always, clean things up!

    return files_list;
}
#else
auto FileStructure::list() const -> std::vector<std::string>
{
    DIR *d_stream         = nullptr;
    struct dirent *f_comp = nullptr;
    std::vector<std::string> files_list;
    std::string string_c = toString() + separator;
    std::string string_tc;

    if (isDirectory()) {
        if ((d_stream = opendir(toString().c_str())) == nullptr) {
            msg_error(toString());
        }
        while ((f_comp = readdir(d_stream)) != nullptr) {
            string_tc = f_comp->d_name;
            if ((string_tc != std::string(".")) && (string_tc != std::string(".."))) {
                string_c += string_tc;
                files_list.push_back(string_c);
                string_c = toString() + separator;
            }
        }
        closedir(d_stream);

        return (files_list);
    }
    return (files_list);
}

auto FileStructure::list(const std::string &filter) const -> std::vector<std::string>
{
    DIR *d_stream         = nullptr;
    struct dirent *f_comp = nullptr;
    std::vector<std::string> files_list;
    std::string string_c = toString() + separator;
    std::string string_tc;

    if (isDirectory()) {
        if ((d_stream = opendir(toString().c_str())) == nullptr) {
            msg_error(toString());
        }
        while ((f_comp = readdir(d_stream)) != nullptr) {
            string_tc = f_comp->d_name;
            if ((string_tc != std::string(".")) && (string_tc != std::string(".."))) {
                string_c += string_tc;
                if (string_c.find_last_of(filter) != std::string::npos) {
                    files_list.push_back(string_c);
                }
                string_c = toString() + separator;
            }
        }
        closedir(d_stream);
        return (files_list);
    }
    return (files_list);
}
#endif

auto FileStructure::listFiles() const -> std::vector<FileStructure>
{
    std::vector<std::string> files_list_s;
    std::vector<FileStructure> files_list_f;

    files_list_s = list();
    auto first_e = files_list_s.begin();
    auto end_e   = files_list_s.end();

    while (first_e != end_e) {
        files_list_f.emplace_back(*first_e++);
    }
    return (files_list_f);
}

auto FileStructure::listFiles(const std::string &filter) const -> std::vector<FileStructure>
{
    std::vector<std::string> files_list_s;
    std::vector<FileStructure> files_list_f;

    files_list_s = list(filter);
    auto first_e = files_list_s.begin();
    auto end_e   = files_list_s.end();

    while (first_e != end_e) {
        files_list_f.emplace_back(*first_e++);
    }
    return (files_list_f);
}

auto FileStructure::openfile(char *mode) const -> FILE *
{
    FILE *fp             = nullptr;
    std::string string_e = toString();
    errno                = 0;
    char *tmp_error      = nullptr;

    fp = fopen(string_e.c_str(), mode);
    if (fp == nullptr) {
        char *c        = strerror(errno);
        std::size_t s = strlen(c) + 1;
        tmp_error      = static_cast<char *>(malloc(sizeof(char) * s));
        strcpy(tmp_error, c);
        std::ostringstream os;
        os << "Cannot open the file \"" << string_e.c_str() << "\". \n " << tmp_error;
        messageError(os.str(), nullptr, nullptr);
    }
    return (fp);
}

auto FileStructure::make_dir() -> bool
{
    if (abstractName.back() == ".") {
        return true;
    }
    std::error_code ec;
    std::filesystem::create_directory(toString(), ec);
    if (ec) {
        msg_error(toString());
        return false;
    }
    return true;
}

auto FileStructure::make_dirs() -> bool
{
    if (abstractName.empty()) {
        return true;
    }

    std::string string_e;
    auto first_e = abstractName.begin();
    auto end_e   = abstractName.end();
    struct stat buf {
    };

    if (isAbsolute()) {
        string_e.append(prefix);
        string_e += *first_e++;
    } else {
        string_e.append(*first_e++);
    }

    while (first_e != end_e) {
        if (*first_e != ".") {
            if (::stat(string_e.c_str(), &buf) != 0) {
                if (hif_mkdir(
                        string_e.c_str(), PERMISSION_RWX_USR | PERMISSION_R_GRP | PERMISSION_X_GRP | PERMISSION_R_OTH |
                                              PERMISSION_X_OTH) != 0) {
                    if (errno != EEXIST) {
                        msg_error(string_e);
                    }
                }
            }
        }
        string_e += separator;
        string_e += *first_e++;
    }

    if (hif_mkdir(
            string_e.c_str(),
            PERMISSION_RWX_USR | PERMISSION_R_GRP | PERMISSION_X_GRP | PERMISSION_R_OTH | PERMISSION_X_OTH) != 0) {
        if (string_e != ".") {
            if (errno != EEXIST) {
                msg_error(string_e);
            }
        }
    }
    return (true);
}

auto FileStructure::symbolicLink(FileStructure &dest) -> bool
{
    std::string target = dest.toString();
    std::string link   = toString();
    std::string name   = abstractName.back();
    if (name == "." || name == ".." || target.empty()) {
        return true;
    }
    std::error_code ec;
    std::filesystem::create_symlink(target, link, ec);
    if (ec) {
        if (ec.value() == static_cast<int>(std::errc::file_exists)) {
            return true;
        }
        msg_error(link);
        return false;
    }
    return true;
}

auto FileStructure::chdir() const -> bool
{
    const std::filesystem::path path = toString();
    std::error_code ec;
    std::filesystem::current_path(path, ec);
    if (ec) {
        std::ostringstream os;
        os << "Path \"" << path.string() << "\" not found.";
        messageError(os.str(), nullptr, nullptr);
        return false;
    }
    return true;
}

auto FileStructure::rmdir() -> bool
{
    std::error_code ec;
    std::filesystem::remove_all(toString(), ec);
    if (ec) {
        msg_error(toString());
        return false;
    }
    return true;
}

auto FileStructure::rmdirs() -> bool
{
    FileStructure current = *this;
    while (!current.abstractName.empty()) {
        if (!current.rmdir()) {
            return false;
        }
        current.abstractName.pop_back();
    }
    return true;
}

auto FileStructure::rmfile() const -> bool
{
    if ((isFile()) || (isHidden())) {
        if (::remove(toString().c_str()) != 0) {
            msg_error(toString());
        }
        return (true);
    }
    return (false);
}

auto FileStructure::rmfile_weak() const -> bool
{
    if ((isFile()) || (isHidden())) {
        if (::remove(toString().c_str()) != 0) {
            return (false);
        }
        return (true);
    }
    return (false);
}

auto FileStructure::remove() -> bool
{
    if (isDirectory()) {
        return (rmdir());
    }
    return (rmfile());
}

auto FileStructure::renameTo(FileStructure dest) -> bool
{
    bool this_is_a_dir_s = isDirectory();

    erase_point_element(&abstractName);
    erase_point_element(&(dest.abstractName));
    //  if (abstractName.size() != dest.abstractName.size())
    //    return (false);
    if (!this_is_a_dir_s) {
        FileStructure parent_d = dest.getParentFile();
        if (!parent_d.make_dirs()) {
            return (false);
        }
        if (::rename(toString().c_str(), dest.toString().c_str()) != 0) {
            msg_error(toString());
        }
    } else {
        if (!dest.make_dirs()) {
            return (false);
        }
        if (!copy_dir(toString(), dest.toString())) {
            return (false);
        }
    }
    if (exists()) {
        if (!remove()) { // a la place de rmdirs
            return (false);
        }
    }
    prefix       = dest.prefix;
    separator    = dest.separator;
    absolutePath = dest.absolutePath;
    abstractName = dest.abstractName;
    return (true);
}

auto FileStructure::copyTo(FileStructure dest) -> bool
{
    bool this_is_a_dir_s = isDirectory();

    if (!dest.make_dirs()) {
        return (false);
    }
    if (!this_is_a_dir_s) {
        dest.addChild(getChild());
        if (!copy_file(toString(), dest.toString())) {
            std::ostringstream os;
            os << "Cannot copy the file \"" << toString().c_str() << "\" onto the file \"" << dest.toString().c_str()
               << "\".";
            messageError(os.str(), nullptr, nullptr);
        }
    } else {
        if (!copy_dir(toString(), dest.toString())) {
            return (false);
        }
    }
    prefix       = dest.prefix;
    separator    = dest.separator;
    absolutePath = dest.absolutePath;
    abstractName = dest.abstractName;
    return (true);
}

auto FileStructure::renameFile(const FileStructure &source) -> bool
{
    prefix       = source.prefix;
    separator    = source.separator;
    absolutePath = source.absolutePath;
    abstractName = source.abstractName;
    return (true);
}

auto FileStructure::toString() const -> std::string
{
    if (abstractName.empty()) {
        return ".";
    }

    std::string path_name;
    auto first_s = abstractName.begin();
    auto end_s   = abstractName.end();

    if (isAbsolute()) {
        path_name.append(prefix);
    } else {
        path_name.append(*first_s++);
    }

    while (first_s != end_s) {
        if (first_s != abstractName.begin()) {
            path_name += separator;
        }
        path_name += *first_s++;
    }
    return (path_name);
}

auto FileStructure::eval(const std::string &path) -> std::string
{
    std::string res          = path;
    std::string::size_type i = res.find("$SEP$");
    while (i != std::string::npos) {
        res.replace(i, 5, FileStructure::DefaultSeparator);
        i = res.find("$SEP$");
    }
    i = res.find("$PRE$");
    if (i != std::string::npos) {
        res.replace(i, 5, FileStructure::DefaultPrefix);
    }
    return res;
}

} // namespace application_utils
} // namespace hif
