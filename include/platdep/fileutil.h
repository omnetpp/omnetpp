//==========================================================================
//  PLATDEP/FILEUTIL.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __PLATDEP_FILEUTIL_H
#define __PLATDEP_FILEUTIL_H

#ifdef _WIN32
#include <direct.h>
#include <stdlib.h> // _MAX_PATH
#include <ctype.h>
#else
#include <unistd.h>
#endif

#include <string>

inline void splitFileName(const char *pathname, std::string& dir, std::string& fnameonly)
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

inline std::string directoryOf(const char *pathname)
{
    std::string dir, dummy;
    splitFileName(pathname, dir, dummy);
    return dir;
}

inline std::string absolutePath(const char *pathname)
{
#ifdef _WIN32
    if ((pathname[0] && pathname[1]==':' && (pathname[2]=='/' || pathname[2]=='\\')) ||
        ((pathname[0]=='/' || pathname[0]=='\\') && (pathname[1]=='/' || pathname[1]=='\\')))
        return std::string(pathname);  // already absolute

    char wd[_MAX_PATH];
    if (pathname[0] && pathname[1]==':') // drive only, must get cwd on that drive
    {
        if (!_getdcwd(toupper(pathname[0])-'A'+1,wd,_MAX_PATH))
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

inline std::string concatDirAndFile(const char *basedir, const char *pathname)
{
#ifdef _WIN32
    if ((pathname[0] && pathname[1]==':' && (pathname[2]=='/' || pathname[2]=='\\')) ||
        ((pathname[0]=='/' || pathname[0]=='\\') && (pathname[1]=='/' || pathname[1]=='\\')))
        return std::string(pathname);  // pathname absolute: no need to concat

    if (pathname[0] && pathname[1]==':') // drive only
    {
        if (!basedir[0] || basedir[1]!=':' || toupper(basedir[0])!=toupper(pathname[0]))  // no or different drive letter
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

#endif

