//==========================================================================
//  FSUTILS.CC - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//  Implementation of
//    TOmnetApp
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "fsutils.h"

#if defined(_WIN32) && !defined(__CYGWIN32__)
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


long getProcessId()
{
    return getpid();
}

void splitFileName(const char *pathname, opp_string& dir, opp_string& fnameonly)
{
    if (!pathname || !*pathname)
    {
         dir = "";
         fnameonly = "";
         return;
    }

    dir = pathname;

    // find last "/" or "\"
    char *dirbeg = dir.buffer();
    char *s = dirbeg + strlen(dirbeg) - 1;
    while (s>=dirbeg && *s!='\\' && *s!='/') s--;

    // split along that
    if (s<dirbeg)
    {
        fnameonly = dirbeg;
        dir = ".";
    }
    else
    {
        fnameonly = s+1;
        *(s+1) = '\0';
    }

}

//------------

PushDir::PushDir(const char *changetodir)
{
    // FIXME error handling
    olddir.allocate(1024);
    getcwd(olddir.buffer(),1024);
    chdir(changetodir);
}

PushDir::~PushDir()
{
    // FIXME error handling
    chdir((const char *)olddir);
}

//-----------


#if defined(_WIN32) && !defined(__CYGWIN32__)

struct GlobPrivateData
{
    long handle;
    struct _finddata_t fdata;
    char dir[_MAX_PATH];
    char tmpfname[_MAX_PATH];
};

Globber::Globber(const char *filenamepattern)
{
    fnamepattern = filenamepattern;
    data = NULL;
}

Globber::~Globber()
{
    delete data;
}

const char *Globber::getNext()
{
    if (!data)
    {
        // first call
        data = new GlobPrivateData();
        data->handle = _findfirst((const char *)fnamepattern, &data->fdata);
        if (data->handle<0)
        {
            _findclose(data->handle);
            return NULL;
        }

        // remember directory in data->dir
        strcpy(data->dir,(const char *)fnamepattern);
        char *s = data->dir + strlen(data->dir);
        while (--s>=data->dir)
            if (*s=='/' || *s=='\\')
                break;
        *(s+1)='\0';  // points (s+1) points either after last "/" or at beg of string.

        // concat file name on directory
        strcpy(data->tmpfname,data->dir);
        strcat(data->tmpfname,data->fdata.name);
        return data->tmpfname;
    }
    else
    {
        // subsequent calls
        int done=_findnext(data->handle, &data->fdata);
        if (done)
        {
            _findclose(data->handle);
            return NULL;
        }
        // concat file name on directory
        strcpy(data->tmpfname,data->dir);
        strcat(data->tmpfname,data->fdata.name);
        return data->tmpfname;
    }
}

#else

struct GlobPrivateData
{
    glob_t globdata;
    int globpos;
};

Globber::Globber(const char *filenamepattern)
{
    fnamepattern = filenamepattern;
    data = NULL;
}

Globber::~Globber()
{
    globfree(&globdata);
    delete data;
}

const char *Globber::getNext()
{
    if (!data)
    {
        data = new GlobPrivateData();
        if (glob((const char *)fnamepattern, 0, NULL, &data->globdata)!=0)
            return NULL;
        data->globpos = 0;
        return data->globdata.gl_pathv[data->globpos++];
    }
    else
    {
        return data->globdata.gl_pathv[data->globpos++];
    }
}

#endif


