//=========================================================================
//  GLOBALS.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "globals.h"
#include "carray.h"
#include "onstartup.h"  // cSingleton
#include "ccomponenttype.h"


//
// Global objects
//
cSingleton<cArray> nedDeclarations("NED declarations");
cSingleton<cArray> componentTypes("component types");
cSingleton<cArray> nedFunctions("NED functions");
cSingleton<cArray> classes("classes");
cSingleton<cArray> enums("enums");


cModuleType *findModuleType(const char *s)
{
    // compatibility
    //FIXME check it is a module type
//    return cComponentType::find(name);
return NULL; //XXX
}
