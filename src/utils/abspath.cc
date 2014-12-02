//=========================================================================
//  ABSPATH.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  converts the first argument to absolute path
//
//
//  Author: rhornig
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2008 Rudolf Hornig
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string>
#ifdef _WIN32
#include <direct.h>
#include <stdlib.h>
#else
#include <unistd.h>
#endif

#include "../common/ver.h"

std::string toAbsolutePath(const char *pathname)
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
    return std::string(wd) + "\\" + pathname;   //XXX results in double backslash if wd is the root
#else
    if (pathname[0] == '/')
        return std::string(pathname);  // already absolute

    char wd[1024];
    return std::string(getcwd(wd,1024)) + "/" + pathname; //XXX results in double slash if wd is the root
#endif
}

int main(int argc, char *argv[])
{

    if (argc==1)
    {
         fprintf(stderr,
               "abspath -- part of " OMNETPP_PRODUCT ", (C) 2006-2014 OpenSim Ltd.\n"
               "Version: " OMNETPP_VERSION_STR ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION "\n"
               "\n"
               "Returns the absolute path of the argument."
                );

         exit(0);
    }

    printf("%s", toAbsolutePath(argv[1]).c_str());
    return 0;
}


