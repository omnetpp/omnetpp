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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "opp_ctype.h"
#include "fsutils.h"
#include "platmisc.h"
#include "onstartup.h"
#include "cexception.h"

NAMESPACE_BEGIN

void loadExtensionLibrary(const char *lib)
{
    try {
        opp_loadlibrary(lib);
        ExecuteOnStartup::executeAll();
    }
    catch (std::runtime_error e) {
        throw cRuntimeError(e.what());
    }
}

NAMESPACE_END
