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

#ifdef __WIN32__
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
#ifdef __WIN32__
    if ((pathname[0] && pathname[1]==':' && (pathname[2]=='/' || pathname[2]=='\\')) ||
        ((pathname[0]=='/' || pathname[0]=='\\') && (pathname[1]=='/' || pathname[1]=='\\')))
        return std::string(pathname);  // already absolute

    char wd[_MAX_PATH];
    if (pathname[0] && pathname[1]==':') // drive only, must get cwd on that drive
        return std::string(_getdcwd(toupper(pathname[0])-'A'+1,wd,_MAX_PATH)) + "\\" + (pathname+2);
    if (pathname[0]=='/' || pathname[1]=='\\')
    {
        // directory only, must prepend with current drive
        wd[0] = 'A'+_getdrive()-1;
        wd[1] = ':';
        wd[2] = '\0';
        return std::string(wd) + pathname;
    }
    return std::string(_getcwd(wd,_MAX_PATH)) + "\\" + pathname;
#else
    if (pathname[0] == '/')
        return std::string(pathname);  // already absolute

    char wd[1024];
    return std::string(getcwd(wd,1024)) + "/" + pathname;
#endif
}

#endif

