/// @file FileStructure.hpp
/// @brief Provides a platform-independent API for managing file and directory
/// paths.
/// @details The `FileStructure` class offers functionality for accessing and
/// manipulating file and directory paths in a platform-independent manner. It
/// supports common file operations such as reading, writing, checking
/// existence, listing contents, managing directories, and handling symbolic
/// links.
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#pragma once

#include <cstdio>
#include <ctime>
#include <string>
#include <vector>

#include "hif/application_utils/Log.hpp"
#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace application_utils
{

/// FileStructure
/// @brief Files management API, this class provide a
/// common, platform independent access to system file names.
class FileStructure
{
public:
    /// @name Constants
    /// @{

    /// @brief Default system-specific separator (e.g., '/' on Linux, '\' on Windows).
#ifdef _MSC_VER
    static constexpr char DefaultSeparator[] = "\\";
#else
    static constexpr char DefaultSeparator[] = "/";
#endif

    /// @brief Default system prefix (e.g., root '/' on Linux, drive letter on Windows).
#ifdef _MSC_VER
    static constexpr char DefaultPrefix[] = "C:\\";
#else
    static constexpr char DefaultPrefix[] = "/";
#endif

    /// @brief Separator list used for splitting paths (platform-dependent).
#ifdef _MSC_VER
    static constexpr char SeparatorList[] = "\\/";
#else
    static constexpr char SeparatorList[] = "/";
#endif

    /// @brief File open modes for binary writing.
    static constexpr char ModeBinWrite[] = "wb";

    /// @brief File open modes for ASCII writing.
    static constexpr char ModeTxtWrite[] = "w";

    /// @brief File open modes for binary reading.
    static constexpr char ModeBinRead[] = "rb";

    /// @brief File open modes for ASCII reading.
    static constexpr char ModeTxtRead[] = "r";

    /// @}

    /// @brief Default constructor.
    FileStructure();

    /// @brief Constructor with a path name.
    /// @param path_name Path to a file or directory.
    FileStructure(const std::string &path_name);

    /// @brief Constructor combining a parent and a child path.
    /// @param parent Parent directory path.
    /// @param child Child file or directory name.
    FileStructure(const FileStructure &parent, const std::string &child);

    /// @brief Constructor combining a parent and a child path.
    /// @param parent Parent directory path.
    /// @param child Child file or directory name.
    FileStructure(const std::string &parent, const std::string &child);

    /// @brief Constructor using a vector of path components.
    /// @param path_name Components of the path.
    FileStructure(const std::vector<std::string> &path_name);

    /// @brief Destructor.
    ~FileStructure();

    /// @brief Default copy constructor.
    /// @param other The object to copy.
    FileStructure(const FileStructure &other) = default;

    /// @brief Default copy assignment operator.
    /// @param other The object to copy.
    /// @return A reference to this object.
    auto operator=(const FileStructure &other) -> FileStructure & = default;

    /// @name General FileStructure support functions.
    ///	Functions to elaborate a general FileStructure (both of file and directory)
    /// @{

    /// @brief Return true if the file structure is readable.
    /// @return true if the file structure is readable.
    static auto canRead() -> bool;

    /// @brief Return true if the file structure is writable.
    /// @return true if the file structure is writable.
    static auto canWrite() -> bool;

    /// @brief Compare two FileStructure
    /// @param path_name FileStructure to compare with this.
    /// @return -1 if the PathName.length > this.length
    /// Return -2 if the this.length > PathName.length
    /// Return 0  if the PathName != this
    /// Return 1  if the PathName == this
    auto compareTo(const FileStructure &path_name) const -> int;

    /// @brief Return true if the file structure exists.
    /// @return true if file structure exists, false otherwise.
    auto exists() const -> bool;

    /// @brief Return the absolute path of the file structure.
    /// @return absolute path FileStructure of current file structure.
    auto getAbsoluteFile() const -> FileStructure;

    /// @brief Return the absolute path of the file structure.
    /// @return string representation of path of current file structure.
    auto getAbsolutePath() const -> std::string;

    /// @brief Return the string of the file structure.
    /// @return string representation of current file structure.
    auto getName() const -> std::string;

    /// @brief Return the parent file structure of the file structure.
    /// @return FileStructure (file) representation of parent current file structure.
    auto getParentFile() const -> FileStructure;

    /// @brief Add a child file structure to the current file structure.
    /// @param string_e string representation of path to file that is added to current file structure
    void addChild(const std::string &string_e);

    /// @brief Return the local path of the file structure.
    /// @return local path of the file structure.
    auto getPath() const -> std::string;

    /// @brief Return the local path of the file structure.
    /// @return local path of the file structure.
    auto getAbstractName() const -> std::vector<std::string>;

    /// @brief Return true if the file structure is an absolute path.
    /// @return true if the file structure is an absolute path.
    auto isAbsolute() const -> bool;

    /// @brief Return true if the file structure is a symbolic link.
    /// @return true is actual FileStructure is a link
    auto isLink() const -> bool;

    /// @brief Return true if the file structure is a hidden file.
    /// @return true if actual FileStructure is a hidden file
    auto isHidden() const -> bool;

    /// @brief Return the last modified date of the file structure.
    /// @return time_t of the last modified date of the file structure
    auto lastModified() const -> time_t;

    /// @brief Return the last access date of the file structure.
    /// @return time_t of the last access date of the file structure
    auto lastAccess() const -> time_t;

    /// @brief Return the creation date of the file structure.
    /// @return time_t of the creation date of the file structure.
    auto lastAttributesChange() const -> time_t;

    /// @brief Return the length of the file structure (number of characters
    /// 	of the absolute path).
    /// @return long representation of the length of the file structure
    auto length() const -> long;

    /// @brief Return the size in blocks of the file structure.
    /// @return long representation of the size in blocks of the file structure
    auto size() const -> long;

    /// @brief Return the number of element of the file structure.
    /// @return number of element of the file structure.
    auto depth() const -> int;

    /// @brief Return a list of paths name contained into the directory
    /// corresponded to the file structure.
    /// @return a list of paths name contained into the directory corresponded to the file structure.
    auto list() const -> std::vector<std::string>;

    /// @brief Return a list of paths name filtered by the filter string
    /// contained into the directory corresponded to the file structure.
    /// @param filter string filter for filtered list of paths
    /// @return Return a vector list of paths name filtered by the filter string
    auto list(const std::string &filter) const -> std::vector<std::string>;

    /// @brief Create a symbolic link to the dest file structure.
    /// @param dest destination FileStructure/
    /// @return true on success, false otherwise.
    auto symbolicLink(FileStructure &dest) -> bool;

    /// @brief Change to the directory corresponded to the file structure.
    /// @return true on success, false otherwise.
    auto chdir() const -> bool;

    /// @brief Remove the file structure whichever it is a file or a directory.
    /// @return true on success, false otherwise.
    auto remove() -> bool;

    /// @brief Rename the directory or the file into the Dest file structure.
    /// @param dest destination FileStructure.
    /// @return true on success, false otherwise.
    auto renameTo(FileStructure dest) -> bool;

    /// @brief Copy all the directory or the file into the Dest file structure.
    /// @param dest destination FileStructure.
    /// @return true on success, false otherwise.
    auto copyTo(FileStructure dest) -> bool;

    /// @brief Rename only the file structure variable without physical action.
    /// @param source FileStructure to rename.
    /// @return true on success, false otherwise.
    auto renameFile(const FileStructure &source) -> bool;

    /// @brief Return the string of the file structure.
    /// @return string representation of actual FileStructure.
    auto toString() const -> std::string;

    /// @brief Eval separators in a string
    /// @param path string with $SEP$ and $PRE$ as platform independent separator.
    /// @return evaluate separators in the path parameter and return it.
    static auto eval(const std::string &path) -> std::string;

    /// @}

    /// @name Directory support functions.
    ///	Functions to elaborate a directory FileStructure
    /// @{

    /// @brief Return the parent directory of the file structure.
    /// @return string representation of directory name of current file structure.
    auto getParent() const -> std::string;

    /// @brief Return the child directory or file of the file structure.
    /// @return child directory or file of the file structure.
    auto getChild() const -> std::string;

    /// @brief Return true if the file structure is a directory.
    /// @return true if FileStructure is a directory
    auto isDirectory() const -> bool;

    /// @brief Create the directory (if it didn't exists) corresponded to the
    /// child abstract name of the file structure.
    /// @return true on success, false otherwise.
    auto make_dir() -> bool;

    /// @brief Create directories (if they didn't exist) corresponded to the
    /// file structure.
    /// @return true on success, false otherwise.
    auto make_dirs() -> bool;

    /// @brief Remove the child abstract name of the file structure even
    /// it is full.
    /// @return true on success, false otherwise.
    auto rmdir() -> bool;

    /// @brief Remove all directories represented by the file structure even
    /// they are full.
    /// @return true on success, false otherwise.
    auto rmdirs() -> bool;

    /// @}

    /// @name File support functions.
    ///	Functions to elaborate file FileStructure
    /// @{

    /// @brief Return true if the file structure is a file.
    /// @return true if actual FileStructure is a file
    auto isFile() const -> bool;

    /// @brief Set file to executable mode. Return chmod status.
    /// @return the chmod status
    auto setToExe() const -> int;

    /// @brief Return a list of files structure contained into the directory
    /// corresponded to the file structure.
    /// @return list of files structure contained into the directory
    auto listFiles() const -> std::vector<FileStructure>;

    /// @brief Return a list of files structure filtered by the filter string
    /// contained into the directory corresponded to the file structure.
    /// @param filter string filter for filtered list of files
    /// @return vector list of FileStructure filtered by the filter string
    auto listFiles(const std::string &filter) const -> std::vector<FileStructure>;

    /// @brief Create or open a file in a mode. There are different modes:
    /// MODE_BIN_WRITE : binary writing.
    /// MODE_TXT_WRITE : ASCII  writing.
    /// MODE_BIN_READ  : binary reading.
    /// MODE_TXT_READ  : ASCII  reading.
    /// @param mode modes to open file
    /// @return FILE* to file opened.
    auto openfile(char *mode) const -> FILE *;

    /// @brief Remove the file represented by the file structure.
    /// @return true on success, false otherwise.
    auto rmfile() const -> bool;

    /// @brief Remove the file represented by the file structure (without error msg.
    /// @return true on success, false otherwise.
    auto rmfile_weak() const -> bool;

    /// @}

private:
    /// @brief abtract name (list of fields) corresponding to a path
    std::vector<std::string> abstractName;

    /// @brief Start point of an absolute path
    std::string prefix;

    /// @brief Separator between fields inside an abstract name
    std::string separator;

    /// @brief Flag to identify if abstractName is an absolute path or not
    bool absolutePath;
};

} // namespace application_utils
} // namespace hif