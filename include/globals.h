//==========================================================================
//  GLOBALS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __GLOBALS_H
#define __GLOBALS_H

#include "onstartup.h"
#include "carray.h"
#include "cclassfactory.h"

NAMESPACE_BEGIN

class cModuleType;


//
// Global objects
//

//< Internal: list in which objects are accumulated if there's no simple module in context.
//< @see cOwnedObject::setDefaultOwner() and cSimulation::setContextModule())
SIM_API extern cDefaultList defaultList;

SIM_API extern cRegistrationList componentTypes;  ///< List of all component types (cComponentType)
SIM_API extern cRegistrationList nedFunctions;    ///< List if all NED functions (cMathFunction and cNEDFunction)
SIM_API extern cRegistrationList classes;         ///< List of all classes that can be instantiated using createOne(); see cClassFactory and Register_Class() macro
SIM_API extern cRegistrationList enums;           ///< List of all enum objects (cEnum)
SIM_API extern cRegistrationList classDescriptors;///< List of all class descriptors (cClassDescriptor)
SIM_API extern cRegistrationList configKeys;      ///< List of supported configuration keys (cConfigKey)


/**
 * @name Miscelleous
 * @ingroup Functions
 */
//@{

/**
 * DEPRECATED. Use cComponentType::find() instead.
 */
SIM_API cModuleType *findModuleType(const char *s);

//@}

NAMESPACE_END


#endif

