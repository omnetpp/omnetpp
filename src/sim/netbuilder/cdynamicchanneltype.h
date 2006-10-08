//==========================================================================
//  CDYNAMICCHANNELTYPE.H - part of
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

#ifndef __CDYNAMICCHANNELTYPE_H
#define __CDYNAMICCHANNELTYPE_H

#include "cchannel.h"
#include "ccomponenttype.h"

#include "cnednetworkbuilder.h"


/**
 * NEDXML-based cChannelType: takes all info from cNEDResourceCache
 */
class cDynamicChannelType : public cChannelType
{
  protected:
    /** Redefined from cChannelType */
    virtual void addParametersTo(cChannel *module);

    /** Redefined from cChannelType */
    virtual void buildInside(cChannel *module);

  public:
    cDynamicChannelType(const char *name);
};


#endif


