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

void splitFileName(const char *pathname, std::string& dir, std::string& fnameonly);
std::string directoryOf(const char *pathname);
std::string tidyFilename(const char *pathname, bool slashes=false);
std::string absolutePath(const char *pathname);
std::string concatDirAndFile(const char *basedir, const char *pathname);

#endif

