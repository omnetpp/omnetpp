//==========================================================================
//  FSUTILS.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "fsutils.h"
#include "platdep/misc.h"
#include "platdep/loadlib.h"
#include "onstartup.h"
#include "cexception.h"

PushDir::PushDir(const char *changetodir)
{
    if (!changetodir)
        return;
    olddir.reserve(1024);
    if (!getcwd(olddir.buffer(),1024))
        throw new cRuntimeError("Cannot get the name of current directory");
    if (chdir(changetodir))
        throw new cRuntimeError("Cannot temporarily change to directory `%s' (does it exist?)", changetodir);
}

PushDir::~PushDir()
{
    if (!olddir.empty())
    {
        if (chdir(olddir.c_str()))
            throw new cRuntimeError("Cannot change back to directory `%s'", olddir.c_str());
    }
}

//------------

void loadExtensionLibrary(const char *lib)
{
    try {
        opp_loadlibrary(lib);
        ExecuteOnStartup::executeAll();
    } catch (std::runtime_error e) {
        throw new cRuntimeError(e.what());
    }
}


