//==========================================================================
//  FSUTILS.CC - part of
//                     OMNeT++/OMNEST
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

#include "platdep/misc.h"
#include "fsutils.h"

#include "cexception.h"

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
    olddir.reserve(1024);
    if (!getcwd(olddir.buffer(),1024))
        throw new cException("Cannot get the name of current directory");
    if (chdir(changetodir))
        throw new cException("Cannot temporarily change to directory `%s' (does it exist?)", changetodir);
}

PushDir::~PushDir()
{
    if (chdir(olddir.c_str()))
        throw new cException("Cannot change back to directory `%s'", olddir.c_str());
}


