//==========================================================================
//  CDYNAMICMODULETYPE.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDYNAMICMODULETYPE_H
#define __CDYNAMICMODULETYPE_H

#include "cmodule.h"
#include "ccompoundmodule.h"
#include "ccomponenttype.h"

#include "cnednetworkbuilder.h"


/**
 * NEDXML-based cModuleType: takes all info from cNEDResourceCache
 */
class cDynamicModuleType : public cModuleType
{
  protected:
    /** Redefined from cModuleType */
    virtual cModule *createModuleObject();

    /** Redefined from cModuleType */
    virtual void addParametersGatesTo(cModule *module);

    /** Redefined from cModuleType */
    virtual void buildInside(cModule *module);

  public:
    cDynamicModuleType(const char *name);
};

#endif


