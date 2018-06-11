//==========================================================================
//  FILEUTIL.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
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
#endif
#include <sys/stat.h>
#include <cstring>
#include <cerrno>
#include <string>
#include <vector>

#include "omnetpp/platdep/platmisc.h"  // mkdir
#include "opp_ctype.h"
#include "fileutil.h"
#include "stringtokenizer.h"
#include "exception.h"

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

std::string directoryOf(const char *pathname)
{
    std::string dir, dummy;
    splitFileName(pathname, dir, dummy);
    return dir;
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

    char wd[1024];
    return std::string(getcwd(wd, 1024)) + "/" + pathname;  // XXX results in double slash if wd is the root
#endif
}

std::string concatDirAndFile(const char *basedir, const char *pathname)
{
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

//----

PushDir::PushDir(const char *changetodir)
{
    if (!changetodir)
        return;
    char buf[1024];
    if (!getcwd(buf, 1024))
        throw opp_runtime_error("Cannot get the name of current directory");
    if (chdir(changetodir))
        throw opp_runtime_error("Cannot temporarily change to directory '%s' (does it exist?)", changetodir);
    olddir = buf;
}

PushDir::~PushDir() noexcept(false)
{
    if (!olddir.empty()) {
        if (chdir(olddir.c_str()))
            throw opp_runtime_error("Cannot change back to directory '%s'", olddir.c_str());
    }
}

}  // namespace common
}  // namespace omnetpp

