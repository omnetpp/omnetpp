//==========================================================================
//  PLATDEP/FILEGLOBBER.C - part of
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

//
// IMPLEMENTATION of class FileGlobber. This file is shared between the
// simulation linrary, nedtool, etc. It has to be #included by exactly one
// source file in the executable. For simulation programs, the std_sim
// library (src/sim) already includes it, so if you use this class in models,
// you don't need to #include it again.
//

#include "platdep/misc.h"
#include "platdep/fileglobber.h"

#ifdef _WIN32

#include <io.h>
#include <direct.h>
#include <stdlib.h> // _MAX_PATH


struct GlobPrivateData
{
    bool nowildcard;
    long handle;
    struct _finddata_t fdata;
    char dir[_MAX_PATH];
    char tmpfname[_MAX_PATH];
};

FileGlobber::FileGlobber(const char *filenamepattern)
{
    fnamepattern = filenamepattern;
    data = NULL;
}

FileGlobber::~FileGlobber()
{
    delete data;
}

const char *FileGlobber::getNext()
{
    if (!data)
    {
        // first call
        data = new GlobPrivateData();
        if (!strchr(fnamepattern.c_str(),'*') && !strchr(fnamepattern.c_str(),'?'))
        {
            data->nowildcard = true;
            return fnamepattern.c_str();
        }

        data->nowildcard = false;

        // remember directory in data->dir
        strcpy(data->dir,fnamepattern.c_str());
        char *s = data->dir + strlen(data->dir);
        while (--s>=data->dir)
            if (*s=='/' || *s=='\\')
                break;
        *(s+1)='\0';  // points (s+1) points either after last "/" or at beg of string.

        if (strchr(data->dir,'*') || strchr(data->dir,'?'))
            throw std::runtime_error(std::string(data->dir)+": wildcard characters in directory names are not allowed");

        // get first file
        data->handle = _findfirst(fnamepattern.c_str(), &data->fdata);
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

#include <glob.h>

struct GlobPrivateData
{
    bool nowildcard;
    glob_t globdata;
    int globpos;
};

FileGlobber::FileGlobber(const char *filenamepattern)
{
    fnamepattern = filenamepattern;
    data = NULL;
}

FileGlobber::~FileGlobber()
{
    globfree(&data->globdata);
    delete data;
}

const char *FileGlobber::getNext()
{
    if (!data)
    {
        data = new GlobPrivateData();
        if (!strchr(fnamepattern.c_str(),'*') && !strchr(fnamepattern.c_str(),'?'))
        {
            data->nowildcard = true;
            return fnamepattern.c_str();
        }

        data->nowildcard = false;
        if (glob(fnamepattern.c_str(), 0, NULL, &data->globdata)!=0)
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
