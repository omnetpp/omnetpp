//==========================================================================
//   FSUTILS.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    TOmnetApp:  abstract base class for simulation applications
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FSUTILS_H
#define __FSUTILS_H

#include "util.h"

/**
 * Wrapper around Unix getpid().
 */
long getProcessId();

/**
 * Utility function to split a file path into directory and file name parts.
 */
void splitFileName(const char *pathname, opp_string& dir, opp_string& fnameonly);

/**
 * Utility class for temporary change of directory. Changes back to
 * original dir when goes out of scope.
 */
class PushDir
{
  private:
    opp_string olddir;
  public:
    PushDir(const char *changetodir);
    ~PushDir();
};


struct GlobPrivateData;

/**
 * Utility for resolving wildcards in file names
 */
class Globber
{
  private:
    opp_string fnamepattern;
    GlobPrivateData *data;
  public:
    Globber(const char *filenamepattern);
    ~Globber();
    const char *getNext();
};

#endif

