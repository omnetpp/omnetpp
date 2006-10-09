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
cRegistrationList componentTypes("component types");
cRegistrationList nedFunctions("NED functions");
cRegistrationList classes("classes");
cRegistrationList enums("enums");


cModuleType *findModuleType(const char *name)
{
    return cModuleType::find(name);
}
