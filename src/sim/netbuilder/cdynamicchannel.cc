//==========================================================================
//   CDYNAMICCHANNEL.CC
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

#include "cdynamicchannel.h"
#include "cnednetworkbuilder.h"


cDynamicChannelType::cDynamicChannelType(const char *name, ChannelNode *chan) :
  cChannelType(name)
{
    channelnode = chan;
}

cDynamicChannelType::~cDynamicChannelType()
{
    delete channelnode;
}

cChannel *cDynamicChannelType::create(const char *name)
{
    cNEDNetworkBuilder builder;
    cChannel *chan = builder.createChannel(name, channelnode);
    return chan;
}

