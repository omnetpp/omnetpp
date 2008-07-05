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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "globals.h"
#include "ccomponenttype.h"

NAMESPACE_BEGIN

//
// Global objects
//
cGlobalRegistrationList componentTypes("component types");
cGlobalRegistrationList nedFunctions("NED functions");
cGlobalRegistrationList classes("classes");
cGlobalRegistrationList enums("enums");
cGlobalRegistrationList classDescriptors("class descriptors");
cGlobalRegistrationList configKeys("supported configuration keys");

NAMESPACE_END

