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
#include "cexception.h"

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
    olddir.allocate(1024);
    if (!getcwd(olddir.buffer(),1024))
        throw new cException("Cannot get the name of current directory");
    if (chdir(changetodir))
        throw new cException("Cannot temporarily change to directory `%s' (does it exist?)", changetodir);
}

PushDir::~PushDir()
{
    if (chdir((const char *)olddir))
        throw new cException("Cannot change back to directory `%s'", (const char *)olddir);
}

//-----------


#if defined(_WIN32) && !defined(__CYGWIN32__)

struct GlobPrivateData
{
    bool nowildcard;
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
        if (!strchr((const char *)fnamepattern,'*') && !strchr((const char *)fnamepattern,'?'))
        {
            data->nowildcard = true;
            return (const char *)fnamepattern;
        }

        data->nowildcard = false;

        // remember directory in data->dir
        strcpy(data->dir,(const char *)fnamepattern);
        char *s = data->dir + strlen(data->dir);
        while (--s>=data->dir)
            if (*s=='/' || *s=='\\')
                break;
        *(s+1)='\0';  // points (s+1) points either after last "/" or at beg of string.

        if (strchr(data->dir,'*') || strchr(data->dir,'?'))
            throw new cException("Wildcard characters in directory names are not allowed: `%s'", data->dir);

        // get first file
        data->handle = _findfirst((const char *)fnamepattern, &data->fdata);
        if (data->handle<0)
        {
            _findclose(data->handle);
            return NULL;
        }

        // concat file name on directory
        strcpy(data->tmpfname,data->dir);
        strcat(data->tmpfname,data->fdata.name);
        return data->tmpfname;
    }
    else
    {
        // subsequent calls
        if (data->nowildcard)
            return NULL;
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
    bool nowildcard;
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
        if (!strchr((const char *)fnamepattern,'*') && !strchr((const char *)fnamepattern,'?'))
        {
            data->nowildcard = true;
            return (const char *)fnamepattern;
        }

        data->nowildcard = false;
        if (glob((const char *)fnamepattern, 0, NULL, &data->globdata)!=0)
            return NULL;
        data->globpos = 0;
        return data->globdata.gl_pathv[data->globpos++];
    }
    else
    {
        if (data->nowildcard)
            return NULL;
        return data->globdata.gl_pathv[data->globpos++];
    }
}

#endif


