//==========================================================================
//  FILEGLOBBER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include "omnetpp/platdep/platmisc.h"
#include "fileglobber.h"

#ifdef _WIN32

#include <io.h>
#include <direct.h>
#include <cstdlib>  // _MAX_PATH

namespace omnetpp {
namespace common {

struct GlobPrivateData
{
    bool nowildcard;
    intptr_t handle;
    struct _finddata_t fdata;
    char dir[_MAX_PATH];
    char tmpfname[_MAX_PATH];
};

FileGlobber::FileGlobber(const char *filenamepattern) : fnamepattern(filenamepattern)
{

}

FileGlobber::~FileGlobber()
{
    delete data;
}

const char *FileGlobber::getNext()
{
    if (!data) {
        // first call
        data = new GlobPrivateData();
        if (!strchr(fnamepattern.c_str(), '*') && !strchr(fnamepattern.c_str(), '?')) {
            data->nowildcard = true;
            return fnamepattern.c_str();
        }

        data->nowildcard = false;

        // remember directory in data->dir
        strcpy(data->dir, fnamepattern.c_str());
        char *s = data->dir + strlen(data->dir);
        while (--s >= data->dir)
            if (*s == '/' || *s == '\\')
                break;

        *(s+1) = '\0';  // points (s+1) points either after last "/" or at beg of string.

        if (strchr(data->dir, '*') || strchr(data->dir, '?'))
            throw std::runtime_error(std::string(data->dir)+": Wildcard characters in directory names are not allowed");

        // get first file
        data->handle = _findfirst(fnamepattern.c_str(), &data->fdata);
        if (data->handle < 0) {
            _findclose(data->handle);
            return nullptr;
        }

        // concat file name on directory
        strcpy(data->tmpfname, data->dir);
        strcat(data->tmpfname, data->fdata.name);
        return data->tmpfname;
    }
    else {
        // subsequent calls
        if (data->nowildcard)
            return nullptr;
        int done = _findnext(data->handle, &data->fdata);
        if (done) {
            _findclose(data->handle);
            return nullptr;
        }
        // concat file name on directory
        strcpy(data->tmpfname, data->dir);
        strcat(data->tmpfname, data->fdata.name);
        return data->tmpfname;
    }
}

}  // namespace common
}  // namespace omnetpp

#elif defined __ANDROID__

#include <dirent.h>
#include <cerrno>
#include <fnmatch.h>
#include <limits.h>

namespace omnetpp {
namespace common {

struct GlobPrivateData
{
    bool nowildcard;
    DIR *pdir;
    struct dirent *pent;
    char dir[PATH_MAX];
    char tmpfname[PATH_MAX];
};

FileGlobber::FileGlobber(const char *filenamepattern) : fnamepattern(filenamepattern)
{

}

FileGlobber::~FileGlobber()
{
    closedir(data->pdir);
    delete data;
}

const char *FileGlobber::getNext()
{
    if (!data) {
        // first call
        data = new GlobPrivateData();

        if (!strchr(fnamepattern.c_str(), '*') && !strchr(fnamepattern.c_str(), '?')) {
            data->nowildcard = true;
            return fnamepattern.c_str();
        }

        data->nowildcard = false;

        // remember directory in data->dir
        strcpy(data->dir, fnamepattern.c_str());
        char *s = data->dir + strlen(data->dir);
        while (--s >= data->dir)
            if (*s == '/' || *s == '\\')
                break;

        *(s+1) = '\0';  // points (s+1) points either after last "/" or at beginning of string.

        if (strchr(data->dir, '*') || strchr(data->dir, '?'))
            throw std::runtime_error(std::string(data->dir)+": Wildcard characters in directory names are not allowed");

        // get first file
        errno = 0;

        if (std::string(data->dir).size() == 0)
            data->pdir = opendir(".");
        else
            data->pdir = opendir(data->dir);

        if (!data->pdir)
            throw std::runtime_error(std::string(data->dir)+": Directory cannot be opened");

        data->pent = readdir(data->pdir);

        if (errno)
            throw std::runtime_error(std::string(data->dir)+": Cannot read directory");

        if (!data->pent)
            return nullptr;

        if (fnmatch(fnamepattern.c_str(), data->pent->d_name, 0) != 0)
            return getNext();

        // concat file name on directory
        strcpy(data->tmpfname, data->dir);
        strcat(data->tmpfname, data->pent->d_name);
        return data->tmpfname;
    }
    else {
        // subsequent calls
        if (data->nowildcard)
            return nullptr;

        data->pent = readdir(data->pdir);
        if (errno)
            throw std::runtime_error(std::string(data->dir)+": Cannot read directory");

        if (!data->pent) {
            return nullptr;
        }

        if (fnmatch(fnamepattern.c_str(), data->pent->d_name, 0) != 0)
            return getNext();

        // concat file name on directory
        strcpy(data->tmpfname, data->dir);
        strcat(data->tmpfname, data->pent->d_name);
        return data->tmpfname;
    }
}

}  // namespace common
}  // namespace omnetpp

#else

#include <glob.h>

namespace omnetpp {
namespace common {

struct GlobPrivateData
{
    bool nowildcard;
    glob_t globdata;
    int globpos;
};

FileGlobber::FileGlobber(const char *filenamepattern) : fnamepattern(filenamepattern)
{

}

FileGlobber::~FileGlobber()
{
    globfree(&data->globdata);
    delete data;
}

const char *FileGlobber::getNext()
{
    if (!data) {
        data = new GlobPrivateData();
        if (!strchr(fnamepattern.c_str(), '*') && !strchr(fnamepattern.c_str(), '?')) {
            data->nowildcard = true;
            return fnamepattern.c_str();
        }

        data->nowildcard = false;
        if (glob(fnamepattern.c_str(), 0, nullptr, &data->globdata) != 0)
            return nullptr;
        data->globpos = 0;
        return data->globdata.gl_pathv[data->globpos++];
    }
    else {
        if (data->nowildcard)
            return nullptr;
        return data->globdata.gl_pathv[data->globpos++];
    }
}

}  // namespace common
}  // namespace omnetpp

#endif

namespace omnetpp {
namespace common {

std::vector<std::string> FileGlobber::getFilenames()
{
    std::vector<std::string> v;
    const char *filename;
    while ((filename = getNext()) != nullptr)
        v.push_back(filename);
    return v;
}

}  // namespace common
}  // namespace omnetpp

