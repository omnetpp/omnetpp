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

USING_NAMESPACE


//
// Global objects
//
cRegistrationList componentTypes("component types");
cRegistrationList nedFunctions("NED functions");
cRegistrationList classes("classes");
cRegistrationList enums("enums");
cRegistrationList classDescriptors("class descriptors");
cRegistrationList configKeys("supported configuration keys");


cModuleType *findModuleType(const char *name)
{
    return cModuleType::find(name);
}
