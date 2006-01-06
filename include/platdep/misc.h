//==========================================================================
//  PLATDEP/MISC.H - part of
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

#ifndef __PLATDEP_MISC_H
#define __PLATDEP_MISC_H

//
// Common Unix functionality
//
#ifdef _WIN32

#include <process.h>
#include <io.h>
#include <direct.h>
#include <stdlib.h> // _MAX_PATH

#define getpid _getpid
#define getcwd _getcwd
#define chdir  _chdir
#define usleep(x) _sleep((x)/1000)
#define mkdir(x,y) _mkdir(x)

#else

#include <sys/types.h>
#include <unistd.h>  // getpid(), getcwd(), etc

#endif


//
// Getting Windows error strings
//
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef min
#undef max
#include <string>

// errorCode usually comes from GetLastError()
inline std::string opp_getWindowsError(DWORD errorCode)
{
     // Some nice microsoftism to produce an error msg...
     LPVOID lpMsgBuf;
     FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    errorCode,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR) &lpMsgBuf,
                    0,
                    NULL );
     int len = strlen((const char *)lpMsgBuf);
     std::string ret((const char *)lpMsgBuf, 0, len>3 ? len-3 : len); // chop ".\r\n"
     LocalFree( lpMsgBuf );
     return ret;
}
#endif


//
// Basic platform-independent way of getting the file listing of a directory
//
#ifdef _WIN32

#define SHELL_DOES_NOT_EXPAND_WILDCARDS 1

#include <io.h>
#include <direct.h>
static long _handle;
static struct _finddata_t _fdata;
static char _dir[_MAX_FNAME];
static char _tmpfname[_MAX_FNAME];
inline const char *findFirstFile(const char *mask)
{
    _handle = _findfirst(mask, &_fdata);
    if (_handle<0) {_findclose(_handle); return NULL;}
    strcpy(_dir,mask);
    for (char *s = _dir+strlen(_dir)-1; true; s--)
        if (s<_dir || *s=='/' || *s=='\\')
            {*(s+1)='\0'; break;}
    strcpy(_tmpfname,_dir);
    strcat(_tmpfname,_fdata.name);
    return _tmpfname;
}
inline const char *findNextFile()
{
    int done=_findnext(_handle, &_fdata);
    if (done) {_findclose(_handle); return NULL;}
    strcpy(_tmpfname,_dir);
    strcat(_tmpfname,_fdata.name);
    return _tmpfname;
}
inline void findCleanup()
{
}

#else

#include <glob.h>
static glob_t globdata;
static int globpos;
inline const char *findFirstFile(const char *mask)
{
    if (glob(mask, 0, NULL, &globdata)!=0)
        return NULL;
    globpos = 0;
    return globdata.gl_pathv[globpos++];
}
inline char *findNextFile()
{
    return globdata.gl_pathv[globpos++];
}
inline void findCleanup()
{
    globfree(&globdata);
}
#endif

/***
//
// code for Borland C++ -- currently unused
//
#include <dir.h>
struct ffblk ffblk;
const char *findFirstFile(const char *mask)
{
    int done = findfirst(argv[k],&ffblk,0);
    return done ? NULL : ffblk.ff_name;
}
const char *findNextFile()
{
    int done = findnext(&ffblk);
    return done ? NULL : ffblk.ff_name;
}
***/

#endif

