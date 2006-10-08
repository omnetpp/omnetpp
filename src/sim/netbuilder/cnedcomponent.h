//==========================================================================
// CNEDCOMPONENT.H -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __CNEDCOMPONENT_H
#define __CNEDCOMPONENT_H

#include "defs.h"
#include "nedcomponent.h"


/**
 * Wraps a NEDElement tree of a NED declaration (module, channel, module
 * interface or channel interface). cNEDComponent objects serve as input
 * for the dynamic NED builder.
 */
class SIM_API cNEDComponent : public NEDComponent
{
  public:
    /** Constructor */
    cNEDComponent(NEDElement *tree);

    /** Destructor */
    virtual ~cNEDComponent();
};

#endif

