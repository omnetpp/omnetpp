//=========================================================================
//
//  GLOBALS.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "macros.h"
#include "globals.h"
#include "carray.h"
#include "onstartup.h"  // cSingleton

//=== Global objects:
cSingleton<cArray> networks("networks");
cSingleton<cArray> modinterfaces("module-interfaces");
cSingleton<cArray> modtypes("module-types");
cSingleton<cArray> linktypes("link-types");
cSingleton<cArray> functions("functions");
cSingleton<cArray> classes("classes");
cSingleton<cArray> enums("enums");


cNetworkType *findNetwork(const char *s)
{
    return (cNetworkType *)networks.instance()->get(s);
}

cModuleType *findModuleType(const char *s)
{
    return (cModuleType *)modtypes.instance()->get(s);
}

cModuleInterface *findModuleInterface(const char *s)
{
    return (cModuleInterface *)modinterfaces.instance()->get(s);
}

cLinkType *findLink(const char *s)
{
    return (cLinkType *)linktypes.instance()->get(s);
}

cEnum *findEnum(const char *s)
{
    return (cEnum *)enums.instance()->get(s);
}

cFunctionType *findFunction(const char *s,int argcount)
{
    cArray *a = functions.instance();
    for (int i=0; i<a->items(); i++)
    {
        cFunctionType *f = (cFunctionType *) a->get(i);
        if (f->isName(s) && f->argCount()==argcount)
            return f;
    }
    return NULL;
}

