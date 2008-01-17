//==========================================================================
//  FILEUTIL.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _WIN32
#include <direct.h>
#include <stdlib.h> // _MAX_PATH
#else
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <errno.h>
#include <string>
#include <vector>

#include "opp_ctype.h"
#include "fileutil.h"
#include "stringtokenizer.h"
#include "exception.h"

USING_NAMESPACE


void splitFileName(const char *pathname, std::string& dir, std::string& fnameonly)
{
    if (!pathname || !*pathname)
    {
         dir = ".";
         fnameonly = "";
         return;
    }

    dir = pathname;

    // find last "/" or "\"
    const char *s = pathname + strlen(pathname) - 1;
    while (s>=pathname && *s!='\\' && *s!='/') s--;

    // split along that
    if (s<pathname)
    {
        fnameonly = pathname;
        dir = ".";
    }
    else
    {
        fnameonly = s+1;
        dir = "";
        dir.append(pathname, s-pathname+1);
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
    if ((pathname[0]=='/' || pathname[0]=='\\') && (pathname[1]=='/' || pathname[1]=='\\')) {
        prefix = std::string(DELIM) + DELIM;
        prefixlen = 2;
    }
    else if (pathname[0]=='/' || pathname[0]=='\\') {
        prefix = DELIM;
        prefixlen = 1;
    }
    else if (pathname[0] && pathname[1]==':' && (pathname[2]=='/' || pathname[2]=='\\')) {
        prefix = std::string(pathname, 2) + DELIM;
        prefixlen = 3;
    }
    else if (pathname[0] && pathname[1]==':') {
        prefix = std::string(pathname, 2);
        prefixlen = 2;
    }

    // split it to segments, so that we can normalize ".."'s
    // Note: tokenizer will also swallow multiple slashes
    std::vector<std::string> segments;
    StringTokenizer tokenizer(pathname+prefixlen, "/\\");
    while (tokenizer.hasMoreTokens())
    {
        const char *segment = tokenizer.nextToken();
        if (!strcmp(segment, "."))
            continue; // ignore "."
        if (!strcmp(segment, "..")) {
            const char *lastsegment = segments.empty() ? NULL : segments.back().c_str();
            bool canPop = lastsegment!=NULL &&
                          strcmp(lastsegment, "..")!=0 &&  // don't pop ".."
                          strchr(lastsegment, ':')==NULL;  // hostname prefix or something, don't pop
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
    for (int i=1; i<(int)segments.size(); i++)
        result = result + DELIM + segments[i];
    return result;
}

std::string absolutePath(const char *pathname)
{
#ifdef _WIN32
    if ((pathname[0] && pathname[1]==':' && (pathname[2]=='/' || pathname[2]=='\\')) ||
        ((pathname[0]=='/' || pathname[0]=='\\') && (pathname[1]=='/' || pathname[1]=='\\')))
        return std::string(pathname);  // already absolute

    char wd[_MAX_PATH];
    if (pathname[0] && pathname[1]==':') // drive only, must get cwd on that drive
    {
        if (!_getdcwd(opp_toupper(pathname[0])-'A'+1,wd,_MAX_PATH))
            return std::string(pathname);  // error (no such drive?), cannot help
        return std::string(wd) + "\\" + (pathname+2);
    }
    if (pathname[0]=='/' || pathname[0]=='\\')
    {
        // directory only, must prepend with current drive
        wd[0] = 'A'+_getdrive()-1;
        wd[1] = ':';
        wd[2] = '\0';
        return std::string(wd) + pathname;
    }
    if (!_getcwd(wd,_MAX_PATH))
        return std::string(pathname);  // error, cannot help
    return std::string(wd) + "\\" + pathname;
#else
    if (pathname[0] == '/')
        return std::string(pathname);  // already absolute

    char wd[1024];
    return std::string(getcwd(wd,1024)) + "/" + pathname;
#endif
}

std::string concatDirAndFile(const char *basedir, const char *pathname)
{
#ifdef _WIN32
    if ((pathname[0] && pathname[1]==':' && (pathname[2]=='/' || pathname[2]=='\\')) ||
        ((pathname[0]=='/' || pathname[0]=='\\') && (pathname[1]=='/' || pathname[1]=='\\')))
        return std::string(pathname);  // pathname absolute: no need to concat

    if (pathname[0] && pathname[1]==':') // drive only
    {
        if (!basedir[0] || basedir[1]!=':' || opp_toupper(basedir[0])!=opp_toupper(pathname[0]))  // no or different drive letter
            return std::string(pathname);  // possibly different drives: don't touch pathname
        return std::string(basedir) + "\\" + (pathname+2);
    }
    if (pathname[0]=='/' || pathname[0]=='\\')  // directory only (no drive letter)
    {
        // must prepend with drive from basedir if it has one
        if (!basedir[0] || basedir[1]!=':')  // no drive letter
            return std::string(pathname);  // possibly different drives: don't touch pathname
        return std::string(basedir,2)+pathname;
    }
    return std::string(basedir) + "\\" + pathname;
#else
    if (pathname[0] == '/')
        return std::string(pathname);  // pathname absolute: no need to concat
    return std::string(basedir) + "/" + pathname;
#endif
}

bool isDirectory(const char *pathname)
{
    struct stat statbuf;
    if (stat(pathname, &statbuf) != 0)
        throw opp_runtime_error("cannot stat file '%s': %s", pathname, strerror(errno));
    return statbuf.st_mode & S_IFDIR;
}

//----

PushDir::PushDir(const char *changetodir)
{
    if (!changetodir)
        return;
    char buf[1024];
    if (!getcwd(buf,1024))
        throw opp_runtime_error("cannot get the name of current directory");
    if (chdir(changetodir))
        throw opp_runtime_error("cannot temporarily change to directory `%s' (does it exist?)", changetodir);
    olddir = buf;
}

PushDir::~PushDir()
{
    if (!olddir.empty())
    {
        if (chdir(olddir.c_str()))
            throw opp_runtime_error("cannot change back to directory `%s'", olddir.c_str());
    }
}

