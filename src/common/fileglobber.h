//==========================================================================
//  FILEGLOBBER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FILEGLOBBER_H
#define __FILEGLOBBER_H

#ifdef _WIN32
#include <io.h>
#include <stdlib.h> // _MAX_PATH
#else
#include <sys/types.h>
#include <glob.h>
#endif

#include <string>
#include <vector>
#include <exception>
#ifndef _MSC_VER
#include <stdexcept>   // std::runtime_exception (with MSVC, it's in <exception>)
#endif

#ifdef _WIN32
#define SHELL_EXPANDS_WILDCARDS 0
#else
#define SHELL_EXPANDS_WILDCARDS 1
#endif

#include "commondefs.h"

NAMESPACE_BEGIN

struct GlobPrivateData;

/**
 * Utility for resolving wildcards in file names. If the pattern doesn't contain
 * a wildcard, it is returned as it is, regardless whether such file exists
 * or not.
 */
class COMMON_API FileGlobber
{
  private:
    std::string fnamepattern;
    GlobPrivateData *data;
  public:
    /**
     * Constructor. Accepts the wildcard pattern to glob.
     */
    FileGlobber(const char *filenamepattern);

    /**
     * Destructor
     */
    ~FileGlobber();

    /**
     * Returns a file names, one at each call, and finally it returns NULL.
     * Caution: may throw std::runtime_exception!
     */
    const char *getNext();

    /**
     * Returns all matching files at once. Using this method also solves problems
     * that would occur when new matching files get created during iteration.
     */
    std::vector<std::string> getFilenames();
};

NAMESPACE_END


#endif

