//==========================================================================
//   CDYNAMICNETWORK.CC
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

#include "nedelements.h"
#include "nederror.h"

#include "nedparser.h"
#include "nedxmlparser.h"
#include "neddtdvalidator.h"
#include "nedbasicvalidator.h"
#include "nedsemanticvalidator.h"

#include "cdynamicnetwork.h"
#include "cnednetworkbuilder.h"


cDynamicNetworkType::cDynamicNetworkType(const char *name, NetworkNode *networkNode) :
  cNetworkType(name)
{
    networknode = networkNode;
}

cDynamicNetworkType::~cDynamicNetworkType()
{
    delete networknode;
}


void cDynamicNetworkType::setupNetwork()
{
    cNEDNetworkBuilder builder;
    builder.setupNetwork(networknode);
}


//------------------

#if 0
cDynamicChannelType::cDynamicChannelType(const char *name, ChannelNode *channelNode) :
  cChannelType(name)
{
    channelnode = channelNode;
}

cDynamicChannelType::~cDynamicChannelType()
{
    delete channelnode;
}

cChannel *cDynamicChannelType::create()
{
    cNEDNetworkBuilder builder;
    return builder.createChannel(channelnode);
}
#endif
