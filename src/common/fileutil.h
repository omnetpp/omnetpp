//==========================================================================
//  FILEUTIL.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FILEUTIL_H
#define __FILEUTIL_H

#include <string>
#include "commondefs.h"

COMMON_API void splitFileName(const char *pathname, std::string& dir, std::string& fnameonly);
COMMON_API std::string directoryOf(const char *pathname);
COMMON_API std::string tidyFilename(const char *pathname, bool slashes=false);
COMMON_API std::string absolutePath(const char *pathname);
COMMON_API std::string concatDirAndFile(const char *basedir, const char *pathname);
COMMON_API bool isDirectory(const char *pathname);

/**
 * Utility class for temporary change of directory. Changes back to
 * original dir when goes out of scope. Does nothing if NULL is passed
 * to the constructor.
 */
class COMMON_API PushDir
{
  private:
    std::string olddir;
  public:
    PushDir(const char *changetodir);
    ~PushDir();
};

#endif

