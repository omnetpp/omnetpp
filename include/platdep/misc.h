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

#ifndef __PLATDEP_MISC_H
#define __PLATDEP_MISC_H

#ifdef _MSC_VER
#include <process.h>
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

#endif

#endif

