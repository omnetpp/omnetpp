//==========================================================================
//  PLATDEP/MISC.H - part of
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

#ifndef __PLATDEP_FILEGLOBBER_H
#define __PLATDEP_FILEGLOBBER_H

#ifdef _MSC_VER
#include <process.h>
#include <direct.h>
#include <io.h>
#include <direct.h>
#include <stdlib.h> // _MAX_PATH

#define getpid _getpid
#define getcwd _getcwd
#define chdir  _chdir
#define usleep(x) _sleep((x)/1000)
#define stat _stat
#define mkdir(x,y) _mkdir(x)

#else

#include <sys/types.h>
#include <unistd.h>  // getpid(), getcwd(), etc
#include <glob.h>

#endif

#include <string>
#include <exception>   // std::runtime_exception


class GlobPrivateData;

/**
 * Utility for resolving wildcards in file names. If the pattern doesn't contain
 * a wildcard, it is returned as it is, regardless whether such file exists
 * or not.
 */
class FileGlobber
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
};

#endif

