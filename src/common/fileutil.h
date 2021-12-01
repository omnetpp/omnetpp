//==========================================================================
//  FILEUTIL.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_FILEUTIL_H
#define __OMNETPP_COMMON_FILEUTIL_H

#include <string>
#include <vector>
#include "commondefs.h"

namespace omnetpp {
namespace common {

#ifdef _WIN32
#define PATH_SEPARATOR   ";"
#else
#define PATH_SEPARATOR   ":;"
#endif

/**
 * Convert backslashes to slashes.
 */
COMMON_API std::string fileNameToSlash(const char *fileName);

/**
 * Splits the last segment of the given path. Use only with canonical absolute paths!
 * (In extreme cases and irregularities in the path the result is unpredictable)
 */
COMMON_API void splitFileName(const char *path, std::string& dir, std::string& filename);

/**
 * Calls splitFileName() and returns the dir part of the result
 */
COMMON_API std::string directoryOf(const char *path);

/**
 * Calls splitFileName() and returns the filename part of the result
 */
COMMON_API std::string filenameOf(const char *path);

/**
 * Canonicalizes the given path. For example, changes backslashes to slashes
 * (or the other way round, depending on the slashes arg), kills off multiple slashes,
 * dot and dot-dot components ((foo//bar --> foo/bar, foo/./bar --> foo/bar,
 * foo/../bar --> bar), etc. You may want to convert the path to absolute
 * (toAbsolutePath()) before calling this function.
 */
COMMON_API std::string tidyFilename(const char *pathname, bool slashes=false);

/**
 * Returns the input file name/path without the filename extension, or unchanged
 * if it has none.
 */
COMMON_API std::string removeFileExtension(const char *fileName);

/**
 * Returns true if this is an absolute path.
 */
COMMON_API bool isAbsolutePath(const char *pathname);

/**
 * Converts the given path to absolute.
 */
COMMON_API std::string toAbsolutePath(const char *pathname);

/**
 * Concatenates the given directory and file name or relative path.
 */
COMMON_API std::string concatDirAndFile(const char *basedir, const char *pathname);

/**
 * Returns the current working directory
 */
COMMON_API std::string getWorkingDir();

/**
 * Returns true if given file or directory exists
 */
COMMON_API bool fileExists(const char *pathname);

/**
 * Returns true if the given pathname exists in the file system and is a
 * regular file, and false otherwise.
 */
COMMON_API bool isFile(const char *pathname);

/**
 * Returns true if the given pathname exists in the file system and is a directory,
 * and false otherwise.
 */
COMMON_API bool isDirectory(const char *pathname);

/**
 * Removes the given file, or does nothing if such file does not exist.
 * Throws an error if the file exists but could not be removed.
 */
COMMON_API void removeFile(const char *fname, const char *descr);

/**
 * Recursively creates all directories in the specified path
 */
COMMON_API void mkPath(const char *pathname);

/**
 * Collect files in the specified folder with the given file suffix (normally
 * a file extension with the dot included, e.g. ".txt"). Specify suffix=nullptr
 * to collect all files. If deep=true, subdirectories are also searched,
 * otherwise only the given directory. See also collectMatchingFiles().
 */
COMMON_API std::vector<std::string> collectFilesInDirectory(const char *foldername, bool deep, const char *suffix=nullptr);

/**
 * Glob a path that may contain '?', '*' and '**' wildcards. '**' can match
 * multiple directory levels. This is similar to the "globstar" mode in bash.
 * See also collectFilesInDirectory().
 */
COMMON_API std::vector<std::string> collectMatchingFiles(const char *globstarPattern);

/**
 * Utility class for temporary change of directory. Changes back to
 * original dir when goes out of scope. Does nothing if nullptr is passed
 * to the constructor.
 */
class COMMON_API PushDir
{
  private:
    std::string olddir;
  public:
    PushDir(const char *changetodir);
    ~PushDir() noexcept(false);
};

}  // namespace common
}  // namespace omnetpp

#endif

