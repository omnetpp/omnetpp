//==========================================================================
//  GLOBALS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include "onstartup.h"
#include "cregistrationlist.h"
#include "cclassfactory.h"

NAMESPACE_BEGIN

class cModuleType;

//
// Global objects
//

//< Internal: list in which objects are accumulated if there's no simple module in context.
//< @see cOwnedObject::setDefaultOwner() and cSimulation::setContextModule())
SIM_API extern cDefaultList defaultList;

SIM_API extern cGlobalRegistrationList componentTypes;  ///< List of all component types (cComponentType)
SIM_API extern cGlobalRegistrationList nedFunctions;    ///< List if all NED functions (cMathFunction and cNEDFunction)
SIM_API extern cGlobalRegistrationList classes;         ///< List of all classes that can be instantiated using createOne(); see cClassFactory and Register_Class() macro
SIM_API extern cGlobalRegistrationList enums;           ///< List of all enum objects (cEnum)
SIM_API extern cGlobalRegistrationList classDescriptors;///< List of all class descriptors (cClassDescriptor)
SIM_API extern cGlobalRegistrationList configKeys;      ///< List of supported configuration keys (cConfigKey)


/**
 *
 * @addtogroup Functions
 */
//@{

/**
 * DEPRECATED. Use cComponentType::find() instead.
 */
SIM_API _OPPDEPRECATED cModuleType *findModuleType(const char *s);
//@}

NAMESPACE_END


#endif

