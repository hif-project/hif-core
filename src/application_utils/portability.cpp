/// @file portability.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/types.h>

static_assert(sizeof(void *) == 8, "HIF requires to be compiled with 64-bit compiler");

#if defined(_WIN32)
#    include <direct.h>
#    include <fcntl.h>
#    include <io.h>
#    include <windows.h>
#else
#    include <sys/time.h>
#    include <unistd.h>
#endif

#if (defined _MSC_VER)
#    include <windows.h>
#    pragma warning(push)
// disabling unreferenced params under Windows
#    pragma warning(disable : 4100)
#elif (defined __MINGW32__)
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#    pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#include "hif/application_utils/Log.hpp"
#include "hif/application_utils/portability.hpp"

namespace hif
{
namespace application_utils
{

auto hif_mkdir(const char *path, int mode) -> int
{
// Also for MinGW
#if (defined _WIN32)
    return _mkdir(path);
#else
    return mkdir(path, static_cast<__mode_t>(mode));
#endif
}

auto hif_strdup(const char *s) -> char *
{
#if defined(__APPLE__) || defined(__linux__)
    return strdup(s);
#elif defined(_WIN32)
    return _strdup(s);
#else
#    error "strdup not supported on this platform"
#endif
}

auto hif_fmemopen(const char *buffer, int size, const char *mode) -> FILE *
{
#if defined(__APPLE__) || defined(__linux__)
    // POSIX systems have fmemopen
    return fmemopen(const_cast<char *>(buffer), static_cast<size_t>(size), mode);
#elif defined(_WIN32)
    // Windows: simulate fmemopen using a temporary file
    char tmp_path[L_tmpnam];
    if (tmpnam_s(tmp_path, L_tmpnam) != 0) {
        return nullptr;
    }
    FILE *fp = std::fopen(tmp_path, "w+b");
    if (!fp) {
        return nullptr;
    }
    // Write buffer into the file.
    if (std::fwrite(buffer, 1, size, fp) != static_cast<size_t>(size)) {
        std::fclose(fp);
        std::remove(tmp_path);
        return nullptr;
    }
    // Rewind for reading.
    std::rewind(fp);
    // Caller is responsible for closing the file.
    // Temporary file will be deleted when closed if using _unlink/_close trick.
    return fp;
#else
#    error "_fmemopen not supported on this platform"
#endif
}

auto hif_getCurrentTimeAsString() -> std::string
{
#if (defined _MSC_VER)
    int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";
#    if 0
    char result[100] = {0};
    //static DWORD first = GetTickCount();
    snprintf(result, 100, "%s"/*.%06ld*/, buffer/*, (long)(GetTickCount() - first)*/);

#    endif

    std::string result;
    result = buffer;
    return result;
#else
    struct timeval tv {
    };
    gettimeofday(&tv, nullptr);
    char buffer[100];
#    ifdef __MINGW32__
    time_t tt = tv.tv_sec;
    strftime(buffer, sizeof(buffer), "%X", localtime(&tt));
#    else
    tm r{};
    strftime(buffer, sizeof(buffer), "%X", localtime_r(&tv.tv_sec, &r));
#    endif
    char result[100];
    snprintf(result, 100, "%s" /*.%06ld"*/, buffer /*, (long)tv.tv_usec*/);
    return result;
#endif
}

auto hif_getCurrentDateAsString() -> std::string
{
    time_t rawtime      = 0;
    struct tm *timeinfo = nullptr;
    char buffer[100];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    if (strftime(buffer, 100, "%Y/%m/%d", timeinfo) == 0) {
        // error
        return "";
    }
    return buffer;
}

auto hif_getCurrentDateAndTimeAsString() -> std::string
{
    time_t rawtime      = 0;
    struct tm *timeinfo = nullptr;
    char buffer[100];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    if (strftime(buffer, 100, "%Y/%m/%d %H:%M:%S", timeinfo) == 0) {
        // error
        return "";
    }
    return buffer;
}

auto hif_getCurrentDateAndTimeAsFMIStringFormat() -> std::string
{
    time_t rawtime      = 0;
    struct tm *timeinfo = nullptr;
    char buffer[100];

    time(&rawtime);
    timeinfo = gmtime(&rawtime);
    if (strftime(buffer, 100, "%Y-%m-%dT%H:%M:%S", timeinfo) == 0) {
        // error
        return "";
    }
    return buffer;
}

#if (defined _MSC_VER)
#    pragma warning(pop)
#endif

// Constants.
// Also for MinGW
#if (defined _WIN32)
int PERMISSION_RWX_USR = _S_IWRITE | _S_IREAD;
int PERMISSION_R_USR   = _S_IREAD;
int PERMISSION_W_USR   = _S_IWRITE;
int PERMISSION_X_USR   = 0;
int PERMISSION_RWX_GRP = _S_IWRITE | _S_IREAD;
int PERMISSION_R_GRP   = _S_IREAD;
int PERMISSION_W_GRP   = _S_IWRITE;
int PERMISSION_X_GRP   = 0;
int PERMISSION_RWX_OTH = _S_IWRITE | _S_IREAD;
int PERMISSION_R_OTH   = _S_IREAD;
int PERMISSION_W_OTH   = _S_IWRITE;
int PERMISSION_X_OTH   = 0;
#else
int PERMISSION_RWX_USR = S_IRWXU;
int PERMISSION_R_USR   = S_IRUSR;
int PERMISSION_W_USR   = S_IWUSR;
int PERMISSION_X_USR   = S_IXUSR;
int PERMISSION_RWX_GRP = S_IRWXG;
int PERMISSION_R_GRP   = S_IRGRP;
int PERMISSION_W_GRP   = S_IWGRP;
int PERMISSION_X_GRP   = S_IXGRP;
int PERMISSION_RWX_OTH = S_IRWXO;
int PERMISSION_R_OTH   = S_IROTH;
int PERMISSION_W_OTH   = S_IWOTH;
int PERMISSION_X_OTH   = S_IXOTH;

#endif
} // namespace application_utils
} // namespace hif

/*
	// FILE SYSTEM SUPPORT
	#define symlink(x,y) 0
	#define S_ISLNK(X) 0 // windows don't have links
	#define S_ISDIR(x) x & S_IFDIR

    #define make_dir_hif_macro(x,y) _mkdir(x)

	#define FILE_SIZE st_size // field of stat construct
	#define DIR_PERM _S_IREAD | _S_IWRITE
	#define EXE_PERM _S_IREAD | _S_IWRITE
	// END FILE SYSTEM SUPPORT

    // Linux
	#include <dirent.h>
	#include <unistd.h>

	#define FILE_SIZE st_blocks // field of stat construct
	#define DIR_PERM S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
	#define EXE_PERM S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

	#define make_dir_hif_macro(x,y) mkdir(x,y)

#endif
#endif
	*/
