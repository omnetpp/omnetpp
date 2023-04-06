//==========================================================================
//  FILEUTIL.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _WIN32
#include <direct.h>
#include <cstdlib>  // _MAX_PATH
#else
#include <unistd.h>
#include <limits.h>  // PATH_MAX
#endif
#include <sys/stat.h>
#include <cstring>
#include <cerrno>
#include <string>
#include <vector>

#include "omnetpp/platdep/platmisc.h"  // mkdir
#include "opp_ctype.h"
#include "fileutil.h"
#include "fileglobber.h"
#include "stringutil.h"
#include "stringtokenizer.h"
#include "exception.h"
#include "commonutil.h"

namespace omnetpp {
namespace common {

std::string fileNameToSlash(const char *fileName)
{
    std::string res;
    res.reserve(strlen(fileName));
    for ( ; *fileName; fileName++)
        res.append(1, *fileName == '\\' ? '/' : *fileName);
    return res;
}

void splitFileName(const char *pathname, std::string& dir, std::string& fnameonly)
{
    if (!pathname || !*pathname) {
        dir = ".";
        fnameonly = "";
        return;
    }

    // find last "/" or "\"
    const char *s = pathname + strlen(pathname) - 1;
    s--;  // ignore potential trailing "/"
    while (s > pathname && *s != '\\' && *s != '/')
        s--;
    const char *sep = s <= pathname ? nullptr : s;

    // split along that
    if (!sep) {
        // no slash or colon
        if (strchr(pathname, ':') || strcmp(pathname, ".") == 0 || strcmp(pathname, "..") == 0) {
            fnameonly = "";
            dir = pathname;
        }
        else {
            fnameonly = pathname;
            dir = ".";
        }
    }
    else {
        fnameonly = s+1;
        dir = std::string(pathname, s-pathname+1);
    }
}

std::string directoryOf(const char *path)
{
    std::string dir, dummy;
    splitFileName(path, dir, dummy);
    return dir;
}

std::string filenameOf(const char *path)
{
    std::string dir, filename;
    splitFileName(path, dir, filename);
    return filename;
}

std::string tidyFilename(const char *pathname, bool slashes)
{
#ifdef _WIN32
    const char *DELIM = slashes ? "/" : "\\";
#else
    const char *DELIM = "/";
#endif
    // remove any prefix that needs to be treated specially: leading "/", drive letter etc.
    std::string prefix;
    int prefixlen = 0;
    if ((pathname[0] == '/' || pathname[0] == '\\') && (pathname[1] == '/' || pathname[1] == '\\')) {
        prefix = std::string(DELIM) + DELIM;
        prefixlen = 2;
    }
    else if (pathname[0] == '/' || pathname[0] == '\\') {
        prefix = DELIM;
        prefixlen = 1;
    }
    else if (pathname[0] && pathname[1] == ':' && (pathname[2] == '/' || pathname[2] == '\\')) {
        prefix = std::string(pathname, 2) + DELIM;
        prefixlen = 3;
    }
    else if (pathname[0] && pathname[1] == ':') {
        prefix = std::string(pathname, 2);
        prefixlen = 2;
    }

    // split it to segments, so that we can normalize ".."'s
    // Note: tokenizer will also swallow multiple slashes
    std::vector<std::string> segments;
    StringTokenizer tokenizer(pathname+prefixlen, "/\\");
    while (tokenizer.hasMoreTokens()) {
        const char *segment = tokenizer.nextToken();
        if (!strcmp(segment, "."))
            continue;  // ignore "."
        if (!strcmp(segment, "..")) {
            const char *lastsegment = segments.empty() ? nullptr : segments.back().c_str();
            bool canPop = lastsegment != nullptr &&
                strcmp(lastsegment, "..") != 0 &&  // don't pop ".."
                strchr(lastsegment, ':') == nullptr;  // hostname prefix or something, don't pop
            if (canPop)
                segments.pop_back();
            else
                segments.push_back(segment);
        }
        else {
            segments.push_back(segment);
        }
    }

    // reassemble from segments
    std::string result = prefix + (segments.empty() ? "." : segments[0]);
    for (int i = 1; i < (int)segments.size(); i++)
        result += DELIM + segments[i];
    return result;
}

std::string canonicalize(const char *pathname)
{
#ifdef _WIN32
    // Note: This doesn't resolve junctions, but it should.
    return tidyFilename(toAbsolutePath(pathname).c_str(), true);
#else
    char buf[PATH_MAX];
    const char *result = realpath(pathname, buf);
    if (result == nullptr)
        throw opp_runtime_error("Cannot canonicalize '%s': realpath() failed: %s", pathname, strerror(errno));
    return result;
#endif
}

std::string removeFileExtension(const char *fileName)
{
    const char *dot = strrchr(fileName, '.');
    const char *slash = strrchr(fileName, '/');
    const char *backslash = strrchr(fileName, '\\');
    if (!dot || dot < slash || dot < backslash)
        return fileName;
    return std::string(fileName, dot - fileName);
}

bool isAbsolutePath(const char *pathname)
{
#ifdef _WIN32
    return (pathname[0] && pathname[1] == ':' && (pathname[2] == '/' || pathname[2] == '\\')) ||
        ((pathname[0] == '/' || pathname[0] == '\\') && (pathname[1] == '/' || pathname[1] == '\\'));
#else
    return pathname[0] == '/';
#endif
}

std::string toAbsolutePath(const char *pathname)
{
#ifdef _WIN32
    if ((pathname[0] && pathname[1] == ':' && (pathname[2] == '/' || pathname[2] == '\\')) ||
        ((pathname[0] == '/' || pathname[0] == '\\') && (pathname[1] == '/' || pathname[1] == '\\')))
        return std::string(pathname);  // already absolute

    char wd[_MAX_PATH];
    if (pathname[0] && pathname[1] == ':') {  // drive only, must get cwd on that drive
        if (!_getdcwd(opp_toupper(pathname[0])-'A'+1, wd, _MAX_PATH))
            return std::string(pathname);  // error (no such drive?), cannot help
        return std::string(wd) + "\\" + (pathname+2);
    }
    if (pathname[0] == '/' || pathname[0] == '\\') {
        // directory only, must prepend with current drive
        wd[0] = 'A'+_getdrive()-1;
        wd[1] = ':';
        wd[2] = '\0';
        return std::string(wd) + pathname;
    }
    if (!_getcwd(wd, _MAX_PATH))
        return std::string(pathname);  // error, cannot help
    return std::string(wd) + "\\" + pathname;  // XXX results in double backslash if wd is the root
#else
    if (pathname[0] == '/')
        return std::string(pathname);  // already absolute

    char wd[PATH_MAX];
    return std::string(getcwd(wd, PATH_MAX)) + "/" + pathname;  // XXX results in double slash if wd is the root
#endif
}

std::string concatDirAndFile(const char *basedir, const char *pathname)
{
    if (opp_isempty(basedir))
        return pathname;
#ifdef _WIN32
    if ((pathname[0] && pathname[1] == ':' && (pathname[2] == '/' || pathname[2] == '\\')) ||
        ((pathname[0] == '/' || pathname[0] == '\\') && (pathname[1] == '/' || pathname[1] == '\\')))
        return std::string(pathname);  // pathname absolute: no need to concat

    if (pathname[0] && pathname[1] == ':') {  // drive only
        if (!basedir[0] || basedir[1] != ':' || opp_toupper(basedir[0]) != opp_toupper(pathname[0]))  // no or different drive letter
            return std::string(pathname);  // possibly different drives: don't touch pathname
        return std::string(basedir) + "\\" + (pathname+2);
    }
    if (pathname[0] == '/' || pathname[0] == '\\') {  // directory only (no drive letter)
        // must prepend with drive from basedir if it has one
        if (!basedir[0] || basedir[1] != ':')  // no drive letter
            return std::string(pathname);  // possibly different drives: don't touch pathname
        return std::string(basedir, 2)+pathname;
    }
    return std::string(basedir) + "\\" + pathname;
#else
    if (pathname[0] == '/')
        return std::string(pathname);  // pathname absolute: no need to concat
    return std::string(basedir) + "/" + pathname;
#endif
}

/*
   static std::string removeTrailingSlash(const char *pathname)
   {
    char lastChar = *(pathname+strlen(pathname)-1);
    return (lastChar=='/' || lastChar=='\\') ? std::string(pathname, strlen(pathname)-1) : pathname;

   }
 */

std::string getWorkingDir()
{
    char wd[1024];
    return std::string(getcwd(wd, 1024));
}

bool fileExists(const char *pathname)
{
    // Note: stat("foo/") ==> error, even when "foo" exists and is a directory!
    struct stat statbuf;
    return stat(pathname, &statbuf) == 0;
}

bool isFile(const char *pathname)
{
    struct stat statbuf;
    int err = stat(pathname, &statbuf);  // note: do not throw if file/directory does not exist or there's some other error, see bug #284
    return err == 0 && (statbuf.st_mode & S_IFREG) != 0;
}

bool isDirectory(const char *pathname)
{
    struct stat statbuf;
    int err = stat(pathname, &statbuf);  // note: do not throw if file/directory does not exist or there's some other error, see bug #284
    return err == 0 && (statbuf.st_mode & S_IFDIR) != 0;
}

void removeFile(const char *fname, const char *descr)
{
    if (unlink(fname) != 0 && errno != ENOENT)
        throw opp_runtime_error("Cannot remove %s '%s': %s", descr, fname, strerror(errno));
}

void mkPath(const char *pathname)
{
    if (!fileExists(pathname)) {
        std::string pathprefix, dummy;
        splitFileName(pathname, pathprefix, dummy);
        mkPath(pathprefix.c_str());
        // note: anomaly with slash-terminated dirnames: stat("foo/") says
        // it does not exist, and mkdir("foo/") says cannot create (EEXIST):
        if (opp_mkdir(pathname, 0755) != 0 && errno != EEXIST)
            throw opp_runtime_error("Cannot create directory '%s': %s", pathname, strerror(errno));
    }
}

static void doCollectFilesInDirectory(std::vector<std::string>& result, const std::string& prefix, bool deep, const char *suffix)
{
    FileGlobber globber("*");
    const char *filename;
    while ((filename = globber.getNext()) != nullptr) {
        if (filename[0] == '.')
            continue;  // ignore ".", "..", and dotfiles
        else if (isDirectory(filename)) {
            if (deep) {
                PushDir pushDir(filename);
                doCollectFilesInDirectory(result, prefix + filename + "/", deep, suffix);
            }
        }
        else if (suffix == nullptr || opp_stringendswith(filename, suffix))
            result.push_back(prefix + filename);
    }
}

std::vector<std::string> collectFilesInDirectory(const char *foldername, bool deep, const char *suffix)
{
    std::vector<std::string> result;
    PushDir pushDir(foldername);
    doCollectFilesInDirectory(result, std::string(foldername) == "." ? "" : std::string(foldername) + "/", deep, suffix);
    return result;
}

void doCollectMatchingFiles(const std::vector<std::string>& segments, int index, const std::string& prefix, std::vector<std::string>& result)
{
    const char *segment = segments[index].c_str();
    if (strcmp(segment, "**") == 0) {
        if (index+1 != segments.size())
            doCollectMatchingFiles(segments, index+1, prefix, result);

        // go into all directories with this segment again
        FileGlobber globber("*");
        const char *name;
        while ((name = globber.getNext()) != nullptr) {
            if (name[0] == '.')
                continue;  // ignore ".", "..", and dotfiles

            if (isDirectory(name)) {
                PushDir pushDir(name);
                if (index+1 != segments.size())
                    doCollectMatchingFiles(segments, index, prefix + name + "/", result); // note: NOT index+1!
            }
        }
    }
    else if (strchr(segment, '*') == nullptr && strchr(segment, '?') == nullptr) {
        // no wildcard -- skip globbing
        // note: this block is exactly like the next one, only without the globbing
        const char *name = segment;
        if (index == segments.size()-1) {
            result.push_back(prefix + name);
        }

        if (isDirectory(name)) {
            PushDir pushDir(name);
            if (index+1 != segments.size())
                doCollectMatchingFiles(segments, index+1, prefix + name + "/", result);
        }
    }
    else {
        // glob it
        FileGlobber globber(segment);
        const char *name;
        while ((name = globber.getNext()) != nullptr) {
            if (name[0] == '.') {
                if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 || segment[0] != '.')
                    continue;  // ignore "." and ".." (always), and dotfiles (unless the segment starts with ".")
            }

            if (index == segments.size()-1) {
                result.push_back(prefix + name);
            }

            if (isDirectory(name)) {
                PushDir pushDir(name);
                if (index+1 != segments.size())
                    doCollectMatchingFiles(segments, index+1, prefix + name + "/", result);
            }
        }
    }


}

std::vector<std::string> collectMatchingFiles(const char *pattern)
{
    // split to segments:
    //  "foo/bar/" -> ["foo", "bar"]
    //  "foo//bar///" -> ["foo", "bar"]
    //  "fo*/b?r" -> ["fo*", "b?r"]
    //  "foo/**/bar" -> ["foo", "**", "bar"]
    //  "foo**bar" -> ["foo*", "**", "*bar"]
    //  "foo/**bar" -> ["foo", "**", "*bar"]
    //  "foo**/bar" -> ["foo*", "**", "bar"]
    //  "/foo/bar" -> ["/foo", "bar"]
    //  "//foo/bar" -> ["//foo", "bar"]
    //  "c:/foo/bar" (on Windows) -> ["c:/foo", "bar"]
    //  "c:foo/bar" (on Windows) -> ["c:foo", "bar"]
    //
    std::vector<std::string> result;

    std::string tidyPattern = tidyFilename(pattern, true);
    pattern = tidyPattern.c_str();

    const char *firstwildcard = pattern + strcspn(pattern, "/*?");

    // if no wildcard, simply return it as a file (if exists)
    if (!*firstwildcard) {
        if (isFile(pattern))
            result.push_back(pattern);
        return result;
    }

    std::vector<std::string> segments;
    const char *segstart = pattern;

    const char *s = pattern;

#ifdef _WIN32
    // skip drive letter/device name (they'll be included in the first segment)
    while (*s && *s != ':' && *s != '/')
        s++;
    if (*s != ':')
        s = pattern; // reset
    else
        s++; // skip colon
#endif

    // skip leading slashes (they'll be included in the first segment)
    while (*s == '/')
        s++;

    // parse segments
    while (*s) {
        if (*s == '/') {
            segments.push_back(std::string(segstart, s-segstart));
            while (*s == '/')
                s++;
            segstart = s;
        }
        else if (*s == '*' && *(s+1) == '*') {
            if (segstart != s)
                segments.push_back(std::string(segstart, s-segstart+1)); // include trailing asterisk
            if (segments.empty() || segments.back() != "**")
                segments.push_back("**");
            while (*s == '*')
                s++; // skip asterisks
            if (*s && *s != '/')
                segstart = s-1;  // include leading asterisk
            else {
                while (*s == '/')
                    s++;
                segstart = s;
            }
        }
        else {
            s++;
        }
    }
    if (s != segstart)
        segments.push_back(std::string(segstart, s-segstart)); // last segment

    // recurse
    doCollectMatchingFiles(segments, 0, "", result);
    return result;
}

//----

PushDir::PushDir(const char *changetodir)
{
    if (!changetodir)
        return;
    char buf[1024];
    if (!getcwd(buf, 1024))
        throw opp_runtime_error("Cannot get the name of current directory: %s", strerror(errno));
    if (chdir(changetodir))
        throw opp_runtime_error("Cannot temporarily change to directory '%s': %s", changetodir, strerror(errno));
    olddir = buf;
}

PushDir::~PushDir() noexcept(false)
{
    if (!olddir.empty()) {
        if (chdir(olddir.c_str()))
            panic(opp_runtime_error("Cannot change back to directory '%s': %s", olddir.c_str(), strerror(errno)));
    }
}

}  // namespace common
}  // namespace omnetpp

