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
#include "onstartup.h"
#include "ccomponenttype.h"


//
// Global objects
//
cRegistrationList nedDeclarations("NED declarations");
cRegistrationList componentTypes("component types");
cRegistrationList nedFunctions("NED functions");
cRegistrationList classes("classes");
cRegistrationList enums("enums");


cModuleType *findModuleType(const char *s)
{
    // compatibility
    //FIXME check it is a module type
//    return cComponentType::find(name);
return NULL; //XXX
}
