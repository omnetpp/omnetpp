//==========================================================================
//  CDYNAMICCHANNEL.H - part of
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

#ifndef __CDYNAMICCHANNEL_H
#define __CDYNAMICCHANNEL_H

#include "cchannel.h"
#include "ctypes.h"

#include "cnednetworkbuilder.h"


/**
 * Special cChannelType: takes all info from the stored NED tree
 */
class cDynamicChannelType : public cChannelType
{
  protected:
    ChannelNode *channelnode;  // contains NEDElement tree

  public:
    cDynamicChannelType(const char *name, ChannelNode *channelnode);
    virtual ~cDynamicChannelType();

    /**
     * Create a channel object using cNEDNetworkBuilder
     */
    virtual cChannel *create(const char *name);
};


#endif


